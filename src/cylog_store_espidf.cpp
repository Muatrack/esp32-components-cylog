
#include <iostream>
/* c++ 17 */
#include <filesystem>
// c++ 17
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>

#include "private_include/cylog_store_espidf.hpp"
#include "private_include/cylog_file.hpp"

#ifdef USE_ASSERTION
    #include <assert.h>
#endif

namespace fs=std::filesystem;

CL_TYPE_t StoreEspidf::dirCreate( const std::string & logDir ) {
    CL_TYPE_t err = CL_OK;
    std::string absPath = rootDirGet() + "/" + logDir;

    /* test */
    std::string _rd = "/sdb/logroot";
    if( doesExists(_rd) ) {
        CYLOG_PRINT(  std::cout<<"[ StoreEspidf::dirCreate() ] | dir : " << _rd << " does exists" <<std::endl );
    } else {
        CYLOG_PRINT(  std::cout<<"[ StoreEspidf::dirCreate() ] | dir : " << _rd << " doesn't exists" <<std::endl );
    }

    /* 目录如已存在，跳过新建 */
    if( doesExists(absPath) ) {
        CYLOG_PRINT(  std::cout<<"[ StoreEspidf::dirCreate() ] | dir : " << absPath << " does exists" <<std::endl );
        goto done;
    }

    CYLOG_PRINT(  std::cout<<"[ StoreEspidf::dirCreate() ] | dir : " << absPath << " doesn't exists" <<std::endl );

    /* 目录不存在，则新建 */
#if 1
    if( fs::create_directory(absPath) ) {
        CYLOG_PRINT(  std::cout<<"[ StoreEspidf::dirCreate() ] | succ to create " << absPath <<std::endl );
        goto done;
    } else {
        CYLOG_PRINT(  std::cout<<"[ StoreEspidf::dirCreate() ] | fail to create " << absPath <<std::endl );
        goto excp;
    }
#else
    if( mkdir(absPath.c_str(), 0777)==0 ) { goto done; }
    else { 
        CYLOG_PRINT(  std::cout<<"[ StoreEspidf::dirCreate() fail to create " << absPath << " , errno:"<< errno <<std::endl );
        err = CL_EXCP_UNKNOW; goto excp; 
    }
#endif
excp:
    return err;
done:
    return CL_OK;
}

CL_TYPE_t StoreEspidf::fileCreate( std::unique_ptr<FileDesc> & pFDesc ) {
    CYLOG_PRINT(  std::cout<<__FILE__<<":"<<__LINE__<<std::endl );
    std::string fPath = "";
    std::string absPath = rootDirGet() + "/" + pFDesc->relativePathGet();
    int fd = 0;

    CYLOG_PRINT(  std::cout<<__FILE__<<": gonna create dir: "<<__LINE__<<" "<<absPath<<std::endl );

    /* 分类日志的目录是否存在， 应当在新建前被创建 */
    if( doesExists(absPath)==false ) {
        CYLOG_PRINT(  std::cout<<__FILE__<<":"<<__LINE__<<" path doesn't exist"<<std::endl );
        goto excp;
    }

    CYLOG_PRINT(  std::cout<<__FILE__<<":"<<__LINE__<<" path does exist"<<std::endl );
    /* 拼接日志文件名称，在日志目录下逐一生成文件*/
    for(int i=0; i<pFDesc->fileCountGet(); i++) {
        fPath = absPath+"/"+ pFDesc->filePrefixGet() + "_"+((i<10)?"0":"") + std::to_string(i);
        CYLOG_PRINT(  std::cout<<__FILE__<<": gonna create file: "<<__LINE__<<" "<<fPath<<std::endl );

        // 如果文件存在，则跳过
        if( doesExists(fPath) ) { continue;  }

        if( fd=open(fPath.c_str(), O_CREAT|O_EXCL|O_RDWR), fd>=0 ) { close(fd); }
        else { CYLOG_PRINT(  std::cout<<"StoreEspidf:: Fail to create log file:"<<fPath<<std::endl ); }

        // 初始文件的大小
        truncate( fPath.c_str(), pFDesc->fileSizeGet());
        CYLOG_PRINT(  std::cout<<"StoreEspidf:: Create log file:"<<fPath<<std::endl );
    }

    CYLOG_PRINT(  std::cout<<__FILE__<<":"<<__LINE__<<"-------------------------- done -------------------------"<<std::endl );
    return CL_OK;

excp:
    return CL_EXCP_UNKNOW;
}

CL_TYPE_t StoreEspidf::itemWrite( std::unique_ptr<CLFile::FileDesc> & pFDesc, const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen, uint32_t timeoutMs) {

    CL_TYPE_t _err = CL_OK;
    uint32_t wOff = 0;
    uint8_t  tailZeroArray[4] = {0x0};

    std::ofstream ofe;
    std::string fPath;
    
    {
        // 判断文件是否已满，判断文件是否能够写下当前数据
        if( isFileFull(pFDesc->wFileOffsetGet(), pFDesc) ) {
            nextFileSelect(pFDesc);


            // 经过nextFileSelect()后，不应当存在被选择文件已满的情况
            if( isFileFull(pFDesc->wFileOffsetGet(), pFDesc) ) { 
                #ifdef USE_ASSERTION
                    assert(false); 
                #else
                    CYLOG_PRINT(  std::cout<< "[ Fatil exception ] :" << __FILE__<< ":"<< __LINE__ << std::endl );
                    return CL_EXCP_UNKNOW;
                #endif
            }
        }

        // 判断当前可写文件是否能够存储待写的日志, 此种情况发生后属于致命异常
        if( (pFDesc->wFileOffsetGet()+iLen)>= pFDesc->fileSizeGet() ) { 
            #ifdef USE_ASSERTION
                assert(false);
            #else
                CYLOG_PRINT(  std::cout<< "[ Fatil exception ] :" << __FILE__<< ":"<< __LINE__ << std::endl );
                return CL_EXCP_UNKNOW;
            #endif
        }
    }
    
    fPath = pFDesc->wFilePathGet();

    // 判断参数有效性
    if( (pIn==nullptr) || (iLen<1) ) {  goto excp;  }
    // 获取读写资源 
    if( lockTake(timeoutMs) == false ) {
        CYLOG_PRINT( std::cout << "\n------------------------\nFail to get store lock\n" << "------------------------\n" <<std::endl );
        _err = CL_LOG_BUSY;
        goto lock_excp;
    }

    ofe = std::ofstream( fPath, std::ios::in | std::ios::out | std::ios::binary );
    if( ofe.is_open()==false ) {
        CYLOG_PRINT(  std::cout << "Fail to open file: " << fPath << std::endl );
        goto excp;
    }

    wOff = pFDesc->wFileOffsetGet();
    ofe.seekp( wOff, std::ios::beg);
    CYLOG_PRINT(  std::cout <<"[ "<< fPath << " ] writable offset:" << wOff << std::endl );

    // 写数据到文件
    ofe.write( reinterpret_cast<const char*>(pIn.get()), iLen);
    ofe.write( (char*)tailZeroArray, 4);    //写入额外的4个字节0. 当覆盖写的时候便于将就数据自动失效
    pFDesc->wFileOffsetSet( pFDesc->wFileOffsetGet()+ iLen);

    // ofe.flush();
    ofe.close();

    // CYLOG_PRINT(  std::cout << std::endl << "Succ to write file: "<< fPath << " data size:" << iLen << std::endl );

#if 0
re_write:
    // 判断当前文件是否能够写下 iLen 长的数据
    if( (m_curWriteOffset + sizeof(iLen) + iLen + 2) > m_fileMaxLength ) {
        CYLOG_PRINT(  std::cout<< "   " << __func__ << "()." << __LINE__ << std::endl );
        // 如已重选文件，则跳出，否则异常时会出现循环-导致死机
        if( _bReWriten ) {
            _err = CL_FILE_FULL;
            goto excp;
        }
        CYLOG_PRINT(  std::cout<< "   " << __func__ << "()." << __LINE__ << std::endl );
        // 当前文件已写满，选择下一个文件
        nextFileSelect();
        _bReWriten = true;
        goto re_write;
    }
    {
        // 写数据到文件
        std::fstream _ff;
        if( _ff.open( m_curWriteFilePath, std::ios::binary | std::ios::out | std::ios::in ), !_ff.is_open() ) {
            CYLOG_PRINT(  std::cout << "     StoreEspidf::itemWrite file closed [ Excep ]"  << std::endl );
            goto excp;
        }
        CYLOG_PRINT(  std::cout<< __func__ << "() " << "write to :" << m_curWriteFilePath << 
                            " offset:" << std::setw(4) << m_curWriteOffset << 
                            " len:" << iLen << std::endl );

        CyLogUtils::Serializer::Serialize<decltype(iLen)>( iLen, sizeof(iLen), _buf );
        _ff.seekp( m_curWriteOffset );
        // _ff.write( (char*)&iLen, sizeof(iLen));
        _ff.write( (char*)&_buf, sizeof(iLen));
        _ff.write( (char*)in, iLen );
        memset( _buf, 0, sizeof(iLen) );
        _ff.write( (char*)&_buf, sizeof(iLen));  // 此处用于将覆盖写过程中，刚刚写入的数据其后紧跟的字节置零，否则文件写位置的检索将异常。
        _ff.close();
        m_curWriteOffset += (sizeof(iLen) + iLen);
    }
#endif

excp:
    lockGive();    
    return _err;

lock_excp:
    return _err;
}

CL_TYPE_t StoreEspidf::dirRead( std::unique_ptr<FileDesc> & pFDesc ) {

    std::filesystem:: path absDir = rootDirGet() + pFDesc->relativePathGet() ;
    std::filesystem::path fPath;
    CYLOG_PRINT(  std::cout << "************** " << __func__ << "(), traverse dir: " << absDir << " **************" << std::endl );

    {
        std::filesystem::directory_iterator _dir_iter( absDir );
        for( auto & _dir : _dir_iter ) {
            fPath = _dir.path();
            CYLOG_PRINT(  std::cout << "  file:" << fPath << std::endl );
        }
    }

    CYLOG_PRINT(  std::cout << "************** " << __func__ << " done **************" << std::endl );
    return CL_OK;
};

/* 遍历文件，查找可写位置 */
CL_TYPE_t StoreEspidf::singleFileTraverse(std::unique_ptr<FileDesc> & pFDesc, std::string & fPath,  FileUsage & fUsage ) {

    uint32_t remainSize = 0;
    uint32_t rOfSet   = 0;
    std::unique_ptr<uint8_t[]> pData;

    /* 识别文件名称中的数字 ID */
    {
        size_t pos = fPath.rfind('_');
        if( pos != std::string::npos ) {
            auto ss = fPath.substr(pos+1);
            fUsage.m_FId = atoi(ss.c_str());
        }
    }

    /* 打开文件 */
    std::ifstream ifs( fPath, std::ios::in | std::ios::binary );
    if( ifs.is_open()==false ) { goto excp; }
    
    /* 读取文件大小 */
    ifs.seekg(0, std::ios::end);
    fUsage.m_Size = ifs.tellg();

    {
        /* 设置读取偏移量 */
        rOfSet = 0;
        remainSize = fUsage.m_Size;

        while( remainSize>0 ) {
            ifs.seekg(rOfSet, std::ios::beg);
            // CYLOG_PRINT(  std::cout<<  fPath << " Next offset:" << rOfSet << std::endl );
            pData = std::make_unique<uint8_t[]>(4); /* 4: 记录头大小 */
            ifs.read(reinterpret_cast<char*>(pData.get()), 4);
            
            auto item = ItemDesc::itemDeSerialize( std::move(pData), 4 );   /*  */
            if( item->isValid()==false ) { break; }  /* 读取记录无效， 此处*/

            /* 日志过滤 */
            pData = std::make_unique<uint8_t[]>(item->itemSizeGet());
            ifs.read(reinterpret_cast<char*>(pData.get()), item->itemSizeGet());
            if( pFDesc->traverCbGet() && pFDesc->traverFilterGet() ) {  /* 回调及过滤函数均已定义， 则执行 */
                if( pFDesc->traverFilterGet()(reinterpret_cast<uint8_t*>(pData.get()), item->itemSizeGet()) ) {
                    pFDesc->traverCbGet()(reinterpret_cast<uint8_t*>(pData.get()), item->itemSizeGet());
                }
            }
            // 日志过滤

            rOfSet += item->itemSizeGet() + 4; /* 4: 记录头大小 */
            remainSize -= rOfSet+4;
        }

        fUsage.m_WOfSet = rOfSet;
    }
    
    ifs.close();
    return CL_OK;
excp:
    return CL_EXCP_UNKNOW;
}


/* 遍历文件，查找可写位置 */
CL_TYPE_t StoreEspidf::multiFilesTraverse(std::unique_ptr<FileDesc> & pFDesc, std::string & fPath,  FileUsage & fUsage ) {

    uint32_t remainSize = 0;
    uint32_t rOfSet   = 0;
    std::unique_ptr<uint8_t[]> pData;

    /* 识别文件名称中的数字 ID */
    {
        size_t pos = fPath.rfind('_');
        if( pos != std::string::npos ) {
            auto ss = fPath.substr(pos+1);
            fUsage.m_FId = atoi(ss.c_str());
        }
    }

    /* 打开文件 */
    std::ifstream ifs( fPath, std::ios::in | std::ios::binary );
    if( ifs.is_open()==false ) { goto excp; }
    
    /* 读取文件大小 */
    ifs.seekg(0, std::ios::end);
    fUsage.m_Size = ifs.tellg();

    {
        /* 设置读取偏移量 */
        rOfSet = 0;
        remainSize = fUsage.m_Size;

        while( remainSize>0 ) {
            ifs.seekg(rOfSet, std::ios::beg);
            // CYLOG_PRINT(  std::cout<<  fPath << " Next offset:" << rOfSet << std::endl );
            pData = std::make_unique<uint8_t[]>(4); /* 4: 记录头大小 */
            ifs.read(reinterpret_cast<char*>(pData.get()), 4);
            
            auto item = ItemDesc::itemDeSerialize( std::move(pData), 4 );   /*  */
            if( item->isValid()==false ) { break; }  /* 读取记录无效， 此处*/

            /* 日志过滤 */
            pData = std::make_unique<uint8_t[]>(item->itemSizeGet());
            ifs.read(reinterpret_cast<char*>(pData.get()), item->itemSizeGet());
            if( pFDesc->traverCbGet() && pFDesc->traverFilterGet() ) {  /* 回调及过滤函数均已定义， 则执行 */
                if( pFDesc->traverFilterGet()(reinterpret_cast<uint8_t*>(pData.get()), item->itemSizeGet()) ) {
                    pFDesc->traverCbGet()(reinterpret_cast<uint8_t*>(pData.get()), item->itemSizeGet());
                }
            }
            // 日志过滤

            rOfSet += item->itemSizeGet() + 4; /* 4: 记录头大小 */
            remainSize -= rOfSet+4;
        }

        fUsage.m_WOfSet = rOfSet;
    }
    
    ifs.close();
    return CL_OK;
excp:
    return CL_EXCP_UNKNOW;
}

CL_TYPE_t StoreEspidf::dirTraverse( std::unique_ptr<FileDesc> & pFDesc, std::vector<std::string> & fList ) {

    std::filesystem:: path logDir = rootDirGet() + "/" + pFDesc->relativePathGet() ;
    std::filesystem::path fPath;
    DIR *pDir = nullptr;
    struct dirent *pDItem = nullptr;

    CYLOG_PRINT(  std::cout << "************** " << __func__ << "(), traverse dir: " << logDir << " **************" << std::endl );
#if 0
    std::filesystem::directory_iterator _dir_iter( logDir );
    for( auto & _dir : _dir_iter ) {
        fList.push_back(static_cast<std::string>( _dir.path() ));
    }
#else

    if( pDir=opendir(logDir.c_str()), pDir==nullptr ) { goto excp; }
    while( pDItem=readdir(pDir), pDItem ) {
        fList.push_back( logDir.string()+"/"+std::string(pDItem->d_name));
    }
#endif

    CYLOG_PRINT(  std::cout << "************** " << __func__ << " done **************" << std::endl );
    return CL_OK;
excp:
    return CL_EXCP_UNKNOW;
}

CL_TYPE_t StoreEspidf::dirDelete( const std::string & absPath ) {

    std::string destFile = absPath; //rootDirGet() + "/" + absPath;

    // 如目录不存在，则跳过
    if( fs::exists(destFile)==false ) {
        CYLOG_PRINT(  std::cout << "No such file dir: "<<destFile<<std::endl );
        goto done; 
    }

    if( fs::is_directory(destFile) ) {
        CYLOG_PRINT(  std::cout << "[ StoreEspidf ] " << destFile << " is a directory" <<std::endl );

        // Traverse dir, ensure it is empty
        {
            DIR * pDir = nullptr;
            dirent * pItem = nullptr;

            if( pDir=opendir(destFile.c_str()), pDir) { // 遍历目录元素
                while( pItem=readdir(pDir), pItem ) {
                    std::string _path = destFile+"/"+pItem->d_name;
                    dirDelete(_path);
                }

                closedir(pDir);
            }
        }

        // 删除空目录
        if( rmdir(destFile.c_str())!=0 ) {
            CYLOG_PRINT(  std::cout << "[ StoreEspidf ] " << destFile << " fail to del directory, err:"<< errno <<std::endl );
            goto excp;
        }
        CYLOG_PRINT(  std::cout << "[ StoreEspidf ] " << destFile << " succ to del directory" <<std::endl );
    } else {
        CYLOG_PRINT(  std::cout << "[ StoreEspidf ] " << destFile << " is a file" <<std::endl );
        // 删除文件
        if( remove(destFile.c_str())!=0 ) {
            CYLOG_PRINT(  std::cout << "[ StoreEspidf ] " << destFile << " fail to del file, err:"<< errno <<std::endl );
            goto excp;
        }
        CYLOG_PRINT(  std::cout << "[ StoreEspidf ] " << destFile << " succ to del file" <<std::endl );
    }
done:
    return CL_OK;
excp:
    return CL_EXCP_UNKNOW;
}

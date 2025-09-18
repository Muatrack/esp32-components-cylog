
#include <stdio.h>
#include <iostream>
// #include <fcntl.h>
// #include <dirent.h>
// #include <unistd.h>
#include <filesystem>
#include "private_include/cylog_store_abs.hpp"
#include <unistd.h>
#ifdef USE_ASSERTION
    #include <assert.h>
#endif

sem_t           StoreAbs::m_signal;
std::string     StoreAbs::m_LogRootDir;

using namespace std;

namespace fs=std::filesystem;

void StoreAbs::StoreInit(uint8_t concurCount, std::string & logRootDir) {
    
    /* 保存日志绝对路径 */
    StoreAbs::m_LogRootDir = logRootDir;

    /** 设置信号量初值 */
    sem_init(&StoreAbs::m_signal, 0, concurCount);

    /** 检查目录 */
    // if(access(logDir.c_str(), 6)==0) 
    if(fs::exists(logRootDir)) {
        std::cout << "The path "<< logRootDir << " does exist" << std::endl;
        goto done;
    }

    std::cout << "The path "<< logRootDir << " doesn't exist. Gonna create it" << std::endl;
    fs::create_directory(logRootDir);
done:;
}

CL_TYPE_t StoreAbs::dirCreate( const std::string & logDir ) {
    bool bRet = false;
    CL_TYPE_t err = CL_OK;
    // std::stringstream ss;
    std::string absPath = rootDirGet() + "/" + logDir;
    /* 检查路径是否已存在 */
    if( std::filesystem::exists(absPath) ) { goto done; }
    /* 不存在，新建 */
    if( bRet=std::filesystem::create_directories(absPath), bRet ) { goto done; } 
    else { err = CL_EXCP_UNKNOW; goto excp; }

excp:
    return err;
done:
    return CL_OK;
}

CL_TYPE_t StoreAbs::fileCreate( std::unique_ptr<FileDesc> & pFDesc, const std::string prefix, uint8_t fCount, uint32_t fSize ) {
    std::string fPath = "";
    std::string absPath = rootDirGet() + "/" + pFDesc->relativePathGet();
    /* 新建存储日志的绝对路径 */
    if( dirCreate(absPath)!=CL_OK ) { goto excp; }

    /* 拼接日志文件名称，在日志目录下逐一生成文件*/
    for(int i=0; i<fCount; i++) {
        fPath = absPath+"/"+prefix+"_"+((i<10)?"0":"") + std::to_string(i);
        // 如果文件存在，则跳过
        if( std::filesystem::exists(fPath) ) continue;

        std::ofstream _f(fPath);
        _f.close();
        // 初始文件的大小
        std::filesystem::resize_file( fPath, fSize );
        std::cout<<"Create log file:"<<fPath<<std::endl;
    }

    return CL_OK;

excp:
    return CL_EXCP_UNKNOW;
}

CL_TYPE_t StoreAbs::itemWrite( std::unique_ptr<CLFile::FileDesc> & pFDesc, const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen) {

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
                    std::cout<< "[ Fatil exception ] :" << __FILE__<< ":"<< __LINE__ << std::endl;
                    return CL_EXCP_UNKNOW;
                #endif
            }
        }

        // 判断当前可写文件是否能够存储待写的日志, 此种情况发生后属于致命异常
        if( (pFDesc->wFileOffsetGet()+iLen)>= pFDesc->fileSizeGet() ) { 
            #ifdef USE_ASSERTION
                assert(false);
            #else
                std::cout<< "[ Fatil exception ] :" << __FILE__<< ":"<< __LINE__ << std::endl;
                return CL_EXCP_UNKNOW;
            #endif
        }
    }
    
    fPath = pFDesc->wFilePathGet();

    // 判断参数有效性
    if( (pIn==nullptr) || (iLen<1) ) {  goto excp;  }
    // 获取读写资源 
    if( lockTake() == false ) {
        cout << "\n------------------------\nFail to get store lock\n" << "------------------------\n" <<endl;
        _err = CL_LOG_BUSY;
        goto lock_excp;
    }

    ofe = std::ofstream( fPath, std::ios::in | std::ios::out | std::ios::binary );
    if( ofe.is_open()==false ) {
        std::cout << "Fail to open file: " << fPath << std::endl;
        goto excp;
    }

    wOff = pFDesc->wFileOffsetGet();
    ofe.seekp( wOff, std::ios::beg);
    std::cout << "Succ to open file: " << fPath << " offset:" << wOff << std::endl;

    // 写数据到文件
    ofe.write( reinterpret_cast<const char*>(pIn.get()), iLen);
    ofe.write( (char*)tailZeroArray, 4);    //写入额外的4个字节0. 当覆盖写的时候便于将就数据自动失效
    pFDesc->wFileOffsetSet( pFDesc->wFileOffsetGet()+ iLen);

    // ofe.flush();
    ofe.close();

    // std::cout << std::endl << "Succ to write file: "<< fPath << " data size:" << iLen << std::endl;

#if 0
re_write:
    // 判断当前文件是否能够写下 iLen 长的数据
    if( (m_curWriteOffset + sizeof(iLen) + iLen + 2) > m_fileMaxLength ) {
        std::cout<< "   " << __func__ << "()." << __LINE__ << std::endl;
        // 如已重选文件，则跳出，否则异常时会出现循环-导致死机
        if( _bReWriten ) {
            _err = CL_FILE_FULL;
            goto excp;
        }
        std::cout<< "   " << __func__ << "()." << __LINE__ << std::endl;
        // 当前文件已写满，选择下一个文件
        nextFileSelect();
        _bReWriten = true;
        goto re_write;
    }
    {
        // 写数据到文件
        std::fstream _ff;
        if( _ff.open( m_curWriteFilePath, std::ios::binary | std::ios::out | std::ios::in ), !_ff.is_open() ) {
            std::cout << "     StoreLinux::itemWrite file closed [ Excep ]"  << std::endl;
            goto excp;
        }
        std::cout<< __func__ << "() " << "write to :" << m_curWriteFilePath << 
                            " offset:" << std::setw(4) << m_curWriteOffset << 
                            " len:" << iLen << std::endl;

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

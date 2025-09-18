
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

/** 
 * 比较全部文件信息，筛选可用于写入的文件
 * @param  vFDecs 收集到的全部文件信息
 * @return 被选中文件的 id, 0xFF 无效ID
 */
static std::unique_ptr<FileUsage> writableFileHit( std::unique_ptr<FileDesc> &fDesc , std::vector<FileUsage> & vFUsage ) {

    uint16_t fileCount = 0;
    bool     bVal = true;
    uint32_t _wOffset = 0;
    uint16_t _fId = 0;
    uint32_t _ts = 0;
    std::unique_ptr<FileUsage> pHitFUsage = nullptr;
    if(fileCount=vFUsage.size(),fileCount<1) { goto excp; }
    std::cout << __FILE__<<":"<<__LINE__<<std::endl;
    goto route_1;

route_1:    /** 
                规则1: (全新文件)
                如全部文件的 wOffset 为0, 则使用文件id为0
            */
    bVal = true;
    for( auto &fu : vFUsage ) {
        if( fu.m_WOfSet!=0 ) { bVal=false; break; }    // 如有一个文件，其写偏移量不为0, 表示文件被使用过。 则不能直接使用id为0的文件
    }
    // 非全部文件为空， 跳至下一规则
    if(bVal==false) { goto route_2; };
    // 全部文件均未被写入, 选择id为0的对象
    for( auto &fu : vFUsage ) {
        if(fu.m_FId==0) {
            pHitFUsage = std::make_unique<FileUsage>(fu);
            goto done;
        }
    }

route_2:    /** 规则2: (全部写满， 重新覆盖)
                如果全部文件均已写满, 比较各文件的最后写入日期, 取日期最小者
            */

    std::cout << __FILE__<<":"<<__LINE__<<std::endl;
    bVal = true;
    // 判断全部文件是否已满
    for( auto &fu : vFUsage ) { if( fu.m_IsFull==false ) { bVal=false; break; } }

    // 非全部文件已写满，跳至规则3
    if(bVal==false) { goto route_3; } 

    // 选择 last time 最小的文件, 并将其offset 设置为0
    _ts = ~1;
    for( auto &fu : vFUsage ) {        
        if( _ts > fu.m_FMTime ) { _ts = fu.m_FMTime; pHitFUsage = std::make_unique<FileUsage>(fu); }
    }
    if(pHitFUsage) {
        pHitFUsage->m_WOfSet = 0;
        goto done; 
    }

route_3:    /** 规则3: (找出正在写入的文件)
                当部分文件被写入后，排除已写满的文件, 排除空文件. 
                按照wOffset 由大到小排序，选择wOffset最小的文件
            */
    std::cout << __FILE__<<":"<<__LINE__<<std::endl;
    _wOffset = 0;
    for( auto &fu : vFUsage ) { // 选择 wOffset 最大的文件
        if( fu.m_IsFull ) { continue; } // 排除已写满的文件
        if( fu.m_WOfSet==0 ) { continue; } // 排除空文件
        if( _wOffset < fu.m_WOfSet ) { _wOffset=fu.m_WOfSet; pHitFUsage = std::make_unique<FileUsage>(fu); }
    }
    if(pHitFUsage) { goto done;  }
    goto route_4;
    
route_4:   /**
            * 规则4: (一些文件一些满， 其余文件均为空)
            * (在空文件-woffset==0, 中选择id最小的)
            * 1. 排除非空文件
            * 3. 选出ID最小的文件
            */

    std::cout << __FILE__<<":"<<__LINE__<<std::endl;
    std::cout<<"[TESTCASE_ROUTE-4] | Usage count:"<<vFUsage.size()<<std::endl;
    _fId = ~1;
    for( auto &fu : vFUsage ) {
        if( fu.m_WOfSet>0 ) { // 排除非空文件
            std::cout<<"[TESTCASE_ROUTE-4] | passed file "<< fu.m_Path<<" offset:" << fu.m_WOfSet<<std::endl;
            continue; 
        }
        std::cout<<"[TESTCASE_ROUTE-4] | Offset "<< _wOffset<<" ? " << fu.m_WOfSet<<std::endl;
        if( _fId>fu.m_FId ) {  // 选贼ID最小的文件
            _fId=fu.m_FId; pHitFUsage = std::make_unique<FileUsage>(fu); 
            std::cout<<"[TESTCASE_ROUTE-4] | find new file "<< fu.m_Path<<" offset:" << fu.m_WOfSet<<std::endl;
        }
    }
    if(pHitFUsage) { goto done;  }
     
    std::cout << __FILE__<<":"<<__LINE__<<std::endl;
    goto excp;

    std::cout << __FILE__<<":"<<__LINE__<<std::endl;
done:
    return pHitFUsage;
excp:
    std::cout << __FILE__<<":"<<__LINE__<<std::endl;
    return nullptr;
}

static uint32_t getFileLastModifyTm( std::string & fPath ) {
    
    namespace fs = std::filesystem;
    // 获取最后修改时间
    auto last_write_time = fs::last_write_time(fPath);

    // 转换为 std::chrono::system_clock::time_point
    auto time_point = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        last_write_time - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );

    // 转换为时间戳（秒）
    auto timestamp_s = std::chrono::duration_cast<std::chrono::seconds>(
        time_point.time_since_epoch()
    ).count();

    #if 0
    // 转换为时间戳（毫秒）
    auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        time_point.time_since_epoch()
    ).count();

    // 输出结果
    std::cout << "文件: " << fPath << std::endl;
    std::cout << "最后修改时间（时间戳，秒）: " << timestamp_s << std::endl;
    std::cout << "最后修改时间（时间戳，毫秒）: " << timestamp_ms << std::endl;

    // 可选：将时间戳转换为可读时间
    auto c_time = std::chrono::system_clock::to_time_t(time_point);
    std::cout << "最后修改时间（可读格式）: " << std::ctime(&c_time);
    #endif

    return static_cast<uint32_t>(timestamp_s);
}

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
    // if( dirCreate(absPath)!=CL_OK ) { goto excp; }
    if( std::filesystem::exists(absPath)==false ) { goto excp; }

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

/* 遍历文件，查找可写位置 */
CL_TYPE_t StoreAbs::fileTraverse( std::string & fPath,  FileUsage & fUsage ) {

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
            // std::cout<<  fPath << " Next offset:" << rOfSet << std::endl;
            pData = std::make_unique<uint8_t[]>(4); /* 4: 记录头大小 */
            ifs.read(reinterpret_cast<char*>(pData.get()), 4);
            
            auto item = ItemDesc::itemDeSerialize( std::move(pData), 4 );   /*  */
            if( item->isValid()==false ) { break; }  /* 读取记录无效， 此处*/

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

CL_TYPE_t StoreAbs::dirTraverse( std::unique_ptr<FileDesc> & pFDesc, std::vector<std::string> & fList ) {

    std::filesystem:: path logDir = rootDirGet() + pFDesc->relativePathGet() ;
    std::filesystem::path fPath;
    std::cout << "************** " << __func__ << "(), traverse dir: " << logDir << " **************" << std::endl;

    std::filesystem::directory_iterator _dir_iter( logDir );
    for( auto & _dir : _dir_iter ) {
        fList.push_back(static_cast<std::string>( _dir.path() ));
    }

    std::cout << "************** " << __func__ << " done **************" << std::endl;
    return CL_OK;
}

/** 
 * 遍历分类日志目录
 * 目的： 选择下一个可写的文件，文件中的可写偏移量
 * 
 * @param pFDesc: 分类日志目录的信息
 */
void StoreAbs::nextFileSelect(std::unique_ptr<FileDesc> & pFDesc) {
    
    std::string fPath;
    auto fList  = std::vector<std::string>();
    std::vector<FileUsage> fUsage;

    /* 遍历目录下的文件名称及大小 */
    dirTraverse( pFDesc, fList );
    std::cout << __func__<<"(), file count:"<< fList.size()<<std::endl;    

    fUsage = std::vector<FileUsage>(fList.size());
    /* 遍历文件内的记录，找出下一个可写的位置 */
    for( size_t i=0;i < fList.size(); i++ ) {
        auto &fu = fUsage[i];
        fPath = static_cast<std::string>(fList[i]);
        fu.m_Path = fPath;
        fileTraverse(fPath, fu);        // 遍历文件中的记录信息
        fu.m_IsFull = isFileFull(fu.m_WOfSet, pFDesc);  // 判断文件是否已写满
        fu.m_FMTime = getFileLastModifyTm( fu.m_Path );         
    }

    /* 筛选下一个可写文件 */
    for( size_t i=0;i < fUsage.size(); i++ ) {
        auto &fu = fUsage[i];
        std::cout<<"File id:"<< static_cast<int>(fu.m_FId)
                                                                            <<" size:"<< fu.m_Size 
                                                                            << " wOffset:"<< std::setw(5) << fu.m_WOfSet
                                                                            << std::setw(9) << (fu.m_IsFull?" full":" not full")
                                                                            << " | Modify tm:" << fu.m_FMTime
                                                                            <<std::endl;
    }    

    std::unique_ptr<FileUsage> pHitFu = writableFileHit(pFDesc ,fUsage);
    #ifdef USE_ASSERTION
        assert(pHitFu);
    #else
        std::cout<< "[ Fatil exception ] :" << __FILE__<< ":"<< __LINE__ << std::endl;
        return;
    #endif
    pFDesc->wFileOffsetSet(pHitFu->m_WOfSet);
    pFDesc->wFilePathSet(pHitFu->m_Path);

    std::cout<<"[ Got a writable ] "<<" fsize:"<< static_cast<uint32_t>(pHitFu->m_Size)<<" wOff:"<< std::setw(4) << static_cast<uint32_t>(pHitFu->m_WOfSet) << " path:" << static_cast<std::string>(pHitFu->m_Path)<<std::endl;
}

#ifdef USE_ASSERTION
    #include <assert.h>
#endif

typedef struct {
    std::string name;   /* 文件名称 */
    uint32_t    size;   /* 文件大小 */
    uint32_t    wOfSet; /* 可写偏移量 */
} file_usage_t;

CL_TYPE_t StoreAbs::dirRead( std::unique_ptr<FileDesc> & pFDesc ) {

    std::filesystem:: path absDir = rootDirGet() + pFDesc->relativePathGet() ;
    std::filesystem::path fPath;
    std::cout << "************** " << __func__ << "(), traverse dir: " << absDir << " **************" << std::endl;

    {
        std::filesystem::directory_iterator _dir_iter( absDir );
        for( auto & _dir : _dir_iter ) {
            fPath = _dir.path();
            std::cout << "  file:" << fPath << std::endl;
        }
    }

    std::cout << "************** " << __func__ << " done **************" << std::endl;
    return CL_OK;
};


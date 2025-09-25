
#include <stdio.h>
#include <iostream>
// #include <fcntl.h>
// #include <dirent.h>
// #include <unistd.h>
#include <filesystem>
#include "private_include/cylog_store_abs.hpp"
#include <unistd.h>
#include <sys/stat.h>

#ifdef USE_ASSERTION
    #include <assert.h>
#endif

sem_t           StoreAbs::m_signal;
std::string     StoreAbs::m_LogRootDir;

extern "C" __attribute__((weak)) uint32_t global_cylog_create_ts_get(uint8_t pData[], uint16_t dLen);
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
    goto route_1;

route_1:
    /** 
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

route_2:    
    /** 规则2: (全部写满， 重新覆盖)
        如果全部文件均已写满, 比较各文件的最后写入日期, 取日期最小者
    */

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

route_3:    
    /** 规则3: (找出正在写入的文件)
        当部分文件被写入后，排除已写满的文件, 排除空文件. 
        按照wOffset 由大到小排序，选择wOffset最小的文件
    */
    _wOffset = 0;
    for( auto &fu : vFUsage ) { // 选择 wOffset 最大的文件
        if( fu.m_IsFull ) { continue; } // 排除已写满的文件
        if( fu.m_WOfSet==0 ) { continue; } // 排除空文件
        if( _wOffset < fu.m_WOfSet ) { _wOffset=fu.m_WOfSet; pHitFUsage = std::make_unique<FileUsage>(fu); }
    }
    if(pHitFUsage) { goto done;  }
    goto route_4;
    
route_4:
    /**
    * 规则4: (一些文件一些满， 其余文件均为空)
    * (在空文件-woffset==0, 中选择id最小的)
    * 1. 排除非空文件
    * 3. 选出ID最小的文件
    */

    _fId = ~1;
    for( auto &fu : vFUsage ) {
        if( fu.m_WOfSet>0 ) { // 排除非空文件
            continue; 
        }
        if( _fId>fu.m_FId ) {  // 选贼ID最小的文件
            _fId=fu.m_FId; pHitFUsage = std::make_unique<FileUsage>(fu); 
        }
    }
    if(pHitFUsage) { goto done;  }
     
    CYLOG_PRINT(  std::cout << __FILE__<<":"<<__LINE__<<std::endl );
    goto excp;

    CYLOG_PRINT(  std::cout << __FILE__<<":"<<__LINE__<<std::endl );
done:
    return pHitFUsage;
excp:
    CYLOG_PRINT(  std::cout << __FILE__<<":"<<__LINE__<<std::endl );
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
    CYLOG_PRINT(  std::cout << "文件: " << fPath << std::endl );
    CYLOG_PRINT(  std::cout << "最后修改时间（时间戳，秒）: " << timestamp_s << std::endl );
    CYLOG_PRINT(  std::cout << "最后修改时间（时间戳，毫秒）: " << timestamp_ms << std::endl );

    // 可选：将时间戳转换为可读时间
    auto c_time = std::chrono::system_clock::to_time_t(time_point);
    CYLOG_PRINT(  std::cout << "最后修改时间（可读格式）: " << std::ctime(&c_time)<<std::endl );
    #endif

    return static_cast<uint32_t>(timestamp_s);
}

void StoreAbs::StoreInit(uint8_t concurCount, std::string & logRootDir) {
    
    /* 保存日志绝对路径 */
    StoreAbs::m_LogRootDir = logRootDir;

    CYLOG_PRINT(  std::cout << "StoreAbs::StoreInit() | root path:"<< StoreAbs::m_LogRootDir  << std::endl );
    /** 设置信号量初值 */
    sem_init(&StoreAbs::m_signal, 0, concurCount);

    /** 检查目录 */
    std::string mp = "/sdb/init";
    CYLOG_PRINT(  std::cout << "StoreAbs::StoreInit() | The path "<< mp << (fs::exists(mp)?" does exist":" doesn't exist") << std::endl );

    if(fs::exists(StoreAbs::m_LogRootDir)) {
        CYLOG_PRINT(  std::cout << "StoreAbs::StoreInit() | The path "<< StoreAbs::m_LogRootDir << " does exist" << std::endl );
        goto done;
    } else {
        CYLOG_PRINT(  std::cout << "StoreAbs::StoreInit() | The path "<< StoreAbs::m_LogRootDir << " doesn't exist. Gonna create it" << std::endl );
    }
#if 1
    fs::create_directory( StoreAbs::m_LogRootDir );
#else
    if(mkdir(StoreAbs::m_LogRootDir.c_str(), 0777)!=0) {
        CYLOG_PRINT(  std::cout << "Fail to create dir: "<< StoreAbs::m_LogRootDir << " errno:"<< errno << std::endl );
    } else {
        CYLOG_PRINT(  std::cout << "Succ to create dir: "<< StoreAbs::m_LogRootDir << std::endl );
    }
#endif
done:;
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
    CYLOG_PRINT(  std::cout << __func__<<"(), file count:"<< fList.size()<<std::endl );    

    fUsage = std::vector<FileUsage>(fList.size());
    /* 遍历文件内的记录，找出下一个可写的位置 */
    for( size_t i=0;i < fList.size(); i++ ) {
        CYLOG_PRINT( std::cout<<__func__<<"():"<<__LINE__<<std::endl );
        auto &fu = fUsage[i];
        fPath = static_cast<std::string>(fList[i]);
        fu.m_Path = fPath;
        // multiFilesTraverse(pFDesc, fPath, fu);        // 遍历文件中的记录信息
        singleFileTraverse(pFDesc, fPath, fu);
        fu.m_IsFull = isFileFull(fu.m_WOfSet, pFDesc);  // 判断文件是否已写满
        fu.m_FMTime = getFileLastModifyTm( fu.m_Path );
    }

    /* 筛选下一个可写文件 */
    CYLOG_PRINT(
        for( size_t i=0;i < fUsage.size(); i++ ) {
            auto &fu = fUsage[i];
            std::cout<<"File id:"<< std::setfill('0')<<std::setw(3)  << static_cast<int>(fu.m_FId)
                                                                                <<" size:"<< fu.m_Size 
                                                                                << " wOffset:"<< std::setw(5) << fu.m_WOfSet
                                                                                << std::setfill(' ')<<std::setw(9) << (fu.m_IsFull?" full":" not full")
                                                                                << " | Modify tm:" << fu.m_FMTime
                                                                                <<std::endl;
        }
    )

    std::unique_ptr<FileUsage> pHitFu = writableFileHit(pFDesc ,fUsage);
    #ifdef USE_ASSERTION
        assert(pHitFu);
    #else
        CYLOG_PRINT(  std::cout<< "[ Fatil exception ] :" << __FILE__<< ":"<< __LINE__ << std::endl );
        return;
    #endif
    pFDesc->wFileOffsetSet(pHitFu->m_WOfSet);
    pFDesc->wFilePathSet(pHitFu->m_Path);

    CYLOG_PRINT(  std::cout<<"[ Got a writable ] "<<" fsize:"<< static_cast<uint32_t>(pHitFu->m_Size)<<" wOff:"<< std::setw(4) << static_cast<uint32_t>(pHitFu->m_WOfSet) << " path:" << static_cast<std::string>(pHitFu->m_Path)<<std::endl );
}

#ifdef USE_ASSERTION
    #include <assert.h>
#endif

typedef struct {
    std::string name;   /* 文件名称 */
    uint32_t    size;   /* 文件大小 */
    uint32_t    wOfSet; /* 可写偏移量 */
} file_usage_t;

bool StoreAbs::doesExists( std::string & path ) {
    struct stat st;

    if( path=="" ) { goto excp; }    
    if( stat(path.c_str(), &st)==0 ) {
        return true;
    } else {
        return false;
    }

excp:
    return false;
}

/*
 * 缓存数据检查逻辑：
 * [1] 从缓存首个字节开始
 * [2] 将缓存数据实例化，检查数据是否有效
 * [3] - 数据无效，则前进一个字节, 重复[2]
 * [4] - 数据有效，则找到数据长度，将原数据回传至调用上层
 * [5] 如缓存剩余数据长度不足实例头部定义的长度，当前数据算做为识别。返回当前已识别的数据位置
 * [6] 如缓存剩余数据长度满足实例头部定义的长度，读取的位置更新为内存中当前数据的尾部出数据的长度
*/
uint32_t StoreAbs::memBlockTraverse( std::unique_ptr<FileDesc> & pFDesc, FileUsage & fUsage, uint32_t absOffset, std::unique_ptr<uint8_t[]> & pData, uint32_t dLen ) {
    uint32_t checkedSize = 0;
    uint32_t remainSize  = dLen;
    uint32_t maxTs = 0;
    uint32_t itemTs = 0;    

    while ( remainSize>4 ) {
        std::unique_ptr<uint8_t[]> pHead = std::make_unique<uint8_t[]>(4);
        /* 拷贝数据头, 大小4个字节 */
        std::copy(pData.get()+checkedSize, pData.get()+checkedSize + 4, pHead.get());

        /* 实例化数据 */
        auto item = ItemDesc::itemDeSerialize( pHead, 4 );

        /* 读取记录无效，检查字节的索引前进一个字节并再次重复检查  */
        if( item->isValid()==false ) {
            checkedSize++;
            remainSize --;
            continue; 
        }

        /* 检查剩余数据是否能够涵盖数据体, 如不能涵盖，跳出 */
        if( remainSize<(uint32_t)(item->itemSizeGet()+4) ) { break; }
        checkedSize += 4;
        remainSize  -= 4;
    
        /* 如数据有效， 则继续将内存中的数据返回给上层进一步检查 */
        std::unique_ptr<uint8_t[]> pBody = std::make_unique<uint8_t[]>( item->itemSizeGet() );
        std::copy( pData.get() + checkedSize, pData.get() + checkedSize + item->itemSizeGet(), pBody.get());

        if( pFDesc->traverCbGet() && pFDesc->traverFilterGet() ) {  /* 回调及过滤函数均已定义， 则执行 */
            if( pFDesc->traverFilterGet()(reinterpret_cast<uint8_t*>(pBody.get()), item->itemSizeGet()) ) {
                pFDesc->traverCbGet()(reinterpret_cast<uint8_t*>(pBody.get()), item->itemSizeGet());
            }
        }

        /** 拦截日志，读取其创建时间戳, 找到创建时间最大的日志并记录其偏移量，用作再次写入的位置  */
        if( global_cylog_create_ts_get==nullptr ) { goto jump; }
        if( itemTs=global_cylog_create_ts_get(reinterpret_cast<uint8_t*>(pBody.get()), item->itemSizeGet()), itemTs>=maxTs ) {
            maxTs = itemTs;
            uint32_t tOffset = absOffset + checkedSize + item->itemSizeGet();
            fUsage.m_WOfSet = tOffset;
        }
    jump:

        checkedSize += item->itemSizeGet();
        remainSize  -= item->itemSizeGet();
    }

    // CYLOG_PRINT( std::cout<<__func__<<":"<<__LINE__<<" checkedSize:"<<checkedSize<<std::endl );
    return checkedSize;
}

CL_TYPE_t StoreAbs::singleFileTraverse(std::unique_ptr<FileDesc> & pFDesc, std::string & fPath,  FileUsage & fUsage ) {

    uint32_t  remainSize = 0;
    uint32_t readSize = 0;
    uint32_t checkedSize = 0;
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

        CYLOG_PRINT( std::cout<<__func__<<":"<<__LINE__<<" fPath:"<< fPath << " fsize:"<<fUsage.m_Size<<std::endl );

        pData = std::make_unique<uint8_t[]>( CYLOG_TRAVERSAL_BLOCK_SIZE );
        CYLOG_PRINT( std::cout<< "CYLOG_TRAVERSAL_BLOCK_SIZE: " << CYLOG_TRAVERSAL_BLOCK_SIZE<< " hole size:"<< pFDesc->tailHoleGet() <<std::endl );

        while( remainSize > pFDesc->tailHoleGet() ) {
            ifs.seekg(rOfSet, std::ios::beg);
            readSize = MIN(remainSize, CYLOG_TRAVERSAL_BLOCK_SIZE);
             /* 4: 记录头大小 */
            ifs.read(reinterpret_cast<char*>(pData.get()), readSize);
            /* 遍历数据，检查有效性 */
            checkedSize = memBlockTraverse( pFDesc, fUsage, rOfSet, pData, ifs.gcount() );
            // 遍历数据，检查有效性

            CYLOG_PRINT( std::cout<<__func__<<"():"<<__LINE__<<" offset: "<<rOfSet<<
                            " expect reading size:"<< readSize << " final read size:"<< ifs.gcount()<<
                            " remain:"<<remainSize<<" checked size:"<<checkedSize <<std::endl
            );

            rOfSet += checkedSize;
            remainSize -= checkedSize;
        }
    }
    
    ifs.close();
    return CL_OK;
excp:
    return CL_EXCP_UNKNOW;
}


#include <iostream>
/* c++ 17 */
#include <filesystem>
// c++ 17
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <unistd.h>
#include "private_include/cylog_store_linux.hpp"
#include "private_include/cylog_file.hpp"

#ifdef USE_ASSERTION
    #include <assert.h>
#endif

typedef struct {
    std::string name;   /* 文件名称 */
    uint32_t    size;   /* 文件大小 */
    uint32_t    wOfSet; /* 可写偏移量 */
} file_usage_t;

/** 
 * 日志目录初始化 
 *      1 目录存在否,
 *      2 文件全数存在否， 
 *      3 遍历文件 找出下一个写数据的目标文件和对应的写数据位置
 */
CL_TYPE_t StoreLinux::init() {
    std::cout<< "------------------- StoreLinux::init() -------------------" << std::endl;
#if 0
    std::shared_ptr<std::vector<FileDesc>> spFHeadList = std::make_shared<std::vector<FileDesc>>();
    // 目录检查
    if( std::filesystem::exists(m_dirPath) ) { //路径存在,执行检查
        std::cout<< "StoreLinux::init dir " << m_dirPath << " exists." << std::endl;
        dirCheck();
    } else { // 路径不存在， 执行新建
        // dirCreate();
    }

    // 遍历目录，收集文件信息, traversal dir, list all head struct of files.
    dirRead( spFHeadList );

    // 依据上步文件信息，计算下一个写数据的文件路径及对应的写数据偏移位置
        // 选文件计算方法
    latestFileSelect( spFHeadList );

    // 执行一次选择文件
    nextFileSelect();
#endif
    std::cout<< "------------------- StoreLinux::init DONE -------------------" << std::endl;
    return CL_OK;
}
#if 0
void StoreLinux::configSet(uint8_t fMaxCount, uint32_t fMaxLen, const std::string &fDir, const std::string &fPrefix) {
    m_fileMaxCount = fMaxCount;
    m_fileMaxLength = fMaxLen;
    m_fileCurCount = 0;
    m_dirPath = fDir;
    m_fileNamePrefix = fPrefix;
    m_fileHoleSize   = 32;

    std::cout<< "StoreLinux::configSet()" << std::endl;
}
#endif

CL_TYPE_t StoreLinux::dirCreate_bak() {
    CL_TYPE_t err = CL_OK;
    std::stringstream ss;
#if 0
    /* 新建路径 */
    if( std::filesystem::create_directories( m_dirPath ) == false ) {
        std::cout<< "StoreLinux::init dir " << m_dirPath << " fail to create dir." << std::endl;
        err = CL_DIR_NOT_EXIST;
        goto excp;
    }
    
    /* 新建全数文件 */
    {
        std::cout<< "StoreLinux::init dir " << m_dirPath << " succ to create dir." << std::endl;
        std::filesystem::path f_path;
        FileHead _f_Head( m_fileMaxLength );

        for( uint32_t i=0; i<m_fileMaxCount; i++ ) {
            ss.str("");
            ss << std::setw(2) << std::setfill('0') << i ;
            f_path = m_dirPath + ss.str();
            std::cout<< "   gonna create file: " << f_path << std::endl;
            std::ofstream _of(f_path, std::ios::out | std::ios::binary | std::ios::app);
            std::cout<< "   file: " << f_path << " resize to " << m_fileMaxLength << std::endl;
            std::filesystem::resize_file( f_path, m_fileMaxLength );
            if( !_of.is_open() ) {
                // 文件没有打开，新建失败
                std::cout << "Fail to create file:" << f_path << " with errno:" << errno << std::endl;
                continue;
            }
            // 写入头数据到目标文件
            headWrite( f_path );
        }
    }
#endif
    return err;
// excp:
//     return err;

}

CL_TYPE_t StoreLinux::dirRead( std::unique_ptr<FileDesc> & pFDesc ) {

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

#if 0
/** 
 * 
 * 此成员函数将会改名为 fileReset( ... )
 *  - 写入文件头部数据到指定的文件 v1.1
 *  - 将文件数据部分全写0，否则会造成文件写偏移量的计算错误 v1.1
*/
CL_TYPE_t StoreLinux::headWrite( const std::filesystem::path &fPath ){
    std::shared_ptr<FileHead> fHead = std::make_shared<FileHead>(m_fileMaxLength);
    std::fstream _ff;
    const uint8_t* pSeried = fHead->serialize();

    if( _ff.open( fPath, std::ios::binary | std::ios::out | std::ios::in ), !_ff.is_open() ) {
        std::cout << "     StoreLinux::headWrite file closed [ Excep ]"  << std::endl;
        goto excp;
    }

    // 写入文件头数据
    _ff.seekp(0);
    _ff.write((char*)pSeried, FileHead::sizeGet());

    #if 1   // v1.0
        // 将文件头后面的2个字节清0, 表示紧邻的一包数据大小为0。否则，虽然文件头部数据被刷新，当此文件被遍历是依旧能够读取到旧数据
        _ff << "\0\0";
    #else   // v1.1
    #endif
    std::cout << __func__ << "()." << __LINE__ << std::endl;
    return CL_OK;
excp:
    return CL_EXCP_UNKNOW;
};

/** 
 * 读取文件头部数据, 从指定的文件
 * @param fPath 文件路径
*/
CL_TYPE_t StoreLinux::headRead( const std::filesystem::path &fPath ) {

    std::shared_ptr<FileHead> fHead = std::make_shared<FileHead>(m_fileMaxLength);
    std::fstream _ff;

    if( _ff.open( fPath, std::ios::binary | std::ios::out | std::ios::in ), !_ff.is_open() ) {
        std::cout << "     StoreLinux::headWrite file closed [ Excep ]"  << std::endl;
        goto excp;
    }

    _ff.seekp(0);
    _ff.read( (char*)fHead->bytesBufGet(), FileHead::sizeGet() );
    _ff.close();

    fHead->deSerialize();
    std::cout << "  Got file head ver:" << fHead->verGet() << " fileLen:" << fHead->maxLenGet() << " reWriteTm:" << fHead->reWriteTmGet()  << std::endl;
    return CL_OK;
excp:
    return CL_EXCP_UNKNOW;
}
#endif

void StoreLinux::latestFileSelect( std::shared_ptr<std::vector<FileDesc>> & spFHeadList ) {
    // uint64_t _reWriteTs = 0;       // 最后重写入时间
    // uint16_t _listIndexSelected = 0; // 遍历过程中选中的数据索引
#if 0
    std::cout << "**************** " << __func__ << " **************" << std::endl;
    // 遍历列表，筛选重写时间戳最大的文件作为写操作目标文件
    {
        for( uint32_t i=0;i < spFHeadList->size(); i ++ ) {
            std::cout   << "  file:" << spFHeadList->at(i).filePrefixGet() 
                        << " offset:" << std::setw(4) << spFHeadList->at(i).wFileOffsetGet() 
            << std::endl;
        }
        m_curWriteFilePath = m_dirPath + spFHeadList->at(_listIndexSelected).filePrefixGet();
    }

    // 遍历目标文件中可写数据的绝对位置(写入偏移量)
    {
        std::shared_ptr<FileItem> spFileItem = std::make_shared<FileItem>();
        std::filesystem::path fPath = m_curWriteFilePath;
        m_curWriteOffset = spFileItem->wOffsetGet( fPath );
    }
    
    std::cout << "    [ Current Writable File: " << m_curWriteFilePath << ", offset: " << m_curWriteOffset << std::endl;
#endif
    std::cout << "************* " << __func__ << " done **************" << std::endl;
}

/** 
 * 比较全部文件信息，筛选可用于写入的文件
 * @param  vFDecs 收集到的全部文件信息
 * @return 被选中文件的 id, 0xFF 无效ID
 */
static std::unique_ptr<FileUsage> writableFileHit( std::unique_ptr<FileDesc> &fDesc , std::vector<FileUsage> & vFUsage ) {

    uint16_t fileCount = 0;
    bool     bVal = true;
    std::unique_ptr<FileUsage> pHitFUsage = nullptr;
    if(fileCount=vFUsage.size(),fileCount<1) { goto excp; }
    std::cout << __FILE__<<":"<<__LINE__<<std::endl;
    goto route_1;

route_1:    /** 规则1: 如全部文件的 wOffset 为0, 则使用文件id为0 */    
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

route_2:    /** 规则2:  如果全部文件均已写满, 比较各文件的最后写入日期, 取日期最小者 */

    std::cout << __FILE__<<":"<<__LINE__<<std::endl;    
    bVal = true;
    // 判断全部文件是否已满
    for( auto &fu : vFUsage ) { if( fu.m_IsFull==false ) { bVal=false; break; } }

    // 非全部文件已写满，跳至规则3
    if(bVal==false) { goto route_3; } 

    // 选择 last time 最小的文件, 并将其offset 设置为0
    for( auto &fu : vFUsage ) {
        static uint32_t _ts = ~1;
        if( _ts > fu.m_FMTime ) { _ts = fu.m_FMTime; pHitFUsage = std::make_unique<FileUsage>(fu); }
    }
    if(pHitFUsage) {
        pHitFUsage->m_WOfSet = 0;
        goto done; 
    }

route_3:    /** 规则3:
                当部分文件被写入后，排除已写满的文件, 排除空文件. 
                按照wOffset 由大到小排序，选择wOffset最小的文件
            */
    std::cout << __FILE__<<":"<<__LINE__<<std::endl;    
    for( auto &fu : vFUsage ) { // 选择 wOffset 最大的文件
        static uint32_t _wOffset = 0;
        if( fu.m_IsFull ) { continue; } // 排除已写满的文件
        if( fu.m_WOfSet==0 ) { continue; } // 排除空文件
        if( _wOffset < fu.m_WOfSet ) { _wOffset=fu.m_WOfSet; pHitFUsage = std::make_unique<FileUsage>(fu); }
    }
    if(pHitFUsage) { goto done;  }
    goto route_4;
    
route_4:    /** 
                * 规则4: 当前文件被写满后，使用预期ID相邻的下一个文件
                * (在空文件-woffset==0, 中选择id最小的)
                * 1. 解析当前文件的 ID
                * 2. 得到下一个可写文件的ID，找到新ID对应的 fUsage
                * 3. 将可写文件的 wOffset设置为 0
            */

    std::cout << __FILE__<<":"<<__LINE__<<std::endl;

    for( auto &fu : vFUsage ) { // 选择 wOffset 最大的文件
        static uint32_t _wOffset = ~1;
        if( fu.m_WOfSet>0 ) { continue; } // 排除非空文件
        if( _wOffset > fu.m_WOfSet ) { _wOffset=fu.m_WOfSet; pHitFUsage = std::make_unique<FileUsage>(fu); }
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

uint32_t getFileLastModifyTm( std::string & fPath ) {
    
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

/** 
 * 遍历分类日志目录
 * 目的： 选择下一个可写的文件，文件中的可写偏移量
 * 
 * @param pFDesc: 分类日志目录的信息
 */
void StoreLinux::nextFileSelect(std::unique_ptr<FileDesc> & pFDesc) {
    
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

CL_TYPE_t StoreLinux::dirTraverse( std::unique_ptr<FileDesc> & pFDesc, std::vector<std::string> & fList ) {

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

/* 遍历文件，查找可写位置 */
CL_TYPE_t StoreLinux::fileTraverse( std::string & fPath,  FileUsage & fUsage ) {

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

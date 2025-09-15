
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

/** 
 * 日志目录初始化 
 *      1 目录存在否,
 *      2 文件全数存在否， 
 *      3 遍历文件 找出下一个写数据的目标文件和对应的写数据位置
 */
CL_TYPE_t StoreLinux::init() {
    std::cout<< "------------------- StoreLinux::init() -------------------" << std::endl;
#if 0
    std::shared_ptr<std::vector<CLFile::FileDesc>> spFHeadList = std::make_shared<std::vector<CLFile::FileDesc>>();
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

CL_TYPE_t StoreLinux::dirCreate( const std::string & absPath) {
    bool bRet = false;
    CL_TYPE_t err = CL_OK;

    std::stringstream ss;

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
        CLFile::FileHead _f_Head( m_fileMaxLength );

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

CL_TYPE_t StoreLinux::dirRead( std::unique_ptr<CLFile::FileDesc> & pFDesc ) {

    std::filesystem:: path absDir = rootDirGet() + pFDesc->relativePathGet() ;
    std::filesystem::path fPath;
    std::cout << "************** " << __func__ << "(), traverse dir: " << absDir << " **************" << std::endl;

    {
        std::filesystem::directory_iterator _dir_iter( absDir );
        for( auto & _dir : _dir_iter ) {
            fPath = _dir.path();
            // spFileHead->deSerialize( fPath );
            // pfHeadList->push_back( CLFile::FileDesc( fPath.filename(), fPath.root_directory(), spFileHead->sizeGet(), 
            // spFileItem->wOffsetGet(fPath), spFileHead->reWriteTmGet()) );
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
    std::shared_ptr<CLFile::FileHead> fHead = std::make_shared<CLFile::FileHead>(m_fileMaxLength);
    std::fstream _ff;
    const uint8_t* pSeried = fHead->serialize();

    if( _ff.open( fPath, std::ios::binary | std::ios::out | std::ios::in ), !_ff.is_open() ) {
        std::cout << "     StoreLinux::headWrite file closed [ Excep ]"  << std::endl;
        goto excp;
    }

    // 写入文件头数据
    _ff.seekp(0);
    _ff.write((char*)pSeried, CLFile::FileHead::sizeGet());

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

    std::shared_ptr<CLFile::FileHead> fHead = std::make_shared<CLFile::FileHead>(m_fileMaxLength);
    std::fstream _ff;

    if( _ff.open( fPath, std::ios::binary | std::ios::out | std::ios::in ), !_ff.is_open() ) {
        std::cout << "     StoreLinux::headWrite file closed [ Excep ]"  << std::endl;
        goto excp;
    }

    _ff.seekp(0);
    _ff.read( (char*)fHead->bytesBufGet(), CLFile::FileHead::sizeGet() );
    _ff.close();

    fHead->deSerialize();
    std::cout << "  Got file head ver:" << fHead->verGet() << " fileLen:" << fHead->maxLenGet() << " reWriteTm:" << fHead->reWriteTmGet()  << std::endl;
    return CL_OK;
excp:
    return CL_EXCP_UNKNOW;
}
#endif

CL_TYPE_t StoreLinux::itemWrite( std::unique_ptr<CLFile::FileDesc> & pFDesc, const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen) {
    CL_TYPE_t _err = CL_OK;
    // bool _bReWriten = false;
    // uint8_t _buf[2] = {0};

    cout << "Gonna write data to " << pFDesc->wFilePathGet() << "/" << pFDesc->filePrefixGet() << "/..." << endl;

    // 判断参数有效性
    if( (pIn==nullptr) || (iLen<1) ) {
        goto excp;
    }

    // 获取读写资源 
    if( lockTake() == false ) {
        cout << "\n------------------------\nFail to get store lock\n" << "------------------------\n" <<endl;
        _err = CL_LOG_BUSY;
        goto lock_excp;
    }
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
};

void StoreLinux::latestFileSelect( std::shared_ptr<std::vector<CLFile::FileDesc>> & spFHeadList ) {
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
        std::shared_ptr<CLFile::FileItem> spFileItem = std::make_shared<CLFile::FileItem>();
        std::filesystem::path fPath = m_curWriteFilePath;
        m_curWriteOffset = spFileItem->wOffsetGet( fPath );
    }
    
    std::cout << "    [ Current Writable File: " << m_curWriteFilePath << ", offset: " << m_curWriteOffset << std::endl;
#endif
    std::cout << "************* " << __func__ << " done **************" << std::endl;
}

void StoreLinux::nextFileSelect() {
    /** 
     * 依据当前已使用的文件名称，拼接下一个选中的文件
     */
#if 0    
    uint16_t _curFileIdx = 0xff;
    std::stringstream ss;
    std::shared_ptr<CLFile::FileHead> spFHead = std::make_shared<CLFile::FileHead>();

    if( isCurFileFull() == false ) {
        // 当前文件未写满， 继续使用
        goto done;
    }

    {
        // 获取当前日志分类文件的数量， 依据当前使用的文件名拼接下一个文件名
        std::cout << "StoreLinux::nextFileSelect cur file path:" << m_curWriteFilePath;
        std::filesystem::path curPath = m_curWriteFilePath;
        std::cout << "  root path:" << m_dirPath;
        _curFileIdx = atoi(curPath.stem().c_str());
        _curFileIdx = (_curFileIdx + 1) % m_fileMaxCount;
        std::cout << "  next file idx:" << _curFileIdx << std::endl;

        { // 拼接新路径, 重置 写操作的偏移量
            ss.str("");
            ss << std::setw(2) << std::setfill('0') << _curFileIdx ;
            m_curWriteFilePath = m_dirPath + ss.str();
            m_curWriteOffset = spFHead->sizeGet();
        }

        std::cout << __func__ << "()." << __LINE__ << std::endl;
        // 执行文件头更新
        headWrite( m_curWriteFilePath );

        std::cout << "  write offset:" << m_curWriteOffset << std::endl;
    }
#endif

//done:
    return;
}

CL_TYPE_t StoreLinux::traverse( std::unique_ptr<CLFile::FileDesc> & pFDesc ) {

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
}



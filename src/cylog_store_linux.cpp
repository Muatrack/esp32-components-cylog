
#include <iostream>
/* c++ 17 */
#include <filesystem>
// c++ 17
#include <fstream>
#include <sstream>
#include <iomanip>
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

    // 目录检查
    if( std::filesystem::exists(m_dirPath) ) { //路径存在,执行检查
        std::cout<< "StoreLinux::init dir " << m_dirPath << " exists." << std::endl;
        dirCheck();
    } else { // 路径不存在， 执行新建
        dirCreate();
    }

    // 遍历目录，收集文件信息
        // traversal dir, list all head struct of files.

    // 依据上步文件信息，计算下一个写数据的文件路径及对应的写数据偏移位置
        // 选文件计算方法

    std::cout<< "------------------- StoreLinux::init DONE -------------------" << std::endl;
    return CL_OK;
}

void StoreLinux::configSet(uint8_t fMaxCount, uint32_t fMaxLen, const std::string &fDir, const std::string &fPrefix) {
    m_fileMaxCount = fMaxCount;
    m_fileMaxLength = fMaxLen;
    m_fileCurCount = 0;
    m_dirPath = fDir;
    m_fileNamePrefix = fPrefix;

    std::cout<< "StoreLinux::configSet()" << std::endl;
}

/* 检查指定目录中文件的合法性 */
CL_TYPE_t StoreLinux::dirCheck() {
    std::cout << __FILE__ << "::" << __func__ <<"()." << __LINE__<< std::endl;
    return CL_OK;
};

CL_TYPE_t StoreLinux::dirCreate() {
    CL_TYPE_t err = CL_OK;

    std::cout<< "StoreLinux::init dir " << m_dirPath << " doesn't exists." << std::endl;

    std::stringstream ss;
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

            /* test */
            headRead( f_path );
            // test
        }
    }

    return err;
excp:
    return err;

}

CL_TYPE_t StoreLinux::dirRead() {
    std::cout << __FILE__ << "::" << __func__ <<"()." << __LINE__<< std::endl;
    std::cout << "... got dirpath:" << m_dirPath.c_str() << std::endl;

    /* try read */
    if( 0 ) {
        std::filesystem::path f_path;
        char buf[36] = {0};
        std::ifstream _if(f_path, std::ios::in | std::ios::binary);
        if( _if.is_open() ) {
            _if.read(buf, 12);
            std::cout << "  file: " << f_path << " read val:: " << buf << std::endl;

            _if.close();
        }
    }
    // try read

    return CL_OK;
};


/** 
 * 写入文件头部数据到指定的文件
 * @param fPath 文件路径
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
    // 将文件头后面的2个字节清0, 表示紧邻的一包数据大小为0。否则，虽然文件头部数据被刷新，当此文件被遍历是依旧能够读取到旧数据
    _ff << "\0\0";
    _ff.close();

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
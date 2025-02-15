
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

CL_TYPE_t StoreLinux::init() {
    CL_TYPE_t err = CL_OK;
    std::stringstream ss;

    std::cout<< "------------------- StoreLinux::init() -------------------" << std::endl;

    /** 判断目录是否存在 */
    if( std::filesystem::exists(m_dirPath) ) {
        std::cout<< "StoreLinux::init dir " << m_dirPath << " exists." << std::endl;        
    } else {
        std::cout<< "StoreLinux::init dir " << m_dirPath << " doesn't exists." << std::endl;

        /* 新建路径 */
        if( std::filesystem::create_directories( m_dirPath ) == false ) {
            std::cout<< "StoreLinux::init dir " << m_dirPath << " fail to create dir." << std::endl;
            err = CL_DIR_NOT_EXIST;
            goto excp;
        }
        
        /* 新建全数文件 */
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
    /** 遍历目录下所有文件 */

    std::cout<< "------------------- StoreLinux::init DONE -------------------" << std::endl;
    return err;
excp:
    return err;
}

void StoreLinux::configSet(uint8_t fMaxCount, uint32_t fMaxLen, const std::string &fDir, const std::string &fPrefix) {
    // std::cout << "maxCount:" << (uint32_t)fMaxCount << std::endl;
    // std::cout << "curCount:" << fMaxLen << std::endl;
    m_fileMaxCount = fMaxCount;
    m_fileMaxLength = fMaxLen;
    m_fileCurCount = 0;
    m_dirPath = fDir;
    m_fileNamePrefix = fPrefix;

    std::cout<< "StoreLinux::configSet()" << std::endl;
}

CL_TYPE_t StoreLinux::dirRead( const std::shared_ptr<std::string> pDPath ){
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
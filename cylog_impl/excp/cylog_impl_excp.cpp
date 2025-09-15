#include "private_include/cylog_factory.hpp"
#include "cylog_impl_excp.hpp"

CL_TYPE_t CYLogExcpImpl::write(const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen) {
    // std::cout << "Excp write." << std::endl;
    storeGet()->itemWrite( m_fDesc, pIn, iLen );
    return CL_OK;
}

void CYLogExcpImpl::logInit() {
    /* 读取告警日志目录所有的文件，记录文件的数量，和当前可写文件及其位置 */
   std::cout << "CYLogExcpImpl::logInit()" << std::endl;
//    storeGet()->init();
}

CL_TYPE_t CYLogExcpImpl::traverse(log_read_cb_t cb) {
    if( cb==nullptr ) { goto excp; }

    while( 0 ) {
        if( cb(NULL, 0)<0 ) { break; }
    }

    return CL_OK;
excp:
    return CL_PARAM_INVALID;
}

CYLogExcpImpl::CYLogExcpImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store, std::shared_ptr<CLFile::FileDesc>&fDesc ):CYLogImplAbs( store, fDesc ) {
    std::cout << "CYLogExcpImpl instance created." << std::endl;
}

/******************************************************* Factory *********************************************************/

CYLogImplAbs* CyLogExcpFactory::create(std::shared_ptr<StoreAbs> &store, std::string logDir, std::string prefix, uint32_t  fileSize, uint8_t fileCount) {
    std::cout << "CyLogExcpFactory::create" << std::endl;
    std::shared_ptr<CLFile::FileDesc> pExcpFD = std::make_shared<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount);
    return new CYLogExcpImpl(logDir, store, pExcpFD);
}
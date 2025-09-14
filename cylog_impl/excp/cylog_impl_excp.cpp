#include "private_include/cylog_factory.hpp"
#include "cylog_impl_excp.hpp"

CL_TYPE_t CYLogExcpImpl::write(const uint8_t* in, uint16_t iLen){
    // std::cout << "Excp write." << std::endl;
    storeGet()->itemWrite( in, iLen, m_fDesc );
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

CYLogExcpImpl::CYLogExcpImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store ):CYLogImplAbs( store ) {
    std::cout << "CYLogExcpImpl instance created." << std::endl;
    // storeGet()->configSet( EXCP_LOG_FILE_MAX_COUNT, EXCP_LOG_FILE_MAX_LEN, dir, EXCP_LOG_FILE_NAME_PREFIX );
}

/******************************************************* Factory *********************************************************/

CYLogImplAbs* CyLogExcpFactory::create(const std::string & logDir, std::shared_ptr<StoreAbs> &store ) {
    std::cout << "CyLogExcpFactory::create" << std::endl;
    return new CYLogExcpImpl(logDir, store);
}
#include "private_include/cylog_factory.hpp"
#include "cylog_impl_excp.hpp"

CL_TYPE_t CYLogImplExcp::write(const uint8_t* in, uint16_t iLen){
    // std::cout << "Excp write." << std::endl;
    storeGet()->itemWrite( in, iLen );
    return CL_OK;
}

CL_TYPE_t CYLogImplExcp::create() {
    std::cout << "Excp create." << std::endl;
    return CL_OK;
}

void CYLogImplExcp::logInit() {
    /* 读取告警日志目录所有的文件，记录文件的数量，和当前可写文件及其位置 */
   std::cout << "CYLogImplExcp::logInit()" << std::endl;
   storeGet()->init();
}

CYLogImplExcp::CYLogImplExcp(const std::string & dir, std::shared_ptr<StoreAbs> &store ):
                                                                CYLogImplAbs( store ) {
    std::cout << "CYLogImplExcp instance created." << std::endl;
    storeGet()->configSet( EXCP_LOG_FILE_MAX_COUNT, EXCP_LOG_FILE_MAX_LEN, dir, EXCP_LOG_FILE_NAME_PREFIX );
}

CYLogImplAbs* CyLogFactoryExcp::createLog(const std::string & logDir, std::shared_ptr<StoreAbs> &store ) {
    std::cout << "CyLogFactoryExcp::createLog" << std::endl;
    return new CYLogImplExcp(logDir, store);
}


CL_TYPE_t CYLogImplExcp::traverse(log_read_cb_t cb) {
    if( cb==nullptr ) { goto excp; }

    while( 0 ) {
        if( cb(NULL, 0)<0 ) { break; }
    }

    return CL_OK;
excp:
    return CL_PARAM_INVALID;
}

#if 0

CL_TYPE_t CYLogImplExcp::remove(const std::string &path){
    std::cout << "Excp remove." << std::endl;
    return CL_OK;
}

CL_TYPE_t CYLogImplExcp::listGet(){
    return CL_OK;
}

CL_TYPE_t CYLogImplExcp::read(const std::string &path, void* out ) {
    std::cout << "Excp read." << std::endl;
    // storeGet()->dirRead();
    return CL_OK; 
};

#endif
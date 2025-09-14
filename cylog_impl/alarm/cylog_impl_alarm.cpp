#include "private_include/cylog_factory.hpp"
#include "cylog_impl_alarm.hpp"

CL_TYPE_t CYLogAlarmImpl::write(const uint8_t* in, uint16_t iLen){
    // std::cout << "Alarm write." << std::endl;
    storeGet()->itemWrite( in, iLen, m_fDesc);
    return CL_OK;
}

void CYLogAlarmImpl::logInit() {
    /* 读取告警日志目录所有的文件，记录文件的数量，和当前可写文件及其位置 */
   std::cout << "CYLogAlarmImpl::logInit()" << std::endl;
//    storeGet()->init();
}

CL_TYPE_t CYLogAlarmImpl::traverse(log_read_cb_t cb=nullptr) {
    if( cb==nullptr ) { goto excp; }

    while( 0 ) {
        if( cb(NULL, 0)<0 ) { break; }
    }

    return CL_OK;
excp:
    return CL_PARAM_INVALID;
}

CYLogAlarmImpl::CYLogAlarmImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store, std::shared_ptr<CLFile::FileDesc>&fDesc ):CYLogImplAbs( store, fDesc ) {
    std::cout << "CYLogAlarmImpl instance created." << std::endl;
}

/*************************************************** Factory ******************************************************/

CYLogImplAbs* CyLogAlarmFactory::create(std::shared_ptr<StoreAbs> &store, std::string logDir, std::string prefix, uint32_t  fileSize, uint8_t fileCount) {
    std::cout << "CyLogAlarmFactory::create" << std::endl;
    std::shared_ptr<CLFile::FileDesc> pAlarmFD = std::make_shared<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount);
    return new CYLogAlarmImpl(logDir, store, pAlarmFD);
}

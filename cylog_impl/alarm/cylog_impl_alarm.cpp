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

CYLogAlarmImpl::CYLogAlarmImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store ):CYLogImplAbs( store ) {
    std::cout << "CYLogAlarmImpl instance created." << std::endl;
    // storeGet()->configSet( ALARM_LOG_FILE_MAX_COUNT, ALARM_LOG_FILE_MAX_LEN, dir, ALARM_LOG_FILE_NAME_PREFIX );
}

/*************************************************** Factory ******************************************************/

CYLogImplAbs* CyLogAlarmFactory::create(const std::string & logDir, std::shared_ptr<StoreAbs> &store ) {
    std::cout << "CyLogAlarmFactory::create" << std::endl;
    return new CYLogAlarmImpl(logDir, store);
}

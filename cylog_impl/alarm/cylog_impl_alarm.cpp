#include "private_include/cylog_factory.hpp"
#include "cylog_impl_alarm.hpp"

CL_TYPE_t CYLogImplAlarm::write(const uint8_t* in, uint16_t iLen){
    // std::cout << "Alarm write." << std::endl;
    storeGet()->itemWrite( in, iLen );
    return CL_OK;
}

CL_TYPE_t CYLogImplAlarm::create() {
    std::cout << "Alarm create." << std::endl;
    return CL_OK;
}

void CYLogImplAlarm::logInit() {
    /* 读取告警日志目录所有的文件，记录文件的数量，和当前可写文件及其位置 */
   std::cout << "CYLogImplAlarm::logInit()" << std::endl;
   storeGet()->init();
}

CYLogImplAlarm::CYLogImplAlarm(const std::string & dir, std::shared_ptr<StoreAbs> &store ):
                                                                CYLogImplAbs( store ) {
    std::cout << "CYLogImplAlarm instance created." << std::endl;
    storeGet()->configSet( ALARM_LOG_FILE_MAX_COUNT, ALARM_LOG_FILE_MAX_LEN, dir, ALARM_LOG_FILE_NAME_PREFIX );
}

CYLogImplAbs* CyLogFactoryAlarm::createLog(const std::string & logDir, std::shared_ptr<StoreAbs> &store ) {
    std::cout << "CyLogFactoryAlarm::createLog" << std::endl;
    return new CYLogImplAlarm(logDir, store);
}

CL_TYPE_t CYLogImplAlarm::traverse(log_read_cb_t cb=nullptr) {
    if( cb==nullptr ) { goto excp; }

    while( 0 ) {
        if( cb(NULL, 0)<0 ) { break; }
    }

    return CL_OK;
excp:
    return CL_PARAM_INVALID;
}

#if 0

CL_TYPE_t CYLogImplAlarm::remove(const std::string &path){
    std::cout << "Alarm remove." << std::endl;
    return CL_OK;
}

CL_TYPE_t CYLogImplAlarm::listGet(){
    return CL_OK;
}

CL_TYPE_t CYLogImplAlarm::read(const std::string &path, void* out ) {
    std::cout << "Alarm read." << std::endl;
    // storeGet()->dirRead();
    return CL_OK; 
};

#endif
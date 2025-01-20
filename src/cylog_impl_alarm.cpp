#include "private_include/cylog_factory.hpp"
#include "private_include/cylog_impl_alarm.hpp"

CL_TYPE_t CYLogImplAlarm::read(const std::string &path, void* out ) {
    std::cout << "Alarm read." << std::endl;
    return CL_OK; 
};

CL_TYPE_t CYLogImplAlarm::write(const std::string &path, const void* in){
    std::cout << "Alarm write." << std::endl;
    return CL_OK;
}

CL_TYPE_t CYLogImplAlarm::remove(const std::string &path){
    std::cout << "Alarm remove." << std::endl;
    return CL_OK;
}

CL_TYPE_t CYLogImplAlarm::create() {
    std::cout << "Alarm create." << std::endl;
    return CL_OK;
}

CL_TYPE_t CYLogImplAlarm::listGet(){
    return CL_OK;
}

void CYLogImplAlarm::dirInit() {
    /* 读取告警日志目录所有的文件，记录文件的数量，和当前可写文件及其位置 */
    /** 
    fStore.traversal(m_Path, list) //遍历目录
    for( f in list ) {
        f.size() 文件大小
        f.read(sizeof(HEAD)) //解析头部
        f.offset    // 解析写操作的偏移位置
        if( f.availableSize ) // 文件可用大小与预设阈值比较，判别当前文件是否可以继续写入     
    }
    */
}

// CYLogImplAlarm::CYLogImplAlarm(const std::string & dir ):CYLogImplAbs( dir ) {
CYLogImplAlarm::CYLogImplAlarm(const std::string & dir, std::shared_ptr<StoreAbs> &store ):
                                                                CYLogImplAbs( dir, store ) {
    std::cout << "CYLogImplAlarm instance created." << std::endl;

    this->m_FileMaxCount = ALARM_LOG_FILE_MAX_COUNT;
    this->m_FileMaxLength = ALARM_LOG_FILE_MAX_LEN;
    this->m_FilePrefix = "ALARM_LOG_FILE_NAME_PREFIX";
}

CYLogImplAbs* CyLogFactoryAlarm::createLog(const std::string & logDir, std::shared_ptr<StoreAbs> &store ) {
    std::cout << "CyLogFactoryAlarm::createLog" << std::endl;
    return new CYLogImplAlarm(logDir, store);
}

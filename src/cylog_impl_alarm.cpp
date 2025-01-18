#include "private_include/cylog_factory.hpp"
#include "private_include/cylog_impl_alarm.hpp"


CL_TYPE_t CYLogImplAlarm::read(const std::string &path, FileContent &out ) {
    std::cout << "Alarm read." << std::endl;
    return CL_OK; 
};

CL_TYPE_t CYLogImplAlarm::write(const std::string &path, const FileContent &in){
        std::cout << "Alarm write." << std::endl;
        return CL_OK;
    }

CL_TYPE_t CYLogImplAlarm::remove(const std::string &path){
    std::cout << "Alarm remove." << std::endl;
    return CL_OK;
}

CYLogImplAbs* CyLogFactoryAlarm::createLog(const std::string & logDir ) {
    std::cout << "CyLogFactoryAlarm::createLog" << std::endl;
    return new CYLogImplAlarm(logDir);
}
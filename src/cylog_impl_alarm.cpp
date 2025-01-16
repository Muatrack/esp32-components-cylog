#include "private_include/cylog_impl_alarm.hpp"


CL_TYPE_t CYLogImplAlarm::CYLogImplAbs::read(const std::string &path, FileContent &out ) { 
    return CL_OK; 
};

CL_TYPE_t CYLogImplAlarm::CYLogImplAbs::write(const std::string &path, const FileContent &in){
    std::cout << "Alarm write." << std::endl;
    return CL_OK;
}

CL_TYPE_t CYLogImplAlarm::CYLogImplAbs::remove(const std::string &path){
    std::cout << "Alarm remove." << std::endl;
    return CL_OK;
}


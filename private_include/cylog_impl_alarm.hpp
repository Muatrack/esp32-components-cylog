#pragma once

#include "common.hpp"
#include "cylog_impl.hpp"

/** 
 * 日志类别-告警日志
 * - 
 */
class CYLogImplAlarm : public CYLogImplAbs {

public:

    CYLogImplAlarm(const std::string & dir, const std::string& fPrefix, uint16_t maxFileCount, uint16_t maxFileLen)
                                                                :CYLogImplAbs(dir,fPrefix,maxFileCount,maxFileLen){

        std::cout << "CYLogImplAlarm instance created." << std::endl;
    }
    
    CL_TYPE_t read(const std::string &path, FileContent &out ) override;
    CL_TYPE_t write(const std::string &path, const FileContent &in) override;
    CL_TYPE_t remove(const std::string &path) override;

private:
    
};
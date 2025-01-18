#pragma once

#include "common.hpp"
#include "cylog_impl.hpp"

/* 告警日志文件数量的上限 */
#define ALARM_LOG_FILE_MAX_COUNT    8
/* 单个告警日志文件的大小(包含文件头) */
#define ALARM_LOG_FILE_MAX_LEN      1024
/* 文件名称前缀 */
#define ALARM_LOG_FILE_NAME_PREFIX  "alarm"

/** 
 * 日志类别-告警日志
 * - 
 */
class CYLogImplAlarm : public CYLogImplAbs {

public:

    CYLogImplAlarm(const std::string & dir ):CYLogImplAbs( dir ) {
        std::cout << "CYLogImplAlarm instance created." << std::endl;

        this->m_FileMaxCount = ALARM_LOG_FILE_MAX_COUNT;
        this->m_FileMaxLength = ALARM_LOG_FILE_MAX_LEN;
        this->m_FilePrefix = "ALARM_LOG_FILE_NAME_PREFIX";
    }

    CL_TYPE_t read(const std::string &path, FileContent &out ) override;
    CL_TYPE_t write(const std::string &path, const FileContent &in) override;
    CL_TYPE_t remove(const std::string &path) override;
    
private:
    
};

class CyLogFactoryAlarm : public CYLogFactoryAbs {
public:
    CYLogImplAbs* createLog(const std::string & logDir ) override;
};
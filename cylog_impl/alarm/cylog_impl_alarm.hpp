#pragma once

#include "private_include/common.hpp"
#include "private_include/cylog_impl.hpp"


/* 告警日志文件数量的上限 */
#define ALARM_LOG_FILE_MAX_COUNT    8
/* 单个告警日志文件的大小(包含文件头) */
#define ALARM_LOG_FILE_MAX_LEN      (1024*4)
/* 文件名称前缀 */
#define ALARM_LOG_FILE_NAME_PREFIX  "alarm"

/** 
 * 日志类别-告警日志
 * - 
 */
class CYLogAlarmImpl : public CYLogImplAbs {
public:
    
    CL_TYPE_t traverse(cylog_traversal_cb_t cb) override;

    CYLogAlarmImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc );
    ~CYLogAlarmImpl(){
        CYLOG_PRINT( std::cout<<"[ TESTCASE_CYLOG ] " << "~CYLogAlarmImpl()" << std::endl );
    };

    // CL_TYPE_t read(const std::string &path, void* out ) override;
    // CL_TYPE_t remove(const std::string &path) override;
    // CL_TYPE_t listGet() override;
private:
};

class CyLogAlarmFactory : public CYLogFactoryAbs {
public:
    /** 
         * @param logDir 存储日志的相对路径
         * @param store  存储对象
     */
    CYLogImplAbs* create(std::shared_ptr<StoreAbs> &store, std::string logDir, uint32_t  fileSize, uint8_t fileCount, std::string prefix, cylog_traversal_cb_t cb, cylog_traversal_filter_t filter) override;
    CYLogImplAbs *create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount, cylog_traversal_cb_t cb, cylog_traversal_filter_t filter) override;
    CYLogImplAbs *create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount) override;
};
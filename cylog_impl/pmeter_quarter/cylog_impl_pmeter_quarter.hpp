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
class CYLogPMeterQuarterImpl : public CYLogImplAbs {
public:
    
    CL_TYPE_t traverse(cylog_traversal_cb_t cb) override;

    CYLogPMeterQuarterImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc );
    ~CYLogPMeterQuarterImpl(){
        std::cout << "~CYLogPMeterQuarterImpl()" << std::endl;
    };
private:
};

class CyLogPMeterQuarterFactory : public CYLogFactoryAbs {
public:
    /** 
         * @param logDir 存储日志的相对路径
         * @param store  存储对象
     */
    CYLogImplAbs* create(std::shared_ptr<StoreAbs> &store, std::string logDir, uint32_t  fileSize, uint8_t fileCount, std::string prefix) override;
    CYLogImplAbs *create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount) override;
};
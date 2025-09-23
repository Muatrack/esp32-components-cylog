#pragma once

#include "private_include/common.hpp"
#include "private_include/cylog_impl.hpp"


/* 告警日志文件数量的上限 */
#define EXCP_LOG_FILE_MAX_COUNT    8
/* 单个告警日志文件的大小(包含文件头) */
#define EXCP_LOG_FILE_MAX_LEN      (1024*4)
/* 文件名称前缀 */
#define EXCP_LOG_FILE_NAME_PREFIX  "excp"

/** 
 * 日志类别-告警日志
 * - 
 */
class CYLogPowerImpl : public CYLogImplAbs {
public:

    CL_TYPE_t traverse(cylog_traversal_cb_t cb) override;

    CYLogPowerImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc );
    ~CYLogPowerImpl(){
        CYLOG_PRINT(  std::cout << "~CYLogPowerImpl()" << std::endl );
    };

private:
};

class CyLogPowerFactory : public CYLogFactoryAbs {
public:
    CYLogImplAbs* create(std::shared_ptr<StoreAbs> &store, std::string logDir, uint32_t  fileSize, uint8_t fileCount, std::string prefix, cylog_traversal_cb_t cb, cylog_traversal_filter_t filter) override;
    CYLogImplAbs *create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount) override;
};
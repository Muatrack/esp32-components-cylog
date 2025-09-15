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

    // void logInit() override;

    // CL_TYPE_t create() override;

    /**
     * 写数据到日志文件
     * @param in 待写数据的地址
     * @param iLen 待写数据的共计长度
     */
    CL_TYPE_t write(const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen) override;    
    
    CL_TYPE_t traverse(log_read_cb_t cb) override;

    CYLogAlarmImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc );
    ~CYLogAlarmImpl(){
        std::cout << "~CYLogAlarmImpl()" << std::endl;
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
    CYLogImplAbs* create(std::shared_ptr<StoreAbs> &store, std::string logDir, std::string prefix, uint32_t  fileSize, uint8_t fileCount) override;
};
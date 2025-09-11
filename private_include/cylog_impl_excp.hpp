#pragma once

#include "common.hpp"
#include "cylog_impl.hpp"


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
class CYLogImplExcp : public CYLogImplAbs {
public:
    CL_TYPE_t create() override;

    /**
     * 写数据到日志文件
     * @param in 待写数据的地址
     * @param iLen 待写数据的共计长度
     */
    CL_TYPE_t write(const uint8_t* in, uint16_t iLen) override;    

    void logInit() override;

    CYLogImplExcp(const std::string & dir, std::shared_ptr<StoreAbs> &store );
    ~CYLogImplExcp(){
        std::cout << "~CYLogImplExcp()" << std::endl;
    };

    // CL_TYPE_t read(const std::string &path, void* out ) override;
    // CL_TYPE_t remove(const std::string &path) override;
    // CL_TYPE_t listGet() override;
private:
};

class CyLogFactoryExcp : public CYLogFactoryAbs {
public:
    CYLogImplAbs* createLog(const std::string & logDir, std::shared_ptr<StoreAbs> &store ) override;
};
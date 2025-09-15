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
class CYLogExcpImpl : public CYLogImplAbs {
public:
    
    // void logInit() override;

    /**
     * 写数据到日志文件
     * @param in 待写数据的地址
     * @param iLen 待写数据的共计长度
     */
    CL_TYPE_t write(const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen) override;        

    CL_TYPE_t traverse(log_read_cb_t cb) override;

    CYLogExcpImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store, std::shared_ptr<CLFile::FileDesc>&fDesc );
    ~CYLogExcpImpl(){
        std::cout << "~CYLogExcpImpl()" << std::endl;
    };

private:
};

class CyLogExcpFactory : public CYLogFactoryAbs {
public:
    CYLogImplAbs* create(std::shared_ptr<StoreAbs> &store, std::string logDir, std::string prefix, uint32_t  fileSize, uint8_t fileCount) override;
};
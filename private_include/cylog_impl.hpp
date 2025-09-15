#pragma once

#include <iostream>
#include <stdbool.h>
#include <string>
#include <memory>

#include "errno.hpp"
#include "common.hpp"
#include "cylog_file.hpp"
#include "cylog_store_abs.hpp"
#include "cylog_c_api.hpp"

////////////////////////////////////////////////////

/////////////////////////////////////////////////////

/** 日志抽象类 */
class CYLogImplAbs {

public:

    CYLogImplAbs( std::shared_ptr<StoreAbs> &store, std::shared_ptr<CLFile::FileDesc>&fDesc ):m_Store(store){};

    /** 日志目录初始化 */
    virtual void logInit() = 0;
    
    /* 遍历日志 */
    virtual CL_TYPE_t traverse( log_read_cb_t cb) { return CL_OK; };
    
    /** 写日志到文件 */
    virtual CL_TYPE_t write(std::unique_ptr<uint8_t[]> pIn, uint16_t iLen) = 0;
    
    /** 读取日志文件 */
    // virtual CL_TYPE_t read( const std::string &path, void* out ) = 0;

    /** 删除日志文件 */
    // virtual CL_TYPE_t remove( const std::string &path) { return CL_OK; };

    virtual ~CYLogImplAbs(){};

    std::shared_ptr<StoreAbs> storeGet() { return m_Store; }

protected:
    CLFile::FileDesc m_fDesc;
private:
    std::shared_ptr<StoreAbs> m_Store;
};


/** 
 * 日志类别-波形记录
 * - 内容待议（数据突发， 需要内存配合）
 */

/** 
 * 日志类别-统计运行 
 * - 空调机开关记录
 * - 压缩机启停记录
 */

/** 
 * 日志类别-计量数据
 * - 15分钟写一条（单个采集器）。当有多个采集器时，每15分钟写多条
 */

/** 
 * 日志类别-系统异常
 * - 通信异常
 * - 数据异常
 */

#pragma once

#include <iostream>
#include <stdbool.h>
#include <string>

#include "errno.hpp"

#ifdef USE_SYSTEM_LINUX
    // #include <unistd.h>
    #include <cstdint>
#else
    #include <stdint.h>
#endif

/* 日志文件头类 */
class FileHead {
    public:
    uint16_t headVer;       // 头部结构的版本 0-65535
    uint32_t maxLen;        // 文件的最大长度(含头部长度)
    uint32_t offset;        // 文件的写偏移位置
};

/** 日志接口类 */
template<class T_Data>
class LogIf {
    public:
    /** 
     * 读日志 
     * @param path 日志文件的路径
     * @param pIn  待输入数据的内存地址
    */
    virtual CL_TYPE_t read(const std::string &path, T_Data &out ) = 0;
    /** 
     * 写日志 
     * @param path 日志文件的路径
     * @param pIn  待输入数据的内存地址
    */
    virtual CL_TYPE_t write(const std::string &path, const T_Data &in) = 0;
    /* 删除日志文件 */
    virtual CL_TYPE_t remove(const std::string &path) = 0;
};


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

////////////////////////////////////////////////////

/* 日志文件-头类 */
class FileHead {
    public:
    uint16_t headVer;       // 头部结构的版本 0-65535
    uint32_t maxLen;        // 文件的最大长度(含头部长度)
    uint32_t offset;        // 文件的写偏移位置
};

/* 日志文件-内容类*/
class FileContent {
};

/////////////////////////////////////////////////////

/** 日志接口类 */
class LogIf {
    public:
    /** 
     * 读日志 
     * @param path 日志文件的路径
     * @param pIn  待输入数据的内存地址
    */
    virtual CL_TYPE_t read(const std::string &path, FileContent &out ) = 0;
    /** 
     * 写日志 
     * @param path 日志文件的路径
     * @param pIn  待输入数据的内存地址
    */
    virtual CL_TYPE_t write(const std::string &path, const FileContent &in) = 0;
    /* 删除日志文件 */
    virtual CL_TYPE_t remove(const std::string &path) = 0;
};

/** 日志抽象类 */
class LogGeneric : public LogIf {

public:
    LogGeneric(const std::string&dirPath, const std::string&filePrefix,uint8_t fileMaxCount,uint32_t fileMaxLength):
    mDirPath(dirPath),mFilePrefix(filePrefix),mFileMaxCount(fileMaxCount),mFileMaxLength(fileMaxLength){}

    CL_TYPE_t read(const std::string &path, FileContent &out );
    CL_TYPE_t write(const std::string &path, const FileContent &in);
    CL_TYPE_t remove(const std::string &path);
protected:
    std::string mDirPath;        // 日志所属类别的目录
    std::string mFilePrefix;     // 日志文件名称的前缀
    uint8_t     mFileMaxCount;   // 所属类别日志文件的数量最大值
    uint32_t    mFileMaxLength;  // 所属类别日志的单文件大小的上限 <字节>
};


/** 日志类别-告警日志 */
class AlarmLog : public LogGeneric {
public:
    AlarmLog(const std::string & dir, const std::string& fPrefix, uint16_t maxFileCount, uint16_t maxFileLen)
    :LogGeneric(dir,fPrefix,maxFileCount,maxFileLen){};    
private:
};

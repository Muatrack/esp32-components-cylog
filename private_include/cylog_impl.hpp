#pragma once

#include <iostream>
#include <stdbool.h>
#include <string>
#include "errno.hpp"
#include "common.hpp"
#include "cylog_file.hpp"

////////////////////////////////////////////////////

/////////////////////////////////////////////////////

/** 日志抽象类 */
class CYLogImplAbs {

public:
    CYLogImplAbs(const std::string&dirPath, const std::string&filePrefix,uint8_t fileMaxCount,uint32_t fileMaxLength):
    mDirPath(dirPath),mFilePrefix(filePrefix),mFileMaxCount(fileMaxCount),mFileMaxLength(fileMaxLength){}

    virtual CL_TYPE_t read(const std::string &path, FileContent &out );
    virtual CL_TYPE_t write(const std::string &path, const FileContent &in);
    virtual CL_TYPE_t remove(const std::string &path);
    virtual ~CYLogImplAbs(){};
protected:
    std::string mDirPath;        // 日志所属类别的目录
    std::string mFilePrefix;     // 日志文件名称的前缀
    uint8_t     mFileMaxCount;   // 所属类别日志文件的数量最大值
    uint32_t    mFileMaxLength;  // 所属类别日志的单文件大小的上限 <字节>
};


/** 
 * 日志类别-告警日志
 * - 
 */
class AlarmLog : public CYLogImplAbs {
public:
    AlarmLog(const std::string & dir, const std::string& fPrefix, uint16_t maxFileCount, uint16_t maxFileLen)
    :CYLogImplAbs(dir,fPrefix,maxFileCount,maxFileLen){};
private:
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

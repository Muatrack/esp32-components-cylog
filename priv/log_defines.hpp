#pragma once

#include <iostream>
#include <string>

#if defined(USE_SYSTEM_FREERTOS)
    #include <unistd.h>
#else
    /** for linux */
    #include <stdint.h>
#endif

namespace log_generic {

using namespace std;

class FileHead {
public:
    uint16_t headVer;       // 头部结构的版本 0-65535
    uint32_t maxLen;        // 文件的最大长度(含头部长度)
    uint32_t offset;        // 文件的写偏移位置
};

class LogFile {
private:
    string name;
    string path;
    FileHead fHead;

    virtual void read() {
        cout <<"[ LogFile ] read.. " << endl;
    };

    virtual void write() {
        cout <<"[ LogFile ] write.. " << endl;
    };
};

class LogGeneric: LogFile {
private:
    LogFile file;
public:
    virtual void path_set(string path) = 0;
};

};
// namespace log_generic;
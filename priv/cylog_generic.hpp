#pragma once

#include <iostream>
#include <string>

#if defined(USE_SYSTEM_FREERTOS)
    #include <unistd.h>
    #error "dddddddddddd freertos"
#else
    /** for linux */
    #include <stdint.h>
#endif

namespace cylog_generic {

using namespace std;

class FileHead {
public:
    uint16_t headVer;       // 头部结构的版本 0-65535
    uint32_t maxLen;        // 文件的最大长度(含头部长度)
    uint32_t offset;        // 文件的写偏移位置
};

class CYLogFile {
private:
    string name;
    string path;
    FileHead fHead;
};

class CYLogGeneric: CYLogFile {
private:
    CYLogFile file;
public:
    virtual void path_set(string path) = 0;
};

};
// namespace cylog;
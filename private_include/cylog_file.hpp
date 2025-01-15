
#include "common.hpp"

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

#include "common.hpp"

namespace CLFile {

enum class FileVer:uint8_t {
};

class CYLogFile{
public:
    virtual ~CYLogFile() = 0;

    virtual void serialize() = 0;
};

/* 日志文件-头类 */
class FileHead : public CYLogFile {
public:
    FileHead() = delete;
    FileHead(FileHead&fh) = delete;
    FileHead& operator=(FileHead&fh) = delete;

    FileHead(uint8_t ver, uint32_t maxFileLen):m_Ver(ver),m_MaxFLen(maxFileLen) {
        this->m_WOffset = 0;
    };

    /* 对象序列化 */
    void serialize(){
        std::cout << "File head serialization ." << std::endl;
    };

private:
    uint8_t  m_Ver;            // 头部结构的版本 0-65535
    uint32_t m_MaxFLen;        // 文件的最大长度(含头部长度)
    uint32_t m_WOffset;        // 文件的写偏移位置
};

/* 日志文件-item头*/
class ItemHead : public CYLogFile {
public:
    ItemHead() = delete;
    ItemHead(ItemHead&fh) = delete;
    ItemHead& operator=(ItemHead&fh) = delete;

    /* 对象序列化 */
    void serialize(){
        std::cout << "File head serialization ." << std::endl;
    };
private:
    uint8_t  m_ItemFlag;
    uint16_t m_DataLen;
};

/* 日志文件-内容类*/
class ItemContent {
public:
private:
    void *m_DataPtr;
    uint32_t m_DataLen;
};

}
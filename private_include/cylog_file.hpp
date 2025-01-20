
#include "common.hpp"
#include <vector>

namespace CLFile {

#define FITEM_VALID_FLAG    0x55

enum class FileState:uint8_t {
    FST_EMPTY,      //新文件,空
    FST_USING,      //文件使用中
    FST_FULL,       //文件已满
};

class FileAbs{
public:
    virtual ~FileAbs() = 0;

    virtual void serialize() = 0;
};

/* 日志文件-头类 */
class FileHead : public FileAbs {
public:
    FileHead() = delete;
    FileHead(FileHead&fh) = delete;
    FileHead& operator=(FileHead&fh) = delete;

    FileHead(uint8_t ver, uint32_t maxFileLen) : m_FSt(FileState::FST_EMPTY),m_Ver(ver),m_MaxFLen(maxFileLen) {};

    /* 对象序列化 */
    void serialize(){
        std::cout << "File head serialization ." << std::endl;
    };

private:
    FileState   m_FSt;            // 日志文件当前状态
    uint8_t     m_Ver;            // 头部结构的版本 0-65535    
    uint32_t    m_MaxFLen;        // 文件的最大长度(含头部长度)
};

/* 日志文件-item头*/
class ItemHead : public FileAbs {
public:
    ItemHead():m_validFlag(0),m_dataLen(0){}
    ItemHead(ItemHead&fh) = delete;
    ItemHead& operator=(ItemHead&fh) = delete;

    /* 对象序列化 */
    void serialize(){
        std::cout << "File head serialization ." << std::endl;
    };

private:
    uint8_t  m_validFlag;            //文件中item数据的标记, FITEM_VALID_FLAG-有效/其他-无效
    uint16_t m_dataLen;             //item数据长度
    std::vector<uint8_t> m_data;    //数据集合
};

/* 日志文件-内容类*/
class ItemContent {
public:
private:
    void *m_DataPtr;
    uint32_t m_DataLen;
};

}
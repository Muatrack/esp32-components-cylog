
#include "common.hpp"
#include <vector>
#include <iostream>

namespace CLFile {

#define FITEM_VALID_FLAG    0x55

#ifdef CYLOG_FILE_VER
    #define FILE_VERSION        CYLOG_FILE_VER
#else
    #define FILE_VERSION        0
#endif

enum class FileState:uint8_t {
    FST_EMPTY,      //新文件,空
    FST_USING,      //文件使用中
    FST_FULL,       //文件已满
};

class FileAbs{
public:
    virtual ~FileAbs() {};

    // virtual bool   serialize(unsigned char *pByteBuf) = 0;
    // virtual bool deserialize(unsigned char *pByteBuf) = 0;

    virtual const unsigned char * serialize() { return nullptr ;};
};

/* 日志文件-头类 */
class FileHead : public FileAbs {
public:
    FileHead() = delete;
    FileHead(FileHead&fh) = delete;
    FileHead& operator=(FileHead&fh) = delete;
    FileHead(uint32_t maxFileLen) : m_Ver(FILE_VERSION),m_FMaxLen(maxFileLen),m_ReWriteTm(0) {};

    /**
     * 对象反序列化，通过序列数据实例化
     * @param pBytes 带序列化的字节序列
     */
    FileHead(const unsigned char * pBytes) {
        m_Ver = pBytes[0];
        m_FMaxLen = pBytes[1] | (pBytes[2] << 8) | (pBytes[3] << 16) | (pBytes[4] << 24);
        m_ReWriteTm = pBytes[5] | (pBytes[6] << 8) | (pBytes[7] << 16) | (pBytes[8] << 24);
    };

    ~FileHead() {};

    uint8_t  verGet() { return m_Ver; };
    uint32_t maxLenGet() { return m_FMaxLen; };
    uint32_t reWriteTmGet() { return m_ReWriteTm; };

    /* 对象序列化, 将实例数据转化为字节序列 */
    const unsigned char * serialize() override {
        *(uint32_t*) &m_Bytes[0] = sizeof(m_Bytes);
        m_Bytes[4] = m_Ver;
        m_Bytes[5] = m_FMaxLen & 0xFF;
        m_Bytes[6] = (m_FMaxLen >> 8) & 0xFF;
        m_Bytes[7] = (m_FMaxLen >> 16) & 0xFF;
        m_Bytes[8] = (m_FMaxLen >> 24) & 0xFF;
        m_Bytes[9] = m_ReWriteTm & 0xFF;
        m_Bytes[10] = (m_ReWriteTm >> 8) & 0xFF;
        m_Bytes[11] = (m_ReWriteTm >> 16) & 0xFF;
        m_Bytes[12] = (m_ReWriteTm >> 24) & 0xFF;
        return m_Bytes;
    };

    // bool deserialize(unsigned char byteBuf[16]) override {
    //     return true;
    // };

private:
    unsigned char m_Bytes[sizeof(uint8_t)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t)] = {0};
    uint8_t     m_Ver;              // 头部结构的版本 0-127
    uint32_t    m_FMaxLen;          // 单文件的最大长度(含头部长度),也是新建时的预占大小
    uint32_t    m_ReWriteTm;        // 文件被重写时的时间戳(s)
};

/* 日志文件-item头*/
class ItemHead : public FileAbs {
public:
    ItemHead():m_validFlag(0),m_dataLen(0){}
    ItemHead(ItemHead&fh) = delete;
    ItemHead& operator=(ItemHead&fh) = delete;

    /* 对象序列化 */
    const unsigned char * serialize() override {
        return nullptr;
    };

    // bool   deserialize(unsigned char byteBuf[16]) override {
    //     std::cout << "File head serialization ." << std::endl;
    //     return true;
    // };

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
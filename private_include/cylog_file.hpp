
#include "common.hpp"
#include <vector>
#include <iostream>
#include "private_include/cylog_utils.hpp"
#include "time.h"

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

using Serial=CyLogUtils::Serializer;

class FileAbs{
public:
    virtual ~FileAbs() {};

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
        Serial::Deserialize<uint8_t>(&pBytes[0], sizeof(uint8_t), m_Ver);
        Serial::Deserialize<uint32_t>(&pBytes[1], sizeof(uint32_t), m_FMaxLen);
        Serial::Deserialize<uint32_t>(&pBytes[5], sizeof(uint32_t), m_ReWriteTm);
    };

    ~FileHead() {};

    uint8_t  verGet() { return m_Ver; };
    uint32_t maxLenGet() { return m_FMaxLen; };
    uint32_t reWriteTmGet() { return m_ReWriteTm; };

    /** 
     * 对象序列化, 将实例数据转化为字节序列 
     * @return 成功： 返回序列化后的字节序列， 其中低4字节( [0 ... 4]==byteCount )保存了有效字节的数量
    */
    const unsigned char * serialize() override {
        *(uint32_t*) &m_Bytes[0] = sizeof(m_Bytes);
        Serial::Serialize<uint8_t>(m_Ver, sizeof(m_Ver), &m_Bytes[4]);
        Serial::Serialize<uint32_t>(m_FMaxLen, sizeof(m_FMaxLen), &m_Bytes[5]);
        Serial::Serialize<uint32_t>(m_ReWriteTm=time(nullptr), sizeof(m_ReWriteTm), &m_Bytes[9]);
        return m_Bytes;
    };

    /** 
     * 获取文件头部占用的字节数
     * @return 返回文件中占用字节数
     */
    static uint32_t size() { return sizeof(m_Bytes); };

private:
    /* 字节空间长度组成 表示有效字节长度的数值(4字节) | mVer (1字节) | m_FMaxLen(4字节) | m_ReWriteTm(8字节) + mPreserved(24字节) */
    uint8_t     m_Bytes[ sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint64_t) + 24 ] = {0};
    uint8_t     m_Ver;              // 头部结构的版本 0-127
    uint32_t    m_FMaxLen;          // 单文件的最大长度(含头部长度),也是新建时的预占大小
    uint64_t    m_ReWriteTm;        // 文件被重写时的时间戳(s)
    uint8_t      _Preserved[24];        // 预留24字节
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
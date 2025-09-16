
#pragma once

#include "common.hpp"
#include <vector>
#include <iostream>
#include <cstring>
#include <filesystem>
#include <fstream>
#include "private_include/cylog_utils.hpp"
#include "time.h"

namespace CLFile {

#define ITEM_VALID_FLAG    0x55

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

/** 
 * 文件描述类
 * 文件名称的组成由文件名称前缀+ '_' + [00-FF] 组成
 */
class FileDesc {
public:
    FileDesc()=default;
    // FileDesc(FileDesc&)=default;

    /**      
     * logPath: 分类日志存储的相对路径
     * prefix: 文件名称的前缀
     * fSize: 单个文件的大小
     * fCount: 日志文件的总数
     */
    FileDesc(std::string logPath, std::string prefix, uint32_t fSize, uint8_t fCount):
                m_LogPath(logPath),m_Prefix(prefix), m_FSize(fSize), m_FCount(fCount){};
    ~FileDesc() {};

    /** 读取文件名称前缀 */
    std::string filePrefixGet() { return m_Prefix; };
    /** 读取文件相对路径 */
    std::string relativePathGet() { return m_LogPath; };
    /** 读取文件的数量 */
    uint8_t     fileCountGet() { return m_FCount; }
    /** 读取单个文件的大小 */
    uint32_t    fileSizeGet() { return m_FSize; };
    /** 读取当前可写文件的相对路径 */
    std::string wFilePathGet() { return m_WPath; };
    /** 读取当前可写文件的写入偏移量 */
    uint32_t    wFileOffsetGet() { return m_WOffset; };

    /* 设置分类日志，当前可写的文件相对路径 */
    void writableFilePathSet(std::string & path) { m_WPath = path; }
    /* 设置分类日志，当前可写文件中的写时偏移量*/
    void writableFileOffsetSet(uint32_t offset) { m_WOffset = offset; }

private:

    /*immutable member */
    std::string m_LogPath;      // 日志文件的相对路径
    std::string m_Prefix;       // 文件名称前缀
    uint32_t    m_FSize;        // 单个日志文件大小
    uint8_t     m_FCount;       // 日志文件的数量

    /* mutable member */
    std::string m_WPath;        // 当前可写文件文件相对路径
    uint32_t    m_WOffset;      // 当前可写文件的位置偏移量
};

/**
 * 文件中的数据抽象类
 */
class DataAbs{

public:
    virtual ~DataAbs() {};
    virtual const unsigned char * serialize(){ return nullptr; };
    virtual bool  deSerialize( uint8_t *pBuf = nullptr){ return false; };
protected:
    uint8_t                     m_Ver;          // 版本
    uint8_t                     m_ValidFlag;    // 数据是否有效标志
    uint16_t                    m_OriDlen;   // 输入数据的长度
    
    std::unique_ptr<uint8_t[]>  m_OriData;   // 输入数据的指针
};

class ItemDesc : public DataAbs {

public:
    ItemDesc() = delete;
    ItemDesc(ItemDesc&) = default;
    ItemDesc(std::unique_ptr<uint8_t[]> pData, uint16_t dLen, bool bWithHead=false) {

        if( bWithHead ) {   // 数据带头部，即数据为文件中读取到的数据
            parseHead( pData );
            m_OriData = std::make_unique<uint8_t[]>(dLen-4);
            std::copy(pData.get()+4, pData.get()+dLen, m_OriData.get());
        } else {    // 数据不带头部，即数据为新生成的日志
            makeHead( dLen );
            m_OriData = std::move(pData);
        }
    };

    bool isValid() { return (m_ValidFlag==ITEM_VALID_FLAG); };
    uint16_t itemSizeGet() { return m_OriDlen; };
    uint16_t itemVerGet() { return m_Ver; };
    std::unique_ptr<uint8_t[]> itemGet() { return std::move(m_OriData); };

private:

    /**
     * uint8_t[0] : ver
     * uint8_t[1] : valid flag
     * uint8_t[2] : data length
     */
    void parseHead(std::unique_ptr<uint8_t[]> & pData) {
        m_Ver       = pData[0];
        m_ValidFlag = pData[1];
        m_OriDlen   = *((uint16_t*)&pData[2]);
    }

    void makeHead( uint16_t dLen ){
        m_Ver       = FILE_VERSION;
        m_ValidFlag = ITEM_VALID_FLAG;
        m_OriDlen   = dLen;
    }

public:
    static std::unique_ptr<ItemDesc> itemSerialize(std::unique_ptr<uint8_t[]> pData, uint16_t dLen);    // 序列化    
    static std::unique_ptr<ItemDesc> itemDeSerialize(std::unique_ptr<uint8_t[]> pData, uint16_t dLen);  // 实例化
};

/******************************* @deprecated *********************************/
#if 0
/* 日志文件-头类 */
class FileHead : public DataAbs {
public:
    FileHead(){};
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
     * @return 成功： 返回序列化后的字节序列
    */
    const unsigned char * serialize() override {
        uint8_t zeroByte = 0xFF;
        Serial::Serialize<uint8_t>(m_Ver, sizeof(m_Ver), &m_Bytes[0]);
        Serial::Serialize<uint32_t>(m_FMaxLen, sizeof(m_FMaxLen), &m_Bytes[sizeof(uint8_t)]);
        Serial::Serialize<uint64_t>(m_ReWriteTm=time(nullptr), sizeof(m_ReWriteTm), &m_Bytes[sizeof(uint8_t)+sizeof(uint32_t)]);
        for( int i=0; i < 24; i ++ ) {
            (&m_Bytes[sizeof(uint8_t)+sizeof(uint32_t)+sizeof(uint64_t)])[i]=zeroByte;
        }
        return m_Bytes;
    };

    /** 
     * 反序列化字节序列为实例
     * @param pBytesBuf 传入的字节序列
     * @return  成功: true,  失败: false
     */
    bool  deSerialize( uint8_t *pBytesBuf = nullptr) {
        if(pBytesBuf == nullptr ) {
            pBytesBuf = m_Bytes;
        }

        Serial::Deserialize<uint8_t>(&pBytesBuf[0], sizeof(uint8_t), m_Ver);
        Serial::Deserialize<uint32_t>(&pBytesBuf[sizeof(uint8_t)], sizeof(uint32_t), m_FMaxLen);
        Serial::Deserialize<uint64_t>(&pBytesBuf[sizeof(uint8_t)+sizeof(uint32_t)], sizeof(uint64_t), m_ReWriteTm);
        return true;
    };

    /**
     * 反序列化字节序列为实例
     * 
     */
    bool deSerialize( std::filesystem::path & fPath ) {
        std::fstream _ff;
        if( _ff.open( fPath, std::ios::binary | std::ios::out | std::ios::in ), _ff.is_open() ) {
            _ff.seekp(0);
            _ff.read( (char*)bytesBufGet(), sizeGet() );
            _ff.close();
            deSerialize();
        }

        return true;
    }

    /** 
     * 获取文件头部占用的字节数
     * @return 返回文件中占用字节数
     */
    static uint32_t sizeGet() { return sizeof(m_Bytes); };

    /**
     * 获取头部数据结构字节缓冲区
     */
    uint8_t* bytesBufGet() {
        // 初始化字节序列
        memset( (void*)m_Bytes, 0, sizeof(m_Bytes));
        return m_Bytes;
    };

private:
    /* 字节空间长度组成 表示有效字节长度的数值(4字节) | mVer (1字节) | m_FMaxLen(4字节) | m_ReWriteTm(8字节) + mPreserved(24字节) */
    uint8_t     m_Bytes[ sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint64_t) + 24 ] = {0};
    uint8_t     m_Ver;              // 头部结构的版本 0-127
    uint32_t    m_FMaxLen;          // 单文件的最大长度(含头部长度),也是新建时的预占大小
    uint64_t    m_ReWriteTm;        // 文件被重写时的时间戳(s)
    uint8_t      _Preserved[24];        // 预留24字节
};

/* 日志文件-内容类*/
class FileItem :  public DataAbs  {
public:
    const uint8_t* serialize() override {
        return nullptr;
    }

    bool  deSerialize( uint8_t *pBuf = nullptr) override {
        return false;
    }

    /** 定位文件中可写的绝对offset */
    static uint32_t wOffsetGet( std::filesystem::path & fPath ) {
        std::fstream _ff;
        uint32_t _wOffset = 0;
        uint8_t  _itemBuf[8] = {0};
        uint16_t itemLen = 0;
        std::shared_ptr<FileHead> spFHead = std::make_shared<FileHead>();        

        if( _ff.open( fPath, std::ios::binary | std::ios::out | std::ios::in ), _ff.is_open() ) {
            // 跳过文件头部
            _wOffset = FileHead::sizeGet();
            _ff.seekp( _wOffset );

            // 遍历文件
            while( _wOffset < spFHead->maxLenGet() ) {
                _ff.read( (char*)&_itemBuf, sizeof(uint16_t) );
                CyLogUtils::Serializer::Deserialize<uint16_t>((const uint8_t *)_itemBuf, sizeof(itemLen), itemLen);
                // 读取到的item长度为0, 表示此处无item， 则当前写位置可用
                if( itemLen==0 ) {
                    break;
                }
                // 读取到的item长度不为0, 表示当前位置已写入有效item数据，继续增加读偏移量
                _wOffset += itemLen + sizeof(uint16_t);
                _ff.seekp( _wOffset );
            }
            _ff.close();
        }
        return _wOffset;
    }

private:
    void *m_DataPtr;
    uint32_t m_DataLen;
};

#endif
/******************************* @deprecated END *********************************/

} // namespace CLFile
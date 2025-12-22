#pragma once

#include "common.hpp"
#include <filesystem>
#include <private_include/cylog_file.hpp>
#include <semaphore.h>
#include <private_include/errno.hpp>

using namespace CLFile;

/** 记录文件的名称，大小，写入偏移量 */
class FileUsage {
public:
    bool        m_IsFull;   /* 判断文件是否已写满 */
    uint8_t     m_FId;      /* 文件名称追加的数字编号 */
    std::string m_Path;     /* 文件名称 */
    uint32_t    m_Size;     /* 文件大小 */
    uint32_t    m_WOfSet;   /* 可写偏移量*/
    uint32_t    m_FMTime;   /* 文件的修改时间 */

    FileUsage():m_IsFull(true),m_FId(0xFF), m_Path(""), m_Size(0), m_WOfSet(0),m_FMTime(0) {};
    // FileUsage(FileUsage&) = delete;
};

/**
 * 日志文件写入方案
 * 
 * 对于每个日志文件，其中的数据由 HEAD + DATA 组成
 * - HEAD：标识当前item是否有效，标识item的长度, valid: 1byte, len:2 bytes
 * - DATA: 即上层需要写入的数据，与日志类别相关
 */

/** 面对系统api, 读取写文件操作 */
class StoreAbs {
public:

    StoreAbs(){};

    virtual ~StoreAbs(){
        CYLOG_PRINT( std::cout<<"[ TESTCASE_CYLOG ] " << "~StoreAbs()" << std::endl );
    }

    /* 删除指定日志目录 */
    virtual CL_TYPE_t dirDelete( const std::string & absPath ) = 0;
    /* 新建指定目录, 如存在则不重复新建 */
    virtual CL_TYPE_t dirCreate( const std::string & absPath ) = 0;
    /** 
     * 建立全部日志文件
     * 
     * @param  absPath:  在此目录下新建文件
     * @param  prefix:   新见文件的前缀
     * @param  fCount:   新建文件的数量 
     * @param  fSize:    单文件的大小-预占方式
     */
    virtual CL_TYPE_t fileCreate( std::unique_ptr<FileDesc> & pFDesc ) = 0;
    /* 读取指定文件内容数据，指定偏移量、长度的数据*/
    virtual CL_TYPE_t itemWrite( std::unique_ptr<FileDesc> & pFDesc, const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen, uint32_t timeoutMs=10 ) = 0;
    /* 读取指定目录，所有文件名称 */
    virtual CL_TYPE_t dirRead( std::unique_ptr<FileDesc> & pFDesc ) = 0;
    /* 遍历目录，查找可写文件、可写偏移量 */
    virtual CL_TYPE_t dirTraverse( std::unique_ptr<FileDesc> & pFDesc, std::vector<std::string> & fList ) = 0;
    /* 遍历多文件，查找可写位置 */
    virtual CL_TYPE_t multiFilesTraverse(std::unique_ptr<FileDesc> & pFDesc, std::string &,  FileUsage & ) = 0;
    /* 遍历多文件，查找可写位置 */
    virtual CL_TYPE_t singleFileTraverse(std::unique_ptr<FileDesc> & pFDesc, std::string &,  FileUsage & );
    /**
    * 遍历内存块中数据，逐字节匹配有效数据
    * @param pFdesc: 
    * @param pData:
    * @param dLen:
    * @return <uint32_t>  已检查过的字节数
    */
    virtual uint32_t memBlockTraverse( std::unique_ptr<FileDesc> & pFDesc, FileUsage & fUsage, uint32_t absOffset, std::unique_ptr<uint8_t[]> & pData, uint32_t dLen );
    /* 判断文件/目录是否存在 */
    virtual bool doesExists( std::string & path );

    /* 读取指定文件内容数据，指定偏移量、长度的数据*/
    virtual CL_TYPE_t itemRead( uint16_t readSize, std::shared_ptr<uint8_t[]> &pOData, FileDesc &fDesc){
        return CL_OK;
    };

    /** 
     * 选择下一个文件，并初始化文件头部。
     */
    virtual void nextFileSelect(std::unique_ptr<FileDesc> & pFDesc);

    /**
     * 向存储发起读写请求前，获取操作锁
     * 
     * @param wait_tms 等待锁超时时间(ms), 缺省值10ms
     * @return 
     * 
     * - true: 加锁成功
     * 
     * - false: 加锁失败
     */
    bool lockTake(uint32_t wait_tms=10) {
        if((wait_tms<1)||(wait_tms>500)) { wait_tms = 10; }

        timespec wt = { 
            .tv_sec = (long int)(wait_tms/1000),
            .tv_nsec = 
        #ifdef USE_SYSTEM_FREERTOS
        (long uint32_t)
        #else
        (long int)
        #endif
            (wait_tms%1000)*1000000
        };
        return sem_timedwait(&m_signal, &wt)==0;
    }

    /* 获取存储日志的根目录 */
    std::string rootDirGet() { return m_LogRootDir; }

    /**
     * 对操作所解锁(增加信号量的值)
     */
    void lockGive() {
        sem_post(&m_signal);
    }

    /**
     * 判断文件是否已满
     * @param wOffset: 文件当前可识别的写偏移量
     * @param pFDesc:  分类日志文件的描述
     */
    virtual bool isFileFull( uint32_t wOffset, std::unique_ptr<FileDesc>& pFDesc ) {
        uint32_t remainSize = pFDesc->fileSizeGet()-wOffset;
        return (remainSize<pFDesc->tailHoleGet());
    };

    /** 
     * 过滤路径字串中多余的‘/’
     */
    std::string cleanPath(std::string oriStr) {
        return oriStr;
    }

public:
    /**
     * 初始化存储资源
     * 
     * @param concurrCount 读写并行操作数量
     * @param logDir 日志的存储路径
     */
    static void StoreInit(uint8_t concurCount, std::string & logRootDir);

private:
    /** 文件系统读写信号量 */
    static sem_t m_signal;
    /* 存储日志的根目录 */
    static std::string m_LogRootDir;    
};

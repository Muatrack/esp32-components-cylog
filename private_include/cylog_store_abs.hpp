#pragma once

#include "common.hpp"
#include <filesystem>
#include <private_include/cylog_file.hpp>
#include <semaphore.h>
#include <private_include/errno.hpp>

using namespace CLFile;

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
        std::cout << "~StoreAbs()" << std::endl;
    }

    /* 新建指定目录, 如存在则不重复新建 */
    virtual CL_TYPE_t dirCreate( const std::string & absPath );

    /* 新建指定目录，及所有新文件 */
    virtual CL_TYPE_t dirCreate_bak(){ return CL_EXCP_UNKNOW; } ;

    /* 读取指定目录，所有文件名称 */
    virtual CL_TYPE_t dirRead( std::unique_ptr<FileDesc> & pFDesc ) { return CL_OK; };

    /* 遍历目录，查找可写文件、可写偏移量 */
    virtual CL_TYPE_t dirTraverse( std::unique_ptr<FileDesc> & pFDesc, std::vector<std::string> & fList )=0;
    /* 遍历文件，查找可写位置 */
    virtual CL_TYPE_t fileTraverse( std::string & pFDesc,  std::unique_ptr<uint8_t[]> & buf, uint16_t bufSize )=0;

    /** 
     * 建立全部日志文件
     * 
     * @param  absPath:  在此目录下新建文件
     * @param  prefix:   新见文件的前缀
     * @param  fCount:   新建文件的数量 
     * @param  fSize:    单文件的大小-预占方式
     */
    CL_TYPE_t fileCreate( const std::string & absPath, const std::string prefix, uint8_t fCount, uint32_t fSize );

    /* 读取指定文件内容数据，指定偏移量、长度的数据*/
    virtual CL_TYPE_t itemRead( uint16_t readSize, std::shared_ptr<uint8_t[]> &pOData, FileDesc &fDesc){
        return CL_OK;
    };

    /* 读取指定文件内容数据，指定偏移量、长度的数据*/
    virtual CL_TYPE_t itemWrite( std::unique_ptr<FileDesc> & pFDesc, const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen);

    /** 
     * 文件目录初始化. 新建对象后首先执行此函数 
     * - 文件存在，遍历目录下所有文件
     * - 文件不存在，新建目录，新建全数日志文件
    */
    virtual CL_TYPE_t init() = 0;

    /**
     * 选择最后写入的文件
     */
    virtual void latestFileSelect( std::shared_ptr<std::vector<FileDesc>> & pfHeadList ) = 0;

    /** 
     * 选择下一个文件，并初始化文件头部。
     */
    virtual void nextFileSelect(std::unique_ptr<FileDesc> & pFDesc) = 0;

    /* 配置当前类别的日志，其文件数量，但文件大小，目录的路径，文件前缀 等 */
    // virtual void configSet(uint8_t fMaxCount, uint32_t fMaxLen, const std::string &fDir, const std::string &fPrefix) = 0;

    /**
     * 判断当前文件是否已满 
     * - 判断逻辑使用当前的写偏移量 + 空洞大小 与文件大小比较
    */
    bool isCurFileFull() {
        // return ( (m_curWriteOffset + CYLOG_FILE_HOLE_SIZE) >= m_fileMaxLength);
        return true;
    }

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
        timespec wt = { .tv_sec = wait_tms/1000, .tv_nsec = (wait_tms%1000)*1000000};
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


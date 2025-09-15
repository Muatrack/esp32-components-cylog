#pragma once

#include "common.hpp"
#include <filesystem>
#include <private_include/cylog_file.hpp>
#include <semaphore.h>
#include <private_include/errno.hpp>

/** 面对系统api, 读取写文件操作 */
class StoreAbs {
public:

    StoreAbs(){};

    virtual ~StoreAbs(){
        std::cout << "~StoreAbs()" << std::endl;
    }

    /* 新建指定目录，及所有新文件 */
    virtual CL_TYPE_t dirCreate() {
        std::cout << __FILE__ << "::" << __func__ <<"()." << __LINE__<< std::endl;
        return CL_OK;
    };

    /* 读取指定目录，所有文件名称 */
    virtual CL_TYPE_t dirRead( std::shared_ptr<std::vector<CLFile::FileDesc>> & pfHeadList ) {
        std::cout << __FILE__ << "::" << __func__ <<"()." << __LINE__<< std::endl;
        return CL_OK;
    };

    /* 读取指定文件内容数据，指定偏移量、长度的数据*/
    virtual CL_TYPE_t itemRead( uint16_t readSize, std::shared_ptr<uint8_t[]> &pOData, CLFile::FileDesc &fDesc){
        return CL_OK;
    };

    /* 读取指定文件内容数据，指定偏移量、长度的数据*/
    virtual CL_TYPE_t itemWrite(CLFile::FileDesc &fDesc, std::unique_ptr<uint8_t[]> pOData, uint16_t iLen) {
        return CL_OK;
    };

    /** 
     * 文件目录初始化. 新建对象后首先执行此函数 
     * - 文件存在，遍历目录下所有文件
     * - 文件不存在，新建目录，新建全数日志文件
    */
    virtual CL_TYPE_t init() = 0;

    /**
     * 选择最后写入的文件
     */
    virtual void latestFileSelect( std::shared_ptr<std::vector<CLFile::FileDesc>> & pfHeadList ) = 0;

    /** 
     * 选择下一个文件，并初始化文件头部。
     */
    virtual void nextFileSelect() = 0;

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
    static void StoreInit(uint8_t concurCount, std::string & logDir);

private:
    /** 文件系统读写信号量 */
    static sem_t m_signal;

protected:

/* v1 */
#if 0
    uint32_t     m_fileMaxCount;     // 文件数量上限
    uint32_t     m_fileCurCount;     // 已存在文件数量
    uint32_t    m_fileMaxLength;    // 单文件的最大长度
    uint32_t    m_curWriteOffset;   // 当前正在写文件中的写操作偏移位置
    std::string m_curWriteFilePath;     // 当前正在写入的文件路径
    std::string m_fileNamePrefix;   //文件名称前缀
    std::string m_dirPath;          // 文件目录
#endif
/* v1 done */
};


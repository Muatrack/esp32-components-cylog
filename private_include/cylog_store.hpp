#pragma once

#include "common.hpp"
#include <filesystem>

/** 面对系统api, 读取写文件操作 */
class StoreAbs {
public:

    virtual ~StoreAbs(){
        std::cout << "~StoreAbs()" << std::endl;
    }

    /* 读取指定目录，所有文件名称 */
    virtual CL_TYPE_t dirRead( const std::shared_ptr<std::string> pDPath = nullptr) {
        std::cout << __FILE__ << "::" << __func__ <<"()." << __LINE__<< std::endl;
        return CL_OK;
    };

    /* 读取指定文件头部数据*/
    virtual CL_TYPE_t headRead( const std::filesystem::path &fPath ){
        return CL_OK;
    };

    /* 读取指定文件内容数据，指定偏移量、长度的数据*/
    virtual CL_TYPE_t itemRead( const std::shared_ptr<std::string> pFPath, uint32_t readSize, std::shared_ptr<uint8_t[]> &pOData){
        return CL_OK;
    };

    /* 写入指定文件头部数据 */
    virtual CL_TYPE_t headWrite( const std::filesystem::path &fPath ){
        return CL_OK; 
    };

    /* 读取指定文件内容数据，指定偏移量、长度的数据*/
    virtual CL_TYPE_t itemWrite( const std::shared_ptr<std::string> pFPath, uint32_t readSize, std::shared_ptr<uint8_t[]> &pOData){
        return CL_OK;
    };

    /** 
     * 文件目录初始化. 新建对象后首先执行此函数 
     * - 文件存在，遍历目录下所有文件
     * - 文件不存在，新建目录，新建全数日志文件
    */
    virtual CL_TYPE_t init() = 0;

    /* 配置当前类别的日志，其文件数量，但文件大小，目录的路径，文件前缀 等 */
    virtual void configSet(uint8_t fMaxCount, uint32_t fMaxLen, const std::string &fDir, const std::string &fPrefix) = 0;

protected:
    uint32_t     m_fileMaxCount;     // 文件数量上限
    uint32_t     m_fileCurCount;     // 已存在文件数量
    uint32_t    m_fileMaxLength;    // 单文件的最大长度
    uint32_t    m_curWriteOffset;   // 当前正在写文件中的写操作偏移位置
    std::string m_curWriteFile;     // 当前正在写入的文件路径
    std::string m_fileNamePrefix;   //文件名称前缀
    std::string m_dirPath;          // 文件目录
};


#pragma once

#include <iostream>
#include "errno.hpp"
#include "common.hpp"
#include "cylog_store_abs.hpp"
#include "private_include/cylog_file.hpp"

using namespace std;

class StoreLinux : public StoreAbs {

public:    
    StoreLinux() = default;
    StoreLinux(StoreLinux&s) = delete;

    // void configSet(uint8_t fMaxCount, uint32_t fMaxLen, const std::string &fDir, const std::string &fPrefix) override;
    
    /* 新建指定目录，及所有新文件 */
    CL_TYPE_t dirCreate( const std::string & absPath) override;

    CL_TYPE_t dirCreate_bak() override;
    /** 
     * 遍历目录下所有文件，收集文件的名称，文件开写的时间戳（位于头部), 文件的写数据
     */
    CL_TYPE_t   dirRead( std::unique_ptr<CLFile::FileDesc> & pFDesc ) override;
    /* 检查指定目录中文件的合法性 */
    // CL_TYPE_t dirCheck() override;

    ~StoreLinux() {
        std::cout << "~StoreLinux()" << std::endl;
    }

    /** 
     * 文件目录初始化. 新建对象后首先执行此函数 
     * - 文件存在，遍历目录下所有文件
     * - 文件不存在，新建目录，新建全数日志文件
    */
    CL_TYPE_t init() override;
    /* 写入指定文件头部数据*/
    // CL_TYPE_t headWrite( const std::filesystem::path &fPath ) override;
    // CL_TYPE_t  headRead( const std::filesystem::path &fPath ) override;
    CL_TYPE_t itemWrite( std::unique_ptr<CLFile::FileDesc> &fDesc, const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen) override;

    /** 
     * 找到最后写入的文件。
     */
    void latestFileSelect( std::shared_ptr<std::vector<CLFile::FileDesc>> & pfHeadList ) override;

    /** 
     * 选择下一个文件，并初始化文件头部。
     * - 当前文件如已写满，则选择下一个文件. 
     * - 如未写满继续使用当前文件.
     * 
     */
    void nextFileSelect() override;

private:
    uint16_t    m_fileHoleSize;     /* 文件尾部的空洞大小 32字节 */
};

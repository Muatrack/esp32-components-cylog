#pragma once

#include <iostream>
#include "errno.hpp"
#include "common.hpp"
#include "cylog_store.hpp"


class StoreLinux : public StoreAbs {

public:

    StoreLinux() = default;
    StoreLinux(StoreLinux&s) = delete;

    void configSet(uint8_t fMaxCount, uint32_t fMaxLen, const std::string &fDir, const std::string &fPrefix) override;
    
    /* 读取指定目录，所有文件名称 */
    CL_TYPE_t dirRead( const std::shared_ptr<std::string> pDPath ) override;
    
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
    CL_TYPE_t headWrite( const std::filesystem::path &fPath ) override;
    CL_TYPE_t  headRead( const std::filesystem::path &fPath ) override;
};

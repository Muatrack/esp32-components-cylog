#pragma once

#include <iostream>
#include "cylog_impl.hpp"

class CYLogFactoryAbs {
    
public:
    /** 
     * @param store  存储对象
     * @param logDir 存储日志的相对路径     
     * @param prefix 文件名称前缀
     * @param fileSize 单个日志文件大小
     * @param fileCount 日志文件的数量
     */
    virtual CYLogImplAbs *create(std::shared_ptr<StoreAbs> &store, std::string logDir, uint32_t  fileSize, uint8_t fileCount, std::string prefix) = 0;

    virtual CYLogImplAbs *create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount) = 0;
    
    virtual ~CYLogFactoryAbs(){};
};

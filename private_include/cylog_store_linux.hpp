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
    ~StoreLinux() { std::cout << "~StoreLinux()" << std::endl; }


    /* 新建目录 @param absPath, 新建目录的绝对路径 */
    CL_TYPE_t dirCreate( const std::string & absPath ) override;
    /** 
     * 建立全部日志文件
     * 
     * @param  absPath:  在此目录下新建文件
     * @param  prefix:   新见文件的前缀
     * @param  fCount:   新建文件的数量 
     * @param  fSize:    单文件的大小-预占方式
     */
    CL_TYPE_t fileCreate( std::unique_ptr<FileDesc> & pFDesc ) override;
    /* 读取指定文件内容数据，指定偏移量、长度的数据*/
    CL_TYPE_t itemWrite( std::unique_ptr<FileDesc> & pFDesc, const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen, uint32_t timeoutMs) override;
    CL_TYPE_t dirRead( std::unique_ptr<FileDesc> & pFDesc ) override;
    CL_TYPE_t dirTraverse( std::unique_ptr<FileDesc> & pFDesc, std::vector<std::string> & fList ) override;    
    CL_TYPE_t fileTraverse( std::unique_ptr<FileDesc> & pFDesc, std::string &,  FileUsage & ) override; /* 遍历文件，查找可写位置 */
    /* 删除指定目录 */
    CL_TYPE_t dirDelete( const std::string & absPath ) override;
};

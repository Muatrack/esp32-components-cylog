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

    /** 
     * 遍历目录下所有文件，收集文件的名称，文件开写的时间戳（位于头部), 文件的写数据
     */
    // CL_TYPE_t   dirRead( std::unique_ptr<FileDesc> & pFDesc ) override;
    
    /*** 遍历目录，找到可写文件，可写位置*/
    // CL_TYPE_t dirTraverse( std::unique_ptr<FileDesc> & pFDesc, std::vector<std::string> & fList ) override;
    /* 遍历文件，查找可写位置 */
    // CL_TYPE_t fileTraverse( std::string &,  FileUsage & ) override;

    /** 
     * 选择下一个文件，并初始化文件头部。
     * - 当前文件如已写满，则选择下一个文件. 
     * - 如未写满继续使用当前文件.
     * 
     */
    // void nextFileSelect(std::unique_ptr<FileDesc> & pFDesc) override;
};

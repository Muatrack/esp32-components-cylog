#pragma once

#include <iostream>
#include "errno.hpp"
#include "common.hpp"
#include "cylog_store_abs.hpp"
#include "private_include/cylog_file.hpp"

using namespace std;

class StoreEspidf : public StoreAbs {

public:    
    StoreEspidf() = default;
    StoreEspidf(StoreEspidf&s) = delete;
    ~StoreEspidf() { std::cout << "~StoreEspidf()" << std::endl; }


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
    CL_TYPE_t fileCreate( std::unique_ptr<FileDesc> & pFDesc, const std::string prefix, uint8_t fCount, uint32_t fSize ) override;
    /* 读取指定文件内容数据，指定偏移量、长度的数据*/
    CL_TYPE_t itemWrite( std::unique_ptr<FileDesc> & pFDesc, const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen) override;
    
    CL_TYPE_t dirRead( std::unique_ptr<FileDesc> & pFDesc ) override;
};

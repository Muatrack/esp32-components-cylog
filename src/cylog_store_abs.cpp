
#include <stdio.h>
#include <iostream>
// #include <fcntl.h>
// #include <dirent.h>
// #include <unistd.h>
#include <filesystem>
#include "private_include/cylog_store_abs.hpp"

sem_t           StoreAbs::m_signal;
std::string     StoreAbs::m_LogRootDir;

namespace fs=std::filesystem;

void StoreAbs::StoreInit(uint8_t concurCount, std::string & logRootDir) {
    
    /* 保存日志绝对路径 */
    StoreAbs::m_LogRootDir = logRootDir;

    /** 设置信号量初值 */
    sem_init(&StoreAbs::m_signal, 0, concurCount);

    /** 检查目录 */
    // if(access(logDir.c_str(), 6)==0) 
    if(fs::exists(logRootDir)) {
        std::cout << "The path "<< logRootDir << " does exist" << std::endl;
        goto done;
    }

    std::cout << "The path "<< logRootDir << " doesn't exist. Gonna create it" << std::endl;
    fs::create_directory(logRootDir);
done:;
}

CL_TYPE_t StoreAbs::dirCreate( const std::string & absPath ) {
        bool bRet = false;
    CL_TYPE_t err = CL_OK;

    std::stringstream ss;

    /* 检查路径是否已存在 */
    if( std::filesystem::exists(absPath) ) { goto done; }

    /* 不存在，新建 */
    if( bRet=std::filesystem::create_directories(absPath), bRet ) { goto done; } 
    else { err = CL_EXCP_UNKNOW; goto excp; }

excp:
    return err;
done:
    return CL_OK;
}

CL_TYPE_t StoreAbs::fileCreate( const std::string & absPath, const std::string prefix, uint8_t fCount, uint32_t fSize ) {
    std::string fPath = "";

    /* 新建存储日志的绝对路径 */
    if( dirCreate(absPath)!=CL_OK ) { goto excp; }

    /* 拼接日志文件名称，在日志目录下逐一生成文件*/
    for(int i=0; i<fCount; i++) {
        fPath = absPath+"/"+prefix+"_"+((i<10)?"0":"") + std::to_string(i);
        // 如果文件存在，则跳过
        if( std::filesystem::exists(fPath) ) continue;

        std::ofstream _f(fPath);
        _f.close();
        // 初始文件的大小
        std::filesystem::resize_file( fPath, fSize );
        std::cout<<"Create log file:"<<fPath<<std::endl;
    }

    return CL_OK;

excp:
    return CL_EXCP_UNKNOW;
}
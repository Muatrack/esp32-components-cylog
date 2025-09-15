
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

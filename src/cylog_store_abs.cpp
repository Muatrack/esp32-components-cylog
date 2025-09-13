
#include <stdio.h>
#include <iostream>
// #include <fcntl.h>
// #include <dirent.h>
// #include <unistd.h>
#include <filesystem>
#include "private_include/cylog_store_abs.hpp"

sem_t   StoreAbs::m_signal;

namespace fs=std::filesystem;

void StoreAbs::StoreInit(uint8_t concurCount, std::string & logDir) {
    
    /** 设置信号量初值 */
    sem_init(&StoreAbs::m_signal, 0, concurCount);

    /** 检查目录 */
    // if(access(logDir.c_str(), 6)==0) 
    if(fs::exists(logDir)) {
        std::cout << "The path "<< logDir << " does exist" << std::endl;
        goto done;
    }

    std::cout << "The path "<< logDir << " doesn't exist. Gonna create it" << std::endl;
    fs::create_directory(logDir);
done:;
}

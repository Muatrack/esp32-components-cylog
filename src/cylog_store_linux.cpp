
#include <iostream>
#include "private_include/cylog_store_linux.hpp"

CL_TYPE_t StoreLinux::init() {
    std::cout<< "StoreLinux::init()" << std::endl;
    return CL_OK;
}

void StoreLinux::configSet(uint8_t fMaxCount, uint32_t fMaxLen, const std::string &fDir, const std::string &fPrefix) {
        std::cout<< "StoreLinux::configSet()" << std::endl;
}

#include <iostream>
#include "private_include/cylog_store_linux.hpp"

CL_TYPE_t StoreLinux::init() {
    std::cout<< "------------------- StoreLinux::init() -------------------" << std::endl;
    std::cout<< "Dir:" << m_dirPath << std::endl;
    std::cout<< "File Max Count:" << (unsigned)m_fileMaxCount << std::endl;
    std::cout<< "File Cur Count:" << (unsigned)m_fileCurCount << std::endl;
    std::cout<< "File Max Length:" << m_fileMaxLength << std::endl;
    std::cout<< "File Name Prefix:" << m_fileNamePrefix << std::endl;
    std::cout<< "------------------- StoreLinux::init DONE -------------------" << std::endl;
    return CL_OK;
}

void StoreLinux::configSet(uint8_t fMaxCount, uint32_t fMaxLen, const std::string &fDir, const std::string &fPrefix) {
    std::cout << "maxCount:" << fMaxCount << std::endl;
    std::cout << "curCount:" << fMaxLen << std::endl;
    m_fileMaxCount = fMaxCount;
    m_fileMaxLength = fMaxLen;
    m_fileCurCount = 0;
    m_dirPath = fDir;
    m_fileNamePrefix = fPrefix;

    std::cout<< "StoreLinux::configSet()" << std::endl;
}

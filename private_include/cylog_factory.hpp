#pragma once

#include <iostream>
#include "cylog_impl.hpp"

class CYLogFactoryAbs {
    
public:
    /** 
     * @param dir 日志存储路径
     */
    virtual CYLogImplAbs *createLog( const std::string & logDir ) = 0;
    // {
    //      std::cout << "CYLogFactoryAbs::createLog" << std::endl;
    //      return nullptr;
    // };

    virtual ~CYLogFactoryAbs(){};
};



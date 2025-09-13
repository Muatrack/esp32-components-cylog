#pragma once

#include <iostream>
#include "cylog_impl.hpp"

class CYLogFactoryAbs {
    
public:
    /** 
     * @param dir 日志的存储路径
     */
    virtual CYLogImplAbs *create( const std::string & logDir, std::shared_ptr<StoreAbs> &store ) = 0;
    
    virtual ~CYLogFactoryAbs(){};
};

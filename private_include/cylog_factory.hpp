#pragma once

#include <iostream>
#include "cylog_impl.hpp"

class CYLogFactoryAbs {
    
public:
    /** 
     * @param logDir 存储日志的相对路径
     * @param store  存储对象
     */
    virtual CYLogImplAbs *create( const std::string & logDir, std::shared_ptr<StoreAbs> &store ) = 0;
    
    virtual ~CYLogFactoryAbs(){};
};

#pragma once

#include <iostream>
#include "cylog_impl.hpp"

class CYLogFactoryAbs {
    
public:
    virtual CYLogImplAbs *createLogInstance() = nullptr;
    virtual ~CYLogFactoryAbs(){};
};

class CyLogAlarmFactory : public CYLogFactoryAbs {
public:
    CYLogImplAbs *createLogInstance() override {
        return nullptr;
    }
};
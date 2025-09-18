#pragma once

#include <iostream>
#include "errno.hpp"
#include "common.hpp"
#include "cylog_store_abs.hpp"
#include "private_include/cylog_file.hpp"

using namespace std;

class StoreLinux : public StoreAbs {

public:    
    StoreLinux() = default;
    StoreLinux(StoreLinux&s) = delete;
    ~StoreLinux() { std::cout << "~StoreLinux()" << std::endl; }
};

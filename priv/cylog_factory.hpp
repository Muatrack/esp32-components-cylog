#pragma once

#include <iostream>
#include "cylog_generic.hpp"

class CYLogFactory {
public:
    static CYLog* create(char t) {
        switch ( t )
        {
        case 'p':
            return new CYLogPerformance;
        case 'e':
            return new CYLogException;
        default:
            return nullptr;
        }
    }
};
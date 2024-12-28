#pragma once

#include "log_generic.hpp"
#include <stdbool.h>
#include <string>

class CYLog: public log_generic::LogGeneric
{
public:
    void write();
    void path_set(std::string path);
};



class CYLogPerformance: public CYLog
{
private:
    /* data */
public:
};


class CYLogException: public CYLog
{
private:
    /* data */
public:
};


class CYLogElc: public CYLog
{
private:
    /* data */
public:
};


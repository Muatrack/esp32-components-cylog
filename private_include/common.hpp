#pragma once

#ifdef USE_SYSTEM_LINUX
    // #include <unistd.h>
    #include <cstdint>
#else
    #include <stdint.h>
#endif
#include <memory>

enum  class CYlogType:char {
    CYT_ALARM,          //告警
    CYT_PERFORM,        //运行
    CYT_EWAVE,          //波形
    CYT_SYSEXCP         //系统异常
};

/** 定义日志输出开关宏 */
#ifdef USING_CYLOG_DEBUG
    #define CYLOG_PRINT(log) log;
#else
    #define CYLOG_PRINT(log)
#endif

#define MIN(x,y) x>y?y:x

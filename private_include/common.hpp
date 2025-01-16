#pragma once

#ifdef USE_SYSTEM_LINUX
    // #include <unistd.h>
    #include <cstdint>
#else
    #include <stdint.h>
#endif

enum  class CYlogType:char {
    CYT_ALARM,          //告警
    CYT_PERFORM,        //运行
    CYT_EWAVE,          //波形
    CYT_SYSEXCP         //系统异常
};
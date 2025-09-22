#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * 遍历日志后，以回调的方式将日志内容返回。 定义读取日志的回调函数类型
 * @param data: uint8_t[] 数据的字节序列
 * @param dataLen: 数据的长度
 * @return 
 *  - -1: 终止遍历
 * 
 *  -  0: 继续便利
*/
typedef int (*cylog_traversal_cb_t)(uint8_t data[], uint16_t dataLen );

/** 
 * 告警日志过滤，回调函数类型
 */
typedef int (*cylog_alarm_filter_t)(uint8_t data[], uint16_t dataLen);


/**
 * 定义日志类型
 */
typedef enum {
    CYLOG_T_ALARM   = 0x00, /* 告警 */
    CYLOG_T_EXCP,           /* 异常 */
    CYLOG_T_PMETE_QTR,      /* 整刻钟电量记录 */
    CYLOG_T_PMETE_DAY,      /* 每日电量 */
    CYLOG_T_POWER,          /* 负载开关机日志 power on/off */
    CYLOG_T_SWITCH,         /* 采集器分合闸机日志 switch on/off */
    CYLOG_T_DEF             /* 缺省值， 默认值*/
} cylog_type_t;

#pragma pack(push, 1)

typedef struct{
    uint16_t circuit_ID;    // 回路ID，经过CRC16(目标SN)得到
}cylog_def_t;

/** 告警日志数据结构 */
typedef  struct {
    cylog_def_t base;       /* 日志共用数据结构 */
    uint16_t    type;       /* 告警类型 */
    uint32_t    createTm;   /* 发生产生时间戳 */
    uint32_t    clearTm;    /* 消警时间戳 */    
    uint32_t    seq;        /* 告警序列号 */
    uint16_t    val;        /* 触发告警的异常值 */
} cylog_alarm_t;

/** 电量日志数据结构 */
typedef  struct {
    cylog_def_t base;       /* 日志共用数据结构 */
    uint32_t    createTm;   /* 发生产生时间戳 */
    uint64_t    val;        /* 计量值 */
} cylog_pmeter_t;

/** 开关机日志数据结构 */
typedef  struct {
    cylog_def_t base;       /* 日志共用数据结构 */
    uint32_t    createTm;   /* 发生产生时间戳 */
    uint8_t     val;        /* 负载开/关的状态值 */
} cylog_power_t;

/** 分合闸日志数据结构 */
typedef  struct {
    cylog_def_t base;       /* 日志共用数据结构 */
    uint32_t    createTm;   /* 发生产生时间戳 */
    uint8_t     action;     /* 分/合闸动作 */
    uint8_t     src;        /* 分合闸指令来源 */
} cylog_switch_t;

/** 系统异常日志数据结构 */
typedef  struct {
    cylog_def_t base;       /* 日志共用数据结构 */
    uint32_t    createTm;   /* 发生产生时间戳 */
    uint16_t    type;       /* 异常分类 */
    uint16_t    val;        /* 异常值 */
} cylog_sysexcp_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

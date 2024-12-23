#ifndef _CYCLELOG_H_
#define _CYCLELOG_H_

#include "cylog_comm.h"
#include "unistd.h"

#pragma pack(4)

/** 事件的分区初始化 */
#define CYLOG_EVENT_PARTITION_INIT() ({cylog_init(CYP_EVNT);})
#define CYLOG_EALARM(_dev_id,...) _CYLOG_WRITE(CYP_EVNT, CYLTYP_EVENT_ALARM,_dev_id, __VA_ARGS__)
#define CYLOG_EEXCEPT(_dev_id,...) _CYLOG_WRITE(CYP_EVNT, CYLTYP_EVENT_EXCP,_dev_id, __VA_ARGS__)
#define CYLOG_EWARN(_dev_id,...) _CYLOG_WRITE(CYP_EVNT, CYLTYP_EVENT_WARN,_dev_id, __VA_ARGS__) 
#define CYLOG_EREAD(_blk_idx, _cb) _CYLOG_READ(CYP_EVNT, _blk_idx, _cb) 


/** 记录的分区初始化 */
#define CYLOG_RECORD_PARTITION_INIT() ({cylog_init(CYP_RPOT);})
#define CYLOG_RCURRENT(_dev_id,...) _CYLOG_WRITE(CYP_RPOT, CYLTYP_RECORD_CUR,_dev_id, __VA_ARGS__)
#define CYLOG_RVOLTAGE(_dev_id,...) _CYLOG_WRITE(CYP_RPOT, CYLTYP_RECORD_VOL,_dev_id, __VA_ARGS__)
#define CYLOG_RPOWER(_dev_id,...) _CYLOG_WRITE(CYP_RPOT, CYLTYP_RECORD_POWER,_dev_id, __VA_ARGS__)
#define CYLOG_RREAD(_blk_idx, _cb) _CYLOG_READ(CYP_RPOT, _blk_idx, _cb) 

#endif

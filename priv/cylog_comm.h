#ifndef _CYLOG_COMMON_H_
#define _CYLOG_COMMON_H_
#include "unistd.h"
#include <stdint.h>

/** @brief 日志分区的类型
 * @param CYP_EVNT 分区用于alamm、warning... 日志
 * @param CYP_RPOT 分区用于电流、电压、功率... 日志
*/
typedef enum {
    /** 对应ALARM、warning...event*/
    CYP_EVNT = 0x00,
    /** 对应 电流、电压等 record*/
    CYP_RPOT,
    _CYP_CONT,
}cylog_partition_type_t;

/** @brief 定义日志类别 */
typedef enum {
    CYLTYP_EVENT_ALARM = 0x1,   //告警
    CYLTYP_EVENT_WARN,          //警告
    CYLTYP_EVENT_EXCP,          //异常

    CYLTYP_RECORD_VOL = 0x7,        //电压记录
    CYLTYP_RECORD_CUR,              //电流记录
    CYLTYP_RECORD_POWER,            //功率记录

    _CYLTYPE_
}cylog_type_t;

typedef struct {
    /** 子设备Id*/
    uint8_t  _devId;
    /** 日志长度bytes, 不含头部 */
    uint16_t _log_length;
    /** 日志发生时间戳 */
    uint32_t _timestamp;
    /** 日志发生时毫秒 */
    uint16_t _time_ms;
    /** 日志类型 */
    cylog_type_t _type;
    /** 日志数据*/    
    uint8_t _data[0];
}cylog_info_t;

typedef void(*cylog_read_cb_t)(cylog_info_t *);

/** 定义每块尾部预留空间 */
#define CYLOG_BLOCK_HOLE 128

/** @brief 定义日志单元的固定大小, 含头部信息*/
#define CYLOG_EVENT_ITEM_FIX_SIZE   CYLOG_BLOCK_HOLE

/** @brief 定义日志单元的固定大小, 含头部信息*/
#define CYLOG_RECORD_ITEM_FIX_SIZE   CYLOG_BLOCK_HOLE

typedef int8_t cylog_partition_handle_t;
cylog_partition_handle_t cylog_init(cylog_partition_type_t);
int8_t cylog_write(cylog_partition_handle_t, cylog_type_t _t, uint8_t _id, char* _log);
int8_t cylog_read(cylog_partition_handle_t _pH, uint8_t blockStartIndex, cylog_read_cb_t cb);

/** @brief  通用写入日志宏
 * @param _id 设备id或者地址
 * @param ... 可变日志
 * @return 成功返回0， 失败返回-1
*/
#define _CYLOG_WRITE(_parti_type, _event_type,_id, ...) \
    ({  \
        int8_t _ret = 0;    \
        char *_pBuf = malloc(CYLOG_EVENT_ITEM_FIX_SIZE); \
        if (_pBuf == NULL) {    \
            _ret = -1;  \
        } else {    \
            if(_ret = snprintf(_pBuf, (CYLOG_EVENT_ITEM_FIX_SIZE - 1), __VA_ARGS__), _ret > 0){    \
                _ret = cylog_write(_parti_type, _event_type, _id, _pBuf);    \
            }   \
            free(_pBuf);    \
        }   \
        _ret;   \
    })

/** @brief  通用读取日志宏
 * @param cb 日志数据回调函数
 * @return 成功返回0， 失败返回-1
*/
#define _CYLOG_READ(_parti_type, idx, cb) cylog_read(_parti_type, idx, cb)

#define CY_LOGI(_fmt_, ...) printf("\e[0;32;40m" _fmt_ "\n", ##__VA_ARGS__)
#define CY_LOGW(_fmt_, ...) printf("\e[0;33;40m" _fmt_ "\n", ##__VA_ARGS__)
#define CY_LOGE(_fmt_, ...) printf("\e[0;31;40m" _fmt_ "\n", ##__VA_ARGS__)

#endif
#ifndef _CYLOG_PRIV_H_
#define _CYLOG_PRIV_H_

#include "unistd.h"
#include "cylog_comm.h"
#include <stdint.h>

#define CYLOG_PARITI_EVENT_NAME "events"
#define CYLOG_PARITI_RECORD_NAME "records"

/** @brief 定义block(块)大小 4k */
#define CYLOG_BLOCK_SIZE (4 * 1024)

/** @brief 日志头标记定义*/
#define CYLOG_ITEM_HEAD_FLAG   0x33

#pragma pack(4)
/** 
 * @brief log 分区信息, 贮存在RAM 
*/
typedef struct{
    /** @brief 日志固定大小*/
    uint8_t _clog_fixed_len;
    /** @brief 分区表中的name */
    char* _clog_parti_name;
    /** @brief block 数量 */
    uint8_t _clog_block_count;

    /** @brief block 选中索引 */
    uint8_t _clog_block_index;

    /** @brief 分区大小 */
    uint32_t _clog_parti_size;

    /** @brief 下次写入block中的偏移*/
    uint16_t _clog_block_offset;

    /** @brief 分区入口 */
    void *_clog_parti_handl;
}cylog_partition_t;

/** 定义块信息 */
typedef struct {
    // 块中可写入位置的偏移量
    uint16_t _tail_offset;
    // 块中尾部item的时间戳;
    uint32_t _ts;
    uint16_t _ms;
}cylog_block_info_t;

/** 
 * @brief 定义日志头部信息 
*/
typedef struct {

    /** 日志条目头标记 */
    uint8_t  _flag;
    /** 设备的id*/
    uint8_t  _devId;
    /** 日志长度bytes, 不含头部 */
    uint16_t _log_length;
    /** 日志发生时间戳 */
    uint32_t _timestamp;
    /** 日志发生时毫秒 */
    uint16_t _time_ms;
    /** 日志类型 */
    cylog_type_t _type;
    /** 日志内容校验值 */
    uint16_t _crc16_val;
    /** 日志数据*/    
    uint8_t _data[0];
}cylog_head_t;


int8_t  cylog_block_select(void *pConf, cylog_block_info_t *pBlockInfo);
int8_t  cylog_partition_find(void *pConf);
void    cylog_block_erase(void *pConf);
void    cylog_block_traversal(void *pConf, uint8_t blockIndex, void *pBlockInfo);
int8_t  cylog_partition_write(void *pConf, uint8_t *pData, uint32_t dataLen);
void    cylog_block_log_read(void *pConf, uint8_t blockIndex, cylog_read_cb_t logItemCb);
#endif
#include <stdio.h>
#include "cylog.h"
#include "cylog_priv.h"
#include "malloc.h"
#include "string.h"
#include "time.h"
#include "sys/time.h"
#include "unistd.h"

static cylog_partition_t *pCylogPartiWrap[_CYP_CONT];

#define TAG "cylog"

static void _cylog_data_assamble (uint8_t *pData, cylog_type_t _t, uint8_t _id, char* _log, uint8_t logLen) {
    cylog_head_t *pHead = pData;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    pHead->_flag = CYLOG_ITEM_HEAD_FLAG;
    pHead->_log_length = logLen;
    pHead->_timestamp = tv.tv_sec;// time(NULL);
    pHead->_time_ms = (tv.tv_usec / 1000);
    pHead->_type = _t;
    pHead->_devId = _id;
    memcpy(pHead->_data, _log, pHead->_log_length);

    // CY_LOGI("-------%s(). ts:%ld, ms:%ld / 1000 = %d\n", __func__, tv.tv_sec, tv.tv_usec, (int)(tv.tv_usec / 1000));
}

/** 
 * @brief 遍历日志分区，查找可写入的block。填充 cylog_partition_t 全局变量
 * @return 成功返回0， 失败返回-1
 */
static int8_t _cylog_partition_traversal(void *_pConf, void *_pBlockInfo) {
    cylog_block_info_t *pBlockInfo = _pBlockInfo;
    uint8_t tmpBlockIndex = 0;
    cylog_partition_t *pCylogConf = _pConf;

    while(tmpBlockIndex < pCylogConf->_clog_block_count) {
        // 检索块信息
        cylog_block_traversal(pCylogConf, tmpBlockIndex, &pBlockInfo[tmpBlockIndex]);

        CY_LOGI("\e[1;32m" "block[%d] traversal done. last_timestamp:%u _ ms:%u .offset:%u \n", 
                        tmpBlockIndex, 
                        pBlockInfo[tmpBlockIndex]._ts, 
                        pBlockInfo[tmpBlockIndex]._ms, 
                        pBlockInfo[tmpBlockIndex]._tail_offset);
        //如果块满，则继续遍历下一个块
        tmpBlockIndex ++;
    }

    return 0;
}

/**************************** public below ****************************/

/**
 * @brief 写入日志
 * @param _pH 日志分区的handle
 * @param _t, 日志类型为 ALARM ...
 * @param _log, 日志内容字串._id, 设备ID/address，用于识别日志所属的设备
 * @return 成功返回0， 失败返回 -1
*/
int8_t cylog_write(cylog_partition_handle_t _pH, cylog_type_t _t, uint8_t _id, char* _log) {
    uint8_t *pData = NULL;

    cylog_partition_t *pCylogConf = pCylogPartiWrap[_pH];
    if (pCylogConf == NULL)
        goto excp;

#ifdef CONFIG_CYLOG_LOG_LEN_IMMUTA
    uint8_t logLen = pCylogConf->_clog_fixed_len;
#endif

#ifdef CONFIG_CYLOG_LOG_LEN_MUTA
    uint8_t logLen = strlen(_log);
#endif

    uint16_t dataLen = sizeof(cylog_head_t) + logLen;
    if (pData = malloc(dataLen), pData == NULL)
        goto excp;

    _cylog_data_assamble(pData, _t, _id, _log, logLen);

    CY_LOGI("%s(). write data  type:0x%x len:%d\n", __func__, _t, dataLen);
    if (cylog_partition_write(pCylogConf, pData, dataLen) < 0) {
        goto excp;
    }

    if (pData)  free(pData);
    return 0;
excp:
    if (pData)  free(pData);
    return -1;
}
 
/**
 * @brief 读取日志,读取范围=整块
 * @param partiType 分区类型-事件分区/记录分区
 * @param blockStartIndex 块的索引
 * @param cb 读取到日志后,调用此函数进行传递
 * @param return 成功返回0， 失败放回-1
*/
int8_t cylog_read(cylog_partition_handle_t _pH, uint8_t blockStartIndex, cylog_read_cb_t cb) {
    cylog_partition_t *pCylogConf = pCylogPartiWrap[_pH];
    if (pCylogConf == NULL)
        goto excp;

    cylog_block_log_read(pCylogConf, blockStartIndex, cb);
    return 0;
excp:
    return -1;
}

/**
 * @brief 初始化
 * @param return 成功返回0， 失败返回-1
*/
cylog_partition_handle_t cylog_init(cylog_partition_type_t _pt) {
    cylog_block_info_t *pBlockInfo = NULL;

    if (_pt < 0 || (_pt >= _CYP_CONT)) {
        goto excp;
    }

    if (pCylogPartiWrap[_pt] != NULL) {
        return _pt;
    }

    if (pCylogPartiWrap[_pt] = malloc(sizeof(cylog_partition_t)), pCylogPartiWrap[_pt] == NULL) {
            goto excp;
    }

    cylog_partition_t  *pCylogConf = pCylogPartiWrap[_pt];

    if (_pt == CYP_EVNT) {
#ifdef CONFIG_CYLOG_LOG_LEN_IMMUTA
        pCylogConf->_clog_fixed_len = CONFIG_CYLOG_EVENT_LEN;
#endif        
        pCylogConf->_clog_parti_name = CYLOG_PARITI_EVENT_NAME;

    } else if (_pt == CYP_RPOT) {
#ifdef CONFIG_CYLOG_LOG_LEN_IMMUTA
        pCylogConf->_clog_fixed_len = CONFIG_CYLOG_RECORD_LEN;
#endif
        pCylogConf->_clog_parti_name = CYLOG_PARITI_RECORD_NAME;
    } else {
        goto excp;
    }

    CY_LOGI("\e[1;33m" "------------------ %s() parti name %s %c%c ------------------\n", __func__, pCylogConf->_clog_parti_name, 25, 25);

    pCylogConf->_clog_block_count = 0;
    pCylogConf->_clog_block_index = 0;
    pCylogConf->_clog_block_offset = 0;
    pCylogConf->_clog_parti_handl = NULL;
    pCylogConf->_clog_parti_size = 0;

    do {
        // 查找分区
        if (cylog_partition_find(pCylogConf) < 0)
            goto excp;

        if (pBlockInfo = malloc(pCylogConf->_clog_block_count * sizeof(cylog_block_info_t)), pBlockInfo == NULL)
            goto excp;

        memset(pBlockInfo, 0, pCylogConf->_clog_block_count * sizeof(cylog_block_info_t));

        // 遍历分区
        if (_cylog_partition_traversal(pCylogConf, pBlockInfo) < 0)
            break;

        // 选择写入块
        int8_t blockIndex = cylog_block_select(pCylogConf, pBlockInfo);
        if (blockIndex < 0) {
            CY_LOGI("------------- %s() line:%d, Exception.", __func__, __LINE__);
            goto excp;
        }

    }while(0);

    CY_LOGI("\e[1;31m" "%s().[selected block][hit index:%02d -> offset:%u].\n", __func__, 
            pCylogConf->_clog_block_index,
            pCylogConf->_clog_block_offset);

    if (pBlockInfo) free(pBlockInfo);

    CY_LOGI("\e[1;33m" "------------------------ %s() done %c%c------------------------\n", __func__, 24, 24);
    return _pt;
excp:
    if (pBlockInfo) free(pBlockInfo);
    if (pCylogPartiWrap[_pt]) free (pCylogPartiWrap[_pt]);
    pCylogPartiWrap[_pt] = NULL;
    CY_LOGI("-------- %s() %d , failed to init cylog.\n", __func__, __LINE__);
    CY_LOGI("--------------------- %s(). done. Excp --------------------\n", __func__);
    return -1;
}

/** @brief 释放资源 */
void cylog_deinit(cylog_partition_handle_t _pt) {
    if (pCylogPartiWrap[_pt] != NULL) {
        free(pCylogPartiWrap[_pt]);
        pCylogPartiWrap[_pt] = NULL;
    }
}
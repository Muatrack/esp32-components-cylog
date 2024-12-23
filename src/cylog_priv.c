#include "cylog.h"
#include "cylog_priv.h"
#include "cylog_comm.h"
#include "stdio.h"
#include "memory.h"
#include "cylog_wrapper_esp32.h"

#define CYLOG_PARTI_MMAP_SIZE   (1024)
#define ESP_PARTITION_SUBTYPE_LOG   0x10

/** @brief 查找分区表中的Log 分区  */
int8_t cylog_partition_find(void *pConf) {
    cylog_partition_t *pCylogConf = pConf;
    cylog_partition_esp32_t partiEsp32;

    void  *pLogParti = NULL;

    if (pLogParti = cylog_partition_find_wrapper(pCylogConf->_clog_parti_name, &partiEsp32), pLogParti == NULL) {
        CY_LOGI("%s(), %d .", __func__, __LINE__);
        return -1;
    }
    
    CY_LOGI("log parittion address:0x%x,  size:%uKB", partiEsp32._address, partiEsp32._size / 1024);//, pLogParti->erase_size);
    pCylogConf->_clog_block_count = (partiEsp32._size / CYLOG_BLOCK_SIZE);
    pCylogConf->_clog_parti_handl = pLogParti;
    pCylogConf->_clog_parti_size  = partiEsp32._size;
    return 0;
}

/**
 * @brief 遍历映射内存，查找可写入的offset. cylog_head_t 类型，比较flag是否为 CYLOG_ITEM_HEAD_FLAG，\
 * @brief 函数结束条件，（1）当前offset所指内存非有效日志头部，返回当前offset, (2)当前offset所指日志的尾部超出当前映射内存大小，返回当前offset
 * @param pMem 内存地址
 * @param memSize 内存字节长度
 * @param logLen  固定长度的日志大小--不含日志头
 * @return 返回当前内存映射范围最后一条完整日志的尾部偏移量,可能 <0, ==0
*/
static int16_t _cylog_mem_read_len_immutable(uint8_t *pMem, uint16_t memSize, cylog_read_cb_t cb, uint16_t blockOffset, uint8_t fixedLen) {

    uint16_t offset = 0;
    cylog_head_t *pLogHead = NULL;
    cylog_info_t *pLogInfo = NULL;

    // CY_LOGI("-----%s(), line:%d. new mmap [ start offset:%u mmapSize:%u ]", __func__, __LINE__, blockOffset, memSize);

    while((offset + sizeof(cylog_head_t)) <= memSize) {
        pLogHead = pMem + offset;

        // CY_LOGI("-----%s(), line:%d. blockOffset:%u  mmap offset:%u", __func__, __LINE__, blockOffset + offset, offset);
        if (pLogHead->_flag == CYLOG_ITEM_HEAD_FLAG) {
            // CY_LOGI("-----%s(), line:%d.", __func__, __LINE__);
            if ((offset + sizeof(cylog_head_t) + fixedLen) <= memSize) {
                // CY_LOGI("-----%s(), line:%d.", __func__, __LINE__);
                pLogInfo = malloc(sizeof(cylog_info_t) + fixedLen);
                if (pLogInfo) {
                    pLogInfo->_log_length = pLogHead->_log_length;
                    pLogInfo->_timestamp = pLogHead->_timestamp;
                    pLogInfo->_time_ms = pLogHead->_time_ms;
                    pLogInfo->_devId = pLogHead->_devId;
                    pLogInfo->_type  = pLogHead->_type;
                    memcpy(pLogInfo->_data, pLogHead->_data, fixedLen);

                    cb(pLogInfo);
                    free(pLogInfo);
                } else {
                    // CY_LOGI("-----%s(), line:%d. blockOffset:%u  mmap offset:%u", __func__, __LINE__, blockOffset + memSize, offset);
                }
            } else {
                break;
            }
        } else {
            // CY_LOGI("-----%s(), line:%d. blockOffset:%u  mmap offset:%u", __func__, __LINE__, blockOffset + memSize, offset);
        }

        offset += sizeof(cylog_head_t) + fixedLen;
    }

    // 映射的内存，已全部检查完毕。未找到可写入位置
    return (offset);
}

/**
 * @brief 遍历映射内存，查找可写入的offset. cylog_head_t 类型，比较flag是否为 CYLOG_ITEM_HEAD_FLAG，\
 * @brief 函数结束条件，（1）当前offset所指内存非有效日志头部，返回当前offset, (2)当前offset所指日志的尾部超出当前映射内存大小，返回当前offset
 * @param pMem 内存地址
 * @param memSize 内存字节长度
 * @param logLen  固定长度的日志大小--不含日志头
 * @return 返回当前内存映射范围最后一条完整日志的尾部偏移量,可能 <0, ==0
*/
static int16_t _cylog_mem_traversal_len_immutable(uint8_t *pMem, uint16_t memSize, cylog_block_info_t* pBlockInfo, uint16_t blockOffset, uint8_t fixedLen) {

    uint16_t offset = 0;
    cylog_head_t *pLogHead = NULL;

    CY_LOGI("-----%s(), line:%d. new mmap", __func__, __LINE__);
    while((offset + sizeof(cylog_head_t)) <= memSize) {
        pLogHead = pMem + offset;

        if (pLogHead->_flag == CYLOG_ITEM_HEAD_FLAG) {
            if ((offset + sizeof(cylog_head_t) + fixedLen) <= memSize) {
                //保存最新有效日志的时间戳
                pBlockInfo->_ts = pLogHead->_timestamp;
                pBlockInfo->_ms = pLogHead->_time_ms;
                pBlockInfo->_tail_offset = blockOffset + offset + sizeof(cylog_head_t) + fixedLen;
            }
        }

        offset += sizeof(cylog_head_t) + fixedLen;
    }

    CY_LOGI("%s().line:%d, block tail offset: %u",__func__, __LINE__, pBlockInfo->_tail_offset);
    // 映射的内存，已全部检查完毕。未找到可写入位置
    return (offset);
}

/**
 * @brief 遍历映射内存，查找可写入的offset. cylog_head_t 类型，比较flag是否为 CYLOG_ITEM_HEAD_FLAG，\
 * @brief 函数结束条件，（1）当前offset所指内存非有效日志头部，返回当前offset, (2)当前offset所指日志的尾部超出当前映射内存大小，返回当前offset
 * @param pMem 内存地址
 * @param memSize 内存字节长度
 * @return 返回当前内存映射范围最后一条完整日志的尾部偏移量,可能 <0, ==0
*/
static int16_t _cylog_mem_read_len_mutable(uint8_t *pMem, uint16_t memSize, cylog_block_info_t* pBlockInfo, uint16_t blockOffset) {

    uint16_t offset = 0;
    cylog_head_t *pLogHead = NULL;

    CY_LOGI("-----%s(), line:%d. new mmap", __func__, __LINE__);
    while((offset + sizeof(cylog_head_t)) <= memSize) {
        pLogHead = pMem + offset;
        if (pLogHead->_flag == CYLOG_ITEM_HEAD_FLAG) {
            
            CY_LOGI("-----%s(), line:%d. [ block mmap offset:%u | memSize:%u ]", __func__, __LINE__, offset, memSize);

            if ((offset + sizeof(cylog_head_t) + pLogHead->_log_length) <= memSize) {
                offset += sizeof(cylog_head_t) + pLogHead->_log_length;

                //保存最新有效日志的时间戳
                pBlockInfo->_ts = pLogHead->_timestamp;
                pBlockInfo->_tail_offset = blockOffset + offset;
                continue;
            } else {
                // CY_LOGI("-----%s(), line:%d. [ offset:%u | memSize:%u ]", __func__, __LINE__, offset, memSize);
                break;
            }
        } else {
            CY_LOGI("-----%s(), line:%d. [ block mmap offset:%u | flag:0x%x ]", __func__, __LINE__, offset, pLogHead->_flag);
            break;
        }
    }

    // 映射的内存，已全部检查完毕。未找到可写入位置
    return (offset);
}

/**
 * @brief 遍历映射内存，查找可写入的offset. cylog_head_t 类型，比较flag是否为 CYLOG_ITEM_HEAD_FLAG，\
 * @brief 函数结束条件，（1）当前offset所指内存非有效日志头部，返回当前offset, (2)当前offset所指日志的尾部超出当前映射内存大小，返回当前offset
 * @param pMem 内存地址
 * @param memSize 内存字节长度
 * @return 返回当前内存映射范围最后一条完整日志的尾部偏移量,可能 <0, ==0
*/
static int16_t _cylog_mem_traversal_len_mutable(uint8_t *pMem, uint16_t memSize, cylog_block_info_t* pBlockInfo, uint16_t blockOffset) {

    uint16_t offset = 0;
    cylog_head_t *pLogHead = NULL;

    CY_LOGI("-----%s(), line:%d. new mmap", __func__, __LINE__);
    while((offset + sizeof(cylog_head_t)) <= memSize) {
        pLogHead = pMem + offset;
        if (pLogHead->_flag == CYLOG_ITEM_HEAD_FLAG) {
            
            CY_LOGI("-----%s(), line:%d. [ block mmap offset:%u | memSize:%u ]", __func__, __LINE__, offset, memSize);

            if ((offset + sizeof(cylog_head_t) + pLogHead->_log_length) <= memSize) {
                offset += sizeof(cylog_head_t) + pLogHead->_log_length;

                //保存最新有效日志的时间戳
                pBlockInfo->_ts = pLogHead->_timestamp;
                pBlockInfo->_tail_offset = blockOffset + offset;
                continue;
            } else {
                // CY_LOGI("-----%s(), line:%d. [ offset:%u | memSize:%u ]", __func__, __LINE__, offset, memSize);
                break;
            }
        } else {
            CY_LOGI("-----%s(), line:%d. [ block mmap offset:%u | flag:0x%x ]", __func__, __LINE__, offset, pLogHead->_flag);
            break;
        }
    }

    // 映射的内存，已全部检查完毕。未找到可写入位置
    return (offset);
}

void cylog_block_erase(void *pConf) {
    cylog_partition_t *pCylogConf = pConf;
    int8_t ret = cylog_partition_erase_wrapper(pCylogConf->_clog_parti_handl, (pCylogConf->_clog_block_index * CYLOG_BLOCK_SIZE), CYLOG_BLOCK_SIZE);
    
    if ( ret != 0 ) {
        CY_LOGI("--------%s(), line:%d, erase flash", __func__, __LINE__);
    } else {
        CY_LOGI("--------%s(), line:%d, erase flash OK", __func__, __LINE__);
    }
}

/** 
 * @brief 遍历日志区块，记录区块的尾部偏移量，尾部日志时间戳。
 *        累计映射flash为 CYLOG_BLOCK_SIZE
 * @param pConf 区块配置结构体
 * @param blockIndex  区块的索引
 * @param pBlockInfo 区块的信息变量指针
 * @return void
 */
void cylog_block_traversal(void *pConf, uint8_t blockIndex, void *pBInfo) {
    cylog_partition_t *pCylogConf = pConf;
    cylog_block_info_t *pBlockInfo = pBInfo;

    //逻辑块的相对偏移量, 取值 0-CYLOG_BLOCK_SIZE
    uint16_t blockOffset = 0;
    // 映射内存段中，最后有效日志的尾部偏移量
    int16_t memOffset = 0;
    void *pMemData = NULL;
    uint16_t mMapSize = CYLOG_PARTI_MMAP_SIZE;
    // esp_partition_t mMapHandl;
    uint32_t mMapHandl;

    while((blockOffset + mMapSize) <= CYLOG_BLOCK_SIZE) {
        uint8_t ret = cylog_partition_mmap_wrapper(
            pCylogConf->_clog_parti_handl, 
            (blockIndex * CYLOG_BLOCK_SIZE) + blockOffset,  //计算分区的相对偏移量
            mMapSize,
            &pMemData, 
            &mMapHandl);

        if (ret < 0) 
            return;

#ifdef CONFIG_CYLOG_LOG_LEN_IMMUTA
        memOffset = _cylog_mem_traversal_len_immutable(pMemData, mMapSize, pBlockInfo, blockOffset, pCylogConf->_clog_fixed_len);
#endif

#ifdef CONFIG_CYLOG_LOG_LEN_MUTA
        memOffset = _cylog_mem_traversal_len_mutable(pMemData, mMapSize, pBlockInfo, blockOffset);
#endif
        cylog_partition_unmmap_wrapper(mMapHandl);

        blockOffset += memOffset;
        // pBlockInfo->_tail_offset = blockOffset;
        mMapSize = (CYLOG_PARTI_MMAP_SIZE + (mMapSize - memOffset));
        // vTaskDelay(pdMS_TO_TICKS(100));
        cylog_delay(100);
    }
}


/** 
 * @brief 读取日志区块范围全部日志
 *        累计映射flash为 CYLOG_BLOCK_SIZE
 * @param pConf 区块配置结构体
 * @param blockIndex  区块的索引
 * @param pBlockInfo 区块的信息变量指针
 * @return void
 */
void cylog_block_log_read(void *pConf, uint8_t blockIndex, cylog_read_cb_t logItemCb) {
    cylog_partition_t *pCylogConf = pConf;

    //逻辑块的相对偏移量, 取值 0-CYLOG_BLOCK_SIZE
    uint16_t blockOffset = 0;
    // 映射内存段中，最后有效日志的尾部偏移量
    int16_t memOffset = 0;
    void *pMemData = NULL;
    uint16_t mMapSize = CYLOG_PARTI_MMAP_SIZE;
    // esp_partition_t mMapHandl;
    uint32_t mMapHandl;

    while((blockOffset + mMapSize) <= CYLOG_BLOCK_SIZE) {
        int8_t ret = cylog_partition_mmap_wrapper(
            pCylogConf->_clog_parti_handl, 
            (blockIndex * CYLOG_BLOCK_SIZE) + blockOffset,  //计算分区的相对偏移量
            mMapSize,
            &pMemData, 
            &mMapHandl);

        if (ret < 0)  {
            CY_LOGI("failed to mmap flash.");
            return;
        }

#ifdef CONFIG_CYLOG_LOG_LEN_IMMUTA
        memOffset = _cylog_mem_read_len_immutable(pMemData, mMapSize, logItemCb, blockOffset, pCylogConf->_clog_fixed_len);
#endif

#ifdef CONFIG_CYLOG_LOG_LEN_MUTA
        // memOffset = _cylog_mem_read_len_mutable(pMemData, mMapSize, blockOffset);
#endif
        cylog_partition_unmmap_wrapper(mMapHandl);

        blockOffset += memOffset;
        mMapSize = (CYLOG_PARTI_MMAP_SIZE + (mMapSize - memOffset));
        cylog_delay(100);
    }
}

void _cylog_block_hit(void *pConf, uint16_t dataLen) {
    cylog_partition_t *pCylogConf = pConf;

    /** 计算日志长度未超出block */
    if ( (pCylogConf->_clog_block_offset + CYLOG_BLOCK_HOLE) <= CYLOG_BLOCK_SIZE) {
        // CY_LOGI( "%s(), hit block index:%u.offset:%u", __func__, pCylogConf->_clog_block_index, pCylogConf->_clog_block_offset);
        return;
    }

    /** 计算日志长度将超出block, 切换下一个block */
    pCylogConf->_clog_block_index = (pCylogConf->_clog_block_index + 1) % pCylogConf->_clog_block_count;
    pCylogConf->_clog_block_offset = 0;

    /** 选择新块，写前擦除*/
    cylog_block_erase(pCylogConf);
}

// void _cylog_partition_read_test(void *pConf) {
//     esp_err_t err;
//     cylog_partition_t *pCylogConf = pConf;
//     spi_flash_mmap_handle_t spi_handle;
//     char *pData = NULL;

//     err = esp_partition_mmap(pCylogConf->_clog_parti_handl, 0, 1024, SPI_FLASH_MMAP_DATA, &pData, &spi_handle);
//     if (err != ESP_OK) {
//         CY_LOGI("%s() read flash, err:%s", __func__, esp_err_to_name(err));
//     } else {
//         CY_LOGI("%s() read flash[0].[0]: 0x%x", __func__, pData[0]);
//     }
// }

/** 
 * @brief 遍历日志分区，填充 cylog_partition_t 全局变量
 * @return 成功返回0， 失败返回-1
 */
int8_t cylog_partition_write(void *pConf, uint8_t *pData, uint32_t dataLen) {
    cylog_partition_t *pCylogConf = pConf;

    /** 选择 block */
    _cylog_block_hit(pCylogConf, dataLen);

    uint32_t wOffset = pCylogConf->_clog_block_index * CYLOG_BLOCK_SIZE + pCylogConf->_clog_block_offset;
    // if (err = esp_partition_write(pCylogConf->_clog_parti_handl, wOffset, pData, dataLen), err == ESP_OK) 
    if (cylog_partition_write_wrapper(pCylogConf->_clog_parti_handl, wOffset, pData, dataLen) == 0) 
    {
        pCylogConf->_clog_block_offset += dataLen;
        CY_LOGI("%s(), hit block index:%u.offset:%u", __func__, pCylogConf->_clog_block_index, wOffset);
    } else {
        CY_LOGI("------- %s(), failed to write flash.", __func__);
    }

    return 0;
}


/**
 * @brief 日志分区有多个块，每个块的大小是4K,
 *        读取每个块的可写位置偏移量，如果偏移量超出 (CYLOG_BLOCK_SIZE - CYLOG_BLOCK_HOLE)的块
 *        不参与比较。在可比较blockInfo中选择偏移量最小的块，返回其索引.
 *        如所有块的可写偏移量均超出(CYLOG_BLOCK_SIZE - CYLOG_BLOCK_HOLE),则表示所有的块均已写满，返回-1.
 * @param pBlockInfo, 各个块检索后的信息。
 * @param nBlockCont, 区块的数量.
 * @return 找到可写块，返回对应索引 >= 0。 未找到可写块返回 -1.
*/
static int8_t _cylog_block_soft_by_tail_offset(cylog_block_info_t *pBlockInfo, uint8_t nBlockCont) {
    int8_t   tmpBlockIndex = -1;
    uint16_t tmpTailOffset = (CYLOG_BLOCK_SIZE - CYLOG_BLOCK_HOLE);

    for(uint8_t i = 0; i < nBlockCont; i ++) {

        // CY_LOGI("------- %s() line:%d [ blkIdx:%d, tmpOffset:%u, blockOffset:%u", __func__, __LINE__, i, tmpTailOffset, pBlockInfo[i]._tail_offset);
        if (tmpTailOffset > pBlockInfo[i]._tail_offset) {
            tmpTailOffset = pBlockInfo[i]._tail_offset;
            tmpBlockIndex = i;
            break;
        }
    }
    // CY_LOGI("------- %s() line:%d [ blkIdx:%d, tmpOffset:%u, blockOffset:%u", __func__, __LINE__, tmpBlockIndex, tmpTailOffset, pBlockInfo[tmpBlockIndex]._tail_offset);
    return tmpBlockIndex;
}

/**
 * @brief 日志分区有多个块，每个块的大小是4K,
 *        读取每个块的尾部日志时间戳，选择时间戳最小的block，返回其blockindex。
 * @param pBlockInfo, 各个块检索后的信息。
 * @param nBlockCont, 区块的数量.
 * @return 找到可写块，返回块的索引 >= 0。未找到可写块返回 -1.
*/
static int8_t _cylog_block_soft_by_tail_timestamp(cylog_block_info_t *pBlockInfo, uint8_t nBlockCont) {
    int8_t   tmpBlockIndex = -1;
    uint32_t tmpTailTS = 0xffffffff;

    for(uint8_t i = 0; i < nBlockCont; i ++) {
        // CY_LOGI("------- %s() line:%d [ blkIdx:%d, tmpTs:%u, blockTs:%u", __func__, __LINE__, i, tmpTailTS, pBlockInfo[i]._ts);
        if (tmpTailTS > pBlockInfo[i]._ts) {
            tmpTailTS = pBlockInfo[i]._ts;
            tmpBlockIndex = i;
        }
    }

    CY_LOGI("------- %s() line:%d [ blkIdx:%d, tmpTs:%u, blockTs:%u", __func__, __LINE__, tmpBlockIndex, tmpTailTS, pBlockInfo[tmpBlockIndex]._ts);
    return tmpBlockIndex;
}

/** 
 * @brief 遍历日志分区，查找可写入的block。填充 cylog_partition_t 全局变量
 * @return 成功返回0， 失败返回-1
 */
int8_t cylog_block_select(void *pConf, cylog_block_info_t *pBlockInfo) {
    int8_t  blockIndex = -1;
    uint8_t tmpBlockIndex = 0;
    // uint32_t tmpTs = 0;
    cylog_partition_t *pCylogConf = pConf;

    if (blockIndex = _cylog_block_soft_by_tail_offset(pBlockInfo, pCylogConf->_clog_block_count), blockIndex >= 0) {
        pCylogConf->_clog_block_index = blockIndex;
        pCylogConf->_clog_block_offset = pBlockInfo[blockIndex]._tail_offset;

        if ( pCylogConf->_clog_block_offset == 0 ) {
            cylog_block_erase(pCylogConf);
        }

        return blockIndex;
    }

    if (blockIndex = _cylog_block_soft_by_tail_timestamp(pBlockInfo, pCylogConf->_clog_block_count), blockIndex >= 0) {
        pCylogConf->_clog_block_index = blockIndex;
        pCylogConf->_clog_block_offset = 0;//pBlockInfo[blockIndex]._tail_offset;

        cylog_block_erase(pCylogConf);
        return blockIndex;
    }

    return -1;
}
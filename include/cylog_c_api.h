#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "cylog_types.h"

/**
 * 初始化日志组件
 * @param rootDir: 日志根目录
 * 
 * @return :
 * 
 *  - 成功: true
 * 
 *  - 失败: false
 */
bool cylog_init(char *rootDir);

/**
 * 新建分类日志
 * @param  logType: 日志分类
 * @param  logPath: 存储分类日志的目录名称
 * @param  fSize:   单个日志文件的大小
 * @param  fCount:  分类日志，文件的数量
 * @param  pPrefix: str|NULL 日志文件名称前缀
 * @param  cb: 通过回调函数，将为消警的数据返回
 * @param  filter: (NULL|Non-NULL) 告警日志过滤器 | e.g 判别日志是否未消警
 * @return :
 * 
 *  - 成功: true
 * 
 *  - 失败: false
 */
// bool cylog_create(cylog_type_t logType, char *logPath, uint16_t fSize, uint16_t fCount, char *pPrefix);
bool cylog_create(cylog_type_t logType, uint16_t fSize, uint16_t fCount, cylog_traversal_cb_t cb, cylog_traversal_filter_t filter);

/**
 * 写日志数据
 * @param  logType: 日志分类
 * @param  pData: 待写数据
 * @param  dLen:  待写数据长度
 * @param  timeoutTms: 写时等待超时(ms)
 * 
 * @return :
 * 
 *  - 成功: true
 * 
 *  - 失败: false
 */
bool cylog_write(cylog_type_t logType, uint8_t pData[], uint16_t dLen, uint32_t timeoutTms );

#ifdef __cplusplus
}
#endif

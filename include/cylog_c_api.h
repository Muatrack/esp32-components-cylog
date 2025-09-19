#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * 遍历日志后，以回调的方式将日志内容返回。 定义读取日志的回调函数类型
 * @param data: uint8_t[] 数据的字节序列
 * @param dataLen: 数据的长度
 * @return 
 *  - -1: 终止遍历
 * 
 *  -  0: 继续便利
*/
typedef int (log_read_cb_t)(uint8_t data[], uint16_t dataLen );


/** 测试用，函数入口 */
void test_alarm_log();

#ifdef __cplusplus
}
#endif
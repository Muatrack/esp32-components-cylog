#pragma once

#ifdef USE_SYSTEM_LINUX
    #include <cstdint>
#else
    #include <stdint.h>
#endif

/** 
 * 定义日志返回值
 */

using CL_TYPE_t = uint16_t;

/** 通用定义 */
#define CL_OK                  00   // 执行成功
#define CL_FILE_NOT_EXIST      01   // 文件不存在
#define CL_FILE_FULL           02   // 文件已写满
#define CL_DIR_NOT_EXIST       10    // 目录不存在
#define CL_PARAM_INVALID       20    // 参数无效
#define CL_EXCP_UNKNOW         0xff    // 未知异常

/** 读取日志 */

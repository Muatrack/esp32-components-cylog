#pragma once

#ifdef USE_SYSTEM_LINUX
    #include <unistd.h>
#else
    #include <stdint.h>
#endif

/** 
 * 定义日志返回值
 */

typedef uint16_t CL_TYPE_t;

/** 通用定义 */
#define CL_OK                  0   // 执行成功
#define CL_FILE_NOT_EXIST      1   // 文件不存在
#define CL_FILE_FULL           2   // 文件已写满

/** 读取日志 */


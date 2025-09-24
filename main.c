#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cylog_c_api.h>

extern void alarm_log_test();
extern void cylog_dir_del( char *path );

int main(void) {
    printf("------------------- Hi, CYLOG ----------------------\n");

    // cylog_dir_del( "/tmp/logroot" );

    /* 初始化日志目录结构 */
    
    alarm_log_test();
}

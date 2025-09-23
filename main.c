#include <stdio.h>
#include <stdlib.h>

extern void alarm_log_test();
extern void cylog_dir_del( char *path );

int main(void) {
    printf("------------------- Hi, CYLOG ----------------------\n");

    cylog_dir_del( "/sdb/logroot" );

    /* 初始化日志目录结构 */
    
    alarm_log_test();
}
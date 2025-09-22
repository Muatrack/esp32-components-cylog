#include <stdio.h>
#include <stdlib.h>

extern void test_alarm_log();
// extern void test_dir_del(  );

extern void alarm_log_test();
extern void test_dir_del_c_api( char *path );

int main(void) {
    printf("------------------- Hi, CYLOG ----------------------\n");

    test_dir_del_c_api( "/sdb/logroot" );

    // std::cout << "Hi, CYLOG" << std::endl;
    // std::cout << "-------- dir removed testing ---------" << std::endl;
    // test_dir_del("excp/b");
    // test_alarm_log();

    /* 初始化日志目录结构 */
    
    alarm_log_test();
}
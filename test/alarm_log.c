
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <cylog_c_api.h>

#if 1
int cylog_alarm_traversal_cb(uint8_t data[], uint16_t dataLen ) {
    static uint16_t counter = 0;
    printf("[ alarm ], counter:%d\n", ++counter);
    return 0;
}

// cylog_traversal_filter_t filter
int cylog_alarm_traversal_filter(uint8_t data[], uint16_t dataLen) {
    return 1;
}
#endif

void alarm_log_test() {

    cylog_init("/sdb/logroot");

    cylog_create( CYLOG_T_ALARM, 40960, 1, NULL, NULL);

    cylog_alarm_t alarm = { .base.circuit_ID = 1, 0 };

    uint16_t remainCount = 1000;
    uint32_t seq = 1;
    while ( remainCount-->0 ) {
        alarm.createTm = (uint32_t)time(NULL);
        alarm.seq = seq ++;
        alarm.type = 0xF0;
        alarm.val = (seq+2)%4096;

        cylog_write(CYLOG_T_ALARM, (uint8_t*)&alarm, sizeof(alarm), 10);
        usleep(100000);
    }
    
}
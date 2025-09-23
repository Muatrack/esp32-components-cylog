
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <cylog_c_api.h>

/* 告警日志的回调与过滤 */
int cylog_alarm_traversal_cb(uint8_t data[], uint16_t dataLen ) {
    if(data==NULL) { goto done; }

    cylog_alarm_t *pAlarm = (cylog_alarm_t *)data;
    printf("got an expected alarm log with create time:%u\n", pAlarm->base.createTm );
done:
    return 0;
}

int cylog_alarm_traversal_filter(uint8_t data[], uint16_t dataLen) {
    if(data==NULL) { goto ignore; }

    cylog_alarm_t *pAlarm = (cylog_alarm_t *)data;
    if(pAlarm->type!=ALARM_T_3) { goto ignore; }

    return 1;
ignore:
    return 0;
}
// 告警日志的回调与过滤

void alarm_log_test() {

    cylog_init("/sdb/logroot");

    cylog_create( CYLOG_T_ALARM, 40960, 1, NULL, NULL);

    cylog_alarm_t alarm = { .base.circuit_ID = 1, 0 };

    uint16_t remainCount = 1000;
    uint32_t seq = 1;
    while ( remainCount-->0 ) {
        alarm.base.createTm = (uint32_t)time(NULL);
        alarm.seq = seq ++;
        if(remainCount%600==0)  alarm.type = ALARM_T_3;
        else alarm.type = ALARM_T_2;
        alarm.val = (seq+2)%4096;

        cylog_write(CYLOG_T_ALARM, (uint8_t*)&alarm, sizeof(alarm), 10);
        usleep(100000);
    }
    
}
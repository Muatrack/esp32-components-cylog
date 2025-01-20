/** 
 *  类相关的各种测试
 */

#include <iostream>
#include <memory>
#include "private_include/cylog_factory.hpp"
#include "private_include/cylog_impl_alarm.hpp"

#ifdef USE_SYSTEM_LINUX
#include <stdint.h>
#endif

extern void test_adv_prointer();

using namespace std;

void alarm_log_test() {
    
    /** do test */
    test_adv_prointer();
    //  do test

    CYLogFactoryAbs *pFactory     = new CyLogFactoryAlarm();
    CYLogImplAbs    *pAlarmLog    = pFactory->createLog( "/tmp/a" );

    FileContent fc;
    if( pAlarmLog != nullptr ) {
        pAlarmLog->create();
        pAlarmLog->read("/dddfa", fc);
        pAlarmLog->write("ddddfa", fc);
        pAlarmLog->remove("dfadfa");
    } else {
        std::cout << "pAlarmLog is NULL" << std::endl;
    }

    delete pAlarmLog;
    delete pFactory;
}


void test_cls1() {
    alarm_log_test();
}
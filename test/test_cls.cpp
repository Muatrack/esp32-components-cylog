/** 
 *  类相关的各种测试
 */

#include <iostream>
#include <memory>
#include <unistd.h>
#include "private_include/cylog_factory.hpp"
#include "private_include/cylog_impl_alarm.hpp"
#include "private_include/cylog_store_linux.hpp"

#ifdef USE_SYSTEM_LINUX
#include <stdint.h>
#endif

extern void test_adv_prointer();

using namespace std;

void alarm_log_test() {

    uint8_t _dataBuf[64] = { 0x0 };
    /** do test */
    #if 0
    test_adv_prointer();
    return;
    #endif
    //  do test

    std::cout<< __func__<< "()." << __LINE__ << std::endl;
    std::shared_ptr<StoreAbs> pStore = std::make_shared<StoreLinux>();
    CYLogFactoryAbs *pFactory     = new CyLogFactoryAlarm();
    CYLogImplAbs    *pAlarmLog    = pFactory->createLog( "/tmp/a/", pStore );

    std::cout<< __func__<< "()." << __LINE__ << std::endl;

    if( pAlarmLog != nullptr ) {
        pAlarmLog->logInit();
        pAlarmLog->create();
        // pAlarmLog->read("/dddfa", fc);
        for( int i=0;i < (int)sizeof(_dataBuf); i ++ ) { _dataBuf[i] = i; }

    #if 1
        for( int i=0; i < 200; i ++ ) {
            pAlarmLog->write(_dataBuf, sizeof(_dataBuf));
            sleep(1);
        }
    #endif

        // pAlarmLog->remove("dfadfa");
    } else {
        std::cout << "pAlarmLog is NULL" << std::endl;
    }

    delete pAlarmLog;
    delete pFactory;
}


void test_cls1() {
    alarm_log_test();
}
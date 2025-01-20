/** 
 *  类相关的各种测试
 */

#include <iostream>
#include <memory>
#include "private_include/cylog_factory.hpp"
#include "private_include/cylog_impl_alarm.hpp"
#include "private_include/cylog_store_linux.hpp"

#ifdef USE_SYSTEM_LINUX
#include <stdint.h>
#endif

extern void test_adv_prointer();

using namespace std;

void alarm_log_test() {
    
    /** do test */
    // test_adv_prointer();
    //  do test
    std::shared_ptr<std::string> alarmLogDir    = std::make_shared<std::string>("/tmp/a");
    std::shared_ptr<std::string> alarmLogPrefix = std::make_shared<std::string>("alarm_");

    std::shared_ptr<StoreAbs> pStore = std::make_shared<StoreLinux>( 0, 0, alarmLogDir, alarmLogPrefix );
    CYLogFactoryAbs *pFactory     = new CyLogFactoryAlarm();
    CYLogImplAbs    *pAlarmLog    = pFactory->createLog( "/tmp/a/", pStore );

    void* fc = nullptr;
    if( pAlarmLog != nullptr ) {
        pAlarmLog->logInit();
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
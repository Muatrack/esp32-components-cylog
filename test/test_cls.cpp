/** 
 *  类相关的各种测试
 */

#include <iostream>
#include <memory>
#include <unistd.h>
#include "private_include/cylog_factory.hpp"
#include "private_include/cylog_store_linux.hpp"
#include "cylog_impl/alarm/cylog_impl_alarm.hpp"
#include "cylog_impl/excp/cylog_impl_excp.hpp"


#ifdef USE_SYSTEM_LINUX
#include <stdint.h>
#endif

#ifdef  CYLOG_MAX_RW_CUROPTS    /* 定义同时执行读写操作的数量 */
    #define STORE_CURR_OPTS_COUNT CYLOG_MAX_RW_CUROPTS
#else
    #define STORE_CURR_OPTS_COUNT 1 /*缺省值*/
#endif
#define STORE_LOG_ROOT_DIR  "/tmp/logroot/"     /* 日志根目录 */


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

    std::string rootPath = STORE_LOG_ROOT_DIR;
    std::cout<< __func__<< "()." << __LINE__ << std::endl;
    StoreAbs::StoreInit(STORE_CURR_OPTS_COUNT, rootPath);
    std::shared_ptr<StoreAbs> pStore = std::make_shared<StoreLinux>();
    CYLogFactoryAbs *pAlarmFactory     = new CyLogFactoryAlarm();
    CYLogFactoryAbs *pExcpFactory      = new CyLogFactoryExcp();
    CYLogImplAbs    *pAlarmLog    = pAlarmFactory->create( "/tmp/alarm/", pStore );
    CYLogImplAbs    *pExcpLog     = pExcpFactory->create( "/tmp/excp/", pStore );

    std::cout<< __func__<< "()." << __LINE__ << std::endl;

    if( pAlarmLog != nullptr ) {
        pAlarmLog->logInit();
        // pAlarmLog->create();

        pExcpLog->logInit();
        // pExcpLog->create();

        // pAlarmLog->read("/dddfa", fc);
        for( int i=0;i < (int)sizeof(_dataBuf); i ++ ) { _dataBuf[i] = i; }

    #if 1
        for( int i=0; i < 60; i ++ ) {
            pAlarmLog->write(_dataBuf, sizeof(_dataBuf));
            usleep(500000);
            pExcpLog->write(_dataBuf, sizeof(_dataBuf));
            usleep(500000);
        }
    #endif

        // pAlarmLog->remove("dfadfa");
    } else {
        std::cout << "pAlarmLog is NULL" << std::endl;
    }

    delete pAlarmLog;
    delete pAlarmFactory;
}


void test_cls1() {
    alarm_log_test();
}
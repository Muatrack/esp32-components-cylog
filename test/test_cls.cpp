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
    std::cout<< "-------------------------------------------" << __func__<< "()." << __LINE__ << "-------------------------------------------" << std::endl;
    StoreAbs::StoreInit(STORE_CURR_OPTS_COUNT, rootPath);
    std::shared_ptr<StoreAbs> pStore = std::make_shared<StoreLinux>();
    CYLogFactoryAbs *pAlarmFactory     = new CyLogAlarmFactory();
    CYLogFactoryAbs *pExcpFactory      = new CyLogExcpFactory();
    CYLogImplAbs    *pAlarmLog    = pAlarmFactory->create( pStore, "alarm", "alm", 1024, 4 );
    CYLogImplAbs    *pExcpLog     = pExcpFactory->create(  pStore, "excp", "ex", 1024, 4 );

    std::cout<< __func__<< "()." << __LINE__ << std::endl;

    // 写入日志测试
    {
        // 初始化数组
        for( int i=0;i < (int)sizeof(_dataBuf); i ++ ) { _dataBuf[i] = i; }
    #if 1
        std::unique_ptr<uint8_t[]> alarmItemArray;
        std::unique_ptr<uint8_t[]> excpItemArray;
        std::cout << std::endl << "---------------------- Gonna write log to file ----------------------" << std::endl;

        uint8_t dLen = 12;
        for( int i=0; i < 128; i ++ ) {
            alarmItemArray = std::make_unique<uint8_t[]>(dLen);
            for( int j=0; j<dLen; j ++ ) { alarmItemArray[j] = i + 1 + j + 1; }
            pAlarmLog->write( std::move(alarmItemArray), dLen);
            if((i>0)&&(i%16==0)) {  sleep(3); }
            else usleep(100000);
            // pExcpLog->write( std::move(excpItemArray),  sizeof(_dataBuf));
            // usleep(200000);
        }
    #endif
    }

    delete pAlarmLog;
    delete pAlarmFactory;
}


void test_cls1() {
    alarm_log_test();
}

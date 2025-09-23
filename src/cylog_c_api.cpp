

/** 
 *  类相关的各种测试
 */
#include <iostream>
#include <memory>
#include <unistd.h>
#include "private_include/cylog_factory.hpp"
#include "cylog_impl/alarm/cylog_impl_alarm.hpp"
#include "cylog_impl/excp/cylog_impl_excp.hpp"
#include "cylog_impl/pmeter_quarter/cylog_impl_pmeter_quarter.hpp"
#include "cylog_impl/pmeter_day/cylog_impl_pmeter_day.hpp"
#include "cylog_impl/power/cylog_impl_power.hpp"
#include "cylog_impl/switch/cylog_impl_switch.hpp"

#include <stdint.h>

#ifdef USE_SYSTEM_LINUX
    #include "private_include/cylog_store_linux.hpp"
#else
    #include "private_include/cylog_store_espidf.hpp"
#endif

#ifdef  CYLOG_RW_CONOPTS    /* 定义同时执行读写操作的数量 */
    #define STORE_CURR_OPTS_COUNT CYLOG_RW_CONOPTS
#else
    #define STORE_CURR_OPTS_COUNT 1 /*缺省值*/
#endif

#ifdef CYLOG_ROOT_DIR
    #define STORE_ROOT_DIR  CYLOG_ROOT_DIR
#else
    #define STORE_ROOT_DIR  "/tmp/logroot/"     /* 日志根目录 */
#endif

extern void test_adv_prointer();

using namespace std;

static std::shared_ptr<StoreAbs> m_pStore = nullptr;

#define CYLOG_INIT_CHECK(gt){   \
    if( m_pStore==nullptr ) {   \
        CYLOG_PRINT(  std::cout<<"cylog un-init"<<std::endl );  \
        goto gt;    \
    }   \
}

typedef struct {
    CYLogImplAbs *pLogImpl; // 日志对象
}cylog_session_t;

static cylog_session_t m_logSession[ CYLOG_T_DEF ] = { nullptr };

/* 
    判断 cylog_type_t 是否有效 
    * 如无效，则跳至 gt
*/
#define CYLOG_TYPE_CHECK(t,gt) if( (t<CYLOG_T_ALARM)||(t>=CYLOG_T_DEF) ) {goto gt;}

/* 检查分类日志的对象是否有效*/
#define CYLOG_TOBJ_CHECK(t,gt) if( !m_logSession[t].pLogImpl ) {goto gt;}

/**
 * 获得分类日志对象
 * @param t: 日志类别 cylog_type_t 
 * @return: (CYLogImplAbs*) 类型
*/
#define CYLOG_TOBJ_TAKE(t) ({   \
    CYLogImplAbs *pSp = nullptr; \
    CYLOG_TYPE_CHECK(t,_excp);  \
    pSp=m_logSession[t].pLogImpl; \
_excp:  \
    pSp;    \
})

/*******************************************************************************/

extern "C"
bool cylog_init(char *rootDir) {

    std::string rootPath = rootDir;
    CYLOG_PRINT(  std::cout<< "-------------------------------------------" << __func__<< "()." << __LINE__ << "-------------------------------------------" << std::endl );

    /* 检查文件系统是否已初始化 */
    if( std::filesystem::exists("/sdb/init")==false ) { /* 文件系统未初始化 */
        CYLOG_INIT_CHECK(excp); /* 借助已有宏定义，跳出 */
    }

    if( !rootDir ) { goto excp; }   /* 参数无效 */    
    StoreAbs::StoreInit(STORE_CURR_OPTS_COUNT, rootPath);
    if( m_pStore ) { goto done; } /* 已初始化 */

#ifdef USE_SYSTEM_LINUX
    m_pStore = std::make_shared<StoreLinux>();
#else
    m_pStore = std::make_shared<StoreEspidf>();
#endif

done:
    return (m_pStore!=nullptr);
excp:
    return false;
}

extern "C"
// bool cylog_create(cylog_type_t logType, char *logPath, uint16_t fSize, uint16_t fCount, char *pPrefix) {
bool cylog_create(cylog_type_t logType, uint16_t fSize, uint16_t fCount, cylog_traversal_cb_t cb, cylog_traversal_filter_t filter) {

    CYLogFactoryAbs *pFactory = nullptr;
    std::string logPrefix;

    CYLOG_INIT_CHECK(excp);

    /* 检查日志类型是否有效, 如无效则调至 label:excp */
    CYLOG_TYPE_CHECK( logType, excp );

    /* 检查当前分类的日志，如已初始化则直接跳过 */
    if(m_logSession[logType].pLogImpl) { goto done; }

    switch (logType) {
        case CYLOG_T_ALARM:     pFactory    = new CyLogAlarmFactory();          break;
        case CYLOG_T_EXCP:      pFactory    = new CyLogExcpFactory();           break;
        case CYLOG_T_PMETE_QTR: pFactory    = new CyLogPMeterQuarterFactory();  break;
        case CYLOG_T_PMETE_DAY: pFactory    = new CyLogPMeterDayFactory();      break;
        case CYLOG_T_POWER:     pFactory    = new CyLogPowerFactory();          break;
        case CYLOG_T_SWITCH:    pFactory    = new CyLogSwitchFactory();         break;
        default:  goto excp;
    }

    if( !pFactory ) { goto excp; }

    /* 对于有效的日志类型，当其日志对象为空， 为其新建日志对象 */
    if( logType==CYLOG_T_ALARM ) {
        m_logSession[logType].pLogImpl = pFactory->create( m_pStore, fSize, fCount, nullptr, nullptr);
    } else {
        m_logSession[logType].pLogImpl = pFactory->create( m_pStore, fSize, fCount);
    }

done:
    if( pFactory ) { delete pFactory; }
    return true;
excp:
    if( pFactory ) { delete pFactory; }
    return false;
}

extern "C"
bool cylog_write(cylog_type_t logType, uint8_t pData[], uint16_t dLen, uint32_t timeoutTms ) {

    std::unique_ptr<uint8_t[]> pDPtr = nullptr;
    CYLogImplAbs *pLogObj = nullptr;

    CYLOG_INIT_CHECK(excp);

    if( (!pData)||(dLen<1)) { goto excp; }
    /* 检查日志类型是否有效, 如无效则跳至 label:excp */
    CYLOG_TYPE_CHECK( logType, excp );

    /* 检查分类日志对象是否有效，如无效则跳至 label:excp */
    CYLOG_TOBJ_CHECK( logType, excp );

    pDPtr = std::make_unique<uint8_t[]>(dLen);
    std::memcpy( pDPtr.get(), pData, dLen );

    if(pLogObj=CYLOG_TOBJ_TAKE(logType),!pLogObj) {goto excp;}
    pLogObj->write( std::move(pDPtr), dLen, timeoutTms);

    return true;
excp:
    return false;
}

extern "C"
void cylog_dir_del( char *path ) {

    std::string rootPath = std::string(path);

    CYLOG_INIT_CHECK(excp);

    if( m_pStore ) { goto opt; }
    StoreAbs::StoreInit(STORE_CURR_OPTS_COUNT, rootPath);
    #ifdef USE_SYSTEM_LINUX
        m_pStore = std::make_shared<StoreLinux>();
    #else
        m_pStore = std::make_shared<StoreEspidf>();
    #endif
opt:
    // m_pStore->dirDelete(rootPath+"/"+path);
    m_pStore->dirDelete(path);
excp:;
}


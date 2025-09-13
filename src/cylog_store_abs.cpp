
#include "private_include/cylog_store_abs.hpp"

#ifdef  CYLOG_MAX_RW_CUROPTS
    #define STORE_CURR_OPTS_COUNT CYLOG_MAX_RW_CUROPTS
#else
    #define STORE_CURR_OPTS_COUNT 1 /*缺省值*/
#endif

sem_t   StoreAbs::m_signal;

void StoreAbs::StoreInit() {
    sem_init(&StoreAbs::m_signal, 0, STORE_CURR_OPTS_COUNT);
}

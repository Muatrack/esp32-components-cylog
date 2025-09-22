#include "private_include/cylog_factory.hpp"
#include "cylog_impl_alarm.hpp"

using namespace CLFile;

extern "C" __attribute__((weak)) int cylog_alarm_traversal_cb(uint8_t data[], uint16_t dataLen );
extern "C" __attribute__((weak)) int cylog_alarm_traversal_filter(uint8_t data[], uint16_t dataLen);

CL_TYPE_t CYLogAlarmImpl::traverse(cylog_traversal_cb_t cb=nullptr) {
    if( cb==nullptr ) { goto excp; }

    while( 0 ) {
        if( cb(NULL, 0)<0 ) { break; }
    }

    return CL_OK;
excp:
    return CL_PARAM_INVALID;
}

CYLogAlarmImpl::CYLogAlarmImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc ):
                                                                                            CYLogImplAbs( store, std::move(pFDesc) ) 
{};

/*************************************************** Factory ******************************************************/

CYLogImplAbs* CyLogAlarmFactory::create(std::shared_ptr<StoreAbs> &store, std::string logDir, uint32_t  fileSize, uint8_t fileCount, std::string prefix, cylog_traversal_cb_t cb, cylog_traversal_filter_t filter) {
    std::cout << "CyLogAlarmFactory::create, prefix: "<< prefix << std::endl;
    std::unique_ptr<CLFile::FileDesc> pFDesc = nullptr;

    /** 建立文件对象 */
    if(cylog_alarm_traversal_cb && cylog_alarm_traversal_filter) {
        pFDesc = std::make_unique<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount, 64, cylog_alarm_traversal_cb, cylog_alarm_traversal_filter);
    } else {
        pFDesc = std::make_unique<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount);
    }

    return new CYLogAlarmImpl(logDir, store, std::move(pFDesc) );
}

CYLogImplAbs * CyLogAlarmFactory::create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount, cylog_traversal_cb_t cb, cylog_traversal_filter_t filter) {
    return create(store, "alarm", fileSize, fileCount, "alm", cb, filter);
}

CYLogImplAbs * CyLogAlarmFactory::create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount) {
    return create(store, "alarm", fileSize, fileCount, "alm", nullptr, nullptr);
}

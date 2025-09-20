#include "private_include/cylog_factory.hpp"
#include "cylog_impl_alarm.hpp"

using namespace CLFile;

CL_TYPE_t CYLogAlarmImpl::traverse(log_read_cb_t cb=nullptr) {
    if( cb==nullptr ) { goto excp; }

    while( 0 ) {
        if( cb(NULL, 0)<0 ) { break; }
    }

    return CL_OK;
excp:
    return CL_PARAM_INVALID;
}

CYLogAlarmImpl::CYLogAlarmImpl(const std::string & dir, std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc ):
                                                                                            CYLogImplAbs( store, std::move(pFDesc) ) {};
/*************************************************** Factory ******************************************************/

CYLogImplAbs* CyLogAlarmFactory::create(std::shared_ptr<StoreAbs> &store, std::string logDir, uint32_t  fileSize, uint8_t fileCount, std::string prefix) {
    std::cout << "CyLogAlarmFactory::create, prefix: "<< prefix << std::endl;
    
    /** 建立文件对象 */
    std::unique_ptr<CLFile::FileDesc> pFDesc = std::make_unique<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount);
    return new CYLogAlarmImpl(logDir, store, std::move(pFDesc) );
}

CYLogImplAbs * CyLogAlarmFactory::create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount) {
    return create(store, "alarm", fileSize, fileCount, "alm");
}

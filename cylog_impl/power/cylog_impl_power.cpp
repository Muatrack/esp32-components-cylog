#include "private_include/cylog_factory.hpp"
#include "cylog_impl_power.hpp"

CL_TYPE_t CYLogPowerImpl::traverse(cylog_traversal_cb_t cb) {
    if( cb==nullptr ) { goto excp; }

    while( 0 ) {
        if( cb(NULL, 0)<0 ) { break; }
    }

    return CL_OK;
excp:
    return CL_PARAM_INVALID;
}

CYLogPowerImpl::CYLogPowerImpl(const std::string & logDir, std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc ):
                                                                                            CYLogImplAbs( store, std::move(pFDesc) ) {}

/******************************************************* Factory *********************************************************/

CYLogImplAbs* CyLogPowerFactory::create(std::shared_ptr<StoreAbs> &store, std::string logDir, uint32_t  fileSize, uint8_t fileCount, std::string prefix, cylog_traversal_cb_t cb, cylog_traversal_filter_t filter) {
    std::cout << "CyLogExcpFactory::create" << std::endl;

    /** 建立文件对象 */
    std::unique_ptr<CLFile::FileDesc> pFDesc = std::make_unique<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount);
    return new CYLogPowerImpl(logDir, store, std::move(pFDesc));
}

CYLogImplAbs * CyLogPowerFactory::create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount) {
    return create(store, "power", fileSize, fileCount, "pw", nullptr, nullptr);
}
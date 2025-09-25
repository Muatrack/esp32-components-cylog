#include "private_include/cylog_factory.hpp"
#include "cylog_impl_switch.hpp"

CL_TYPE_t CYLogSwitchImpl::traverse(cylog_traversal_cb_t cb) {
    if( cb==nullptr ) { goto excp; }

    while( 0 ) {
        if( cb(NULL, 0)<0 ) { break; }
    }

    return CL_OK;
excp:
    return CL_PARAM_INVALID;
}

CYLogSwitchImpl::CYLogSwitchImpl(const std::string & logDir, std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc ):
                                                                                            CYLogImplAbs( store, std::move(pFDesc) ) {}

/******************************************************* Factory *********************************************************/

CYLogImplAbs* CyLogSwitchFactory::create(std::shared_ptr<StoreAbs> &store, std::string logDir, uint32_t  fileSize, uint8_t fileCount, std::string prefix, cylog_traversal_cb_t cb, cylog_traversal_filter_t filter) {
    CYLOG_PRINT( std::cout<<"[ TESTCASE_CYLOG ] " << "CyLogExcpFactory::create" << std::endl );

    /** 建立文件对象 */
    std::unique_ptr<CLFile::FileDesc> pFDesc = std::make_unique<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount);
    return new CYLogSwitchImpl(logDir, store, std::move(pFDesc));
}

CYLogImplAbs * CyLogSwitchFactory::create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount) {
    return create(store, "switch", fileSize, fileCount, "sw", nullptr, nullptr);
}
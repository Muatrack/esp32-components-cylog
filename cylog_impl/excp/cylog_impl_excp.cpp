#include "private_include/cylog_factory.hpp"
#include "cylog_impl_excp.hpp"

extern "C" __attribute__((weak)) int cylog_excp_traversal_cb(uint8_t data[], uint16_t dataLen );
extern "C" __attribute__((weak)) int cylog_excp_traversal_filter(uint8_t data[], uint16_t dataLen);

CL_TYPE_t CYLogExcpImpl::traverse(cylog_traversal_cb_t cb=nullptr) {
    if( cb==nullptr ) { goto excp; }

    while( 0 ) {
        if( cb(NULL, 0)<0 ) { break; }
    }

    return CL_OK;
excp:
    return CL_PARAM_INVALID;
}

CYLogExcpImpl::CYLogExcpImpl(const std::string & logDir, std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc ):
                                                                                            CYLogImplAbs( store, std::move(pFDesc) ) {}

/******************************************************* Factory *********************************************************/

CYLogImplAbs* CyLogExcpFactory::create(std::shared_ptr<StoreAbs> &store, std::string logDir, uint32_t  fileSize, uint8_t fileCount, std::string prefix, cylog_traversal_cb_t cb, cylog_traversal_filter_t filter) {
    std::cout << "CyLogExcpFactory::create" << std::endl;
    std::unique_ptr<CLFile::FileDesc> pFDesc = nullptr;

    /** 建立文件对象 */
    if(cylog_excp_traversal_cb && cylog_excp_traversal_filter) {
        std::cout << "[ CyLogExcpFactory ] traversal_cb and filter are not defined"<<std::endl;
        pFDesc = std::make_unique<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount, 64, cylog_excp_traversal_cb, cylog_excp_traversal_filter);
    } else {
        std::cout << "[ CyLogExcpFactory ] traversal_cb and filter are defined"<<std::endl;
        pFDesc = std::make_unique<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount);
    }

    return new CYLogExcpImpl(logDir, store, std::move(pFDesc));
}

CYLogImplAbs * CyLogExcpFactory::create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount) {
    return create(store, "excp", fileSize, fileCount, "ex", nullptr, nullptr);
}
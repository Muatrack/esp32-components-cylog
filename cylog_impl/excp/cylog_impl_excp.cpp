#include "private_include/cylog_factory.hpp"
#include "cylog_impl_excp.hpp"

CL_TYPE_t CYLogExcpImpl::write( std::unique_ptr<uint8_t[]> pIn, uint16_t iLen) {
    // 将日志数据序列化
    std::unique_ptr<ItemDesc> pItem = ItemDesc::itemSerialize( std::move(pIn), iLen);

    // 将序列化后的数据写入文件
    storeGet()->itemWrite( m_pFDesc, pItem->packData(), iLen+4);
    return CL_OK;
}

// void CYLogExcpImpl::logInit() {
//     /* 读取告警日志目录所有的文件，记录文件的数量，和当前可写文件及其位置 */
//    std::cout << "CYLogExcpImpl::logInit()" << std::endl;
// //    storeGet()->init();
// }

CL_TYPE_t CYLogExcpImpl::traverse(log_read_cb_t cb) {
    if( cb==nullptr ) { goto excp; }

    while( 0 ) {
        if( cb(NULL, 0)<0 ) { break; }
    }

    return CL_OK;
excp:
    return CL_PARAM_INVALID;
}

CYLogExcpImpl::CYLogExcpImpl(const std::string & logDir, std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc ):
                                                                                            CYLogImplAbs( store, std::move(pFDesc) ) {    
    std::cout << "CYLogExcpImpl instance created." << std::endl;
    m_Store->nextFileSelect( m_pFDesc );
}

/******************************************************* Factory *********************************************************/

CYLogImplAbs* CyLogExcpFactory::create(std::shared_ptr<StoreAbs> &store, std::string logDir, uint32_t  fileSize, uint8_t fileCount, std::string prefix) {
    std::cout << "CyLogExcpFactory::create" << std::endl;

    /** 建立文件对象 */
    std::unique_ptr<CLFile::FileDesc> pFDesc = std::make_unique<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount);

    /** 建立日志绝对目录 */
    if( store->dirCreate(logDir)==CL_OK ) {
        std::cout << "CYLogAlarmImpl log directory " << store->rootDirGet()+"/"+pFDesc->relativePathGet() << " get ready." << std::endl;
    }

    /** 建立日志文件 */
    store->fileCreate(pFDesc, prefix, fileCount, fileSize);

    return new CYLogExcpImpl(logDir, store, std::move(pFDesc));
}

CYLogImplAbs * CyLogExcpFactory::create(std::shared_ptr<StoreAbs> &store, uint32_t  fileSize, uint8_t fileCount) {
    return create(store, "excp", fileSize, fileCount, "ex");
}
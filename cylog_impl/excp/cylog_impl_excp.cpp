#include "private_include/cylog_factory.hpp"
#include "cylog_impl_excp.hpp"

CL_TYPE_t CYLogExcpImpl::write(const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen) {
    storeGet()->itemWrite( m_pFDesc, pIn, iLen );
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

    std::vector<std::string> fList;
    m_Store->dirTraverse( m_pFDesc, fList );

    std::cout << "TESTCASE_dirtraverse | EXCP | size: " << fList.size()<<std::endl;
    for( auto & p: fList ) {
        std::cout << "TESTCASE_dirtraverse | EXCP | " << static_cast<std::string>(p)<<std::endl;
    }
}

/******************************************************* Factory *********************************************************/

CYLogImplAbs* CyLogExcpFactory::create(std::shared_ptr<StoreAbs> &store, std::string logDir, std::string prefix, uint32_t  fileSize, uint8_t fileCount) {
    std::cout << "CyLogExcpFactory::create" << std::endl;
    
    /** 建立文件对象 */
    std::unique_ptr<CLFile::FileDesc> pExcpFD = std::make_unique<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount);

    /** 建立日志绝对目录 */
    std::string alarmLogAbsPath = store->rootDirGet() + "/" + logDir;
    if( store->dirCreate(alarmLogAbsPath)==CL_OK ) {
        std::cout << "CYLogAlarmImpl log directory " << alarmLogAbsPath << " get ready." << std::endl;
    }

    /** 建立日志文件 */
    store->fileCreate(alarmLogAbsPath, prefix, fileCount, fileSize);

    return new CYLogExcpImpl(logDir, store, std::move(pExcpFD));
}
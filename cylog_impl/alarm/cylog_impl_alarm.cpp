#include "private_include/cylog_factory.hpp"
#include "cylog_impl_alarm.hpp"

using namespace CLFile;

CL_TYPE_t CYLogAlarmImpl::write(const std::unique_ptr<uint8_t[]> & pIn, uint16_t iLen){
    // std::cout << "Alarm write." << std::endl;
    /* 新建 */

    // std::unique_ptr<ItemDesc> pItem = ItemDesc::itemSerialize(pIn, iLen);
    storeGet()->itemWrite( m_pFDesc, pIn, iLen);
    return CL_OK;
}

// void CYLogAlarmImpl::logInit() {
//     /* 读取告警日志目录所有的文件，记录文件的数量，和当前可写文件及其位置 */
//    std::cout << "CYLogAlarmImpl::logInit()" << std::endl;
// //    storeGet()->init();
// }

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
                                                                                            CYLogImplAbs( store, std::move(pFDesc) ) {

    /** 遍历日志目录下的文件 */
    std::vector<std::string> fList;
    m_Store->dirTraverse( m_pFDesc, fList );
    std::cout << "TESTCASE_dirtraverse | ALARM | size: " << fList.size()<<std::endl;
    std::unique_ptr<uint8_t[]> pBuf = std::make_unique<uint8_t[]>(32);
    for( auto & p: fList ) {
        std::cout << "TESTCASE_dirtraverse | ALARM | " << static_cast<std::string>(p)<<std::endl;

        m_Store->fileTraverse( p, pBuf, 4 );
        std::cout << "Alarm file:" << static_cast<uint8_t>(pBuf[0]) << std::endl;
    }

    std::cout << "TESTCASE_dirtraverse | ALARM 1" << std::endl;
    /**
     * 遍历日志，找到可写文件的相对路径和可写offset 
     * 
     * - 对每个日志文件，遍历其中的全部数据,判定日志文件是否已满
    */
    std::unique_ptr<ItemDesc> item = ItemDesc::itemDeSerialize(std::move(pBuf), 8);
    
    if( item->isValid() ) std::cout << "Item valid" << std::endl;
    else std::cout << "Item invalid" << std::endl;
    std::cout << "Item len:" << item->itemSizeGet() << std::endl;

    // std::unique_ptr<uint8_t[]> pBuf = std::make_unique<uint8_t[]>(32);
    // for( auto & p: fList ) {
    //     m_Store->fileTraverse( p, pBuf, 4 );
    //     std::cout << "Alarm file:" << static_cast<uint8_t>(pBuf[0]) << std::endl;
    // }
}

/*************************************************** Factory ******************************************************/

CYLogImplAbs* CyLogAlarmFactory::create(std::shared_ptr<StoreAbs> &store, std::string logDir, std::string prefix, uint32_t  fileSize, uint8_t fileCount) {
    std::cout << "CyLogAlarmFactory::create" << std::endl;
    
    /** 建立文件对象 */
    std::unique_ptr<CLFile::FileDesc> pFileDesc = std::make_unique<CLFile::FileDesc>(logDir, prefix, fileSize, fileCount);

    /** 建立日志绝对目录 */
    std::string alarmLogAbsPath = store->rootDirGet() + "/" + logDir;
    if( store->dirCreate(alarmLogAbsPath)==CL_OK ) {
        std::cout << "CYLogAlarmImpl log directory " << alarmLogAbsPath << " get ready." << std::endl;
    }

    if( pFileDesc ) std::cout << "TESTCASE_NULLPTR " <<__func__<< "(). "<< __LINE__ << " isn't nullptr" << std::endl;
    else            std::cout << "TESTCASE_NULLPTR " <<__func__<< "(). "<< __LINE__ << " is nullptr" << std::endl;
    /** 建立日志文件 */
    store->fileCreate(alarmLogAbsPath, "alm", fileCount, fileSize);

    if( pFileDesc ) std::cout << "TESTCASE_NULLPTR " <<__func__<< "(). "<< __LINE__ << " isn't nullptr" << std::endl;
    else            std::cout << "TESTCASE_NULLPTR " <<__func__<< "(). "<< __LINE__ << " is nullptr" << std::endl;

    return new CYLogAlarmImpl(logDir, store, std::move(pFileDesc) );
}


#include <private_include/cylog_impl.hpp>

extern "C" __attribute__((weak)) uint32_t global_cylog_create_ts_get(uint8_t pData[], uint16_t dLen);

CYLogImplAbs::CYLogImplAbs( std::shared_ptr<StoreAbs> &store, std::unique_ptr<CLFile::FileDesc> pFDesc ):m_Store(store), m_pFDesc(std::move(pFDesc)){
    
    /** 建立日志绝对目录 */
    if( m_Store->dirCreate(m_pFDesc->relativePathGet())==CL_OK ) {
        CYLOG_PRINT( std::cout<<"[ TESTCASE_CYLOG ] " << "CYLogAlarmImpl log directory " << m_Store->rootDirGet()+"/"+m_pFDesc->relativePathGet() << " get ready." << std::endl );
    }

    /** 建立日志文件 */
    m_Store->fileCreate(m_pFDesc);

    /** 选择下一个写日志的文件 */
    m_Store->nextFileSelect( m_pFDesc );
};

bool CYLogImplAbs::write( std::unique_ptr<uint8_t[]> pIn, uint16_t iLen, uint32_t timeoutTms) {
    // 将日志数据序列化
    std::unique_ptr<ItemDesc> pItem = ItemDesc::itemSerialize( std::move(pIn), iLen);
    // 将序列化后的数据写入文件
    return (storeGet()->itemWrite( m_pFDesc, pItem->packData(), iLen+4, timeoutTms)==CL_OK);
}

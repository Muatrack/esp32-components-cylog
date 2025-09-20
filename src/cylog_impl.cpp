
#include <private_include/cylog_impl.hpp>

CL_TYPE_t CYLogImplAbs::write( std::unique_ptr<uint8_t[]> pIn, uint16_t iLen) {
    // 将日志数据序列化
    std::unique_ptr<ItemDesc> pItem = ItemDesc::itemSerialize( std::move(pIn), iLen);

    // 将序列化后的数据写入文件
    storeGet()->itemWrite( m_pFDesc, pItem->packData(), iLen+4);
    return CL_OK;
}
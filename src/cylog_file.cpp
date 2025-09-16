
// #pragma once

#include "private_include/cylog_file.hpp"

namespace CLFile {
/************************************ ItemDesc *************************************/

#if 1
/**
 * 序列化
 * @param pData: 需要打包的日志原始数据
 * @param dLen:  需要打包的日志数据长度
 * @return 返回 ItemDesc对象指针
**/
std::unique_ptr<ItemDesc>  ItemDesc::itemSerialize(std::unique_ptr<uint8_t[]> pData, uint16_t dLen){
    return std::make_unique<ItemDesc>(std::move(pData), dLen, false);
};

/** 
 * 实例化 
 * @param pData: 从文件中读取到的包含HEAD的一条完整记录
 * @param dLen:  从文件中读取到的数据长度
 * @return 返回 ItemDesc对象指针
**/
std::unique_ptr<ItemDesc>  ItemDesc::itemDeSerialize(std::unique_ptr<uint8_t[]> pData, uint16_t dLen) {
    return std::make_unique<ItemDesc>(std::move(pData), dLen, true);
};

#endif


} // namespace CLFile
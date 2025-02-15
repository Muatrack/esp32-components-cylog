#pragma once
#include "common.hpp"

namespace CyLogUtils {

class Serializer {
public:
    Serializer() = delete;
    Serializer(Serializer&) = delete;
    Serializer operator=(Serializer&) = delete;
    ~Serializer() = delete;

    template<typename T>    
    static bool Serialize(const T & iData, uint32_t iDSize, uint8_t * poBytes);

    template<typename T>
    static bool Deserialize(const uint8_t * piBytes, uint32_t iBSize, const T & oData);

};

template<typename T>
bool Serializer::Serialize(const T & iData, uint32_t iDSize, uint8_t * poBytes) {
    if( (iDSize < 1) || (poBytes == NULL) ) { 
        goto excp; 
    };

    for( uint32_t i=0; i<iDSize; i++ ) {
        poBytes[i] = ((uint8_t*)&iData)[i];
    } // end

    return true;
excp:
    return false;
}

template<typename T>
bool Serializer::Deserialize(const uint8_t * piBytes, uint32_t iBSize, const T & oData) {
    if( (iBSize < 1) || (piBytes == NULL) ) { 
        goto excp; 
    };

    for( uint32_t i=0;i < iBSize; i ++ ) {
        ((uint8_t*)&oData)[i] = piBytes[i];
    }

    return true;
excp:
    return false;
}


}
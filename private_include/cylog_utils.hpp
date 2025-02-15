#pragma once

namespace CyLogUtils {

class Serializer {
public:
    static void Serialize(const char* log);
    static void Deserialize(char* log);
};

}
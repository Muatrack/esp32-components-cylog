cmake_minimum_required(VERSION 3.16)
project(cylog)

set(
    SRCS        
        src/cylog_impl_alarm.cpp
        src/cylog_store_linux.cpp
)

SET(CMAKE_BUILD_TYPE "Debug")
if(CMAKE_BUILD_TYPE)
    SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
    SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") 
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_executable(cylog ${SRCS}
                         main.cpp
                         test/test_cls.cpp
                         test/test_adv_prointer.cpp
                        )
    target_sources(cylog PRIVATE main.cpp)    
    add_compile_definitions(USE_SYSTEM_LINUX)
    message("CYLOG_SYSTEM is Linux")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Generic")
    add_library(cylog STATIC ${SRCS})
    add_compile_definitions(USE_SYSTEM_GENERIC)
    message("CYLOG_SYSTEM is generic")
else()
    message(ERROR "No system matched")
endif()

target_include_directories(cylog
                PRIVATE         ./
                INTERFACE       include
)

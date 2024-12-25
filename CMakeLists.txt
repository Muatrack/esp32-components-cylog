cmake_minimum_required(VERSION 3.16)
project(cylog)
# PRIV_REQUIRES cylog_esp32_wrapper

set(SRCS 
        src/cylog.cpp
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_executable(cylog ${SRCS})
    add_compile_definitions(USE_SYSTEM_LINUX)
    message("CYLOG_SYSTEM is linux")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Generic")
    add_library(cylog STATIC ${SRCS})
    add_compile_definitions(USE_SYSTEM_GENERIC)
    message("CYLOG_SYSTEM is generic")
else()
    message(ERROR "No system matched")
endif()

target_include_directories(cylog  
                PRIVATE         priv    inc
                INTERFACE       inc
)

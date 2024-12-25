cmake_minimum_required(VERSION 3.16)
project(cylog)
# PRIV_REQUIRES cylog_esp32_wrapper

add_library(cylog STATIC 
# add_executable(cylog
                src/cylog.cpp
)

target_include_directories(cylog  
                PRIVATE         priv    inc
                INTERFACE       inc
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_compile_definitions(USE_SYSTEM_LINUX)
    message("CYLOG_SYSTEM is linux")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Generic")
    add_compile_definitions(USE_SYSTEM_GENERIC)
    message("CYLOG_SYSTEM is generic")
endif()

# 宏开关  USE_OS_LINUX, USE_OS_FREERTOS
add_compile_definitions(USE_OS_LINUX)
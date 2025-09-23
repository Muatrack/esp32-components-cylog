cmake_minimum_required(VERSION 3.20)
project(cylog)

SET(CMAKE_BUILD_TYPE "Debug")

set(
    SRCS
        cylog_impl/excp/cylog_impl_excp.cpp
        cylog_impl/alarm/cylog_impl_alarm.cpp
        cylog_impl/pmeter_day/cylog_impl_pmeter_day.cpp
        cylog_impl/pmeter_quarter/cylog_impl_pmeter_quarter.cpp
        cylog_impl/power/cylog_impl_power.cpp
        cylog_impl/switch/cylog_impl_switch.cpp
        src/cylog_impl.cpp
        src/cylog_store_abs.cpp
        src/cylog_file.cpp
        src/cylog_c_api.cpp
)

set(USE_ASSERTION true)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CYLOG_READ_SIZE)
    add_compile_definitions(CYLOG_TRAVERSAL_BLOCK_SIZE=CYLOG_READ_SIZE)
else()
    add_compile_definitions(CYLOG_TRAVERSAL_BLOCK_SIZE=4096)
endif()

# 定义文件数据结构的版本
add_compile_definitions(CYLOG_FILE_VER=1)
# 定义文件判断是否已满时用到的空洞大小
add_compile_definitions(CYLOG_FILE_HOLE_SIZE=64)

if(CMAKE_BUILD_TYPE)
    SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
    SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") 
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_compile_definitions(USE_SYSTEM_LINUX)
    add_executable(cylog ${SRCS}
                         main.c
                         test/alarm_log.c
                         src/cylog_store_linux.cpp
                         src/cylog_store_espidf.cpp
                        )
    target_sources(cylog PRIVATE main.c)
    add_compile_definitions(USE_SYSTEM_LINUX)
    message("CYLOG_SYSTEM is Linux")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Generic")
    add_compile_definitions(USE_SYSTEM_FREERTOS)
    add_library(cylog STATIC ${SRCS}
                            src/cylog_store_espidf.cpp
    )
    message("CYLOG_SYSTEM is generic")
else()
    message(ERROR "No system matched")
endif()

include(./cmk_include/pre.cmake)

target_include_directories(cylog
                PRIVATE      ./
                PUBLIC       include
)
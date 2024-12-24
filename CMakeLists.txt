cmake_minimum_required(VERSION 3.16)

# PRIV_REQUIRES cylog_esp32_wrapper

add_library(cylog STATIC
                src/cylog_priv.c
                src/cylog.c
)

target_include_directories(cylog  
                PRIVATE         priv    inc
                INTERFACE       inc
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_compile_definitions(SYSTEM_LINUX)
    message("CYLOG_SYSTEM is linux")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Generic")
    add_compile_definitions(SYSTEM_GENERIC)
    message("CYLOG_SYSTEM is generic")
endif()


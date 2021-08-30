cmake_minimum_required(VERSION 3.12)

project(CortexClientBase)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/CortexCommon)

set(CORTEX_CLIENT_BASE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/client_base.cpp
)

add_library(CortexClientBase STATIC 
    ${CORTEX_CLIENT_BASE_SOURCES}
)

add_compile_definitions(CORTEX_CLIENT)

target_link_libraries(CortexClientBase 
    CortexCommon
)
target_include_directories(CortexClientBase PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/sources
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/CortexCommon/include
)
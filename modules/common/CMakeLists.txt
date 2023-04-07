
cmake_minimum_required(VERSION 3.15)

project(tte_common VERSION 0.0.0 LANGUAGES CXX)

add_library("${PROJECT_NAME}" INTERFACE)

set(
    include_files
    include/tte/common/assert.hpp
    include/tte/common/number_types.hpp
)

set(
    source_files
)

target_include_directories(
    "${PROJECT_NAME}"
    INTERFACE
    include
)

target_link_libraries(${PROJECT_NAME} INTERFACE warning_flags)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files} ${include_files})


cmake_minimum_required(VERSION 3.15)

project(tte_engine VERSION 0.0.0 LANGUAGES CXX)

set(
    include_files
    include/tte/engine/engine.hpp
)

set(
    source_files
    src/naive_engine.cpp
)

add_library(
    "${PROJECT_NAME}"
    STATIC
    ${source_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files} ${include_files})

target_include_directories(
    "${PROJECT_NAME}"
    PUBLIC
    include
)

target_link_libraries(
    "${PROJECT_NAME}"
    PUBLIC tte_common
    PRIVATE warning_flags)

if(APPLE)
    set_target_properties("${PROJECT_NAME}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()

if(TTE_UNIT_TEST)
    add_subdirectory(test)
endif()

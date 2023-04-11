
cmake_minimum_required(VERSION 3.15)

project(tte_platform_layer VERSION 0.0.0 LANGUAGES CXX)

set(
    include_files
    include/tte/platform_layer/platform_layer.hpp
)

set(
    source_files
    src/platform_layer.cpp
)

add_library(
    "${PROJECT_NAME}"
    STATIC
    ${source_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files} ${include_files})

target_link_libraries("${PROJECT_NAME}"
    PUBLIC
    tte_common
    PRIVATE
    SDL2::SDL2-static
    SDL2_image::SDL2_image-static
    SDL2_ttf::SDL2_ttf-static
    warning_flags
)

target_include_directories(
    "${PROJECT_NAME}"
    PUBLIC
    include
)

if(APPLE)
    set_target_properties("${PROJECT_NAME}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()

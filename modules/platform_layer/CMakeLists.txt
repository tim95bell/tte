
cmake_minimum_required(VERSION 3.15)

set(TTE_PLATFORM SDL CACHE STRING "SDL | MacOS")

project(tte_platform_layer VERSION 0.0.0 LANGUAGES CXX)

if(TTE_PLATFORM STREQUAL SDL)
    set(TTE_SDL TRUE)
    set(TTE_MACOS FALSE)
elseif(TTE_PLATFORM STREQUAL MacOS)
    set(TTE_SDL FALSE)
    set(TTE_MACOS TRUE)
    if(NOT APPLE)
        message(FATAL_ERROR "Must be on Apple to build MacOS platform layer")
    endif()
else()
    message(FATAL_ERROR "TTE_PLATFORM must be \"SDL\" OR \"MacOS\"")
endif()

set(
    include_files
    include/tte/platform_layer/platform_layer.hpp
)

if (TTE_SDL)
    set(
        source_files
        src/sdl/platform_layer.cpp
    )
elseif(TTE_MACOS)
    set(
        source_files
        src/macos/platform_layer.mm
    )
else()
    message(FATAL_ERROR "UNREACHABLE`")
endif()

add_library(
    "${PROJECT_NAME}"
    SHARED
    ${source_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files} ${include_files})

target_link_libraries("${PROJECT_NAME}"
    PUBLIC
    tte_common
    PRIVATE
    warning_flags
)

if (TTE_SDL)
    target_link_libraries("${PROJECT_NAME}"
        PRIVATE
        SDL2::SDL2-static
        SDL2_image::SDL2_image-static
        SDL2_ttf::SDL2_ttf-static
    )
elseif(TTE_MACOS)
    enable_language(OBJCXX)
    target_link_libraries("${PROJECT_NAME}" PRIVATE "-framework AppKit")
else()
    message(FATAL_ERROR "UNREACHABLE`")
endif()

target_compile_definitions("${PROJECT_NAME}" PUBLIC "TTE_SDL=$<BOOL:${TTE_SDL}>" "TTE_MACOS=$<BOOL:${TTE_MACOS}>")

target_include_directories(
    "${PROJECT_NAME}"
    PUBLIC
    include
)

if(APPLE)
    set_target_properties("${PROJECT_NAME}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()

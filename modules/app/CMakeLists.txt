
cmake_minimum_required(VERSION 3.15)

project(tte_app VERSION 0.0.0 LANGUAGES CXX)

#region app_lib
set(
    app_lib_source_files
    src/App.cpp
)

set(
    app_lib_include_files
    include/tte/app/App.hpp
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${app_lib_source_files} ${app_lib_include_files})

if(TTE_HOT_RELOAD)
    add_library(
        "${PROJECT_NAME}_lib"
        SHARED
        ${app_lib_source_files}
    )


    target_link_libraries("${PROJECT_NAME}_lib" PRIVATE tte_common tte_platform_layer tte_engine PUBLIC warning_flags)

    target_include_directories(
        "${PROJECT_NAME}_lib"
        PUBLIC
        include
    )
endif()
#endregion

# region app
set(
    app_source_files
    src/main.cpp
)

set(
    app_header_files
)

add_executable(
    "${PROJECT_NAME}"
    ${app_source_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${app_source_files} ${app_header_files})

target_link_libraries("${PROJECT_NAME}" PRIVATE tte_common tte_platform_layer tte_engine PUBLIC warning_flags)

target_include_directories(
    "${PROJECT_NAME}"
    PUBLIC
    include
)
# endregion

if(APPLE)
    set_target_properties("${PROJECT_NAME}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()

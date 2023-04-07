
cmake_minimum_required(VERSION 3.15)

project(tte_ui VERSION 0.0.0 LANGUAGES CXX)

set(
    header_files
)

set(
    source_files
    src/main.cpp
)

add_executable(
    "${PROJECT_NAME}"
    ${source_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files} ${header_files})

target_compile_features("${PROJECT_NAME}" PUBLIC cxx_std_17)

target_link_libraries("${PROJECT_NAME}" PRIVATE tte_common tte_platform_layer tte_engine PUBLIC warning_flags)

if(APPLE)
    set_target_properties("${PROJECT_NAME}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()

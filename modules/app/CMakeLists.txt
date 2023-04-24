
cmake_minimum_required(VERSION 3.15)

project(tte_app VERSION 0.0.0 LANGUAGES CXX)

set(tte_app_name tte)
set(tte_app_lib_name "${tte_app_name}_lib")
set(tte_hot_reload_name tte_hot_reload)

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
        "${tte_app_lib_name}"
        SHARED
        ${app_lib_source_files}
    )


    target_link_libraries("${tte_app_lib_name}" PRIVATE tte_common tte_platform_layer tte_engine PUBLIC warning_flags)

    target_include_directories(
        "${tte_app_lib_name}"
        PUBLIC
        include
    )

    set_target_properties("${tte_app_lib_name}" PROPERTIES OUTPUT_NAME "tte")
            
    add_custom_target("${tte_hot_reload_name}"
        DEPENDS "${tte_app_lib_name}"
        COMMAND ${CMAKE_COMMAND} -E rename
            "$<TARGET_FILE:${tte_app_lib_name}>"
            "$<TARGET_FILE:${tte_app_lib_name}>.copy")
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
    "${tte_app_name}"
    ${app_source_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${app_source_files} ${app_header_files})

target_link_libraries("${tte_app_name}" PRIVATE tte_common tte_platform_layer tte_engine PUBLIC warning_flags)

target_include_directories(
    "${tte_app_name}"
    PUBLIC
    include
)

if(TTE_HOT_RELOAD)
    add_dependencies(
        "${tte_app_name}"
        "${tte_hot_reload_name}"
    )
endif()
# endregion

if(APPLE)
    set_target_properties("${tte_app_name}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()

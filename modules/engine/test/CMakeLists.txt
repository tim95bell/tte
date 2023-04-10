
cmake_minimum_required(VERSION 3.15)

project(tte_engine_tests VERSION 0.0.0 LANGUAGES CXX)

enable_testing() 

set(
    test_files
    engine_tests.cpp
)

add_executable("${PROJECT_NAME}" ${test_files})

#target_link_libraries(
#    "${PROJECT_NAME}"
#    PRIVATE
#    gtest_main
#    tte
#)

add_test(
    NAME "${PROJECT_NAME}"
    COMMAND "${PROJECT_NAME}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main tte_engine)

if(APPLE)
        set_target_properties("${PROJECT_NAME}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()

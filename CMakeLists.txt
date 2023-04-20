
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

project(tte VERSION 0.0.0 LANGUAGES CXX)

set(BUILD_SHARED_LIBS FALSE)

option(TTE_UNIT_TEST "Build Unit Tests" TRUE)
option(TTE_WARNINGS_AS_ERRORS "Treat Warnings As Errors" TRUE)
option(TTE_WARNING_LEVEL_STRICT "Strict warnings" FALSE)

# TODO(TB): how to set SDL2_DIR, SDL2_image_DIR, SDL2_ttf_DIR for both debug and release?
#set(SDL2_DIR "./build/SDL2/$<IF:$<CONFIG:Debug>,Debug,Release>/lib/cmake/SDL2")
set(SDL2_DIR "./libs/SDL/Debug/lib/cmake/SDL2")
#set(SDL2_image_DIR "./build/SDL2_image/$<IF:$<CONFIG:Debug>,Debug,Release>/SDL2_image.framework/Resources")
set(SDL2_image_DIR "./libs/SDL_image/Debug/SDL2_image.framework/Resources")
set(SDL2_ttf_DIR "./libs/SDL_ttf/Debug/lib/cmake/SDL2_ttf")

add_compile_definitions(
    "TTE_DEBUG=$<BOOL:$<CONFIG:Debug>>"
)

if(TTE_UNIT_TEST)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(third_party/googletest)
endif()

find_package(SDL2 2.26.1 CONFIG REQUIRED)
find_package(SDL2_image 2.6.2 CONFIG REQUIRED)
find_package(SDL2_ttf 2.10.1 CONFIG REQUIRED)

include(cmake/warnings.cmake)
add_subdirectory(modules/common)
add_subdirectory(modules/engine)
add_subdirectory(modules/platform_layer)
add_subdirectory(modules/ui)

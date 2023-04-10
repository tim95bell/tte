
#!/bin/bash

cd $(dirname $0)
set -e

pushd ../third_party

# SDL
pushd SDL
sdl_build_dir="../../build/SDL/Debug"
sdl_debug_install_dir="../../libs/SDL/Debug"
sdl_release_install_dir="../../libs/SDL/Release"

cmake -B$sdl_build_dir -GXcode -DBUILD_SHARED_LIBS=FALSE

cmake --build $sdl_build_dir --config Debug
cmake --install $sdl_build_dir --config Debug --prefix $sdl_debug_install_dir

cmake --build $sdl_build_dir --config Release
cmake --install $sdl_build_dir --config Release --prefix $sdl_release_install_dir

popd

# SDL_image
pushd SDL_image

sdl_image_debug_build_dir="../../build/SDL_image/Debug"
sdl_image_debug_install_dir="../../libs/SDL_image/Debug"
cmake -B$sdl_image_debug_build_dir -GXcode -DBUILD_SHARED_LIBS=FALSE -DSDL2_DIR="../../libs/SDL/Debug/lib/cmake/SDL2" -DSDL2IMAGE_JPG=FALSE
cmake --build $sdl_image_debug_build_dir --config Debug
cmake --install $sdl_image_debug_build_dir --config Debug --prefix $sdl_image_debug_install_dir

sdl_image_release_build_dir="../../build/SDL_image/Release"
sdl_image_release_install_dir="../../libs/SDL_image/Release"
cmake -B$sdl_image_release_build_dir -GXcode -DBUILD_SHARED_LIBS=FALSE -DSDL2_DIR="../../libs/SDL/Release/lib/cmake/SDL2" -DSDL2IMAGE_JPG=FALSE
cmake --build $sdl_image_release_build_dir --config Release
cmake --install $sdl_image_release_build_dir --config Release --prefix $sdl_image_release_install_dir

popd

# SDL_ttf
pushd SDL_ttf

sdl_ttf_debug_build_dir="../../build/SDL_ttf/Debug"
sdl_ttf_debug_install_dir="../../libs/SDL_ttf/Debug"
cmake -B$sdl_ttf_debug_build_dir -GXcode -DBUILD_SHARED_LIBS=FALSE -DSDL2_DIR="../../libs/SDL/Debug/lib/cmake/SDL2" -DSDL2TTF_VENDORED=TRUE
cmake --build $sdl_ttf_debug_build_dir --config Debug
cmake --install $sdl_ttf_debug_build_dir --config Debug --prefix $sdl_ttf_debug_install_dir

sdl_ttf_release_build_dir="../../build/SDL_ttf/Release"
sdl_ttf_release_install_dir="../../libs/SDL_ttf/Release"
cmake -B$sdl_ttf_release_build_dir -GXcode -DBUILD_SHARED_LIBS=FALSE -DSDL2_DIR="../../libs/SDL/Release/lib/cmake/SDL2" -DSDL2TTF_VENDORED=TRUE
cmake --build $sdl_ttf_release_build_dir --config Release
cmake --install $sdl_ttf_release_build_dir --config Release --prefix $sdl_ttf_release_install_dir

popd

popd


#!/bin/bash

cd $(dirname $0)
set -e

pushd ../third_party

# SDL
pushd SDL
cmake -Bbuild -GXcode -DBUILD_SHARED_LIBS=FALSE

cmake --build build --config Debug
cmake --install build --config Debug --prefix ../../build/SDL/Debug

cmake --build build --config Release
cmake --install build --config Release --prefix ../../build/SDL/Release
popd

# SDL_image
pushd SDL_image
cmake -Bbuild/debug -GXcode -DBUILD_SHARED_LIBS=FALSE -DSDL2_DIR="../../build/SDL/Debug/lib/cmake/SDL2" -DSDL2IMAGE_JPG=FALSE
cmake --build build/debug --config Debug
cmake --install build/debug --config Debug --prefix ../../build/SDL_image/Debug

cmake -Bbuild/release -GXcode -DBUILD_SHARED_LIBS=FALSE -DSDL2_DIR="../../build/SDL/Release/lib/cmake/SDL2" -DSDL2IMAGE_JPG=FALSE
cmake --build build/release --config Release
cmake --install build/release --config Release --prefix ../../build/SDL_image/Release
popd

# SDL_ttf
pushd SDL_ttf

cmake -Bbuild/debug -GXcode -DBUILD_SHARED_LIBS=FALSE -DSDL2_DIR="../../build/SDL/Debug/lib/cmake/SDL2" -DSDL2TTF_VENDORED=TRUE
cmake --build build/debug --config Debug
cmake --install build/debug --config Debug --prefix ../../build/SDL_ttf/Debug

cmake -Bbuild/release -GXcode -DBUILD_SHARED_LIBS=FALSE -DSDL2_DIR="../../build/SDL/Release/lib/cmake/SDL2" -DSDL2TTF_VENDORED=TRUE
cmake --build build/release --config Release
cmake --install build/release --config Release --prefix ../../build/SDL_ttf/Release
popd

popd

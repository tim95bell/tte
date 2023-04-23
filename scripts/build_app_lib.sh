
#!/bin/bash

cd $(dirname $0)

pushd ..
cmake --build build/tte --target tte_app_lib
rm ./build/tte/modules/app/Debug/libtte_app_lib_2.dylib
cp ./build/tte/modules/app/Debug/libtte_app_lib.dylib ./build/tte/modules/app/Debug/libtte_app_lib_2.dylib
rm ./build/tte/modules/app/Debug/libtte_app_lib.dylib
popd

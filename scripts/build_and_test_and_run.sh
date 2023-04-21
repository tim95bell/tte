
#!/bin/bash

cd $(dirname $0)
set -e

pushd ..
#cmake -Bbuild/tte -GXcode -DTTE_UNIT_TEST=TRUE -DTTE_PLATFORM=SDL
cmake -Bbuild/tte -GXcode -DTTE_UNIT_TEST=TRUE -DTTE_PLATFORM=MacOS
cmake --build build/tte
./build/tte/modules/engine/test/Debug/tte_engine_tests
./build/tte/modules/app/Debug/tte_app
popd

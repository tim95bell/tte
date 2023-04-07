
#!/bin/bash

cd $(dirname $0)
set -e

pushd ..
cmake -Bbuild/tte -GXcode -DTTE_UNIT_TEST=TRUE
cmake --build build/tte
./build/tte/modules/engine/test/Debug/tte_engine_tests
popd

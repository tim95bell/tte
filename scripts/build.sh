
#!/bin/bash

cd $(dirname $0)
set -e

pushd ..
#cmake -Bbuild/tte -GXcode -DTTE_UNIT_TEST=TRUE -DTTE_PLATFORM=SDL
cmake -Bbuild/tte -GXcode -DTTE_UNIT_TEST=TRUE -DTTE_PLATFORM=MacOS -DTTE_WARNINGS_AS_ERRORS=FALSE -DTTE_HOT_RELOAD=TRUE
cmake --build build/tte
popd

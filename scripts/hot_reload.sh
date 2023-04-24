
#!/bin/bash
set -e

cd $(dirname $0)

pushd ..
cmake --build build/tte --target tte_hot_reload
popd


#!/bin/bash

cd $(dirname $0)

pushd ../modules

find . -iname *.h -o -iname *.c -o -iname *.cpp -o -iname *.hpp \
    | xargs clang-format -style=file -i -fallback-style=none

popd

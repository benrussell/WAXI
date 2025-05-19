#!/bin/bash
mkdir -p build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$WASI_SDK_PATH/share/cmake/wasi-sdk.cmake
cmake --build .

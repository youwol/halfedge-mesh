#!/bin/bash

if [ ! -d ".bin" ]
then
    mkdir bin
fi

emcc -o ./bin/mesh.js \
    -std=c++17 \
    -O3 \
    --bind \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MAXIMUM_MEMORY=4GB \
    -s WASM=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME=MeshModule \
    -s SINGLE_FILE=1 \
    -s ASSERTIONS=1 \
    -s NO_DISABLE_EXCEPTION_CATCHING \
    src/bindings.cxx \
    src/halfedge.cxx \
    
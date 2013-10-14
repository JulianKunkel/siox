#!/bin/bash -e

echo "Compiling the library and test manually"

pushd ..
g++ -g -std=c++11 module-loader.cpp -fPIC -shared -o libmodule-loader.so -I ../../include `pkg-config gmodule-2.0 --cflags --libs` 
popd

g++ -g -std=c++11  -fPIC -shared -o libtest-module.so -I ../../../include/ test-module.cpp

g++ -g -std=c++11  -fPIC  -I ../../../include/ main.cpp -o main -I . -L ../ -l module-loader


export LD_LIBRARY_PATH=../:.
./main

#!/bin/sh
# make clean, make操作


mkdir -p Debug
chmod 777 build.sh
cd Debug
rm -rf *
cd ..
./build.sh
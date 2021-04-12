#!/bin/sh


SVN_VERSION=0
CUR_DATE=`date "+%Y-%m-%d_%H:%M:%S"`
mkdir -p Debug
echo $SVN_VERSION
echo $CUR_DATE
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug -DVERSION_REVISION=$SVN_VERSION -D__DATE__=$CUR_DATE  ..
make -j9

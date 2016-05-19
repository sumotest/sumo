#!/bin/bash
apt-get install -y cmake
apt-get install -y libboost-dev
cd

cd
git clone https://cst.version.fz-juelich.de/jupedsim/jpscore.git
cd jpscore
git checkout as-a-service
mkdir build
cd build
cmake DCMAKE_BUILD_TYPE:STRING=Release ..
make -j4
make install
cd ../
rm -fr build


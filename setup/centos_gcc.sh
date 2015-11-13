#! /bin/bash

GCC_V='4.8.5'
sudo yum install -y glibc-static libstdc++-static

wget http://ftp.gnu.org/gnu/gcc/gcc-${GCC_V}/gcc-${GCC_V}.tar.gz -O gcc-${GCC_V}.tar.gz

tar xzf gcc-${GCC_V}.tar.gz
cd gcc-${GCC_V}
./contrib/download_prerequisites

cd ..
mkdir build_gcc${GCC_V}
cd build_gcc${GCC_V}
../gcc-${GCC_V}/configure --prefix=/usr/ --enable-checking=release --enable-languages=c,c++ --disable-multilib

make -j23
sudo make install

cd ..

rm -rf build_gcc${GCC_V} gcc-${GCC_V} gcc-${GCC_V}.tar.gz

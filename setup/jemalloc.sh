#!/bin/sh
ver="4.2.1"

wget https://github.com/jemalloc/jemalloc/archive/${ver}.tar.gz -O jemalloc.tar.gz

tar xvfz jemalloc.tar.gz

cd jemalloc-${ver}

# we must have installed autoconf

./autogen.sh

make dist
make

sudo make install


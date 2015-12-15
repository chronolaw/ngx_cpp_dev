#!/bin/sh
wget https://github.com/jemalloc/jemalloc/archive/4.0.4.tar.gz -O jemalloc.tar.gz

tar xvfz jemalloc.tar.gz

cd jemalloc-4.0.4

# we must have installed autoconf

./autogen.sh

make dist
make

sudo make install


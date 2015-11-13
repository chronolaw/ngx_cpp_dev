#!/bin/sh
wget -O boost.tar.gz http://downloads.sourceforge.net/project/boost/boost/1.59.0/boost_1_59_0.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F1.59.0%2F&ts=1447386812&use_mirror=jaist

tar xvfz boost.tar.gz

cd boost_1_59_0

./bootstrap.sh

sudo ./b2 link=static install


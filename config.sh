#!/bin/sh

ngx_path=$HOME/nginx
src_path=$HOME/ngx_cpp_dev

cd $ngx_path

./configure                                 \
    --with-cc-opt="-g -O0"                  \
    --add-module=$src_path/ngxpp            \
    --add-module=$src_path/modules/test     \
    --add-module=$src_path/modules/echo     \
    --add-module=$src_path/modules/datahook \
    --add-module=$src_path/modules/filter   \
    --add-module=$src_path/modules/upstream \
    --add-module=$src_path/modules/loadbalance \
    --add-module=$src_path/modules/subrequest \
    --add-module=$src_path/modules/shmem    \
    --add-module=$src_path/modules/variables

#    --with-ld-opt="-lrt"

cd -

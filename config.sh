#!/bin/sh

ngx_path=$HOME/nginx
src_path=$HOME/ngx_cpp_dev

build_date="build at `date '+%Y.%m.%d %H:%M'`"
cc_opt="-g -O0"

ngxpp_module="--add-module=${src_path}/ngxpp"

modules="test echo datahook filter \
         upstream loadbalance \
         subrequest \
         shmem variables \
         "

http_modules=""

for m in $modules
do
    http_modules="${http_modules} --add-module=${src_path}/http/${m} "
done

opts="${ngxpp_module}     \
      ${http_modules}
      "
cd $ngx_path

./configure                     \
    --build="${build_date}"     \
    --with-cc-opt="${cc_opt}"   \
    ${opts}

#    --with-ld-opt="-lrt"

cd -

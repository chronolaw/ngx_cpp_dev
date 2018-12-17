#!/bin/sh

ngx_path=$HOME/nginx
src_path=$HOME/github/ngx_cpp_dev

build_date="${USER} build at `date '+%Y.%m.%d %H:%M'`"
cc_opt="-g -O0"
common_opts="--with-threads --with-pcre-jit"
prefix="--prefix=/opt/nginx_cpp"

ngxpp_module="--add-module=${src_path}/ngxpp"

modules="test echo filter
         variables
         "

http_modules=""

for m in $modules
do
    http_modules="${http_modules} --add-module=${src_path}/http/${m} "
done

modules=""

event_modules=""

for m in $modules
do
    event_modules="${event_modules} --add-module=${src_path}/event/${m} "
done

opts="${prefix}
      ${common_opts}
      ${ngxpp_module}
      ${http_modules}
      ${event_modules}
      "
cd $ngx_path

./configure                     \
    --build="${build_date}"     \
    --with-cc-opt="${cc_opt}"   \
    ${opts}

#    --with-ld-opt="-lrt"

cd -

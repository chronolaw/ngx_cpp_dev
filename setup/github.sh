#!/bin/sh
# download openresty resources

github="~/github"

mkdir ${github}
cd ${github}

git clone git@github.com:simpl/ngx_devel_kit.git
git clone git@github.com:openresty/lua-nginx-module.git
git clone git@github.com:openresty/stream-lua-nginx-module.git

cd -


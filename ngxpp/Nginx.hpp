// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_COMMON_HEADERS_HPP
#define _NGX_COMMON_HEADERS_HPP

#include <nginx.h>      // for NGINX_VER...

extern "C" {

#include <ngx_http.h>

// in ngx_http_core_module.h
//#include <ngx_thread_pool.h>

#include <ngx_md5.h>
#include <ngx_sha1.h>
#include <ngx_murmurhash.h>
}

#define ngx_cpp_version 1001000
#define NGX_CPP_VERSION "1.1.0"

#endif  //_NGX_COMMON_HEADERS_HPP

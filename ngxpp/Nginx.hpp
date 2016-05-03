// Copyright (c) 2015-2016
// Author: Chrono Law
#ifndef _NGX_COMMON_HEADERS_HPP
#define _NGX_COMMON_HEADERS_HPP

#if __cplusplus < 201103L
    #error "ngx_cpp_module need C++11 implementation!"
#endif

extern "C" {

#include <ngx_http.h>

// in ngx_http_core_module.h
//#include <ngx_thread_pool.h>

}

#include <cassert>
#include <boost/core/ignore_unused.hpp>

#define ngx_cpp_version 1000000
#define NGX_CPP_VERSION "1.0.0"

#endif  //_NGX_COMMON_HEADERS_HPP

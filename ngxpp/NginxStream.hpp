// Copyright (c) 2016
// Author: Chrono Law
#ifndef _NGX_STREAM_HPP
#define _NGX_STREAM_HPP

#include <nginx.h>      // for NGINX_VER...

// should patch auto/modules to add macro NGX_STREAM
#if (nginx_version < 1009000) //&& (NGX_STREAM))
    #error "nginx 1.9.0 and later has stream module"
#endif  // nginx 1.9.0 && ngx_stream

extern "C" {

#include <ngx_stream.h>
#include <ngx_thread_pool.h>

}

#include <cassert>
#include <boost/core/ignore_unused.hpp>

#define ngx_stream_cpp_version 1000000
#define NGX_STREAM_CPP_VERSION "1.0.0"

#endif  // _NGX_STREAM_HPP


// Copyright (c) 2016-2017
// Author: Chrono Law
#ifndef _NGX_STREAM_ALL_HPP
#define _NGX_STREAM_ALL_HPP

// nginx headers
#include "NginxStream.hpp"

// cpp headers
#include "NgxCppInc.hpp"

// core classes in namespace ngx{}
#include "NgxCore.hpp"

// namespace define
#define NGX_STREAM_NAMESPACE_BEGIN      namespace ngx { namespace stream {
#define NGX_STREAM_NAMESPACE_END        }}
#define USING_STREAM_NAMESPACE          using namespace ngx::stream;

// stream classes
NGX_STREAM_NAMESPACE_BEGIN

#include "NgxModule.hpp"
#include "NgxStreamModule.hpp"

#include "NgxStreamSession.hpp"

// nginx 1.11.2 support stream variables
#if (nginx_version >= 1011002)
#include "NgxVariable.hpp"
#include "NgxComplexValue.hpp"
#endif

#if (nginx_version >= 1011005)
#include "NgxStreamFilter.hpp"
#endif //(nginx_version >= 1011005)

//#include "NgxFactory.hpp"

NGX_STREAM_NAMESPACE_END

// open namespace ngx::stream
USING_STREAM_NAMESPACE

#endif  // _NGX_STREAM_ALL_HPP


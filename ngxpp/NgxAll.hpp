// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_ALL_HPP
#define _NGX_ALL_HPP

// nginx headers
#include "Nginx.hpp"

// cpp headers
#include "NgxCppInc.hpp"

// core classes in namespace ngx{}
#include "NgxCore.hpp"

// namespace define
#define NGX_HTTP_NAMESPACE_BEGIN    namespace ngx { namespace http {
#define NGX_HTTP_NAMESPACE_END      }}
#define USING_HTTP_NAMESPACE        using namespace ngx::http;

// http classes
NGX_HTTP_NAMESPACE_BEGIN

#include "NgxModule.hpp"
#include "NgxHttpModule.hpp"

#include "NgxRequest.hpp"
#include "NgxFilter.hpp"
#include "NgxUpstream.hpp"
#include "NgxLoadBalance.hpp"
#include "NgxSubRequest.hpp"

#include "NgxVariable.hpp"
#include "NgxComplexValue.hpp"

//#include "NgxFactory.hpp"

NGX_HTTP_NAMESPACE_END

// open namespace ngx::http
USING_HTTP_NAMESPACE

#endif  //_NGX_ALL_HPP

// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_HTTP_SUBREQUEST_HPP
#define _NGX_HTTP_SUBREQUEST_HPP

#include "NgxPool.hpp"

template<typename T, ngx_http_phases ph>
class NgxSubRequestHandler final
{
public:
    typedef NgxSubRequestHandler this_type;
public:
    static ngx_int_t sub_post(ngx_http_request_t* r, void* data, ngx_int_t rc)
    try
    {
        r->parent->write_event_handler = ngx_http_core_run_phases;

        return T::sub_post(r, data, rc);
    }
    catch(const NgxException& e)
    {
        return e.code();
    }
};

template<typename T>
class NgxSubRequestHandler<T, NGX_HTTP_CONTENT_PHASE> final
{
public:
    typedef NgxSubRequestHandler this_type;
public:
    static ngx_int_t sub_post(ngx_http_request_t* r, void* data, ngx_int_t rc)
    try
    {
        r->parent->write_event_handler = &this_type::parent_post;

        return T::sub_post(r, data, rc);
    }
    catch(const NgxException& e)
    {
        return e.code();
    }

    static void parent_post(ngx_http_request_t* r)
    try
    {
        ngx_http_finalize_request(r, T::parent_post(r));
    }
    catch(...)
    {
        return;
    }
};

template<typename T, ngx_http_phases ph = NGX_HTTP_CONTENT_PHASE>
class NgxSubRequest final : public NgxWrapper<ngx_http_request_t>
{
public:
    typedef NgxWrapper<ngx_http_request_t> super_type;
    typedef NgxSubRequest this_type;
    typedef NgxSubRequestHandler<T, ph> this_handler;
public:
    NgxSubRequest(ngx_http_request_t* r):super_type(r)
    {}
    ~NgxSubRequest() = default;
public:
    ngx_http_request_t* create(ngx_str_t& uri,
                               ngx_str_t& args = nullargs(),
                               void* psr_data = nullptr,
                               ngx_uint_t flags = 0) const
    {
        auto psr = NgxPool(get()).alloc<ngx_http_post_subrequest_t>();

        psr->handler = &this_handler::sub_post;
        psr->data = psr_data;

        ngx_http_request_t* sr;

        auto rc = ngx_http_subrequest(
                get(), &uri, &args, &sr, psr, flags);
        NgxException::require(rc);      // create failed throw exception

        return sr;
    }
private:
    static ngx_str_t& nullargs()
    {
        static ngx_str_t s = ngx_null_string;
        return s;
    }
};

#endif  //_NGX_HTTP_SUBREQUEST_HPP

// Copyright (c) 2015-2017
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

template<typename T, ngx_http_phases ph = NGX_HTTP_CONTENT_PHASE, bool copy_vars = false>
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
                               ngx_str_t& args,
                               void* psr_data = nullptr,
                               ngx_uint_t flags = 0) const
    {
        return create(&uri, &args, psr_data, flags);
    }

    ngx_http_request_t* create(ngx_str_t& uri,
                               void* psr_data = nullptr,
                               ngx_uint_t flags = 0) const
    {
        return create(&uri, nullptr, psr_data, flags);
    }
public:
    ngx_http_request_t* create(ngx_str_t* uri,
                               ngx_str_t* args,
                               void* psr_data  = nullptr,
                               ngx_uint_t flags = 0) const
    {
        auto psr = NgxPool(get()).alloc<ngx_http_post_subrequest_t>();

        psr->handler = &this_handler::sub_post;
        psr->data = psr_data;

        ngx_http_request_t* r = get();
        ngx_http_request_t* sr;

        auto rc = ngx_http_subrequest(r, uri, args, &sr, psr, flags);
        NgxException::require(rc);      // create failed throw exception

        // fix nginx's small bug
        if (r->headers_in.headers.last == &r->headers_in.headers.part)
        {
            sr->headers_in.headers.last = &sr->headers_in.headers.part;
        }

        if(copy_vars)   //don't share vars with parent request
        {
            copy_variables(sr);
        }

        return sr;
    }
private:
    void copy_variables(ngx_http_request_t* sr) const
    {
        ngx_http_request_t* r = get();

        auto& cmcf = NgxHttpCoreModule::instance().conf().main(r);

        auto size = sizeof(ngx_http_variable_value_t) * cmcf.variables.nelts;

        sr->variables = NgxPool(sr).nalloc<ngx_http_variable_value_t>(size);

        ngx_memcpy(sr->variables, r->variables, size);
    }
};

#endif  //_NGX_HTTP_SUBREQUEST_HPP

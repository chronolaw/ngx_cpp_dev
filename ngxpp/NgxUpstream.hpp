// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_HTTP_UPSTREAM_HPP
#define _NGX_HTTP_UPSTREAM_HPP

#include "NgxException.hpp"
#include "NgxLog.hpp"
#include "NgxBuf.hpp"

class NgxUpstream final : public NgxWrapper<ngx_http_upstream_t>
{
public:
    typedef NgxWrapper<ngx_http_upstream_t> super_type;
    typedef NgxUpstream this_type;
public:
    NgxUpstream(ngx_http_upstream_t* u)
        :super_type(u)
    {}

    NgxUpstream(ngx_http_request_t* r):
        super_type(r->upstream)
    {}

    ~NgxUpstream() = default;
public:
    void conf(ngx_http_upstream_conf_t& cf) const
    {
        get()->conf = &cf;
        get()->buffering = cf.buffering;
    }

    void buffering(bool flag) const
    {
        get()->buffering = flag;
    }

    void request(ngx_chain_t* bufs) const
    {
        if(!get()->request_bufs)
        {
            get()->request_bufs = bufs;
            return;
        }

        NgxChain ch = get()->request_bufs;
        ch.append(bufs);
    }

    ngx_buf_t* buffer() const
    {
        return &get()->buffer;
    }
public:
    ngx_http_upstream_headers_in_t& headers() const
    {
        return get()->headers_in;
    }

    ngx_http_upstream_state_t& state() const
    {
        return *get()->state;
    }
};

template<
    ngx_int_t(*create_request)(ngx_http_request_t*) = nullptr,
    ngx_int_t(*process_header)(ngx_http_request_t*) = nullptr,
    void(*finalize_request)(ngx_http_request_t*,ngx_int_t) = nullptr
>
class NgxUpstreamHelper final : public NgxWrapper<ngx_http_request_t>
{
public:
    typedef NgxWrapper<ngx_http_request_t> super_type;
    typedef NgxUpstreamHelper this_type;
    typedef NgxUpstream upstream_type;
public:
    NgxUpstreamHelper(ngx_http_request_t* r):
        super_type(r), m_upstream(create())
    {}

    ~NgxUpstreamHelper() = default;
public:
    void conf(ngx_http_upstream_conf_t& cf) const
    {
        upstream().conf(cf);
    }

    ngx_int_t start(bool read_body = false) const
    {
        // set callback
        if(!upstream()->create_request)
        {
            upstream()->create_request = create_request;
            upstream()->process_header = process_header;
            upstream()->finalize_request = finalize_request;
        }

        // check callback function
        assert(upstream()->create_request);

        if(!upstream()->reinit_request)
        {
            upstream()->reinit_request = &this_type::default_reinit_request;
        }

        if(!upstream()->finalize_request)
        {
            upstream()->finalize_request = &this_type::default_finalize_request;
        }

        // init upstream
        if(read_body)
        {
            auto rc =
                ngx_http_read_client_request_body(get(), ngx_http_upstream_init);

            NgxException::fail(rc >= NGX_HTTP_SPECIAL_RESPONSE);
        }
        else
        {
            auto rc = ngx_http_discard_request_body(get());
            NgxException::require(rc);

            ++get()->main->count;
            ngx_http_upstream_init(get());
        }

        return NGX_DONE;
    }
public:
    const upstream_type& upstream() const
    {
        return m_upstream;
    }

private:
    upstream_type m_upstream;
private:
    ngx_http_upstream_t* create() const
    {
        if(!get()->upstream)
        {
            auto rc = ngx_http_upstream_create(get());

            NgxException::require(
                rc == NGX_OK, NGX_HTTP_INTERNAL_SERVER_ERROR);
        }

        return get()->upstream;
    }
private:
    static void default_finalize_request(ngx_http_request_t* r,ngx_int_t rc)
    {
        NgxLogDebug(r).print("default_finalize_request");
    }
    static ngx_int_t default_reinit_request(ngx_http_request_t* r)
    {
        NgxLogDebug(r).print("default_reinit_request");
        return NGX_OK;
    }
};

#endif  //_NGX_HTTP_UPSTREAM_HPP

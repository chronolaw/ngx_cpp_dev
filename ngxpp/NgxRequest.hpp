// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_HTTP_REQUEST_HPP
#define _NGX_HTTP_REQUEST_HPP

#include "NgxException.hpp"
#include "NgxString.hpp"
#include "NgxBuf.hpp"
#include "NgxHeaders.hpp"

class NgxRequestBody final : public NgxWrapper<ngx_http_request_t>
{
public:
    typedef NgxWrapper<ngx_http_request_t> super_type;
    typedef NgxRequestBody this_type;
public:
    NgxRequestBody(ngx_http_request_t* r):super_type(r)
    {}

    ~NgxRequestBody() = default;
public:
    ngx_chain_t* bufs() const
    {
        return get()->request_body?
               get()->request_body->bufs : nullptr;
    }
public:
    void discard() const
    {
        auto rc = ngx_http_discard_request_body(get());

        NgxException::require(rc);
    }

    template<typename F>
    ngx_int_t read(F f) const
    {
        auto rc = ngx_http_read_client_request_body(get(), f);

        NgxException::fail(rc >= NGX_HTTP_SPECIAL_RESPONSE, rc);

        return NGX_DONE;
    }
};

class NgxRequest final : public NgxWrapper<ngx_http_request_t>
{
public:
    typedef NgxWrapper<ngx_http_request_t> super_type;
    typedef NgxRequest this_type;

    typedef NgxHeadersIn headers_type;
    typedef NgxRequestBody body_type;
public:
    NgxRequest(ngx_http_request_t* r):super_type(r),
        m_headers(r), m_body(r)
    {}

    ~NgxRequest() = default;
public:
    // x maybe NGX_HTTP_GET|NGX_HTTP_HEAD|...
    bool method(ngx_uint_t x) const
    {
        return get()->method & x;
    }

    bool original() const
    {
        return get() == get()->main;
    }
public:
    const headers_type& headers() const
    {
        return m_headers;
    }

    const body_type& body() const
    {
        return m_body;
    }
public:
    ngx_str_t arg(const ngx_str_t& name) const
    {
        ngx_str_t value = ngx_null_string;

        ngx_http_arg(get(), name.data, name.len, &value);

        return value;
    }
public:
    void finalize(ngx_int_t rc = NGX_HTTP_INTERNAL_SERVER_ERROR) const
    {
        ngx_http_finalize_request(get(), rc);
    }
private:
    headers_type        m_headers;
    body_type           m_body;
};

class NgxResponse final : public NgxWrapper<ngx_http_request_t>
{
public:
    typedef NgxWrapper<ngx_http_request_t> super_type;
    typedef NgxResponse this_type;

    typedef NgxHeadersOut headers_type;
    typedef boost::string_ref string_ref_type;
public:
    NgxResponse(ngx_http_request_t* r):super_type(r),
        m_headers(r), m_pool(r)
    {}

    ~NgxResponse() = default;
public:
    const headers_type& headers() const
    {
        return m_headers;
    }

    void status(ngx_uint_t x) const
    {
        headers()->status = x;
    }

    void length(off_t len) const
    {
        headers()->content_length_n = len;
    }

public:
    ngx_uint_t status() const
    {
        return headers()->status;
    }

    off_t length() const
    {
        return headers()->content_length_n;
    }
public:
    void set_content_type() const
    {
        auto rc = ngx_http_set_content_type(get());

        NgxException::require(rc);
    }

    void clear_content_length() const
    {
        ngx_http_clear_content_length(get());
    }

    void clear_accept_ranges() const
    {
        ngx_http_clear_accept_ranges(get());
    }

public:
    ngx_int_t send() const
    {
        if(get()->header_sent)
        {
            return NGX_OK;
        }

        if(!headers()->status)
        {
            headers()->status = NGX_HTTP_OK;
        }

        auto rc = ngx_http_send_header(get());

        NgxException::fail(
            rc == NGX_ERROR || rc > NGX_OK, rc);

        return rc;
    }
public:
    ngx_int_t send(ngx_chain_t* out) const
    {
        send();      // send headers

        return out && !get()->header_only ?
               ngx_http_output_filter(get(), out) : NGX_OK;
    }

    ngx_int_t send(ngx_buf_t* buf) const
    {
        //buf->last_in_chain = true;

        NgxChainNode ch = m_pool.chain();
        ch.set(buf);
        //ch.finish();

        return send(ch);
    }

    ngx_int_t send(ngx_str_t* str) const
    {
        NgxBuf buf = m_pool.buffer();
        buf.range(str);

        return send(buf);
    }

    ngx_int_t send(string_ref_type str) const
    {
        auto s = m_pool.dup(str);
        return send(&s);
    }
public:
    ngx_int_t flush(/*bool sync = false*/) const
    {
        return ngx_http_send_special(get(), NGX_HTTP_FLUSH);
        //NgxBuf buf = m_pool.buffer();

        //buf->flush = true;
        //buf->sync = sync;
        //assert(buf.special());

        //return send(buf);
    }

    ngx_int_t eof() const
    {
        return ngx_http_send_special(get(), NGX_HTTP_LAST);
        //NgxBuf buf = m_pool.buffer();

        //buf.finish();
        //assert(buf.special());

        //return send(buf);
    }
public:
    void finalize(ngx_int_t rc = NGX_HTTP_OK) const
    {
        ngx_http_finalize_request(get(), rc);
    }
private:
    headers_type        m_headers;
    NgxPool             m_pool;
};

class NgxRequestCleanup final : public NgxWrapper<ngx_http_request_t>
{
public:
    typedef NgxWrapper<ngx_http_request_t> super_type;
    typedef NgxRequestCleanup this_type;
public:
    NgxRequestCleanup(ngx_http_request_t* r):super_type(r)
    {}

    ~NgxRequestCleanup() = default;
public:
    template<typename F>
    ngx_http_cleanup_t* add(F f, std::size_t n = 0) const
    {
        auto cln = ngx_http_cleanup_add(get(), n);

        NgxException::require(cln);

        cln->handler = f;

        if(!n)
        {
            cln->data = get();
        }

        return cln;
    }
};

#endif  //_NGX_HTTP_REQUEST_HPP


// Copyright (c) 2016
// Author: Chrono Law
#ifndef _NGX_STREAM_SESSION_HPP
#define _NGX_STREAM_SESSION_HPP

#include "NgxWrapper.hpp"

class NgxStreamSession : public NgxWrapper<ngx_stream_session_t>
{
public:
    typedef NgxWrapper<ngx_stream_session_t> super_type;
    typedef NgxStreamSession this_type;
public:
    NgxStreamSession(ngx_stream_session_t* s) : super_type(s)
    {}

    ~NgxStreamSession() = default;
public:
    NgxStreamSession(ngx_connection_t* c) :
        NgxStreamSession(reinterpret_cast<ngx_stream_session_t*>(c->data))
    {}

    NgxStreamSession(ngx_event_t* ev) :
        NgxStreamSession(reinterpret_cast<ngx_connection_t*>(ev->data))
    {}

public:
    ngx_connection_t* connection() const
    {
        return get()->connection;
    }
public:
#if nginx_version >= 1011005
    void send( ngx_chain_t *chain, ngx_uint_t from_upstream = true) const
    {
        auto rc = ngx_stream_top_filter(get(), chain, from_upstream);

        NgxException::fail(rc == NGX_ERROR);
    }
#endif

    void close(ngx_uint_t rc = NGX_STREAM_OK) const
    {
#if nginx_version >= 1011004
        ngx_stream_finalize_session(get(), rc);
#else
        boost::ignore_unused(rc);
        ngx_stream_close_connection(connection());
#endif
    }
};

#endif  // _NGX_STREAM_SESSION_HPP

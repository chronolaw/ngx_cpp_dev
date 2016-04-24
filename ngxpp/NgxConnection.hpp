// Copyright (c) 2016
// Author: Chrono Law
#ifndef _NGX_CONNECTION_HPP
#define _NGX_CONNECTION_HPP

#include "NgxWrapper.hpp"

class NgxConnection final : public NgxWrapper<ngx_connection_t>
{
public:
    typedef NgxWrapper<ngx_connection_t>    super_type;
    typedef NgxConnection                   this_type;
public:
    NgxConnection(ngx_connection_t* c) : super_type(c)
    {}

    NgxConnection(ngx_event_t *ev) :
        super_type(reinterpret_cast<ngx_connection_t*>(ev->data))
    {}

    ~NgxConnection() = default;
public:
    // for http modules
    NgxConnection(ngx_http_request_t* r) : super_type(r->connection)
    {}

public:
    ngx_http_request_t* http_request() const
    {
        return reinterpret_cast<ngx_http_request_t*>(get()->data);
    }
public:
    void read_handler(ngx_event_handler_pt h) const
    {
        get()->read->handler = h;
    }

    void read_timeout(ngx_msec_t timer) const
    {
        ngx_add_timer(get()->read, timer);
    }

    ngx_int_t read_event(ngx_uint_t flags = 0) const
    {
        return ngx_handle_read_event(get()->read, flags);
    }
public:
    void write_handler(ngx_event_handler_pt h) const
    {
        get()->write->handler = h;
    }

    void write_timeout(ngx_msec_t timer) const
    {
        ngx_add_timer(get()->write, timer);
    }

    ngx_int_t write_event(size_t lowat) const
    {
        return ngx_handle_write_event(get()->write, lowat);
    }
public:
    void close() const
    {
        ngx_close_connection(get());
    }

    void http_close() const
    {
        ngx_http_close_connection(get());
    }
};


#endif  // _NGX_CONNECTION_HPP

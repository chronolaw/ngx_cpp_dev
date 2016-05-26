// Copyright (c) 2016
// Author: Chrono Law
#ifndef _NGX_CONNECTION_HPP
#define _NGX_CONNECTION_HPP

#include <boost/type_traits.hpp>
#include <boost/tti/member_type.hpp>
#include <boost/utility/enable_if.hpp>

#include "NgxWrapper.hpp"
#include "NgxException.hpp"
#include "NgxEvent.hpp"

class NgxConnection final : public NgxWrapper<ngx_connection_t>
{
public:
    typedef NgxWrapper<ngx_connection_t>    super_type;
    typedef NgxConnection                   this_type;
public:
    NgxConnection(ngx_connection_t* c) : super_type(c)
    {}

    ~NgxConnection() = default;
public:
    NgxConnection(ngx_event_t *ev) :
        super_type(reinterpret_cast<ngx_connection_t*>(ev->data))
    {}

    // for http/stream modules
    template<typename T>
    NgxConnection(T* x) : super_type(x->connection)
    {}

public:
    BOOST_TTI_MEMBER_TYPE(wrapped_type)

    // for NgxSession/NgxRequest
    template<typename T>
    NgxConnection(const T& x,
           typename boost::enable_if<
                boost::tti::valid_member_type<
                    member_type_wrapped_type<T>>
                >::type* p = 0):
        NgxConnection(x.get())
    {}
public:
    // T = ngx_http_request_t/ngx_stream_session_t
    template<typename T>
    T* data() const
    {
        return reinterpret_cast<T*>(get()->data);
    }
public:
    ngx_event_t* read_event() const
    {
        return get()->read;
    }

    ngx_event_t* write_event() const
    {
        return get()->write;
    }
public:
    ssize_t recv(u_char *buf, size_t size) const noexcept
    {
        return get()->recv(get(), buf, size);
    }

    ssize_t send(u_char *buf, size_t size) const noexcept
    {
        return get()->send(get(), buf, size);
    }
public:
    ssize_t send(ngx_buf_t *buf) const noexcept
    {
        return send(buf->pos, ngx_buf_size(buf));
    }

    ngx_chain_t* send(ngx_chain_t *in, off_t limit = 0) const noexcept
    {
        return get()->send_chain(get(), in, limit);
    }
public:
    void free() const
    {
        ngx_free_connection(get());
    }

    void reusable(bool is_reusable) const
    {
        ngx_reusable_connection(get(), is_reusable);
    }
public:
    bool closed() const
    {
        return get()->close;
    }

    void close() const
    {
        ngx_close_connection(get());
    }

    //void http_close() const
    //{
    //    ngx_http_close_connection(get());
    //}
};


#endif  // _NGX_CONNECTION_HPP

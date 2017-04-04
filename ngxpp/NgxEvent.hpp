// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_EVENT_HPP
#define _NGX_EVENT_HPP

//#include "Nginx.hpp"
#include "NgxWrapper.hpp"
#include "NgxException.hpp"

class NgxEvent : public NgxWrapper<ngx_event_t>
{
public:
    typedef NgxWrapper<ngx_event_t> super_type;
    typedef NgxEvent this_type;
public:
    NgxEvent(ngx_event_t* ev) : super_type(ev)
    {}

    NgxEvent(ngx_event_t& ev) : super_type(ev)
    {}

    ~NgxEvent() = default;
public:
    template<typename T>
    void data(T* p) const
    {
        get()->data = p;
    }

    template<typename T>
    void data(T& v) const
    {
        get()->data = &v;
    }

public:
    template<typename T>
    T* data() const
    {
        return reinterpret_cast<T*>(get()->data);
    }

    ngx_connection_t* connection() const
    {
        return data<ngx_connection_t>();
    }
public:
    template<typename F>
    void handler(F f) const
    {
        get()->handler = f;
    }

    // ngx_nil = ngx_del_timer
    void timeout(ngx_msec_t timer, bool reset = false) const
    {
        if(get()->timer_set)
        {
            if(timer == ngx_nil)
            {
                ngx_del_timer(get());
                return;
            }

            if(!reset)
            {
                return;
            }
        }

        ngx_add_timer(get(), timer);
    }

public:
    bool ready() const
    {
        return get()->ready;
    }

    bool expired() const
    {
        return get()->timedout;
    }
public:
    void process() const
    {
        get()->handler(get());
    }
};

class NgxReadEvent final: public NgxEvent
{
public:
    typedef NgxEvent super_type;
    typedef NgxReadEvent this_type;
public:
    NgxReadEvent(ngx_event_t* ev) : super_type(ev)
    {
        assert(!ev->write);
    }

    NgxReadEvent(ngx_event_t& ev) : super_type(ev)
    {}

    ~NgxReadEvent() = default;
public:
    void wait(ngx_uint_t flags = 0) const
    {
        auto rc = ngx_handle_read_event(get(), flags);

        NgxException::require(rc);
    }

    void wait_for(ngx_msec_t timer, bool reset = false, ngx_uint_t flags = 0) const
    {
        timeout(timer, reset);
        wait(flags);
    }
};

class NgxWriteEvent final: public NgxEvent
{
public:
    typedef NgxEvent super_type;
    typedef NgxWriteEvent this_type;
public:
    NgxWriteEvent(ngx_event_t* ev) : super_type(ev)
    {
        assert(ev->write);
    }

    NgxWriteEvent(ngx_event_t& ev) : super_type(ev)
    {}

    ~NgxWriteEvent() = default;
public:
    void wait(size_t lowat = 0) const
    {
        auto rc = ngx_handle_write_event(get(), lowat);

        NgxException::require(rc);
    }

    void wait_for(ngx_msec_t timer, bool reset = false, size_t lowat = 0) const
    {
        timeout(timer, reset);
        wait(lowat);
    }
};
#endif  //_NGX_EVENT_HPP

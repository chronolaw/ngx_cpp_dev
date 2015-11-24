// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_EVENT_HPP
#define _NGX_EVENT_HPP

#include "Nginx.hpp"

class NgxEvent final : public NgxWrapper<ngx_event_t>
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

    template<typename T>
    T& data() const
    {
        return *reinterpret_cast<T>(get()->data);
    }
public:
    template<typename F>
    void handler(F f) const
    {
        get()->handler = f;
    }
};

#endif  //_NGX_EVENT_HPP

// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_TIMER_HPP
#define _NGX_TIMER_HPP

#include <deque>
#include <boost/function.hpp>
#include <boost/functional/factory.hpp>

#include "NgxValue.hpp"

template<typename T>
class NgxTimerEvent final
{
public:
    NgxTimerEvent() = default;
    ~NgxTimerEvent() = default;
public:
    typedef boost::function<void()> func_type;
public:
    ngx_int_t init(ngx_msec_t ms, func_type f)
    {
        m_func = f;

        NgxValue::memzero(m_ev);

        m_ev.handler = &T::call;
        m_ev.data = this;
        m_ev.log = ngx_cycle->log;

        ngx_add_timer(&m_ev, ms);

        return NGX_OK;
    }

    void operator()() const
    {
        m_func();
    }
private:
    ngx_event_t m_ev;
    func_type m_func;
};

class NgxTimer final
{
public:
    typedef NgxTimerEvent<NgxTimer> timer_event_type;
    typedef timer_event_type::func_type func_type;
public:
    NgxTimer() = default;
    ~NgxTimer() = default;
public:
    static ngx_int_t add(ngx_msec_t ms, func_type f)
    {
        return acquire().init(ms, f);
    }
public:
    static void call(ngx_event_t *ev)
    {
        auto& e = *reinterpret_cast<timer_event_type*>(ev->data);

        e();

        release(e);
    }
private:
    typedef std::deque<timer_event_type*> timer_pool_type;

    static timer_pool_type& pool()
    {
        static timer_pool_type p;
        return p;
    }

    static timer_event_type& acquire()
    {
        if(pool().empty())
        {
            return *boost::factory<timer_event_type*>()();
        }

        auto p = pool().front();
        pool().pop_front();

        return *p;
    }

    static void release(timer_event_type& e)
    {
        pool().push_back(&e);
    }
};

#endif  //_NGX_TIMER_HPP


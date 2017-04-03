// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_TIMER_HPP
#define _NGX_TIMER_HPP

//#include <deque>
//#include <boost/function.hpp>
//#include <boost/functional/factory.hpp>

#include "NgxValue.hpp"

template<typename T>
class NgxTimerEv final
{
public:
    typedef NgxTimerEv  this_type;
    typedef T           factory_type;
public:
    NgxTimerEv() = default;
    ~NgxTimerEv() = default;
public:
    typedef boost::function<void()> func_type;
public:
    // f can be bind or lambda
    void init(func_type f, ngx_msec_t ms = ngx_nil)
    {
        m_func = f;

        NgxValue::memzero(m_ev);

        m_ev.handler = &T::call;
        m_ev.data = this;
        m_ev.log = ngx_cycle->log;

        if(NgxValue::invalid(ms))
        {
            return;
        }

        start(ms);
    }

    void start(ngx_msec_t ms)
    {
        ngx_add_timer(&m_ev, ms);
    }

public:
    // deprecated
    ngx_int_t init(ngx_msec_t ms, func_type f)
    {
        init(f, ms);
        return NGX_OK;
    }
public:
    void clear()
    {
        //m_ev.timedout = false;

        if(m_ev.timer_set)
        {
            ngx_del_timer(&m_ev);
        }
    }
public:
    //void cleanup(ngx_http_request_t* r) const
    //{
    //    auto cln = ngx_http_cleanup_add(r, 0);

    //    NgxException::require(cln);

    //    cln->handler = &this_type::cleanup_handler;
    //    cln->data = this;
    //}

public:
    void operator()() const
    {
        if(!m_ev.timedout)      // not timedout, do not call func
        {
            return;
        }

        m_func();
    }
private:
    ngx_event_t m_ev;
    func_type m_func;
};

class NgxTimer final
{
public:
    typedef NgxTimerEv<NgxTimer> timer_event_type;
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

        //e.clear();

        release(e);
    }
private:
    typedef std::deque<timer_event_type*> timer_pool_type;

    static timer_pool_type& pool()
    {
        static timer_pool_type p;
        return p;
    }

private:
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

class NgxTimerCaller final
{
public:
    typedef NgxTimerEv<NgxTimerCaller> timer_event_type;
    typedef timer_event_type::func_type func_type;
public:
    static void call(ngx_event_t *ev)
    {
        auto& e = *reinterpret_cast<timer_event_type*>(ev->data);

        e();
    }
};

typedef NgxTimerCaller::timer_event_type NgxTimerEvent;

#endif  //_NGX_TIMER_HPP


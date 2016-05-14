// Copyright (c) 2015
// Author: Chrono Law
#ifndef _CPP_EPOLL_HANDLER_HPP
#define _CPP_EPOLL_HANDLER_HPP

#include <vector>

#include "CppEpollConf.hpp"

class CppEpollHandler final
{
public:
    typedef CppEpollHandler this_type;
    typedef CppEpollConf    conf_type;

    typedef CppEpollModule  this_module;

    typedef NgxLog<NGX_LOG_EMERG> NgxLogEmerg;
    typedef NgxLog<NGX_LOG_ALERT> NgxLogAlert;
public:
    static ngx_event_actions_t& actions()
    {
        static ngx_event_actions_t a = {
                &this_type::add,
                &this_type::del,

                &this_type::add,
                &this_type::del,

                &this_type::add_conn,
                &this_type::del_conn,

                &this_type::notify,
                &this_type::process_events,

                &this_type::init,
                &this_type::done,
            };

        return a;
    }
public:
    static ngx_int_t init(ngx_cycle_t* cycle, ngx_msec_t timer)
    {
        boost::ignore_unused(timer);

        if(init_epoll_data(cycle) != NGX_OK)
        {
            return NGX_ERROR;
        }

        if(init_notify_data(cycle) != NGX_OK)
        {
            actions().notify = nullptr;     //nginx 1.9.x
            //return NGX_ERROR;               //nginx 1.8.0
        }

        init_global_vars();                 //ngx_io, ngx_event_flags,...

        return NGX_OK;
    }

    static void done(ngx_cycle_t* cycle)
    {
        done_epoll_data(cycle);

        done_notify_data(cycle);
    }

public:
    static ngx_int_t add(ngx_event_t* ev, ngx_int_t event, ngx_uint_t flags)
    {
        // c->fd, c->active, c->instance
        auto c = reinterpret_cast<ngx_connection_t*>(ev->data);

        ngx_event_t* e;     // e is another event for r/w
        uint32_t prev;

        if(event == NGX_READ_EVENT)
        {
            e = c->write;
            prev = EPOLLOUT;
        }
        else
        {
            e = c->read;
            prev = EPOLLIN|EPOLLRDHUP;
        }

        int op;
        auto events = (uint32_t) event;

        if(e->active)       // check all r/w
        {
            op = EPOLL_CTL_MOD;
            events |= prev;
        }
        else
        {
            op = EPOLL_CTL_ADD;
        }

        epoll_event ee;

        ee.events = events | (uint32_t) flags;
        ee.data.ptr = (void *) ((uintptr_t) c | ev->instance);

        if(::epoll_ctl(epoll_data().ep, op, c->fd, &ee) < 0)
        {
            NgxLogAlert(ev).print(ngx_errno, "epoll_ctl(%d, %d) failed", op, c->fd);

            return NGX_ERROR;
        }

        ev->active = true;

        return NGX_OK;
    }

    static ngx_int_t del(ngx_event_t* ev, ngx_int_t event, ngx_uint_t flags)
    {
        if(flags & NGX_CLOSE_EVENT)
        {
            ev->active = false;
            return NGX_OK;
        }

        auto c = reinterpret_cast<ngx_connection_t*>(ev->data);

        ngx_event_t* e;     // e is another event for r/w
        uint32_t prev;

        if(event == NGX_READ_EVENT)
        {
            e = c->write;
            prev = EPOLLOUT;
        }
        else
        {
            e = c->read;
            prev = EPOLLIN|EPOLLRDHUP;
        }

        int op;
        epoll_event ee;

        if(e->active)       // keep another r/w
        {
            op = EPOLL_CTL_MOD;
            ee.events = prev | (uint32_t) flags;
            ee.data.ptr = (void *) ((uintptr_t) c | ev->instance);
        }
        else                // no active, del event
        {
            op = EPOLL_CTL_DEL;
            ee.events = 0;
            ee.data.ptr = nullptr;
        }

        if(::epoll_ctl(epoll_data().ep, op, c->fd, &ee) < 0)
        {
            NgxLogAlert(ev).print(ngx_errno, "epoll_ctl(%d, %d) failed", op, c->fd);

            return NGX_ERROR;
        }

        ev->active = false;

        return NGX_OK;
    }
public:
    static ngx_int_t add_conn(ngx_connection_t* c)
    {
        epoll_event ee;

        ee.events = EPOLLIN|EPOLLOUT|EPOLLET|EPOLLRDHUP;
        ee.data.ptr = (void *) ((uintptr_t) c | c->read->instance);     // set read instance!

        if(::epoll_ctl(epoll_data().ep, EPOLL_CTL_ADD, c->fd, &ee) < 0)
        {
            NgxLogAlert(c).print(ngx_errno, "epoll_ctl(EPOLL_CTL_ADD, %d) failed", c->fd);

            return NGX_ERROR;
        }

        c->read->active = true;
        c->write->active = true;

        return NGX_OK;
    }

    static ngx_int_t del_conn(ngx_connection_t* c, ngx_uint_t flags)
    {
        if(flags & NGX_CLOSE_EVENT)
        {
            c->read->active = false;
            c->write->active = false;

            return NGX_OK;
        }

        epoll_event ee; //{0, nullptr};

        ee.events = 0;
        ee.data.ptr = nullptr;

        if(::epoll_ctl(epoll_data().ep, EPOLL_CTL_DEL, c->fd, &ee) < 0)
        {
            NgxLogAlert(c).print(ngx_errno, "epoll_ctl(EPOLL_CTL_DEL, %d) failed", c->fd);

            return NGX_ERROR;
        }

        c->read->active = false;
        c->write->active = false;

        return NGX_OK;
    }
public:
    static ngx_int_t notify(ngx_event_handler_pt handler)
    {
        static uint64_t v = 1;

        auto& notify = notify_data();

        notify.event.data = (void*)handler;

        // write a int to trigger event
        auto n = ::write(notify.fd, &v, sizeof(v));

        if((size_t) n != sizeof(v))
        {
            NgxLogAlert(notify.event.log).print(ngx_errno,
                    "write() to eventfd %d failed", notify.fd);
        }

        return NGX_OK;
    }
public:
    // in ngx_process_events_and_timers, flags = NGX_UPDATE_TIME|NGX_POST_EVENTS
    static ngx_int_t process_events(ngx_cycle_t* cycle, ngx_msec_t timer, ngx_uint_t flags)
    {
        auto& ep = epoll_data().ep;
        auto& event_list = epoll_data().event_list;

        auto events = ::epoll_wait(
                    ep,
                    event_list.data(), event_list.size(),
                    timer);

        ngx_err_t err = (events < 0) ? ngx_errno : 0;

        if(flags & NGX_UPDATE_TIME ||   // client require update time, wait for signal
           ngx_event_timer_alarm)       // unix signal
        {
            ngx_time_update();
        }

        if(err)
        {
            if(err == NGX_EINTR)        // interrupt by signal
            {
                if(ngx_event_timer_alarm)
                {
                    ngx_event_timer_alarm = false;
                    return NGX_OK;
                }

                NgxLogInfo(cycle).print(err, "epoll_wait() failed");
            }
            else
            {
                NgxLogAlert(cycle).print(err, "epoll_wait() failed");
            }

            return NGX_ERROR;
        }

        if(!events) // ==0
        {
            if(timer != NGX_TIMER_INFINITE)     //(ngx_msec_t) -1
            {
                return NGX_OK;
            }

            // infinite wait but got no events!
            NgxLogAlert(cycle).print("epoll_wait() returned no events without timeout");

            return NGX_ERROR;
        }

        // check all events
        process_events_list(cycle, events, (flags & NGX_POST_EVENTS));

        return NGX_OK;
    }
private:
    static void process_events_list(ngx_cycle_t* cycle, int events, bool posted)
    {
        auto& event_list = epoll_data().event_list;

        uint32_t            revents = 0;
        ngx_event_t*        rev = nullptr;
        ngx_event_t*        wev = nullptr;
        ngx_queue_t*        queue = nullptr;
        ngx_connection_t*   c = nullptr;

        for(int i = 0; i < events; ++i)
        {
            c = reinterpret_cast<ngx_connection_t*>(
                                            event_list[i].data.ptr);       // get conn ptr

            ngx_int_t instance = (uintptr_t) c & 1;
            c = (ngx_connection_t *) ((uintptr_t) c & (uintptr_t) ~1);

            rev = c->read;

            if(NgxValue::invalid(c->fd) || rev->instance != instance)
            {
                continue; // conn stale
            }

            revents = event_list[i].events;        //check events

            if(revents & (EPOLLERR|EPOLLHUP))           //read error occured
            {
                NgxLogDebug(cycle).print(
                        "epoll_wait() error on fd:%d ev:%04XD", c->fd, revents);

                if((revents & (EPOLLIN|EPOLLOUT)) == 0)    //no read or write
                {
                    revents |= EPOLLIN|EPOLLOUT;            //add flags to call some handler
                }

                c->error = true;                            // maby be a nginx's bug?
            }

            if((revents &EPOLLIN) && rev->active)       // read or accept events
            {
                if(revents & EPOLLRDHUP)                // client close socket
                {
                    rev->pending_eof = true;            // int ngx_http_request.c???
                }

                rev->ready = true;                      //can read

                if(posted)             //NGX_POST_EVENTS         2
                {
                    queue = rev->accept ?          //is accept?
                            &ngx_posted_accept_events : //accept queue
                            &ngx_posted_events;         //read queue

                    ngx_post_event(rev, queue);        //ngx_event_posted.h
                }
                else
                {
                    rev->handler(rev);                  //do it now!
                }
            }

            wev = c->write;

            if((revents & EPOLLOUT) && wev->active)     //can write
            {
                if(NgxValue::invalid(c->fd) || wev->instance != instance)
                {
                    continue; // conn stale
                }

                wev->ready = true;

                if(posted)             //NGX_POST_EVENTS         2
                {
                    ngx_post_event(rev, &ngx_posted_events);        //ngx_event_posted.h
                }
                else
                {
                    wev->handler(rev);                  //do it now!
                }
            }
        }   //for all events

        // ngx_event_process_posted events in ngx_process_events_and_timers
    }
private:
    static conf_type& conf(ngx_cycle_t* cycle)
    {
        auto conf_ptr = ngx_get_conf(cycle->conf_ctx, ngx_events_module);

        return this_module::instance().conf().get<conf_type>(*conf_ptr);
    }
private:
    static void init_global_vars()
    {
        ngx_io = ngx_os_io;     //for macro ngx_recv/ngx_send/...

        // set global pointers
        ngx_event_actions = actions();  //for macro ngx_add_event/ngx_del_event/...

        // epoll et mode
        ngx_event_flags = NGX_USE_CLEAR_EVENT|
                          NGX_USE_GREEDY_EVENT|
                          NGX_USE_EPOLL_EVENT;
    }
private:
    struct epoll_data_t final
    {
        int          ep = -1;

        std::vector<epoll_event> event_list;

        //epoll_event* event_list = nullptr;
        //ngx_uint_t   nevents = 0;
    };

    static epoll_data_t& epoll_data()
    {
        static epoll_data_t ep;

        return ep;
    }
private:
    static ngx_int_t init_epoll_data(ngx_cycle_t* cycle)
    {
        auto& ep_data = epoll_data();

        // init epoll
        if(NgxValue::invalid(ep_data.ep))
        {
            ep_data.ep = ::epoll_create(cycle->connection_n / 2);

            if(NgxValue::invalid(ep_data.ep))
            {
                NgxLogEmerg(cycle).print(
                        ngx_errno, "epoll_create() failed");
                return NGX_ERROR;
            }
        }

        auto& cf = conf(cycle);

        // init events list
        auto& event_list = ep_data.event_list;

        if(event_list.size() < cf.events)
        {
            event_list.resize(cf.events);
        }

        //if(ep_data.nevents < cf.events)
        //{
        //    if(ep_data.event_list)
        //    {
        //        ngx_free(ep_data.event_list);
        //    }

        //    ep_data.event_list = reinterpret_cast<epoll_event*>(
        //        ngx_alloc(
        //            sizeof(epoll_event) * cf.events, cycle->log)
        //        );

        //    if(!ep_data.event_list)
        //    {
        //        return NGX_ERROR;
        //    }
        //}

        //ep_data.nevents = cf.events;

        return NGX_OK;
    }

    static void done_epoll_data(ngx_cycle_t* cycle)
    {
        auto& ep_data = epoll_data();

        ::close(ep_data.ep);
        ep_data.ep = NgxUnsetValue::get();

        ep_data.event_list.clear();
        //ngx_free(ep_data.event_list);
        //ep_data.event_list = nullptr;

        //ep_data.nevents = 0;
    }
private:
    struct notify_data_t final
    {
        int              fd = -1;
        ngx_event_t      event;
        ngx_connection_t conn;
    };

    static notify_data_t& notify_data()
    {
        static notify_data_t n;

        return n;
    }
private:
    static ngx_int_t init_notify_data(ngx_cycle_t* cycle)
    {
        auto& notify = notify_data();

        notify.fd = ::eventfd(0, 0);

        if(NgxValue::invalid(notify.fd))
        {
            NgxLogEmerg(cycle).print(ngx_errno, "eventfd() failed");

            return NGX_ERROR;
        }

        notify.event.handler = &this_type::notify_handler;
        notify.event.log = cycle->log;
        notify.event.active = true;

        notify.conn.fd = notify.fd;
        notify.conn.read = &notify.event;
        notify.conn.log = cycle->log;

        // add notify event
        epoll_event ee;

        ee.events = EPOLLIN|EPOLLET;
        ee.data.ptr = &notify.conn;     //allways use ptr to store conn

        if(::epoll_ctl(
                epoll_data().ep, EPOLL_CTL_ADD, notify.fd, &ee) < 0)
        {
            NgxLogEmerg(cycle).print(ngx_errno, "epoll_ctl(EPOLL_CTL_ADD, eventfd) failed");

            ::close(notify.fd);     // ommit close error

            return NGX_ERROR;
        }

        return NGX_OK;
    }

    static void notify_handler(ngx_event_t* ev)
    {
        // check data in fd
        if(++ev->index == NGX_MAX_UINT32_VALUE)
        {
            ev->index = 0;

            uint64_t count;     // read count number

            auto n = ::read(notify_data().fd, &count, sizeof(count));

            // ommit error check
            boost::ignore_unused(n);
        }

        auto handler = reinterpret_cast<ngx_event_handler_pt>(ev->data);

        handler(ev);
    }

    static void done_notify_data(ngx_cycle_t* cycle)
    {
        auto& notify = notify_data();

        ::close(notify.fd);
        notify.fd = NgxUnsetValue::get();
    }
};

#endif  //_CPP_EPOLL_HANDLER_HPP

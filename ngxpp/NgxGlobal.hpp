// Copyright (c) 2015-2016
// Author: Chrono Law
#ifndef _NGX_GLOBAL_HPP
#define _NGX_GLOBAL_HPP

#include <nginx.h>      // for NGINX_VER...
#include <boost/noncopyable.hpp>

#include "Nginx.hpp"

// decltype((xxx)) get the type of expressions!
#define DECL_VAR(x, y)  decltype((y))& x = y

// apis: os(), err(), event(), process(), signal(), cycle(), env()
class NgxGlobal final
{
public:
    NgxGlobal() = default;
   ~NgxGlobal() = default;
public:
    struct os_info_t final : boost::noncopyable
    {
        DECL_VAR(pagesize, ngx_pagesize);
        DECL_VAR(cacheline_size, ngx_cacheline_size);
        DECL_VAR(pagesize_shift, ngx_pagesize_shift);

        DECL_VAR(ncpu, ngx_ncpu);
        DECL_VAR(max_sockets, ngx_max_sockets);
        DECL_VAR(inherited_nonblocking, ngx_inherited_nonblocking);
        DECL_VAR(tcp_nodelay_and_tcp_nopush, ngx_tcp_nodelay_and_tcp_nopush);

        // ngx_recv/ngx_send
        DECL_VAR(io, ngx_os_io);
    };

    static os_info_t& os()
    {
        static os_info_t o;
        return o;
    }
public:
    struct errno_info_t final : boost::noncopyable
    {
        DECL_VAR(no, ngx_errno);
        DECL_VAR(socket, ngx_socket_errno);
    };

    static errno_info_t& err()
    {
        static errno_info_t e;
        return e;
    }
public:
    struct process_info_t final : boost::noncopyable
    {
        // process/worker id
        DECL_VAR(pid, ngx_pid);

#if (nginx_version >= 1009001)
        DECL_VAR(worker, ngx_worker);
#endif

        // process flag = NGX_PROCESS_MASTER/NGX_PROCESS_SINGLE/...
        DECL_VAR(type, ngx_process);

        DECL_VAR(new_binary, ngx_new_binary);

        DECL_VAR(daemonized, ngx_daemonized);
        DECL_VAR(exiting, ngx_exiting);

        DECL_VAR(channel, ngx_channel);
        DECL_VAR(slot, ngx_process_slot);
        DECL_VAR(last, ngx_last_process);

        // array for all processes
        DECL_VAR(array, ngx_processes);
    };

    static process_info_t& process()
    {
        static process_info_t p;
        return p;
    }
public:
    struct event_info_t final : boost::noncopyable
    {
        // ngx_add_event/ngx_del_event/...
        DECL_VAR(actions, ngx_event_actions);
        DECL_VAR(io, ngx_io);

        // sigalarm flag
        DECL_VAR(timer_alarm, ngx_event_timer_alarm);

        // epoll/kqueue flags, NGX_CLEAR_EVENT(EPOLLET)
        DECL_VAR(flags, ngx_event_flags);

        // flags
        DECL_VAR(use_accept_mutex, ngx_use_accept_mutex);
        DECL_VAR(accept_events, ngx_accept_events);
        DECL_VAR(accept_mutex_held, ngx_accept_mutex_held);
        DECL_VAR(accept_disabled, ngx_accept_disabled);

        // delay time in ms
        DECL_VAR(accept_mutex_delay, ngx_accept_mutex_delay);

        // queue
        DECL_VAR(posted_accept, ngx_posted_accept_events);
        DECL_VAR(posted, ngx_posted_events);
    };

    static event_info_t& event()
    {
        static event_info_t e;
        return e;
    }
public:
    struct signal_info_t final : boost::noncopyable
    {
        DECL_VAR(alrm, ngx_sigalrm);
        DECL_VAR(quit, ngx_quit);
        DECL_VAR(terminate, ngx_terminate);
        DECL_VAR(exiting, ngx_exiting);
        DECL_VAR(reap, ngx_reap);       //sigchld, reopen child
        DECL_VAR(reconfigure, ngx_reconfigure);
    };

    static signal_info_t& signal()
    {
        static signal_info_t s;
        return s;
    }
public:
    // we can't use declyte(xxx)& because keyword 'volatile'
    struct cycle_info_t final : boost::noncopyable
    {
        DECL_VAR(connection_n, ngx_cycle->connection_n);

        DECL_VAR(conf_file, ngx_cycle->conf_file);
        DECL_VAR(conf_param, ngx_cycle->conf_param);
        DECL_VAR(conf_prefix, ngx_cycle->conf_prefix);
        DECL_VAR(prefix, ngx_cycle->prefix);
        DECL_VAR(hostname, ngx_cycle->hostname);
    };

    static cycle_info_t& cycle()
    {
        static cycle_info_t c;

        return c;
    }
public:
    // some macro constants
    struct env_info_t final : boost::noncopyable
    {
        // in nginx.h
        ngx_str_t version   = ngx_string(NGINX_VER);
        ngx_str_t build     = ngx_string(NGINX_VER_BUILD);

        // in objs/ngx_auto_config.h
        ngx_str_t configure = ngx_string(NGX_CONFIGURE);
        ngx_str_t compiler  = ngx_string(NGX_COMPILER);
    };

    static env_info_t& env()
    {
        static env_info_t e;
        return e;
    }
};

#undef DECL_VAR

#endif  //_NGX_GLOBAL_HPP

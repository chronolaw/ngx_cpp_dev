// Copyright (c) 2015
// Author: Chrono Law
#ifndef _CPP_EPOLL_INIT_HPP
#define _CPP_EPOLL_INIT_HPP

#include "CppEpollConf.hpp"
#include "CppEpollHandler.hpp"

class CppEpollInit final
{
public:
    typedef CppEpollConf    conf_type;
    typedef CppEpollHandler handler_type;
    typedef CppEpollInit    this_type;
public:
    static ngx_command_t* cmds()
    {
        static ngx_command_t n[] =
        {
            NgxCommand(
                ngx_string("cpp_epoll_events"),
                NgxTake(NGX_EVENT_CONF, 1),
                ngx_conf_set_num_slot,
                0,
                offsetof(conf_type, events)
            ),

            NgxCommand()
        };

        return n;
    }
public:
    static ngx_event_module_t* ctx()
    {
        static ngx_str_t name = ngx_string("cpp_epoll");

        static ngx_event_module_t c =
        {
            &name,

            &conf_type::create,
            &conf_type::init,

            handler_type::actions()
        };

        return &c;
    }
public:
    static const ngx_module_t& module()
    {
        static ngx_module_t m =
        {
            NGX_MODULE_V1,

            ctx(),
            cmds(),

            NGX_EVENT_MODULE,

            NGX_MODULE_NULL(7),

            NGX_MODULE_V1_PADDING
        };

        return m;
    }
};

#endif  //_CPP_EPOLL_INIT_HPP


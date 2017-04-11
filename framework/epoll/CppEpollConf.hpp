// Copyright (c) 2016-2017
// Author: Chrono Law
#ifndef _CPP_EPOLL_CONF_HPP
#define _CPP_EPOLL_CONF_HPP

#include "NgxAll.hpp"

class CppEpollConf final
{
public:
    typedef CppEpollConf this_type;
public:
    CppEpollConf() = default;
    ~CppEpollConf() = default;
public:
    ngx_uint_t  events = ngx_nil;
public:
    static void* create(ngx_cycle_t* cycle)
    {
        return NgxPool(cycle).alloc<this_type>();
    }

    static char* init(ngx_cycle_t *cycle, void *c)
    {
        boost::ignore_unused(cycle);

        auto conf = this_type::cast(c);

        NgxValue::init(conf->events, 512);      //default list size = 512

        return NGX_CONF_OK;
    }
public:
    static this_type* cast(void* c)
    {
        return reinterpret_cast<this_type*>(c);
    }
};

//NGX_MOD_INSTANCE(CppEpollModule, cpp_epoll_module)

#endif  //_CPP_EPOLL_CONF_HPP

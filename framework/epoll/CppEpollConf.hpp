// Copyright (c) 2016
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
    ngx_uint_t  events = NgxUnsetValue::get();
public:
    static void* create(ngx_cycle_t* cycle)
    {
        return NgxPool(cycle).alloc<this_type>();
    }

    static char* init(ngx_cycle_t *cycle, void *c)
    {
        boost::ignore_unused(cycle);

        auto conf = reinterpret_cast<this_type*>(c);

        NgxValue::init(conf->events, 512);      //default list size = 512

        return NGX_CONF_OK;
    }
};

NGX_MOD_INSTANCE(CppEpollModule, cpp_epoll_module)

#endif  //_CPP_EPOLL_CONF_HPP

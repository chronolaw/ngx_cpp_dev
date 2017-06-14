// Copyright (c) 2016-2017
// Author: Chrono Law
#ifndef _NDG_STREAM_TIME_CONF_HPP
#define _NDG_STREAM_TIME_CONF_HPP

#include "NgxStreamAll.hpp"

class NdgStreamTimeConf final
{
public:
    typedef NdgStreamTimeConf this_type;
public:
    NdgStreamTimeConf() = default;
    ~NdgStreamTimeConf() = default;
public:
    static void* create(ngx_conf_t* cf)
    {
        return NgxPool(cf).alloc<this_type>();
    }

    static char* merge(ngx_conf_t *cf, void *parent, void *child)
    {
        return NGX_CONF_OK;
    }
public:
    static this_type& cast(void* p)
    {
        return *reinterpret_cast<this_type*>(p);
    }
};

NGX_MOD_INSTANCE(NdgStreamTimeModule, ndg_stream_datetime_module, NdgStreamTimeConf)

#endif  //_NDG_STREAM_TIME_CONF_HPP

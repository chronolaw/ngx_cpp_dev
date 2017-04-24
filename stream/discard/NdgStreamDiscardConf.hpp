// Copyright (c) 2016-2017
// Author: Chrono Law
#ifndef _NDG_STREAM_DISCARD_CONF_HPP
#define _NDG_STREAM_DISCARD_CONF_HPP

#include "NgxStreamAll.hpp"

class NdgStreamDiscardConf final
{
public:
    typedef NdgStreamDiscardConf this_type;
public:
    NdgStreamDiscardConf() = default;
    ~NdgStreamDiscardConf() = default;
public:
    ngx_msec_t timeout = ngx_nil;
public:
    static void* create(ngx_conf_t* cf)
    {
        return NgxPool(cf).alloc<this_type>();
    }

    static char* merge(ngx_conf_t *cf, void *parent, void *child)
    {
        boost::ignore_unused(cf);

        auto& prev = cast(parent);
        auto& conf = cast(child);

        NgxValue::merge(conf.timeout, prev.timeout, 5000);

        return NGX_CONF_OK;
    }
public:
    static this_type& cast(void* p)
    {
        return *reinterpret_cast<this_type*>(p);
    }
};

NGX_MOD_INSTANCE(NdgStreamDiscardModule,
        ndg_stream_discard_module, NdgStreamDiscardConf)

#endif  //_NDG_STREAM_DISCARD_CONF_HPP

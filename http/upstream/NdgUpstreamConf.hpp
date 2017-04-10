// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NDG_UPSTREAM_CONF_HPP
#define _NDG_UPSTREAM_CONF_HPP

#include "NgxAll.hpp"

class NdgUpstreamConf final
{
public:
    typedef NdgUpstreamConf this_type;
public:
    NdgUpstreamConf() = default;
    ~NdgUpstreamConf() = default;
public:
    ngx_http_upstream_conf_t upstream;
public:
    static void* create(ngx_conf_t* cf)
    {
        auto& c = *NgxPool(cf).alloc<this_type>();

        c.upstream.connect_timeout = 1000*30;
        c.upstream.send_timeout = 1000*30;
        c.upstream.read_timeout = 1000*30;
        c.upstream.buffer_size = ngx_pagesize;

        return &c;
    }

};

NGX_MOD_INSTANCE(NdgUpstreamModule, ndg_upstream_module, NdgUpstreamConf)

#endif  //_NDG_UPSTREAM_CONF_HPP

// Copyright (c) 2015-2018
// Author: Chrono Law
#ifndef _NDG_FILTER_CONF_HPP
#define _NDG_FILTER_CONF_HPP

#include "NgxAll.hpp"

class NdgFilterConf final
{
public:
    typedef NdgFilterConf this_type;
public:
    NdgFilterConf() = default;
    ~NdgFilterConf() = default;
public:
    ngx_array_t*    headers;
    ngx_str_t       footer;
public:
    static void* create(ngx_conf_t* cf)
    {
        return NgxPool(cf).alloc<this_type>();
    }

};

struct NdgFilterCtx final
{
    int flag = 0;
};

NGX_MOD_CTX_INSTANCE(NdgFilterModule, ngx_http_ndg_filter_module, NdgFilterCtx, NdgFilterConf)

#endif  //_NDG_FILTER_CONF_HPP

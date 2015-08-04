// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_SUBREQUEST_CONF_HPP
#define _NDG_SUBREQUEST_CONF_HPP

#include "NgxAll.hpp"

class NdgSubrequestConf final
{
public:
    typedef NdgSubrequestConf this_type;
public:
    NdgSubrequestConf() = default;
    ~NdgSubrequestConf() = default;
public:
    ngx_str_t loc;
    ngx_str_t args;
public:
    static void* create(ngx_conf_t* cf)
    {
        return NgxPool(cf).alloc<this_type>();
    }

    static char* merge(ngx_conf_t *cf, void *parent, void *child)
    {
        boost::ignore_unused(cf);

        auto prev = reinterpret_cast<this_type*>(parent);
        auto conf = reinterpret_cast<this_type*>(child);

        NgxValue::merge(conf->loc, prev->loc, ngx_string("/echo"));
        NgxValue::merge(conf->args, prev->args, ngx_null_string);

        return NGX_CONF_OK;
    }
};

NGX_MOD_INSTANCE(NdgSubrequestModule, ndg_subrequest_module, NdgSubrequestConf)

#endif  //_NDG_SUBREQUEST_CONF_HPP

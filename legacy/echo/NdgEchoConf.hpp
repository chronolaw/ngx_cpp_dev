// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NDG_ECHO_CONF_HPP
#define _NDG_ECHO_CONF_HPP

#include "NgxAll.hpp"

#define ENABLE_SCRIPT

class NdgEchoConf final
{
public:
    typedef NdgEchoConf this_type;
public:
    NdgEchoConf() = default;
    ~NdgEchoConf() = default;
public:
    ngx_str_t msg;
#ifdef ENABLE_SCRIPT
    NgxComplexValue var;
#endif
public:
    static void* create(ngx_conf_t* cf)
    {
        return NgxPool(cf).alloc<this_type>();
    }

    static this_type& cast(void* conf)
    {
        return *reinterpret_cast<this_type*>(conf);
    }
};

NGX_MOD_INSTANCE(NdgEchoModule, ndg_echo_module, NdgEchoConf)

#endif  //_NDG_ECHO_CONF_HPP

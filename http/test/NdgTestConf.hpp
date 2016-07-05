// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_TEST_CONF_HPP
#define _NDG_TEST_CONF_HPP

#include "NgxAll.hpp"

class NdgTestConf final
{
public:
    typedef NdgTestConf this_type;
public:
    NdgTestConf() = default;
    ~NdgTestConf() = default;
public:
    ngx_flag_t enabled = NgxUnsetValue::get();
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

        NgxValue::merge(conf->enabled, prev->enabled, 0);

        return NGX_CONF_OK;
    }
};

NGX_MOD_INSTANCE(NdgTestModule, ndg_test_module, NdgTestConf)

#endif  //_NDG_TEST_CONF_HPP

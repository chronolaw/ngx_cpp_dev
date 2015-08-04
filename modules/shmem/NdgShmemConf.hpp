// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_SHMEM_CONF_HPP
#define _NDG_SHMEM_CONF_HPP

#include "NgxAll.hpp"

class NdgShmemConf final
{
public:
    typedef NdgShmemConf this_type;
public:
    NdgShmemConf() = default;
    ~NdgShmemConf() = default;
public:
    std::size_t size = NgxUnsetValue::get();
public:
    static void* create(ngx_conf_t* cf)
    {
        return NgxPool(cf).alloc<this_type>();
    }

    static char* init(ngx_conf_t *cf, void *c)
    {
        boost::ignore_unused(cf);

        auto conf = reinterpret_cast<this_type*>(c);

        NgxValue::init(conf->size, 1024);

        return NGX_CONF_OK;
    }
};

NGX_MOD_INSTANCE(NdgShmemModule, ndg_shmem_module, void, void, NdgShmemConf)

#endif  //_NDG_SHMEM_CONF_HPP

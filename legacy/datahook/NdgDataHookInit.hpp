// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NDG_DATA_HOOK_INIT_HPP
#define _NDG_DATA_HOOK_INIT_HPP

#include "NdgDataHookConf.hpp"
#include "NdgDataHookHandler.hpp"

class NdgDataHookInit final
{
public:
    typedef NdgDataHookHandler handler_type;
    typedef NdgDataHookInit this_type;
public:
    static ngx_http_module_t* ctx()
    {
        static ngx_http_module_t c =
        {
            NGX_MODULE_NULL(1),
            &handler_type::init,
            NGX_MODULE_NULL(6),
        };

        return &c;
    }
public:
    static const ngx_module_t& module()
    {
        static ngx_module_t m =
        {
            NGX_MODULE_V1,

            ctx(),
            nullptr,

            NGX_HTTP_MODULE,
            NGX_MODULE_NULL(7),
            NGX_MODULE_V1_PADDING
        };

        return m;
    }
};

#endif  //_NDG_DATA_HOOK_INIT_HPP

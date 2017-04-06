// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_TEST_INIT_HPP
#define _NDG_TEST_INIT_HPP

#include "NdgTestConf.hpp"
#include "NdgTestHandler.hpp"

class NdgTestInit final
{
public:
    typedef NdgTestConf conf_type;
    typedef NdgTestHandler handler_type;
    typedef NdgTestInit this_type;
public:
    static ngx_command_t* cmds()
    {
        static ngx_command_t n[] =
        {
            {
                ngx_string("ndg_test"),
                NgxTake(NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG, 1),
                ngx_conf_set_flag_slot,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(conf_type, enabled),
                0
            },

            ngx_null_command
        };

        return n;
    }
public:
    static ngx_http_module_t* ctx()
    {
        static ngx_http_module_t c =
        {
            NGX_MODULE_NULL(1),

            &handler_type::init,

            NGX_MODULE_NULL(4),

            &conf_type::create,
            &conf_type::merge,
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
            cmds(),

            NGX_HTTP_MODULE,
            NGX_MODULE_NULL(7),
            NGX_MODULE_V1_PADDING
        };

        return m;
    }
};

#endif  //_NDG_TEST_INIT_HPP


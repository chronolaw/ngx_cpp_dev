// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_FOOTER_INIT_HPP
#define _NDG_FOOTER_INIT_HPP

#include "NdgFooterConf.hpp"
#include "NdgFooterHandler.hpp"

class NdgFooterInit final
{
public:
    typedef NdgFooterConf conf_type;
    typedef NdgFooterHandler handler_type;
    typedef NdgFooterInit this_type;
public:
    static ngx_command_t* cmds()
    {
        static ngx_command_t n[] =
        {
            NgxCommand(
                ngx_string("ndg_header"),
                NgxTake(NGX_HTTP_LOC_CONF, 2),
                ngx_conf_set_keyval_slot,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(conf_type, headers)
            ),

            NgxCommand(
                ngx_string("ndg_footer"),
                NgxTake(NGX_HTTP_LOC_CONF, 1),
                ngx_conf_set_str_slot,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(conf_type, footer)
            ),

            NgxCommand()
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
            NGX_MODULE_NULL(1),
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

#endif  //_NDG_FOOTER_INIT_HPP

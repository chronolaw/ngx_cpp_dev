// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NDG_ECHO_INIT_HPP
#define _NDG_ECHO_INIT_HPP

#include "NdgEchoConf.hpp"
#include "NdgEchoHandler.hpp"

class NdgEchoInit final
{
public:
    typedef NdgEchoConf         conf_type;
    typedef NdgEchoHandler      handler_type;
    typedef NdgEchoInit         this_type;
public:
    static ngx_command_t* cmds()
    {
        static ngx_command_t n[] =
        {
            {
                ngx_string("ndg_echo"),
                NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
                &this_type::set_echo,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(conf_type, msg),
                nullptr
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
            NGX_MODULE_NULL(6),
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
private:
    static char* set_echo(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
    {
        auto rc = ngx_conf_set_str_slot(cf, cmd, conf);

        if(rc != NGX_CONF_OK)
        {
            return rc;
        }

        NgxHttpCoreModule::handler(
                        cf, &handler_type::handler);

        return NGX_CONF_OK;
    }
};

#endif  //_NDG_ECHO_INIT_HPP

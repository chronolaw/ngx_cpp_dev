// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NDG_LOAD_BALANCE_INIT_HPP
#define _NDG_LOAD_BALANCE_INIT_HPP

#include "NdgLoadBalanceConf.hpp"
#include "NdgLoadBalanceHandler.hpp"

class NdgLoadBalanceInit final
{
public:
    typedef NdgLoadBalanceHandler handler_type;
    typedef NdgLoadBalanceInit this_type;
public:
    static ngx_command_t* cmds()
    {
        static ngx_command_t n[] =
        {
            {
                ngx_string("ndg_load_balance"),
                NgxTake(NGX_HTTP_UPS_CONF, 0),
                &this_type::set_load_balance,
                0,0,nullptr
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
            NGX_MODULE_NULL(8)
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
    static char* set_load_balance(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
    {
        NgxHttpUpstreamModule::init(cf, &handler_type::init);

        return NGX_CONF_OK;
    }
};

#endif  //_NDG_LOAD_BALANCE_INIT_HPP

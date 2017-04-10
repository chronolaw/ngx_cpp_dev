// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NDG_UPSTREAM_INIT_HPP
#define _NDG_UPSTREAM_INIT_HPP

#include "NdgUpstreamConf.hpp"
#include "NdgUpstreamHandler.hpp"

class NdgUpstreamInit final
{
public:
    typedef NdgUpstreamConf conf_type;
    typedef NdgUpstreamHandler handler_type;
    typedef NdgUpstreamInit this_type;
public:
    static ngx_command_t* cmds()
    {
        static ngx_command_t n[] =
        {
            {
                ngx_string("ndg_upstream_pass"),
                NgxTake(NGX_HTTP_LOC_CONF, 1),
                &this_type::set_upstream_pass,
                NGX_HTTP_LOC_CONF_OFFSET,
                0, nullptr
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
    static char* set_upstream_pass(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
    {
        ngx_url_t u;

        u.url = NgxConfig::args(cf)[1];
        u.no_resolve = true;

        auto& lcf = *reinterpret_cast<conf_type*>(conf);

        lcf.upstream.upstream = ngx_http_upstream_add(cf, &u, 0);

        if(!lcf.upstream.upstream)
        {
            return NGX_CONF_ERROR;
        }

        NgxHttpCoreModule::handler(
                        cf, &handler_type::handler);

        return NGX_CONF_OK;
    }
};

#endif  //_NDG_UPSTREAM_INIT_HPP

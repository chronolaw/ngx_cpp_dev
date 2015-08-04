// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_SUBREQUEST_INIT_HPP
#define _NDG_SUBREQUEST_INIT_HPP

#include "NdgSubrequestConf.hpp"
#include "NdgSubrequestHandler.hpp"

class NdgSubrequestInit final
{
public:
    typedef NdgSubrequestConf conf_type;
    typedef NdgSubrequestHandler handler_type;
    typedef NdgSubrequestInit this_type;
public:
    static ngx_command_t* cmds()
    {
        static ngx_command_t n[] =
        {
            NgxCommand(
                ngx_string("ndg_subrequest_loc"),
                NgxTake(NGX_HTTP_LOC_CONF, 1),
                &this_type::set_subrequest_loc,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(conf_type, loc)
            ),

            NgxCommand(
                ngx_string("ndg_subrequest_args"),
                NgxTake(NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF, 1),
                ngx_conf_set_str_slot,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(conf_type, args)
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
            NGX_MODULE_NULL(6),
            &conf_type::create,
            &conf_type::merge
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
    static char* set_subrequest_loc(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
    {
        ngx_conf_set_str_slot(cf, cmd, conf);

        NgxHttpCoreModule::instance().handler(
                        cf, &handler_type::handler);

        return NGX_CONF_OK;
    }
};

#endif  //_NDG_SUBREQUEST_INIT_HPP

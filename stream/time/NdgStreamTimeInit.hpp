// Copyright (c) 2016-2017
// Author: Chrono Law
#ifndef _NDG_STREAM_TIME_INIT_HPP
#define _NDG_STREAM_TIME_INIT_HPP

#include "NdgStreamTimeConf.hpp"
#include "NdgStreamTimeHandler.hpp"

class NdgStreamTimeInit final
{
public:
    typedef NdgStreamTimeConf       conf_type;
    typedef NdgStreamTimeHandler    handler_type;
    typedef NdgStreamTimeInit       this_type;
public:
    static ngx_command_t* cmds()
    {
        static ngx_command_t n[] =
        {
            {
                ngx_string("ndg_stream_time"),
                NgxTake(NGX_STREAM_SRV_CONF, 0),
                &this_type::set_time,
                NGX_STREAM_SRV_CONF_OFFSET,
                0, nullptr
            },

            ngx_null_command
        };

        return n;
    }
public:
    static ngx_stream_module_t* ctx()
    {
        static ngx_stream_module_t c =
        {
            NGX_MODULE_NULL(4),

            NGX_MODULE_NULL(2)
            //&conf_type::create,
            //&conf_type::merge,
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

            NGX_STREAM_MODULE,
            NGX_MODULE_NULL(7),
            NGX_MODULE_V1_PADDING
        };

        return m;
    }
private:
    static char* set_time(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
    {
        boost::ignore_unused(cmd, conf);

        NgxStreamCoreModule::handler(
            cf, &handler_type::handler);

        return NGX_CONF_OK;
    }
};

#endif  //_NDG_STREAM_TIME_INIT_HPP


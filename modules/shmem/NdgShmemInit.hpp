// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_SHMEM_INIT_HPP
#define _NDG_SHMEM_INIT_HPP

#include "NdgShmemConf.hpp"
#include "NdgShmemHandler.hpp"

class NdgShmemInit final
{
public:
    typedef NdgShmemConf conf_type;
    typedef NdgShmemHandler handler_type;
    typedef NdgShmemInit this_type;
public:
    static ngx_command_t* cmds()
    {
        static ngx_command_t n[] =
        {
            NgxCommand(
                ngx_string("ndg_shmem_size"),
                NgxTake(NGX_HTTP_MAIN_CONF, 1),
                ngx_conf_set_size_slot,
                NGX_HTTP_MAIN_CONF_OFFSET,
                offsetof(conf_type, size)
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

            &conf_type::create,
            &conf_type::init,

            NGX_MODULE_NULL(4),
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
            NGX_MODULE_NULL(6),

            &handler_type::exit_master,

            NGX_MODULE_V1_PADDING
        };

        return m;
    }
private:
};

#endif  //_NDG_SHMEM_INIT_HPP


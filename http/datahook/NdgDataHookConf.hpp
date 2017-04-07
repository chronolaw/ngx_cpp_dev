// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NDG_DATA_HOOK_CONF_HPP
#define _NDG_DATA_HOOK_CONF_HPP

#include "NgxAll.hpp"

struct NdgDataHookCtx final
{
public:
    ngx_flag_t              hook = false;

    ngx_int_t               status = 0;
    ngx_http_headers_out_t* headers = nullptr;
    ngx_chain_t*            body = nullptr;

    ngx_flag_t              do_hook = false;
public:
    void hooking()
    {
        hook = true;
        status = 0;
        headers = nullptr;
        body = nullptr;
    }

    void state(ngx_int_t rc)
    {
        if(status)
        {
            return;
        }

        if(rc == NGX_OK)
        {
            status = NGX_HTTP_OK;
            return;
        }

        if(rc < NGX_OK)
        {
            status = NGX_HTTP_INTERNAL_SERVER_ERROR;
            return;
        }

        status = rc;
    }
};

//NGX_MOD_INSTANCE(NdgDataHookModule, ndg_data_hook_module)
NGX_MOD_CTX_INSTANCE(NdgDataHookModule, ndg_data_hook_module, NdgDataHookCtx)

#endif  //_NDG_DATA_HOOK_CONF_HPP

// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_SUBREQUEST_HANDLER_HPP
#define _NDG_SUBREQUEST_HANDLER_HPP

//#include <iostream>
//using namespace std;

#include "NdgSubrequestConf.hpp"
#include "../datahook/NdgDataHookConf.hpp"

class NdgSubrequestCallback final
{
public:
    typedef NgxLogDebug log;
    typedef NdgDataHookModule hook_module;
public:
    static ngx_int_t sub_post(ngx_http_request_t* r, void* data, ngx_int_t rc)
    {
        // r is subrequest
        auto& pr_ctx_data = hook_module::data(r->parent);
        pr_ctx_data.state(rc);

        return NGX_OK;
    }

    static ngx_int_t parent_post(ngx_http_request_t* r)
    {
        // r is parent request
        auto& ctx_data = hook_module::data(r);

        if(ctx_data.status != NGX_HTTP_OK)
        {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        NgxHeadersOut h(ctx_data.headers);
        NgxHeadersChecker()(h);

        ngx_str_t msg = ngx_string("subrequest");

        NgxChain chain = ctx_data.body;
        auto len = msg.len + chain.size();

        NgxResponse resp(r);

        resp.length(len);

        h.list().visit(
            [&](ngx_table_elt_t& x)
            {
                resp.headers().add(x);
            });

        for(auto& ch : chain)
        {
            if(ch.data().special())
            {
                continue;
            }

            if(ch.data().last())
            {
                ch.data().finish(false);
            }

            resp.send(ch.data());
        }
        resp.send(&msg);
        resp.eof();

        return NGX_OK;
    }
};

class NdgSubrequestHandler final
{
public:
    typedef NdgSubrequestHandler this_type;
    typedef NdgSubrequestCallback callback_type;
    typedef NdgDataHookModule hook_module;

    typedef NdgSubrequestModule this_module;
    typedef NgxSubRequest<callback_type> this_subrequest;
public:
    static ngx_int_t handler(ngx_http_request_t *r)
    try
    {
        NgxRequest req(r);
        if(!req.method(NGX_HTTP_GET))
        {
            return NGX_HTTP_NOT_ALLOWED;
        }

        // get config
        auto& cf = this_module::instance().conf().loc(r);

        this_subrequest(r).create(cf.loc, cf.args);

        hook_module::data(r).hooking();

        return NGX_DONE;
    }
    catch(const NgxException& e)
    {
        return e.code();
    }
};

#endif  //_NDG_SUBREQUEST_HANDLER_HPP

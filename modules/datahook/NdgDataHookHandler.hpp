// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_DATA_HOOK_HANDLER_HPP
#define _NDG_DATA_HOOK_HANDLER_HPP

#include "NdgDataHookConf.hpp"

class NdgDataHookHandler final
{
public:
    typedef NdgDataHookHandler this_type;
    typedef NdgDataHookCtx ctx_type;

    typedef NdgDataHookModule this_module;
    typedef NgxFilter<this_type> this_filter;
public:
    static ngx_int_t init(ngx_conf_t* cf)
    {
        this_filter::init(&this_type::header_filter, &this_type::body_filter);
        return NGX_OK;
    }
public:
    static ngx_int_t header_filter(ngx_http_request_t *r)
    try
    {
        auto pr = r->parent;
        if(!pr)     //no parent request
        {
            return this_filter::next(r);
        }

        auto& ctx = this_module::ctx();

        if(ctx.empty(pr))   // no parent ctx set
        {
            return this_filter::next(r);
        }

        auto& pr_ctx_data = this_module::data(pr);

        if(!pr_ctx_data.hook)   // no parent hook flag
        {
            return this_filter::next(r);
        }

        this_module::data(r).do_hook = true;

        //pr_ctx_data.status = r->headers_out.status;
        pr_ctx_data.headers = &r->headers_out;      // subrequest's headers

        r->filter_need_in_memory = true;
        r->header_sent = true;

        return NGX_OK;      // stop header filter chain
    }
    catch(const NgxException& e)
    {
        return e.code();
    }

public:
    static ngx_int_t body_filter(ngx_http_request_t *r, ngx_chain_t *in)
    try
    {
        if(!in)
        {
            return NGX_OK;  //this_filter::next(r, in);
        }

        if( this_module::ctx().empty(r) ||
           !this_module::data(r).do_hook)
        {
            return this_filter::next(r, in);
        }

        // parent ctx data
        auto& pr_ctx_data = this_module::data(r->parent);

        // copy to parent ctx
        auto rc = ngx_chain_add_copy(r->pool, &pr_ctx_data.body, in);

        NgxException::require(rc);

        return NGX_OK;      // stop body filter chain
    }
    catch(const NgxException& e)
    {
        return e.code();
    }

};

#endif  //_NDG_DATA_HOOK_HANDLER_HPP

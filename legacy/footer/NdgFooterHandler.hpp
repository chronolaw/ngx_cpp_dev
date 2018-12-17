// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NDG_FOOTER_HANDLER_HPP
#define _NDG_FOOTER_HANDLER_HPP

#include "NdgFooterConf.hpp"

class NdgFooterHandler final
{
public:
    typedef NdgFooterHandler this_type;
    typedef NdgFooterModule this_module;
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
        do_header_filter(r);
        return this_filter::next(r);
    }
    catch(const NgxException& e)
    {
        return e.code();
    }

    static void do_header_filter(ngx_http_request_t *r)
    {
        auto& ctx = this_module::data(r);
        if(ctx.flag)
        {
            return;
        }

        ctx.flag = 1;
        NgxResponse resp(r);

        auto& cf = this_module::conf().loc(r);

        NgxKvArray headers = cf.headers;

        for(auto i = 0u; i < headers.size();++i)
        {
            auto& header = headers[i];

            ngx_table_elt_t kv;

            kv.hash = 1;
            kv.key = header.key;
            kv.value = header.value;

            resp.headers().add(kv);
        }

        // check r->header_only r->method & NGX_HTTP_HEAD
        NgxString footer = cf.footer;
        if(footer.empty())
        {
            return;
        }

        auto len = resp.headers()->content_length_n;
        if(len > 0)
        {
            resp.length(len + footer.size());
        }
    }

public:
    static ngx_int_t body_filter(ngx_http_request_t *r, ngx_chain_t *in)
    try
    {
        do_body_filter(r, in);
        return this_filter::next(r, in);
    }
    catch(const NgxException& e)
    {
        return e.code();
    }

    static void do_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
    {
        auto& cf = this_module::conf().loc(r);

        NgxString footer = cf.footer;
        if(footer.empty())
        {
            return;
        }

        auto& ctx = this_module::data(r);
        if(ctx.flag != 1)
        {
            return;
        }

        NgxChain ch = in;
        auto p = ch.begin();
        for(; p != ch.end();++p)
        {
            if(p->data().last())
            {
                break;
            }
        }

        if(p == ch.end())
        {
            return;
        }

        ctx.flag = 2;

        NgxPool pool(r);

        NgxBuf buf = pool.buffer();
        buf.range(footer);
        buf.finish();

        if(!p->data().size())
        {
            p->set(buf);
            return;
        }

        NgxChainNode n = pool.chain();
        n.set(buf);
        n.finish();

        p->link(n);
        p->data().finish(false);
    }
};

#endif  //_NDG_FOOTER_HANDLER_HPP

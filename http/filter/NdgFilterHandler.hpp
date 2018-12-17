// Copyright (c) 2015-2018
// Author: Chrono Law
#ifndef _NDG_FILTER_HANDLER_HPP
#define _NDG_FILTER_HANDLER_HPP

#include "NdgFilterConf.hpp"

class NdgFilterHandler final
{
public:
    typedef NdgFilterHandler        this_type;
    typedef NdgFilterModule         this_module;
    typedef NgxFilter<this_type>    this_filter;
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

        auto len = resp->headers_out.content_length_n;
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

        auto p = in;
        for (; p; p = p->next) {
            if (p->buf->last_buf) {
                break;
            }
        }

        // eof not find
        if (p == NULL) {
            return;
        }

        ctx.flag = 2;

        NgxPool pool(r);

        auto b = pool.buffer();

        b->pos = cf.footer.data;
        b->last = cf.footer.data + cf.footer.len;
        b->memory = 1;
        b->last_buf = 1;
        b->last_in_chain = 1;

        // set to the last node
        if (ngx_buf_size(p->buf) == 0) {
            p->buf = b;
            return;
        }

        // add a new last node
        auto out = pool.chain();
        out->buf = b;
        out->next = NULL;

        // link to the new node
        p->next = out;
        p->buf->last_buf = 0;
        p->buf->last_in_chain = 0;

        return; //to next filter
    }
};

#endif  //_NDG_FILTER_HANDLER_HPP

// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NDG_UPSTREAM_HANDLER_HPP
#define _NDG_UPSTREAM_HANDLER_HPP

#include "NdgUpstreamConf.hpp"

class NdgUpstreamCallback final
{
public:
    typedef NgxUpstream upstream_type;
    typedef NgxLogDebug log;
public:
    static ngx_int_t create_request(ngx_http_request_t* r)
    {
        ngx_str_t msg = ngx_string("xxxx");

        if(r->args.len >= 4)
        {
            msg = r->args;
            msg.len = 4;
        }

        NgxPool pool(r);
        NgxUpstream u(r);

        NgxBuf buf = pool.buffer();
        buf.range(msg);

        NgxChainNode ch = pool.chain();
        ch.set(buf);

        log(r).print("proxy %V", msg);
        u.request(ch);

        log(r).print("send:%V", &r->args);

        return NGX_OK;
    }

    static ngx_int_t reinit_request(ngx_http_request_t* r)
    {
        return NGX_OK;
    }

    static ngx_int_t process_header(ngx_http_request_t* r)
    {
        NgxUpstream u(r);
        NgxBuf buf = u.buffer();

        if(buf.size() < 2)
        {
            return NGX_AGAIN;
        }

        auto p = buf->pos;

        u.headers().content_length_n = (p[0] << 8) + p[1];

        u.headers().status_n = NGX_HTTP_OK;

        u.state().status = NGX_HTTP_OK;

        buf.consume(2);

        return NGX_OK;
    }

    // input_filter
};

class NdgUpstreamHandler final
{
public:
    typedef NdgUpstreamHandler this_type;
    typedef NdgUpstreamCallback callback_type;

    typedef NdgUpstreamModule this_module;
    typedef NgxUpstreamHelper<
                    &callback_type::create_request,
                    &callback_type::reinit_request,
                    &callback_type::process_header>
            this_upstream;
public:
    static ngx_int_t handler(ngx_http_request_t *r)
    try
    {
        NgxRequest req(r);
        if(!req.method(NGX_HTTP_GET))
        {
            return NGX_HTTP_NOT_ALLOWED;
        }

        this_upstream u(r);

        // get config
        auto& cf = this_module::conf().loc(r);

        // set upstream confg
        u.conf(cf.upstream);
        //u.buffering(cf.upstream.buffering);

        //u->resolved = NgxPool(r).alloc<ngx_http_upstream_resolved_t>();
        //u->resolved->sockaddr = local_addr();
        //u->resolved->socklen = sizeof(sockaddr_in);
        //u->resolved->naddrs = 1;

        return u.start();
    }
    catch(const NgxException& e)
    {
        return e.code();
    }

    static sockaddr* local_addr()
    {
        static sockaddr_in addr;

        addr.sin_family = AF_INET;
        addr.sin_port = htons(80);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        return (sockaddr*)&addr;
    }
};

#endif  //_NDG_UPSTREAM_HANDLER_HPP

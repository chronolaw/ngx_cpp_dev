// Copyright (c) 2015
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
        ngx_str_t msgs[] = {
            r->request_line,
            ngx_string("\r\n"),
            ngx_string("Host: localhost\r\n"),
            ngx_string("\r\n"),
            ngx_null_string
        };

        NgxPool pool(r);
        NgxUpstream u(r);

        NgxString s = msgs[0];
        for(auto i = 0u;!s.empty();s = msgs[++i])
        {
            NgxBuf buf = pool.buffer();
            buf.range(s);

            NgxChainNode ch = pool.chain();
            ch.set(buf);

            log(r).print("proxy %V", s.get());
            u.request(ch);
        }

        log(r).print("send:%V", &r->request_line);

        return NGX_OK;
    }

    static ngx_int_t process_header(ngx_http_request_t* r)
    {
        NgxUpstream u(r);

        u.headers().status_n = NGX_HTTP_OK;
        u.headers().content_length_n = 192;

        u.state().status = NGX_HTTP_OK;

        // do others ...

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
    typedef NgxUpstreamHelper<&callback_type::create_request,
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
        auto& cf = this_module::instance().conf().loc(r);

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

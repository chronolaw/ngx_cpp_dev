// Copyright (c) 2015-2018
// Author: Chrono Law
#ifndef _NDG_ECHO_HANDLER_HPP
#define _NDG_ECHO_HANDLER_HPP

#include <iostream>

#include "NdgEchoConf.hpp"

class NdgEchoHandler final
{
public:
    typedef NdgEchoHandler this_type;
    typedef NdgEchoModule  this_module;
public:
    static ngx_int_t handler(ngx_http_request_t *r)
    try
    {
        using namespace std;

        NgxRequest req(r);

        if(!req.method(NGX_HTTP_GET))
        {
            return NGX_HTTP_NOT_ALLOWED;
        }

        req.body().discard();

        auto& cf = this_module::conf().loc(r);

        NgxString msg = cf.msg;

        // check args
        NgxString args = req->args;

        auto len = msg.size();
        if(!args.empty())
        {
            len += args.size() + 1;
        }

        NgxResponse resp(r);

        //auto h = resp.headers();

        resp.length(len);
        resp.status(NGX_HTTP_OK);

        if(!args.empty())
        {
            resp.send(args);
            resp.send(",");
        }

        resp.send(msg);

        return resp.eof();
    }
    catch(const NgxException& e)
    {
        return e.code();
    }
};

#endif  //_NDG_ECHO_HANDLER_HPP

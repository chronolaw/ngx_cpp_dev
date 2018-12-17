// Copyright (c) 2015-2017
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
    {
        //req_line(r);
        //req_headers(r);

        return process(r);
    }
private:
    static ngx_int_t process(ngx_http_request_t *r)
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
#ifdef ENABLE_SCRIPT
        auto str = cf.var.str(r);
        NgxString msg = str;

        // warning. do not use like this
        //NgxString msg = cf.var.str(r);
#else
        NgxString msg = cf.msg;
#endif
        // check args
        NgxString args = req->args;

        auto len = msg.size();
        if(!args.empty())
        {
            len += args.size()+1;
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
        resp.flush();

        return resp.eof();
    }
    catch(const NgxException& e)
    {
        return e.code();
    }

    static void req_line(ngx_http_request_t *r)
    {
        using namespace std;

        cout << r->request_line<< endl;
        cout << r->method_name << endl;
        cout << r->http_protocol<< endl;
        cout << r->uri<< endl;
        cout << r->args<< endl;
        cout << r->exten<< endl;
        cout << r->unparsed_uri<< endl;

        //NgxBuf b(r->header_in);
        //cout << b.boundary().len << endl;
        NgxVarManager var(r);
        cout << "var:";
        //cout << var.get("request_method")<< endl;
        cout << var["request_method"]<< endl;
        cout << var["var1"]<< endl;
        assert(!var["abc"].get().len);

        //var.set("var1", "1234567");
        var["var1"] = "1234567";
        cout << var["var1"]<< endl;
    }

    static void req_headers(ngx_http_request_t *r)
    {
        using namespace std;

        NgxRequest req(r);

        auto hi = req.headers();

        cout << "host = "<<hi.has("host") << endl;
        cout << "agent= "<<hi.has("user-agent") << endl;

        hi.list().visit(
            [](ngx_table_elt_t& kv){
                cout << kv.key << "=>"<< kv.value
                    << ":"<<kv.hash << endl;
            });
    }
};

#endif  //_NDG_ECHO_HANDLER_HPP

// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_ECHO_HANDLER_HPP
#define _NDG_ECHO_HANDLER_HPP

#include <iostream>

#include "NdgEchoConf.hpp"

class NdgEchoHandler final
{
public:
    typedef NdgEchoHandler this_type;
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
        //NgxPool pool(r);

        if(!req.method(NGX_HTTP_GET))
        {
            return NGX_HTTP_NOT_ALLOWED;
        }

        req.body().discard();

        auto& cf = NdgEchoModule::instance().conf().loc(r);
        NgxString msg = cf.msg;
        cout << "msg = " << msg << endl;

        // check args
        NgxString args = req->args;

        auto len = msg.size();
        if(!args.empty())
        {
            len += args.size()+1;
        }
        //cout << "len = "<< len << endl;

        NgxResponse resp(r);

        //auto h = resp.headers();

        resp.length(len);
        resp.status(NGX_HTTP_OK);
        //h->content_length_n = len;
        //h->status = NGX_HTTP_OK;
        //h->content_length_n = msg.size();
        //h->status = NGX_HTTP_OK;

        //NgxBuf buf = pool.buffer();
        //buf.range(msg);
        //resp.send(buf);
        if(!args.empty())
        {
            resp.send(args);
            resp.send(",");
        }

        resp.send(msg);
        cout << "send buf" << endl;
        //buf.finish();

        //buf->memory= true;
        //buf->last_buf = true;
        //cout << buf.range() << endl;

        //NgxChainNode ch = pool.chain();
        //ch.set(buf);
        //ch.finish();
        //assert(!ch->next);
        //assert(ch->buf == buf.get());

        resp.flush();
        cout << "flush" << endl;
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

        NgxVarManager var(r);
        cout << "var:";
        //cout << var.get("request_method")<< endl;
        cout << var["request_method"]<< endl;
        cout << var["var1"]<< endl;
        assert(!var.get("abc").len);

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

// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_TEST_HANDLER_HPP
#define _NDG_TEST_HANDLER_HPP

#include <iostream>

#include "NdgTestConf.hpp"

//NGX_MOD_INSTANCE(XHttpCoreModule, ngx_http_core_module,
//    ngx_http_core_main_conf_t,ngx_http_core_srv_conf_t,ngx_http_core_loc_conf_t )

class NdgTestHandler final
{
public:
    typedef NdgTestHandler this_type;
public:
    static ngx_int_t init(ngx_conf_t* cf)
    {
        //auto cmcf = XHttpCoreModule::instance().conf().main(cf);

        //typedef NgxArray<ngx_http_handler_pt> handler_array_t;

        //handler_array_t arr(cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers);
        //arr.push(&this_type::handler);
        NgxHttpCoreModule::instance().handler(
            cf, &this_type::handler, NGX_HTTP_REWRITE_PHASE);

        return NGX_OK;
    }

public:
    static ngx_int_t handler(ngx_http_request_t *r)
    try
    {
        NgxClock clock;

        auto& cf = NdgTestModule::instance().conf().loc(r);

        NgxLogError(r).print("hello c++");

        if (!cf.enabled)
        {
            std::cout << "hello disabled" << std::endl;
            return NGX_DECLINED;
        }

        std::cout << "hello nginx" << std::endl;

        //ngx_sleep(1);
        std::cout << clock.elapsed() << "s"<< std::endl;

        array(r);
        list(r);
        buf(r);
        codec(r);
        escape(r);
        digest();
        timer();
        //printf(r);

        return NGX_DECLINED;
    }
    catch(const NgxException& e)
    {
        return e.code();
    }
private:
    static void array(ngx_http_request_t *r)
    {
        NgxArray<ngx_int_t> arr(r);
        assert(arr.size() == 0);

        arr.push(42);
        arr.push(253);
        assert(arr[0] == 42);

        arr.visit(
            [](ngx_int_t x){
                std::cout << x << ",";
            });
        std::cout << std::endl;
    }

    static void list(ngx_http_request_t *r)
    {
        NgxList<ngx_int_t> l(r, 1);

        l.push(2000);
        l.push(2008);
        l.push(2015);

        assert(*l.begin() = 2000);

        auto p = l.find(
            [](ngx_int_t x){
                return x == 2015;
            });
        assert(p != l.end());
        assert(*p == 2015);

        //for(auto p = l.begin();p;++p)
        //{
        //    std::cout << *p << ",";
        //}
        for(auto& x : l)
        {
            std::cout << x << ",";
        }
        std::cout << std::endl;
    }

    static void buf(ngx_http_request_t *r)
    {
        using namespace std;
        cout << "test buf and chain" << endl;

        NgxPool pool(r);
        NgxBuf buf(pool.buffer(10));

        assert(buf.memory() && buf.memoryonly());
        assert(buf.range().len == 0);
        assert(buf.boundary().len == 10);

        ngx_memset(buf.range().data, 'a', 5);
        buf->last += 5;
        assert(buf.range().len == 5);
        cout << "str="<< buf.range() << endl;

        NgxChain ch = pool.chain();

        auto node = *ch.begin();
        //cout << (node->next ? 1:0) << endl;
        assert(node.last());
        //node.data() = buf;
        node.set(buf);
        cout << node.data().range() << endl;

        NgxChain ch2 = pool.chain(ngx_bufs_t{2, 10});
        node.link(ch2);
        cout << "chain len = "<< ch.size() << endl;

        for(auto&x : ch)
        {
            auto b = x.data();
            ngx_memset(b->pos, 'b', 5);
            b->last = b->pos + 5;
            cout << b.range() << ";";
        }
        cout << endl;
        cout << "chain len2 = "<< ch.size() << endl;
    }

    static void printf(ngx_http_request_t *r)
    {
        using namespace std;

        ngx_int_t i = -100;
        unsigned long ul = 65535L;
        double f = 0.618;
        ngx_str_t str = ngx_string("metroid");

        cout << "test printf" << endl;
        NgxLogError(r).print(
            "%i,%uL,%.5f,%V,%uxz,%p,%P",
            i, ul, f, &str, ul, str.data,ngx_getpid()
            );
    }
    static void digest()
    {
        using namespace std;

        NgxMd5 md5;
        NgxSha1 sha1;
        NgxMurmurHash2 murmur2;

        cout << md5("abcd") <<endl;
        cout << sha1("abcd") <<endl;
        cout << murmur2("abcd") << endl;
    }
    static void codec(ngx_http_request_t* r)
    {
        using namespace std;

        ngx_str_t src = ngx_string("mario");
        ngx_str_t dst;

        dst.len = ngx_base64_encoded_length(src.len);
        dst.data = NgxPool(r).nalloc<u_char>(dst.len);

        ngx_encode_base64(&dst, &src);
        cout << dst << endl;
        ngx_encode_base64url(&dst, &src);
        cout << dst << endl;
    }
    static void escape(ngx_http_request_t* r)
    {
        using namespace std;

        ngx_str_t src = ngx_string("test&/echo?name=#link");
        ngx_str_t dst;

        dst.len = src.len+2*ngx_escape_uri(nullptr,src.data, src.len, NGX_ESCAPE_URI);
        cout << "len = "<<dst.len << endl;
        dst.data = NgxPool(r).nalloc<u_char>(dst.len);

        ngx_escape_uri(dst.data, src.data, src.len, NGX_ESCAPE_URI);
        cout << dst << endl;

        ngx_str_t html = ngx_string("<html>");
        ngx_str_t out;

        out.len = html.len+ngx_escape_html(nullptr, html.data, html.len);
        out.data = NgxPool(r).nalloc<u_char>(out.len);
        ngx_escape_html(out.data, html.data, html.len);
        cout << out << endl;
    }
private:
    static void timer_call()
    {
        std::cout<< "timer"<< std::endl;
        NgxTimer::add(1000, &this_type::timer_call);
    }

    static void timer()
    {
        using namespace std;

        //auto n = NgxClock::now();
        //cout << n.gmtoff << endl;
        //std::cout << NgxDatetime::today() << std::endl;
        //std::cout << (char*)NgxDatetime::today() << std::endl;
        std::cout << NgxDatetime::today() << std::endl;
        std::cout << NgxDatetime::http() << std::endl;
        //NgxTimer::add(5000, [](){std::cout<< "timer"<< std::endl;});

        auto t = NgxDatetime::since();
        auto s = NgxDatetime::http(t);
        assert(t == NgxDatetime::http(s));
        NgxTimer::add(2000, &this_type::timer_call);
        //NgxTimer::add(2000, [](){timer_call();});
    }
};

#endif  //_NDG_TEST_HANDLER_HPP

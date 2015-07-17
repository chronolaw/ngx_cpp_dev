// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_HTTP_MODULE_HPP
#define _NGX_HTTP_MODULE_HPP

#include "NgxModule.hpp"
#include "NgxArray.hpp"

class NgxHttpCoreModule final :
     public NgxModule<ngx_http_core_loc_conf_t,
                      ngx_http_core_srv_conf_t,
                      ngx_http_core_main_conf_t>
{
public:
    typedef NgxModule<ngx_http_core_loc_conf_t,
                      ngx_http_core_srv_conf_t,
                      ngx_http_core_main_conf_t>
            super_type;
    typedef NgxHttpCoreModule this_type;
public:
    NgxHttpCoreModule() : super_type(ngx_http_core_module)
    {}
    ~NgxHttpCoreModule() = default;
public:
    // content handler
    template<typename F>
    void handler(ngx_conf_t* cf, F f) const
    {
        conf().loc(cf).handler = f;
    }

    // phase handler
    template<typename F>
    void handler(ngx_conf_t* cf, F f, ngx_http_phases p) const
    {
        auto& c = conf().main(cf);

        typedef NgxArray<ngx_http_handler_pt> handler_array_t;

        assert(p <= NGX_HTTP_LOG_PHASE);
        handler_array_t arr(c.phases[p].handlers);
        arr.push(f);
    }

public:
    static NgxHttpCoreModule& instance()
    {
        static NgxHttpCoreModule m;
        return m;
    }
};

class NgxHttpUpstreamModule final :
    public NgxModule<void,
                    ngx_http_upstream_srv_conf_t,
                    ngx_http_upstream_main_conf_t>
{
public:
    typedef NgxModule<void,
                      ngx_http_upstream_srv_conf_t,
                      ngx_http_upstream_main_conf_t>
            super_type;
    typedef NgxHttpUpstreamModule this_type;
public:
    NgxHttpUpstreamModule() : super_type(ngx_http_upstream_module)
    {}
    ~NgxHttpUpstreamModule() = default;
public:
    static NgxHttpUpstreamModule& instance()
    {
        static NgxHttpUpstreamModule m;
        return m;
    }
public:
    template<typename F>
    void init(ngx_conf_t* cf, F f, ngx_uint_t flags = 0) const
    {
        auto& uscf = conf().srv(cf);

        NgxException::fail(uscf.peer.init_upstream);

        uscf.peer.init_upstream = f;
        uscf.flags = flags;
    }
};

#endif  //_NGX_HTTP_MODULE_HPP

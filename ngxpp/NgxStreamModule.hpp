// Copyright (c) 2017
// Author: Chrono Law
#ifndef _NGX_STREAM_MODULE_HPP
#define _NGX_STREAM_MODULE_HPP

//#include <type_traits>
//#include <boost/utility/enable_if.hpp>

#include "NgxModule.hpp"

class NgxStreamCoreModule final :
     public NgxModule<ngx_stream_core_srv_conf_t,
                      ngx_stream_core_main_conf_t>
{
public:
    typedef NgxModule<ngx_stream_core_srv_conf_t,
                      ngx_stream_core_main_conf_t>
            super_type;
    typedef NgxStreamCoreModule this_type;
public:
    NgxStreamCoreModule() : super_type(ngx_stream_core_module)
    {}

    ~NgxStreamCoreModule() = default;
public:
    // content handler
    template<typename F>
    static void handler(ngx_conf_t* cf, F f)
    {
        instance().conf().srv(cf).handler = f;
    }

public:
#if (nginx_version < 1011005)
    static void access(ngx_conf_t* cf, ngx_stream_access_pt f)
    {
        instance().conf().main(cf).access_handler = f;
    }

    static void limit_conn(ngx_conf_t* cf, ngx_stream_access_pt f)
    {
        instance().conf().main(cf).limit_conn_handler = f;
    }
#else   // nginx 1.11.5 or later
    // phase handler
    template<typename F>
    static void handler(ngx_conf_t* cf, F f, ngx_stream_phases p)
    {
        auto& c = instance().conf().main(cf);

        typedef NgxArray<ngx_stream_handler_pt> handler_array_t;

        assert(p <= NGX_STREAM_LOG_PHASE);
        handler_array_t arr(c.phases[p].handlers);
        arr.push(f);
    }
#endif
public:
    static this_type& instance()
    {
        static this_type m;
        return m;
    }
};

#endif  //_NGX_STREAM_MODULE_HPP

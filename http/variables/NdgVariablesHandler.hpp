// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_VARIABLES_HANDLER_HPP
#define _NDG_VARIABLES_HANDLER_HPP

#include "NdgVariablesConf.hpp"

class NdgVariablesHandler final
{
public:
    typedef NdgVariablesHandler this_type;
public:
    static ngx_http_variable_t* get_vars()
    {
        static ngx_http_variable_t  vars[] = {

            { ngx_string("rtt"), nullptr,
                &this_type::get_rtt, 0,
                NGX_HTTP_VAR_NOCACHEABLE, 0 },

            { ngx_null_string, nullptr, nullptr, 0, 0, 0 }
        };

        return vars;
    }
public:
    static ngx_int_t get_rtt(
        ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
    {
        boost::ignore_unused(data);

        auto sockfd = r->connection->fd;

        tcp_info info;
        socklen_t len = sizeof(info);

        auto rc = getsockopt(sockfd, SOL_TCP, TCP_INFO, &info, &len);
        assert(!rc);

        auto buf = NgxPool(r).nalloc<u_char>(10);
        ngx_str_t str{10, buf};

        NgxString(str).printf("%d", info.tcpi_rtt / 1000);

        NgxVariableValue(v).set(str);

        return NGX_OK;
    }
};

#endif  //_NDG_VARIABLES_HANDLER_HPP

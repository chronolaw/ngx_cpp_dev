// Copyright (c) 2015-2017
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

            { ngx_string("today"), nullptr,
              &this_type::get_today, 0, 0, 0 },

            { ngx_string("rtt"), nullptr,
              &this_type::get_rtt, 0,
              NGX_HTTP_VAR_NOCACHEABLE, 0 },

            { ngx_string("current_method"), nullptr,
              &this_type::get_current_method, 0,
              NGX_HTTP_VAR_NOCACHEABLE, 0 },

            { ngx_string("parsed_uri"), nullptr,
              &this_type::get_parsed_uri, 0,
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
public:
    static ngx_int_t get_today(
        ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
    {
        boost::ignore_unused(data);

        auto str = NgxDatetime::today();

        NgxVariableValue(v).set(str);

        return NGX_OK;
    }

    static ngx_int_t get_current_method(
        ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
    {
        boost::ignore_unused(data);

        NgxVariableValue(v).set(r->method_name);

        return NGX_OK;
    }

    static ngx_int_t get_parsed_uri(
        ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
    {
        boost::ignore_unused(data);

        NgxVariableValue(v).set(r->uri);

        return NGX_OK;
    }
};

#endif  //_NDG_VARIABLES_HANDLER_HPP

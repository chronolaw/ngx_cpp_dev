// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_DATETIME_HPP
#define _NGX_DATETIME_HPP

//#include "Nginx.hpp"

class NgxDatetime final
{
public:
    NgxDatetime() = default;
    ~NgxDatetime() = default;
public:
    static std::time_t since()
    {
        return ngx_time();
    }

    static ngx_str_t today()
    {
        ngx_tm_t tm;

        //ngx_gmtime(since(), &tm);
        ngx_localtime(since(), &tm);

        static u_char buf[20] = {};

        auto p = ngx_snprintf(buf, 20,
                    "%d-%02d-%02d",
                    tm.ngx_tm_year, tm.ngx_tm_mon, tm.ngx_tm_mday);

        return ngx_str_t{static_cast<std::size_t>(p - buf), buf};
    }
public:
    static ngx_str_t http(std::time_t t = since())
    {
        static u_char buf[50] = {};

        auto p = ngx_http_time(buf, t);

        return ngx_str_t{static_cast<std::size_t>(p - buf), buf};
    }

    static std::time_t http(ngx_str_t& str)
    {
        return ngx_parse_http_time(str.data, str.len);
    }
};

#endif  //_NGX_DATETIME_HPP

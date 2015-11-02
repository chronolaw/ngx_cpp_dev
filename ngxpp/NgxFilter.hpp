// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_FILTER_HPP
#define _NGX_FILTER_HPP

// use Tag to instance static var
template<typename Tag>
class NgxFilter final
{
public:
    typedef NgxFilter this_type;
    typedef ngx_http_output_header_filter_pt header_filter_ptr;
    typedef ngx_http_output_body_filter_pt body_filter_ptr;

    NgxFilter() = default;
    ~NgxFilter() = default;

public:
    static void init(header_filter_ptr header_filter = nullptr,
                     body_filter_ptr body_filter = nullptr)
    {
        if(header_filter)
        {
            set(ngx_http_top_header_filter, header_filter);
        }

        if(body_filter)
        {
            set(ngx_http_top_body_filter, body_filter);
        }
    }
public:
    static ngx_int_t next(ngx_http_request_t* r)
    {
        return next<header_filter_ptr>()(r);
    }

    static ngx_int_t next(ngx_http_request_t* r, ngx_chain_t *chain)
    {
        return next<body_filter_ptr>()(r, chain);
    }
private:
    template<typename T>
    static void set(T& top, T p)
    {
        next<T>() = top;
        top = p;
    }

    template<typename T>
    static T& next()
    {
        static T next_filter;

        return next_filter;
    }
};

// now only request body filter
template<typename Tag>
class NgxRequestFilter final
{
public:
    typedef NgxRequestFilter this_type;
    typedef ngx_http_request_body_filter_pt body_filter_ptr;

    NgxRequestFilter() = default;
    ~NgxRequestFilter() = default;

public:
    static void init(body_filter_ptr body_filter = nullptr)
    {
        if(body_filter)
        {
            set(ngx_http_top_request_body_filter, body_filter);
        }
    }
public:
    static ngx_int_t next(ngx_http_request_t* r, ngx_chain_t *chain)
    {
        return next<body_filter_ptr>()(r, chain);
    }
private:
    template<typename T>
    static void set(T& top, T p)
    {
        next<T>() = top;
        top = p;
    }

    template<typename T>
    static T& next()
    {
        static T next_filter;

        return next_filter;
    }
};

#endif  //_NGX_FILTER_HPP

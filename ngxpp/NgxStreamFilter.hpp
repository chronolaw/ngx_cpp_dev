// Copyright (c) 2016-2017
// Author: Chrono Law
#ifndef _NGX_STREAM_FILTER_HPP
#define _NGX_STREAM_FILTER_HPP

// use Tag to instance static var
template<typename Tag>
class NgxFilter final
{
public:
    typedef NgxFilter this_type;
    typedef ngx_stream_filter_pt stream_filter_ptr;

    NgxFilter() = default;
    ~NgxFilter() = default;

public:
    static void init(stream_filter_ptr filter = nullptr)
    {
        if(filter)
        {
            set(ngx_stream_top_filter, filter);
        }
    }
public:
    static ngx_int_t next(ngx_stream_session_t* s,
                          ngx_chain_t *chain, ngx_uint_t from_upstream)
    {
        return next<stream_filter_ptr>()(s, chain, from_upstream);
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

#endif  //_NGX_STREAM_FILTER_HPP

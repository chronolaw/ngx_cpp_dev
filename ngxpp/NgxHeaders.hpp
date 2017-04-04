// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_HEADERS_HPP
#define _NGX_HEADERS_HPP

//#include <boost/utility/string_ref.hpp>
//#include <boost/algorithm/string.hpp>

#include "NgxList.hpp"
#include "NgxString.hpp"

// T = ngx_http_headers_in_t/ngx_http_headers_out_t
template<typename T,
         T ngx_http_request_t::* ptr>
class NgxHeaders final : public NgxWrapper<T>
{
public:
    typedef boost::string_ref string_ref_type;

    typedef NgxWrapper<T> super_type;
    typedef typename super_type::wrapped_type ngx_headers_type;

    typedef NgxList<ngx_table_elt_t> ngx_headers_list_type;
    typedef ngx_table_elt_t kv_type;

public:
    NgxHeaders(ngx_headers_type& h):
        super_type(h),
        m_headers(h.headers)
    {}

    NgxHeaders(ngx_headers_type* h):NgxHeaders(*h)
    {}

    NgxHeaders(ngx_http_request_t* r):
        NgxHeaders(r->*ptr)
    {}

    ~NgxHeaders() = default;
public:
    const ngx_headers_list_type& list() const
    {
        return m_headers;
    }

    bool has(string_ref_type key) const
    {
        return find(key) != list().end();
    }

    ngx_str_t operator[](string_ref_type key) const
    {
        auto p = find(key);

        if(p == list().end())
        {
            return ngx_null_string;
        }

        return p->value;
    }

    ngx_str_t operator[](const char* key) const
    {
        return operator[](string_ref_type(key));
    }
public:
    void add(const kv_type& kv) const
    {
        auto& tmp = list().prepare();

        tmp = kv;
        if(!tmp.hash)
        {
            tmp.hash = 1;
        }
    }

    void remove(string_ref_type key) const
    {
        auto p = find(key);

        if(p == list().end())
        {
            return;
        }

        p->hash = 0;
    }

public:
    ngx_headers_list_type::iterator find(string_ref_type key) const
    {
        auto v = [&](const kv_type& kv)
        {
            //if(!kv.hash)
            //{
            //    return false;
            //}

            return boost::iequals(
                        NgxString(kv.key).str(), key);
        };

        return list().find(v);
    }
private:
    ngx_headers_list_type m_headers;
};

typedef NgxHeaders<
            ngx_http_headers_in_t, &ngx_http_request_t::headers_in>
        NgxHeadersIn;

typedef NgxHeaders<
            ngx_http_headers_out_t, &ngx_http_request_t::headers_out>
        NgxHeadersOut;

//#include <iostream>

class NgxHeadersChecker final
{
public:
    template<typename T>
    void operator()(T& h)
    {
        //std::cout << "headers "<< std::endl;

        // print headers
        auto v = [](ngx_table_elt_t& x)
        {
            // atention! must use & to print ngx_str_t!
            ngx_log_stderr(0, "%V<=>%V", &x.key, &x.value);

            //std::cout << x.key << "<=>"<< x.value << std::endl;
        };

        h.list().visit(v);
    }
};

#endif  //_NGX_HEADERS_HPP

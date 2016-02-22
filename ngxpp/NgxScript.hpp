// Copyright (c) 2015-2016
// Author: Chrono Law
#ifndef _NGX_SCRIPT_HPP
#define _NGX_SCRIPT_HPP

#include "NgxException.hpp"
#include "NgxValue.hpp"

class NgxScript final
{
public:
    NgxScript() = default;
    ~NgxScript() = default;

public:
    void init(ngx_conf_t* cf, ngx_str_t* source)
    {
        assert(source);

        m_source = *source;

        auto n = ngx_http_script_variables_count(source);

        if(!n)      // n==0, no vars
        {
            return;
        }

        ngx_http_script_compile_t sc;
        NgxValue::memzero(sc);

        sc.cf = cf;
        sc.source = &m_source;
        sc.lengths = &m_lengths;
        sc.values = &m_values;
        sc.variables = n;
        sc.complete_lengths = true;
        sc.complete_values = true;

        auto rc = ngx_http_script_compile(&sc);

        NgxException::require(rc);
    }

    void init(ngx_conf_t* cf, ngx_str_t& source)
    {
        init(cf,&source);
    }
public:
    ngx_str_t source() const
    {
        return m_source;
    }

    ngx_str_t str(ngx_http_request_t* r) const
    {
        if(!m_lengths)  // raw string
        {
            return m_source;
        }

        ngx_str_t value;

        auto pos = ngx_http_script_run(r, &value,
                        m_lengths->elts, 0, m_values->elts);

        NgxException::require(pos);

        return value;
    }
private:
    ngx_str_t       m_source = ngx_null_string;
    ngx_array_t*    m_lengths = nullptr;
    ngx_array_t*    m_values = nullptr;
};

#endif  //_NGX_SCRIPT_HPP

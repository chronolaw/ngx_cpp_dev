// Copyright (c) 2015-2016
// Author: Chrono Law
#ifndef _NGX_SCRIPT_HPP
#define _NGX_SCRIPT_HPP

#include "NgxException.hpp"
#include "NgxValue.hpp"

inline namespace ngx_script_types {
#ifdef ngx_http_variable

typedef ngx_http_request_t          ngx_script_ctx_t;
typedef ngx_http_script_compile_t   ngx_script_compile_t;

static auto ngx_script_count    = &ngx_http_script_variables_count;
static auto ngx_script_compile  = &ngx_http_script_compile;
static auto ngx_script_run     = &ngx_http_script_run;

#else   //ngx_stream_variable

typedef ngx_stream_session_t            ngx_script_ctx_t;
typedef ngx_stream_script_compile_t     ngx_script_compile_t;

static auto ngx_script_count    = &ngx_stream_script_variables_count;
static auto ngx_script_compile  = &ngx_stream_script_compile;
//static auto ngx_script_run     = &ngx_stream_script_run;

#endif
}

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

        auto n = ngx_script_count(source);

        if(!n)      // n==0, no vars
        {
            return;
        }

        ngx_script_compile_t sc;
        NgxValue::memzero(sc);

        sc.cf = cf;
        sc.source = &m_source;
        sc.lengths = &m_lengths;
        sc.values = &m_values;
        sc.variables = n;
        sc.complete_lengths = true;
        sc.complete_values = true;

        auto rc = ngx_script_compile(&sc);

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

    ngx_str_t str(ngx_script_ctx_t* r) const
    {
        if(!m_lengths)  // raw string
        {
            return m_source;
        }

        assert(m_values);

        ngx_str_t value;

        auto pos = ngx_script_run(r, &value,
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

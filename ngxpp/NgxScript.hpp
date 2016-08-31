// Copyright (c) 2015-2016
// Author: Chrono Law
#ifndef _NGX_SCRIPT_HPP
#define _NGX_SCRIPT_HPP

#include "NgxException.hpp"
#include "NgxValue.hpp"
#include "NgxPool.hpp"

inline namespace ngx_script_types {
#ifdef ngx_http_variable

typedef ngx_http_request_t                  ngx_script_ctx_t;

typedef ngx_http_compile_complex_value_t    ngx_compile_complex_value_t;
typedef ngx_http_complex_value_t            ngx_complex_value_t;

static auto ngx_compile_complex_value       = &ngx_http_compile_complex_value;
static auto ngx_complex_value               = &ngx_http_complex_value;

#else   //ngx_stream_variable

typedef ngx_stream_session_t                ngx_script_ctx_t;

typedef ngx_stream_compile_complex_value_t  ngx_compile_complex_value_t;
typedef ngx_stream_complex_value_t          ngx_complex_value_t;

static auto ngx_compile_complex_value       = &ngx_stream_compile_complex_value;
static auto ngx_complex_value               = &ngx_stream_complex_value;

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

        ngx_complex_value_t         cv;
        ngx_compile_complex_value_t ccv;

        NgxValue::memzero(ccv);

        ccv.cf = cf;
        ccv.value = source;
        ccv.complex_value = &cv;

        auto rc = ngx_compile_complex_value(&ccv);

        NgxException::require(rc);

        if(!cv.lengths)   // raw string
        {
            return;
        }

        m_cv = NgxPool(cf).alloc<ngx_complex_value_t>();

        *m_cv = cv;
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
        if(!m_cv)   // raw string
        {
            return m_source;
        }

        ngx_str_t value = ngx_null_string;

        auto rc = ngx_complex_value(r, m_cv, &value);

        NgxException::require(rc);

        return value;
    }
private:
    ngx_str_t           m_source = ngx_null_string;

    ngx_complex_value_t *m_cv = nullptr;
};

#endif  //_NGX_SCRIPT_HPP

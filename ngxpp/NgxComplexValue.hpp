// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_COMPLEX_VALUE_HPP
#define _NGX_COMPLEX_VALUE_HPP

#include "NgxException.hpp"
#include "NgxValue.hpp"
#include "NgxPool.hpp"

inline namespace ngx_complex_value_types {
#ifndef ngx_stream_cpp_version  //ngx_http_variable

using ngx_compile_ctx_t                 = ngx_http_request_t;

using ngx_compile_complex_value_t       = ngx_http_compile_complex_value_t;
using ngx_complex_value_t               = ngx_http_complex_value_t;

static auto ngx_compile_complex_value   = &ngx_http_compile_complex_value;
static auto ngx_complex_value           = &ngx_http_complex_value;

#else   //ngx_stream_variable

using ngx_compile_ctx_t                 = ngx_stream_session_t;

using ngx_compile_complex_value_t       = ngx_stream_compile_complex_value_t;
using ngx_complex_value_t               = ngx_stream_complex_value_t;

static auto ngx_compile_complex_value   = &ngx_stream_compile_complex_value;
static auto ngx_complex_value           = &ngx_stream_complex_value;

#endif
}

class NgxComplexValue final
{
public:
    NgxComplexValue() = default;
    ~NgxComplexValue() = default;

public:
    void init(ngx_conf_t* cf, ngx_str_t* source)
    {
        assert(source);

        m_source = *source;

        ngx_compile_complex_value_t ccv;

        NgxValue::memzero(ccv);

        ccv.cf = cf;
        ccv.value = source;
        ccv.complex_value = &m_cv;

        auto rc = ngx_compile_complex_value(&ccv);

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

    ngx_str_t str(ngx_compile_ctx_t* r)
    {
        ngx_str_t value = ngx_null_string;

        auto rc = ngx_complex_value(r, &m_cv, &value);

        NgxException::require(rc);

        return value;
    }
private:
    ngx_str_t           m_source = ngx_null_string;

    ngx_complex_value_t m_cv;
};

// alias
//typedef NgxComplexValue NgxScript;

#endif  //_NGX_COMPLEX_VALUE_HPP

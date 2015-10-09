// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_COMPLEX_VALUE_HPP
#define _NGX_COMPLEX_VALUE_HPP

#include "NgxException.hpp"
#include "NgxValue.hpp"

class NgxComplexValue final
{
public:
    NgxComplexValue() = default;
    ~NgxComplexValue() = default;

public:
    void init(ngx_conf_t* cf, ngx_str_t* str)
    {
        ngx_http_compile_complex_value_t   ccv;

        NgxValue::memzero(ccv);

        ccv.cf = cf;
        ccv.value = str;
        ccv.complex_value = &cv;

        auto rc = ngx_http_compile_complex_value(&ccv);

        NgxException::require(rc);
    }

public:
    ngx_str_t str(ngx_http_request_t* r)
    {
        ngx_str_t  val;

        auto rc = ngx_http_complex_value(r, &cv, &val);

        return rc == NGX_OK ? val : ngx_str_t ngx_null_string;
    }
private:
    ngx_http_complex_value_t           cv;
};

#endif  //_NGX_COMPLEX_VALUE_HPP

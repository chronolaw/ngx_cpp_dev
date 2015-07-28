// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_VARIABLES_INIT_HPP
#define _NDG_VARIABLES_INIT_HPP

#include "NdgVariablesConf.hpp"
#include "NdgVariablesHandler.hpp"

class NdgVariablesInit final
{
public:
    typedef NdgVariablesHandler handler_type;
    typedef NdgVariablesInit this_type;

    typedef NgxVariables<&handler_type::get_vars>
            these_variables;
public:
    static ngx_http_module_t* ctx()
    {
        static ngx_http_module_t c =
        {
            &these_variables::init,
            NGX_MODULE_NULL(7)
        };

        return &c;
    }
public:
    static const ngx_module_t& module()
    {
        static ngx_module_t m =
        {
            NGX_MODULE_V1,

            ctx(),
            NGX_MODULE_NULL(1),

            NGX_HTTP_MODULE,
            NGX_MODULE_NULL(7),
            NGX_MODULE_V1_PADDING
        };

        return m;
    }
};

#endif  //_NDG_VARIABLES_INIT_HPP

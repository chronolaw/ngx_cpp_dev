// Copyright (c) 2015
// Author: Chrono Law
#include "NdgTestInit.hpp"

auto ndg_test_module = NdgTestInit::module();

/*
#include <iostream>

#include "NgxAll.hpp"

struct NdgTestConf final
{
    ngx_flag_t enabled = NgxUnsetValue::get();
};

static void *create(ngx_conf_t* cf);
static char *merge(ngx_conf_t *cf, void *parent, void *child);

static ngx_command_t ndg_test_cmds[] =
{
    {
        ngx_string("ndg_test"),
        NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(NdgTestConf, enabled),
        nullptr
    },

    ngx_null_command
};

static ngx_int_t init(ngx_conf_t* cf);
static ngx_int_t handler(ngx_http_request_t *r);

static ngx_http_module_t ndg_test_ctx =
{
    nullptr,
    init,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    create,
    nullptr,//merge
};

ngx_module_t ndg_test_module =
{
    NGX_MODULE_V1,
    &ndg_test_ctx,
    ndg_test_cmds,
    NGX_HTTP_MODULE,
    nullptr,                                  // init master
    nullptr,                                  // init module
    nullptr,                                  // init process
    nullptr,                                  // init thread
    nullptr,                                  // exit thread
    nullptr,                                  // exit process
    nullptr,                                  // exit master
    NGX_MODULE_V1_PADDING
};

static void* create(ngx_conf_t* cf)
{
    return NgxPool(cf).alloc<NdgTestConf>();
}

static char *merge(ngx_conf_t *cf, void *parent, void *child)
{
    boost::ignore_unused(cf);

    auto prev = reinterpret_cast<NdgTestConf*>(parent);
    auto conf = reinterpret_cast<NdgTestConf*>(child);

    NgxValue::merge(conf->enabled, prev->enabled, 1);

    return NGX_CONF_OK;
}

static ngx_int_t init(ngx_conf_t* cf)
{
    auto cmcf = reinterpret_cast<ngx_http_core_main_conf_t*>(
        ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module));

    NgxArray<ngx_http_handler_pt> arr(
        cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers);

    arr.push(handler);

    return NGX_OK;
}

static ngx_int_t handler(ngx_http_request_t *r)
{
    auto cf = reinterpret_cast<NdgTestConf*>(
        ngx_http_get_module_loc_conf(r, ndg_test_module));

    NgxLogError(r).print("hello c++");

    if (cf->enabled)
    {
        std::cout << "hello nginx" << std::endl;
    }
    else
    {
        std::cout << "hello disabled" << std::endl;
    }


    return NGX_DECLINED;
}
*/


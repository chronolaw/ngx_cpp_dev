// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_CONFIG_HPP
#define _NGX_CONFIG_HPP

#include "NgxArray.hpp"

// http conf
#ifdef NGX_HTTP_LOC_CONF_OFFSET
#define NGX_CPP_DEV_CMD_OFFSET NGX_HTTP_LOC_CONF_OFFSET
#endif

// stream conf
#ifdef NGX_STREAM_SRV_CONF
#define NGX_CPP_DEV_CMD_OFFSET NGX_STREAM_SRV_CONF
#endif

#if __cplusplus >= 201402L
[[deprecated]] // deprecated in future
#endif
class NgxCommand final
{
private:
    ngx_command_t m_cmd = ngx_null_command;
public:
    template<typename T>
    NgxCommand(const ngx_str_t& n, ngx_uint_t t, T set,
            ngx_uint_t c = NGX_CPP_DEV_CMD_OFFSET,
            ngx_uint_t off = 0, void* p = nullptr):
        m_cmd{n, t, set, c, off, p}     //do not use () to initilize
    {}

    NgxCommand() = default;
    ~NgxCommand() = default;
public:
    operator const ngx_command_t& () const
    {
        return m_cmd;
    }
};

class NgxTake final
{
public:
    NgxTake(ngx_uint_t conf, int m = 0, int n = -1):
        m_type(conf | take(m, n))
    {}
    ~NgxTake() = default;
public:
    operator ngx_uint_t () const
    {
        return m_type;
    }
private:
    ngx_uint_t m_type;
private:
    static ngx_uint_t take(int m, int n)
    {
        static
        ngx_uint_t takes[] = {
            NGX_CONF_NOARGS,
            NGX_CONF_TAKE1,
            NGX_CONF_TAKE2,
            NGX_CONF_TAKE3,
            NGX_CONF_TAKE4,
            NGX_CONF_TAKE5,
            NGX_CONF_TAKE6,
            NGX_CONF_TAKE7,
            //{8, },
        };

        if(n < 0 || n < m)
        {
            return takes[m];
        }

        if(n >= NGX_CONF_MAX_ARGS)
        {
            return m == 1 ?
                   NGX_CONF_1MORE :
                   NGX_CONF_2MORE;
        }

        ngx_uint_t tmp = 0;

        for(int i = m; i <= n; ++i)
        {
            tmp |= takes[i];
        }

        return tmp;
    }

};

class NgxConfig final
{
public:
    NgxConfig() = default;
    ~NgxConfig() = default;
public:
    static NgxStrArray args(ngx_conf_t* cf)
    {
        return NgxStrArray(cf->args);
    }
};

// for config error message
#define NGX_CONF_ERROR_MSG(s)   const_cast<char*>(s)

// #define NGX_CONF_ERROR       (void *) -1
#ifdef NGX_CONF_ERROR
    #undef  NGX_CONF_ERROR
    #define NGX_CONF_ERROR reinterpret_cast<char*>(-1)
#endif

// for module's null
#include <boost/preprocessor/repetition/enum.hpp>

#define NGX_NULL_HELPER(z, n, t) t
#define NGX_MODULE_NULL(n)      \
    BOOST_PP_ENUM(n, NGX_NULL_HELPER, nullptr)

#endif  //_NGX_CONFIG_HPP



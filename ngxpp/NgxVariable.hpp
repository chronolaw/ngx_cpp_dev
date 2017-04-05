// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_VAR_HPP
#define _NGX_VAR_HPP

//#include <boost/format.hpp>
//#include <boost/utility/string_ref.hpp>

#include "NgxString.hpp"
#include "NgxException.hpp"

class NgxVariableValue final : public NgxWrapper<ngx_variable_value_t>
{
public:
    typedef NgxWrapper<ngx_variable_value_t> super_type;
    typedef NgxVariableValue this_type;
public:
    NgxVariableValue(ngx_variable_value_t* v):
        super_type(v)
    {}

    NgxVariableValue(ngx_variable_value_t& v):
        super_type(v)
    {}

    ~NgxVariableValue() = default;
public:
    void set(ngx_str_t& str, bool clear = false)
    {
        set(&str, clear);
    }

    void set(ngx_str_t* str, bool clear = false)
    {
        get()->len = clear ? 0 : str->len;
        get()->data = clear ? nullptr : str->data;

        get()->valid = !clear;
        get()->not_found = clear;
        get()->no_cacheable = false;
    }
public:
    bool valid() const
    {
        return get() && get()->valid && !get()->not_found;
    }

    ngx_str_t str() const
    {
        return valid()?
               ngx_str_t{get()->len, get()->data}:
               ngx_str_t ngx_null_string;
    }
};

inline namespace ngx_variable_types {
#ifndef ngx_stream_cpp_version  //ngx_http_variable

#define NGX_VAR_CHANGEABLE  NGX_HTTP_VAR_CHANGEABLE
#define NGX_VAR_INDEXED     NGX_HTTP_VAR_INDEXED

using NgxCtxCoreModule      = NgxHttpCoreModule;

using ngx_variable_ctx_t    = ngx_http_request_t;
using ngx_variable_t        = ngx_http_variable_t;
using ngx_set_variable_pt   = ngx_http_set_variable_pt;
using ngx_get_variable_pt   = ngx_http_get_variable_pt;

static auto ngx_add_variable = &ngx_http_add_variable;
static auto ngx_get_variable = &ngx_http_get_variable;

#else   //ngx_stream_variable

using NgxCtxCoreModule      = NgxStreamCoreModule;

#define NGX_VAR_CHANGEABLE  NGX_STREAM_VAR_CHANGEABLE
#define NGX_VAR_INDEXED     NGX_STREAM_VAR_INDEXED

using ngx_variable_ctx_t    = ngx_stream_session_t;
using ngx_variable_t        = ngx_stream_variable_t;
using ngx_set_variable_pt   = ngx_stream_set_variable_pt;
using ngx_get_variable_pt   = ngx_stream_get_variable_pt;

static auto ngx_add_variable = &ngx_stream_add_variable;
static auto ngx_get_variable = &ngx_stream_get_variable;

#endif
}

class NgxVariable final
{
public:
    typedef NgxVariable         this_type;
    typedef boost::string_ref   string_ref_type;
public:
    NgxVariable(ngx_variable_ctx_t* r, string_ref_type name):
        m_r(r), m_pool(r)
    {
        m_name = m_pool.dup(name);
        m_key = ngx_hash_strlow(m_name.data, m_name.data, m_name.len);
    }

    ~NgxVariable() = default;
public:
    ngx_str_t get() const
    {
        NgxVariableValue vv = ngx_get_variable(m_r,
            const_cast<ngx_str_t*>(&m_name), m_key);

        return vv.str();
    }

    bool set(string_ref_type value) const
    {
        auto& cmcf = NgxCtxCoreModule::instance().conf().main(m_r);

        // p = void*
        auto p = ngx_hash_find(&cmcf.variables_hash,
                                m_key, m_name.data, m_name.len);

        // not found
        if(!p)
        {
            return false;
        }

        auto v = reinterpret_cast<ngx_variable_t*>(p);

        // not changeable
        if(!(v->flags & NGX_VAR_CHANGEABLE))
        {
            return false;
        }

        auto val = m_pool.dup(value);

        NgxVariableValue vv = v->set_handler ?
            m_pool.alloc<ngx_variable_value_t>():
            // no set handler but indexed in request
            ((v->flags & NGX_VAR_INDEXED)?
                &m_r->variables[v->index] : nullptr);

        if(!vv)
        {
            return false;
        }

        vv.set(val, value.empty());

        if(v->set_handler)
        {
            v->set_handler(m_r, vv, v->data);
        }

        return true;
    }
public:
    operator ngx_str_t() const
    {
        return get();
    }

    void operator=(string_ref_type value) const
    {
        set(value);
    }
public:
    template<typename T>
    friend T& operator<<(T& o, const this_type& x)
    {
        //o << x.get();     // non-deducible context error

        auto s = x.get();
        o.write(reinterpret_cast<const char*>(s.data), s.len);
        return o;
    }
private:
    ngx_variable_ctx_t* m_r = nullptr;
    NgxPool             m_pool;
    ngx_str_t           m_name = ngx_null_string;
    ngx_uint_t          m_key = 0;
};

class NgxVarManager final
{
public:
    typedef NgxVarManager          this_type;
    typedef boost::string_ref      string_ref_type;
public:
    NgxVarManager(ngx_variable_ctx_t* r): m_r(r)
    {}
    ~NgxVarManager() = default;
public:
    NgxVariable operator[](string_ref_type name) const
    {
        return NgxVariable(m_r, name);
    }
private:
    ngx_variable_ctx_t* m_r = nullptr;
};

template<ngx_variable_t*(*get_vars)() = nullptr>
class NgxVariables final
{
public:
    typedef NgxVariables    this_type;
    typedef ngx_variable_t  var_type;
public:
    static ngx_int_t init(ngx_conf_t *cf)
    {
        if(!get_vars)
        {
            return NGX_OK;
        }

        for (auto v = get_vars(); v->name.len; ++v)
        {
            auto var = ngx_add_variable(cf, &v->name, v->flags);

            NgxException::fail(!var);

            var->get_handler = v->get_handler;
            var->data = reinterpret_cast<uintptr_t>(v->data);
        }

        return NGX_OK;
    }
public:
    static var_type* create(ngx_conf_t *cf, ngx_str_t& name, ngx_uint_t flags = 0)
    {
        auto p = ngx_add_variable(cf, &name, flags);

        NgxException::require(p);

        return p;
    }
};

#endif  //_NGX_VAR_HPP


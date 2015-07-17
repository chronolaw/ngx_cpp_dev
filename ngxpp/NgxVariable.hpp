// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_VAR_HPP
#define _NGX_VAR_HPP

#include <boost/format.hpp>
#include <boost/utility/string_ref.hpp>

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

    ~NgxVariableValue() = default;
public:
    void set(ngx_str_t& str)
    {
        get()->len = str.len;
        get()->data = str.data;

        get()->valid = true;
        get()->not_found = false;
        get()->no_cacheable = false;
    }

    void clear()
    {
        get()->len = 0;
        get()->data = nullptr;

        get()->valid = false;
        get()->not_found = true;
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

// step 1: var = ngx_http_add_variable(cf, name, flags);
// step 2: var.handler(func);
class NgxVariable final : public NgxWrapper<ngx_http_variable_t>
{
public:
    typedef NgxWrapper<ngx_http_variable_t> super_type;
    typedef NgxVariable this_type;
public:
    NgxVariable(ngx_http_variable_t* v):
        super_type(v)
    {}

    ~NgxVariable() = default;
public:
    template<typename T>    // T= uintptr_t
    void handler(ngx_http_set_variable_pt p, T data = nullptr) const
    {
        get()->set_handler = p;
        get()->data = reinterpret_cast<uintptr_t>(data);
    }

    template<typename T>    // T= uintptr_t
    void handler(ngx_http_get_variable_pt p, T data = nullptr) const
    {
        get()->get_handler = p;
        get()->data = reinterpret_cast<uintptr_t>(data);
    }

};

template<ngx_http_variable_t*(*get_vars)() = nullptr>
class NgxVariables final
{
public:
    typedef NgxVariables this_type;
    typedef NgxVariable var_type;
public:
    static ngx_int_t init(ngx_conf_t *cf)
    {
        if(!get_vars)
        {
            return NGX_OK;
        }

        for (auto v = get_vars(); v->name.len; ++v)
        {
            var_type var = ngx_http_add_variable(cf, &v->name, v->flags);

            NgxException::fail(!var);

            var.handler(v->get_handler, v->data);
        }

        return NGX_OK;
    }
public:
    static var_type create(ngx_conf_t *cf, ngx_str_t& name, ngx_uint_t flags = 0)
    {
        auto p = ngx_http_add_variable(cf, &name, flags);

        NgxException::require(p);

        return p;
    }
};

#include <boost/core/explicit_operator_bool.hpp>

class NgxVariableValueProxy final
{
public:
    typedef NgxVariableValueProxy this_type;
    typedef boost::string_ref string_ref_type;
public:
    NgxVariableValueProxy(ngx_variable_value_t* v, ngx_http_request_t* r):
        m_v(v), m_pool(r)
    {}
    ~NgxVariableValueProxy() = default;
public:
    operator ngx_str_t() const
    {
        return m_v.str();
    }

    void operator=(string_ref_type value)
    {
        if(!m_v.valid())
        {
            return;
        }

        if(value.empty())
        {
            m_v.clear();
            return;
        }

        auto s = m_pool.dup(value);

        m_v.set(s);
    }
public:
    BOOST_EXPLICIT_OPERATOR_BOOL()

    // check invalid
    bool operator!() const
    {
        return !m_v.valid();
    }

public:
    template<typename T>
    friend T& operator<<(T& o, const this_type& x)
    {
        o << x.m_v.str();
        return o;
    }
private:
    NgxVariableValue m_v;
    NgxPool m_pool;
};

class NgxVarManager final
{
public:
    typedef NgxVarManager this_type;
    typedef boost::string_ref string_ref_type;
public:
    NgxVarManager(ngx_http_request_t* r): m_r(r)
    {}
    ~NgxVarManager() = default;
public:
    ngx_str_t get(string_ref_type name) const
    {
        auto v = var(name);

        return NgxVariableValue(v).str();
    }

    void set(string_ref_type name, string_ref_type value) const
    {
        auto s = NgxPool(m_r).dup(value);

        auto v = var(name);

        NgxVariableValue(v).set(s);
    }

    void clear(string_ref_type name) const
    {
        auto v = var(name);

        NgxVariableValue(v).clear();
    }
public:
    NgxVariableValueProxy operator[](string_ref_type name) const
    {
        auto v = var(name);

        return NgxVariableValueProxy(v, m_r);
    }
private:
    ngx_http_request_t* m_r = nullptr;
private:
    ngx_http_variable_value_t* var(string_ref_type name) const
    {
        auto s = NgxPool(m_r).dup(name);
        auto key = ngx_hash_strlow(s.data, s.data, s.len);

        return ngx_http_get_variable(m_r, &s, key);
    }
};

#endif  //_NGX_VAR_HPP

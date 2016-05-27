// Copyright (c) 2015-2016
// Author: Chrono Law
#ifndef _NGX_VALUE_HPP
#define _NGX_VALUE_HPP

//#include "Nginx.hpp"

// NGX_CONF_UNSET = -1
class NgxUnsetValue final
{
public:
    template<typename T>
    operator T () const
    {
        return static_cast<T>(-1);
    }

    template<typename T>
    operator T* () const
    {
        return reinterpret_cast<T*>(-1);
    }
public:
    static const NgxUnsetValue& get()
    {
        static NgxUnsetValue const v = {};
        return v;
    }
};

class NgxValue final
{
public:
    NgxValue() = default;
    ~NgxValue() = default;
public:
    template<typename T, typename ... Args>
    static void unset(T& v, Args& ... args)
    {
        v = NgxUnsetValue::get();
        unset(args...);
    }

    static void unset() {}

    template<typename T>
    static bool invalid(const T& v)
    {
        return v == static_cast<T>(NgxUnsetValue::get());
    }

    template<typename T, typename U>
    static void init(T& x, const U& v)
    {
        if (invalid(x))
        {
            x = v;
        }
    }
public:
    template<typename T>
    static void memzero(T& v, std::size_t n = sizeof(T))
    {
        ngx_memzero(&v, n);
    }

    template<typename T>
    static void memzero(T* v, std::size_t n = sizeof(T))
    {
        ngx_memzero(v, n);
    }

public:
    template<typename T, typename U, typename V>
    static void merge(T& c, const U& p, const V& d)
    {
        if (invalid(c))
        {
            c = invalid(p) ? d : p;
        }
    }

public:
    static bool invalid(const ngx_str_t& v)
    {
        return !v.data || !v.len;
    }

    //static void merge(ngx_str_t& c, const ngx_str_t& p, const ngx_str_t& d)
    //{
    //    if(!c.data)
    //    {
    //        c = p.data ? p : d;
    //    }
    //}
};

// a convenient var for unset
namespace {
auto&& ngx_nil = NgxUnsetValue::get();
}

// convenient compare

template<typename T>
bool operator==(const T& x, const NgxUnsetValue&)
{
    return NgxValue::invalid(x);
}

template<typename T>
bool operator!=(const T& x, const NgxUnsetValue&)
{
    return !NgxValue::invalid(x);
}

#endif  //_NGX_VALUE_HPP

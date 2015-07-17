// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_ARRAY_HPP
#define _NGX_ARRAY_HPP

#include "NgxPool.hpp"

template<typename T>
class NgxArray final : public NgxWrapper<ngx_array_t>
{
public:
    typedef NgxWrapper<ngx_array_t> super_type;
    typedef NgxArray this_type;

    typedef T value_type;
public:
    NgxArray(const NgxPool& p, ngx_uint_t n = 10):
        super_type(p.array<T>(n))
    {}

    NgxArray(ngx_array_t* arr):super_type(arr)
    {}

    NgxArray(ngx_array_t& arr):super_type(arr)
    {}

    ~NgxArray() = default;
public:
    ngx_uint_t size() const
    {
        return get()?get()->nelts:0;
    }

    T& operator[](ngx_uint_t i) const
    {
        NgxException::require(i < size() && get());

        return elts()[i];
    }

public:
    template<typename V>
    void visit(V v) const
    {
        auto p = elts();

        for(ngx_uint_t i = 0;i < size(); ++i)
        {
            v(p[i]);
        }
    }
public:
    T& prepare() const
    {
        auto tmp = ngx_array_push(get());

        NgxException::require(tmp);

        assert(tmp);
        return *reinterpret_cast<T*>(tmp);
    }

    void push(const T& x) const
    {
        prepare() = x;
    }
public:
    void merge(const this_type& a) const
    {
        auto f = [this](const value_type& v)
        {
            prepare() = v;
        };

        a.visit(f);
    }
private:
    T* elts() const
    {
        return reinterpret_cast<T*>(get()->elts);
    }
};

typedef NgxArray<ngx_str_t> NgxStrArray;
typedef NgxArray<ngx_keyval_t> NgxKvArray;

#endif  //_NGX_ARRAY_HPP

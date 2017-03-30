// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_ALLOCATOR_HPP
#define _NGX_ALLOCATOR_HPP

#include "NgxWrapper.hpp"

template<typename T>
class NgxAlloctor : public NgxWrapper<ngx_pool_t>
{
public:
    typedef NgxWrapper<ngx_pool_t> super_type;
    typedef NgxAlloctor this_type;
public:
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    template<typename U>
    struct rebind
    {
        typedef NgxAlloctor<U> other;
    };
public:
    NgxAlloctor(ngx_pool_t* p) : super_type(p)
    {}

    ~NgxAlloctor() = default;
/*
public:
    pointer address(reference r) const
    {
        return &r;
    }

    const_pointer address(const_reference r) const
    {
        return &r;
    }

    size_type max_size() const
    {
        return std::numeric_limits<size_type>::max();
    }
*/
public:
    pointer allocate(size_type n)
    {
        return reinterpret_cast<pointer>(
                    ngx_pnalloc(get(), n * sizeof(T)));
    }

    void deallocate(pointer ptr, size_type n)
    {
        boost::ignore_unused(n);
        ngx_pfree(get(), ptr);
    }
/*
public:
    bool operator==(const NgxAlloctor& o) const
    {
        return get() == o.get();
    }

    bool operator!=(const NgxAlloctor& o) const
    {
        return get() != o.get();
    }
*/
};

#ifdef NGX_STD_CONTAINER
#include <vector>

template<typename T>
using NgxStdVector = std::vector<T, NgxAlloctor<T> >;

#endif

#endif  //_NGX_ALLOCATOR_HPP

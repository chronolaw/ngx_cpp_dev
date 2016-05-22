// Copyright (c) 2015-2016
// Author: Chrono Law
#ifndef _NGX_WRAPPER_HPP
#define _NGX_WRAPPER_HPP

#include <boost/type_traits.hpp>

//#include "Nginx.hpp"

// T = ngx_list_t, ngx_array_t, ngx_pool_t, ...
template<typename T>
class NgxWrapper
{
public:
    typedef typename boost::remove_pointer<T>::type wrapped_type;

    typedef wrapped_type* pointer_type;
    typedef wrapped_type& reference_type;
private:
    pointer_type m_ptr = nullptr;
protected:
    NgxWrapper(pointer_type p):m_ptr(p)
    {
        //assert(p);
    }

    NgxWrapper(reference_type x):m_ptr(&x)
    {}

    ~NgxWrapper() = default;

public:
    pointer_type get() const
    {
        return m_ptr;
    }

    operator pointer_type () const
    {
        return get();
    }

    pointer_type operator->() const
    {
        return get();
    }
public:
    operator bool () const
    {
        return get();
    }
//public:
//    operator reference_type () const
//    {
//        return *get();
//    }
};

#endif  //_NGX_WRAPPER_HPP

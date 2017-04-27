// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_STRING_HPP
#define _NGX_STRING_HPP

//#include <string>
//#include <boost/next_prior.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/utility/string_ref.hpp>
//#include <boost/iterator/iterator_traits.hpp>

#include "NgxWrapper.hpp"

// typed nginx string
#define typed_ngx_string(str) ngx_str_t ngx_string(str)
#define typed_ngx_null_string ngx_str_t ngx_null_string

class NgxString final : public NgxWrapper<ngx_str_t>
{
public:
    typedef NgxWrapper<ngx_str_t> super_type;
    typedef NgxString this_type;

    typedef boost::string_ref string_ref_type;
public:
    NgxString(ngx_str_t& str):
        super_type(str)
    {}

    // enable convert const object
    NgxString(const ngx_str_t& str):
        super_type(const_cast<ngx_str_t&>(str))
    {}

    // disable temporary object
    NgxString(ngx_str_t&& str) = delete;

    ~NgxString() = default;
public:
    const char* data() const
    {
        return reinterpret_cast<const char*>(get()->data);
    }

    std::size_t size() const
    {
        return get()->len;
    }

    bool empty() const
    {
        return !get()->data || !get()->len;
    }

    string_ref_type str() const
    {
        return string_ref_type(data(), size());
    }
public:
    // nagetive means error
    operator ngx_int_t () const
    {
        return ngx_atoi(get()->data, get()->len);
    }
public:
    // range concept
    typedef u_char  char_type;
    typedef u_char* iterator;
    typedef iterator const_iterator;
    typedef boost::iterator_difference<iterator> difference_type;
public:
    const_iterator begin() const
    {
        return get()->data;
    }

    const_iterator end() const
    {
        return get()->data + get()->len;
    }

public:
    const char_type& front() const
    {
        return *begin();
    }
    const char_type& back() const
    {
        //return *std::prev(end());
        return *boost::prior(end());
    }
public:
    bool contains(const this_type& x) const
    {
        return boost::contains(*this, x);
    }

public:
    template<typename T>
    friend T& operator<<(T& o, const this_type& s)
    {
        o.write(s.data(), s.size());
        return o;
    }

    //template<typename T>
    //friend T& operator<<(T& o, const ngx_str_t& s)
    //{
    //    o.write(reinterpret_cast<const char*>(s.data), s.len);
    //    return o;
    //}

    //template<typename T>
    //friend T& operator<<(T& o, const volatile ngx_str_t& s)
    //{
    //    o.write(reinterpret_cast<const char*>(s.data), s.len);
    //    return o;
    //}

    friend bool operator==(const this_type& l, const this_type& r)
    {
        return l.size() == r.size() &&
               ngx_strncmp(l.data(), r.data(), l.size()) == 0
            ;
    }
public:
    template<typename ... Args>
    void printf(const Args& ... args) const
    {
        auto p = ngx_snprintf(get()->data, get()->len, args ...);
        get()->len = static_cast<std::size_t>(p - get()->data);
    }
};

// workaround for some compilers
inline namespace ostream_for_ngx_str_t{

template<typename T>
T& operator<<(T& o, const ngx_str_t& s)
{
    o.write(reinterpret_cast<const char*>(s.data), s.len);
    return o;
}

template<typename T>
T& operator<<(T& o, const volatile ngx_str_t& s)
{
    o.write(reinterpret_cast<const char*>(s.data), s.len);
    return o;
}

}

#endif //_NGX_STRING_HPP

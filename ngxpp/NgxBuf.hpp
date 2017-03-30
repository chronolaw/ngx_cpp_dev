// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_BUF_HPP
#define _NGX_BUF_HPP

#include "NgxPool.hpp"

class NgxBuf final : public NgxWrapper<ngx_buf_t>
{
public:
    typedef NgxWrapper<ngx_buf_t> super_type;
    typedef NgxBuf this_type;
public:
    NgxBuf(const NgxPool& p, std::size_t n):
        super_type(p.buffer(n))
    {}

    NgxBuf(ngx_buf_t* buf):super_type(buf)
    {}

    NgxBuf(ngx_buf_t& buf):super_type(buf)
    {}

    ~NgxBuf() = default;
public:
    // readable range
    void range(u_char* a, u_char* b) const
    {
        get()->pos = a;
        get()->last = b;

        get()->memory = true;
    }

    void boundary(u_char* a, u_char* b) const
    {
        get()->start = a;
        get()->end = b;
    }

    ngx_str_t range() const
    {
        return ngx_str_t{
            static_cast<std::size_t>(get()->last - get()->pos), get()->pos};
    }

    ngx_str_t boundary() const
    {
        return ngx_str_t{
            static_cast<std::size_t>(get()->end - get()->start), get()->start};
    }
public:
    // convenitable method
    void range(ngx_str_t* s) const
    {
        range(s->data, s->data + s->len);
        boundary(s->data, s->data + s->len);
    }

    void range(ngx_str_t& s) const
    {
        range(&s);
    }
public:
    template<typename ... Args>
    void printf(const Args& ... args) const
    {
        get()->last = ngx_slprintf(get()->pos, get()->end, args ...);
    }

    void copy(u_char* src, size_t n) const
    {
        get()->last = ngx_copy(get()->pos, src, n);
    }
public:
    std::size_t size() const
    {
        return ngx_buf_size(get());
    }

    void clear() const
    {
        get()->pos = get()->last;
    }

    void reset() const
    {
        get()->pos = get()->start;
        get()->last = get()->start;
    }

//    ngx_uint_t capacity() const
//    {
//        return get()->end - get()->start;
//    }

public:
    bool empty() const
    {
        return get()->pos == get()->last;
    }

    bool full() const
    {
        return get()->pos == get()->end;
    }
public:
    void consume(std::size_t n) const
    {
        if(n > size())
        {
            n = size();
        }

        get()->pos += n;
    }

    void produce(std::size_t n) const
    {
        get()->last += n;

        if(get()->last > get()->end)
        {
            get()->last = get()->end;
        }
    }
public:
    u_char* begin() const
    {
        return get()->pos;
    }

    u_char* end() const
    {
        return get()->last;
    }
public:
    bool memory() const
    {
        return ngx_buf_in_memory(get());
    }

    bool memoryonly() const
    {
        return ngx_buf_in_memory_only(get());
    }

    bool special() const
    {
        return ngx_buf_special(get());
    }
public:
    bool last() const
    {
        return get()->last_buf || get()->last_in_chain;
    }

    void finish(bool flag = true) const
    {
        get()->last_buf = flag;
        get()->last_in_chain = flag;
    }
};

#endif  //_NGX_BUF_HPP

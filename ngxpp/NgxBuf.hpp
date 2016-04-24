// Author: Chrono Law
// Copyright (c) 2015
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

    ~NgxBuf() = default;
public:
    // readable range
    void range(u_char* a, u_char* b) const
    {
        get()->pos = a;
        get()->last = b;

        get()->memory = true;
    }

    // convenitable method
    void range(ngx_str_t* s) const
    {
        range(s->data, s->data + s->len);
        boundary(s->data, s->data + s->len);
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

#include <boost/iterator/iterator_facade.hpp>
#include <boost/core/explicit_operator_bool.hpp>

class NgxChainNode final : public NgxWrapper<ngx_chain_t>
{
public:
    typedef NgxWrapper<ngx_chain_t> super_type;
    typedef NgxChainNode this_type;
public:
    NgxChainNode(const NgxPool& p):super_type(p.chain())
    {}

    NgxChainNode(ngx_chain_t* c):super_type(c)
    {}

    ~NgxChainNode() = default;
public:
    bool last() const
    {
        return !get()->next;
    }

    void link(ngx_chain_t* c) const
    {
        get()->next = c;
    }

    void finish() const
    {
        return link(nullptr);
    }
public:
    NgxBuf data() const
    {
        return get()->buf;
    }

    void set(ngx_buf_t* ptr) const
    {
        get()->buf = ptr;
    }
};

class NgxChainIterator final:
        public boost::iterator_facade<
                    NgxChainIterator, NgxChainNode,
                    boost::single_pass_traversal_tag>
{
public:
    typedef boost::iterator_facade<
                    NgxChainIterator, NgxChainNode,
                    boost::single_pass_traversal_tag>
            super_type;
    typedef typename super_type::reference reference;
public:
    NgxChainIterator(ngx_chain_t* c) : m_p(c)
    {}

    NgxChainIterator() = default;
    ~NgxChainIterator() = default;
public:
    BOOST_EXPLICIT_OPERATOR_BOOL()

    bool operator!() const
    {
        return !m_p;
    }

private:
    friend class boost::iterator_core_access;

    reference dereference() const
    {
        m_proxy = m_p;
        return m_proxy;
    }

    void increment()
    {
        if(!m_p)
        {
            return;
        }

        m_p = m_p->next;
    }

    bool equal(NgxChainIterator const& o) const
    {
        return m_p == o.m_p;
    }
private:
    ngx_chain_t* m_p = nullptr;
    mutable NgxChainNode m_proxy{m_p};
};

class NgxChain final : public NgxWrapper<ngx_chain_t>
{
public:
    typedef NgxWrapper<ngx_chain_t> super_type;
    typedef NgxChain this_type;
public:
    NgxChain(ngx_chain_t* c):super_type(c)
    {}

    ~NgxChain() = default;
public:
    typedef NgxChainIterator iterator;
    typedef const iterator const_iterator;

    iterator begin() const
    {
        return iterator(get());
    }

    iterator end() const
    {
        return iterator();
    }
public:
    template<typename V>
    void visit(V v) const
    {
        for(auto iter = begin();
            iter; ++iter)
        {
            v(*iter);
        }
    }
public:
    std::size_t size() const
    {
        std::size_t len = 0;

        for(auto& c : *this)
        {
            len += c.data().size();
        }

        return len;
    }

    ngx_chain_t* tail() const
    {
        auto p = get();

        for(;p->next;p = p->next);

        return p;
    }

    void append(ngx_chain_t* ch) const
    {
        tail()->next = ch;
    }
public:
    void clear() const
    {
        for(auto& c : *this)
        {
            c.data().clear();
        }
    }
public:
    void copy(u_char* p) const
    {
        for(auto& c : *this)
        {
            p = ngx_copy(p, c.data().begin(), c.data().size());
        }
    }
};

#endif  //_NGX_BUF_HPP

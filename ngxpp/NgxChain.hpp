// Author: Chrono Law
// Copyright (c) 2015-2016
#ifndef _NGX_CHAIN_HPP
#define _NGX_CHAIN_HPP

#include <boost/iterator/iterator_facade.hpp>
#include <boost/core/explicit_operator_bool.hpp>

#include "NgxBuf.hpp"

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
public:
    void free(ngx_pool_t* pool) const
    {
        ngx_free_chain(pool, get());
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

public:
    ngx_chain_t* head() const
    {
        return get();
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
    // V is a lambda
    // [](ngx_chain_t* c){}
    // [](const NgxChainNode& c){}
    template<typename V>
    iterator trace(V v) const
    {
        auto p = get();

        for(;p->next;p = p->next)
        {
            v(p);
        }

        v(p);   // last node

        return p;
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
public:
    ngx_chain_t* free(ngx_pool_t* pool, ngx_chain_t* pos = nullptr) const
    {
        auto iter = begin();
        decltype(iter) sentinel = pos;

        for(;iter && iter != sentinel;)
        {
            auto cur = iter;
            ++iter;
            cur->free(pool);
        }

        return pos;
    }
};

#endif  //_NGX_CHAIN_HPP

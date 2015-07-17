// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_QUEUE_HPP
#define _NGX_QUEUE_HPP

#include <boost/static_assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/reverse_iterator.hpp>

#include "NgxWrapper.hpp"

// T must has a T::queue data member!
template<typename T,
         ngx_queue_t T::* q = &T::queue,
         std::size_t offset = (std::size_t)&(((T*)0)->*q)
         >
class NgxQueueNode final : public NgxWrapper<ngx_queue_t>
{
public:
    typedef NgxWrapper<ngx_queue_t> super_type;
    typedef NgxQueueNode this_type;
public:
    NgxQueueNode(ngx_queue_t* ptr): super_type(ptr)
    {}

    NgxQueueNode(T& x): super_type(x.*q)
    {}

    NgxQueueNode(T* x): super_type(x->*q)
    {}

    ~NgxQueueNode() = default;
public:
    NgxQueueNode next() const
    {
        return ngx_queue_next(get());
    }

    NgxQueueNode prev() const
    {
        return ngx_queue_prev(get());
    }
public:
    void append(T& v) const
    {
        ngx_queue_insert_after(get(), &(v.*q));
    }

    void remove() const
    {
        ngx_queue_remove(get());
    }
public:
    T& data() const
    {
        //reinterpret_cast<T*>(...)
        return *(T*)((u_char*)(get()) - offset);
    }
};

// T = NgxQueueNode
template<typename T>
class NgxQueueIterator final :
        public boost::iterator_facade<
                    NgxQueueIterator<T>, T,
                    boost::bidirectional_traversal_tag>
{
public:
    typedef boost::iterator_facade<
                    NgxQueueIterator<T>, T,
                    boost::bidirectional_traversal_tag>
            super_type;
    typedef NgxQueueIterator this_type;

    typedef typename super_type::reference reference;
public:
    NgxQueueIterator(ngx_queue_t* p): m_cur(p)
    {}

    NgxQueueIterator() = default;
    ~NgxQueueIterator() = default;
public:
    NgxQueueIterator(const this_type&) = default;
    NgxQueueIterator& operator=(const this_type&) = default;
private:
    friend class boost::iterator_core_access;

    reference dereference() const
    {
        m_proxy = m_cur;
        return m_proxy;
    }

    void increment()
    {
        m_cur = ngx_queue_next(m_cur);
    }

    void decrement()
    {
        m_cur = ngx_queue_prev(m_cur);
    }

    bool equal(this_type const& o) const
    {
        return m_cur == o.m_cur;
    }
private:
    ngx_queue_t* m_cur = nullptr;
    mutable T m_proxy{m_cur};
};

// T must has a T::queue data member!
template<typename T,
         ngx_queue_t T::* q = &T::queue,
         std::size_t offset = (std::size_t)&(((T*)0)->*q)
         >
class NgxQueue final : public NgxWrapper<ngx_queue_t>
{
public:
    typedef NgxWrapper<ngx_queue_t> super_type;
    typedef NgxQueueNode<T, q, offset> node_type;
    typedef NgxQueue this_type;
public:
    NgxQueue(ngx_queue_t& v): super_type(&v)
    {}

    NgxQueue(ngx_queue_t* ptr): super_type(ptr)
    {}

    NgxQueue(T& x): super_type(x.*q)
    {}

    NgxQueue(T* x): super_type(x->*q)
    {}

    ~NgxQueue() = default;
public:
    void init() const
    {
        ngx_queue_init(get());
    }

    bool empty() const
    {
        return ngx_queue_empty(get());
    }

public:
    typedef NgxQueueIterator<node_type> iterator;
    typedef boost::reverse_iterator<iterator> reverse_iterator;

    typedef const iterator const_iterator;
    typedef const reverse_iterator const_reverse_iterator;

    iterator begin() const
    {
        return iterator(ngx_queue_head(get()));
    }

    iterator end() const
    {
        return iterator(ngx_queue_sentinel(get()));
    }

    reverse_iterator rbegin() const
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend() const
    {
        return reverse_iterator(begin());
    }
public:
    node_type front() const
    {
        return ngx_queue_head(get());
        //return *begin();
    }

    node_type back() const
    {
        return ngx_queue_last(get());
        //return *end();
    }

public:
    void insert(T& v) const
    {
        ngx_queue_insert_head(get(), &(v.*q));
    }

    void append(T& v) const
    {
        ngx_queue_insert_tail(get(), &(v.*q));
    }

    void insert(const iterator& iter, T& v) const
    {
        iter->append(v);
        //ngx_queue_insert_after(iter.get(), &(v.*q));
    }
public:
    void merge(ngx_queue_t* n) const
    {
        ngx_queue_add(get(), n);
    }

    template<typename F>
    void sort(F f) const
    {
        BOOST_STATIC_ASSERT_MSG(boost::is_pointer<F>::value, "not pointer");

        ngx_queue_sort(get(), f);
    }
};

#endif  //_NGX_QUEUE_HPP

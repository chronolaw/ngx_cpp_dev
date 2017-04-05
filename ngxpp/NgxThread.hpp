// Copyright (c) 2017
// Author: Chrono Law
#ifndef _NGX_THREAD_HPP
#define _NGX_THREAD_HPP

#include "NgxPool.hpp"
#include "NgxEvent.hpp"

template<typename T>
class NgxThreadTask final : public NgxWrapper<ngx_thread_task_t>
{
public:
    typedef NgxWrapper<ngx_thread_task_t>   super_type;
    typedef NgxThreadTask                   this_type;
    typedef T                               task_ctx_type;
public:
    NgxThreadTask(ngx_thread_task_t* t) : super_type(t)
    {}

    ~NgxThreadTask() = default;
public:
    T* ctx() const
    {
        return reinterpret_cast<T*>(get()->ctx);
    }

    template<typename F>
    void handler(F f) const
    {
        if(!get()->handler)
        {
            get()->handler = f;
        }
    }
public:
    NgxEvent event() const
    {
        return &get()->event;
    }
public:
    static ngx_thread_task_t* create(const NgxPool& pool)
    {
        auto p = pool.thread_task<T>();

        return p;
    }
};

class NgxThreadPool final : public NgxWrapper<ngx_thread_pool_t>
{
public:
    typedef NgxWrapper<ngx_thread_pool_t>   super_type;
    typedef NgxThreadPool                   this_type;
public:
    NgxThreadPool(ngx_thread_pool_t* p) : super_type(p)
    {}

    NgxThreadPool(ngx_str_t name) : super_type(acquire(name))
    {}

    ~NgxThreadPool() = default;
public:
    void post(ngx_thread_task_t* task) const
    {
        auto rc = ngx_thread_task_post(get(), task);

        NgxException::require(rc);
    }
public:
    static ngx_thread_pool_t* acquire(ngx_str_t& name)
    {
        auto p = ngx_thread_pool_get((ngx_cycle_t *) ngx_cycle, &name);

        NgxException::require(p);

        return p;
    }
};

#endif  //_NGX_THREAD_HPP

// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_POOL_HPP
#define _NGX_POOL_HPP

//#include <new>
#include <boost/type_traits.hpp>
#include <boost/utility/string_ref.hpp>

#include "NgxException.hpp"
#include "NgxWrapper.hpp"

class NgxPool final : public NgxWrapper<ngx_pool_t>
{
public:
    typedef NgxWrapper<ngx_pool_t> super_type;
    typedef NgxPool this_type;
public:
    NgxPool(ngx_pool_t* p): super_type(p)
    {}

    // T = ngx_http_request_t,ngx_conf_t, ...
    // T must has a member pool
    template<typename T>
    NgxPool(T* x):NgxPool(x->pool)
    {}

    ~NgxPool() = default;
public:
    template<typename T, bool no_exception = false>
    T* palloc() const
    {
        auto p = ngx_pcalloc(get(), sizeof(T));

        if(!p)
        {
            if(no_exception)
            {
                return nullptr;
            }

            NgxException::raise();
        }

        assert(p);
        return new (p) T();
    }

    template<typename T>
    T* alloc() const
    {
        return palloc<T, false>();
    }

    template<typename T>
    T* alloc_noexcept() const
    {
        return palloc<T, true>();
    }

    template<typename T>
    T* nalloc(std::size_t n) const
    {
        auto p = ngx_pnalloc(get(), n);

        NgxException::require(p);

        assert(p);
        return reinterpret_cast<T*>(p);
    }

    template<typename T, typename ... Args>
    T* construct(const Args& ... args) const
    {
        auto p = ngx_pcalloc(get(), sizeof(T));

        NgxException::require(p);

        assert(p);
        return new (p) T(args ...);
    }
public:
    template<typename F, typename T>
    ngx_pool_cleanup_t* cleanup(F func, T* data, std::size_t size = 0) const
    {
        auto p = ngx_pool_cleanup_add(get(), size);

        NgxException::require(p);
        p->handler = func;

        if(data)
        {
            p->data = data;
        }

        return p;
    }

public:
    template<typename T>
    static void destory(void* p)
    {
        (reinterpret_cast<T*>(p))->~T();
    }

    template<typename T>
    void cleanup(T* data) const
    {
        cleanup(&this_type::destory<T>, data);
    }

    template<typename T>
    void cleanup(T& data) const
    {
        cleanup(&this_type::destory<T>, &data);
    }
public:
    ngx_buf_t* buffer(std::size_t n = 0) const
    {
        auto tmp = n > 0 ?
                   ngx_create_temp_buf(get(), n):
                   alloc<ngx_buf_t>()//ngx_calloc_buf(get())
                   ;

        NgxException::require(tmp, NGX_HTTP_INTERNAL_SERVER_ERROR);

        return tmp;
    }

    // copy a string in pool
    ngx_str_t dup(boost::string_ref str) const
    {
        ngx_str_t tmp{str.size(), (u_char*)str.data()};
        return dup(tmp);
    }

    ngx_str_t dup(ngx_str_t& str) const
    {
        ngx_str_t tmp;

        tmp.len = str.len;
        tmp.data = ngx_pstrdup(get(), &str);

        NgxException::require(tmp.data);

        return tmp;
    }

public:
    template<typename T>
    ngx_array_t* array(ngx_uint_t n = 1) const
    {
        auto p = ngx_array_create(get(), n, sizeof(T));

        NgxException::require(p);

        return p;
    }

    template<typename T>
    ngx_list_t* list(ngx_uint_t n) const
    {
        auto p = ngx_list_create(get(), n, sizeof(T));

        NgxException::require(p);

        return p;
    }

    ngx_chain_t* chain() const
    {
        auto p = ngx_alloc_chain_link(get());

        NgxException::require(p);

        p->next = nullptr;

        return p;
    }

    ngx_chain_t* chain(const ngx_bufs_t& bufs) const
    {
        auto p = ngx_create_chain_of_bufs(get(),
                const_cast<ngx_bufs_t*>(&bufs));

        NgxException::require(p);

        return p;
    }

public:
    ngx_file_t* file() const
    {
        auto p = alloc<ngx_file_t>();

        return p;
    }

    void cleanup(ngx_file_t* f) const
    {
        auto p = alloc<ngx_pool_cleanup_file_t>();

        p->fd = f->fd;
        p->name = f->name.data;
        p->log = f->log;

        cleanup(ngx_pool_cleanup_file, p);
    }

    ngx_temp_file_t* tempfile() const
    {
        auto p = alloc<ngx_temp_file_t>();

        p->file.fd = NGX_INVALID_FILE;
        p->pool = get();

        return p;
    }
};

#endif  //_NGX_POOL_HPP

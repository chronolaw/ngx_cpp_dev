// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_MODULE_HPP
#define _NGX_MODULE_HPP

#include <type_traits>
#include <boost/utility/enable_if.hpp>

#include "NgxPool.hpp"

template<typename T>
struct eval_type :
    std::conditional<
        std::is_void<T>::value,
        void*,
        typename std::add_lvalue_reference<T>::type>
{};

#define META_TYPE(t) typename eval_type<t>::type

template<typename T3 = void, typename T2 = void, typename T1 = void>
class NgxModuleConfig final
{
public:
    NgxModuleConfig(ngx_uint_t idx): m_idx(idx)
    {}
    ~NgxModuleConfig() = default;
private:
    ngx_uint_t m_idx = 0;
public:
    ngx_uint_t index() const
    {
        return m_idx;
    }

    //template<typename T, typename U>
    //T* get(U conf) const
    //{
    //    return reinterpret_cast<T*>(conf[index()]);
    //}

    template<typename T, typename U>
    typename std::enable_if<std::is_object<T>::value,T&>::type
    //typename boost::disable_if<std::is_void<T>,T&>::type
    get(U conf) const
    {
        assert(conf[index()]);
        return *reinterpret_cast<T*>(conf[index()]);
    }

    template<typename T, typename U>
    typename std::enable_if<std::is_void<T>::value,void*>::type
    //typename boost::enable_if<std::is_void<T>,void*>::type
    get(U conf) const
    {
        return conf[index()];
    }
public:
    META_TYPE(T1) main(ngx_http_request_t* r) const
    {
        return get<T1>(r->main_conf);
    }

    META_TYPE(T2) srv(ngx_http_request_t* r) const
    {
        return get<T2>(r->srv_conf);
    }

    META_TYPE(T3) loc(ngx_http_request_t* r) const
    {
        return get<T3>(r->loc_conf);
    }
public:
    META_TYPE(T1) main(ngx_conf_t* cf) const
    {
        return get<T1>(ctx(cf)->main_conf);
    }

    META_TYPE(T2) srv(ngx_conf_t* cf) const
    {
        return get<T2>(ctx(cf)->srv_conf);
    }

    META_TYPE(T3) loc(ngx_conf_t* cf) const
    {
        return get<T3>(ctx(cf)->loc_conf);
    }

private:
    ngx_http_conf_ctx_t* ctx(ngx_conf_t* cf) const
    {
        return reinterpret_cast<ngx_http_conf_ctx_t*>(cf->ctx);
    }
public:
    META_TYPE(T2) upstream(ngx_http_upstream_srv_conf_t* us) const
    {
        return get<T2>(us->srv_conf);
    }

    META_TYPE(T2) upstream(ngx_http_request_t* r) const
    {
        return upstream(r->upstream->conf->upstream);
    }
};

class NgxModuleCtx final
{
public:
    NgxModuleCtx(ngx_uint_t idx): m_idx(idx)
    {}
    ~NgxModuleCtx() = default;
private:
    ngx_uint_t m_idx = 0;
public:
    ngx_uint_t index() const
    {
        return m_idx;
    }
public:
    bool empty(ngx_http_request_t* r) const
    {
        return !ctx(r);
    }

    void clear(ngx_http_request_t* r) const
    {
        ctx(r) = nullptr;
    }

    template<typename T>
    T& data(ngx_http_request_t* r) const
    {
        if(empty(r))
        {
            ctx(r) = alloc<T>(r);
            //ctx(r) = NgxPool(r).alloc<T>();
            //ngx_http_set_module_ctx(r, ctx, m_module);
        }

        return *reinterpret_cast<T*>(ctx(r));
    }
private:
    typedef void* raw_pointer;

    raw_pointer& ctx(ngx_http_request_t* r) const
    {
        //ngx_http_get_module_ctx(r, m_module));
        return r->ctx[index()];
    }
private:
    template<typename T>
    T* alloc(ngx_http_request_t* r,
            typename boost::disable_if<std::is_constructible<T, ngx_pool_t*>>::type* p=0) const
    {
        return NgxPool(r).alloc<T>();
    }

    template<typename T>
    T* alloc(ngx_http_request_t* r,
            typename boost::enable_if<std::is_constructible<T, ngx_pool_t*>>::type* p=0) const
    {
        return NgxPool(r).construct<T>(r->pool);
    }
};

template<typename T3 = void, typename T2 = void, typename T1 = void>
class NgxModule
{
public:
    NgxModule(ngx_module_t& m ):
        m_conf(m.ctx_index), m_ctx(m.ctx_index)
    {}

    ~NgxModule() = default;
private:
    typedef NgxModuleConfig<T3, T2, T1> config_type;
    typedef NgxModuleCtx ctx_type;

    config_type m_conf;
    ctx_type m_ctx;
public:
    const config_type& conf() const
    {
        return m_conf;
    }

    const ctx_type& ctx() const
    {
        return m_ctx;
    }

    template<typename T>
    T& data(ngx_http_request_t* r) const
    {
        return ctx().template data<T>(r);
    }
};

#define NGX_MOD_INSTANCE(T, mod, ... )      \
    struct T {                              \
        typedef NgxModule<__VA_ARGS__> mod_type;   \
        static mod_type& instance()         \
        {                                   \
            extern ngx_module_t mod;        \
            static mod_type m(mod);         \
            return m;                       \
        }                                   \
        mod_type& operator()() const        \
        {   return instance();}             \
    };

#define NGX_MOD_CTX_INSTANCE(T, mod, c, ... )     \
    struct T {                                      \
        typedef NgxModule<__VA_ARGS__> mod_type;    \
        typedef NgxModuleCtx ctx_type;              \
        typedef c ctx_data_type;                       \
        static mod_type& instance()         \
        {                                   \
            extern ngx_module_t mod;        \
            static mod_type m(mod);         \
            return m;                       \
        }                                   \
        static const ctx_type& ctx()                \
        {   return instance().ctx();}       \
        static ctx_data_type& data(ngx_http_request_t* r)                 \
        {   return instance().template data<ctx_data_type>(r);}             \
    };
#endif  //_NGX_MODULE_HPP

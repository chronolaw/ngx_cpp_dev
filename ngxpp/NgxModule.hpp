// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_MODULE_HPP
#define _NGX_MODULE_HPP

//#include <type_traits>
//#include <boost/utility/enable_if.hpp>

#include "NgxPool.hpp"

#ifdef META_TYPE
#undef META_TYPE
#endif

#define META_TYPE(t) typename eval_type<t>::type

//template<typename T3 = void, typename T2 = void, typename T1 = void>
template<
#ifndef ngx_stream_cpp_version
    typename T3 = void,     // http location conf
#endif
    typename T2 = void, typename T1 = void>
class NgxModuleConfig final
{
public:
#ifndef ngx_stream_cpp_version
    typedef ngx_http_request_t      ngx_session_type;
    typedef ngx_http_conf_ctx_t     ngx_conf_ctx_type;
#else
    typedef ngx_stream_session_t    ngx_session_type;
    typedef ngx_stream_conf_ctx_t   ngx_conf_ctx_type;
#endif
private:
    template<typename T>
    struct eval_type :
        std::conditional<
            std::is_void<T>::value,
            void*,
            typename std::add_lvalue_reference<T>::type>
    {};
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
    META_TYPE(T1) main(ngx_session_type* r) const
    {
        return get<T1>(r->main_conf);
    }

    META_TYPE(T2) srv(ngx_session_type* r) const
    {
        return get<T2>(r->srv_conf);
    }

#ifndef ngx_stream_cpp_version
    META_TYPE(T3) loc(ngx_session_type* r) const
    {
        return get<T3>(r->loc_conf);
    }
#endif
public:
    META_TYPE(T1) main(ngx_conf_t* cf) const
    {
        return get<T1>(ctx(cf)->main_conf);
    }

    META_TYPE(T2) srv(ngx_conf_t* cf) const
    {
        return get<T2>(ctx(cf)->srv_conf);
    }

#ifndef ngx_stream_cpp_version
    META_TYPE(T3) loc(ngx_conf_t* cf) const
    {
        return get<T3>(ctx(cf)->loc_conf);
    }
#endif

public:
#ifndef ngx_stream_cpp_version
    // cycle->conf_ctx[ngx_http_module.index] must not null
    META_TYPE(T1) main(ngx_cycle_t* cycle) const
    {
        return get<T1>(ctx(cycle)->main_conf);
    }

    ngx_http_conf_ctx_t* ctx(ngx_cycle_t* cycle) const
    {
        return reinterpret_cast<ngx_http_conf_ctx_t*>(cycle->conf_ctx[ngx_http_module.index]);
    }
#endif
private:
    ngx_conf_ctx_type* ctx(ngx_conf_t* cf) const
    {
        return reinterpret_cast<ngx_conf_ctx_type*>(cf->ctx);
    }
public:
#ifndef ngx_stream_cpp_version
    META_TYPE(T2) upstream(ngx_http_upstream_srv_conf_t* us) const
    {
        return get<T2>(us->srv_conf);
    }

    META_TYPE(T2) upstream(ngx_http_request_t* r) const
    {
        return upstream(r->upstream->conf->upstream);
    }
#endif
};

#undef META_TYPE

class NgxModuleCtx final
{
public:
#ifndef ngx_stream_cpp_version
    typedef ngx_http_request_t      ngx_session_type;
#else
    typedef ngx_stream_session_t    ngx_session_type;
#endif
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
    bool empty(ngx_session_type* r) const
    {
        return !ctx(r);
    }

    void clear(ngx_session_type* r) const
    {
        ctx(r) = nullptr;
    }

    template<typename T>
    T& data(ngx_session_type* r) const
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

    raw_pointer& ctx(ngx_session_type* r) const
    {
        //ngx_http_get_module_ctx(r, m_module));
        return r->ctx[index()];
    }
private:
    template<typename T>
    T* alloc(ngx_session_type* r,
            typename boost::disable_if<std::is_constructible<T, ngx_pool_t*>>::type* p=0) const
    {
        return NgxPool(r).alloc<T>();
    }

    template<typename T>
    T* alloc(ngx_session_type* r,
            typename boost::enable_if<std::is_constructible<T, ngx_pool_t*>>::type* p=0) const
    {
        NgxPool pool(r);
        return pool.construct<T>(pool);
    }
};

//template<typename T3 = void, typename T2 = void, typename T1 = void>
template<
#ifndef ngx_stream_cpp_version
    typename T3 = void,     // http location conf
#endif
    typename T2 = void, typename T1 = void>
class NgxModule
{
public:
#ifndef ngx_stream_cpp_version
    typedef ngx_http_request_t      ngx_session_type;
#else
    typedef ngx_stream_session_t    ngx_session_type;
#endif
public:
    NgxModule(ngx_module_t& m ):
        m_conf(m.ctx_index), m_ctx(m.ctx_index)
    {}

    ~NgxModule() = default;
public:
    typedef NgxModuleConfig<
#ifndef ngx_stream_cpp_version
                T3,
#endif
                T2, T1> config_type;
    typedef NgxModuleCtx ctx_type;

private:
    config_type     m_conf;
    ctx_type        m_ctx;
public:
    const config_type& conf() const
    {
        return m_conf;
    }

    const ctx_type& ctx() const
    {
        return m_ctx;
    }

    // Session = ngx_http_request_t/ngx_stream_session_t
    template<typename T>
    T& data(ngx_session_type* r) const
    {
        return ctx().template data<T>(r);
    }
};

#define NGX_MOD_INSTANCE(T, mod, ... )      \
    struct T {                              \
        typedef NgxModule<__VA_ARGS__> mod_type;   \
        typedef mod_type::config_type config_type;  \
        static mod_type& instance()         \
        {                                   \
            extern ngx_module_t mod;        \
            static mod_type m(mod);         \
            return m;                       \
        }                                   \
        static const config_type& conf() \
        {   return instance().conf();}             \
    };

#define NGX_MOD_CTX_INSTANCE(T, mod, c, ... )     \
    struct T {                                      \
        typedef NgxModule<__VA_ARGS__> mod_type;    \
        typedef mod_type::config_type config_type;  \
        typedef NgxModuleCtx ctx_type;              \
        typedef c ctx_data_type;                       \
        static mod_type& instance()         \
        {                                   \
            extern ngx_module_t mod;        \
            static mod_type m(mod);         \
            return m;                       \
        }                                   \
        static const config_type& conf() \
        {   return instance().conf();}             \
        static const ctx_type& ctx()                \
        {   return instance().ctx();}       \
        static ctx_data_type& data(mod_type::ngx_session_type* r)                 \
        {   return instance().template data<ctx_data_type>(r);}             \
    };
#endif  //_NGX_MODULE_HPP

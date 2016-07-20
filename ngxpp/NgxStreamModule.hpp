// Copyright (c) 2016
// Author: Chrono Law
#ifndef _NGX_STREAM_MODULE_HPP
#define _NGX_STREAM_MODULE_HPP

#include <type_traits>
#include <boost/utility/enable_if.hpp>

#include "NgxPool.hpp"

#ifdef META_TYPE
#undef META_TYPE
#endif

#define META_TYPE(t) typename eval_type<t>::type

template<typename T2 = void, typename T1 = void>
class NgxStreamModuleConfig final
{
private:
    template<typename T>
    struct eval_type :
        std::conditional<
            std::is_void<T>::value,
            void*,
            typename std::add_lvalue_reference<T>::type>
    {};
public:
    NgxStreamModuleConfig(ngx_uint_t idx): m_idx(idx)
    {}
    ~NgxStreamModuleConfig() = default;
private:
    ngx_uint_t m_idx = 0;
public:
    ngx_uint_t index() const
    {
        return m_idx;
    }

    template<typename T, typename U>
    typename std::enable_if<std::is_object<T>::value,T&>::type
    get(U conf) const
    {
        assert(conf[index()]);
        return *reinterpret_cast<T*>(conf[index()]);
    }

    template<typename T, typename U>
    typename std::enable_if<std::is_void<T>::value,void*>::type
    get(U conf) const
    {
        return conf[index()];
    }
public:
    META_TYPE(T1) main(ngx_stream_session_t* s) const
    {
        return get<T1>(s->main_conf);
    }

    META_TYPE(T2) srv(ngx_stream_session_t* s) const
    {
        return get<T2>(s->srv_conf);
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

private:
    ngx_stream_conf_ctx_t* ctx(ngx_conf_t* cf) const
    {
        return reinterpret_cast<ngx_stream_conf_ctx_t*>(cf->ctx);
    }
};

#undef META_TYPE

class NgxStreamModuleCtx final
{
public:
    NgxStreamModuleCtx(ngx_uint_t idx): m_idx(idx)
    {}
    ~NgxStreamModuleCtx() = default;
private:
    ngx_uint_t m_idx = 0;
public:
    ngx_uint_t index() const
    {
        return m_idx;
    }
public:
    bool empty(ngx_stream_session_t* s) const
    {
        return !ctx(s);
    }

    void clear(ngx_stream_session_t* s) const
    {
        ctx(s) = nullptr;
    }

    template<typename T>
    T& data(ngx_stream_session_t* s) const
    {
        if(empty(s))
        {
            ctx(s) = alloc<T>(s);
        }

        return *reinterpret_cast<T*>(ctx(s));
    }
private:
    typedef void* raw_pointer;

    raw_pointer& ctx(ngx_stream_session_t* s) const
    {
        //ngx_stream_get_module_ctx(s, module));
        return s->ctx[index()];
    }
private:
    template<typename T>
    T* alloc(ngx_stream_session_t* s,
            typename boost::disable_if<std::is_constructible<T, ngx_pool_t*>>::type* p=0) const
    {
        return NgxPool(s->connection).alloc<T>();
    }

    template<typename T>
    T* alloc(ngx_stream_session_t* s,
            typename boost::enable_if<std::is_constructible<T, ngx_pool_t*>>::type* p=0) const
    {
        return NgxPool(s->connection).construct<T>(s->connection->pool);
    }
};

template<typename T2 = void, typename T1 = void>
class NgxStreamModule   // no final
{
public:
    NgxStreamModule(ngx_module_t& m ):
        m_conf(m.ctx_index), m_ctx(m.ctx_index)
    {}

    ~NgxStreamModule() = default;
private:
    typedef NgxStreamModuleConfig<T2, T1> config_type;
    typedef NgxStreamModuleCtx ctx_type;

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
    T& data(ngx_stream_session_t* s) const
    {
        return ctx().template data<T>(s);
    }
};

class NgxStreamCoreModule final :
     public NgxStreamModule<ngx_stream_core_srv_conf_t,
                            ngx_stream_core_main_conf_t>
{
public:
    typedef NgxStreamModule<ngx_stream_core_srv_conf_t,
                            ngx_stream_core_main_conf_t>
            super_type;
    typedef NgxStreamCoreModule this_type;
public:
    NgxStreamCoreModule() : super_type(ngx_stream_core_module)
    {}

    ~NgxStreamCoreModule() = default;
public:
    void access(ngx_conf_t* cf, ngx_stream_access_pt f) const
    {
        conf().main(cf).access_handler = f;
    }

    void limit_conn(ngx_conf_t* cf, ngx_stream_access_pt f) const
    {
        conf().main(cf).limit_conn_handler = f;
    }
public:
    void handler(ngx_conf_t* cf, ngx_stream_handler_pt f) const
    {
        conf().srv(cf).handler = f;
    }
public:
    static NgxStreamCoreModule& instance()
    {
        static NgxStreamCoreModule m;
        return m;
    }
};

#define NGX_STREAM_MOD_INSTANCE(T, mod, ... )      \
    struct T {                              \
        typedef NgxStreamModule<__VA_ARGS__> mod_type;   \
        static mod_type& instance()         \
        {                                   \
            extern ngx_module_t mod;        \
            static mod_type m(mod);         \
            return m;                       \
        }                                   \
        mod_type& operator()() const        \
        {   return instance();}             \
    };

#define NGX_STREAM_MOD_CTX_INSTANCE(T, mod, c, ... )     \
    struct T {                                      \
        typedef NgxStreamModule<__VA_ARGS__> mod_type;    \
        typedef NgxStreamModuleCtx ctx_type;              \
        typedef c ctx_data_type;                       \
        static mod_type& instance()         \
        {                                   \
            extern ngx_module_t mod;        \
            static mod_type m(mod);         \
            return m;                       \
        }                                   \
        static const ctx_type& ctx()                \
        {   return instance().ctx();}       \
        static ctx_data_type& data(ngx_stream_session_t* s)                 \
        {   return instance().template data<ctx_data_type>(s);}             \
    };

#endif  //_NGX_STREAM_MODULE_HPP

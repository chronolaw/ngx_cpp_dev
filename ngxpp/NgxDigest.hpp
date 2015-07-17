// Author: Chrono Law
// Copyright (c) 2015
#ifndef _NGX_DIGEST_HPP
#define _NGX_DIGEST_HPP

extern "C" {
#include <ngx_md5.h>
#include <ngx_sha1.h>
#include <ngx_murmurhash.h>
}

#include <boost/utility/string_ref.hpp>
#include "Nginx.hpp"

struct NgxMd5Meta
{
    typedef ngx_md5_t ctx_type;

    static constexpr int len = 16;

    static constexpr auto init_func = &ngx_md5_init;
    static constexpr auto update_func = &ngx_md5_update;
    static constexpr auto final_func = &ngx_md5_final;
};

struct NgxSha1Meta
{
    typedef ngx_sha1_t ctx_type;

    static constexpr int len = 20;

    static constexpr auto init_func = &ngx_sha1_init;
    static constexpr auto update_func = &ngx_sha1_update;
    static constexpr auto final_func = &ngx_sha1_final;
};

struct NgxMurmurHash2Meta
{
    static constexpr auto update_func = &ngx_murmur_hash2;
};

template<typename DigestMeta>
class NgxDigest final
{
public:
    typedef DigestMeta meta;
    typedef boost::string_ref string_ref_type;
public:
    NgxDigest() //= default;
    {
        init();
    }
    ~NgxDigest() = default;
public:
    void init()
    {
        meta::init_func(&m_ctx);
    }

    void update(const void* data, std::size_t len)
    {
        meta::update_func(&m_ctx, data, len);
    }

    const u_char* final()
    {
        meta::final_func(m_buf, &m_ctx);

        return m_buf;
    }

    ngx_str_t str()
    {
        //final();

        ngx_hex_dump(m_hex, m_buf, sizeof(m_buf));

        return ngx_str_t{sizeof(m_hex), m_hex};
    }
public:
    ngx_str_t operator()(const void* data, std::size_t len)
    {
        init();
        update(data, len);
        final();

        return str();
    }

    ngx_str_t operator()(string_ref_type str)
    {
        return operator()(str.data(), str.size());
    }
private:
    typename meta::ctx_type m_ctx;

    u_char m_buf[meta::len];
    u_char m_hex[meta::len*2];
};

template<>
class NgxDigest<NgxMurmurHash2Meta> final
{
public:
    typedef NgxMurmurHash2Meta meta;
    typedef boost::string_ref string_ref_type;
public:
    NgxDigest() = default;
    ~NgxDigest() = default;
public:
    uint32_t operator()(const void* data, std::size_t len)
    {
        return meta::update_func((u_char*)data, len);
    }

    uint32_t operator()(string_ref_type str)
    {
        return operator()(str.data(), str.size());
    }
};

typedef NgxDigest<NgxMd5Meta> NgxMd5;
typedef NgxDigest<NgxSha1Meta> NgxSha1;
typedef NgxDigest<NgxMurmurHash2Meta> NgxMurmurHash2;

#endif  //_NGX_DIGEST_HPP

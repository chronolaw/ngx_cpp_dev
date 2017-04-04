// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_LOAD_BALANCE_HPP
#define _NGX_LOAD_BALANCE_HPP

#include "NgxPool.hpp"

template<typename T,
         ngx_event_get_peer_pt get_peer,
         ngx_event_free_peer_pt free_peer = nullptr,
         ngx_http_upstream_rr_peer_data_t T::* ptr = &T::rrp>
class NgxLoadBalance final
{
    typedef ngx_http_upstream_rr_peer_data_t peer_data_t;
    typedef ngx_http_upstream_srv_conf_t srv_conf_t;
public:
    static void init(ngx_conf_t *cf, srv_conf_t* us,
                    ngx_http_upstream_init_peer_pt init_peer)
    {
        auto rc = ngx_http_upstream_init_round_robin(cf, us);

        NgxException::require(rc);

        us->peer.init = init_peer;
    }
public:
    static T& init(ngx_http_request_t* r, srv_conf_t* us)
    {
        auto& peer_data = *NgxPool(r).alloc<T>();

        r->upstream->peer.data = &(peer_data.*ptr);

        auto rc = ngx_http_upstream_init_round_robin_peer(r, us);

        NgxException::require(rc);

        r->upstream->peer.get  = get_peer;
        r->upstream->peer.free = free_peer?free_peer:
                                 r->upstream->peer.free;

        return peer_data;
    }

public:
    static ngx_int_t round_robin(ngx_peer_connection_t* pc, void* data)
    {
        return ngx_http_upstream_get_round_robin_peer(pc, data);
    }
};

#endif  //_NGX_LOAD_BALANCE_HPP

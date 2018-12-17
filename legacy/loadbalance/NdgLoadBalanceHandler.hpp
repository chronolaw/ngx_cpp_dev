// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NDG_LOAD_BALANCE_HANDLER_HPP
#define _NDG_LOAD_BALANCE_HANDLER_HPP

#include <boost/random.hpp>

#include "NdgLoadBalanceConf.hpp"

class NdgLoadBalanceCallback final
{
public:
    typedef NgxLogDebug log;
    typedef NdgLoadBalanceCallback this_type;
    typedef boost::rand48 random_type;
public:
    // the round robin data must be first
    ngx_http_upstream_rr_peer_data_t   rrp;

    u_char  tries = 0;
    ngx_event_get_peer_pt get_rr_peer = ngx_http_upstream_get_round_robin_peer;
public:
    static ngx_int_t get(ngx_peer_connection_t *pc, void *data)
    {
        auto& peer_data = *reinterpret_cast<this_type*>(data);
        auto& peers = *peer_data.rrp.peers;

        if(peer_data.tries++ > 5 || peers.single)
        {
            return peer_data.get_rr_peer(pc, &peer_data.rrp);
        }

        static random_type rand(time(0));

        auto& peer = peers.peer[rand() % peers.number];

        pc->sockaddr = peer.sockaddr;
        pc->socklen = peer.socklen;
        pc->name = &peer.name;

        log(pc->log).print("peer=%V", &peer.name);

        pc->cached = false;
        pc->connection = nullptr;

        return NGX_OK;
    }

public:
    static void free(ngx_peer_connection_t *pc, void *data, ngx_uint_t state)
    {
        //auto peer_data = reinterpret_cast<this_type*>(data);
    }
};

class NdgLoadBalanceHandler final
{
public:
    typedef NdgLoadBalanceHandler this_type;
    typedef NdgLoadBalanceCallback callback_type;
    typedef NdgLoadBalanceCallback peer_data_type;

    //typedef NdgLoadBalanceModule this_module;
    typedef NgxLoadBalance<peer_data_type,
                           &callback_type::get>
                           //&callback_type::free>
                           this_load_balance;
public:
    static ngx_int_t init(ngx_conf_t *cf, ngx_http_upstream_srv_conf_t *us)
    try
    {
        this_load_balance::init(cf, us, &this_type::init_peer);
        return NGX_OK;
    }
    catch(const NgxException& e)
    {
        return e.code();
    }
public:
    static ngx_int_t init_peer(ngx_http_request_t *r, ngx_http_upstream_srv_conf_t *us)
    {
        this_load_balance::init(r, us);

        return NGX_OK;
    }
};

#endif  //_NDG_LOAD_BALANCE_HANDLER_HPP

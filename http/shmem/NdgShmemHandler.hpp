// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_SHMEM_HANDLER_HPP
#define _NDG_SHMEM_HANDLER_HPP

#include <boost/atomic.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include "NdgShmemConf.hpp"

class NdgShmemHandler final
{
public:
    typedef NdgShmemHandler this_type;
    typedef NdgShmemModule this_module;
    typedef NgxLogDebug log;

    typedef boost::interprocess::managed_shared_memory shmem_type;
    typedef boost::atomic<long> atomic_type;
public:
    static ngx_int_t init(ngx_conf_t* cf)
    {
        NgxHttpCoreModule::instance().handler(
            cf, &this_type::handler, NGX_HTTP_LOG_PHASE);

        return NGX_OK;
    }

public:
    static void exit_master(ngx_cycle_t *cycle)
    {
        boost::interprocess::shared_memory_object::remove("ndg_shmem");
    }
public:
    static ngx_int_t handler(ngx_http_request_t *r)
    try
    {
        static auto& cf = this_module::instance().conf().main(r);

        static shmem_type segment(
            boost::interprocess::open_or_create, "ndg_shmem",
            cf.size);

        auto& counter = *segment.find_or_construct<atomic_type>("counter")(0);

        counter += r->connection->sent;

        log(r).print("sent=%l", static_cast<long>(counter));

        return NGX_OK;
    }
    catch(const NgxException& e)
    {
        return e.code();
    }
};

#endif  //_NDG_SHMEM_HANDLER_HPP

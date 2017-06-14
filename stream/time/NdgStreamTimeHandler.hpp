// Copyright (c) 2016-2017
// Author: Chrono Law
#ifndef _NDG_STREAM_TIME_HANDLER_HPP
#define _NDG_STREAM_TIME_HANDLER_HPP

#include <iostream>

#include "NdgStreamTimeConf.hpp"

class NdgStreamTimeHandler final
{
public:
    typedef NdgStreamTimeHandler this_type;
    typedef NdgStreamTimeModule  this_module;
public:
    static void handler(ngx_stream_session_t *s)
    try
    {
        NgxConnection conn(s);

        NgxReadEvent rev = conn.read_event();
        rev.handler(&this_type::block_read_handler);

        NgxWriteEvent wev = conn.write_event();
        wev.handler(&this_type::time_write_handler);

        wev.process();
    }
    catch(const NgxException& e)
    {
        NgxLogError(s).print("error = %d", e.code());
    }
private:
    static void time_write_handler(ngx_event_t *ev)
    try
    {
        NgxWriteEvent wev(ev);
        NgxConnection conn(ev); // = wev.connection();

        // check timedout and error
        NgxException::fail(wev.expired(), NGX_ETIMEDOUT);
        NgxException::fail(conn.closed());

        if(!wev.ready())
        {
            //wev.timeout(100);
            //wev.wait();
            wev.wait_for(100);

            return;
        }

        NgxPool pool(conn);

        NgxBuf buf = pool.buffer(20);
        buf.printf("%T", ngx_time());
        buf.finish();

#if 0
        auto n = conn.send(buf.begin(), buf.size());

        // for more readable
        //NgxException::fail(n <= 0 && n != NGX_EAGAIN);
        NgxException::require(n > 0 || n == NGX_EAGAIN);

        if(n == NGX_EAGAIN)
        {
            //wev.timeout(100);
            //wev.wait();
            wev.wait_for(100);

            return;
        }

        // n > 0
        NgxStreamSession(conn).close();
#endif

        NgxChainNode n = pool.chain();
        n.set(buf);
        n.finish();

        NgxStreamSession s(conn);
        s.send(n);

        if(conn->buffered)
        {
            wev.wait_for(100);
            return;
        }

        s.close();
    }
    catch(const NgxException& e)
    {
        //NgxWriteEvent wev(ev);
        //NgxConnection conn = wev.connection();
        //NgxStreamSession s(conn);
        NgxStreamSession s(ev);

        //NgxLogError(s).print("error = %d", e.code());
        s.close(e.code());
    }
private:
    static void block_read_handler(ngx_event_t *ev)
    {
        // do nothing
    }
};

#endif  //_NDG_STREAM_TIME_HANDLER_HPP

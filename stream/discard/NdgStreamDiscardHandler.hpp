// Copyright (c) 2016-2017
// Author: Chrono Law
#ifndef _NDG_STREAM_DISCARD_HANDLER_HPP
#define _NDG_STREAM_DISCARD_HANDLER_HPP

#include <array>

#include "NdgStreamDiscardConf.hpp"

class NdgStreamDiscardHandler final
{
public:
    typedef NdgStreamDiscardHandler this_type;
    typedef NdgStreamDiscardModule  this_module;
public:
    static void handler(ngx_stream_session_t *s)
    try
    {
        NgxConnection conn(s);

        NgxWriteEvent wev = conn.write_event();
        //wev.handler(&this_type::block_write_handler);
        wev.handler([](ngx_event_t *ev){});

        NgxReadEvent rev = conn.read_event();

        rev.handler(&this_type::discard_read_handler);

        rev.process();
    }
    catch(const NgxException& e)
    {
        NgxLogError(s).print("error = %d", e.code());
        NgxStreamSession(s).close();
    }
private:
    static void discard_read_handler(ngx_event_t *ev)
    try
    {
        NgxReadEvent rev(ev);
        NgxConnection conn(ev);

        // check timedout and error
        NgxException::fail(rev.expired(), NGX_ETIMEDOUT);
        NgxException::fail(conn.closed());

        if(rev.ready())
        {
            // read from client
            std::array<u_char, 64> buf;
            NgxLogError log(conn);

            ssize_t n = 0;

            // read until can not read
            for(;;)
            {
                n = conn.recv(buf.data(), buf.size());

                if(n <= 0)  // error accured
                {
                    break;
                }

                log.print("recv %d bytes", n);

                conn.reusable(false);
            }   // end for

            //NgxException::fail(n == NGX_ERROR || n == 0);
            NgxException::fail(n != NGX_AGAIN);

            //(n == NGX_AGAIN)
        }

        NgxStreamSession s(ev);
        auto& cf = this_module::conf().srv(s);

        //rev.timeout(cf.timeout, true);
        //rev.wait();
        rev.wait_for(cf.timeout, true);
    }
    catch(const NgxException& e)
    {
        NgxStreamSession s(ev);

        NgxLogError(s).print("error = %d", e.code());
        s.close();
    }
private:
    static void block_write_handler(ngx_event_t *ev)
    {
        // do nothing
    }

};

#endif  //_NDG_STREAM_DISCARD_HANDLER_HPP

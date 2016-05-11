// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NGX_LOG_HPP
#define _NGX_LOG_HPP

#include <boost/mpl/logical.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/tti/has_member_data.hpp>
#include <boost/tti/member_type.hpp>

#include "NgxWrapper.hpp"

// for ngx_log_error
template<ngx_uint_t level = NGX_LOG_DEBUG>
class NgxLog final : public NgxWrapper<ngx_log_t>
{
public:
    typedef NgxWrapper<ngx_log_t> super_type;
    typedef NgxLog this_type;
public:
    NgxLog(ngx_log_t* l): super_type(l)
    {}

    NgxLog(ngx_log_t& l): super_type(l)
    {}

    ~NgxLog() = default;
public:
    // check member log or connection with boost.tti
    BOOST_TTI_HAS_MEMBER_DATA(log)
    BOOST_TTI_HAS_MEMBER_DATA(connection)
    BOOST_TTI_MEMBER_TYPE(wrapped_type)

    // T has member log
    template<typename T>
    NgxLog(T* x,
           typename boost::enable_if<
                has_member_data_log<T, ngx_log_t*>>::type* p = 0):
        NgxLog(x->log)
    {}

    // T has member connection but has not log
    // ngx_http_request_t/ngx_session_t
    template<typename T>
    NgxLog(T* x,
           typename boost::enable_if<
                boost::mpl::and_<
                    has_member_data_connection<T, ngx_connection_t*>,
                    boost::mpl::not_<has_member_data_log<T, ngx_log_t*>>
                    >
                >::type* p = 0):
        NgxLog(x->connection)
    {}

    // T = NgxConnection/NgxRequest/NgxStreamSession/...
    template<typename T>
    NgxLog(const T& x,
           typename boost::enable_if<
                boost::tti::valid_member_type<
                    member_type_wrapped_type<T>>
                >::type* p = 0):
           NgxLog(x.get())
    {}

    //NgxLog(T* x) : super_type(x->log) {}
    //NgxLog(ngx_http_request_t* r): NgxLog(r->connection){}

public:
    template<typename ... Args>
    void print(const char* fmt, const Args& ... args) const
    {
        ngx_log_error(level, get(), 0, fmt, args...);
    }

    template<typename ... Args>
    void print(ngx_err_t err, const Args& ... args) const
    {
        ngx_log_error(level, get(), err, args...);
    }
};

typedef NgxLog<NGX_LOG_DEBUG>   NgxLogDebug;
typedef NgxLog<NGX_LOG_INFO>    NgxLogInfo;
typedef NgxLog<NGX_LOG_WARN>    NgxLogWarning;
typedef NgxLog<NGX_LOG_ERR>     NgxLogError;
typedef NgxLog<NGX_LOG_ALERT>   NgxLogAlert;

// for ngx_log_stderr
class NgxStderrLog final
{
public:
    typedef NgxStderrLog this_type;
public:
    NgxStderrLog() = default;
    ~NgxStderrLog() = default;
public:
    template<typename ... Args>
    void print(const char* fmt, const Args& ... args) const
    {
        ngx_log_stderr(0, fmt, args...);
    }

    template<typename ... Args>
    void print(ngx_err_t err, const Args& ... args) const
    {
        ngx_log_stderr(err, args...);
    }
};

#endif  //_NGX_LOG_HPP

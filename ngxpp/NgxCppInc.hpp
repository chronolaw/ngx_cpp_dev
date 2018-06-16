// Copyright (c) 2016-2017
// Author: Chrono Law
#ifndef _NGX_CPPINC_HPP
#define _NGX_CPPINC_HPP

#if __cplusplus < 201103L
    #error "ngx_cpp_module need C++11 implementation!"
#endif

#include <cassert>

#include <string>
#include <deque>
#include <exception>

#include <boost/version.hpp>

// ignore_unused since boost 1.56
#if BOOST_VERSION < 105600
namespace boost {
template <typename... Ts>
inline void ignore_unused(Ts const& ...){}
}
#include <boost/utility/explicit_operator_bool.hpp>
#else
#include <boost/core/ignore_unused.hpp>
#include <boost/core/explicit_operator_bool.hpp>
#endif


#include <boost/type_traits.hpp>
#include <boost/noncopyable.hpp>
#include <boost/next_prior.hpp>
#include <boost/static_assert.hpp>
#include <boost/function.hpp>
//#include <boost/format.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/exception/all.hpp>
#include <boost/functional/factory.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>
#include <boost/tti/member_type.hpp>
#include <boost/tti/has_member_data.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/string_ref.hpp>

#endif  // _NGX_CPPINC_HPP

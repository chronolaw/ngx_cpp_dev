// Copyright (c) 2015-2016
// Author: Chrono Law
#ifndef _NGX_RBTREE_HPP
#define _NGX_RBTREE_HPP

#include <type_traits>

//#include "Nginx.hpp"
#include "NgxWrapper.hpp"

template<typename T,                    // data type
         ngx_rbtree_node_t T::* np,     // rbtree node pointer
         ngx_rbtree_insert_pt func      // insert function pointer
         >
class NgxRbtree final : public NgxWrapper<ngx_rbtree_t>
{
public:
    typedef NgxWrapper<ngx_rbtree_t>    super_type;
    typedef NgxRbtree                   this_type;

    typedef ngx_rbtree_key_t            key_type;

    typedef ngx_rbtree_t                tree_type;
    typedef ngx_rbtree_node_t           node_type;
public:
    NgxRbtree(tree_type* t) : super_type(t)
    {}

    NgxRbtree(tree_type& t) : super_type(&t)
    {}

    ~NgxRbtree() = default;
public:
    static void init(tree_type& tree, node_type& s)
    {
        ngx_rbtree_init(&tree, &s, func);
    }
public:
    bool empty() const
    {
        return get()->root == get()->sentinel;
    }

    key_type min_key() const
    {
        auto p = ngx_rbtree_min(get()->root, get()->sentinel);

        return p->key;
    }

    T& min() const
    {
        auto p = ngx_rbtree_min(get()->root, get()->sentinel);

        constexpr auto offset = (std::size_t)&(((T*)0)->*np);

        return *(T*)((u_char*)(p) - offset);
    }
public:
    void add(T& v) const
    {
        ngx_rbtree_insert(get(), &(v.*np));
    }

    void del(T& v) const
    {
        ngx_rbtree_delete(get(), &(v.*np));
    }
public:
    typedef ngx_str_node_t str_node_type;

    // only valid with ngx_str_node_t!
    str_node_type* find(ngx_str_t* val, uint32_t hash) const
    {
        static_assert(
            std::is_same<T, str_node_type>::value,
            "only ngx_str_node_t can find!");

        return ngx_str_rbtree_lookup(get(), val, hash);
    }
};

template<typename T, ngx_rbtree_node_t T::* np>
using NgxValueTree = NgxRbtree<T, np, ngx_rbtree_insert_value>;

template<typename T, ngx_rbtree_node_t T::* np>
using NgxTimerTree = NgxRbtree<T, np, ngx_rbtree_insert_timer_value>;

using NgxEventTimerTree = NgxTimerTree<
                            ngx_event_t, &ngx_event_t::timer>;

using NgxStringTree = NgxRbtree<
                        ngx_str_node_t, &ngx_str_node_t::node, ngx_str_rbtree_insert_value>;


#endif  // _NGX_RBTREE_HPP

// Copyright (c) 2016-2017
// Author: Chrono Law
#ifndef _NGX_KEYVALUE_HPP
#define _NGX_KEYVALUE_HPP

//#include "Nginx.hpp"

// ngx_string.h
//
// typedef struct {
//     ngx_str_t   key;
//     ngx_str_t   value;
// } ngx_keyval_t;
typedef ngx_keyval_t NgxKeyValue;

// ngx_hash.h
//
// typedef struct {
//     ngx_uint_t        hash;
//     ngx_str_t         key;
//     ngx_str_t         value;
//     u_char           *lowcase_key;
// } ngx_table_elt_t;
typedef ngx_table_elt_t NgxHashedKeyValue;

#endif    // _NGX_KEYVALUE_HPP

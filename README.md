Name
====
ngx_cpp_dev

Nginx cpp development kit, with the power of C++11 and Boost Library.

See also :
* [annotated_nginx](https://github.com/chronolaw/annotated_nginx) - 注释nginx1.10.1，学习研究源码
* [stream-lua-nginx-module](https://github.com/chronolaw/stream-lua-nginx-module) - Work with latest nginx, log_by_lua, and more enhanced

Table of Contents
=================

* [Name](#name)
* [Requirements](#Requirements)
* [Usage](#Usage)
* [Examples](Examples)
* [C++ APIs](#C++ APIs)
* [TODO](#TODO)

Requirements
============
* Linux or other UNIX like
* C++11(gcc 4.6 or later)
* Boost(1.57 or later)

Please see directiory "setup".

Usage
=====
At first, you must patch nginx/auto/make to enable g++ compiler and C++11.

Please see directory *"patch/auto"*.

For example:
~~~~
patch -b make make.patch
~~~~


Then configure nginx with:
~~~~
./configure --add-module=path/to/ngxpp	\	            # ngx cpp module
			--add-module=xxxx			                # other modules
make
~~~~

ngx_cpp_module is a pure header library, you should use ngx_cpp_module like this:
~~~
#include "NgxAll.hpp"	// include all c++ tools 
~~~


Examples
========
Please see directory *http*, including echo/filter/variables and so on.

C++ APIs
========

common tools
------

Class Name      | Description
----------------|------------------------
NgxUnsetValue   | generic unset value -1.
NgxValue        | wrapper for nginx int, include init/merge/unset.
NgxException    | wrapper for nginx error code.
NgxPool         | wrapper for nginx memory pool.
NgxAlloctor     | adapter to c++ allocator.
NgxString       | wrapper for nginx ngx_str_t.
NgxClock        | wrapper for nginx time functions.
NgxDatetime     | wrapper for nginx date functions.
NgxLog          | wrapper for nginx error log.

data structure
------

Class Name      | Description
----------------|------------------------
NgxArray        | wrapper for nginx ngx_array_t.
NgxList         | wrapper for nginx ngx_list_t.
NgxQueue        | wrapper for nginx ngx_queue_t.
NgxBuf          | wrapper for nginx ngx_buf_t.
NgxChain        | wrapper for nginx ngx_chain_t.
NgxRbtree       | wrapper for nginx ngx_rbtree_t.

modules
------

Class Name      | Description
----------------|------------------------
NgxModuleConfig | easy to access nginx module confing info.
NgxModule       | get module's config and ctx data.
NgxCommand      | wrapper for nginx ngx_command_t.
NgxTake         |
NgxModuleCtx    | easy to access nginx module's ctx data.
NgxHttpCoreModule|wrapper for ngx_http_core_module
NgxFilter       | wrapper for nginx filter mechanism.

http process
------

Class Name      | Description
----------------|------------------------
NgxHeaders      |
NgxRequest      |
NgxResponse     |
NgxUpstreamHelper|
NgxLoadBalance  |
NgxSubRequest   |

others
------
Class Name      | Description
----------------|------------------------
NgxVariables    |
NgxVarManager   |
NgxDigest       |
NgxTimer        |

TODO
====
* more test suites
* nginx stream
* nginx datagram
* c++ namespace
* ...

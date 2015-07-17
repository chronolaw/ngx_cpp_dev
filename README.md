Name
====
ngx_cpp_dev

Nginx cpp development kit, with the power of C++11 and Boost Library.

Requirements
============
* Linux or other UNIX like
* C++11(gcc 4.6 or later)
* Boost(1.57 or later)

Usage
=====
At first, we must patch nginx/auto/make to enable g++ compiler and C++11.

Then configure nginx with:
~~~~
./configure --add-module=path/to/ngxpp	\	  # ngx cpp module
			   --add-module=xxxx			            # other modules
make
~~~~

You should use ngx_cpp_module like this:
~~~
#include "NgxAll.hpp"	// include all c++ tools 
~~~

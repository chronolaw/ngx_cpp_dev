# Copyright (c) 2016 by chrono

use Test::Nginx::Socket 'no_plan';

run_tests();

__DATA__

=== Test 1 : headers

--- config
location = /filter {
    ndg_echo "filter\n";
    ndg_header x-name chrono;
    ndg_header x-value trigger;
}

--- request
GET /filter

--- response_body
filter

--- response_headers
x-name : chrono
x-value : trigger

=== Test 2 : footer

--- config
location = /filter {
    ndg_echo "hello\n";
    ndg_footer "ocarina of time\n";
}

--- request
GET /filter

--- response_body
hello
ocarina of time

=== Test 3 : headers and footer

--- config
location = /filter {
    ndg_echo "hello\n";
    ndg_footer "ocarina of time\n";
    ndg_header x-name chrono;
    ndg_header x-value trigger;
}

--- request
GET /filter

--- response_headers
x-name : chrono
x-value : trigger

--- response_body
hello
ocarina of time


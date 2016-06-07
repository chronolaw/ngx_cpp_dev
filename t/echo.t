# Copyright (c) 2016 by chrono

use Test::Nginx::Socket 'no_plan';

run_tests();

__DATA__

=== TEST 1 : echo no args

--- config
location = /echo {
    ndg_echo "hello nginx\n";
}

--- request
GET /echo

--- response_body
hello nginx

=== TEST 2 : echo with args

--- config
location = /echo {
    ndg_echo "hello\n";
}

--- request
GET /echo?chrono

--- response_body
chrono,hello

#!/bin/sh
env > test_env.txt;
exec php-cgi $1
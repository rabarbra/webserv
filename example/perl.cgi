#!/usr/bin/perl

use strict;
use warnings;

print "Status: 200 OK\r\n";
print "Content-type: text/html\r\n\r\n";

print "<html><head><title>CGI Environment Variables</title></head><body>";
print "<h1>CGI Environment Variables:</h1>";
print "<table border='1'>";
print "<tr><th>Variable</th><th>Value</th></tr>";

foreach my $key (sort keys %ENV) {
    print "<tr><td>$key</td><td>$ENV{$key}</td></tr>";
}

print "</table></body></html>";


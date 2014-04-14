#!/usr/bin/perl

use strict;

my $prog = './sgrep';

my %patterns = (
    "trademarks" => 'Apple|GCC|ISO|GNU',
    "email" => '[a-zA-Z0-9]+\@[a-zA-Z0-9]+',
    "three vowels" => "[aeiuoAEIUO][aeiuoAEIUO][aeiuoAEIUO]",
    "string" => '\"(\\.|[^\\"])*\"',
    "hex" => '0[xX][0-9a-fA-F]+',
    "integer" => '[0-9]+',
    "float" => '[0-9]+\.[0-9]*',
    "keyword" => 'void|int|float',
    "identifier" => '[a-zA-Z_][a-zA-Z_0-9]*',
    "directive" => '^#[^\n]*'
    );

my $IN = '/usr/include/openssl/ssl.h';
#my $IN = '/usr/include/math.h';
#my $IN = '/usr/include/apache2/httpd.h';

foreach my $key (sort keys %patterns) {
    my $pat = $patterns{$key};
    print "========$key pattern='$pat'==========\n";
    my $cmd = "$prog '$pat' < $IN | head -n 10";
    print "$cmd\n";
    system($cmd) == 0 or die "$!\n";
}

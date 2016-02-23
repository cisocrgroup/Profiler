#!/usr/bin/perl

use Getopt::Long;
use strict;

my $delimiter = '';
GetOptions( 'delimiter=s' => \$delimiter,);

my @lines = <>;

if( !$delimiter ) {
    @lines = sort(@lines);
}
else {
    @lines = sort my_cmp @lines;
}

for(@lines) {
    print $_;
}

sub my_cmp {
    $a =~ m/^(.*?)$delimiter/;
    my $aa= $1;
    $b =~ m/^(.*?)$delimiter/;
    my $bb= $1;
#    print "$aa, $bb\n";
    return ($aa cmp $bb);
}

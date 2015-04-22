#!/usr/bin/perl

use strict;

# This script does a very simple figure-of-merit evaluation on basis of
# the IBM groundtruth file.

#     0           1          2            3           4            5               6             7
# FileName, Ascii-Truth, Hex-Truth, ABBYY-Dict, ABBYY-Valid, ABBYY-OCR-Hex, ABBYY-OCR-Ascii, OCR-Status

my %c; # a counter

my $filterShortWords = 1;

while( my $line = <> ) {
    chomp $line;

    my @f=split(/,/, $line);

    if( $filterShortWords && ( length($f[6]) < 4 ) ) {
	++$c{short};
	next;
    }
    

    if( $f[4] eq '0' ) {
	++$c{suspicious};
    }
    elsif( $f[7] eq "FALSE" ) {
	++$c{falseButValid};
    }
}

print "short:          $c{short}\n";
print "suspicious:     $c{suspicious}\n";
print "falseButValid:  $c{falseButValid}\n";
print "penalty:        ",$c{suspicious} + 5* $c{falseButValid}, "\n";

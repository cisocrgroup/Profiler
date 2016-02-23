#!/usr/bin/perl

use strict;
use Getopt::Long;
use Tie::File;


my ($help,$byteOffset,$lineNumber);
GetOptions('help' => \$help,
	   'line' => \$lineNumber,
	   'byte' => \$byteOffset
	   );



my $db_file = shift @ARGV;

my @dbLines;
tie(@dbLines,'Tie::File', $db_file) or die $!;


for my $line (<>) {
    if($line=~m/^(?:\d+,)+\n$/) {
	chomp $line;
	chop $line; #remove last ','
	my @values = split(/,/,$line);
	for(@values) {
	    print "FO: ".$dbLines[$_-1],"\n";
	}
    }
    else {
	print $line;
    }
}

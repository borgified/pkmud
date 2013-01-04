#!/usr/bin/env perl

use warnings;
use strict;


my %db;

foreach my $area (<./PKarea/*.are>){
	open(INPUT,"$area") or die "cant open: $!\n";

	my $these_are_objects=0;
	$area=~/.*\/(.*)\.are/;
	print "area: $1\n";
	while(defined(my $line=<INPUT>)){
		if($line=~/^#OBJECTS$/){
			$these_are_objects=1;
		}
		if($line=~/^#0$/){
			$these_are_objects=0;
		}
		if($these_are_objects && $line=~/^#(\d+)$/){
			print "$1\n";
		}
	}
}

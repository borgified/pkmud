#!/usr/bin/env perl

use warnings;
use strict;

open(INPUT,"allobj_db");

my $counter=1;

while(defined(my $line=<INPUT>)){
	if($line=~/(\d+),/){
		my $detected_num = $1;
		while($detected_num != $counter){
			print "$counter\n";
			$counter++;
		}
	$counter++;
	}
}

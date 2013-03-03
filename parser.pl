#!/usr/bin/env perl

use strict;
use warnings;

my @areas = <./PKarea/*.are>;

foreach my $area_file (@areas){
	open(INPUT,$area_file);
	my $objects_section=0;
	while(defined(my $line = <INPUT>)){
		if($line =~ /^#OBJECTS/){
			$objects_section=1;
		}
		if($objects_section && $line =~ /^#0/){
			$objects_section=0;
		}

		if($objects_section){
			print $line;
		}
	}
}

__END__
all not null
id, primary key, auto increment, unsigned smallint
vnum, unsigned smallint
keyword_list, char(32)
short_description, char(64)
object_description, char(64)
item_type, unsigned tinyint
extra_bits, unsigned mediumint
wear_flags, unsigned mediumint
item_type_dependant_values, char(12)
weight, unsigned smallint
value, unsigned mediumint
load_percentage, char(2)
affect, enum




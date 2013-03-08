#!/usr/bin/env perl

use strict;
use warnings;

my @areas = <./PKarea/*.are>;

foreach my $area_file (@areas){
	open(INPUT,$area_file);
	my $objects_section=0;
	my @objects;
	while(defined(my $line = <INPUT>)){
		if($line =~ /^#OBJECTS/){
			$objects_section=1;
		}
		if($objects_section && $line =~ /^#0/){
			$objects_section=0;
		}

		if($objects_section){
			chomp($line);
			push(@objects,$line);
		}
	}

	for(my $i=0;$i<$#objects;$i++){
		my($vnum,$keywords,$short_description,$object_description);
		my($action_description,$item_type,$extra_bits,$wear_flags);
		my(@item_dependent_values);
		if($objects[$i]=~/^#(\d+)/){
			$vnum=$1;
			my $tildes_found=0;
			my $ksoa = "";
			my $x=1;
			while($tildes_found < 4){
				if($objects[$i+$x] =~ /~/){
					$tildes_found++;
				}
				$ksoa=$ksoa.$objects[$i+$x];
				$x++;
			}
			($keywords,$short_description,$object_description,$action_description)=split(/~/,$ksoa);
			print "$vnum $keywords $short_description $object_description $action_description\n";

			($item_type,$extra_bits,$wear_flags)=split(/ /,$objects[$i+$x++]);
			@item_dependent_values=split(/ /,$objects[$i+$x++]);
			if(!defined($extra_bits)){
				print "$vnum $keywords $short_description $object_description\n";
				exit;
			}

			print "$item_type $extra_bits $wear_flags @item_dependent_values\n";
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




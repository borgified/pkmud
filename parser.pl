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
		my($weight,$value,$load_percentage);
		my $ekeyword_edescription="";
		my @affects;

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
				print "error with extra_bits\n";
				exit;
			}
			print "iewi: $item_type $extra_bits $wear_flags @item_dependent_values\n";

			($weight,$value,$load_percentage)=split(/ /,$objects[$i+$x++]);
			print "wvl: $weight $value $load_percentage\n";

			#if(!defined($objects[$i+$x])){
			if(!defined($objects[$i+$x])){
				print "i: $i, x: $x $#objects\n";
				next;
			}

			$tildes_found=0;
			if($objects[$i+$x] =~ /^E\s*$/){
				while($tildes_found < 2){
					$x++;
					if($objects[$i+$x] =~ /~/){
						$tildes_found++;
					}
					$ekeyword_edescription=$ekeyword_edescription.$objects[$i+$x];
				}
			}elsif($objects[$i+$x] =~ /^A\s*$/){
				my($a,$b)=split(/ /,$objects[$i+ ++$x]);
				push(@affects,$b);
				push(@affects,$a);
			}elsif($objects[$i+$x] =~ /^#\d+\s*$/){
			}else{
				print "why am i here?: $objects[$i+$x] a\n";exit;
			}
			if($ekeyword_edescription ne ''){
				print "ekdk: $ekeyword_edescription\n";
			}
			if($#affects>0){
				print "a: @affects\n";
			}
		} #ends if($objects[$i]=~/^#(\d+)/){
	} #ends outer for
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

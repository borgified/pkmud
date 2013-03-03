#!/usr/bin/env perl

use warnings;
use strict;
use DBI;

open(INPUT,"input");

my %db;
my $vnum;

while(defined(my $line = <INPUT>)){

  if($line =~ /Loads [io]n (.*): (.*)$/){
# there can be multiple stats affected, so we 
# push everything into an array
		if(exists($db{$vnum}{'whereload'})){
			my $a = $1;
			my $b = $2;
#			$b =~ s/ /_/g;
			push($db{$vnum}{'whereload'},"$a:$b");
		}else{
			my $a = $1;
			my $b = $2;
#			$b =~ s/ /_/g;
			$db{$vnum}{'whereload'}=["$a:$b"];
		}
  }

	elsif($line =~ /That object doesn't seem to load\./){
		next;
	}

	elsif($line =~ /You say '(\d+)'/){
# this is the beginning of a new item and also
# the way we know that the end of the current item
# has been reached

		$vnum=$1;

	}

  else{
    print "i dont recognize this line: (for $vnum)\n";
    print $line;
    exit;
  }

}

sub printHashContents{

	foreach my $vnum (sort {$a<=>$b} keys %db){
		print "vnum: $vnum\n";
		foreach my $category (sort keys $db{$vnum}){
			if($category eq 'whereload'){
				foreach my $item ($db{$vnum}{'whereload'}){
					print "whereload: @$item\n";
				}
			}else{
				print "$category : $db{$vnum}{$category}\n";
			}
		}
		print "========================\n";
	}
}

#&printHashContents;


my $my_cnf = '/secret/my_cnf.cnf';

my $dbh = DBI->connect("DBI:mysql:"
		. ";mysql_read_default_file=$my_cnf"
		.';mysql_read_default_group=pkmud',
		undef,
		undef
		) or die "something went wrong ($DBI::errstr)";


#forming update query

sub storeDB{
	
	my $sql="update pkmud set whereload = ? where vnum = ?";
	my $sth=$dbh->prepare($sql) or die ("cannot prepare: ".$dbh->errstr());


	foreach my $vnum (sort {$a<=>$b} keys %db){
		print "vnum: $vnum\n";
		foreach my $category (sort keys $db{$vnum}){
			if($category eq 'whereload'){
				foreach my $item ($db{$vnum}{'whereload'}){
					print "whereload: @$item\n";
					$sth->execute("@$item","$vnum");
				}
			}else{
				print "oops\n";
				exit;
			}
		}
		print "========================\n";
	}
}

&storeDB;

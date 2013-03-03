#!/usr/bin/env perl

use warnings;
use strict;
use DBI;

my %db;

foreach my $area_file (<./PKarea/*.are>){
	open(INPUT,"$area_file") or die "cant open: $!\n";

	my $these_are_objects=0;
	$area_file=~/.*\/(.*)\.are/;
	#print "area: $1\n";
	my $area=$1;
	while(defined(my $line=<INPUT>)){
		if($line=~/^#OBJECTS$/){
			$these_are_objects=1;
		}
		if($line=~/^#0$/){
			$these_are_objects=0;
		}
		if($these_are_objects && $line=~/^#(\d+)/){
			$db{$1}=$area;
			#print "$1\n";
		}
	}
}

#print %db
foreach my $vnum (sort keys %db){
	print "$vnum: $db{$vnum}\n";
}


#__END__

#store in db

my $my_cnf = '/secret/my_cnf.cnf';

my $dbh = DBI->connect("DBI:mysql:"
. ";mysql_read_default_file=$my_cnf"
.';mysql_read_default_group=pkmud',
undef,
undef
) or die "something went wrong ($DBI::errstr)";

my $sql="update pkmud set area = ? where vnum = ?";
my $sth=$dbh->prepare($sql) or die ("cannot prepare: ".$dbh->errstr());


foreach my $vnum (keys %db){
	print "$vnum: $db{$vnum}\n";
	$sth->execute("$db{$vnum}","$vnum");
}


#!/usr/bin/env perl

use strict;
use warnings;
use CGI qw /:standard/;
use DBI;


my $my_cnf = '/secret/my_cnf.cnf';

my $dbh = DBI->connect("DBI:mysql:" . ";mysql_read_default_file=$my_cnf" .';mysql_read_default_group=pkmud', undef, undef) or die "something went wrong ($DBI::errstr)";

my $sql = "describe pkmud";
my $sth=$dbh->prepare($sql) or die ("Cannot prepare: ".$dbh->errstr());
$sth->execute();

#my %columns;
my $i=0;
my @columns;

while(my($col) = $sth->fetchrow_array){
	#$columns{$i++}=$col;
	push(@columns,$col) unless($col eq 'id');
}

my $cgi = new CGI;

my @params=$cgi->param('categories');

%type = (
	damage		=> 'text',
	namelist	=> 'text',
	spells		=> 'text',
	whereload	=> 'text',
	area			=> 'area',
	type			=> 'type',
	);


foreach my $param (@params){

	if($type{$param} eq 'text'){
	}elsif($type{$param} eq 'type'){
#select distinct(type) from pkmud
	}elsif($type{$param} eq 'area'){
#select distinct(area) from pkmud
	}else{
#treat as if numerical
		#select max($param) from pkmud
		#select min($param) from pkmud
	}
}

my $submit = submit();

print header;

print <<END;
<form action="step3.pl" name="myform" method="post">
determine how you would like to filter your results
<br>
$submit
<br>
@params

</form>
END


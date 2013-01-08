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

#my $dropdown = $cgi->popup_menu(-name=>'column' , -values=>\%columns );
my $dropdown = $cgi->popup_menu(-name=>'column' , -values=>\@columns );

print header;

print <<END;
<form action="lu.pl">

$dropdown

</form>
END


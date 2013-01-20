#!/usr/bin/env perl

use strict;
use warnings;
use CGI qw /:standard/;
use DBI;
use Carp;

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

my %format = (
	damage		=> 'text',
	namelist	=> 'text',
	spells		=> 'text',
	whereload	=> 'text',
	charges		=> 'text',
	area			=> 'area',
	type			=> 'type',
	);

my $string;

my $types=$dbh->prepare("select distinct(type) from pkmud");
my $areas=$dbh->prepare("select distinct(area) from pkmud");

foreach my $param (@params){
	if(!exists($format{$param})){
#treat as if numerical (default)
		#select max($param) from pkmud
		#select min($param) from pkmud
		my $minmax=$dbh->prepare("select min($param),max($param) from pkmud");
		$minmax->execute();
		my($min,$max)=$minmax->fetchrow_array;
		if($min eq ''){
			$min=0;
		}

		$string=$string."<tr><td>&uarr;<input type='radio' name=$param.sort value=0>&darr;<input type='radio' name=$param.sort value=1></td><td>$param</td><td><select name='$param.logic'><option value='1'>=</option><option value='2'>&ne;</option><option value='3'>&lt;</option><option value='4'>&le;</option><option value='5'>&gt;</option><option value='6'>&ge;</option></td><td><input type='text' name='$param'></td><td>range: ($min,$max)</td></tr>";

	}elsif($format{$param} eq 'text'){
		$string=$string."<tr><td>&uarr;<input type='radio' name=$param.sort value=0>&darr;<input type='radio' name=$param.sort value=1></td><td>$param</td><td>eq</td><td><input type='text' name='$param'></td><td>string match</td></tr>";
	}elsif($format{$param} eq 'type'){
#select distinct(type) from pkmud
		$types->execute();
		my @types;
		while(my $type=$types->fetchrow_array){
			push(@types,$type);
		}
		@types=sort(@types);
		unshift(@types,'');
		my $dropdown = $cgi->popup_menu(-name=>$param, -values=>\@types);
		$string=$string."<tr><td></td><td>$param</td><td></td><td>$dropdown</td><td></td></tr>";
	}elsif($format{$param} eq 'area'){
#select distinct(area) from pkmud
		$areas->execute();
		my @areas;
		while(my $area=$areas->fetchrow_array){
			push(@areas,$area);
		}
		@areas=sort(@areas);
		unshift(@areas,'');
		my $dropdown = $cgi->popup_menu(-name=>$param, -values=>\@areas);
		$string=$string."<tr><td></td><td>$param</td><td></td><td>$dropdown</td><td></td></tr>";
	}else{
		die "i shouldnt be here";
	}
}

my $submit = submit();

print header;

print <<END;
<form action="lu.pl" name="myform" method="post">
determine how you would like to filter and sort your results
<br>
$submit
<br>
<table>
$string
</table>

</form>
END


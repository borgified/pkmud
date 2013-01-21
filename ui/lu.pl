#!/usr/bin/env perl

use strict;
use warnings;
use CGI qw /:standard/;
use DBI;
use CGI::Carp qw(fatalsToBrowser);

my $my_cnf = '/secret/my_cnf.cnf';

my $dbh = DBI->connect("DBI:mysql:" . ";mysql_read_default_file=$my_cnf" .';mysql_read_default_group=pkmud', undef, undef) or die "something went wrong ($DBI::errstr)";

my $cgi = new CGI;

my @params=$cgi->param;

my %format = (
	damage		=> 'text',
	namelist	=> 'text',
	spells		=> 'text',
	whereload	=> 'text',
	charges		=> 'text',
	area			=> 'area',
	type			=> 'type',
	);

#sanitizing inputs

my $out = "";

foreach my $param (sort @params){
	if(!exists($format{$param})){
#the value of this param should be numeric
		my $val = param($param);
		if($val ne '' && $val!~/^-?\d+$/){
			print header;
			print "bad value for $param (illegal)";
			exit;
		}
	}else{
		my $val = param($param);
		if($val =~ /[\;\(\)\%\#\@\!\,\.+\=\\\/\^\?\`\:\&\<\>]/){
			print header;
			print "bad value for $param (illegal)";
			exit;
		}
	}
	$out=$out.$param.":".param($param)."<br>";
}

#end sanitizing inputs



my $string;

my $types=$dbh->prepare("select distinct(type) from pkmud");
my $areas=$dbh->prepare("select distinct(area) from pkmud");

my(@sort,@logic,@cat);

foreach my $param (@params){
	if($param =~ /\.sort\b/){
		push(@sort,$param);
	}elsif($param =~ /\.logic\b/){
		push(@logic,$param);
	}else{
		push(@cat,$param);
	}
}

my $num_cat=@cat;
my $num_sort=@sort;
my $num_logic=@logic;

if($num_cat == 0){
	print header;print "go back and pick something to display";exit;
}

my $sql_str="";

#select
my @origcat=@cat;
@cat=map { $_."," } @cat;
my $cat_str = sprintf('%s',"@cat");
$cat_str =~ s/,$//;

if($num_logic == 0){
	$sql_str="select $cat_str from pkmud";
}else{
#filter
	my %logic = qw/1 = 2 != 3 < 4 <= 5 > 6 >=/;

	my $logic_str="";

	foreach my $item (@logic){
		my $logicval = param($item);
		$item=~/(.*)\.logic/;
		my $val = param($1);
		if($val ne ''){
			$logic_str=$logic_str."$1 $logic{$logicval} $val and ";
		}
	}

#check if we need to add regexp
	foreach my $param (@params){
		if(exists($format{$param}) && $format{$param} eq 'text'){
			my $val=param($param);
			if($val ne ''){
				$logic_str=$logic_str."$param regexp \'$val\' and ";
			}
		}
	}

#check if we need to add type filter
	foreach my $param (@params){
		if(exists($format{$param}) && $format{$param} eq 'type'){
			my $val=param($param);
			if($val ne ''){
				$logic_str=$logic_str."$param = \'$val\' and ";
			}
		}
	}

#check if we need to add area filter
	foreach my $param (@params){
		if(exists($format{$param}) && $format{$param} eq 'area'){
			my $val=param($param);
			if($val ne ''){
				$logic_str=$logic_str."$param = \'$val\' and ";
			}
		}
	}



	$logic_str =~ s/and $//;

	if($logic_str ne ''){
		$sql_str="select $cat_str from pkmud where $logic_str";
	}else{
		$sql_str="select $cat_str from pkmud";
	}
}

if($num_sort > 0){

#sort
	my $sort_str;

	foreach my $item (@sort){
		my $sortval = param($item);
		$item=~/(.*)\.sort/;
		if($sortval){
			$sort_str=$sort_str."$1 desc,";
		}else{
			$sort_str=$sort_str."$1 asc,";
		}
	}

	$sort_str =~ s/,$//;

	$sql_str=$sql_str." order by $sort_str";
}

#print output table

my $output = $dbh->prepare($sql_str);
$output->execute();

my $output_table="<tr>";
foreach my $item (@origcat){
	$output_table=$output_table."<th>$item</th>";
}
$output_table=$output_table."</tr>";

while(my(@output)=$output->fetchrow_array){
	$output_table=$output_table."<tr>";
	foreach my $item (@output){
		$output_table=$output_table."<td>$item</td>";
	}
	$output_table=$output_table."</tr>\n";
}

$output_table=$output_table."</table>";


#sort:@sort
#<br>
#logic:@logic
#<br>
#cat:@cat
#<hr>
#$out
#<hr>
#$sql_str
#<hr>


print header;

print <<END;
<table border='1'>
$output_table
</table>
END


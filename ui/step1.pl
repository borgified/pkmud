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
#my $dropdown = $cgi->popup_menu(-name=>'column' , -values=>\@columns );
my $checkbox = $cgi->checkbox_group( -name => 'categories', -values=>\@columns, -linebreak => 'true');

my $submit = submit();

print header;

print <<END;
<SCRIPT LANGUAGE="JavaScript">
<!--	
// by Nannette Thacker
// http://www.shiningstar.net
// This script checks and unchecks boxes on a form
// Checks and unchecks unlimited number in the group...
// Pass the Checkbox group name...
// call buttons as so:
// <input type=button name="CheckAll"   value="Check All"
	//onClick="checkAll(document.myform.list)">
// <input type=button name="UnCheckAll" value="Uncheck All"
	//onClick="uncheckAll(document.myform.list)">
// -->

<!-- Begin
		function checkAll(field)
		{
			for (i = 0; i < field.length; i++)
					field[i].checked = true ;
		}

		function uncheckAll(field)
		{
			for (i = 0; i < field.length; i++)
					field[i].checked = false ;
		}
//  End -->
</script>



check the categories you are interested in
<form action="step2.pl" name="myform" method="post">

<input type="button" name="CheckAll" value="Check All" onClick="checkAll(document.myform.categories)">
<input type="button" name="UnCheckAll" value="Uncheck All" onClick="uncheckAll(document.myform.categories)">
$submit
<br>
$checkbox

</form>
END


#!/usr/bin/env perl

use strict;
use warnings;
use DBI;
use Net::Telnet;

my %config = do "/secret/pkmud.config";

my $login=$config{'username'};
my $password=$config{'password'};

my $t = new Net::Telnet (Timeout => 10,Port => 5000, Input_log => 'input', Output_Log => 'output');
$t->open("pkmud.net");
$t->waitfor('/wish to be known\? /');
sleep 1;
$t->print($login);
$t->waitfor('/Password: /');
sleep 1;
$t->print($password);
sleep 1;
$t->print("");
sleep 1;
$t->print("goto orb");
sleep 1;
$t->print("nop");
sleep 1;

#fetch vnums
my $my_cnf = '/secret/my_cnf.cnf';

my $dbh = DBI->connect("DBI:mysql:" . ";mysql_read_default_file=$my_cnf" .';mysql_read_default_group=pkmud', undef, undef) or die "something went wrong ($DBI::errstr)";

my $sql = "select vnum from pkmud";
my $sth=$dbh->prepare($sql) or die ("Cannot prepare: ".$dbh->errstr());
$sth->execute();

while(my($obj)=$sth->fetchrow_array){
	$t->print("say $obj");
	$t->print("whereload $obj");
	my $data = $t->get;
	print $data;
	sleep 1;
}

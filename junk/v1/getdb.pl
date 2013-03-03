#!/usr/bin/env perl

use strict;
use warnings;

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

#my $obj=29885;
my $obj=1;

while($obj < 32769){
	$t->print("say $obj");
	$t->print("vostat $obj");
	my $data = $t->get;
	print $data;
	sleep 1;
	$obj++;
}
#print "@lines";

__END__
$t->print($login);
$t->waitfor(/N/);
$t->print("y");
$t->waitfor('/password for Borg: /');
$t->print($password);
$t->waitfor('/retype password: /');
$t->print($password);
$t->waitfor('/sex/');
$t->print("m");
$t->waitfor('/PRESS RETURN\*\*/');
$t->print("\n");

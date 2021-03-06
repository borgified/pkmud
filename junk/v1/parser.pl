#!/usr/bin/env perl

use warnings;
use strict;
use DBI;

open(INPUT,"allisf");

my %db;
my $vnum;

while(defined(my $line = <INPUT>)){

	if($line =~ /Namelist: (.*)\.\s+Type: (.*)\./){
		$db{$vnum}{'namelist'}=$1;
		$db{$vnum}{'type'}=$2;
	}

	elsif($line =~ /Weight: (\d+),  Cost: (\d+)\.  Load Percent: (\d+)\%\./){
		$db{$vnum}{'weight'}=$1;
		$db{$vnum}{'cost'}=$2;
		$db{$vnum}{'load_percent'}=$3;
	}

	elsif($line =~ /Damage is (.*) \(average (\d+)\)\./){
		$db{$vnum}{'damage'}=$1;
		$db{$vnum}{'average'}=$2;
    #print "$damage $avg\n";exit;
  }

  elsif($line =~ /Affects (.*) by (.*)\./){
# there can be multiple stats affected, so we 
# push everything into an array
		if(exists($db{$vnum}{'affects'})){
			my $a = $1;
			my $b = $2;
			$a =~ s/ /_/g;
			push($db{$vnum}{'affects'},"$a:$b");
		}else{
			my $a = $1;
			my $b = $2;
			$a =~ s/ /_/g;
			$db{$vnum}{'affects'}=["$a:$b"];
		}
  }

	elsif($line =~ /Object has wear fun\./){
		$db{$vnum}{'wearfun'}=1;
	}

	elsif($line =~ /Object has special function\./){
		$db{$vnum}{'special'}=1;
	}

  elsif($line =~ /Level (\d+) spells of:(.*)\.$/){
		my $a=$1;
		my $b=$2;
		$b =~ s/' '/\//g;
		$b =~ s/'//g;
		$b =~ s/^ //;
		$b =~ s/ /_/g;
		if(exists($db{$vnum}{'spells'})){
			push($db{$vnum}{'spells'},"$a:$b");
		}else{
			$db{$vnum}{'spells'}=["$a:$b"];
		}
  }

  elsif($line =~ /Has (\d+)\((\d+)\) charges of level (\d+) '(.*)'\./){
		my $a=$1;
		my $b=$2;
		my $c=$3;
		my $d=$4;
		$d =~ s/ /_/g;
		if(exists($db{$vnum}{'charges'})){
			push($db{$vnum}{'charges'},"$a:$b:$c:$d");
		}else{
			$db{$vnum}{'charges'}=["$a:$b:$c:$d"];
		}
	}

	elsif($line =~ /Has (\d+)\((\d+)\) charges of level (\d+)\./){
		if(exists($db{$vnum}{'charges'})){
			push($db{$vnum}{'charges'},"$1:$2:$3");
		}else{
			$db{$vnum}{'charges'}=["$1:$2:$3"];
		}
	}

	elsif($line =~ /Armor class is (-?\d+)\./){
		$db{$vnum}{'ac'}=$1;
	}


# stuff to skip

	elsif($line =~ /No such object\.|Short description|Long description|Values:|^$|blood red flowers\.$|Fwiffo leaves in a swirling mist\.|the front\.\.\.|controling the weapon\.|equipment to use when fighting\.|combinations, has been tossed aside here\.|\*{4}|has arrived\.$|hits you\.$|You.*attack\.$|You.*hit\.$|is dead! R\.I\.P\.$|Your blood freezes as you hear.*death cry\.$|The.*leaves (north|south|east|west)\.$|The day has begun\.$|The sun rises in the east\.$|Corpse of.*is grabbed by a quivering hoard of pack rats\.$|Hithere leaves (north|south|east|west)\.$|Howler eats the sun\.\.it gets dark\.$|The night has begun\.$|Hithere has lost its link\.$|Hithere has left the game\.$|The sky is getting cloudy\.$|Corpse of Locutus decays into dust\.$|Rip gossips|brightly\.$|It starts to rain\.$|Lightning flashes in the sky\.$|The lightning has stopped\.$|The rain stopped\.$/){
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
			if($category eq 'affects'){
				foreach my $item ($db{$vnum}{'affects'}){
					print "affects: @$item\n";
				}
			}elsif($category eq 'spells'){
				foreach my $item ($db{$vnum}{'spells'}){
					print "spells: @$item\n";
				}
			}elsif($category eq 'charges'){
				foreach my $item ($db{$vnum}{'charges'}){
					print "charges: @$item\n";
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

sub countColumns{

#count how many columns there should be for the table
	my %columns; #columns in database
	$columns{"vnum"}=1;

		foreach my $vnum (sort {$a<=>$b} keys %db){
#	print "vnum: $vnum\n";
			foreach my $category (sort keys $db{$vnum}){
				if(!exists($columns{$category})){
					$columns{$category}=1;
				}
				if($category eq 'affects'){
					foreach my $item (@{$db{$vnum}{'affects'}}){
						my @a = split(/:/,$item);
						if(!exists($columns{$a[0]})){
							$columns{$a[0]}=1;
						}
					}
				}elsif($category eq 'spells'){
					foreach my $item ($db{$vnum}{'spells'}){
#				print "spells: @$item\n";
					}
				}elsif($category eq 'charges'){
					foreach my $item ($db{$vnum}{'charges'}){
#				print "charges: @$item\n";
					}
				}else{
#			print "$category : $db{$vnum}{$category}\n";
				}
			}
#				print "========================\n";
		}

#dont need affects since each affect already gets a column of its own
	delete($columns{'affects'});

#this shows what columns are used
	my $allcolumns = "";
	my @a = sort keys %columns;
	foreach my $item (@a){
		if($item =~ /spells|charges|namelist|special|type|wearfun/){
			#print "`$item` text NOT NULL,\n";
			$allcolumns=$allcolumns."`$item` tinytext NOT NULL,";
		}elsif($item =~ /damage/){
			$allcolumns=$allcolumns."`$item` varchar(5) NOT NULL,";
		}else{
			#print "`$item` int(4) NOT NULL,\n";
			$allcolumns=$allcolumns."`$item` int(11) NOT NULL,";
		}
	}
	

my $createtable=<<END;
CREATE TABLE IF NOT EXISTS `pkmud` ( `id` int(11) NOT NULL AUTO_INCREMENT, 
$allcolumns
PRIMARY KEY (`id`)) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1;
END

print $createtable;

my $sth=$dbh->prepare($createtable) or die ("Cannot prepare: ".$dbh->errstr());
$sth->execute();
}

&countColumns;

#forming insert query

sub storeDB{

	foreach my $vnum (sort {$a<=>$b} keys %db){
		print "vnum: $vnum\n";
		my $cols = "vnum";
		my $vals = "$vnum";
		foreach my $category (sort keys $db{$vnum}){
			if($category eq 'affects'){
				foreach my $item (@{$db{$vnum}{'affects'}}){
					my @a = split(/:/,$item);
					$cols = $cols.",".$a[0];
					$vals = $vals.",".$a[1];
					print "affects: $item\n";
				}
			}elsif($category eq 'spells'){
				foreach my $item ($db{$vnum}{'spells'}){
					print "spells: @$item\n";
				}
				$cols = $cols.",".$category;
				$vals = $vals.","."@{$db{$vnum}{$category}}";
			}elsif($category eq 'charges'){
				foreach my $item ($db{$vnum}{'charges'}){
					print "charges: @$item\n";
				}
				$cols = $cols.",".$category;
				$vals = $vals.","."@{$db{$vnum}{$category}}";
			}else{
				print "$category : $db{$vnum}{$category}\n";
				$cols = $cols.",".$category;
				$vals = $vals.",".$db{$vnum}{$category};
			}

		}
		$vals =~ s/,/','/g;
		$vals = "'".$vals."'";
		print "insert into pkmud ($cols) values ($vals)\n";

		my $sql="insert into pkmud ($cols) values ($vals)";
		my $sth=$dbh->prepare($sql) or die ("Cannot prepare: ".$dbh->errstr());
		$sth->execute();

		print "========================\n";
	}

}

&storeDB;

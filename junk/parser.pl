#!/usr/bin/env perl

use warnings;
use strict;

open(INPUT,"allisf");

my ($vnum,$namelist,$type,$weight,$cost,$lp,$damage,$avg,$affected_stat,$affected_val,$affected_stat1,$affected_val1,$affected_stat2,$affected_val2,$affected_stat3,$affected_val3,$affected_stat4,$affected_val4,$affected_stat5,$affected_val5,$affected_stat6,$affected_val6,$affected_stat7,$affected_val7,$special,$spell_lvl,$spell,$ac,$num_charges,$total_charges,$wand_lvl,$wand_spell,$wearfun)=map {"na"}(0..29);

my %db;
my $skip=1;
my $affect=0; #keep track of how many affected stats

while(defined(my $line=<INPUT>)){

	if($line =~ /You say '(\d+)'/){
		if($skip==0){
			print "$vnum,$namelist,$type,$weight,$cost,$lp,$damage,$avg,$affected_stat,$affected_val,$affected_stat1,$affected_val1,$affected_stat2,$affected_val2,$affected_stat3,$affected_val3,$affected_stat4,$affected_val4,$affected_stat5,$affected_val5,$affected_stat6,$affected_val6,$affected_stat7,$affected_val7,$special,$spell_lvl,$spell,$ac,$num_charges,$total_charges,$wand_lvl,$wand_spell,$wearfun\n";
		($namelist,$type,$weight,$cost,$lp,$damage,$avg,$affected_stat,$affected_val,$affected_stat1,$affected_val1,$affected_stat2,$affected_val2,$affected_stat3,$affected_val3,$affected_stat4,$affected_val4,$affected_stat5,$affected_val5,$affected_stat6,$affected_val6,$affected_stat7,$affected_val7,$special,$spell_lvl,$spell,$ac,$num_charges,$total_charges,$wand_lvl,$wand_spell,$wearfun)=map {"na"}(0..30);
		$affect=0;
		}
		$skip=0;
		$vnum = $1;
	}

	elsif($line =~ /No such object\./){
		($namelist,$type,$weight,$cost,$lp,$damage,$avg,$affected_stat,$affected_val,$affected_stat1,$affected_val1,$affected_stat2,$affected_val2,$affected_stat3,$affected_val3,$affected_stat4,$affected_val4,$affected_stat5,$affected_val5,$affected_stat6,$affected_val6,$affected_stat7,$affected_val7,$special,$spell_lvl,$spell,$ac,$num_charges,$total_charges,$wand_lvl,$wand_spell,$wearfun)=map {"na"}(0..30);
		#print "$vnum,$namelist,$type,$weight,$cost,$lp,$damage,$avg,$affected_stat,$affected_val,$special,$spell_lvl,$spell,$ac,$num_charges,$total_charges,$wand_lvl,$wand_spell,$wearfun\n";
		$affect=0;
		$skip=1;
		next;
	}

	elsif($line =~ /Namelist: (.*)\.\s{4}Type: (.*)\./){
		$namelist=$1;
		$type=$2;
		#print "$vnum,$1,$2";exit;
	}

	elsif($line =~ /Weight: (\d+),  Cost: (\d+)\.  Load Percent: (\d+)\%\./){
		$weight=$1;
		$cost=$2;
		$lp=$3;
		#print "$weight $cost $lp\n";exit;
	}
	
	elsif($line =~ /Damage is (.*) \(average (\d+)\)\./){
		$damage=$1;
		$avg=$2;
		#print "$damage $avg\n";exit;
	}

	elsif($line =~ /Affects (.*) by (.*)\./ && $affect == 0){
		$affected_stat=$1;
		$affected_val=$2;
		$affect++;
	}

#there could be multiple affected stats, this is ugly but it'll work for our purposes

	elsif($line =~ /Affects (.*) by (.*)\./ && $affect == 1){
		$affected_stat1=$1;
		$affected_val1=$2;
		$affect++;
	}

	elsif($line =~ /Affects (.*) by (.*)\./ && $affect == 2){
		$affected_stat2=$1;
		$affected_val2=$2;
		$affect++;
	}

	elsif($line =~ /Affects (.*) by (.*)\./ && $affect == 3){
		$affected_stat3=$1;
		$affected_val3=$2;
		$affect++;
	}

	elsif($line =~ /Affects (.*) by (.*)\./ && $affect == 4){
		$affected_stat4=$1;
		$affected_val4=$2;
		$affect++;
	}

	elsif($line =~ /Affects (.*) by (.*)\./ && $affect == 5){
		$affected_stat5=$1;
		$affected_val5=$2;
		$affect++;
		print "WARNING: $vnum found with 5 affected stats, anymore than this and they wont be taken into account, please double check.";
		exit;
	}

	elsif($line =~ /Object has special function\./){
		$special=1;
	}

	elsif($line =~ /Object has wear fun\./){
		$wearfun=1;
	}

	elsif($line =~ /Level (\d+) spells of: '(.*)'\.$/){
		$spell_lvl=$1;
		$spell=$2;
		$spell=~s/' '/;/g;
		$spell=~s/'//g;
	}

	elsif($line =~ /Level (\d+) spells of:(.*)\./){
#incomplete item 312
		$spell_lvl=$1;
		$spell=$2;
	}

	elsif($line =~ /Armor class is (-?\d+)\./){
		$ac=$1;
	}

	elsif($line =~ /Has (\d+)\((\d+)\) charges of level (\d+) '(.*)'\./){
		$num_charges=$1;
		$total_charges=$2;
		$wand_lvl=$3;
		$wand_spell=$4;
	}
	elsif($line =~ /Has (\d+)\((\d+)\) charges of level (\d+)\./){
#incomplete item 2349
		$num_charges=$1;
		$total_charges=$2;
		$wand_lvl=$3;
	}


#stuff to skip
	elsif($line =~ /Short description|Long description|Values:|^$|blood red flowers\.$|Fwiffo leaves in a swirling mist\.|the front\.\.\.|controling the weapon\.|equipment to use when fighting\.|combinations, has been tossed aside here\.|\*{4}|has arrived\.$|hits you\.$|You.*attack\.$|You.*hit\.$|is dead! R\.I\.P\.$|Your blood freezes as you hear.*death cry\.$|The.*leaves (north|south|east|west)\.$|The day has begun\.$|The sun rises in the east\.$|Corpse of.*is grabbed by a quivering hoard of pack rats\.$|Hithere leaves (north|south|east|west)\.$|Howler eats the sun\.\.it gets dark\.$|The night has begun\.$|Hithere has lost its link\.$|Hithere has left the game\.$|The sky is getting cloudy\.$|Corpse of Locutus decays into dust\.$|Rip gossips|brightly\.$|It starts to rain\.$|Lightning flashes in the sky\.$|The lightning has stopped\.$|The rain stopped\.$/){
		next;
	}

	else{
		print "i dont recognize this line: (for $vnum)\n";
		print $line;
		exit;
	}

}

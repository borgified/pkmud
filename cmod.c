#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "cmod.h"


struct class_info class_table[MAX_CLASS];


int class_lookup( char *argument )
{
  int iClass;

  for ( iClass = 0; iClass < MAX_CLASS ; iClass++ )
  {
    if (!strcmp( argument, class_table[iClass].who_name )
    || !strcmp( argument, class_table[iClass].class_name ) )
      return iClass;
  }

  return -1;
}

void do_cwrite( CHAR_DATA *ch, char *argument )
{
    FILE *fp;
    int iClass;
    int sn;

    if ( ch != NULL && IS_NPC( ch ) )
	return;

    if ( ( fp = fopen( "classtmp.txt", "w" ) ) == NULL )
    {
	send_to_char("Error opening class file.\n\r", ch );
	return;
    }

    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
	fprintf( fp, "Class '%s' %d\n", class_table[iClass].class_name, iClass );
	fprintf( fp, "Abbreviation '%s'\n", class_table[iClass].who_name );
	fprintf( fp, "Hits %d\n", class_table[iClass].hits );
	fprintf( fp, "Mana %d\n", class_table[iClass].mana );
	fprintf( fp, "Thac0 %d\n", class_table[iClass].thac0 );
	fprintf( fp, "Armor %d\n", class_table[iClass].base_armor );
	fprintf( fp, "Strength %d\n", class_table[iClass].strength );
	fprintf( fp, "Intelligence %d\n", class_table[iClass].intelligence );
	fprintf( fp, "Wisdom %d\n", class_table[iClass].wisdom );
	fprintf( fp, "Dexterity %d\n", class_table[iClass].dexterity );
	fprintf( fp, "Constitution %d\n", class_table[iClass].constitution );
	fprintf( fp, "HitRegen %d\n", class_table[iClass].regen_hit );
	fprintf( fp, "ManaRegen %d\n", class_table[iClass].regen_mana );
	fprintf( fp, "Rating %d\n", class_table[iClass].min_rating );
	fprintf( fp, "ClassRating %d\n", class_table[iClass].rating_12 );
	fprintf( fp, "ClassRating11 %d\n", class_table[iClass].rating_11 );
	fprintf( fp, "Random %d\n", class_table[iClass].random );
	fprintf( fp, "ResistFire %d\n", class_table[iClass].resist_fire );
	fprintf( fp, "ResistCold %d\n", class_table[iClass].resist_cold );
	fprintf( fp, "ResistLightning %d\n", class_table[iClass].resist_lightning );
	fprintf( fp, "ResistPhysical %d\n", class_table[iClass].resist_physical );
	fprintf( fp, "ResistMagic %d\n", class_table[iClass].resist_magic );
	fprintf( fp, "ResistDrain %d\n", class_table[iClass].resist_drain );
	fprintf( fp, "%s\n", (class_table[iClass].enabled?"ENABLED":"DISABLED") );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    if ( class_table[iClass].skill_percent[sn] == 0 )
		continue;
	    fprintf( fp, "Skill '%s' %d %d %d\n",
		skill_table[sn].name,
		class_table[iClass].skill_percent[sn],
		class_table[iClass].skill_cost[sn],
		class_table[iClass].skill_delay[sn] );
	}
	fprintf( fp, "Endclass\n");
    }

    fprintf( fp, "Endfile\n" );

    fclose( fp );
    system( "cp classtmp.txt classes.txt" );
    system( "rm classtmp.txt" );

    if ( ch != NULL )
      send_to_char("Class file written.\n\r", ch );

    return;
}


void read_classes( )
{
    FILE *fp;
    char *word;
    int iClass = -1;
    int sn;

    if ( ( fp = fopen( "classes.txt", "r" ) ) == NULL )
    {
	bug("Error opening class file.\n\r", 0 );
	exit( 1 );
    }

    while ( 1 )
    {
	word = fread_word( fp );

	if ( !str_cmp( word, "Class" ) )
	{
	    iClass++;
	    class_table[iClass].class_name = str_dup( fread_word( fp ) );
	    continue;
	}

	if ( !str_cmp( word, "Abbreviation" ) )
	    class_table[iClass].who_name = str_dup( fread_word( fp ) );

	if ( !str_cmp( word, "Hits" ) )
	    class_table[iClass].hits = fread_number( fp );

	if ( !str_cmp( word, "Mana" ) )
	    class_table[iClass].mana = fread_number( fp );

	if ( !str_cmp( word, "HitRegen" ) )
	    class_table[iClass].regen_hit = fread_number( fp );

	if ( !str_cmp( word, "ManaRegen" ) )
	    class_table[iClass].regen_mana = fread_number( fp );

	if ( !str_cmp( word, "Thac0" ) )
	    class_table[iClass].thac0 = fread_number( fp );

	if ( !str_cmp( word, "Armor" ) )
	    class_table[iClass].base_armor = fread_number( fp );

	if ( !str_cmp( word, "Strength" ) )
	    class_table[iClass].strength = fread_number( fp );

	if ( !str_cmp( word, "Intelligence" ) )
	    class_table[iClass].intelligence = fread_number( fp );

	if ( !str_cmp( word, "Wisdom" ) )
	    class_table[iClass].wisdom = fread_number( fp );

	if ( !str_cmp( word, "Dexterity" ) )
	    class_table[iClass].dexterity = fread_number( fp );

	if ( !str_cmp( word, "Constitution" ) )
	    class_table[iClass].constitution = fread_number( fp );

	if ( !str_cmp( word, "Rating" ) )
	    class_table[iClass].min_rating = fread_number( fp );

	if ( !str_cmp( word, "ClassRating" ) )
	    class_table[iClass].rating_12 = fread_number( fp );

	if ( !str_cmp( word, "ClassRating11" ) )
	    class_table[iClass].rating_11 = fread_number( fp );

	if ( !str_cmp( word, "ENABLED" ) )
	    class_table[iClass].enabled = TRUE;

	if ( !str_cmp( word, "DISABLED" ) )
	    class_table[iClass].enabled = FALSE;

	if ( !str_cmp( word, "Random" ) )
	    class_table[iClass].random = fread_number( fp );

	if ( !str_cmp( word, "ResistFire" ) )
	    class_table[iClass].resist_fire = fread_number( fp );

	if ( !str_cmp( word, "ResistCold" ) )
	    class_table[iClass].resist_cold = fread_number( fp );

	if ( !str_cmp( word, "ResistLightning" ) )
	    class_table[iClass].resist_lightning = fread_number( fp );

	if ( !str_cmp( word, "ResistPhysical" ) )
	    class_table[iClass].resist_physical = fread_number( fp );

	if ( !str_cmp( word, "ResistMagic" ) )
	    class_table[iClass].resist_magic = fread_number( fp );

	if ( !str_cmp( word, "ResistDrain" ) )
	    class_table[iClass].resist_drain = fread_number( fp );

	if ( !str_cmp( word, "Skill" ) )
	{
	    sn = skill_lookup( fread_word( fp ) );

	    class_table[iClass].skill_percent[sn] = fread_number( fp );
	    class_table[iClass].skill_cost[sn] = fread_number( fp );
	    class_table[iClass].skill_delay[sn] = fread_number( fp );

	    continue;
	}

	if ( !str_cmp( word, "Endclass" ) )
	    continue;

	if ( !str_cmp( word, "Endfile" ) )
	    break;
    }

    fclose( fp );

    return;
}

void do_cdisable( CHAR_DATA *ch, char *argument )
{
    int iclass;

    iclass = parse_class( argument );

    if ( iclass == -1 )
    {
	send_to_char( "Bad class.\n\r", ch );
	return;
    }

    send_to_char( "Class disabled.\n\r", ch );

    class_table[iclass].enabled = FALSE;
    return;
}


void do_cenable( CHAR_DATA *ch, char *argument )
{
    int iclass;

    iclass = parse_class( argument );

    if ( iclass == -1 )
    {
	send_to_char( "Bad class.\n\r", ch );
	return;
    }

    send_to_char( "Class enabled.\n\r", ch );

    class_table[iclass].enabled = TRUE;
    return;
}

void do_cmod( CHAR_DATA *ch, char *argument )
{
    int iclass;
    int inum;
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( argument[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Usage: cmod <class> <stat> <number>\n\r", ch );
	return;
    }

    if ( !is_number( argument ) )
    {
	send_to_char("The number must be a number.\n\r", ch );
	return;
    }

    inum = atoi( argument );

    iclass = parse_class( arg1 );

    if ( iclass == -1 )
    {
	send_to_char("Bad class.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "hits" ) )
    {
	send_to_char("Setting class hits.\n\r", ch );
	class_table[iclass].hits = inum;
	return;
    }

    if ( !str_cmp( arg2, "mana" ) )
    {
	send_to_char("Setting class mana.\n\r", ch );
	class_table[iclass].mana = inum;
	return;
    }

    if ( !str_cmp( arg2, "hregen" ) )
    {
        if ( inum < 0 )
	  inum = 0;
	send_to_char("Setting class hit regen.\n\r", ch );
	class_table[iclass].regen_hit = inum;
	return;
    }

    if ( !str_cmp( arg2, "mregen" ) )
    {
        if ( inum < 0 )
	  inum = 0;
	send_to_char("Setting class mana regen.\n\r", ch );
	class_table[iclass].regen_mana = inum;
	return;
    }

    if ( !str_cmp( arg2, "rating" ) )
    {
	send_to_char("Setting class minimum rating.\n\r", ch );
	class_table[iclass].min_rating = inum;
	return;
    }

    if ( !str_cmp( arg2, "random" ) )
    {
	send_to_char("Setting class randomness.\n\r", ch );
	class_table[iclass].random = inum;
	return;
    }

    if ( !str_cmp( arg2, "str" ) )
    {
	if ( inum < 5 || inum > 25 )
	{
	    send_to_char("Str should be between 5 and 25.\n\r", ch );
	    return;
	}
	send_to_char("Setting class str.\n\r", ch );
	class_table[iclass].strength = inum;
	return;
    }

    if ( !str_cmp( arg2, "int" ) )
    {
	if ( inum < 5 || inum > 20 )
	{
	    send_to_char("Int should be between 5 and 20.\n\r", ch );
	    return;
	}
	send_to_char("Setting class int.\n\r", ch );
	class_table[iclass].intelligence = inum;
	return;
    }

    if ( !str_cmp( arg2, "wis" ) )
    {
	if ( inum < 5 || inum > 20 )
	{
	    send_to_char("Wis should be between 5 and 20.\n\r", ch );
	    return;
	}
	send_to_char("Setting class wis.\n\r", ch );
	class_table[iclass].wisdom = inum;
	return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( inum < 5 || inum > 20 )
	{
	    send_to_char("Dex should be between 5 and 20.\n\r", ch );
	    return;
	}
	send_to_char("Setting class dex.\n\r", ch );
	class_table[iclass].dexterity = inum;
	return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
	if ( inum < 5 || inum > 20 )
	{
	    send_to_char("Con should be between 5 and 20.\n\r", ch );
	    return;
	}
	send_to_char("Setting class con.\n\r", ch );
	class_table[iclass].constitution = inum;
	return;
    }

    if ( !str_cmp( arg2, "thac0" ) )
    {
	if ( inum < 0 || inum > 20 )
	{
	    send_to_char("Thac0 should be between 0 and 20.\n\r", ch );
	    return;
	}
	send_to_char("Setting class thac0.\n\r", ch );
	class_table[iclass].thac0 = inum;
	return;
    }

    if ( !str_cmp( arg2, "armor" ) )
    {
	if ( inum < -200 || inum > 200 )
	{
	    send_to_char("Armor should be between -200 and 200.\n\r", ch );
	    return;
	}
	send_to_char("Setting class armor.\n\r", ch );
	class_table[iclass].base_armor = inum;
	return;
    }

    if ( !str_cmp( arg2, "rfire" ) )
    {
      send_to_char("Setting Fire Resist.\n\r", ch );
      class_table[iclass].resist_fire = inum;
      return;
    }

    if ( !str_cmp( arg2, "rcold" ) )
    {
      send_to_char("Setting Cold Resist.\n\r", ch );
      class_table[iclass].resist_cold = inum;
      return;
    }

    if ( !str_cmp( arg2, "rlightning" ) )
    {
      send_to_char("Setting Lightning Resist.\n\r", ch );
      class_table[iclass].resist_lightning = inum;
      return;
    }

    if ( !str_cmp( arg2, "rphysical" ) )
    {
      send_to_char("Setting Physical Resist.\n\r", ch );
      class_table[iclass].resist_physical = inum;
      return;
    }

    if ( !str_cmp( arg2, "rmagic" ) )
    {
      send_to_char("Setting Magic Resist.\n\r", ch );
      class_table[iclass].resist_magic = inum;
      return;
    }

    if ( !str_cmp( arg2, "rdrain" ) )
    {
      send_to_char("Setting Drain Resist.\n\r", ch );
      class_table[iclass].resist_drain = inum;
      return;
    }

/* generate error message */
    send_to_char("Bad stat.\n\r", ch );
    send_to_char("Valid arguments are:", ch );
    send_to_char("  hits mana thac0 armor rating hregen mregen.\n\r", ch );
    send_to_char("  str int wis dex con random.\n\r", ch );
    return;
}

void do_cskill( CHAR_DATA *ch, char *argument )
{
    int iclass;
    int sn;
    int ipercent;
    int icost;
    int idelay;
    char cClass[MAX_STRING_LENGTH];
    char cSkill[MAX_STRING_LENGTH];
    char cPercent[MAX_STRING_LENGTH];
    char cCost[MAX_STRING_LENGTH];

    argument = one_argument( argument, cClass );
    argument = one_argument( argument, cSkill );
    argument = one_argument( argument, cPercent );
    argument = one_argument( argument, cCost );

    if ( cClass[0] == '\0' || cSkill[0] == '\0'
	|| cPercent[0] == '\0' )
    {
	send_to_char("Usage: cskill <class> <skill> <percent> (cost) (delay)\n\r", ch );
	return;
    }

    iclass = parse_class( cClass );

    if ( iclass == -1 )
    {
	send_to_char("Bad class.\n\r", ch );
	return;
    }

    sn = skill_lookup( cSkill );

    if ( sn == -1 )
    {
	send_to_char("Bad skill.\n\r", ch );
	return;
    }

    if ( !is_number( cPercent ) )
    {
	send_to_char("Percent must be a number.\n\r", ch );
	return;
    }

    ipercent = atoi( cPercent );

    if ( skill_table[sn].spell_fun != spell_null && !is_number( cCost ) )
    {
	send_to_char("Spells must have mana cost.\n\r", ch );
	return;
    }

    if ( is_number( cCost ) )
	icost = atoi( cCost );
    else
	icost = 0;

    if ( is_number( argument ) )
	idelay = atoi( argument );
    else
	idelay = 0;

    class_table[iclass].skill_percent[sn] = ipercent;
    class_table[iclass].skill_cost[sn] = icost;
    class_table[iclass].skill_delay[sn] = idelay;

    send_to_char("Skill set.\n\r", ch );
    return;
}


void do_cview( CHAR_DATA *ch, char *argument )
{
  int iclass;
  int sn;
  int col = 0;
  char buf[MAX_STRING_LENGTH];

  iclass = parse_class( argument );

  if ( iclass == -1 )
  {
    send_to_char("What class is that?\n\r", ch );

    for ( iclass = 0; iclass < MAX_CLASS; iclass++ )
    {
      sprintf( buf, "%19s %4d %4d %8s  ",
	class_table[iclass].class_name,
	class_table[iclass].rating_11,
	class_table[iclass].rating_12,
	class_table[iclass].enabled ? "ENABLED" : "DISABLED" );
      send_to_char( buf, ch );

      if ( ++col % 2 == 0 )
      {
	send_to_char("\n\r", ch );
	col = 0;
      }
    }
    if ( col % 2 != 0 )
      send_to_char("\n\r", ch );
    return;
  }

  sprintf( buf, "Class: %s (%s)  %s   Random: %d%%   Min rating: %d\n\r",
    class_table[iclass].class_name,
    class_table[iclass].who_name,
    (class_table[iclass].enabled ? "ENABLED" : "DISABLED" ),
    class_table[iclass].random,
    class_table[iclass].min_rating );
  send_to_char( buf, ch );

  sprintf( buf, "Hits: %d, Mana: %d, Thac0: %d, AC: %d, Regen: Hit: %d  Mana: %d\n\r",
    class_table[iclass].hits,
    class_table[iclass].mana,
    class_table[iclass].thac0,
    class_table[iclass].base_armor,
    class_table[iclass].regen_hit,
    class_table[iclass].regen_mana );
  send_to_char( buf, ch );

  sprintf( buf, "Str: %d, Int: %d, Wis: %d, Dex: %d, Con: %d.\n\r",
    class_table[iclass].strength,
    class_table[iclass].intelligence,
    class_table[iclass].wisdom,
    class_table[iclass].dexterity,
    class_table[iclass].constitution );
  send_to_char( buf, ch );

  sprintf( buf, "Resists: Fire: %d, Cold: %d, Lightning: %d, Physical: %d, Magic: %d, Drain: %d.\n\r",
    class_table[iclass].resist_fire,
    class_table[iclass].resist_cold,
    class_table[iclass].resist_lightning,
    class_table[iclass].resist_physical,
    class_table[iclass].resist_magic,
    class_table[iclass].resist_drain );
  send_to_char( buf, ch );

  for ( sn = 0; sn < MAX_SKILL; sn++ )
  {
    if ( skill_table[sn].name == NULL )
      break;

    if ( class_table[iclass].skill_percent[sn] < 1 )
      continue;

    sprintf( buf, "%25s %3d %3d %3d",
      skill_table[sn].name,
      class_table[iclass].skill_percent[sn],
      class_table[iclass].skill_cost[sn],
      (class_table[iclass].skill_delay[sn] ?
 	class_table[iclass].skill_delay[sn] :
	skill_table[sn].beats ) );
    send_to_char( buf, ch );

    if ( ++col % 2  == 0 )
    {
      send_to_char( "\n\r", ch );
      col = 0;
    }
  }

  if ( col % 2 != 0 )
    send_to_char( "\n\r", ch );

  return;
}


int parse_class( char *argument )
{
    int ii;

    if ( argument == NULL )
      return -1;

    for ( ii = 0; ii < MAX_CLASS; ii++ )
    {
        if ( class_table[ii].who_name == NULL )
	    continue;

	if ( !str_cmp( argument, class_table[ii].who_name ) )
	    return ii;

	if ( !str_cmp( argument, class_table[ii].class_name ) )
	    return ii;
    }

    return -1;
}

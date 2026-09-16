/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "bfs.h"
#include "global.h"
#include "CharIterator.h"
#include "ObjIterator.h"



/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(	spec_archer_hunter	);
DECLARE_SPEC_FUN(	spec_breath_any		);
DECLARE_SPEC_FUN(	spec_breath_acid	);
DECLARE_SPEC_FUN(	spec_breath_fire	);
DECLARE_SPEC_FUN(	spec_breath_frost	);
DECLARE_SPEC_FUN(	spec_breath_gas		);
DECLARE_SPEC_FUN(	spec_breath_lightning	);
DECLARE_SPEC_FUN(	spec_cast_adept		);
DECLARE_SPEC_FUN(	spec_cast_cleric	);
DECLARE_SPEC_FUN(	spec_cast_judge		);
DECLARE_SPEC_FUN(	spec_cast_mage		);
DECLARE_SPEC_FUN(	spec_cast_undead	);
DECLARE_SPEC_FUN(	spec_distributor        );
DECLARE_SPEC_FUN(	spec_fido		);
DECLARE_SPEC_FUN(	spec_guard		);
DECLARE_SPEC_FUN(	spec_janitor		);
DECLARE_SPEC_FUN(	spec_logan              );
DECLARE_SPEC_FUN(	spec_mayor		);
DECLARE_SPEC_FUN(	spec_poison		);
DECLARE_SPEC_FUN(	spec_thief		);
DECLARE_SPEC_FUN(	spec_hunter_killer	);
DECLARE_SPEC_FUN(	spec_thief_hunter	);
DECLARE_SPEC_FUN(	spec_assassin_hunter	);
DECLARE_SPEC_FUN(	spec_dog_of_war		);
DECLARE_SPEC_FUN(	spec_scout		);
DECLARE_SPEC_FUN(	spec_super_wimpy	);
DECLARE_SPEC_FUN(	spec_waldo              );
DECLARE_SPEC_FUN(	spec_watchdog_force_quit);
DECLARE_SPEC_FUN(	spec_zombie             );
DECLARE_INTERACTIVE(	spec_invis_blocker	);



/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN *spec_lookup( const char *name )
{
    if ( !str_cmp( name, "spec_archer_hunter"	  ) ) return spec_archer_hunter;
    if ( !str_cmp( name, "spec_breath_any"	  ) ) return spec_breath_any;
    if ( !str_cmp( name, "spec_breath_acid"	  ) ) return spec_breath_acid;
    if ( !str_cmp( name, "spec_breath_fire"	  ) ) return spec_breath_fire;
    if ( !str_cmp( name, "spec_breath_frost"	  ) ) return spec_breath_frost;
    if ( !str_cmp( name, "spec_breath_gas"	  ) ) return spec_breath_gas;
    if ( !str_cmp( name, "spec_breath_lightning"  ) ) return
							spec_breath_lightning;
    if ( !str_cmp( name, "spec_cast_adept"	  ) ) return spec_cast_adept;
    if ( !str_cmp( name, "spec_cast_cleric"	  ) ) return spec_cast_cleric;
    if ( !str_cmp( name, "spec_cast_judge"	  ) ) return spec_cast_judge;
    if ( !str_cmp( name, "spec_cast_mage"	  ) ) return spec_cast_mage;
    if ( !str_cmp( name, "spec_cast_undead"	  ) ) return spec_cast_undead;
    if ( !str_cmp( name, "spec_distributor"	  ) ) return spec_distributor;
    if ( !str_cmp( name, "spec_fido"		  ) ) return spec_fido;
    if ( !str_cmp( name, "spec_guard"		  ) ) return spec_guard;
    if ( !str_cmp( name, "spec_janitor"		  ) ) return spec_janitor;
    if ( !str_cmp( name, "spec_logan"		  ) ) return spec_logan;
    if ( !str_cmp( name, "spec_mayor"		  ) ) return spec_mayor;
    if ( !str_cmp( name, "spec_poison"		  ) ) return spec_poison;
    if ( !str_cmp( name, "spec_thief"		  ) ) return spec_thief;
    if ( !str_cmp( name, "spec_hunter_killer"     ) ) return spec_hunter_killer;
    if ( !str_cmp( name, "spec_thief_hunter"      ) ) return spec_thief_hunter;
    if ( !str_cmp( name, "spec_assassin_hunter"   ) ) return spec_assassin_hunter;
    if ( !str_cmp( name, "spec_scout"             ) ) return spec_scout;
    if ( !str_cmp( name, "spec_super_wimpy"       ) ) return spec_super_wimpy;
    if ( !str_cmp( name, "spec_dog_of_war"        ) ) return spec_dog_of_war;
    if ( !str_cmp( name, "spec_waldo"             ) ) return spec_waldo;
    if ( !str_cmp( name, "spec_watchdog_force_quit" ) ) return spec_watchdog_force_quit;
    if ( !str_cmp( name, "spec_zombie"            ) ) return spec_zombie;
    return 0;
}

INTERACTIVE *interactive_lookup( const char *name )
{
    if ( !str_cmp( name, "spec_invis_blocker"     ) ) return spec_invis_blocker;
    return 0;
}

bool spec_invis_blocker( CHAR_DATA *mob, CHAR_DATA *ch, char *command, char *argument )
{
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch) || !IS_AFFECTED( ch, AFF_INVISIBLE ))
    return TRUE;

  if ( !mortal_in_main_war( ch ) )
    return TRUE;

  if (!str_prefix( command, "visible"))
    return TRUE;

  mob->data[0] += 3;

  if ( number_percent( ) < mob->data[0] )
  {
    extern void bash( CHAR_DATA *ch, CHAR_DATA *victim );

    sprintf( buf, "%s runs around invis too much.  Time to die.", ch->name );
    do_info( mob, buf );

    bash( mob, ch );

    return TRUE;
  }

  sprintf( buf, "%s spotted travelling invis at %s.",
    ch->name, ch->in_room->name );

  do_info( mob, buf );

  return TRUE;
}

bool spec_watchdog_force_quit( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CharIterator citer;

    for ( victim = citer.Start( ch, CharIterator::ROOM );
          victim != NULL; victim = citer.Next() )
    {
	if ( victim != ch && number_bits( 1 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    if (IS_NPC(victim))
	return FALSE;

    if (victim->level > 30 && victim->level < 39)
    {
	do_say( ch, "Get out of my room!" );
	do_quit( victim, victim->name );
	return TRUE;
    }

    return FALSE;
}



/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, char *spell_name )
{
    CHAR_DATA *victim;
    int sn;
    CharIterator citer;

/*
    if ( ch->position != POS_FIGHTING )
	return FALSE;
*/
    if (ch->fighting == NULL)
	return FALSE;

    if (ch->position != POS_STANDING)
    {
	if (number_bits(1))
		do_stand( ch, str_empty );
	return TRUE;
    }

    for ( victim = citer.Start( ch, CharIterator::ROOM );
          victim != NULL; victim = citer.Next() )

    {
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    if ( ( sn = skill_lookup( spell_name ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



/*
 * Special procedures for mobiles.
 */
bool spec_breath_any( CHAR_DATA *ch )
{
/*
    if ( ch->position != POS_FIGHTING )
	return FALSE;
*/
    if (ch->fighting == NULL)
	return FALSE;

    switch ( number_bits( 3 ) )
    {
    case 0: return spec_breath_fire		( ch );
    case 1:
    case 2: return spec_breath_lightning	( ch );
    case 3: return spec_breath_gas		( ch );
    case 4: return spec_breath_acid		( ch );
    case 5:
    case 6:
    case 7: return spec_breath_frost		( ch );
    }

    return FALSE;
}



bool spec_breath_acid( CHAR_DATA *ch )
{
    return dragon( ch, "acid breath" );
}



bool spec_breath_fire( CHAR_DATA *ch )
{
    return dragon( ch, "fire breath" );
}



bool spec_breath_frost( CHAR_DATA *ch )
{
    return dragon( ch, "frost breath" );
}



bool spec_breath_gas( CHAR_DATA *ch )
{
    int sn;

/*
    if ( ch->position != POS_FIGHTING )
	return FALSE;
*/
    if ( ch->fighting == NULL )
	return FALSE;

    if ( ch->position != POS_STANDING )
    {
	if (number_bits(1))
		do_stand( ch, str_empty );
	return TRUE;
    }

    if ( ( sn = skill_lookup( "gas breath" ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, NULL );
    return TRUE;
}



bool spec_breath_lightning( CHAR_DATA *ch )
{
    return dragon( ch, "lightning breath" );
}


bool spec_cast_adept( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    extern int war_status;
    extern int war_timer;
    CharIterator citer;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    if ( war_status != WAR_ON || war_timer > 10 )
	return FALSE;

    for ( victim = citer.Start( ch, CharIterator::ROOM );
          victim != NULL; victim = citer.Next() )
    {
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    switch ( number_bits( 3 ) )
    {
    case 0:
	act( "$n utters the word 'tehctah'.", ch, NULL, NULL, TO_ROOM, TRUE );
	spell_armor( skill_lookup( "armor" ), ch->level, ch, victim );
	return TRUE;

    case 1:
	act( "$n utters the word 'nhak'.", ch, NULL, NULL, TO_ROOM, TRUE );
	spell_bless( skill_lookup( "bless" ), ch->level, ch, victim );
	return TRUE;

    case 2:
	act( "$n utters the word 'yeruf'.", ch, NULL, NULL, TO_ROOM, TRUE );
	spell_cure_blindness( skill_lookup( "cure blindness" ),
	    ch->level, ch, victim );
	return TRUE;

    case 3:
	act( "$n utters the word 'garf'.", ch, NULL, NULL, TO_ROOM, TRUE );
	spell_cure_light( skill_lookup( "cure light" ),
	    ch->level, ch, victim );
	return TRUE;

    case 4:
	act( "$n utters the words 'rozar'.", ch, NULL, NULL, TO_ROOM, TRUE );
	spell_cure_poison( skill_lookup( "cure poison" ),
	    ch->level, ch, victim );
	return TRUE;

    case 5:
	act( "$n utters the words 'hctib'.", ch, NULL, NULL, TO_ROOM, TRUE );
	spell_curse( skill_lookup( "curse" ),
	    ch->level, ch, victim );
	return TRUE;

    case 6:
	act( "$n utters the words 'ekun'.", ch, NULL, NULL, TO_ROOM, TRUE );
	spell_dispel_magic( skill_lookup( "dispel magic" ),
	    ch->level, ch, victim );
	return TRUE;

    case 7:
	act( "$n utters the words 'remood'.", ch, NULL, NULL, TO_ROOM, TRUE );
	spell_blindness( skill_lookup( "blindness" ),
	    ch->level, ch, victim );
	return TRUE;

    }
    return FALSE;
}



bool spec_cast_cleric( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char *spell;
    int sn;
    CharIterator citer;

    if (ch->fighting == NULL)
	return FALSE;

    if (ch->position != POS_STANDING)
    {
	if (number_bits(1))
		do_stand( ch, str_empty );
	return TRUE;
    }

    for ( victim = citer.Start( ch, CharIterator::ROOM );
          victim != NULL; victim = citer.Next() )
    {
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level = 10; spell = "blindness";      break;
	case  1: min_level =  0; spell = "cause light";  break;
	case  2: min_level =  7; spell = "earthquake";     break;
	case  3: min_level =  9; spell = "cause critical"; break;
	case  4: min_level = 10; spell = "dispel evil";    break;
	case  5: min_level = 12; spell = "curse";          break;
	case  6: min_level = 12; spell = "change sex";     break;
	case  7:
	case  8:
	case  9: min_level =  3; spell = "flamestrike";    break;
	case 10: min_level = 15; spell = "harm";           break;
	default: min_level = 16; spell = "dispel magic";   break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_judge( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char *spell;
    int sn;
    CharIterator citer;

    if (ch->fighting == NULL)
	return FALSE;

    if (ch->position != POS_STANDING)
    {
	if (number_bits(1))
		do_stand(ch, str_empty );
	return TRUE;
    }

    for ( victim = citer.Start( ch, CharIterator::ROOM );
          victim != NULL; victim = citer.Next() )
    {
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    spell = "high explosive";
    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_mage( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char *spell;
    int sn;
    CharIterator citer;

    if (ch->fighting == NULL)
	return FALSE;

    if (ch->position != POS_STANDING)
    {
	if (number_bits(1))
		do_stand( ch, str_empty );
	return TRUE;
    }

    for ( victim = citer.Start( ch, CharIterator::ROOM );
          victim != NULL; victim = citer.Next() )
    {
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level = 10; spell = "blindness";      break;
	case  1: min_level =  3; spell = "chill touch";    break;
	case  2: min_level =  7; spell = "weaken";         break;
	case  4: min_level = 11; spell = "colour spray";   break;
	case  5: min_level = 12; spell = "change sex";     break;
	case  6: min_level = 13; spell = "vampiric touch";   break;
	case  7: min_level = 0; spell = "magic missile";  break;
	case  3: min_level =  3; spell = "teleport";       break;
	case  8:
	case  9: min_level = 15; spell = "fireball";       break;
	default: min_level = 20; spell = "ice storm";     break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_undead( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char *spell;
    int sn;
    CharIterator citer;

    if (ch->fighting == NULL)
	return FALSE;

    if (ch->position != POS_STANDING)
    {
	if (number_bits(1))
		do_stand( ch, str_empty );
	return TRUE;
    }

    for ( victim = citer.Start( ch, CharIterator::ROOM );
          victim != NULL; victim = citer.Next() )
    {
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "curse";          break;
	case  1: min_level =  3; spell = "weaken";         break;
	case  2: min_level =  6; spell = "chill touch";    break;
	case  3: min_level =  9; spell = "blindness";      break;
	case  4: min_level = 12; spell = "poison";         break;
	case  5: min_level = 15; spell = "vampiric touch";   break;
	case  6: min_level = 18; spell = "harm";           break;
	case  7: min_level = 21; spell = "teleport";       break;
	default: min_level = 24; spell = "gate";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_fido( CHAR_DATA *ch )
{
    OBJ_DATA *corpse;
    OBJ_DATA *c_next;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    ObjIterator oiter;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( corpse = oiter.StartList( ch, ch->in_room->contents );
          corpse != NULL; corpse = oiter.Next( ) )
    {
	if ( corpse->item_type != ITEM_CORPSE_NPC )
	    continue;

	act( "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM, TRUE );

        /* Using full un-phased list to allow incorrectly phased
         * items to drop from the corpse. */
	for ( obj = corpse->contains; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    obj_from_obj( obj );
	    obj_to_room( obj, ch->in_room );
	}
	extract_obj( corpse, "spec_fido" );
	return TRUE;
    }

    return FALSE;
}



bool spec_guard( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *ech;
    char *crime;
    int max_evil;
    CharIterator citer;

    if ( !IS_AWAKE(ch) || ch->fighting != NULL )
	return FALSE;

    max_evil = 300;
    ech      = NULL;
    crime    = "";

    for ( victim = citer.Start( ch, CharIterator::ROOM );
          victim != NULL; victim = citer.Next() )
    {
	if ( victim->fighting != NULL
	&&   victim->fighting != ch )
	{
	    if ( victim->fighting->char_class == CLASS_ARCHER )
		continue;
	    if ( victim->alignment < max_evil )
	    {
		max_evil = victim->alignment;
		ech      = victim;
	    }
	}
    }

    if ( victim != NULL )
    {
	sprintf( buf, "%s is a %s!  PROTECT THE INNOCENT!!  BANZAI!!",
	    victim->name, crime );
	do_shout( ch, buf );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return TRUE;
    }

    if ( ech != NULL )
    {
	act( "$n screams 'PROTECT THE INNOCENT!!  BANZAI!!",
	    ch, NULL, NULL, TO_ROOM, TRUE );
	multi_hit( ch, ech, TYPE_UNDEFINED );
	return TRUE;
    }

    return FALSE;
}



bool spec_janitor( CHAR_DATA *ch )
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;
    ObjIterator oiter;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( trash = oiter.StartList( ch, ch->in_room->contents );
          trash != NULL; trash = oiter.Next( ) )
    {
	if ( !IS_SET( trash->wear_flags, ITEM_TAKE ) )
	    continue;
	if ( trash->item_type == ITEM_DRINK_CON
	||   trash->item_type == ITEM_TRASH
	||   trash->cost < 10 )
	{
	    act( "$n picks up some trash.", ch, NULL, NULL, TO_ROOM, TRUE );
	    obj_from_room( trash, "spec_janitor" );
	    obj_to_char( trash, ch );
	    return TRUE;
	}
    }

    return FALSE;
}



bool spec_mayor( CHAR_DATA *ch )
{
    static const char open_path[] =
	"W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";

    static const char close_path[] =
	"W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

    static const char *path;
    static int pos;
    static bool move;

    if ( !move )
    {
	if ( time_info.hour ==  6 )
	{
	    path = open_path;
	    move = TRUE;
	    pos  = 0;
	}

	if ( time_info.hour == 20 )
	{
	    path = close_path;
	    move = TRUE;
	    pos  = 0;
	}
    }

    if ( ch->fighting != NULL )
	return spec_cast_cleric( ch );
    if ( !move || ch->position < POS_SLEEPING )
	return FALSE;

    switch ( path[pos] )
    {
    case '0':
    case '1':
    case '2':
    case '3':
	move_char( ch, path[pos] - '0', TRUE );
	break;

    case 'W':
	ch->position = POS_STANDING;
	act( "$n awakens and groans loudly.", ch, NULL, NULL, TO_ROOM, TRUE );
	break;

    case 'S':
	ch->position = POS_SLEEPING;
	act( "$n lies down and falls asleep.", ch, NULL, NULL, TO_ROOM, TRUE );
	break;

    case 'a':
	act( "$n says 'Get to work!  I want those death lists by noon!'",
            ch, NULL, NULL, TO_ROOM, TRUE );
	break;

    case 'b':
	act( "$n says 'What a smell!  Stinking corpses rotting all over!'",
	    ch, NULL, NULL, TO_ROOM, TRUE );
	break;

    case 'c':
	act( "$n says 'Vandals!  You youngsters go kill something!'",
	    ch, NULL, NULL, TO_ROOM, TRUE );
	break;

    case 'd':
	act( "$n says 'Ahhh!  I love the smell of blood in the air!'", 
            ch, NULL, NULL, TO_ROOM, TRUE );
	break;

    case 'e':
	act( "$n says 'Hell is now open!  Let the suffering begin!'",
	    ch, NULL, NULL, TO_ROOM, TRUE );
	break;

    case 'E':
	act( "$n says 'Hell is now closed!  Remain here and suffer!'",
	    ch, NULL, NULL, TO_ROOM, TRUE );
	break;

    case 'O':
	do_unlock( ch, "gate" );
	do_open( ch, "gate" );
	break;

    case 'C':
	do_close( ch, "gate" );
	do_lock( ch, "gate" );
	break;

    case '.' :
	move = FALSE;
	break;
    }

    pos++;
    return FALSE;
}



bool spec_poison( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
/*
    if ( ch->position != POS_FIGHTING
*/
    if (ch->fighting == NULL
    || ( victim = ch->fighting ) == NULL
    ||   number_percent( ) > 2 * ch->level )
	return FALSE;

    if (ch->position != POS_STANDING)
    {
	if (number_bits(1))
		do_stand( ch, str_empty );
	return TRUE;
    }

    act( "You bite $N!",  ch, NULL, victim, TO_CHAR, TRUE    );
    act( "$n bites $N!",  ch, NULL, victim, TO_NOTVICT, TRUE );
    act( "$n bites you!", ch, NULL, victim, TO_VICT, TRUE    );
    spell_poison( gsn_poison, ch->level, ch, victim );
    return TRUE;
}



bool spec_thief( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    int gold;
    CharIterator citer;

    if ( ch->position != POS_STANDING )
	return FALSE;

    for ( victim = citer.Start( ch, CharIterator::ROOM );
          victim != NULL; victim = citer.Next() )
    {
	if ( IS_NPC(victim)
	||   victim->level >= LEVEL_IMMORTAL
	||   number_bits( 2 ) != 0
	||   !can_see( ch, victim ) )	/* Thx Glop */
	    continue;

	if ( IS_AWAKE(victim) && number_range( 0, ch->level ) == 0 )
	{
	    act( "You discover $n's hands in your wallet!",
		ch, NULL, victim, TO_VICT, TRUE );
	    act( "$N discovers $n's hands in $S wallet!",
		ch, NULL, victim, TO_NOTVICT, TRUE );
	    return TRUE;
	}
	else
	{
	    gold = victim->gold * number_range( 1, 20 ) / 100;
	    ch->gold     += 7 * gold / 8;
	    victim->gold -= gold;
	    return TRUE;
	}
    }

    return FALSE;
}



bool spec_hunter_killer( CHAR_DATA *mob )
{
  OBJ_DATA *wield;
  char buf[MAX_STRING_LENGTH];
  int direction;

  if (mob->wait != 0)
    return FALSE;

  if (mob->hunting == NULL)
    return FALSE;

  if ( !in_main_war( mob->hunting ) )
    return FALSE;

  if (mob->position != POS_STANDING)
  {
    if (!number_bits(2))
      do_stand( mob, str_empty );
    return FALSE;
  }

  if ( (wield = get_eq_char( mob, WEAR_WIELD )) == NULL 
  && (wield = get_obj_carry( mob, "sword" )) != NULL )
  {
    equip_char( mob, wield, WEAR_WIELD );
    do_say( mob, "I hate when I drop my weapon!" );
    act("$n picks up his gun.", mob, NULL, NULL, TO_ROOM, TRUE);
    return FALSE;
  }

  switch( mob->level )
  {
    case 40: case 39: case 38: case 37: case 36:
    case 35: case 34: case 33: case 32: case 31:
    case 30:
    case 29: case 28: case 27: case 26:
    case 25: case 24: case 23: case 22: case 21:

      if (!IS_AFFECTED( mob, AFF_SANCTUARY ) )
      {
	do_say( mob, "I like to glow!" );
	spell_sanctuary( gsn_sanct, mob->level, mob, mob );
	return FALSE;
      }

    case 20: case 19: case 18: case 17: case 16:
    case 15: case 14: case 13: case 12: case 11:
      if (!IS_AFFECTED( mob, AFF_PASS_DOOR ) )
      {
	act("$n mutters some words about doors annoying him.",
	    mob, NULL, NULL, TO_ROOM, TRUE);
	spell_pass_door( gsn_pass_door, mob->level, mob, mob );
	return FALSE;
      }

    case 10: case 9: case 8: case 7: case 6:
    case 5:

      if (!IS_AFFECTED( mob, AFF_DETECT_INVIS ) )
      {
	do_say( mob, "I can see invisofreaks!" );
	spell_detect_invis( gsn_detect_invis, mob->level, mob, mob );
	return FALSE;
      }

      if (!IS_AFFECTED( mob, AFF_DETECT_HIDDEN ) )
      {
	do_say( mob, "I can see hidden freaks too!" );
	spell_detect_hidden( gsn_detect_hidden, mob->level, mob, mob );
	return FALSE;
      }
    case 4: case 3: case 2: case 1: case 0:
      default:
      break;
  }

  if (mob->fighting != NULL
  && mob->fighting != mob->hunting )
  {
    do_say( mob, "I'm not after you!" );
    act("$n points at $N.", mob, NULL, mob->fighting, TO_NOTVICT, TRUE );
    act("$n points at you.", mob, NULL, mob->fighting, TO_VICT, TRUE );
    act("$n makes some strange gesture and combat ceases.",
        mob, NULL, NULL, TO_ROOM, TRUE );
    do_peace( mob, str_empty);
    return FALSE;
  }

  if (mob->in_room == mob->hunting->in_room)
  {
    if (mob->fighting == NULL)
    {
      if (!can_see( mob, mob->hunting ))
      {
        if (number_bits(1))
	{
	  if ( mob->level >= 25 )
	  {
	    act("$n utters the words, 'pan mort'.",
	      mob, NULL, NULL, TO_ROOM, TRUE );
	    spell_death_spray( gsn_death_spray, mob->level, mob, mob );
	  }
	  else
	  {
	    act("$n utters the words, 'di zhen'.",
	      mob, NULL, NULL, TO_ROOM, TRUE );
	    spell_earthquake( gsn_earthquake, mob->level, mob, mob );
	  }
	}

	return FALSE;
      }
      else
      {
	switch(number_bits(4))
	{
	  case 1:  case 2:
            if ( mob->level >= 25 )
	    {
	      act("$n utters the words, 'NoMoRegen'.",
	      	mob, NULL, NULL, TO_ROOM, TRUE );
	      spell_blackmantle( gsn_blackmantle, mob->level, mob,
	        mob->hunting );
	    }
	    break;
	  case 3:  case 4:  case 5:  case 6:
	    sprintf( buf, "%s", mob->hunting->name );
	    do_backstab( mob, buf );
	    break;
	  case 7:  case 8:
	    if ( mob->level >= 12 )
	    {
	      act("$n utters the words, 'Ooom! YouGlowRed'.",
	      	mob, NULL, NULL, TO_ROOM, TRUE );
	      spell_curse( gsn_curse, mob->level, mob, mob->hunting );
	    }
	    break;
	  case 9:  case 10:
	    sprintf( buf, "%s", mob->hunting->name );
	    do_bash( mob, buf );
	    break;
	  default:
	    break;
	}

	return FALSE;
      }
    }
/* otherwise, we are already in combat */

	switch (number_bits(3))
	{
	    case 1:  case 2:  case 3:
		do_bash( mob, str_empty );
		break;
	    case 4:
		spell_high_explosive( skill_lookup( "high explosive" ), mob->level, mob, mob->fighting );
		break;
	    case 5:
		if ( mob->level >= 20 )
		    spell_confusion( skill_lookup( "confusion" ), mob->level, mob, mob->fighting );
		break;
	    default:
		break;
	}

	return FALSE;

    }

/* ok, we are not in the room with him... lets find em */

    direction = bfs_find_room( mob, mob->in_room, mob->hunting->in_room, 1000 );

    if (direction < 0 || direction > 5)
    {
	sprintf(buf, "summon %s", mob->hunting == NULL ? "" : mob->hunting->name );
	do_cast( mob, buf );
	return FALSE;
    }

    move_char( mob, direction, TRUE );
    return FALSE;
}


bool spec_dog_of_war( CHAR_DATA *mob )
{
    char buf[MAX_STRING_LENGTH];
    int direction;

    if (mob->wait != 0)
	return FALSE;

    if (mob->hunting == NULL)
	return FALSE;

    if ( !in_main_war( mob->hunting ) )
	return FALSE;

    if (mob->position != POS_STANDING)
    {
	if (!number_bits(2))
	    do_stand( mob, str_empty );
	return FALSE;
    }

    switch( mob->level )
    {
	case 40: case 39: case 38: case 37: case 36:
	case 35: case 34: case 33: case 32: case 31:
	case 30:
	case 29: case 28: case 27: case 26:
	case 25: case 24: case 23: case 22: case 21:
	case 20: case 19: case 18: case 17: case 16:
	case 15:
	    if (!IS_AFFECTED( mob, AFF_PASS_DOOR ) )
	    {
		act("$n growls and begins to shine.", mob, NULL, NULL, TO_ROOM, TRUE);
		spell_pass_door( gsn_pass_door, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }

	case 14: case 13: case 12: case 11:
	case 10: case 9: case 8: case 7: case 6:
	case 5:

	    if (!IS_AFFECTED( mob, AFF_DETECT_INVIS ) )
	    {
		act("$n barks loudly, fire growing in $s eyes.", mob, NULL, NULL, TO_ROOM, TRUE );
		spell_detect_invis( gsn_detect_invis, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }

	    if (!IS_AFFECTED( mob, AFF_DETECT_HIDDEN ) )
	    {
		act("$n glares at the sky, $s eyes shining.", mob, NULL, NULL, TO_ROOM, TRUE );
		spell_detect_hidden( gsn_detect_hidden, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }
	case 4: case 3: case 2: case 1: case 0:
	default:
          break;
    }

    if (mob->fighting != NULL
    && mob->fighting != mob->hunting )
    {
	act("$n growls and shakes his teeth.", mob, NULL, NULL, TO_ROOM, TRUE );
	act("$n snaps at $N.", mob, NULL, mob->fighting, TO_NOTVICT, TRUE );
	act("$n snaps at you.", mob, NULL, mob->fighting, TO_VICT, TRUE );
	act("$n emits a howl such that everyone covers their ears and stops fighting.", mob, NULL, NULL, TO_ROOM, TRUE );
	do_peace( mob, str_empty );
	return FALSE;
    }

    if (mob->in_room == mob->hunting->in_room)
    {
	if (mob->fighting == NULL)
	{
	   if (!can_see( mob, mob->hunting ))
	   {
		if (number_bits(1))
		{
		    act("$n jumps into the air and lands with a thundering crash.", mob, NULL, NULL, TO_ROOM, TRUE );
		    spell_earthquake( gsn_earthquake, mob->level, mob, mob );
		}
		WAIT_STATE( mob, 12 );
		return FALSE;
	   }
	   else
	   {
		switch(number_bits(4))
		{
		    case 1:  case 2:
			if ( mob->level >= 25 )
			{
			    act("$n spits black acid at $N.", mob, NULL, mob->hunting, TO_ROOM, TRUE );
			    spell_blackmantle( gsn_blackmantle, mob->level, mob, mob->hunting );
			    WAIT_STATE( mob, 12 );
			}
			break;
		    case 3:  case 4:  case 5:  case 6:
			act("$n opens his mouth and spews forth a tongue of flame!", mob, NULL, NULL, TO_ROOM, TRUE );
			spell_fire_breath( gsn_fire_breath, mob->level, mob, mob->hunting );
			WAIT_STATE( mob, 12 );
			break;
		    case 7:  case 8:
			if ( mob->level >= 12 )
			{
			   act("$n glares at you, a red light shining in $s eyes.", mob, NULL, NULL, TO_ROOM, TRUE );
			    spell_curse( gsn_curse, mob->level, mob, mob->hunting );
			    WAIT_STATE( mob, 12 );
			}
			break;
		    case 9:  case 10:
			if ( mob->level >= 17 )
			{
			    act("$n lifts a paw, and calls forth a ball of fire!", mob, NULL, NULL, TO_ROOM, TRUE );
			    spell_fireball( gsn_fireball, mob->level, mob, mob->hunting );
			    WAIT_STATE( mob, 12 );
			}
			break;
		    case 11:
			do_bash( mob, mob->hunting->name );
			break;
		    default:
			break;
		}

		return FALSE;
	    }
	}
/* otherwise, we are already in combat */

	switch (number_bits(3))
	{
	    case 1:  case 2:  case 3: case 4:
		act("$n opens his mouth and spews forth a tongue of flame!", mob, NULL, NULL, TO_ROOM, TRUE );
		spell_fire_breath( gsn_fire_breath, mob->level, mob, mob->fighting );
		WAIT_STATE( mob, 12 );
		break;
	    case 5: case 6:
		if ( mob->level >= 8 )
		{
		    act("$n swipes a flaming paw at $N!", mob, NULL, mob->fighting, TO_NOTVICT, TRUE );
		    act("$n swipes a flaming paw at you!", mob, NULL, mob->fighting, TO_VICT, TRUE );
		    spell_burning_hands( gsn_burning_hands, mob->level, mob, mob->fighting );
		    WAIT_STATE( mob, 12 );
		}
		break;
	    case 7:
		if ( mob->level >= 17 )
		{
		    act("$n lifts a paw, and calls forth a ball of fire!", mob, NULL, NULL, TO_ROOM, TRUE );
		    spell_fireball( gsn_fireball, mob->level, mob, mob->hunting );
		}
		break;
	    default:
		break;
	}

	return FALSE;

    }

/* ok, we are not in the room with him... lets find em */

    direction = bfs_find_room( mob, mob->in_room, mob->hunting->in_room, 1000 );

    if (direction < 0 || direction > 5)
    {
	sprintf(buf, "summon %s", mob->hunting == NULL ? "" : mob->hunting->name );
	do_cast( mob, buf );
	return FALSE;
    }

    move_char( mob, direction, TRUE );
    return FALSE;
}


bool spec_thief_hunter( CHAR_DATA *mob )
{
    OBJ_DATA *wield;
    char buf[MAX_STRING_LENGTH];
    int direction;
    ObjIterator oiter;

    if (mob->wait != 0)
	return FALSE;

    if (mob->hunting == NULL)
	return FALSE;

    if ( !in_main_war( mob->hunting ) )
	return FALSE;

    if (mob->position != POS_STANDING)
    {
	if (!number_bits(2))
	    do_stand( mob, str_empty );
	return FALSE;
    }

    if ( (wield = get_eq_char( mob, WEAR_WIELD )) == NULL  )
    {
	if ( (wield = get_obj_carry( mob, "dirk" )) == NULL )
	{
	    do_say( mob, "Damn.  Lost my weapon.  Good thing I've got another." );
	    act("$n draws a Thieves Dirk from his boot.", mob, NULL, NULL, TO_ROOM, TRUE );
            wield = create_object( get_obj_index(4053), mob->phase );  /* thieve's dirk */
            obj_to_char( wield, mob );
	}
	else
	    do_say( mob, "I hate when I drop my blade!" );
	equip_char( mob, wield, WEAR_WIELD );
	act("$n wields his dirk.", mob, NULL, NULL, TO_ROOM, TRUE);
	WAIT_STATE( mob, 8 );
	return FALSE;
    }

    switch( mob->level )
    {
	case 40: case 39: case 38: case 37: case 36:
	case 35: case 34: case 33: case 32: case 31:
	case 30:
	case 29: case 28: case 27: case 26:
	case 25: case 24: case 23: case 22: case 21:
	case 20: case 19: case 18: case 17: case 16:
	case 15:
	    if (!IS_AFFECTED( mob, AFF_PASS_DOOR ) )
	    {
		act("$n quaffs a virtual swirling potion.", mob, NULL, NULL, TO_ROOM, TRUE);
		spell_pass_door( gsn_pass_door, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }

	case 14: case 13: case 12: case 11:
	case 10: case 9: case 8: case 7: case 6:
	case 5:

	    if (!IS_AFFECTED( mob, AFF_DETECT_INVIS ) )
	    {
		act("$n quaffs a virtual yellow potion.", mob, NULL, NULL, TO_ROOM, TRUE );
		spell_detect_invis( gsn_detect_invis, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }

	    if (!IS_AFFECTED( mob, AFF_DETECT_HIDDEN ) )
	    {
		act("$n quaffs a virtual orange potion.", mob, NULL, NULL, TO_ROOM, TRUE );
		spell_detect_hidden( gsn_detect_hidden, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }
	case 4: case 3: case 2: case 1: case 0:
	default:
          break;
    }

    if (mob->fighting != NULL
    && mob->fighting != mob->hunting )
    {
	do_say( mob, "What the hell are you trying to do?" );
	act("$n points at $N.", mob, NULL, mob->fighting, TO_NOTVICT, TRUE );
	act("$n points at you.", mob, NULL, mob->fighting, TO_VICT, TRUE );
	act("$n gestures and fighting ceases.", mob, NULL, NULL, TO_ROOM, TRUE );
	do_peace( mob, str_empty );
	return FALSE;
    }

    if (mob->in_room == mob->hunting->in_room)
    {
	if (mob->fighting == NULL)
	{
	   if (!can_see( mob, mob->hunting ))
	   {
		do_say( mob, "Damned invisos.. I'll just stab yer ass anyways." );
		multi_hit( mob, mob->hunting, TYPE_UNDEFINED );
		WAIT_STATE( mob, 12 );
		return FALSE;
	   }
	   else
	   {
		OBJ_DATA *osteal;

		switch(number_bits(4))
		{
		    case 1:  case 2:
                        for ( osteal = oiter.StartList( mob, mob->hunting->carrying );
			      osteal != NULL; osteal = oiter.Next( ) )
			{
			    if ( osteal->item_type == ITEM_DRINK_CON
			    || osteal->item_type == ITEM_CONTAINER
			    || osteal->item_type == ITEM_POTION
			    || osteal->item_type == ITEM_PILL )
				break;
			}
			if ( osteal == NULL )
			{
			    WAIT_STATE( mob, 12 );
			    return FALSE;
			}
			attempt_steal_object( mob, mob->hunting, osteal );
			return FALSE;
			break;
		    case 3:  case 4:
		    case 5:  case 6:
		    case 7:  case 8:
		    case 9:  case 10:
			if ( mob->hunting->fighting != NULL )
			    multi_hit( mob, mob->hunting, TYPE_UNDEFINED );
			else
			    do_backstab( mob, mob->hunting->name );
			break;
		    default:
			break;
		}

		return FALSE;
	    }
	}
/* otherwise, we are already in combat */

	switch (number_bits(3))
	{
	    case 1:
		do_disarm( mob, str_empty );
		break;
	    case 2: case 3: case 4: case 5: case 6: case 7:
		do_flee( mob, str_empty );
		break;
	    default:
		break;
	}

	return FALSE;

    }

/* ok, we are not in the room with him... lets find em */

    direction = bfs_find_room( mob, mob->in_room, mob->hunting->in_room, 1000 );

    if (direction < 0 || direction > 5)
    {
	sprintf(buf, "summon %s", mob->hunting == NULL ? "" : mob->hunting->name );
	do_cast( mob, buf );
	return FALSE;
    }

    move_char( mob, direction, TRUE );
    return FALSE;
}


bool spec_assassin_hunter( CHAR_DATA *mob )
{
    OBJ_DATA *wield;
    char buf[MAX_STRING_LENGTH];
    int direction;

    if (mob->wait != 0)
	return FALSE;

    if (mob->hunting == NULL)
	return FALSE;

    if ( !in_main_war( mob->hunting ) )
	return FALSE;

    if (mob->position != POS_STANDING)
    {
	if (!number_bits(2))
	    do_stand( mob, str_empty );
	return FALSE;
    }

    if ( (wield = get_eq_char( mob, WEAR_WIELD )) == NULL  )
    {
	if ( (wield = get_obj_carry( mob, "shadow" )) == NULL )
	{
	    act("$n draws a shadow knife from his boot.", mob, NULL, NULL, TO_ROOM, TRUE );
            wield = create_object( get_obj_index(9008), mob->phase );  /* thieve's dirk */
            obj_to_char( wield, mob );
	}
	else
	    do_say( mob, "Blade's never there when I need it." );
	equip_char( mob, wield, WEAR_WIELD );
	act("$n wields his knife.", mob, NULL, NULL, TO_ROOM, TRUE);
	WAIT_STATE( mob, 8 );
	return FALSE;
    }

    switch( mob->level )
    {
	case 40: case 39: case 38: case 37: case 36:
	case 35: case 34: case 33: case 32: case 31:
	case 30:
	case 29: case 28: case 27: case 26:
	case 25: case 24: case 23: case 22: case 21:
	case 20: case 19: case 18: case 17: case 16:
	case 15:
	    if (!IS_AFFECTED( mob, AFF_PASS_DOOR ) )
	    {
		act("$n quaffs a virtual swirling potion.", mob, NULL, NULL, TO_ROOM, TRUE);
		spell_pass_door( gsn_pass_door, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }

	case 14: case 13: case 12: case 11:
	case 10: case 9: case 8: case 7: case 6:
	case 5:

	    if (!IS_AFFECTED( mob, AFF_DETECT_INVIS ) )
	    {
		act("$n quaffs a virtual yellow potion.", mob, NULL, NULL, TO_ROOM, TRUE );
		spell_detect_invis( gsn_detect_invis, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }

	    if (!IS_AFFECTED( mob, AFF_DETECT_HIDDEN ) )
	    {
		act("$n quaffs a virtual orange potion.", mob, NULL, NULL, TO_ROOM, TRUE );
		spell_detect_hidden( gsn_detect_hidden, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }
	case 4: case 3: case 2: case 1: case 0:
	default:
          break;
    }

    if (mob->fighting != NULL
    && mob->fighting != mob->hunting )
    {
	do_say( mob, "What the hell are you trying to do?" );
	act("$n points at $N.", mob, NULL, mob->fighting, TO_NOTVICT, TRUE );
	act("$n points at you.", mob, NULL, mob->fighting, TO_VICT, TRUE );
	act("$n gestures and fighting ceases.", mob, NULL, NULL, TO_ROOM, TRUE );
	do_peace( mob, str_empty);
	return FALSE;
    }

    if (mob->in_room == mob->hunting->in_room)
    {
	if (mob->fighting == NULL)
	{
	   if (!can_see( mob, mob->hunting ))
	   {
		do_say( mob, "I can smell you, but I can't see you...  Hm..." );
		multi_hit( mob, mob->hunting, TYPE_UNDEFINED );
		WAIT_STATE( mob, 12 );
		return FALSE;
	   }
	   else
	   {
		switch(number_bits(4))
		{
		    case 1:  case 2:
			if ( IS_AFFECTED( mob->hunting, AFF_FIRESHIELD ) )
			{
			    act("$n quaffs a virtual creamy potion.", mob, NULL, NULL, TO_ROOM, TRUE );
			    spell_invulnerability( gsn_invulnerability, mob->level, mob, mob );
			}
			break;
		    case 3:  case 4:
		    case 5:  case 6:
		    case 7:  case 8:
		    case 9:  case 10:
			if ( mob->hunting->fighting != NULL )
			    multi_hit( mob, mob->hunting, TYPE_UNDEFINED );
			else
			    do_backstab( mob, mob->hunting->name );
			break;
		    default:
			break;
		}

		return FALSE;
	    }
	}
/* otherwise, we are already in combat */

	switch (number_bits(3))
	{
	    case 1: case 2: case 3: case 4: case 5: case 6: case 7:
		do_flee( mob, str_empty );
		break;
	    default:
		break;
	}

	return FALSE;

    }

/* ok, we are not in the room with him... lets find em */

    direction = bfs_find_room( mob, mob->in_room, mob->hunting->in_room, 1000 );

    if (direction < 0 || direction > 5)
    {
	sprintf(buf, "summon %s", mob->hunting == NULL ? "" : mob->hunting->name );
	do_cast( mob, buf );
	return FALSE;
    }

    move_char( mob, direction, TRUE );
    return FALSE;
}


bool spec_scout( CHAR_DATA *mob )
{
    char buf[MAX_STRING_LENGTH];
    int direction;
    static char str_what_the_hell[36] = "What the hell are you trying to do?";

    if (mob->wait != 0)
	return FALSE;

    if (mob->hunting == NULL)
	return FALSE;

    if ( !in_main_war( mob->hunting ) )
	return FALSE;

    if (mob->position != POS_STANDING)
    {
	if (!number_bits(2))
	    do_stand( mob, str_empty );
	return FALSE;
    }

    switch( mob->level )
    {
	case 40: case 39: case 38: case 37: case 36:
	case 35: case 34: case 33: case 32: case 31:
	case 30:
	case 29: case 28: case 27: case 26:
	case 25: case 24: case 23: case 22: case 21:
	case 20: case 19: case 18: case 17: case 16:
	case 15:
	    if (!IS_AFFECTED( mob, AFF_PASS_DOOR ) )
	    {
		act("$n quaffs a virtual swirling potion.", mob, NULL, NULL, TO_ROOM, TRUE);
		spell_pass_door( gsn_pass_door, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }

	case 14: case 13: case 12: case 11:
	case 10: case 9: case 8: case 7: case 6:
	case 5:

	    if (!IS_AFFECTED( mob, AFF_DETECT_INVIS ) )
	    {
		act("$n quaffs a virtual yellow potion.", mob, NULL, NULL, TO_ROOM, TRUE );
		spell_detect_invis( gsn_detect_invis, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }

	    if (!IS_AFFECTED( mob, AFF_DETECT_HIDDEN ) )
	    {
		act("$n quaffs a virtual orange potion.", mob, NULL, NULL, TO_ROOM, TRUE );
		spell_detect_hidden( gsn_detect_hidden, mob->level, mob, mob );
		WAIT_STATE( mob, 12 );
		return FALSE;
	    }
	case 4: case 3: case 2: case 1: case 0:
	default:
          break;
    }

    if (mob->fighting != NULL
    && mob->fighting != mob->hunting )
    {
	do_say( mob, str_what_the_hell );
	act("$n points at $N.", mob, NULL, mob->fighting, TO_NOTVICT, TRUE );
	act("$n points at you.", mob, NULL, mob->fighting, TO_VICT, TRUE );
	act("$n gestures and fighting ceases.", mob, NULL, NULL, TO_ROOM, TRUE );
	do_peace( mob, str_empty );
	return FALSE;
    }

    if (mob->in_room == mob->hunting->in_room)
    {
	if ( mob->mana < mob->max_mana )
	{
	    mob->mana++;
	    return FALSE;
	}

	mob->mana = 0;
	sprintf( buf, "%s is at %s.", mob->hunting->name, mob->in_room->name );
	do_info( mob, buf );
	return FALSE;
    }

/* ok, we are not in the room with him... lets find em */

    direction = bfs_find_room( mob, mob->in_room, mob->hunting->in_room, 1000 );

    if (direction < 0 || direction > 5)
	return FALSE;

    move_char( mob, direction, TRUE );
    return FALSE;
}


bool spec_zombie( CHAR_DATA *mob )
{
  char buf[MAX_STRING_LENGTH];
  static char str_brains[8] = "Brains!";
  static char str_no_brains[13] = "No brains...";
  int direction;

  if (mob->wait != 0)
    return FALSE;

  if (mob->hunting == NULL)
    return FALSE;

  if ( !in_main_war( mob->hunting ) )
    return FALSE;

  if (mob->position != POS_STANDING)
  {
    if (!number_bits(2))
      do_stand( mob, str_empty );
    return FALSE;
  }

  if (mob->fighting != NULL
  && mob->fighting != mob->hunting )
  {
    do_say( mob, str_no_brains );
    do_peace( mob, str_empty );
    return FALSE;
  }

  /* Zombie is slower than other terminators */
  if ( number_bits(3) == 0 )
    return FALSE;

  if (mob->in_room == mob->hunting->in_room)
  {
    if (mob->fighting == NULL)
    {
      do_say( mob, str_brains );
      multi_hit( mob, mob->hunting, TYPE_UNDEFINED );
      return FALSE;
    }
  }

  direction = bfs_find_room( mob, mob->in_room, mob->hunting->in_room, 1000 );

  if (direction < 0 || direction > 5)
  {
    sprintf(buf, "summon %s", mob->hunting == NULL ? "" : mob->hunting->name );
    do_cast( mob, buf );
    return FALSE;
  }

  move_char( mob, direction, TRUE );
  return FALSE;
}

bool spec_dancing_sword( CHAR_DATA *mob )
{
    int direction;

    if (mob->wait != 0)
	return FALSE;

    if (mob->hunting == NULL)
	return FALSE;

    if ( !in_main_war( mob->hunting ) )
	return FALSE;

    if (mob->position != POS_STANDING)
    {
	if (!number_bits(2))
	    do_stand( mob, str_empty );
	return FALSE;
    }

    if (mob->in_room == mob->hunting->in_room)
    {
	if ( mob->mana < mob->max_mana )
	{
	    mob->mana += 2;
	    return FALSE;
	}

	if ( mob->hunting->fighting != NULL )
	{
	  mob->mana = 0;
	  damage( mob, mob->hunting->fighting, 
	    2 * dice( mob->data[0], mob->data[1] ), gsn_dancing_sword,
	    mob->hunting->fighting->resist_physical );
	  stop_fighting( mob, TRUE );
	  mob->max_hit--;
	  if ( mob->max_hit == 0 )
	  {
	    act( "$n disappears with a flash.",
	      mob, NULL, NULL, TO_ROOM, TRUE );
	    extract_char( mob, TRUE );
	  }
	}

	return FALSE;
    }

/* ok, we are not in the room with him... lets find em */

    direction = bfs_find_room( mob, mob->in_room, mob->hunting->in_room, 1000 );

    if (direction < 0 || direction > 5)
	return FALSE;

    move_char( mob, direction, TRUE );
    mob->wait = 1 + ( number_bits( 2 ) ? 0 : 1 );
    return FALSE;
}



bool spec_distributor( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CharIterator citer;
  static char str_tobattle[11] = "To Battle!";

  for ( victim = citer.Start( ch, CharIterator::ROOM );
        victim != NULL; victim = citer.Next() )
  {
    if ( victim != ch && IS_NPC( victim ) )
      break;
  }

  if ( victim == NULL )
    return FALSE;

  do_say( ch, str_tobattle );

  spell_teleport( 63, 50, victim, victim );

  return FALSE;
}


bool spec_waldo( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  CharIterator citer;
  char str_foundme[31] = "You found me! I'm out of here!";
  char str_herespotion[32] = "Here's a potion for finding me!";

  for ( victim = citer.Start( ch, CharIterator::ROOM );
        victim != NULL; victim = citer.Next() )
  {
    if ( !IS_NPC( victim ) && number_bits( 1 ) == 0 )
      break;
  }

  if ( victim == NULL )
    return FALSE;

  do_say( ch, str_foundme );

  if ( mortal_in_main_war( victim ) )
  {
    do_say( ch, str_herespotion );
    obj = create_random_potion( ch->phase );
    act( "$n gives you $p.", ch, obj, victim, TO_VICT, TRUE );
    obj_to_char( obj, victim );
  }

  spell_teleport( 63, 50, ch, ch );

  return FALSE;
}


bool spec_logan( CHAR_DATA *ch )
{
  int direction;
  int delay;

  if (ch->wait != 0)
    return FALSE;

  /* 2 second delay at max health, 0.5 second delay below 20% */
  delay = ch->hit * 100 / ch->max_hit;
  delay -= 20;
  if ( delay < 0 ) delay = 0;
  if ( delay > 80 ) delay = 80;
  delay /= 16;

  ch->data[1]++;
  if ( ch->data[1] < ( 2 + delay ) )
    return FALSE;
  ch->data[1] = 0;

  if ( ch->position != POS_STANDING )
  {
    do_stand( ch, str_empty );
    return FALSE;
  }

  /* Objective 1 - get out of combat */
  if ( ch->fighting != NULL )
  {
    do_flee( ch, str_empty );
    return FALSE;
  }

  /* No need to run if we're healthy. */
  if ( ch->hit >= ch->max_hit )
    return FALSE;

  if ( ch->in_room->vnum == ch->data[0] )
  {
    get_new_target_room( ch );
    return FALSE;
  }

  direction = bfs_find_room( ch, ch->in_room, get_room_index( ch->data[0] ), 1000 );

  if (direction < 0 || direction > 5)
  {
    get_new_target_room( ch );
    return FALSE;
  }

  move_char( ch, direction, TRUE );
 
  return FALSE; 
}


bool spec_super_wimpy( CHAR_DATA *ch )
{
  if ( ch->fighting == NULL )
    return FALSE;

  do_flee( ch, str_empty );

  return FALSE;
}




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
#include "cmod.h"
#include "CharIterator.h"
#include "ObjIterator.h"



/*
 * Local functions.
 */
void	say_spell	args( ( CHAR_DATA *ch, int sn ) );
bool	will_die	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam ) );



/*
 * Lookup a skill by name.
 */
int skill_lookup( const char *name )
{
    int sn;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( skill_table[sn].name == NULL )
	    break;
	if ( LOWER(name[0]) == LOWER(skill_table[sn].name[0])
	&&   !str_prefix( name, skill_table[sn].name ) )
	    return sn;
    }

    return -1;
}



/*
 * Lookup a skill by slot number.
 * Used for object loading.
 */
int slot_lookup( int slot )
{
    extern bool fBootDb;
    int sn;

    if ( slot <= 0 )
	return -1;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( slot == skill_table[sn].slot )
	    return sn;
    }

    if ( fBootDb )
    {
	bug( "Slot_lookup: bad slot %d.", slot );
	abort( );
    }

    return -1;
}



/*
 * Utter mystical words for an sn.
 */
void say_spell( CHAR_DATA *ch, int sn )
{
    char buf  [MAX_STRING_LENGTH];
    char buf2 [MAX_STRING_LENGTH];
    CHAR_DATA *rch;
    char *pName;
    int iSyl;
    int length;
    CharIterator citer;

    struct syl_type
    {
	char *	old;
	char *	new_syl;
    };

    static const struct syl_type syl_table[] =
    {
	{ " ",		" "		},
	{ "ar",		"rab"		},
	{ "au",		"lkiz"		},
	{ "bless",	"neso"		},
	{ "blind",	"nerz"		},
	{ "bur",	"lihx"		},
	{ "cu",		"pab"		},
	{ "de",		"antr"		},
	{ "en",		"unso"		},
	{ "light",	"sat"		},
	{ "lo",		"wlu"		},
	{ "mor",	"sle"		},
	{ "move",	"tup"		},
	{ "ness",	"lhach"		},
	{ "ning",	"gnip"		},
	{ "per",	"duda"		},
	{ "ra",		"sun"		},
	{ "re",		"non"		},
	{ "son",	"rapo"		},
	{ "tect",	"ilnb"		},
	{ "tri",	"cula"		},
	{ "ven",	"nofo"		},
	{ "a", "e" }, { "b", "n" }, { "c", "c" }, { "d", "f" },
	{ "e", "x" }, { "f", "i" }, { "g", "a" }, { "h", "s" },
	{ "i", "o" }, { "j", "y" }, { "k", "m" }, { "l", "w" },
	{ "m", "q" }, { "n", "u" }, { "o", "d" }, { "p", "p" },
	{ "q", "v" }, { "r", "b" }, { "s", "g" }, { "t", "h" },
	{ "u", "j" }, { "v", "k" }, { "w", "l" }, { "x", "r" },
	{ "y", "t" }, { "z", "z" },
	{ "", "" }
    };

    buf[0]	= '\0';
    for ( pName = skill_table[sn].name; *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(syl_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( syl_table[iSyl].old, pName ) )
	    {
		strcat( buf, syl_table[iSyl].new_syl );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    sprintf( buf2, "$n utters the words, '%s'.", buf );
    sprintf( buf,  "$n utters the words, '%s'.", skill_table[sn].name );

    for ( rch = citer.Start( ch, CharIterator::ROOM );
          rch; rch = citer.Next() )
    {
	if ( rch != ch )
	    act( ch->char_class==rch->char_class ? buf : buf2, ch, NULL, rch, TO_VICT, TRUE );
    }

    return;
}



/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
bool saves_spell( int level, CHAR_DATA *victim )
{
    int save;

    save = 50 + ( victim->level - level - victim->saving_throw ) * 5;
    save = URANGE( 5, save, 95 );
    return number_percent( ) < save;
}



/*
 * The kludgy global is for spells who want more stuff from command line.
 */
char *target_name;

void do_cast( CHAR_DATA *ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *tch;
  OBJ_DATA *obj;
  void *vo;
  int mana;
  int sn;
  int temp;
  CharIterator citer;

  if ( is_affected( ch, gsn_silence ) )
  {
    send_to_char("You cannot cast spells, you're silenced!\n\r", ch );
    return;
  }

  /*
   * charmed, non switched npcs may not cast
   */
  if ( IS_NPC( ch )
  && is_affected( ch, gsn_charm_person )
  && ch->desc == NULL )
    return;

  target_name = one_argument( argument, arg1 );
  one_argument( target_name, arg2 );

  if ( arg1[0] == '\0' )
  {
    send_to_char( "Cast which what where?\n\r", ch );
    return;
  }

/* can't have dead people casting */
  if ( !can_interact( ch ) )
  {
    send_to_char("You are either not alive or the war hasn't started.\n\r", ch);
    return;
  }

  if ( ( sn = skill_lookup( arg1 ) ) < 0 )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
 
  temp = ch->position;
  if (ch->fighting != NULL)
    temp = POS_FIGHTING;
 
  if ( temp < skill_table[sn].minimum_position )
  {
    send_to_char( "You can't concentrate enough.\n\r", ch );
    return;
  }

  mana = IS_NPC(ch)
    ? 0
    : class_table[ch->char_class].skill_cost[sn] == 0
      ? UMAX( skill_table[sn].min_mana, 50 )
      : class_table[ch->char_class].skill_cost[sn];

  /*
   * Locate targets.
   */
  victim	= NULL;
  obj		= NULL;
  vo		= NULL;
      
  switch ( skill_table[sn].target )
  {
    default:
      bug( "Do_cast: bad target for sn %d.", sn );
      return;

    case TAR_IGNORE:
      break;

    case TAR_CHAR_HINDRANCE:
    case TAR_CHAR_OFFENSIVE:
      if ( arg2[0] == '\0' )
      {
	     
	if ( ( victim = ch->fighting ) == NULL )
	{
	  send_to_char( "Cast the spell on whom?\n\r", ch );
	  check_failed_combat( ch );	
          return;
	}

	if ( victim->in_room != ch->in_room )
	{
	  send_to_char( "They're too far away!\n\r", ch );
	  return;
	}
      }
      else
      {
	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	  send_to_char( "They aren't here.\n\r", ch );
	  check_failed_combat( ch );	
 	  return;
	}
      }

      if ( ch == victim )
      {
	send_to_char( "You can't do that to yourself.\n\r", ch );
	return;
      }

      if ( !in_same_war( ch, victim ) )
      {
	send_to_char("They're already dead.\n\r", ch );
	return;
      }  	
		    
      if ( !IS_NPC( ch ) )
	ch->pcdata->failed_combat = 0;

      vo = (void *) victim;
      break;

/* temporary comment out */
    case TAR_CHAR_DEFENSIVE:
    case TAR_CHAR_SELF:
      if ( arg2[0] == '\0' )
      {
	victim = ch;
      }
      else
      {
	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	  send_to_char( "They aren't here.\n\r", ch );
	  return;
	}

	if ( !IS_GOD( ch ) && victim != ch )
	{
	  send_to_char( "You may only cast that spell on yourself.\n\r", ch );
	  return;
	}

	if ( !in_same_war( ch, victim ) )
	{
	  send_to_char("They're already dead.\n\r", ch );
	  return;
	}
      }

      vo = (void *) victim;
      break;

    case TAR_OBJ_INV:
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "What should the spell be cast upon?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You are not carrying that.\n\r", ch );
	    return;
	}

	vo = (void *) obj;
	break;
    }

    if ( !IS_NPC(ch) && ch->mana < mana )
    {
	send_to_char( "You don't have enough mana.\n\r", ch );
	return;
    }
      
    if ( str_cmp( skill_table[sn].name, "ventriloquate" ) )
	say_spell( ch, sn );
      
    if (!IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[sn]) 
    {
	send_to_char( "You lost your concentration.\n\r", ch );
	ch->mana -= mana / 2;

	set_join_status( ch );

/* chance to improve spell % */
     if (ch->pcdata->learned[sn] >= 5 && skill_table[sn].spell_fun != spell_null )
	if (dice(1,20) < ch->pcdata->perm_int)
		ch->pcdata->learned[sn] = UMIN( 95, ch->pcdata->learned[sn]++);
	return;
    }
	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
	{
		act( "$N is your beloved master.", ch, NULL, victim,TO_CHAR, TRUE );
		return;
	}
    else
    {
	ch->mana -= mana;

	if ( IS_NPC( ch ) )
	{
	    WAIT_STATE( ch, skill_table[sn].beats );
	}
	else
	{
	    WAIT_STATE( ch,
		UMAX(skill_table[sn].beats,
		    class_table[ch->char_class].skill_delay[sn]));
	}

        if ( ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
             || skill_table[sn].target == TAR_CHAR_HINDRANCE )
        && victim != NULL && is_affected( victim, gsn_reflect ) )
        {
	  send_to_char( "Your spell was reflected!\n\r", ch );
	  send_to_char( "You reflect the spell!\n\r", victim );
          tch = victim;
          victim = ch;
          vo = (void *) ch;
          ch = tch;
          affect_strip( ch, gsn_reflect );
        }

	{
	  int level;

	  level = ch->level;

	  if ( get_curr_int( ch ) > 16 )
	    level += ( get_curr_int( ch ) - 16 );
	  else if ( get_curr_int( ch ) < 11 )
	    level -= ( 11 - get_curr_int( ch ) );

	  (*skill_table[sn].spell_fun) ( sn, level, ch, vo );
	}

	if( !IS_NPC(ch ) )
		set_join_status( ch );
    }

    if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
    &&   victim != ch
    &&   victim->master != ch )
    {
	CHAR_DATA *vch;

        for ( vch = citer.Start( ch, CharIterator::ROOM );
              vch; vch = citer.Next() )
	{
	    if ( victim == vch && victim->fighting == NULL )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	 
	  
	}
    }

    return;
}



/*
 * Cast spells at targets using a magical object.
 */
void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim,
  OBJ_DATA *obj )
{
    void *vo = NULL;
    CharIterator citer;

    if ( sn <= 0 )
	return;

    if ( sn >= MAX_SKILL || skill_table[sn].spell_fun == 0 )
    {
	bug( "Obj_cast_spell: bad sn %d.", sn );
	return;
    }

    switch ( skill_table[sn].target )
    {
    default:
	bug( "Obj_cast_spell: bad target for sn %d.", sn );
	return;

    case TAR_IGNORE:
	vo = (void *)victim;
	break;

    case TAR_CHAR_HINDRANCE:
    case TAR_CHAR_OFFENSIVE:
	if ( victim == NULL )
	    victim = ch->fighting;
	vo = (void *) victim;
	if ( vo == NULL )
	    return;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( victim == NULL )
	    victim = ch;
	vo = (void *) victim;
	if ( vo == NULL )
	    return;
	break;

    case TAR_CHAR_SELF:
	if ( victim == NULL )
	    victim = ch;
	vo = (void *) victim;
	break;

    case TAR_OBJ_INV:
	if ( obj == NULL )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	vo = (void *) obj;
	if ( vo == NULL )
	    return;
	break;
    }

    target_name = "";
    (*skill_table[sn].spell_fun) ( sn, level, ch, vo );

    if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
    &&   victim != ch
    &&   victim->master != ch )
    {
	CHAR_DATA *vch;

        for ( vch = citer.Start( ch, CharIterator::ROOM );
              vch; vch = citer.Next() )
	{
	    if ( victim == vch && victim->fighting == NULL )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }

    return;
}

/* function to check if victim will die */
bool will_die( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
  int total = 0;

  if (IS_AFFECTED( victim, AFF_PROTECT) && ch->alignment < -350)
    total += 1 / 4 * total;

  if (IS_AFFECTED( victim, AFF_SANCTUARY ) )
    total *= 2;

  if (IS_AFFECTED( victim, AFF_TOWERID))
    total += 10;

  if (IS_AFFECTED( victim, AFF_INVULNERABILITY))
    total = 100000;

  if (total >= dam)
    return FALSE;

  return TRUE;
}


/*
 * Spell functions.
 */
void spell_ice_storm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = dice( 30, 6 );
    if ( ch->char_class != CLASS_INVOKER )
      if ( saves_spell( level, victim ) )
	dam /= 2;

    if ( saves_spell( level, victim )
    && ( victim->char_class == CLASS_INVOKER || victim->char_class == CLASS_BARBARIAN ) )
	dam /= 2;

    damage( ch, victim, dam, sn, victim->resist_cold );
    return;
}



void spell_armor( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || victim->char_class == CLASS_BARBARIAN )
	return;
    af.type      = sn;
    af.duration  = 24;
    af.modifier  = -20;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel someone protecting you.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}


void spell_banish_item( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  ObjIterator oiter;

  target_name = one_argument( target_name, arg );  /* strip victim name */
  one_argument( target_name, arg );

  if ( IS_NPC( victim ) )
    return;

  if ( arg[0] == '\0' )
    return;

  for ( obj = oiter.StartList( ch, victim->carrying );
        obj != NULL; obj = oiter.Next( ) )
  {
    if ( !is_name( arg, obj->name ) )
      continue;

    if ( saves_spell( level + 4, victim ) )
      continue;

    act("Your $p is blasted from this dimension.",
      victim, obj, NULL, TO_CHAR, TRUE );
    act("$n's $p is blasted from this dimension.",
      victim, obj, NULL, TO_ROOM, TRUE );

    if ( obj->wear_loc != WEAR_NONE )
      unequip_char( victim, obj );

    obj_from_char( obj );
    extract_obj( obj, "spell_banish_item" );
  }

  return;
}


void spell_become_rambo( int sn, int level, CHAR_DATA *ch, void *vo )
{
/* gives the quaffer some slick powers, kinda */
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type      = sn;
    af.duration  = 5;
    af.location  = APPLY_DAMROLL;
    af.modifier  = 40; 
    af.bitvector = 0;
    affect_to_char( victim, &af );

    victim->max_hit = victim->max_hit / 3;
    victim->hit = UMIN( victim->hit , victim->max_hit );

    return;
}



void spell_blackmantle( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_BLACKMANTLE)
    || saves_spell( level + 10, victim ) )
	return;

/* another save for the barbarian and specialist */
    if ( (victim->char_class == CLASS_NECROMANCER
    || victim->char_class == CLASS_BARBARIAN )
	&& saves_spell( level, victim ) )
	return;

    af.type      = sn;
    af.location  = 0;
    af.modifier  = 0;
    af.duration  = 4;
    af.bitvector = AFF_BLACKMANTLE;
    affect_to_char( victim, &af );
    send_to_char( "You have been decomposed!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_bless( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN || is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.duration  = 6 + level;
    af.location  = APPLY_HITROLL;
    af.modifier  = 1; 
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = -1;
    affect_to_char( victim, &af );
    send_to_char( "You feel righteous.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
  extern int war_timer;
/* changed duration from 1 + level */
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_BLIND)
    || saves_spell( ( level + 5 - war_timer ), victim ) )
	return;
/* another save for the barbarian and specialist */
    if ( (victim->char_class == CLASS_ILLUSIONIST || victim->char_class == CLASS_BARBARIAN )
	&& saves_spell( level, victim ) )
	return;

    af.type      = sn;
    af.location  = APPLY_HITROLL;
    af.modifier  = -4;
    af.duration  = 0 + number_bits(1) + number_bits(1);
    af.bitvector = AFF_BLIND;
    affect_to_char( victim, &af );
    send_to_char( "You are blinded!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    act("$n appears to be blinded.", victim, NULL, NULL, TO_ROOM, TRUE);
    return;
}


void spell_brilliance( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

/*
    if ( is_affected( victim, sn ) )
	return;
*/

    af.type      = sn;
    af.location  = APPLY_INT;
    af.duration  = 1;
    af.modifier  = number_range( 3, 10 );
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "Raw mental energy streams through your mind.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_burning_hands( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam		= number_range( 30, 50 );
    if (ch->char_class != CLASS_ALTERER )
      if ( saves_spell( level, victim ) )
	dam /= 2;
    if ( (victim->char_class == CLASS_ALTERER || victim->char_class == CLASS_BARBARIAN )
	&& saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, victim->resist_fire );
    return;
}



void spell_call_lightning( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  int dam;
  CharIterator citer;

  if ( !IS_OUTSIDE(ch) )
  {
    send_to_char( "You must be out of doors.\n\r", ch );
    return;
  }

  if ( weather_info.sky < SKY_RAINING )
  {
    send_to_char( "You need bad weather.\n\r", ch );
    return;
  }

  dam = dice(level/2, 8);

  send_to_char( "God's lightning strikes your foes!\n\r", ch );
  act( "$n calls God's lightning to strike $s foes!",
    ch, NULL, NULL, TO_ROOM, TRUE );

  for ( vch = citer.Start( ch, CharIterator::WORLD );
        vch != NULL; vch = citer.Next() )
  {
    if ( !in_same_war( ch, vch ) )
      continue;

    if ( vch->in_room == NULL )
      continue;

    if ( vch->in_room == ch->in_room )
    {
      int vdam = dam;

      if ( vch != ch && ( IS_NPC(ch) ? !IS_NPC(vch) :      1      ) )
        damage( ch, vch,
	  saves_spell( level, vch ) ? vdam/2 : vdam,
	  sn, vch->resist_lightning );
      continue;
    }

    if ( vch->in_room->area == ch->in_room->area
    &&   IS_OUTSIDE(vch)
    &&   IS_AWAKE(vch) )
      send_to_char( "Lightning flashes in the sky.\n\r", vch );
  }

  return;
}

void spell_confusion( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if( is_affected( victim, gsn_confusion ) )
	return;

    if( saves_spell( level, victim ) )
	return;

    af.type      = gsn_confusion;
    af.duration  = number_range( 15, 30 );
    af.location  = APPLY_INT;
    af.modifier  = -3;
    af.bitvector = 0;
    affect_join( victim, &af );

    act("$E looks REALLY confused.", ch, NULL, victim, TO_CHAR, TRUE );
    act("$N looks REALLY confused.", ch, NULL, victim, TO_NOTVICT, TRUE );
    act("You feel REALLY confused.", ch, NULL, victim, TO_VICT, TRUE );
    return;
}


void spell_cause_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam;

  dam = dice( 1, 8 ) + level / 3;

  damage( ch, victim, dam, sn, victim->resist_magic );
  return;
}



void spell_cause_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam;

  dam = dice( 3, 8 ) + level - 6;

  damage( ch, victim, dam, sn, victim->resist_magic );
  return;
}



void spell_cause_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam;

  dam = dice( 2, 8 ) + level / 2;

  damage( ch, victim, dam, sn, victim->resist_magic );
  return;
}



void spell_change_sex( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.duration  = 10 * level;
    af.location  = APPLY_SEX;
    do
    {
	af.modifier  = number_range( 0, 2 ) - victim->sex;
    }
    while ( af.modifier == 0 );
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel different.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  CHAR_DATA *fch;
  AFFECT_DATA af;
  int followers = 0;
  CharIterator citer;

  if ( victim == ch )
  {
    send_to_char( "You like yourself even better!\n\r", ch );
    return;
  }

  if (IS_NPC(victim) && victim->level > 20)
    return;

  if ( get_trust( victim ) > 30 )
    return;

  if ( victim->pIndexData && victim->pIndexData->pShop )
    return;

  if ( IS_AFFECTED(victim, AFF_CHARM)
  ||   IS_AFFECTED(ch, AFF_CHARM)
  ||   level < victim->level
  ||   saves_spell( level, victim ) )
    return;

  if ( victim->master )
    stop_follower( victim );

  for ( fch = citer.Start( ch, CharIterator::ROOM ); fch; fch = citer.Next() )
    if ( fch->master == ch )
      followers++;

  if ( followers && saves_spell( level - ( 5 * followers ), victim ) )
  {
    do_say( victim, "Die, Vile Charm Casting Scum!" );
    set_fighting( victim, ch );
    return;
  }

  add_follower( victim, ch );
  af.type      = sn;
  af.duration  = (IS_NPC(victim) ? 20 : 0);
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_CHARM;
  affect_to_char( victim, &af );
  act( "Isn't $n just so nice?", ch, NULL, victim, TO_VICT, TRUE );
  if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
  return;
}



void spell_chill_touch( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int dam;

  dam		= number_range( 15, 25 );
  if ( !saves_spell( level - 4, victim ) && ch->char_class != CLASS_NECROMANCER )
  {
    af.type      = sn;
    af.duration  = 6;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = 0;
    affect_to_char( victim, &af );
  }
  else
  {
    dam /= 2;
  }

  if (victim->char_class == CLASS_BARBARIAN || victim->char_class == CLASS_NECROMANCER )
    if (saves_spell( level, victim ) )
      dam /= 2;

  damage( ch, victim, dam, sn, victim->resist_drain );
  return;
}



/* modified colour spray by Rip */
void spell_colour_spray( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dam;

    dam		= number_range( 30, 50 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    if ( victim->char_class == CLASS_BARBARIAN || victim->char_class == CLASS_ALTERER )
	if (saves_spell( level, victim ) )
		dam /= 2;

  if (!saves_spell(level - 5, victim ) )
  {
    af.type      = sn;
    af.location  = APPLY_HITROLL;
    af.modifier  = -4;
    af.duration  = 1;
    af.bitvector = AFF_BLIND;
    affect_join( victim, &af );
    send_to_char( "You have been blinded!\n\r", victim );
    act("$n appears to be blinded.", victim, NULL, NULL, TO_ROOM, TRUE);
  }

    damage( ch, victim, dam, sn, victim->resist_magic );
    return;
}



void spell_continual_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *light;

    light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), ch->phase );
    obj_to_room( light, ch->in_room );
    act( "$n twiddles $s thumbs and $p appears.",   ch, light, NULL, TO_ROOM, TRUE );
    act( "You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR, TRUE );
    return;
}



void spell_control_weather( int sn, int level, CHAR_DATA *ch, void *vo )
{
  if ( !in_main_war( ch ) )
  {
    send_to_char( "You can only control weather while in the main war.\n\r", ch );
    return;
  }

  if ( !str_cmp( target_name, "better" ) )
    weather_info.change += dice( level / 3, 4 );
  else if ( !str_cmp( target_name, "worse" ) )
    weather_info.change -= dice( level / 3, 4 );
  else
    send_to_char ("Do you want it to get better or worse?\n\r", ch );

  send_to_char( "Ok.\n\r", ch );
  return;
}



void spell_create_food( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *mushroom;

    mushroom = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), ch->phase );
    mushroom->value[0] = 5 + level;
    obj_to_room( mushroom, ch->in_room );
    act( "$p suddenly appears.", ch, mushroom, NULL, TO_ROOM, TRUE );
    act( "$p suddenly appears.", ch, mushroom, NULL, TO_CHAR, TRUE );
    return;
}


void spell_create_illusory_blood( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *blood;

    if ( !can_interact( ch ) )
    {
      send_to_char( "You cannot create blood if you are not in the main war.\n\r", ch );
      return;
    }

    blood = create_object( get_obj_index( 2288 ), ch->phase );
    obj_to_room( blood, ch->in_room );
    blood->value[2] = LIQ_KOOL_AID;
    act( "$p suddenly appears.", ch, blood, NULL, TO_ROOM, TRUE );
    act( "$p suddenly appears.", ch, blood, NULL, TO_CHAR, TRUE );
    return;
}


void spell_create_spring( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *spring;

    spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), ch->phase );
    spring->timer = level * PULSE_TICK;
    obj_to_room( spring, ch->in_room );
    act( "$p flows from the ground.", ch, spring, NULL, TO_ROOM, TRUE );
    act( "$p flows from the ground.", ch, spring, NULL, TO_CHAR, TRUE );
    return;
}



void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int water;

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "It is unable to hold water.\n\r", ch );
	return;
    }

    if ( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
    {
	send_to_char( "You fill the container.\n\r", ch );
        obj->value[2] = 9;
        obj->value[1] = obj->value[0];
	return;
    }

    water = UMIN(
		level * (weather_info.sky >= SKY_RAINING ? 4 : 2),
		obj->value[0] - obj->value[1]
		);
  
    if ( water > 0 )
    {
	obj->value[2] = LIQ_WATER;
	obj->value[1] += water;
	if ( !is_name( "water", obj->name ) )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "%s water", obj->name );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}
	act( "$p is filled.", ch, obj, NULL, TO_CHAR, TRUE );
    }

    return;
}



void spell_cure_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( !is_affected( victim, gsn_blindness ) || victim->char_class == CLASS_BARBARIAN )
	return;
    affect_strip( victim, gsn_blindness );
    send_to_char( "Your vision returns!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    if (victim->char_class == CLASS_BARBARIAN)
	return;

    if (IS_AFFECTED( victim, AFF_BLACKMANTLE ))
	return;

    heal = dice(3, 8) + level - 6;
    heal_character( victim, heal );
    update_pos( victim );
    send_to_char( "You feel better!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    if (victim->char_class == CLASS_BARBARIAN)
	return;

    if (IS_AFFECTED( victim, AFF_BLACKMANTLE ))
	return;

    heal = dice(1, 8) + level / 3;
    heal_character( victim, heal );
    update_pos( victim );
    send_to_char( "You feel better!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if (victim->char_class == CLASS_BARBARIAN)
	return;

    if ( is_affected( victim, gsn_poison ) )
    {
	affect_strip( victim, gsn_poison );
	act( "$N looks better.", ch, NULL, victim, TO_NOTVICT, TRUE );
	send_to_char( "A warm feeling runs through your body.\n\r", victim );
	send_to_char( "Ok.\n\r", ch );
    }
    return;
}



void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    if (victim->char_class == CLASS_BARBARIAN)
	return;

    if (IS_AFFECTED( victim, AFF_BLACKMANTLE ))
	return;

    heal = dice(2, 8) + level /2 ;
    heal_character( victim, heal );
    update_pos( victim );
    send_to_char( "You feel better!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_CURSE) || saves_spell( level, victim ) )
	return;
    if (ch->char_class == CLASS_BARBARIAN )
	if (saves_spell( level, victim ) )
		return;

    af.type      = sn;
    af.duration  = 4*level;
    af.location  = APPLY_HITROLL;
    af.modifier  = -1;
    af.bitvector = AFF_CURSE;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
/* changed modifyer to 4 from 1 */
    af.modifier  = 4;
    affect_to_char( victim, &af );

    send_to_char( "You are briefly surrounded by a red aura.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Your victim is briefly surrounded by a red aura.\n\r", ch );
    return;
}


void spell_dancing_sword( int sn, int level, CHAR_DATA *ch, void *vo )
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  CHAR_DATA *mob;
  char buf[MAX_STRING_LENGTH];
  extern bool spec_dancing_sword( CHAR_DATA *mob );
  CharIterator citer;

  if ( obj->item_type != ITEM_WEAPON
  || obj->value[3] != 3 )
  {
    send_to_char("You may only enchant swords!\n\r", ch );
    return;
  }

  for ( mob = citer.Start( ch, CharIterator::ALL );
        mob != NULL; mob = citer.Next() )
  {
    if ( mob->hunting && mob->hunting == ch
    && mob->spec_fun == spec_dancing_sword )
    {
      send_to_char( "You can only control one dancing sword at a time.\n\r",ch);
      return;
    }
  }

  mob = create_mobile( get_mob_index( 16 ), ch->phase );

  mob->hunting = ch;
  mob->max_hit = UMAX( 5, obj->weight );
  mob->hit = mob->max_hit;

  mob->max_mana = URANGE( 12, obj->weight * 2, 36 );
								                  mob->level = 30;
  mob->gold = 0;
  mob->char_class = CLASS_WARRIOR;

  mob->data[0] = obj->value[1];
  mob->data[1] = obj->value[2];

  free_string( mob->name );
  mob->name = str_dup( obj->name );
  free_string( mob->short_descr );
  mob->short_descr = str_dup( obj->short_descr );
  free_string( mob->long_descr );
  sprintf( buf, "%s floats about nearby.\n\r", mob->short_descr );
  mob->long_descr = str_dup( buf );

  mob->spec_fun = spec_dancing_sword;

  char_to_room( mob, ch->in_room );

  act("$n animates $p!", ch, obj, NULL, TO_ROOM, TRUE );
  act("You animate $p!", ch, obj, NULL, TO_CHAR, TRUE );

  extract_obj( obj, "spell_dancing_sword" );

  return;
}



void spell_death_spray( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;
    CharIterator citer;

    send_to_char("Black rays shoot from your fingertips.\n\r", ch );
    act("Black rays shoot from $n's fingertips.\n\r", ch, NULL, NULL, TO_ROOM, TRUE );

    for ( vch = citer.Start( ch, CharIterator::ROOM );
          vch != NULL; vch = citer.Next() )
    {
	vch_next = vch->next;

	if ( !in_same_war( ch, vch ) ) 
	    continue;

	if (vch->in_room == ch->in_room)
        {
            dam = 80;
            if (vch == ch)
		dam = 0;
		if (ch->char_class != CLASS_INVOKER)
			if (saves_spell( level, vch ) )
				dam /= 2;

                damage( ch, vch, dam, sn, vch->resist_magic );
            continue;
         }
    }

    return;
}



void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (victim->char_class == CLASS_BARBARIAN)
	return;

    if ( IS_AFFECTED(victim, AFF_DETECT_EVIL) )
	return;
    af.type      = sn;
/* modified duration to 60 from level */
    af.duration  = 24;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_EVIL;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes tingle.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_hidden( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if ( IS_AFFECTED(victim, AFF_DETECT_HIDDEN) )
	return;
    af.type      = sn;
/* modified duration from level to 60 */
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( victim, &af );
    send_to_char( "Your awareness improves.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (victim->char_class == CLASS_BARBARIAN)
	return;

    if ( IS_AFFECTED(victim, AFF_DETECT_INVIS) )
	return;
    af.type      = sn;
    af.duration = 5*level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes tingle.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (victim->char_class == CLASS_BARBARIAN)
	return;

    if ( IS_AFFECTED(victim, AFF_DETECT_MAGIC) )
	return;
    af.type      = sn;
    af.duration  = 24;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_MAGIC;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes tingle.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
    {
	if ( obj->value[3] != 0 )
	    send_to_char( "You smell poisonous fumes.\n\r", ch );
	else
	    send_to_char( "It looks very delicious.\n\r", ch );
    }
    else
    {
	send_to_char( "It doesn't look poisoned.\n\r", ch );
    }

    return;
}



void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
  
    if ( !IS_NPC(ch) && IS_EVIL(ch) )
	victim = ch;
  
    if ( IS_GOOD(victim) )
    {
	act( "God protects $N.", ch, NULL, victim, TO_ROOM, TRUE );
	return;
    }

    if ( IS_NEUTRAL(victim) )
    {
	act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR, TRUE );
	return;
    }

    dam = dice( level, 4 );
    if ( level > victim->level )
        dam = 100;
    if ( saves_spell( level, victim ) )
	dam /= 2;
    if ( victim->char_class == CLASS_BARBARIAN && saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, victim->resist_magic );
    return;
}


void spell_dispel_good( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
  
    if ( !IS_NPC(ch) && IS_EVIL(ch) )
	victim = ch;
  
    if ( IS_EVIL(victim) )
    {
	act( "Satan disallows you to destroy his good friend $N.", ch, NULL, victim, TO_ROOM, TRUE );
	return;
    }

    if ( IS_NEUTRAL(victim) )
    {
	act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR, TRUE );
	return;
    }

    dam = dice( level, 4 );
    if ( level > victim->level )
        dam = 100;
    if ( saves_spell( level, victim ) )
	dam /= 2;
    if ( victim->char_class == CLASS_BARBARIAN && saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, victim->resist_magic );
    return;
}




void spell_dispel_magic( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
/*
    int affected_by;
*/

    if ( victim->affected_by == 0)
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( victim->char_class == CLASS_ABJURER && ch->char_class != CLASS_ABJURER )
	if (saves_spell( level, victim ) )
		return;

/* remove all bits, not just one randomly */
/*
    for ( ;; )
    {
	affected_by = 1 << number_bits( 5 );
	if ( IS_SET(victim->affected_by, affected_by) )
	    break;
    }
    REMOVE_BIT(victim->affected_by, affected_by);
    send_to_char( "OK.\n\r", ch);
    
*/
	while( victim->affected )
		affect_remove( victim, victim->affected );
	victim->affected = 0;
        send_to_char( "Your skin tightens as the magical aura around your body dispruses.\n\r", victim);
        act( "The air tingles as the magical aura around $n dispurses.", victim, NULL, NULL, TO_ROOM, TRUE);

    return;
}



void spell_dragon_power( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int breath;

    if ( IS_NPC(ch) )
	return;

    breath = dice( 1, 5 );

    ch->pcdata->learned[gsn_fly] = 100;

    switch(breath)
    {
	case 1:
	    send_to_char( "You gain the power of acid!\n\r", ch );
	    ch->pcdata->learned[gsn_acid_breath] = 100;
	    ch->pcdata->learned[gsn_ice_storm] = 100;
	    ch->pcdata->learned[gsn_colour_spray] = 100;
	    ch->pcdata->learned[gsn_death_spray] = 100;
	    break;

	case 2:
	    send_to_char( "You gain the power of fire!\n\r", ch );
	    ch->pcdata->learned[gsn_fire_breath] = 100;
	    ch->pcdata->learned[gsn_fireball] = 100;
	    ch->pcdata->learned[gsn_burning_hands] = 100;
	    ch->pcdata->learned[gsn_flamestrike] = 100;
	    break;

	case 3:
	    send_to_char( "You gained the power of frost!\n\r", ch );
	    ch->pcdata->learned[gsn_frost_breath] = 100;
	    ch->pcdata->learned[gsn_chill_touch] = 100;
	    ch->pcdata->learned[gsn_vampiric_touch] = 100;
	    break;

	case 4:
	    send_to_char( "You gained the power of gas!\n\r", ch );
	    ch->pcdata->learned[gsn_gas_breath] = 100;
	    ch->pcdata->learned[gsn_poison] = 100;
	    ch->pcdata->learned[gsn_blindness] = 100;
	    ch->pcdata->learned[gsn_faerie_fog] = 100;
	    break;

	case 5:
	    send_to_char( "You gained the power of lightning!\n\r", ch );
	    ch->pcdata->learned[gsn_lightning_breath] = 100;
	    ch->pcdata->learned[gsn_lightning_bolt] = 100;
	    ch->pcdata->learned[gsn_shocking_grasp] = 100;
	    ch->pcdata->learned[gsn_call_lightning] = 100;
	    break;
	default:
	    break;
    }

    ch->pcdata->learned[gsn_dragon_power] = 0;	

    return;
}


void spell_earthquake( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CharIterator citer;

    send_to_char( "The earth trembles beneath your feet!\n\r", ch );
    act( "$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM, TRUE );


    for ( vch = citer.Start( ch, CharIterator::WORLD );
          vch != NULL; vch = citer.Next() )
    {
	if ( !in_same_war( ch, vch ) )
	    continue;

	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
/* need to mod to affect pcs... */
/*
	    if ( vch != ch && ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) ) )
*/
            if ( vch != ch && ( IS_NPC(ch) ? !IS_NPC(vch) :     1       ) )
		damage( ch, vch, level + dice(2, 8), sn, vch->resist_physical );
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char( "The earth trembles and shivers.\n\r", vch );
    }

    return;
}



void spell_enchant_weapon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

/* modified to enchant jewelry */
    if ( !(obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_JEWELRY )
    ||   IS_OBJ_STAT(obj, ITEM_MAGIC)
    ||   obj->pIndexData->affected != NULL
    ||   obj->affected != NULL )
	return;

    obj_from_char( obj );

    if ( affect_free == NULL )
    {
	paf		= (AFFECT_DATA *) alloc_perm( sizeof(*paf) );
    }
    else
    {
	paf		= affect_free;
	affect_free	= affect_free->next;
    }

    paf->type		= -1;
    paf->duration	= -1;
    paf->location	= APPLY_HITROLL;
/* uh, i like mages, change from lev/5 to lev/3 */
/* actually, eltanin's method is better, lets change it again */
    paf->modifier	= 1 + number_bits(1) + number_bits(1)
                            + (level > 32 && number_bits(1))
                            + (level > 39 + number_bits(1));
    if (ch->char_class == CLASS_ENCHANTER)
	paf->modifier += 1 + number_bits(1);
    if (ch->char_class == CLASS_INVOKER || ch->char_class == CLASS_NECROMANCER)
	paf->modifier--;
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    if ( affect_free == NULL )
    {
	paf		= (AFFECT_DATA *) alloc_perm( sizeof(*paf) );
    }
    else
    {
	paf		= affect_free;
	affect_free	= affect_free->next;
    }

    paf->type		= -1;
    paf->duration	= -1;
    paf->location	= APPLY_DAMROLL;
/* eltanins method is better */
    paf->modifier	= 1 + number_bits(1) + number_bits(1)
                            + (level > 32 && number_bits(1))
                            + (level > 39 + number_bits(1));
    if (ch->char_class == CLASS_ENCHANTER)
	paf->modifier += 1 + number_bits(1);
    if (ch->char_class == CLASS_INVOKER || ch->char_class == CLASS_NECROMANCER)
	paf->modifier--;
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    if ( IS_GOOD(ch) )
    {
	SET_BIT(obj->extra_flags, ITEM_ANTI_EVIL);
	act( "$p glows blue.", ch, obj, NULL, TO_CHAR, TRUE );
    }
    else if ( IS_EVIL(ch) )
    {
	SET_BIT(obj->extra_flags, ITEM_ANTI_GOOD);
	act( "$p glows red.", ch, obj, NULL, TO_CHAR, TRUE );
    }
    else
    {
/* neutral mages make neutral ones, not that it matters */
/*
	SET_BIT(obj->extra_flags, ITEM_ANTI_EVIL);
	SET_BIT(obj->extra_flags, ITEM_ANTI_GOOD);
*/
	act( "$p glows yellow.", ch, obj, NULL, TO_CHAR, TRUE );
    }

    obj_to_char( obj, ch );

    send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_energy_drain( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if ( saves_spell( level, victim )
    && ch->char_class != CLASS_NECROMANCER )
    {
    	damage( ch, victim, 0, sn, 0 );
	return;
    }

    dam		= dice( 1, 10 ) * victim->mana / 10;

    if ( victim->char_class == CLASS_NECROMANCER
    && saves_spell( level, victim ) )
	dam /= 2;

    if ( victim->resist_drain > 0 )
      dam -= victim->resist_drain;

    if ( dam < 0 )
      dam = 0;

    ch->mana		+= dam;
    victim->mana	-= dam;

    damage( ch, victim, dice( 1, 10 ), sn, victim->resist_drain );

    return;
}


/*
 * Drain HP.
 * Caster gains HP.
 */
void spell_vampiric_touch( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    int top;

    if ( ch->char_class == CLASS_NECROMANCER )
      level += 4;

    if ( saves_spell( level, victim ) )
    {
    	damage( ch, victim, 0, sn, 0 );
	return;
    }

    dam		= 20 + dice(1, level);

    if ( ch->char_class == CLASS_NECROMANCER )
        dam *= 2;

    if (victim->char_class == CLASS_BARBARIAN || victim->char_class == CLASS_NECROMANCER)
	if (saves_spell( level, victim ) )
		dam /= 2;

    if ( victim->resist_drain > 0 )
      dam -= victim->resist_drain;

    if ( dam < 0 )
      dam = 0;

    if ( !IS_AFFECTED( victim, AFF_BLACKMANTLE ) )
    {
      ch->hit		+= dam;
      if ( !IS_NPC( ch ) )
        ch->pcdata->potential_dr += dam;
      top = ch->hit;
      ch->hit		= UMIN( ch->max_hit * 3 / 2 , ch->hit );
      if ( !IS_NPC( ch ) )
        ch->pcdata->potential_dr -= top - ch->hit;
    }

    damage( ch, victim, dam, sn, 0 );

    return;
}


void spell_entangle( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( is_affected(victim, gsn_entangle) )
    return;

  if ( (victim->char_class == CLASS_DRUID
       || victim->char_class == CLASS_BARBARIAN )
  && saves_spell( level, victim ) )
    return;

  af.type      = sn;
  af.location  = APPLY_AC;
  af.modifier  = 100;
  af.duration = 10;
  while ( !saves_spell( level, victim ) && af.duration < 51 )
    af.duration += 10;

  if ( ch->char_class != CLASS_DRUID )
    af.duration /= 2;

  af.bitvector = 0;
  affect_to_char( victim, &af );
  send_to_char( "Roots spring from the ground and grasp your feet!\n\r",
    victim );
  act("Roots spring from the ground and grasp $n's feet.", victim, NULL,
    NULL, TO_ROOM, TRUE);
  return;
}



void spell_farsee( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN || is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.duration  = 2 * level;
    af.location  = 0;
    af.modifier  = 0; 
    af.bitvector = AFF_FARSEE;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes become sharp as a hawk's.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}


void spell_feeblemind( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( ch->char_class != CLASS_ENCHANTER
    && saves_spell( level, victim ) )
	return;

    if ( is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.duration  = level/7;
    af.location  = APPLY_INT;
    af.modifier  = -1 * number_range( 5, 10 );
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel quite retarded.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_fireball( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam		=  70 + dice( 2, 20 );
    if ( saves_spell( level, victim ) && ch->char_class != CLASS_INVOKER )
	dam /= 2;
    if ( victim->char_class == CLASS_BARBARIAN || victim->char_class == CLASS_INVOKER)
	dam /= 2;

    if (victim->char_class == CLASS_MAGE)
	dam /= 2;
    if (victim->char_class == CLASS_BARD)                                    
	dam /= 2;

    damage( ch, victim, dam, sn, victim->resist_fire );
    return;
}



void spell_fireshield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if ( IS_AFFECTED(victim, AFF_FIRESHIELD) )
	return;
    af.type      = sn;
    af.duration  = 2;

    af.location  = APPLY_RESIST_FIRE;
    af.modifier  = 10;
    af.bitvector = AFF_FIRESHIELD;
    affect_to_char( victim, &af );
    send_to_char( "You start to burn.\n\r", victim );
    act( "$n's starts to burn.", victim, NULL, NULL, TO_ROOM, TRUE );
    return;
}


void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam;

  dam = dice(10, 8);
  if ( saves_spell( level, victim ) )
    dam /= 2;
  if ( victim->char_class == CLASS_BARBARIAN && saves_spell( level, victim ) )
    dam /= 2;

  damage( ch, victim, dam, sn, victim->resist_fire );
  return;
}



void spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
	return;
    af.type      = sn;
/* changed duration from level to level / 6 */
    af.duration  = level;
    af.location  = APPLY_AC;
    af.modifier  = 2 * level;
    af.bitvector = AFF_FAERIE_FIRE;
    affect_to_char( victim, &af );
    send_to_char( "You are surrounded by a pink outline.\n\r", victim );
    act( "$n is surrounded by a pink outline.", victim, NULL, NULL, TO_ROOM, TRUE );
    return;
}



void spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *ich;
    CharIterator citer;

    act( "$n conjures a cloud of purple smoke.", ch, NULL, NULL, TO_ROOM, TRUE );
    send_to_char( "You conjure a cloud of purple smoke.\n\r", ch );

    for ( ich = citer.Start( ch, CharIterator::ROOM ); ich; ich = citer.Next() )
    {
	if ( !IS_NPC(ich) && IS_SET(ich->act, PLR_WIZINVIS) )
	    continue;

	if ( ich == ch || saves_spell( level, ich ) )
	    continue;

	if ( ich->char_class == CLASS_ILLUSIONIST )
	    continue;

	affect_strip ( ich, gsn_invis			);
	affect_strip ( ich, gsn_mass_invis		);
	affect_strip ( ich, gsn_sneak			);
	REMOVE_BIT   ( ich->affected_by, AFF_HIDE	);
	REMOVE_BIT   ( ich->affected_by, AFF_INVISIBLE	);
	REMOVE_BIT   ( ich->affected_by, AFF_SNEAK	);
	act( "$n is revealed!", ich, NULL, NULL, TO_ROOM, TRUE );
	send_to_char( "You are revealed!\n\r", ich );
    }

    return;
}



void spell_fly( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if ( IS_AFFECTED(victim, AFF_FLYING) )
	return;
    af.type      = sn;
    af.duration  = level + 3;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char( victim, &af );
    send_to_char( "Your feet rise off the ground.\n\r", victim );
    act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM, TRUE );
    return;
}



void spell_gate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;

    if ( ( victim = get_char_phase( ch, target_name ) ) == NULL
    ||   victim == ch
    ||   victim->in_room == NULL
    ||   IS_AFFECTED(victim, AFF_NO_SUMMON)
    ||   victim->level > level
    ||   victim->fighting != NULL )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( !in_main_war( ch ) && in_main_war( victim ) )
    {
      send_to_char( "You cannot gate to main war players if you are not in the war.\n\r", ch );
      return;
    }

    if (IS_NPC(victim))
    {
	send_to_char("Gate is currently limited to players.\n\r", ch );
	return;
    }

    act( "$n becomes ethereal and dispapears.", ch, NULL, NULL, TO_ROOM, TRUE );
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
    act( "$n suddenly fades into existance.", ch, NULL, NULL, TO_ROOM, TRUE );
    do_look( ch, "" );
    
    return;
}



/*
 * Spell for mega1.are from Glop/Erkenbrand.
 */
void spell_general_purpose( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = number_range( 25, 100 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, victim->resist_physical );
    return;
}

void reduce_smoke( CHAR_DATA *ch, int amount )
{
  if ( !ch->in_room )
    return;

  if ( ch->in_room->smoke <= 0 )
    return;

  ch->in_room->smoke = UMAX( 0, ch->in_room->smoke - amount );

  if ( ch->in_room->smoke == 0 )
  {
    send_to_char( "The smoke dissipates.\n\r", ch );
    act( "The smoke dissipates.", ch, NULL, NULL, TO_ROOM, FALSE );
  }

  return;
}

void spell_smoke_bomb( int sn, int level, CHAR_DATA *ch, void *vo )
{
  if ( !in_main_war( ch ) )
    return;

  if ( ch->in_room == NULL )
    return;

  act("$n conjures up a small shining ball and hurls it at the ground!",
    ch, NULL, NULL, TO_ROOM, TRUE );
  act("You conjure up a small shining ball and hurl it at the ground!",
    ch, NULL, NULL, TO_CHAR, TRUE );
  act("POOF!  The room is covered in smoke...", ch, NULL, NULL, TO_ROOM, TRUE );
  act("POOF!  The room is covered in smoke...", ch, NULL, NULL, TO_CHAR, TRUE );

  ch->in_room->smoke += 30;

  return;
}


void spell_steal_enchantment( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  AFFECT_DATA *paf;
  int steal_sn;
  char arg[MAX_INPUT_LENGTH];

  if ( saves_spell( level, victim ) )
    return;

  target_name = one_argument( target_name, arg );
  one_argument( target_name, arg );

  steal_sn = skill_lookup( arg );

  if ( steal_sn == -1 )
  {
    send_to_char("What kind of spell is that?\n\r", ch );
    return;
  }

  for ( paf = victim->affected; paf != NULL; paf = paf->next )
    if ( steal_sn == paf->type )
      break;

  if ( paf == NULL )
  {
    send_to_char("They aren't affected by that enchantment.\n\r", ch );
    return;
  }

  af.type      = paf->type;
  af.location  = paf->location;
  af.duration  = paf->duration;
  af.modifier  = paf->modifier;
  af.bitvector = paf->bitvector;
  affect_to_char( ch, &af );

  affect_strip( victim, steal_sn );

  act("You steal an enchantment from $N.", ch, NULL, victim, TO_CHAR, TRUE );
  act("$n steals an enchantment from $N.", ch, NULL, victim, TO_NOTVICT, TRUE );
  act("$n steals an enchantment from you.", ch, NULL, victim, TO_VICT, TRUE );

  return;
}


void spell_strength( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

/*
    if ( is_affected( victim, sn ) )
	return;
*/

    af.type      = sn;
    af.location  = APPLY_STR;
    af.duration  = number_range( 14, 15 );
    af.modifier  = 2;
    if ( victim->char_class == CLASS_ALTERER )
    {
	af.duration *= 2;
	af.modifier *= 2;
    }
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel stronger.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = UMIN( 50, victim->hit - dice(1,4) );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    if ( victim->char_class == CLASS_BARBARIAN && saves_spell( level, victim ) )
	dam /= 2;
    if ( victim->char_class == CLASS_MONK)
	dam /= 2;
    damage( ch, victim, dam, sn, victim->resist_magic );
    return;
}



void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if (IS_AFFECTED( victim, AFF_BLACKMANTLE ))
	return;

    heal_character( victim, 100 );
    update_pos( victim );
    send_to_char( "A warm feeling fills your body.\n\r", victim );
    act( "$n heals $N.", ch, NULL, victim, TO_NOTVICT, TRUE );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



/*
 * Spell for mega1.are from Glop/Erkenbrand.
 */
void spell_high_explosive( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = number_range( 30, 120 );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, victim->resist_physical );
    return;
}


void spell_illusory_appearance( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( victim->char_class == CLASS_BARBARIAN || is_affected( victim, sn ) )
    return;

  af.type      = sn;
  af.duration  = 6;
  af.location  = APPLY_AC;
  af.modifier  = -20; 
  af.bitvector = 0;
  affect_to_char( victim, &af );

  send_to_char( "Your figure changes form.\n\r", victim );
  if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
  return;
}


void spell_illusory_auras( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( victim->char_class == CLASS_BARBARIAN || is_affected( victim, sn ) )
    return;

  af.type      = sn;
  af.duration  = 6;
  af.location  = APPLY_AC;
  af.modifier  = -10; 
  af.bitvector = 0;
  affect_to_char( victim, &af );

  send_to_char( "You begin randomly glowing.\n\r", victim );
  if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
  return;
}


void do_identify( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj;

  if ( argument[0] == '\0' )
  {
    send_to_char( "Identify what?\n\r", ch );
    return;
  }

  obj = get_obj_carry( ch, argument );

  if ( obj == NULL )
  {
    send_to_char( "You don't seem to have that.\n\r", ch );
    return;
  }

  identify_item( obj, ch );

  return;
}


void identify_item( OBJ_DATA *obj, CHAR_DATA *ch )
{
  char buf[MAX_STRING_LENGTH];
  AFFECT_DATA *paf;

  sprintf( buf,
    "Object '%s' is type %s, extra flags %s.\n\r",
    obj->name,
    item_type_name( obj->item_type, obj->pIndexData->vnum ),
    extra_bit_name( obj->extra_flags ) );
  send_to_char( buf, ch );

  sprintf( buf, "Weight is %d, value is %d, equip points: %d.\n\r",
    obj->weight,
    obj->cost,
    get_equip_points( obj ) );
  send_to_char( buf, ch );

  switch ( obj->item_type )
  {
    case ITEM_QUIVER:
      sprintf( buf, "Contains %d arrows.\n\r", obj->value[2] );
      send_to_char( buf, ch );
      if ( obj->value[1] > 0 )
      {
        sprintf( buf, "These arrows cast the spell %s.\n\r",
          skill_table[obj->value[1]].name );
        send_to_char( buf, ch );
        if( IS_SET( obj->extra_flags, ITEM_RANGED_MAGIC ) )
          send_to_char(
            "These arrows strike normally in addition to the spell.\n\r", ch );
      }
      break;
    case ITEM_SCROLL: 
    case ITEM_POTION:
      sprintf( buf, "Level %d spells of:", obj->value[0] );
      send_to_char( buf, ch );

      if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
      {
	send_to_char( " '", ch );
	send_to_char( skill_table[obj->value[1]].name, ch );
	send_to_char( "'", ch );
      }

      if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
      {
	send_to_char( " '", ch );
	send_to_char( skill_table[obj->value[2]].name, ch );
	send_to_char( "'", ch );
      }

      if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
      {
	send_to_char( " '", ch );
	send_to_char( skill_table[obj->value[3]].name, ch );
	send_to_char( "'", ch );
      }

      send_to_char( ".\n\r", ch );
      break;

    case ITEM_WAND: 
    case ITEM_STAFF: 
      sprintf( buf, "Has %d(%d) charges of level %d",
	obj->value[1], obj->value[2], obj->value[0] );
      send_to_char( buf, ch );
      
      if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
      {
	send_to_char( " '", ch );
	send_to_char( skill_table[obj->value[3]].name, ch );
	send_to_char( "'", ch );
      }

      send_to_char( ".\n\r", ch );
      break;
      
    case ITEM_WEAPON:
      sprintf( buf, "Damage is %dd%d (average %d).\n\r",
	obj->value[1], obj->value[2],
	( obj->value[1] + obj->value[1]*obj->value[2] ) / 2 );
      send_to_char( buf, ch );
      break;

    case ITEM_ARMOR:
    case ITEM_JEWELRY:
      sprintf( buf, "Armor class is %d.\n\r", obj->value[0] );
      send_to_char( buf, ch );
      break;
  }

  for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
  {
    if ( paf->location != APPLY_NONE && paf->modifier != 0 )
    {
      sprintf( buf, "Affects %s by %d.\n\r",
	affect_loc_name( paf->location ), paf->modifier );
      send_to_char( buf, ch );
    }
  }

  for ( paf = obj->affected; paf != NULL; paf = paf->next )
  {
    if ( paf->location != APPLY_NONE && paf->modifier != 0 )
    {
      sprintf( buf, "Affects %s by %d.\n\r",
	affect_loc_name( paf->location ), paf->modifier );
      send_to_char( buf, ch );
    }
  }

  return;
}



void spell_improved_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if ( IS_AFFECTED(victim, AFF_IMPROVED_INVIS) )
	return;

    act( "$n fades out of existence.", victim, NULL, NULL, TO_ROOM, TRUE );
    af.type      = sn;
	af.duration = 4;
    af.location  = APPLY_REGEN_MANA;
    af.modifier  = -200;
    af.bitvector = AFF_IMPROVED_INVIS;
    affect_to_char( victim, &af );
    send_to_char( "You fade out of existence.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if ( IS_AFFECTED(victim, AFF_INFRARED) )
	return;
    act( "$n's eyes glow red.\n\r", ch, NULL, NULL, TO_ROOM, TRUE );
    af.type      = sn;
    af.duration  = 2*level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INFRARED;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes glow red.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if ( IS_AFFECTED(victim, AFF_INVISIBLE) )
    {
	send_to_char(" You are already invisible!", ch );
	return;
    }

    act( "$n fades out of existence.", victim, NULL, NULL, TO_ROOM, TRUE );
    af.type      = sn;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char( victim, &af );
    send_to_char( "You fade out of existence.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_ki_release( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = 0;

    dam = number_range( 30, 50 );

    move_char( victim, number_range(0, 5), FALSE );
    WAIT_STATE( victim, 12 );
    victim->position = POS_RESTING;
    damage( ch, victim, dam, sn, victim->resist_magic );
    return;	
}	

void spell_know_alignment( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char *msg;
    int ap;

/* chars dont have alignment... */

    ap = victim->alignment;

         if ( ap >  700 ) msg = "$N has an aura as white as the driven snow.";
    else if ( ap >  350 ) msg = "$N is of excellent moral character.";
    else if ( ap >  100 ) msg = "$N is often kind and thoughtful.";
    else if ( ap > -100 ) msg = "$N doesn't have a firm moral commitment.";
    else if ( ap > -350 ) msg = "$N lies to $S friends.";
    else if ( ap > -700 ) msg = "$N's slash DISEMBOWELS you!";
    else msg = "I'd rather just not say anything at all about $N.";

    act( msg, ch, NULL, victim, TO_CHAR, TRUE );
    return;
}



void spell_no_summon( int sn, int level, CHAR_DATA *ch, void *vo )
{
/* another spell by rip, no_summon, but a nasty little penalty */
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if (IS_AFFECTED( victim, AFF_NO_SUMMON ))
        return;

    af.type = sn;
    af.duration = 5;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = AFF_NO_SUMMON;
    affect_to_char( victim, &af );

    send_to_char("You feel very, very heavy.\n\r", victim );
    return;
}



void spell_lightning_bolt( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam		= number_range( 55, 85 );
    if ( saves_spell( level, victim ) && ch->char_class != CLASS_INVOKER )
	dam /= 2;
    if ( victim->char_class == CLASS_BARBARIAN || victim->char_class == CLASS_INVOKER )
	if (saves_spell( level, victim ) )
		dam /= 2;
    if ( victim->char_class == CLASS_MONK)
		dam /= 2;

    damage( ch, victim, dam, sn, victim->resist_lightning );
    return;
}



void spell_locate_object( int sn, int level, CHAR_DATA *ch, void *vo )
{
    char buf[MAX_INPUT_LENGTH];
    char targ2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;
    int counter;
    ObjIterator oiter;

    if ( !can_interact( ch ) )
    {
      send_to_char( "You cannot locate object if you are not in the main war.\n\r", ch );
      return;
    }

    counter = 0;

    number_argument( target_name, targ2 );

    found = FALSE;
    for ( obj = oiter.Start( ch, ObjIterator::WORLD );
          obj != NULL; obj = oiter.Next( ) )
    {
	if (counter > level)
	{
		send_to_char("You feel confused.\n\r", ch );
		break;
	}

/*
	if ( !can_see_obj( ch, obj ) || !is_name( target_name, obj->name ) )
*/
	if ( !can_see_obj( ch, obj ) || !is_name( targ2, obj->name ) )
	    continue;

	if ( obj->carried_by
	&& !can_see( ch, obj->carried_by ) )
	    continue;

	counter += 2;

	found = TRUE;

	for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by != NULL )
	{
	    sprintf( buf, "%s carried by %s.\n\r",
		obj->short_descr, PERS(in_obj->carried_by, ch) );
	}
	else
	{
	    sprintf( buf, "%s in %s.\n\r",
		obj->short_descr, in_obj->in_room == NULL
		    ? "somewhere" : in_obj->in_room->name );
	}

	buf[0] = UPPER(buf[0]);
	send_to_char( buf, ch );
    }

    if ( !found )
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );

    return;
}



void spell_magic_missile( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int dam;
  int number;
  int alive = 1;

  if ( ch->char_class != CLASS_INVOKER )
  {
    dam = number_range( 10, 15 );
    if ( saves_spell( level, victim ) )
      dam /= 2;

    damage( ch, victim, dam, sn, victim->resist_magic );

    return;
  }
    
  number = number_range( 2, 6 );

  send_to_char( "You unleash a volley of missiles!\n\r", ch );
  act( "$n unleashes a volley of missiles!", ch, NULL, NULL, TO_ROOM, TRUE );

  while ( number > 0 && alive )
  {
    number--;
    switch( number_range( 0, 20 ) )
    {
      default:
      case 0:
        dam = number_range( 10, 15 );
        if ( saves_spell( level, victim ) )
          dam /= 2;

        alive = damage( ch, victim, dam, sn, victim->resist_magic );
	break;
      case 1:
        dam = number_range( 30, 50 );
        if ( (victim->char_class == CLASS_ALTERER
	|| victim->char_class == CLASS_BARBARIAN )
        && saves_spell( level, victim ) )
          dam /= 2;

	alive = damage( ch, victim, dam, gsn_burning_hands, victim->resist_fire );
	break;
      case 2:
        dam = number_range( 30, 50 );
        if ( saves_spell( level, victim ) )
          dam /= 2;

        if ( victim->char_class == CLASS_BARBARIAN
	|| victim->char_class == CLASS_ALTERER )
          if (saves_spell( level, victim ) )
            dam /= 2;

        if (!saves_spell(level - 5, victim ) )
        {
          af.type      = gsn_blindness;
          af.location  = APPLY_HITROLL;
          af.modifier  = -4;
          af.duration  = 1;
          af.bitvector = AFF_BLIND;
          affect_join( victim, &af );
          send_to_char( "You have been blinded!\n\r", victim );
          act("$n appears to be blinded.", victim, NULL, NULL, TO_ROOM, TRUE);
        }
        alive = damage( ch, victim, dam, gsn_colour_spray, victim->resist_magic );
	break;
      case 3:
        dam =  70 + dice( 2, 20 );
        if ( saves_spell( level, victim ) && ch->char_class != CLASS_INVOKER )
          dam /= 2;
        if ( victim->char_class == CLASS_BARBARIAN || victim->char_class == CLASS_INVOKER)
          dam /= 2;

        if (victim->char_class == CLASS_MAGE)
          dam /= 2;
        if (victim->char_class == CLASS_BARD)
          dam /= 2;

        alive = damage( ch, victim, dam, gsn_fireball, victim->resist_fire );
	break;
      case 4:
        dam         = number_range( 55, 85 );
        if ( saves_spell( level, victim ) && ch->char_class != CLASS_INVOKER )
          dam /= 2;
        if ( victim->char_class == CLASS_BARBARIAN
	|| victim->char_class == CLASS_INVOKER )
          if (saves_spell( level, victim ) )
            dam /= 2;
        if ( victim->char_class == CLASS_MONK)
          dam /= 2;

        alive = damage( ch, victim, dam, gsn_lightning_bolt, victim->resist_lightning );
	break;
      case 5:
        dam         = number_range( 50, 70 );
        if ( saves_spell( level, victim ) && ch->char_class != CLASS_ALTERER )
          dam /= 2;
        if (victim->char_class == CLASS_BARBARIAN || victim->char_class == CLASS_ALTERER )
	  if (saves_spell(level, victim))
	    dam /= 2;

        alive = damage( ch, victim, dam, gsn_shocking_grasp, victim->resist_lightning );
	break;
      case 6:
        dam = number_range( 15, 25 );
        if ( !saves_spell( level - 4, victim )
	&& ch->char_class != CLASS_NECROMANCER )
        {
          af.type      = gsn_chill_touch;
          af.duration  = 6;
          af.location  = APPLY_STR;
          af.modifier  = -2;
          af.bitvector = 0;
          affect_to_char( victim, &af );
        }
        else
        {
          dam /= 2;
        }

        if (victim->char_class == CLASS_BARBARIAN
	|| victim->char_class == CLASS_NECROMANCER )
          if (saves_spell( level, victim ) )
            dam /= 2;

        alive = damage( ch, victim, dam, gsn_chill_touch, victim->resist_drain );
	break;
    }
  }

  return;
}



void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    AFFECT_DATA af;
    CHAR_DATA *gch;
    CharIterator citer;

  if ( !can_interact( ch ) )
  {
    send_to_char( "You cannot cast mass invis if you are not in the main war.\n\r", ch );
    return;
  }

    for ( gch = citer.Start( ch, CharIterator::ROOM ); gch; gch = citer.Next() )
    {
	if ( IS_AFFECTED(gch, AFF_INVISIBLE) )
	    continue;
	act( "$n slowly fades out of existence.", gch, NULL, NULL, TO_ROOM, TRUE );
	send_to_char( "You slowly fade out of existence.\n\r", gch );
	af.type      = sn;
	af.duration  = 24;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_INVISIBLE;
	affect_to_char( gch, &af );
    }
    send_to_char( "Ok.\n\r", ch );

    return;
}



void spell_null( int sn, int level, CHAR_DATA *ch, void *vo )
{
    send_to_char( "That's not a spell!\n\r", ch );
    return;
}



void spell_mesmerize( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (saves_spell( level, victim ) )
	return;

    af.type      = sn;
    af.duration  = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_join( victim, &af );

    send_to_char( "You lose mental focus!\n\r", victim );
    act( "$n begins to stare at the sky.", victim, NULL,
	NULL, TO_ROOM, TRUE );
    /* 10 second stun */
    stop_fighting( victim, TRUE );
    WAIT_STATE( victim, 40 );

    return;
}


void spell_miracle( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if (IS_AFFECTED( victim, AFF_BLACKMANTLE ))
	return;

    heal_character( victim, victim->max_hit - victim->hit );
    update_pos( victim );

    send_to_char( "You are miraculously restored.\n\r", victim );
    return;
}


void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if ( IS_AFFECTED(victim, AFF_PASS_DOOR) )
	return;
    af.type      = sn;
    af.duration  = level / 4;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PASS_DOOR;
    affect_to_char( victim, &af );
    act( "$n turns translucent.", victim, NULL, NULL, TO_ROOM, TRUE );
    send_to_char( "You turn translucent.\n\r", victim );
    return;
}


void spell_personal_illusion( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  CHAR_DATA *mob;
  char buf[MAX_STRING_LENGTH];

  if ( !can_interact( ch ) )
  {
    send_to_char( "You cannot create illusions if you are not in the main war.\n\r", ch );
    return;
  }

  mob = create_mobile( get_mob_index( 30 ), ch->phase );

  mob->max_hit = 50;
  mob->hit = mob->max_hit;
								                  mob->level = 1;
  mob->gold = 0;
  mob->char_class = 0;

  free_string( mob->name );
  mob->name = str_dup( victim->name );
  free_string( mob->short_descr );
  mob->short_descr = str_dup( victim->name );
  free_string( mob->long_descr );
  sprintf( buf, "(IN_WAR) %s is here.\n\r", mob->short_descr );
  mob->long_descr = str_dup( buf );

  char_to_room( mob, victim->in_room );

  act("$n creates a double of $mself!", victim, NULL, NULL, TO_ROOM, TRUE );
  act("You create an illusion of yourself!", victim, NULL, NULL, TO_CHAR, TRUE );

  return;
}



void spell_phantasmal_weaponry( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN || is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.duration  = 6;
    af.location  = APPLY_HITROLL;
    af.modifier  = 2; 
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "A ghastly fighter seems to shadow your motions.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}




void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( saves_spell( level, victim ) && ch->char_class != CLASS_NECROMANCER )
	return;
    if ( victim->char_class == CLASS_BARBARIAN || victim->char_class == CLASS_NECROMANCER ||
		victim->char_class == CLASS_MONK )
	if ( saves_spell( level, victim ) )
		return;
    af.type      = sn;
    af.duration  = 5;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = AFF_POISON;
    affect_join( victim, &af );
    send_to_char( "You feel very sick.\n\r", victim );
    return;
}



void spell_protection( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN )
	return;

    if ( IS_AFFECTED(victim, AFF_PROTECT) )
	return;
    af.type      = sn;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PROTECT;
    affect_to_char( victim, &af );
    send_to_char( "You feel righteous.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_invulnerability( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( victim->char_class == CLASS_BARBARIAN )
    return;

  if ( IS_AFFECTED(victim, AFF_INVULNERABILITY) )
    return;

  af.type      = sn;
  af.duration  = number_range(20,50);
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = AFF_INVULNERABILITY;
  affect_to_char( victim, &af );
  send_to_char( "You are surrounded by a creamy white sphere.\n\r", victim );

  if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
  return;
}

void spell_psionic_blast( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *vch;
  int dam = 0;
  AFFECT_DATA af_1;
  AFFECT_DATA af_2;
  CharIterator citer;

  act("$n sends a giant blast flaring about $s body!",
    ch, NULL, NULL, TO_ROOM, TRUE );
  act("A giant blast of energy flares about your body!",
    ch, NULL, NULL, TO_CHAR, TRUE );

  af_1.type      = gsn_confusion;
  af_1.duration  = 15;
  af_1.location  = APPLY_INT;
  af_1.modifier  = -1 * number_range( 3, 6 );
  af_1.bitvector = 0;

  af_2.type      = sn;
  af_2.duration  = 3;
  af_2.location  = APPLY_DEX;
  af_2.modifier  = -1 * number_range( 2, 4 );
  af_2.bitvector = 0;

  for ( vch = citer.Start( ch, CharIterator::ROOM );
        vch != NULL; vch = citer.Next() )
  {

    if ( !in_same_war( ch, vch ) )
      continue;

    dam = number_range( 30, 50 );

    if ( vch->in_room != ch->in_room || vch == ch )
      continue;

    if ( saves_spell( level + 9, vch ) )
      continue;

    if ( !saves_spell( level, vch ) )
    {
      move_char( vch, number_range( 0, 5 ), FALSE );
      vch->position = POS_RESTING;
    }

    if ( !saves_spell( level, vch ) )
    {
      WAIT_STATE( vch, 24 );
    }

    if ( !saves_spell( level, vch ) )
      affect_to_char( vch, &af_1 );
    if ( !saves_spell( level, vch ) )
      affect_to_char( vch, &af_2 );

    damage( ch, vch, dam, sn, vch->resist_magic );
  }
  return;
}

void spell_ego_whip( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = number_range( 40, 75 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, victim->resist_magic );

    return;

}


void spell_reflect( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( victim->char_class == CLASS_BARBARIAN )
    return;

  if ( is_affected(victim, gsn_reflect ) )
    return;

  af.type      = gsn_reflect;
  af.duration  = number_range(10,20);
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char( victim, &af );
  send_to_char( "The air around you begins tingling.\n\r", victim );

  if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
  return;
}

void spell_remove_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( ch->char_class == CLASS_BARBARIAN )
	return;

    if ( is_affected( victim, gsn_curse ) )
    {
	affect_strip( victim, gsn_curse );
	send_to_char( "You feel better.\n\r", victim );
    }

    return;
}



void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (victim->char_class == CLASS_BARBARIAN)
	return;

    if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
	return;
    af.type      = sn;
    af.duration  = level / 6;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SANCTUARY;
    affect_to_char( victim, &af );
    act( "$n is surrounded by a white aura.", victim, NULL, NULL, TO_ROOM, TRUE );
    send_to_char( "You are surrounded by a white aura.\n\r", victim );
    return;
}



void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (ch->char_class == CLASS_BARBARIAN)
	return;

    if ( is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.duration = 24;
    af.location  = APPLY_AC;
    af.modifier  = -15;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act( "$n is surrounded by a force shield.", victim, NULL, NULL, TO_ROOM, TRUE );
    send_to_char( "You are surrounded by a force shield.\n\r", victim );
    return;
}



void spell_shocking_grasp( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam		= number_range( 50, 70 );
    if ( saves_spell( level, victim ) && ch->char_class != CLASS_ALTERER )
	dam /= 2;
    if (victim->char_class == CLASS_BARBARIAN || victim->char_class == CLASS_ALTERER )  
                                   
	if (saves_spell(level, victim))
		dam /= 2;

    damage( ch, victim, dam, sn, victim->resist_lightning );
    return;
}



void spell_silence( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (saves_spell( level, victim ) )
	return;

    af.type      = sn;
    af.duration  = 1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_join( victim, &af );

    send_to_char( "You can not speak!\n\r", victim );
    act( "$n clutches $s mouth, unable to speak!", victim, NULL,
	NULL, TO_ROOM, TRUE );

    return;
}


void spell_sleep( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (victim->level > ch->level)
	return;

    if ( IS_AFFECTED(victim, AFF_SLEEP )
    ||   saves_spell( level, victim ) )
	return;

    if (ch->char_class == CLASS_BARBARIAN || ch->char_class == CLASS_ENCHANTER)
	if (saves_spell( level, victim ) )
		return;

  if ( ch->char_class == CLASS_ENCHANTER
  && !saves_spell( level, victim ) )
  {
    af.type      = sn;
    af.duration  = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SLEEP;
    affect_join( victim, &af );
  }

    if ( IS_AWAKE(victim) )
    {
	send_to_char( "You feel very sleepy ..... zzzzzz.\n\r", victim );
	act( "$n goes to sleep.", victim, NULL, NULL, TO_ROOM, TRUE );
	victim->position = POS_SLEEPING;
    }

    return;
}


void spell_slow( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (saves_spell( level, victim ) )
	return;

    af.type      = sn;
    af.duration  = 4;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_join( victim, &af );

    send_to_char( "Your body slows down!\n\r", victim );
    act( "$n's body seems to slow down.", victim, NULL,
	NULL, TO_ROOM, TRUE );

    return;
}



void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (ch->char_class == CLASS_BARBARIAN)
	return;

    if ( is_affected( ch, sn ) )
	return;
    af.type      = sn;
    af.duration = 12;
    af.location  = APPLY_AC;
    af.modifier  = -15;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act( "$n's skin turns to stone.", victim, NULL, NULL, TO_ROOM, TRUE );
    send_to_char( "Your skin turns to stone.\n\r", victim );
    return;
}



void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    extern int war_flags;

    if ( !can_interact( ch ) )
      return;

    if ( ( victim = get_char_phase( ch, target_name ) ) == NULL
    ||   victim == ch
    ||   victim->in_room == NULL
    ||   IS_AFFECTED(victim, AFF_NO_SUMMON)
    ||   victim->level >= level + 3
    ||   victim->fighting != NULL
    ||   (IS_NPC(victim) && saves_spell( level, victim ) ) )
    {
	send_to_char( "You failed.\n\r", ch );
	act("$n just tried to summon you!",ch,NULL,victim,TO_VICT, TRUE);
	return;
    }

    if (!IS_NPC(ch) && !IS_NPC(victim) && !IS_SET(war_flags, WAR_FLAG_SUMMON ))
    {
	send_to_char("Summon is currently limited to mobiles.\n\r", ch );
	return;
    }
 
    if( !IS_NPC(ch) )
    {
	EXIT_DATA *pexit;
	bool found = FALSE;
	int iRoom;

	for( iRoom = 0;iRoom <= 5;iRoom++ )
	{
	   if((pexit = ch->in_room->exit[iRoom]) != NULL 
	   && pexit->to_room != NULL
	   && !is_door_closed( pexit, ch ) )
		found = TRUE; 

	}
    
	if( !found )
	{
		send_to_char("No summoning in noexit rooms.\n\r", ch );
		return;
	}
    }


    act( "$n disappears suddenly.", victim, NULL, NULL, TO_ROOM, TRUE );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act( "$n arrives suddenly.", victim, NULL, NULL, TO_ROOM, TRUE );
    act( "$n has summoned you!", ch, NULL, victim,   TO_VICT, TRUE );
    do_look( victim, "auto" );
    return;
}



void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *pRoomIndex;

    if (ch->char_class == CLASS_BARBARIAN || ch->char_class == CLASS_ALTERER)
			       
	if (saves_spell( level, victim ) && ch != victim )
		return;

    if ( in_private_war( victim ) )
    {
      send_to_char( "Teleports disabled in private wars.\n\r", victim );
      return;
    }

  if (!IS_NPC(ch))
    if ( victim->in_room == NULL
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
/* tele fighting people!! */
/*
    || ( !IS_NPC(ch) && victim->fighting != NULL )
*/
    || ( victim != ch
    && ( saves_spell( level, victim ) || saves_spell( level, victim ) ) ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    for ( ; ; )
    {
	pRoomIndex = get_room_index( number_range( 0, 65535 ) );
	if ( pRoomIndex == NULL )
	    continue;

	if ( !IS_SET(pRoomIndex->area->area_flags, AREA_DISABLED ) )
	    break;
    }

    act( "$n slowly fades out of existence.", victim, NULL, NULL, TO_ROOM, TRUE );
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    act( "$n slowly fades into existence.", victim, NULL, NULL, TO_ROOM, TRUE );
    send_to_char("The world spins before your eyes, poof!\n\r", victim );
    do_look( victim, "auto" );
    return;
}

void spell_tower_of_id( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if( IS_AFFECTED( ch, AFF_TOWERID ) ) 
    {
	send_to_char( "You already are using the tower of id!", ch );
	return;
    }

    act( "$n starts to waver before your eyes.", victim, NULL, NULL, TO_ROOM, TRUE );

    af.type      = sn;
    af.duration  = 5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_TOWERID;
    affect_to_char( victim, &af );

    send_to_char( "You create an almost invisible shield around you.\n\r", victim );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );

    return;
}

void spell_ventriloquate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char speaker[MAX_INPUT_LENGTH];
    CHAR_DATA *vch;
    CharIterator citer;

    if ( !can_interact( ch ) )
    {
      send_to_char( "You cannot ventriloquate if you are not in the main war.\n\r", ch );
      return;
    }

    target_name = one_argument( target_name, speaker );

    sprintf( buf1, "%s says '%s'.\n\r",              speaker, target_name );
    sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
    buf1[0] = UPPER(buf1[0]);

    for ( vch = citer.Start( ch, CharIterator::ROOM ); vch; vch = citer.Next() )
    {
	if ( !is_name( speaker, vch->name ) )
/* um, taking out the save */
/*
	    send_to_char( saves_spell( level, vch ) ? buf2 : buf1, vch );
*/
            send_to_char( buf1, vch );
    }

    return;
}



void spell_void_aura( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN || is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.duration  = 6;
    af.location  = APPLY_REGEN_MANA;
    af.modifier  = -200;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "An aura of nothingness surrounds your body.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( ch->char_class != CLASS_ENCHANTER
    && saves_spell( level, victim ) )
	return;

    if ( is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.duration  = level/2;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel weaker.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}


void spell_weapon_immunity( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->char_class == CLASS_BARBARIAN || is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.duration  = 1;
    af.location  = APPLY_RESIST_PHYSICAL;
    af.modifier  = 50;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "A metallic aura surrounds your body.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}


void spell_whirling_wind( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam;

  dam = dice(8, 8);
  if ( saves_spell( level, victim ) )
    dam /= 2;
  if ( victim->char_class == CLASS_BARBARIAN
  && saves_spell( level, victim ) )
    dam /= 2;

  if ( is_affected( victim, gsn_fly ) )
  {
    dam *= 2;
    affect_strip( victim, gsn_fly );
  }

  if ( IS_AFFECTED( victim, AFF_FLYING ) )
  {
    dam *= 2;
    REMOVE_BIT( victim->affected_by, AFF_FLYING );
  }

  damage( ch, victim, dam, sn, victim->resist_physical );
  return;
}




void spell_wizard_eye( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim;
  void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch );

  if ( ( victim = get_char_phase( ch, target_name ) ) == NULL
  ||   victim == ch
  ||   victim->in_room == NULL
  ||   victim->level > level )
  {
    send_to_char( "You failed.\n\r", ch );
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("Wizard Eye is currently limited to players.\n\r", ch );
    return;
  }

  send_to_char( "You sense an eye staring at you from above.\n\r", victim );

  if ( saves_spell( level, victim ) )
  {
    send_to_char("Your view of them is too blurry to make out details.\n\r",ch);
    return;
  }

  show_char_to_char_1( victim, ch );
  return;
}


/*
 *  word_of_recall by Rip
 */
void spell_word_of_recall( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vict2;
    ROOM_INDEX_DATA *location;

    if ( ( location = get_room_index( ROOM_VNUM_TEMPLE_OF_YS ) ) == NULL )
    {
	send_to_char("Odd, can't seem to find the temple.\n\r", ch );
	return;
    }

    if (victim->in_room == location || IS_NPC(victim))
	return;

    if ((vict2 = victim->fighting) != NULL )
	stop_fighting( victim, TRUE );

    act("$n disappears.", victim, NULL, NULL, TO_ROOM, TRUE );
    char_from_room( victim );
    char_to_room( victim, location );
    act("$n appears in the middle of the room.", victim, NULL, NULL, TO_ROOM, TRUE );
    do_look( victim, "auto" );

    return;
}



/*
 * NPC spells.
 */
void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj_lose;
    int dam;
    int hpch;
    ObjIterator oiter;

    if ( number_percent( ) < 20 * level && !saves_spell( level, victim ) )
    {
        for ( obj_lose = oiter.StartList( ch, victim->carrying );
              obj_lose != NULL; obj_lose = oiter.Next( ) )
	{
	    int iWear;

	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    case ITEM_ARMOR:
	    case ITEM_JEWELRY:
		if ( obj_lose->value[0] > 0 )
		{
		    act( "$p is pitted and etched!",
			victim, obj_lose, NULL, TO_CHAR, TRUE );
		    if ( ( iWear = obj_lose->wear_loc ) != WEAR_NONE )
			victim->armor -= apply_ac( obj_lose, iWear );
		    obj_lose->value[0] -= 1;
		    obj_lose->cost      = 0;
		    if ( iWear != WEAR_NONE )
			victim->armor += apply_ac( obj_lose, iWear );
		}
		break;

	    case ITEM_POTION:
	    case ITEM_DRINK_CON:
	    case ITEM_CONTAINER:
		act( "$p fumes and dissolves!",
		    victim, obj_lose, NULL, TO_CHAR, TRUE );
		extract_obj( obj_lose, "spell_acid_breath" );
		break;
	    }
	}
    }

    hpch = UMAX( 10, ch->hit );
    dam  = number_range( hpch/16+1, hpch/8 );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, victim->resist_magic );
    return;
}



void spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj_lose;
    int dam;
    int hpch;
    static char str_spell_fire_breath[18] = "spell_fire_breath";
    ObjIterator oiter;

    if ( number_percent( ) < 20 * level && !saves_spell( level, victim ) )
    {
        for ( obj_lose = oiter.StartList( ch, victim->carrying );
              obj_lose != NULL; obj_lose = oiter.Next( ) )
	{
	    char *msg;

	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    default:             continue;
	    case ITEM_CONTAINER: msg = "$p ignites and burns!";   break;
	    case ITEM_DRINK_CON: msg = "$p bubbles and boils!";   break;
	    case ITEM_POTION:    msg = "$p bubbles and boils!";   break;
	    case ITEM_SCROLL:    msg = "$p crackles and burns!";  break;
	    case ITEM_STAFF:     msg = "$p smokes and chars!";    break;
	    case ITEM_WAND:      msg = "$p sparks and sputters!"; break;
	    case ITEM_FOOD:      msg = "$p blackens and crisps!"; break;
	    case ITEM_PILL:      msg = "$p melts and drips!";     break;
	    }

	    act( msg, victim, obj_lose, NULL, TO_CHAR, TRUE );
	    extract_obj( obj_lose, str_spell_fire_breath );
	}
    }

    hpch = UMAX( 16, ch->hit );
    dam  = number_range( hpch/16+1, hpch/8 );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, victim->resist_fire );
    return;
}



void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj_lose;
    int dam;
    int hpch;
    char str_spell_frost_breath[19] = "spell_frost_breath";
    char str_freezes_and_shatters[25] = "$p freezes and shatters!";
    ObjIterator oiter;

    if ( number_percent( ) < 40 * level && !saves_spell( level, victim ) )
    {
        for ( obj_lose = oiter.StartList( ch, victim->carrying );
              obj_lose != NULL; obj_lose = oiter.Next( ) )
	{
	    char *msg;

	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    default:            continue;
	    case ITEM_CONTAINER:
	    case ITEM_DRINK_CON:
	    case ITEM_POTION:   msg = str_freezes_and_shatters; break;
	    }

	    act( msg, victim, obj_lose, NULL, TO_CHAR, TRUE );
	    extract_obj( obj_lose, str_spell_frost_breath );
	}
    }

    hpch = UMAX( 16, ch->hit );
    dam  = number_range( hpch/16+1, hpch/8 );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, victim->resist_cold );
    return;
}



void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    int dam;
    int hpch;
    CharIterator citer;

    for ( vch = citer.Start( ch, CharIterator::ROOM ); vch; vch = citer.Next() )
    {

	if ( ch == vch )
	    continue;

	if ( in_same_war( ch, vch ) )
	{
	    hpch = UMAX( 16, ch->hit );
	    dam  = number_range( hpch/3+1, hpch/2 );
	    if ( saves_spell( level, vch ) )
		dam /= 2;
	    damage( ch, vch, dam, sn, vch->resist_magic );
	}
    }
    return;
}



void spell_lightning_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    int hpch;

    hpch = UMAX( 10, ch->hit );
    dam = number_range( hpch/4+1, hpch/3 );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, victim->resist_lightning );
    return;
}

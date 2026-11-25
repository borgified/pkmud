
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
 *                                                                         *
 *    And Rip's Mods (c) 1994 by Robert L. Peckham....                     *
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



/*
 * The following special functions are available for objects.
 */
DECLARE_OBJECT_FUN(	spec_caltrops		);
DECLARE_OBJECT_FUN(	spec_cleric_biter	);
DECLARE_OBJECT_FUN(	spec_dragon_slayer	);
DECLARE_OBJECT_FUN(	spec_energy_drain	);
DECLARE_OBJECT_FUN(	spec_entangle		);
DECLARE_OBJECT_FUN(	spec_fireball_weapon	);
DECLARE_OBJECT_FUN(	spec_frost_blade	);
DECLARE_OBJECT_FUN(	spec_lightning_weapon	);
DECLARE_OBJECT_FUN(	spec_poison_weapon	);
DECLARE_OBJECT_FUN(	spec_regenerater	);
DECLARE_OBJECT_FUN(	spec_snare		);
DECLARE_OBJECT_FUN(	spec_stun_weapon	);
DECLARE_OBJECT_FUN(	spec_trap		);
DECLARE_OBJECT_FUN(	spec_vampiric_weapon	);
DECLARE_OBJECT_FUN(	spec_xyzzy		);


OBJECT_FUN *object_fun_lookup( const char *name )
{
    if ( !str_cmp( name, "spec_caltrops"	 ) ) return spec_caltrops;
    if ( !str_cmp( name, "spec_cleric_biter"     ) ) return spec_cleric_biter;
    if ( !str_cmp( name, "spec_dragon_slayer"    ) ) return spec_dragon_slayer;
    if ( !str_cmp( name, "spec_energy_drain"     ) ) return spec_energy_drain;
    if ( !str_cmp( name, "spec_entangle"         ) ) return spec_entangle;
    if ( !str_cmp( name, "spec_fireball_weapon"  ) )
                                                   return spec_fireball_weapon;
    if ( !str_cmp( name, "spec_frost_blade"      ) ) return spec_frost_blade;
    if ( !str_cmp( name, "spec_lightning_weapon" ) )
					          return spec_lightning_weapon;
    if ( !str_cmp( name, "spec_poison_weapon"    ) ) return spec_poison_weapon;
    if ( !str_cmp( name, "spec_regenerater"      ) ) return spec_regenerater;
    if ( !str_cmp( name, "spec_snare"            ) ) return spec_snare;
    if ( !str_cmp( name, "spec_stun_weapon"      ) ) return spec_stun_weapon;
    if ( !str_cmp( name, "spec_trap"		 ) ) return spec_trap;
    if ( !str_cmp( name, "spec_vampiric_weapon"	 ) )
                                                   return spec_vampiric_weapon;
    if ( !str_cmp( name, "spec_xyzzy"            ) ) return spec_xyzzy;
    return 0;
}

bool spec_trap( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument ) 
{
  if( IS_NPC(ch) )
    return TRUE;

  if( obj->value[1] == FALSE )
    return TRUE;

  if( str_cmp(command, "called_from_move_char" ))
    return TRUE;

  if( obj->killer != NULL && obj->killer == ch )
  {
    send_to_char(
       "You remember a trap you placed here earlier and avoid it.\n\r", ch );
    return TRUE;
  }

  if( number_percent() < ch->pcdata->learned[gsn_find_traps] )
  {
    send_to_char("You barely avoid a deadly trap.\n\r", ch );
      return TRUE;
  }

  send_to_char("*snatch* You step right into a trap!\n\r", ch );
  act("$n steps right into a trap!",ch,NULL,NULL,TO_ROOM, TRUE);

  if( obj->killer != NULL )
  {
    if( obj->value[0] != 0 )
      obj_cast_spell( obj->value[0], obj->killer->level,
        obj->killer, ch, NULL );
    else
    {
      send_to_char("It envelops you in flames!!\n\r", ch );
      damage( obj->killer, ch, number_range(10,20), 0, ch->resist_fire );
    }
    if(obj->in_room != NULL)
    {
      obj_from_room( obj, "spec_trap" );
      extract_obj(obj, "spec_trap" );
    }
    if(obj->carried_by != NULL)
    {
      obj_from_char(obj);
      extract_obj(obj, "spec_trap" );
    }
    return TRUE;
  }
  else
  {
    if( obj->value[0] != 0 )
      obj_cast_spell( obj->value[0], ch->level, ch, ch, NULL );
    else
    {
      send_to_char("It envelops you in flames!!\n\r", ch );
      damage(ch, ch, number_range(10,20), 0, ch->resist_fire );
    }

    if(obj->in_room != NULL)
    {
      obj_from_room( obj, "spec_trap" );
      extract_obj(obj, "spec_trap" );
    }
    if(obj->carried_by != NULL)
    {
      obj_from_char(obj);
      extract_obj(obj, "spec_trap" );
    }
    return TRUE;
  }

  return TRUE;
}	

bool spec_xyzzy( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
	if (!str_cmp( "xyzzy", command ) )
	{
		char_from_room( ch );
		char_to_room( ch, get_room_index( 1200 ) );
		do_look( ch, "" );
		extract_obj( obj, "spec_xyzzy" );
		return FALSE;
	}
	return TRUE;
}


bool spec_regenerater( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
  if (obj->wear_loc == WEAR_NONE)
    return TRUE;

  if (str_cmp( command, "called_from_char_update" ) )
    return TRUE;

  ch->hit = UMIN(ch->hit+obj->value[0],ch->max_hit);
  ch->mana = UMIN(ch->mana+obj->value[1],ch->max_mana);

  return TRUE;
}

bool spec_caltrops( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
  if( !mortal_in_main_war( ch ) )
    return TRUE;

  if( obj->in_room == NULL )
    return TRUE;

  if ( obj->wear_flags )
    return TRUE;

  if( ch == obj->killer )
    return TRUE;

  if( str_cmp( command, "called_from_move_char" ) )
    return TRUE;

  if( obj->value[1] == FALSE )
    return TRUE;

  if( IS_AFFECTED( ch, AFF_FLYING ) )
    return TRUE;

  if( number_percent( ) < obj->value[0]*20 )
  {
    act("OUCH!  You step right into a very sharp caltrop!",
      ch, NULL, NULL, TO_CHAR, TRUE );
    act("$n goes 'OUCH!' as $s steps right into a caltrop!",
      ch, NULL, NULL, TO_ROOM, TRUE );

    if( obj->killer != NULL )
    {
      damage( obj->killer, ch, number_range( 10, 15 ), 0, ch->resist_physical );
      stop_fighting( ch, FALSE );
    }
    else
      damage( ch, ch, number_range( 10, 15 ), 0, ch->resist_physical );

    obj->value[0]--;

    if( obj->value[0] == 0 )
    {
      obj_from_room( obj, "spec_caltrops" );
      extract_obj( obj, "spec_caltrops" );
    }

    return TRUE;
  }
  send_to_char( "You manage to avoid some deadly caltrops.\n\r", ch );
  return TRUE;
}


bool spec_snare( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
	if ( !mortal_in_main_war( ch ) )
	    return TRUE;

	if (obj->carried_by != NULL)
		return TRUE;

	if (obj->in_room == NULL)
		return TRUE;

	if (str_cmp( command, "called_from_move_char" ) )
		return TRUE;

	if ( IS_AFFECTED( ch, AFF_FLYING ) )
		return TRUE;

	act("$n trips over a snare and falls flat on $s face.", ch, NULL, NULL, TO_ROOM, TRUE );
	send_to_char("You trip over a snare and fall flat on your face.\n\r", ch );
	ch->position = POS_RESTING;
	WAIT_STATE ( ch, 2 * PULSE_VIOLENCE );
	obj_from_room( obj, "spec_snare" );
	extract_obj( obj, "spec_snare" );

	return TRUE;
}


bool spec_dragon_slayer( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
    CHAR_DATA *victim;

    if (str_cmp( command, "called_from_multi_hit" ) )
	return TRUE;

    if ( (victim = ch->fighting )== NULL)
	return TRUE;

    if (is_name( "dragon", victim->name )
    || ( !IS_NPC( victim ) && victim->char_class == CLASS_DRAGON ) )
    {
	if (number_bits(4))
	    return TRUE;

	act("$p smashes the dragon $N into smithereens.", ch, obj, ch->fighting, TO_NOTVICT, TRUE );
	act("$p smashes you into smithereens.", ch, obj, ch->fighting, TO_VICT, TRUE );
	act("$p reaches out and smashes $N.", ch, obj, ch->fighting, TO_CHAR, TRUE );
	damage( ch, ch->fighting, (dice(10 , 10)), 0, ch->resist_magic );
    }

    return TRUE;
}


bool spec_fireball_weapon( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
  if (str_cmp( command, "called_from_multi_hit" ) )
    return TRUE;

  if (ch->fighting == NULL)
    return TRUE;

  if (number_bits(3))
    return TRUE;

  act("A burst of flame explodes from $p as it strikes $N.",
    ch, obj, ch->fighting, TO_CHAR, TRUE );
  act("A burst of flame explodes from $n's $p it strikes you.",
    ch, obj, ch->fighting, TO_VICT, TRUE );
  obj_cast_spell( gsn_fireball, ch->level, ch, ch->fighting, NULL );

  return TRUE;
}


bool spec_lightning_weapon( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
  if (str_cmp( command, "called_from_multi_hit" ) )
    return TRUE;

  if (ch->fighting == NULL)
    return TRUE;

  if (number_bits(2))
    return TRUE;

  act("$p glows as bolts of lightning arc towards $N.",
    ch, obj, ch->fighting, TO_CHAR, TRUE );
  act("$n's $p glows as bolts of lightning arc towards you.",
    ch, obj, ch->fighting, TO_VICT, TRUE );
  obj_cast_spell( gsn_lightning_bolt, ch->level, ch, ch->fighting, NULL );

  return TRUE;
}


bool spec_poison_weapon( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
  if (str_cmp( command, "called_from_multi_hit" ) )
    return TRUE;

  if (ch->fighting == NULL)
    return TRUE;

  if (number_bits(4))
    return TRUE;

  act("$p makes small cuts in $N's skin.",
    ch, obj, ch->fighting, TO_CHAR, TRUE );
  act("You feel a stinging sensation as $n's $p cuts your skin.",
    ch, obj, ch->fighting, TO_VICT, TRUE );
  obj_cast_spell( gsn_poison, ch->level, ch, ch->fighting, NULL );

  return TRUE;
}


bool spec_vampiric_weapon( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
  if (str_cmp( command, "called_from_multi_hit" ) )
    return TRUE;

  if (ch->fighting == NULL)
    return TRUE;

  if (number_bits(2))
    return TRUE;

  act("$p draws $N's life force towards you.",
    ch, obj, ch->fighting, TO_CHAR, TRUE );
  act("$n glows red as $s $p draws away your life forces.",
    ch, obj, ch->fighting, TO_VICT, TRUE );
  obj_cast_spell( gsn_vampiric_touch, ch->level, ch, ch->fighting, NULL );

  return TRUE;
}


bool spec_stun_weapon( OBJ_DATA *obj, CHAR_DATA *ch,
  char *command, char *argument )
{
  if (str_cmp( command, "called_from_multi_hit" ) )
    return TRUE;

  if (ch->fighting == NULL)
    return TRUE;

  if (number_bits(3))
    return TRUE;

  act("$p glows as it slams into $N.",
    ch, obj, ch->fighting, TO_CHAR, TRUE );
  act("You feel lethargic as $n's $p smashes into your body.",
    ch, obj, ch->fighting, TO_VICT, TRUE );
  /* 3 second wait state */
  WAIT_STATE( ch->fighting, 12 );
  ch->fighting->position = POS_RESTING;
  stop_fighting( ch->fighting, FALSE );

  return TRUE;
}



bool spec_cleric_biter( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
	if (str_cmp( command, "called_from_multi_hit" ) )
		return TRUE;

	if (ch->fighting == NULL)
		return TRUE;

	if (!IS_AFFECTED(ch->fighting, AFF_SANCTUARY))
		return TRUE;

		if (number_bits(3))
			return TRUE;

		act("$p fires a blast of black fire toward $N.", ch, obj, ch->fighting, TO_CHAR, TRUE );
		act("You are corroded by a blast of black fire from $n's $p.", ch, obj, ch->fighting, TO_VICT, TRUE );
		act("$n's $p fires a blast of black fire toward $N.", ch, obj, ch->fighting, TO_NOTVICT, TRUE );
	obj_cast_spell( gsn_dispel_magic, ch->level, ch, ch->fighting, NULL );
	obj_cast_spell( gsn_blackmantle, ch->level, ch, ch->fighting, NULL );

	return TRUE;
}

bool spec_frost_blade( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
    if (str_cmp( command, "called_from_multi_hit" ) )
	return TRUE;

    if (ch->fighting == NULL)
	return TRUE;

    if (!IS_AFFECTED(ch->fighting, AFF_FIRESHIELD))
	return TRUE;

    if (number_bits(3))
	return TRUE;

    act("$p emits a cone of ice towards $N.", ch, obj, ch->fighting, TO_CHAR, TRUE );
    act("You are blasted by a cone of ice from $n's $p.", ch, obj, ch->fighting, TO_VICT, TRUE );
    act("$n's $p emits a cone of ice towards $N.", ch, obj, ch->fighting, TO_NOTVICT, TRUE );
    obj_cast_spell( gsn_dispel_magic, ch->level, ch, ch->fighting, NULL );
    obj_cast_spell( gsn_frost_breath, ch->level, ch, ch->fighting, NULL );

    return TRUE;
}


bool spec_energy_drain( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
  if (str_cmp( command, "called_from_multi_hit" ) )
    return TRUE;

  if (ch->fighting == NULL)
    return TRUE;

  if (number_bits(3))
    return TRUE;

  obj_cast_spell( gsn_energy_drain, ch->level, ch, ch->fighting, NULL );

  ch->mana = UMIN( ch->mana, ( ch->max_mana + 100 ) );

  return TRUE;
}


bool spec_entangle( OBJ_DATA *obj, CHAR_DATA *ch, char *command, char *argument )
{
  if (str_cmp( command, "called_from_multi_hit" ) )
    return TRUE;

  if (ch->fighting == NULL)
    return TRUE;

  if (number_bits(4))
    return TRUE;

  obj_cast_spell( gsn_entangle, ch->level, ch, ch->fighting, NULL );

  return TRUE;
}


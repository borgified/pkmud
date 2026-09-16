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
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "cmod.h"
#include "global.h"
#include "ObjIterator.h"


/*
 * Local functions.
 */
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	dam_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	kill_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	make_corpse	args( ( CHAR_DATA *ch ) );
void	one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	raw_kill	args( ( CHAR_DATA *victim ) );
void	disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	bash		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

int	pc_alive	args( ( void ) );
int	has_blood	args( ( CHAR_DATA *ch ) );

/*
 * global to prevent linked list corruption from strength items falling
 * off before weapons.
 */
int	making_corpse = FALSE;



/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	ch_next	= ch->next;

	if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
	    continue;

	if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	else
	    stop_fighting( ch, FALSE );

	if ( ( victim = ch->fighting ) == NULL )
	    continue;
    }

    return;
}

/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int     chance;
    OBJ_DATA *obj;

    if ( ch->in_room
    && ch->in_room->smoke > 0 )
    {
      reduce_smoke( ch, 1 );
    }

    obj = get_eq_char( ch, WEAR_WIELD );
    if ( obj != NULL && obj->item_type != ITEM_WEAPON )
      obj = NULL;

    if ( is_affected( ch, gsn_phantasmal_weaponry )
         && dt == TYPE_UNDEFINED )
    {
      int df = TYPE_HIT;

      if ( obj != NULL )
        df += obj->value[3];

      dam_message( ch, victim, 10 + dice( 1, 30 ), df );
    }

    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_ILLUSION ) )
    {
      int df = TYPE_HIT;

      if ( obj != NULL )
        df += obj->value[3];

      dam_message( ch, victim, 10 + dice( 1, 30 ), df );
      dam_message( ch, victim, 10 + dice( 1, 30 ), df );
      return;
    }

    one_hit( ch, victim, dt );
    if ( ch->fighting != victim )
	return;

    if ( obj != NULL && obj->special != 0 )
	(*obj->special) (obj, ch, "called_from_multi_hit", str_null );

    if ( ch->fighting != victim )
	return;

    if ( is_affected( ch, gsn_slow ) )
	return;

    chance = IS_NPC(ch) ? 0 : ch->pcdata->learned[gsn_second_attack];
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt );
	if ( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC(ch) ? 0 : ch->pcdata->learned[gsn_third_attack];
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt );
	if ( ch->fighting != victim )
	    return;
    }

    chance = 0;
    if ( number_percent( ) < chance )
	one_hit( ch, victim, dt );
    
    return;
}



/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *wield;
    int victim_ac;
    int dam;
    int thac0;
    int diceroll;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    if ( is_affected( victim, gsn_mesmerize ) )
        return;

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, WEAR_WIELD );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
    }


/* have to put this in from below since i zap it for mobs */
    thac0     = IS_NPC(ch) ? 10 :
        class_table[ch->char_class].thac0 - GET_HITROLL(ch);


    victim_ac = UMAX( -25, GET_AC(victim) / 10 );
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

/* help out backstab again.. */
    if ( dt == gsn_backstab )
	victim_ac += 4;

    if ( victim->position < POS_STANDING )
	victim_ac += 4;

    if ( victim->position < POS_RESTING )
	victim_ac += 4;

    if (victim->position < POS_SLEEPING )
	victim_ac += 20;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if( !IS_NPC(ch) && ch->char_class == CLASS_MONK )
	thac0 -= 4;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt, 0 );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC(ch) )
    {
	dam = dice( 1, ch->level );
        if (wield != NULL )
	{
            dam = dice( wield->value[1], wield->value[2] );
	    if( IS_SET(wield->extra_flags, ITEM_RANGED_WEAPON) )
		dam /= 2;
	}
    }
    else if (ch->char_class == CLASS_MONK)
    {
	dam = dice( 1, 10 ) + 5;
	if (wield != NULL)
	    dam = dice( wield->value[1], wield->value[2] );
    }
    else
    {
	if ( wield != NULL )
	{
            dam = dice( wield->value[1], wield->value[2] );

	    if( IS_SET(wield->extra_flags, ITEM_RANGED_WEAPON) )
		dam /= 2;
	}
	else
	    dam = number_range( 1, 4 );
    }

    /*
     * Bonuses.
     */

/* modify to standard diku mobs */
    if ( !IS_NPC(ch))
	dam += GET_DAMROLL(ch);

    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
	dam += dam * ch->pcdata->learned[gsn_enhanced_damage] / 100;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( dt == gsn_backstab )
    {
	dam *= (2 + (ch->level / 8) + ((ch->char_class == CLASS_ASSASSIN) ? 2 : 0 ));
	if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_backstab] < 25 )
	    dam /= 2;
    }

    if ( IS_NPC( ch ) && ch->char_class == CLASS_ASSASSIN )
	dam += dam
	  * class_table[CLASS_ASSASSIN].skill_percent[gsn_enhanced_damage]/100;

    if ( wield == NULL || !IS_OBJ_STAT( wield, ITEM_MAGIC )
    || victim->resist_physical > victim->resist_magic )
    {
      if ( victim->resist_physical > 0 )
        dam -= victim->resist_physical;
    }
    else
    {
      if ( victim->resist_magic > 0 )
        dam -= victim->resist_magic;
    }

    if ( dam <= 0 )
	dam = 1;

/* add fireshield here, as it only affects physical attacks */
    /* fireshield can no longer kill NPCs. bug fix? */
    if (IS_AFFECTED( victim, AFF_FIRESHIELD)
    && ( !IS_NPC( ch ) || dam < ch->hit ) )
    {
      damage( victim, ch, dam, gsn_fireshield, victim->resist_fire );
    }

/* trying to fix the fireshield bug.. Bruce was here */
    if ( get_eq_char( ch, WEAR_WIELD ) == NULL )
	damage( ch, victim, dam, TYPE_HIT, 0 );
    else
	damage( ch, victim, dam, dt, 0 );

    if ( wield != NULL
    && get_eq_char( ch, WEAR_WIELD ) != NULL
    && is_affected( victim, gsn_void_aura ) )
    {
      act("$n's weapon vanishes as it strikes $N.",
	ch, NULL, victim, TO_NOTVICT, TRUE );
      act("$n's weapon vanishes as it strikes you.",
	ch, NULL, victim, TO_VICT, TRUE );
      act("Your weapon vanishes as it strikes $N.",
	ch, NULL, victim, TO_CHAR, TRUE );
      unequip_char( ch, wield );
      obj_from_char( wield );
      extract_obj( wield, "one_hit" );
      affect_strip( victim, gsn_void_aura );
    }

    tail_chain( );
    return;
}

/*
 * Inflict damage from a hit.
 * returns TRUE if victim still alive, FALSE if dead.
 */
bool damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int resist )
{
    extern int war_flags;
    extern int war_death;
    int i;

    if ( victim->position == POS_DEAD )
	return FALSE;

    /* stop them from attacking if no war or already dead */
    if ( !in_same_war( ch, victim )
    && !IS_AFFECTED( victim, AFF_FIRESHIELD ) )
    {
         send_to_char( "Either there is no war on or you are already dead!\n\r", ch );
         return FALSE;
    }

    /* stop them from attacking people already dead! */
    if ( !IS_NPC( ch ) && !in_same_war( ch, victim )
    && !IS_AFFECTED( victim, AFF_FIRESHIELD ) )
    {
         send_to_char( "You can't kill a dead being!!\n\r", ch );
         return FALSE;
    }

    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 1000 )
    {
	bug( "Damage: %d: more than 1000 points!", dam );
	dam = 1000;
    }

    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return FALSE;

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );

	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
	{
	    affect_strip( ch, gsn_invis );
	    affect_strip( ch, gsn_mass_invis );
	    REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
	    act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM, TRUE );
	}

        if ( is_affected( ch, gsn_mesmerize ) )
	    affect_strip( ch, gsn_mesmerize );

	/*
	 * Damage modifiers.
	 */
	if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
	    dam /= 2;

	if ( IS_AFFECTED(victim, AFF_TOWERID ) )
	    dam -= 10;

	if ( get_curr_con( victim ) > 16 )
	    dam -= ( get_curr_con( victim ) - 16 );

	if ( IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) )
	    dam -= dam / 4;

	if( is_affected(victim, gsn_invulnerability ) )
	{
	    affect_strip( victim, gsn_invulnerability );
	    dam = 0;
	    act("$N's powerful shield disperses.", ch, NULL, victim, TO_ROOM, TRUE );
	    send_to_char("Your powerful shield disperses.\n\r", victim );
	}

	if ( resist > 0 )
	    dam -= resist;

	if ( dam < 0 )
	    dam = 0;

	/*
	 * Check for disarm, bash, parry, and dodge.
	 */
	if ( dt >= TYPE_HIT )
	{
	switch( number_bits( 2 ) )
	{
	    case 1:
	    if ( IS_NPC(ch) && number_percent( ) < ch->level / 2
	    && ch->spec_fun == NULL )
		disarm( ch, victim );
            case 2:
	    if ( IS_NPC(ch)
	    && number_percent( ) < ch->level / 2
	    && ch->spec_fun == NULL )
		bash( ch, victim );
            case 3:
	    if ( check_parry( ch, victim ) )
		return TRUE;
            case 4:
            default:
	    if ( check_dodge( ch, victim ) )
		return TRUE;
	}
	}

    }

    if (dt != 0)
      dam_message( ch, victim, dam, dt );


    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;

    if ( !IS_NPC( victim ) )
	set_join_status( victim );

    if ( mortal_in_main_war( ch )
    && mortal_in_main_war( victim ) )
    {
	ch->pcdata->damage_inflicted += dam;
	victim->pcdata->damage_received += dam;
    }

    if ( !IS_NPC(victim) )
    {
      if ( victim->level >= LEVEL_IMMORTAL
      &&   victim->hit < 1 )
	victim->hit = 1;

      if ( IS_NPC( ch )
      && ( !ch->hunting || ch->hunting != victim )
      && victim->hit < 1 )
	victim->hit = 1;
    }
    else
    {
      if ( victim->pIndexData
      && victim->pIndexData->pShop
      && victim->hit < 1 )
	victim->hit = 1;
    }

    update_pos( victim );

    if ( !in_same_war( ch, victim ) )
	stop_fighting( ch, TRUE );

/* trying to fix fs bug... Bruce was here  */
    if ( !IS_NPC(ch)
    && !in_main_war( victim )
    && IS_AFFECTED(victim,AFF_FIRESHIELD)
    && victim->position != POS_DEAD
    && !IS_SET( war_flags, WAR_FLAG_BLOODS ) )
    {
       victim->hit = victim->max_hit;
       victim->mana = victim->max_mana;
    }

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM, TRUE );
	send_to_char( 
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM, TRUE );
	send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM, TRUE );
	send_to_char("You are stunned, but will probably recover.\n\r",
	    victim );
	break;

    case POS_DEAD:
	act( "$n is dead! R.I.P.", victim, 0, 0, TO_ROOM, TRUE );
	send_to_char( "You are dead! R.I.P.\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "That really did HURT!\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "You sure are BLEEDING!\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

  /*
   * Payoff for killing things.
   */
  if ( victim->position == POS_DEAD )
  {
    victim->fighting = NULL;

    if ( in_main_war( victim )
    && ( !IS_NPC(victim)
       || ( victim->pIndexData != NULL
         && victim->pIndexData->vnum >= 10
          && victim->pIndexData->vnum <= 19 ) ) )
    {
      char buf[MAX_STRING_LENGTH];
      int ratio, bottom;

      sprintf( log_buf, "%s killed by %s at %d",
        victim->name,
        (IS_NPC(ch) ? ch->short_descr : ch->name),
        victim->in_room->vnum );
      log_string( log_buf );

      if ( IS_NPC( victim ) )
        ratio = 0;
      else
      {
        ratio = victim->pcdata->doomsdays * 100;
        bottom = victim->pcdata->wins + victim->pcdata->deaths;
        bottom = bottom?bottom:1;
        ratio /= bottom;
      }

      if ( !IS_NPC( ch ) && ch->pcdata->rating > 1200
      && !IS_NPC( victim )
      && ( ch->pcdata->rating - victim->pcdata->rating > 400 ) )
      {
        sprintf( buf, "**        %s FARMED by %s.        **",
	victim->name, ch->name );
      }
      else if ( ratio > 5 )
      {
        sprintf( buf,
       "**  %s (%d%% dooms) has been destroyed by %s, preventing doomsday. **",
          IS_NPC(victim) ? victim->short_descr : victim->name,
          ratio,
          IS_NPC(ch) ? ch->short_descr : ch->name );
      }
      else if ( dam >= victim->max_hit )
        sprintf( buf, "**        %s BLICKED by %s.        **",
          IS_NPC(victim) ? victim->short_descr : victim->name,
          IS_NPC(ch) ? ch->short_descr : ch->name );
      else if ( ( i = has_blood( victim ) ) )
        sprintf( buf, "**        %s DOOSHED (%d) by %s.        **",
          IS_NPC(victim) ? victim->short_descr : victim->name,
          i,
          IS_NPC(ch) ? ch->short_descr : ch->name );
      else
        sprintf( buf, "**        %s terminated by %s.        **",
          IS_NPC(victim) ? victim->short_descr : victim->name,
          IS_NPC(ch) ? ch->short_descr : ch->name );
        do_warmessage( NULL, buf );
    }

    if ( mortal_in_main_war( ch ) &&
    !IS_NPC(victim) )
    {
      kill_gain( ch, victim );

      if ( dam > victim->max_hit )
      {
        ch->pcdata->blicks++;
        victim->pcdata->blicked++;
      }

      if ( victim != ch )
      {
        if ( !IS_SET( war_flags, WAR_FLAG_BLOODS ) )
        {
          send_to_char("You are restored and given 300 coins for your glorious victory!\n\r", ch );
          if ( ch->char_class == CLASS_BARBARIAN )
          {
            send_to_char("You resist the magical healing flow!\n\r", ch );
          }
          else
          {
            ch->hit = ch->max_hit;
            ch->mana = ch->max_mana;
          }
          ch->gold += 300;
        }
      }
      else
      {
        send_to_char("You have been awarded nothing for killing yourself!\n\r", ch );
      }
    }

    raw_kill( victim );

    return FALSE;
  }

  tail_chain( );
  return TRUE;
}



bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( IS_NPC(ch) || IS_NPC(victim) )
	return FALSE;

    /* Thx Josh! */
    if ( victim->fighting == ch )
	return FALSE;

    return FALSE;
}



/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if (victim->position != POS_STANDING)
	return FALSE;

    if ( IS_NPC(victim) )
    {
	/* Tuan was here.  :) */
	if ( victim->char_class == CLASS_WARRIOR )
	    chance	= UMIN( 60, 2 * victim->level );
	else
	    chance = 0;
    }
    else
    {
	if ( get_eq_char( victim, WEAR_WIELD ) == NULL )
	    return FALSE;
	chance	= victim->pcdata->learned[gsn_parry] / 2;
    }

    if ( number_percent( ) >= chance + victim->level - ch->level )
	return FALSE;

    act( "You parry $n's attack.",  ch, NULL, victim, TO_VICT, TRUE    );
    act( "$N parries your attack.", ch, NULL, victim, TO_CHAR, TRUE    );
    return TRUE;
}



/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if (victim->position != POS_STANDING)
	return FALSE;

    if ( IS_NPC(victim) )
 	if ( victim->char_class == CLASS_THIEF
	|| victim->char_class == CLASS_ASSASSIN )
            chance  = UMIN( 60, 2 * victim->level );
	else
	    chance = 0;
    else
        chance  = victim->pcdata->learned[gsn_dodge] / 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    act( "You dodge $n's attack.", ch, NULL, victim, TO_VICT, TRUE    );
    act( "$N dodges your attack.", ch, NULL, victim, TO_CHAR, TRUE    );
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }

    victim->position = POS_DEAD;
    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fighting != NULL )
    {
	bug( "Set_fighting: already fighting", 0 );
	return;
    }

    if ( is_affected(ch, gsn_mesmerize) )
    {
	affect_strip( ch, gsn_mesmerize );
	ch->wait = 0;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
	affect_strip( ch, gsn_sleep );

    ch->fighting = victim;
/*
    ch->position = POS_FIGHTING;
*/

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{
	    fch->fighting	= NULL;
	    fch->position	= POS_STANDING;
	    update_pos( fch );
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    making_corpse = TRUE;

    if ( !in_main_war( ch ) && 0 )
    {
      while ( ch->carrying != NULL )
        extract_obj( ch->carrying, "make_corpse" );

      making_corpse = FALSE;
      return;
    }

    if ( IS_NPC(ch) )
    {
	name		= ch->short_descr;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), ch->phase );
	corpse->timer	= number_range( 2, 4 ) * PULSE_TICK;
	if ( ch->gold > 0 )
	{
	    obj_to_obj( create_money( ch->gold, ch->phase ), corpse );
	    ch->gold = 0;
	}
    }
    else
    {
	name		= ch->name;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), ch->phase );
	corpse->timer	= number_range( 25, 40 ) * PULSE_TICK;
	if ( ch->gold > 0 )
	    obj_to_obj( create_money( ch->gold, ch->phase ), corpse );
        ch->gold = 0;
    }

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    /* Leaving this touching next_content in case some objects
     * are in the wrong phase. */
    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	obj_from_char( obj );
	obj_to_obj( obj, corpse );
    }

    obj_to_room( corpse, ch->in_room );
    making_corpse = FALSE;
    return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;

    vnum = 0;
/*
    switch ( number_bits( 4 ) )
*/
    switch (0)
    {
    default: msg  = "Your blood freezes as you hear $n's death cry.";	break;
/*
    case  0: msg  = "$n is dead!  R.I.P.";			break;
*/
    case  1: msg  = "$n splatters blood on your armor.";
/* added a little blood */
/* only pops on harder mobs */
/*
             if ( ch->level > 10 )
                 vnum = 23;
*/
             break;
    case  2: msg  = "You smell $n's sphincter releasing in death.";
	     vnum = OBJ_VNUM_FINAL_TURD;				break;
    case  3: msg  = "$n's severed head plops on the ground.";
	     vnum = OBJ_VNUM_SEVERED_HEAD;				break;
    case  4: msg  = "$n's heart is torn from $s chest.";
	     vnum = OBJ_VNUM_TORN_HEART;				break;
    case  5: msg  = "$n's arm is sliced from $s dead body.";
	     vnum = OBJ_VNUM_SLICED_ARM;				break;
    case  6: msg  = "$n's leg is sliced from $s dead body.";
	     vnum = OBJ_VNUM_SLICED_LEG;				break;
    }

    act( msg, ch, NULL, NULL, TO_ROOM, TRUE );

    if ( vnum != 0 )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	name		= IS_NPC(ch) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ), ch->phase );
        if (vnum != 23)
		obj->timer	= number_range( 4, 7 ) * PULSE_TICK;

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	obj_to_room( obj, ch->in_room );
    }

    if ( IS_NPC(ch) )
	msg = "Your blood freezes as you hear something's death cry.";
    else
	msg = "Your blood freezes as you hear someone's death cry.";

    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM, TRUE );
	}
    }
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *victim )
{
  int num_alive;
  CHAR_DATA *cch;
  extern int war_timer;
  extern int war_status;

  stop_fighting( victim, TRUE );
  death_cry( victim );
  make_corpse( victim );

  /* take dead guy out of war */
  if (!IS_NPC(victim)) 
  {
    if ( mortal_in_main_war( victim )
    && war_status == WAR_ON )
    {
      if ( war_timer < 10 )
        war_timer += 2;

      if ( war_timer < 5 )
        war_timer = 5;

      set_join_status( victim );
    }

    if ( in_private_war( victim ) )
    {
      if ( victim->private_war && !IS_NPC( victim->private_war ) )
      {
        remove_from_war( victim->private_war );
	victim->private_war->pcdata->war_num = WAR_NUM_DEAD;
	victim->private_war->private_war = NULL;
      }
      victim->private_war = NULL;
    }

    remove_from_war( victim );
  }

/* check to make sure there are players left to kill */
    num_alive = pc_alive( );

    if (num_alive <= 1)
	end_war();

    if ( IS_NPC(victim) )
    {
	extract_char( victim, TRUE );
	return;
    }

/* don't need to extract the character */
    while ( victim->affected )
	affect_remove( victim, victim->affected );
    victim->affected_by	= 0;
    victim->armor	= class_table[victim->char_class].base_armor;
    victim->position	= POS_RESTING;
    victim->hit		= UMAX( 1, victim->hit  );
    victim->mana	= UMAX( 1, victim->mana );
    save_char_obj( victim );

/* get rid of charmies */
    for ( cch = char_list ; cch != NULL ; cch = cch->next )
    {
	if ( cch->master != NULL
	&&   cch->master == victim )
		stop_follower(cch);
    }

/* get rid of terminators */
    for ( cch = char_list ; cch != NULL ; cch = cch->next )
    {
	if ( cch->hunting != NULL
	&&   cch->hunting == victim )
		cch->hunting = NULL;
    }

    if( !IS_NPC(victim) )
	victim->nojoin = -1;

    if( !IS_NPC(victim) )
    {
	victim->damroll = 0;
    }

    return;
}


void adjust_ratings( CHAR_DATA *ch, CHAR_DATA *victim )
{
  double chance;
  double difference;
  int change;
  int cclass_r;
  int vclass_r;

  if ( IS_NPC( ch ) || IS_NPC( victim ) )
    return;

  if ( !mortal_in_main_war( ch )
  || !mortal_in_main_war( victim ) )
    return;

  if ( ch->pcdata->kills < 10 || victim->pcdata->kills < 10 )
    return;

  if ( check_no_rating_list( ch, TRUE )
  || check_no_rating_list( victim, TRUE ) )
    return;

  ch->pcdata->ratings_victims += victim->pcdata->rating;
  ch->pcdata->rated_kills++;
  victim->pcdata->ratings_killers += ch->pcdata->rating;
  victim->pcdata->rated_deaths++;

  difference = (double) ( victim->pcdata->rating - ch->pcdata->rating );

  if ( difference < -400 )
    chance = 1.00;
  else
    chance = 1 / ( 1 + pow( 10, ( difference / 400 ) ) );

  change = (int) ( 30.0 * ( 1.0 - chance ) );

  ch->pcdata->rating += change;
  victim->pcdata->rating -= change;

  adjust_high_ratings( ch, victim );

  if ( ch->pcdata->rating > 1000
  && victim->pcdata->rating > 1000
  && ch->char_class != victim->char_class
  && !IS_IMMORTAL( ch ) )
  {
    cclass_r = class_table[ch->char_class].rating_12;
    vclass_r = class_table[victim->char_class].rating_12;

    difference = (double) ( vclass_r - cclass_r );

    if ( difference < -400 )
      chance = 1.00;
    else
      chance = 1 / ( 1 + pow( 10, ( difference / 400 ) ) );

    change = (int) ( 30.0 * ( 1.0 - chance ) );

    class_table[ch->char_class].rating_12 += change;
    class_table[victim->char_class].rating_12 -= change;
  }

#if 0
  if ( ch->pcdata->rating > 1000
  && victim->pcdata->rating > 1000
  && ch->class != victim->class
  && !IS_IMMORTAL( ch ) )
#endif
  if ( ch->char_class != victim->char_class
  && !IS_IMMORTAL( ch ) )
  {
    cclass_r = class_table[ch->char_class].rating_11;
    vclass_r = class_table[victim->char_class].rating_11;

    difference = (double) ( vclass_r - cclass_r );

    if ( difference < -400 )
      chance = 1.00;
    else
      chance = 1 / ( 1 + pow( 10, ( difference / 400 ) ) );

    change = (int) ( 30.0 * ( 1.0 - chance ) );

    class_table[ch->char_class].rating_11 += change;
    class_table[victim->char_class].rating_11 -= change;

    do_cwrite( NULL, "" );
  }

  return;
}


void kill_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if ( !mortal_in_main_war( ch ) )
      return;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( IS_NPC( ch ) )
    {
	victim->pcdata->mob_deaths++;
	return;
    }
    if ( IS_NPC( victim ) )
	return;

/* tally class kills */
    ch->pcdata->class_kills[victim->char_class] += 1;
    victim->pcdata->class_deaths[ch->char_class] += 1;

   if ( victim->pcdata->deaths != 0 )
    ch->pcdata->total_victim_kds += victim->pcdata->kills * 100 / victim->pcdata->deaths;
   else
    ch->pcdata->total_victim_kds += victim->pcdata->kills * 100;

    ch->pcdata->kills += 1;
    ch->exp = ch->pcdata->kills + (10 * ch->pcdata->wins);
    victim->pcdata->deaths += 1;

    adjust_ratings( ch, victim );

    save_char_obj(ch);

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->wear_loc == WEAR_NONE )
	    continue;

	if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
	||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
	||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
	{
	    act( "You are zapped by $p.", ch, obj, NULL, TO_CHAR, TRUE );
	    act( "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM, TRUE );
	    obj_from_char( obj );
	    obj_to_room( obj, ch->in_room );
	}
    }

    return;
}





void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    static char * const attack_table[] =
    {
	"hit",
	"slice",  "stab",  "slash", "whip", "claw",
	"blast",  "pound", "crush", "grep", "bite",
	"pierce", "suction"
    };

    static char * const attack_table_2[] =
    {
	"hits",
	"slices",  "stabs",  "slashes", "whips", "claw",
	"blasts",  "pounds", "crushes", "greps", "bites",
	"pierces", "sucks"
    };

    char buf1[256], buf2[256], buf3[256];
    const char *attack;
    const char *attack2;
    int percent;
    OBJ_DATA *obj;

    obj = get_eq_char( ch, WEAR_WIELD );
		
    percent = (int) ( ((float) dam) / ((float) victim->max_hit) * 100);

    if ( dt >= 0 && dt < MAX_SKILL && skill_table[dt].msg_death_char != NULL )
    {
	if ( dam <= 0 )
	{
          memcpy( buf1, skill_table[dt].msg_miss_room, 255 );
          memcpy( buf2, skill_table[dt].msg_miss_char, 255 );
          memcpy( buf3, skill_table[dt].msg_miss_targ, 255 );
	}
	else if ( percent < 100 )
	{
          memcpy( buf1, skill_table[dt].msg_dam_room, 255 );
          memcpy( buf2, skill_table[dt].msg_dam_char, 255 );
          memcpy( buf3, skill_table[dt].msg_dam_targ, 255 );
	}
	else
	{
          memcpy( buf1, skill_table[dt].msg_death_room, 255 );
          memcpy( buf2, skill_table[dt].msg_death_char, 255 );
          memcpy( buf3, skill_table[dt].msg_death_targ, 255 );
	}
    }
    else
    {
	if ( dt >= 0 && dt < MAX_SKILL )
	{
	    attack	= skill_table[dt].noun_damage;
	    attack2	= skill_table[dt].noun_damage;
	}
	else if ( dt >= TYPE_HIT
	&& dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
	{
	    attack	= attack_table[dt - TYPE_HIT];
	    attack2	= attack_table_2[dt - TYPE_HIT];
	}
	else
	{
	    bug( "Dam_message: bad dt %d.", dt );
	    dt  = TYPE_HIT;
	    attack  = attack_table[0];
	}

	if (dam <= 0)
	{
	    sprintf( buf1, "$n misses $N with $s %s.", attack );
	    sprintf( buf2, "You miss $N with your %s.", attack );
	    sprintf( buf3, "$n miss you with $s %s.", attack );
	}
	else if (percent <= 1)
	{
	    sprintf( buf1, "$n tickles $N with $s %s.", attack );
	    sprintf( buf2, "You tickle $N as you %s $M.", attack );
	    sprintf( buf3, "$n tickle you as $e %s you.", attack2 );
	}
	else if (percent <= 3)
	{
	    sprintf( buf1, "$n barely %s $N.", attack2 );
	    sprintf( buf2, "You barely %s $N.", attack );
	    sprintf( buf3, "$n barely %s you.", attack2 );
	}
	else if (percent <= 5)
	{
	    sprintf( buf1, "$n %s $N.", attack2 );
	    sprintf( buf2, "You %s $N.", attack );
	    sprintf( buf3, "$n %s you.", attack2 );
	}
	else if (percent <= 8)
	{
	    sprintf( buf1, "$n %s $N hard.", attack2 );
	    sprintf( buf2, "You %s $N hard.", attack );
	    sprintf( buf3, "$n %s you hard.", attack2 );
	}
	else if (percent <= 11)
	{
	    sprintf( buf1, "$n %s $N very hard.", attack2 );
	    sprintf( buf2, "You %s $N very hard.", attack );
	    sprintf( buf3, "$n %s you very hard.", attack2 );
	}
	else if (percent <= 14)
	{
	    sprintf( buf1, "$n %s $N extremely hard.", attack2 );
	    sprintf( buf2, "You %s $N extremely hard.", attack );
	    sprintf( buf3, "$n %s you extremely hard.", attack2 );
	}
	else if (percent <= 20)
	{
	    sprintf( buf1, "$n massacre $N to small fragments with $s %s.", attack );
	    sprintf( buf2, "You massacre $N to small fragments with your %s.", attack );
	    sprintf( buf3, "$n massacre you to small fragments with $s %s.", attack );
	}
	else if (percent <= 101)
	{
	    sprintf( buf1, "$n utterly annihilates $N with $s %s.", attack );
	    sprintf( buf2, "You utterly annihilate $N with your %s.", attack );
	    sprintf( buf3, "$n utterly annihilates you with $s %s.", attack );
	}
	else
	{
	    sprintf( buf1, "$n brutally slays $N with $s %s.", attack );
	    sprintf( buf2, "You brutally slay $N with your %s.", attack );
	    sprintf( buf3, "$n brutally slays you with $s %s.", attack );
	}
    }

    act( buf3, ch, obj, victim, TO_VICT, TRUE );
    act( buf2, ch, obj, victim, TO_CHAR, TRUE );
    act( buf1, ch, obj, victim, TO_NOTVICT, TRUE );

    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    OBJ_DATA *wield;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
	return;

/* monks can disarm without a weapon */
    if ( (wield = get_eq_char( ch, WEAR_WIELD )) == NULL
    && number_bits( 1 ) == 0 
    && ch->char_class != CLASS_MONK )
	return;

    if (ch->char_class != CLASS_MONK
    && !IS_NPC(ch)
    && !IS_OBJ_STAT( wield, ITEM_DISARM ) )
    {
	send_to_char("Your weapon is not appropriate for disarming.\n\r", ch );
	return;
    }

    sprintf( buf, "$n disarms you" );
    sprintf( buf2,"$n disarms $N" );
    sprintf( buf3,"You disarm $N" );

    obj_from_char( obj );
    if ( IS_NPC(victim) )
	obj_to_char( obj, victim );
    else
	obj_to_room( obj, victim->in_room );

    if( ch->char_class == CLASS_MONK && number_percent( ) < 26 ) {

	strcat( buf, " and destroys your weapon!!" );
	strcat( buf2," and destroys $S weapon!!" );
	strcat( buf3," and destroy $S weapon!!" );
	act( buf, ch, NULL, victim, TO_VICT, TRUE );
	act( buf2,ch, NULL, victim, TO_NOTVICT, TRUE );
	act( buf3,ch, NULL, victim, TO_CHAR, TRUE );
	extract_obj( obj, "disarm" );
	return;
    }
    else {

	strcat( buf, "!" );
	strcat( buf2,"!" );
	strcat( buf3,"!" );
	act( buf, ch, NULL, victim, TO_VICT, TRUE );
	act( buf2,ch, NULL, victim, TO_NOTVICT, TRUE );
	act( buf3,ch, NULL, victim, TO_CHAR, TRUE );
	return;
    }

    return;
}



/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void bash( CHAR_DATA *ch, CHAR_DATA *victim )
{
  int skill;

  if ( !IS_NPC( ch ) )
    skill = ch->pcdata->learned[gsn_bash];
  else
    skill = 85;

  if ( !IS_NPC( victim ) )
    skill -= victim->pcdata->learned[gsn_balance];

  if ( IS_AFFECTED( ch, AFF_BLIND ) )
    skill /= 2;

  if ( number_percent( ) < skill )
  {
    act("Your bash sends $N sprawling!",
      ch, NULL, victim, TO_CHAR, TRUE );
    act("$n sends you sprawling with a powerful bash!",
      ch, NULL, victim, TO_VICT, TRUE );
    act("$n sends $N sprawling with a powerful bash!",
      ch, NULL, victim, TO_NOTVICT, TRUE );

    WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
    if ( victim->wait == 1 && !IS_NPC( victim ) )
      SET_BIT( victim->act, PLR_CMD_SUCCESS );
    WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
    victim->position = POS_RESTING;
    damage( ch, victim, 0, 0, 0 );
  }
  else
  {
    act("As $N avoids your bash, you topple over and fall to the ground!",
      ch, NULL, victim, TO_CHAR, TRUE );
    act("You dodge a bash from $n who loses $s balance and falls!",
      ch, NULL, victim, TO_VICT, TRUE );
    act("$N avoids being bashed by $n who loses $s balance and falls!",
      ch, NULL, victim, TO_NOTVICT, TRUE );

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
    ch->position = POS_RESTING;
    damage( ch, victim, 0, 0, 0 );
  }

  return;
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

//    if ( !in_a_war( ch ) )
    if ( !can_interact( ch ) )
    {
	send_to_char("Alas, you cannot lift an arm...\n\r", ch );
	return;
    }


    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	check_failed_combat( ch );
	return;
    }

    if ( !in_same_war( ch, victim ) )
    {
	send_to_char("Your hand passes right thru him...\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "You hit yourself.  Ouch!\n\r", ch );
/*
	multi_hit( ch, ch, TYPE_UNDEFINED );
*/
	return;
    }

    if (!IS_NPC(ch))
	ch->pcdata->failed_combat = 0;

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR, TRUE );
	return;
    }

/*
    if ( ch->position == POS_FIGHTING )
*/
    if ( ch->fighting != NULL )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

    if ( !can_interact( ch ) )
    {
	send_to_char("Alas, you cannot lift an arm...\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Backstab whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	check_failed_combat( ch );
	return;
    }

    if ( !in_same_war( ch, victim ) )
    {
	send_to_char("Your hand passes right thru him...\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
	send_to_char( "But, his back is so nice looking!", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if (!IS_NPC(ch))
	ch->pcdata->failed_combat = 0;

    if ( ch->in_room
    && ch->in_room->smoke > 0 )
      reduce_smoke( ch, 5 );

    if ( is_safe( ch, victim ) )
      return;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
    ||   obj->value[3] != 11 )
    {
	send_to_char( "You need to wield a piercing weapon.\n\r", ch );
	return;
    }

    if ( victim->fighting != NULL
    && ch->char_class != CLASS_ASSASSIN )
    {
	send_to_char( "You can't backstab a fighting person.\n\r", ch );
	return;
    }

    if ( ch->fighting != NULL )
    {
	send_to_char( "You are a little busy for that.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_backstab].beats );

    if ( IS_NPC( ch ) )
	percent = UMIN( ch->level * 3 + 10, 95 );
    else
	percent = ch->pcdata->learned[gsn_backstab];

    if ( !IS_NPC( victim ) )
        percent -= victim->pcdata->learned[gsn_sixth_sense];

    percent = UMIN( percent, 95 );

    if ( number_percent() >= percent
    && IS_AWAKE( victim ) )
    {
	damage( ch, victim, 0, gsn_backstab, 0 );
	return;
    }
    else
    {
	one_hit( ch, victim, gsn_backstab );
    }
    return;
}



void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    int attempt;

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	||   pexit->to_room == NULL
	|| ( is_door_closed( pexit, ch )
	  && !IS_AFFECTED( ch, AFF_PASS_DOOR ) )
	|| ( IS_NPC(ch)
	&&   IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) ) )
	    continue;

	move_char( ch, door, TRUE );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( "$n has fled!", ch, NULL, NULL, TO_ROOM, TRUE );
	ch->in_room = now_in;

	stop_fighting( ch, TRUE );
	return;
    }
    send_to_char("!PANIC! You failed to escape!\n\r", ch );

    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

/* don't let em do it if not alive */
    if ( !can_interact( ch ) )
    {
        send_to_char( "Either there is no war on or you are dead already!\n\r", ch);
        return;
    }

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

/* well, pcs can rescue npcs if they want... */
/*
    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }
*/

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_rescue] )
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	return;
    }

    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR, TRUE    );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT, TRUE    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT, TRUE );

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    set_fighting( ch, fch );
    set_fighting( fch, ch );
    return;
}



void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *wear;
    int i = 0, iWear = 0;
    static char str_kick[8] = "do_kick";

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	if ( !IS_NPC( ch->master ) )
	    WAIT_STATE( ch->master, 4 );
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_kick] )
	damage( ch, victim, ch->level>>1, gsn_kick, victim->resist_physical );
    else
	damage( ch, victim, 0, gsn_kick, 0 );

	if( ch->char_class == CLASS_MONK && number_percent( ) < 30 ) {

		for( i = 0; i <= 5; i++ ) {

			wear = get_eq_char( victim, ( iWear = number_range( WEAR_NONE+1, MAX_WEAR ) ) );
			
			if( wear == NULL || wear->item_type == ITEM_WEAPON )
				continue;

			act("$n crushes $p to useless scrap!", ch, wear, victim, TO_ROOM, TRUE );
			act("You crush $p to useless scrap!", ch, wear, victim, TO_CHAR, TRUE );
			extract_obj( wear, str_kick );
			break;
		}
		return;
	}

    return;
}


void do_caltrops( CHAR_DATA *ch, char *argument ) 
{
  char arg1[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  bool bFound = FALSE;
  OBJECT_FUN spec_caltrops;
  int iCaltrops;
  static char str_caltrops[12] = "do_caltrops";
  ObjIterator oiter;

  if( !IS_NPC(ch)
  && number_percent() > ch->pcdata->learned[gsn_caltrops] )
  {
    send_to_char("You failed.\n\r",ch );
    return;
  }

#if 0
  if( !in_main_war( ch )
  && !IS_SET( ch->in_room->area->area_flags, AREA_DISABLED ) )
#endif
  if ( !can_interact( ch ) )
  {
    send_to_char("You may not drop caltrops in an enabled zone unless you are in the main war.\n\r", ch );
    return;
  }

  for( obj = ch->carrying; obj != NULL; obj = obj->next )
  {
    if( obj->special == NULL )
      continue;

    if( obj->special == spec_caltrops )
    {
      bFound = TRUE;
      break;
    }
  }

  if( !bFound )
  {
    send_to_char("You don't have any caltrops to drop.\n\r", ch );
    return;
  }


  one_argument( argument, arg1 );

  if( !is_number( arg1 ) || arg1[0] == '\0' )
  {
    send_to_char("Caltrops <number to drop>\n\r", ch );
    return;
  }

  iCaltrops = atoi( arg1 );

  if( obj->value[0] < iCaltrops )
  {
    send_to_char( "You don't have that many caltrops!", ch );
    return;
  }

  sprintf(buf, "You skillfully place %d caltrop%s around the room.\n\r",
    iCaltrops, iCaltrops == 1 ? "" : "s" );

  send_to_char( buf, ch );
  obj->value[0] -= iCaltrops;
  free_string( obj->short_descr );
  sprintf(buf, "%d caltrop%s", obj->value[0], obj->value[0] == 1 ? "" : "s" );
  obj->short_descr = str_dup( buf );

  if( obj->value[0] == 0 )
  {
    send_to_char( "You have used the last of your caltrops.\n\r", ch );
    obj_from_char( obj );
    extract_obj( obj, str_caltrops );
  }

  for ( obj = oiter.StartList( ch, ch->in_room->contents );
        obj != NULL; obj = oiter.Next( ) )
  {
    if( obj->special == NULL )
      continue;

    if( obj->special == spec_caltrops )
    {
      obj->value[0] += iCaltrops;
      obj->killer    = ch;
      return;
    }
  }

  obj = create_object( get_obj_index( 44 ), ch->phase );
  obj->killer   = ch;
  obj->value[0] = iCaltrops;
  obj->value[1] = TRUE;
  obj->wear_flags = 0;
  obj_to_room( obj, ch->in_room );
  return;
}


void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

    if ( !can_interact( ch ) )
    {
      send_to_char( "You must be in the war to disarm.\n\r", ch );
      return;
    }

/* monks can disarm without a weapon!! */
    if ( get_eq_char( ch, WEAR_WIELD ) == NULL && ch->char_class != CLASS_MONK )
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_disarm].beats );

    percent = number_percent( ) + victim->level - ch->level;
    if ( IS_NPC(ch) || percent < ch->pcdata->learned[gsn_disarm] * 2 / 3 )
    {
	disarm( ch, victim );
	WAIT_STATE( victim, skill_table[gsn_disarm].beats - 1 );
    }
    else
	send_to_char( "You failed.\n\r", ch );
    return;
}



void do_bash( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if ( !can_interact( ch ) )
    {
	send_to_char("Alas, you cannot lift an arm...\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' && (victim = ch->fighting) == NULL)
    {
	send_to_char( "Bash whom?\n\r", ch );
	return;
    }

   if ( arg[0] != '\0' )
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	check_failed_combat( ch );
	return;
    }

    if ( !in_same_war( ch, victim ) )
    {
	send_to_char("Your hand passes right thru him...\n\r", ch );
	return;
    }

    if ( ch->in_room != victim->in_room )
    {
	send_to_char("But they're too far away!\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "You can't bash yourself.\n\r", ch );
	return;
    }

    if (!IS_NPC(ch))
	ch->pcdata->failed_combat = 0;

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR, TRUE );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	send_to_char("You can't concentrate enough while under control.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch )
    && ( get_eq_char( ch, WEAR_WIELD ) == NULL
	 || get_eq_char( ch, WEAR_SHIELD ) == NULL ) )
    {
	send_to_char( "You must wield a weapon and use a shield to bash.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_bash].beats );
    bash( ch, victim );
    return;
}

void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && ( victim->level >= ch->level )
    && (  get_trust(victim) >= ch->level ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    act( "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR, TRUE    );
    act( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT, TRUE    );
    act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT, TRUE );
    raw_kill( victim );

    if( !IS_NPC(victim) && !IS_NPC(ch) )
	    victim->pcdata->class_deaths[ch->char_class]++;
    if( !IS_NPC(ch) && !IS_NPC(victim) ) {
	    ch->pcdata->kills++;
	    ch->pcdata->class_kills[victim->char_class]++;
    }

    return;
}


int has_blood( CHAR_DATA *ch )
{
  OBJ_DATA *obj;
  int i = 0;

  for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    if ( obj->item_type == ITEM_DRINK_CON
    && obj->value[2] == LIQ_BLOOD )
  {
    i++;
  }

  return i;
}


int pc_alive( void )
{
  DESCRIPTOR_DATA *d;
  int live_pcs = 0;

  for (d = descriptor_list ; d != NULL ; d = d->next )
  {
    if (d->connected == CON_PLAYING
    && mortal_in_main_war( d->character ) )
      live_pcs++;
  }

  return live_pcs;
}


void check_failed_combat( CHAR_DATA *ch )
{
    if (IS_NPC(ch))
	return;

    ch->pcdata->failed_combat++;

    if (ch->pcdata->failed_combat > 5)
	WAIT_STATE( ch, (ch->pcdata->failed_combat - 5));

    return;
}

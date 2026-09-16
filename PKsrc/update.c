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
#include <string.h>
#include <time.h>
#include "merc.h"
#include "cmod.h"
#include "global.h"

#include "twitterAnnounce.h"
#include "CharIterator.h"
#include "ObjIterator.h"

/*
 * Global restore variables.
 */
int		max_restore_hits;
int		max_restore_mana;


/*
 * Local functions.
 */
int	hit_gain	args( ( CHAR_DATA *ch ) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
void	echo_update	args( ( void ) );
void	mobile_update	args( ( void ) );
void	terminator_update args( ( void ) );
void	weather_update	args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );
void	aggr_update	args( ( void ) );
void	war_update	args( ( void ) );
void get_dummy_char    args( ( CHAR_DATA *dummy) );
/*
void    to_pit          args( ( CHAR_DATA *ch );
*/
int	playing_pcs	args( ( void ) );



/*
 * Advancement stuff.
 */
void advance_level( CHAR_DATA *ch )
{
    /* control stuff? */

    return;
}   



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;
    extern int war_flags;

    gain = 1;

    if ( IS_NPC(ch) )
    {
	gain = ch->level * 3 / 2;
    }
    else
    {
	gain = class_table[ch->char_class].regen_hit;

	switch ( ch->position )
	{
		case POS_SLEEPING: gain += gain>>1;
		case POS_RESTING:  gain += gain>>2;
/* divide by 2 and 4 respectively */
	}

    }

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;

    if ( IS_AFFECTED( ch, AFF_BLACKMANTLE ) )
	gain = 0;

#if 0
    if( IS_SET(war_flags, WAR_FLAG_NOREGEN )
    && ( ch->class != CLASS_BARBARIAN || ch->class != CLASS_MONK ) )
	gain = 0;
#else
    if( IS_SET(war_flags, WAR_FLAG_NOREGEN ) )
	gain = 0;
#endif

    gain += (gain * ch->hit_regen_bonus) / 100;

    if ( gain < 0 )
      gain = 0;

    return gain;
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;
    extern int war_flags;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	gain = class_table[ch->char_class].regen_mana;

	if (ch->position == POS_RESTING)
		gain += gain;

	if (ch->position == POS_SLEEPING)
		gain = 0;
    }

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;

#if 0
    if( IS_SET(war_flags, WAR_FLAG_NOREGEN)
    && ( ch->class != CLASS_BARBARIAN || ch->class != CLASS_MONK ) )
	gain = 0;
#else
    if( IS_SET(war_flags, WAR_FLAG_NOREGEN) )
	gain = 0;
#endif

    gain += (gain * ch->mana_regen_bonus) / 100;

    if ( gain < 0 )
      gain = 0;

    return UMIN(gain, ch->max_mana - ch->mana);
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    EXIT_DATA *pexit;
    int door;
    ObjIterator oiter;

    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next = ch->next;

	if ( !IS_NPC(ch) || ch->in_room == NULL || IS_AFFECTED(ch, AFF_CHARM) )
	    continue;

	/* Examine call for special procedure */
	if ( ch->spec_fun != 0 && ch->desc == NULL )
	{
	    if ( (*ch->spec_fun) ( ch ) )
		continue;
	}

	/* That's all for sleeping / busy monster */
	if ( ch->position != POS_STANDING )
	    continue;

	/* Scavenge */
	if ( IS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->contents != NULL
	&&   number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = 0;
            for ( obj = oiter.StartList( ch, ch->in_room->contents );
                  obj != NULL; obj = oiter.Next() )
	    {
		if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best, "scavenger" );
		obj_to_char( obj_best, ch );
		act( "$n gets $p.", ch, obj_best, NULL, TO_ROOM, TRUE );
/* try this */
		do_wear( ch, obj_best->name );
	    }
	}

	/* Wander */
	if ( !IS_SET(ch->act, ACT_SENTINEL)
/* fix for new combat */
	&& ( ch->fighting == NULL )
	&& ( door = number_bits( 5 ) ) <= 5
	&& ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !is_door_closed( pexit, ch )
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	&& ( !IS_SET(ch->act, ACT_STAY_AREA)
	||   pexit->to_room->area == ch->in_room->area ) )
	{
	    move_char( ch, door, FALSE );
	}

    }

    return;
}

void terminator_update( void )
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;

  for ( ch = char_list; ch != NULL; ch = ch_next )
  {
    ch_next = ch->next;

    if (!IS_NPC(ch)
    || !IS_SET( ch->act, ACT_TERMINATOR )
    || ch->desc != NULL )
      continue;

    ch->wait = UMAX( 0, ch->wait-1 );

    if ( ch->spec_fun == NULL )
      REMOVE_BIT( ch->act, ACT_TERMINATOR );

    /* Disable this for now - we want some mobs to choose */
    if ( ch->hunting == NULL && 0 )
      continue;

    if ( (*ch->spec_fun) ( ch ) )
      continue;
  }

  return;
}



/*
 * Update the weather.
 */
void weather_update( void )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int diff;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  5:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "The day has begun.\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "The sun rises in the east.\n\r" );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "Howler eats the sun..it gets dark.\n\r" );
	break;

    case 20:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "The night has begun.\n\r" );
	break;

    case 24:
	time_info.hour = 0;
	time_info.day++;
	break;
    }

    if ( time_info.day   >= 35 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 17 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    /*
     * Weather change.
     */
    if ( time_info.month >= 9 && time_info.month <= 16 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change    = UMAX(weather_info.change, -12);
    weather_info.change    = UMIN(weather_info.change,  12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
    weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if ( weather_info.mmhg <  990
	|| ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The sky is getting cloudy.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_CLOUDY:
	if ( weather_info.mmhg <  970
	|| ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "It starts to rain.\n\r" );
	    weather_info.sky = SKY_RAINING;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "The clouds disappear.\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "Lightning flashes in the sky.\n\r" );
	    weather_info.sky = SKY_LIGHTNING;
	}

	if ( weather_info.mmhg > 1030
	|| ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The rain stopped.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_LIGHTNING:
	if ( weather_info.mmhg > 1010
	|| ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The lightning has stopped.\n\r" );
	    weather_info.sky = SKY_RAINING;
	    break;
	}
	break;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   IS_OUTSIDE(d->character)
	    &&   IS_AWAKE(d->character) )
		send_to_char( buf, d->character );
	}
    }

    return;
}



/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_save;
    CHAR_DATA *ch_quit;
    time_t save_time;
    ObjIterator oiter;

    save_time	= current_time;
    ch_save	= NULL;
    ch_quit	= NULL;
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	ch_next = ch->next;

	/*
	 * Find dude with oldest save time.
	 */
	if ( !IS_NPC(ch)
	&& ( ch->desc == NULL || ch->desc->connected == CON_PLAYING )
	&&   ch->level >= 2
	&&   ch->save_time < save_time )
	{
	    ch_save	= ch;
	    save_time	= ch->save_time;
	}

	if ( ch->position >= POS_STUNNED )
	{
	    heal_character( ch, hit_gain(ch) );

	    if ( ch->mana < ch->max_mana )
		ch->mana += mana_gain(ch);
	}

	if ( ch->position == POS_STUNNED )
	    update_pos( ch );

	if ( !IS_NPC(ch) )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_check;

	    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 )
	    {
		if ( --obj->value[2] == 0 && ch->in_room != NULL )
		{
		    --ch->in_room->light;
		    act( "$p goes out.", ch, obj, NULL, TO_ROOM, TRUE );
		    act( "$p goes out.", ch, obj, NULL, TO_CHAR, TRUE );
		    extract_obj( obj, "char_update" );
		}
	    }

	    if( !IS_NPC(ch) )
	    {
		if( ch->nojoin > 0 )
		{
			--ch->nojoin;
			if( ch->nojoin == 0 && ch->level == 30 )
			send_to_char("You may now join.\n\r", ch );
		}
	    }

/* changing void delay so that people can't sit idle after
   a few minutes to hide from pcs until end...
   (changed from 12 to 24  */
/* changing from 24 to 48, may 6, 1996 */
	    if ( ++ch->timer >= 48
	    && ch->level < LEVEL_IMMORTAL
	    && !in_main_war( ch ) )
	    {
		if ( ch->was_in_room == NULL && ch->in_room != NULL )
		{
		    ch->was_in_room = ch->in_room;
		    if ( ch->fighting != NULL )
			stop_fighting( ch, TRUE );
		    act( "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM, TRUE );
		    send_to_char( "You disappear into the void.\n\r", ch );
		    save_char_obj( ch );
		    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }

/* if they were idle for more than 30 minutes, we will assume they
    lost link due to circumstances not of thier control and not
    penalize them with a death. */
	    if ( ch->timer > 30 && ch->level < 40 )
		ch_quit = ch;

            /* for regenerating items, or others that activate on ticks */
            for ( obj_check = oiter.StartList( ch, ch->carrying );
                  obj_check != NULL; obj_check = oiter.Next() )
	    {
		if (obj_check->special != NULL)
		{
		    (*obj_check->special)
                      (obj_check, ch, "called_from_char_update", str_null);
		}
	    }

	}

	for ( paf = ch->affected; paf != NULL; paf = paf_next )
	{
	    paf_next	= paf->next;
	    if ( paf->duration > 0 )
		paf->duration--;
	    else if ( paf->duration < 0 )
		;
	    else
	    {
		if ( paf_next == NULL
		||   paf_next->type != paf->type
		||   paf_next->duration > 0 )
		{
		    if ( paf->type > 0 && skill_table[paf->type].msg_off )
		    {
			send_to_char( skill_table[paf->type].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		}
	  
		affect_remove( ch, paf );
	    }
	}

	/*
	 * Careful with the damages here,
	 *   MUST NOT refer to ch after damage taken,
	 *   as it may be lethal damage (on NPC).
	 */
	if ( IS_AFFECTED(ch, AFF_POISON) )
	{
	    int dam;

	    act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM, TRUE );
	    send_to_char( "You shiver and suffer.\n\r", ch );
	    dam = UMIN( dice( 7, 7 ), ( ch->hit - 1 ) );
	    damage( ch, ch, dam, gsn_poison, 0 );
	}
	else if ( ch->position == POS_INCAP )
	{
	    damage( ch, ch, 1, TYPE_UNDEFINED, 0 );
	}
	else if ( ch->position == POS_MORTAL )
	{
	    damage( ch, ch, 2, TYPE_UNDEFINED, 0 );
	}
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    if ( ch_save != NULL || ch_quit != NULL )
    {
	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
	    ch_next = ch->next;
	    if ( ch == ch_save )
		save_char_obj( ch );
	    if ( ch == ch_quit )
		do_quit( ch, str_empty );
	}
    }

    return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  CharIterator citer;

  for ( obj = object_list; obj != NULL; obj = obj_next )
  {
    CHAR_DATA *rch;
    char *message;

    obj_next = obj->next;

    if ( obj->timer <= 0 )
      continue;

    /* Handle archery reload only if bow is wielded. */
    if ( obj->item_type == ITEM_WEAPON
      && IS_SET( obj->extra_flags, ITEM_RANGED_WEAPON ) )
    {
      if ( obj->wear_loc == WEAR_WIELD )
        obj->timer--;

      if ( obj->timer == 0 && obj->carried_by != NULL )
        send_to_char("Your weapon is ready to fire.\n\r", obj->carried_by );

      continue;
    }

    if ( --obj->timer > 0 )
      continue;
       
        if ( obj->item_type == ITEM_GRENADE )
        {
          explode_grenade( obj );
          continue;
        }

	switch ( obj->item_type )
	{
	default:              message = "$p vanishes.";         break;
	case ITEM_FOUNTAIN:   message = "$p dries up.";         break;
	case ITEM_CORPSE_NPC: message = "$p is grabbed by a quivering hoard of pack rats."; break;
	case ITEM_CORPSE_PC:  message = "$p decays into dust."; break;
	case ITEM_FOOD:       message = "$p decomposes.";	break;
	}

	if ( obj->carried_by != NULL )
	{
	    act( message, obj->carried_by, obj, NULL, TO_CHAR, TRUE );
	}
	else if ( obj->in_room != NULL )
	{
            rch = citer.StartPhase( obj->phase, obj->in_room,
                  CharIterator::ROOM );
            if ( rch != NULL )
            {
	      act( message, rch, obj, NULL, TO_ROOM, TRUE );
	      act( message, rch, obj, NULL, TO_CHAR, TRUE );
            }
	}

	extract_obj( obj, "obj_update" );
    }

    return;
}



/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *    who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update( void )
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *vch;
    CHAR_DATA *victim;
    extern int war_status;
    CharIterator citer;
    CharIterator viter;

/* however, if the war is not on... */

    if (war_status != WAR_ON && 0 )
	return;

    for ( wch = char_list; wch != NULL; wch = wch_next )
    {
	wch_next = wch->next;
/*
	if ( !mortal_in_main_war( wch )
	||   wch->in_room == NULL )
*/
        if ( !can_interact( wch )
           || IS_NPC( wch )
           || wch->in_room == NULL )
	    continue;

        for ( ch = citer.Start( wch, CharIterator::ROOM );
	      ch != NULL; ch = citer.Next() )
	{
	    int count;

            /* mob has a small chance to attack, but not always... */
            if ( number_bits( 3 ) )
		continue;

	    if ( !IS_NPC(ch)
	    ||   !IS_SET(ch->act, ACT_AGGRESSIVE)
	    ||   ch->fighting != NULL
	    ||   IS_AFFECTED(ch, AFF_CHARM)
	    ||   !can_see( ch, wch ) )
		continue;

	    /*
	     * Ok we have a 'wch' player character and a 'ch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
	    count	= 0;
	    victim	= NULL;
	    for ( vch = viter.Start( wch, CharIterator::ROOM );
                  vch != NULL; vch = viter.Next() )
	    {
                if ( IS_NPC( vch ) )
                  continue;

		if ( can_interact( vch )
// ( ch->phase != WAR_NUM_MAIN || mortal_in_main_war( vch ) )
		&&   can_see( ch, vch ) )
		{
		    if ( number_range( 0, count ) == 0 )
			victim = vch;
		    count++;
		}
	    }

	    if ( victim == NULL )
	    {
		bug( "Aggr_update: null victim.", count );
		continue;
	    }

	    multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }

    return;
}


void echo_update( void )
{
  return;

    switch( number_bits( 12 ) )
    {
	case 1:
	    execute_echo( "That really did HURT!" );
	    break;
	case 2:
	    execute_echo( "You sure are BLEEDING!" );
	    break;
	case 3:
	    execute_echo( "Suddenly, Knife stabs you in the back!" );
	    break;
	case 4:
	    execute_echo( "*Click.*" );
	    break;
	case 5:
	    execute_echo( "Self has arrived." );
	    break;
	case 6:
	    execute_echo( "Reboot by Rip." );
	    break;
        case 7:
	    execute_echo( "##SESSION 'pk' DIED." );
	    break;
	case 8:
	    execute_echo( "Connection closed by foreign host." );
	    break;
	case 9:
	    execute_echo( "Alas, you cannot lift an arm..." );
	    break;
	case 10:
	    execute_echo( "You are dead! R.I.P." );
	    break;
	case 11:
	    execute_echo( "You are either not alive or the war hasn't started." );
	    break;
	case 12:
	    execute_echo( "**** The Terminator shall hunt you down! ****" );
	    break;
	case 13:
	    execute_echo( "#GWyrd gossips 'Lag!'#0" );
            break;
        case 14:
            execute_echo( "Rizbolin sends you sprawling with a powerful bash." );
            break;
        default:
	    break;
    }

    return;
}


/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static  int     pulse_area;
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;


    if ( --pulse_area     <= 0 )
    {
	pulse_area	= PULSE_AREA;
	area_update	( );
/* adding for war */
        war_update	( );
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update	( );
    }

    if (number_bits(1) == 1)
    {
	terminator_update();
    }


    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
    }

    if ( --pulse_point    <= 0 )
    {
	pulse_point     = number_range( PULSE_TICK - PULSE_TICK/4 , PULSE_TICK + PULSE_TICK/4 );
	pulse_point	= PULSE_TICK;
	weather_update	( );
	char_update	( );
	echo_update	( );
    }

    obj_update( );

    aggr_update( );
    tail_chain( );
    return;
}


void war_update( void )
{
  extern int shutdown_time;
  extern int war_total;
  extern int war_timer;
  extern int war_status;
  extern int war_flags;
  extern int war_death;
  extern int war_max;
  extern bool merc_down;
  extern bool reboot_on;
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  CHAR_DATA *ch, *ch_next;
  AREA_DATA *pArea;
  extern AREA_DATA *area_first;
  extern void reset_area( AREA_DATA *pArea, unsigned int phase );
  extern void set_class_profile( CHAR_DATA *ch );

  static char str_autoboot[27] = "Automatic post-war reboot.";

  if( shutdown_time >= 0 )
  {
    if( shutdown_time == 0 )
    {
      do_warmessage(NULL,"Game shutting down immediately." );
      append_file( char_list, REBOOT_FILE, buf );
      merc_down = TRUE;
      return;
    }
    else
    {
      --shutdown_time;
      sprintf(buf,"Game shutting down in %d minute%s%s",
	shutdown_time,
	shutdown_time == 1 ? "" : "s",
	shutdown_time == 1 ? "!" : "." );
      do_warmessage(NULL,buf);
    }
  }

  if ( char_list == NULL )
    return;

  if (war_status == NO_WAR)
    return;

  war_timer--;
  if (war_status == PRE_WAR && war_timer > 0 )
  {
    sprintf( buf, "%d tick%s until war breaks out.",
      war_timer, war_timer == 1 ? "" : "s" );
    do_warmessage( NULL, buf );
  }

  if (war_status == PRE_WAR && war_timer == 0)
  {
    DESCRIPTOR_DATA *d;
    int iCount = 0;

    if (playing_pcs( ) <= 1)
    {
      do_warmessage( NULL, "Not enough players to start war, aborting." );
      set_war_status ( NO_WAR );
      return;
    }

    war_timer = 0;
    war_total = 0;
    war_max = 0;
    do_warmessage( NULL, "The war has begun!!" );

    max_restore_hits = 80;
    max_restore_mana = 80;

    REMOVE_BIT(war_flags, WAR_FLAG_CONTINUOUS	);
    REMOVE_BIT(war_flags, WAR_FLAG_LOADALL		);
    REMOVE_BIT(war_flags, WAR_FLAG_BLOODS		);
    REMOVE_BIT(war_flags, WAR_FLAG_SUMMON		);
    REMOVE_BIT(war_flags, WAR_FLAG_NOREGEN		);
    REMOVE_BIT(war_flags, WAR_FLAG_FOUNTAINS	);
    REMOVE_BIT(war_flags, WAR_FLAG_RANDOMCLASS	);
    REMOVE_BIT(war_flags, WAR_FLAG_NOQUAFF	);
    REMOVE_BIT(war_flags, WAR_FLAG_HEALTHLAG	);
    REMOVE_BIT(war_flags, WAR_FLAG_NOEXTEND	);

/*
    if(number_percent() < 51)
      do_warflags(char_list, "continuous" );
*/
    if(number_percent() < 80)
      do_warflags(char_list, "bloods" );
/*
    if(number_percent() < 4)
      do_warflags(char_list, "summon" );
    if(number_percent() < 2)
      do_warflags(char_list, "fountains"	);
    if(number_percent() < 5)
      do_warflags(char_list, "randomclass"	);
*/
    if(number_percent() < 10)
      do_warflags(char_list, "loadall"	);
    if(number_percent() < 10)
      do_warflags(char_list, "noquaff"	);
#if 0
    if(number_percent() < 10)
      do_warflags(char_list, "healthlag" );
#endif


    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
      if ( IS_SET( pArea->area_flags, AREA_CRITICAL ) )
        REMOVE_BIT( pArea->area_flags, AREA_DISABLED );
      else if ( IS_SET( pArea->area_flags, AREA_NEVER ) )
        SET_BIT( pArea->area_flags, AREA_DISABLED );
      else if ( number_percent( ) <= pArea->access_percent )
        REMOVE_BIT( pArea->area_flags, AREA_DISABLED );
      else
        SET_BIT( pArea->area_flags, AREA_DISABLED );
    }

    reset_mud( WAR_NUM_MAIN );
    set_war_status( WAR_ON );

    for (d = descriptor_list ; d != NULL ; d = d->next )
    {
      if (d->connected == CON_PLAYING
      && d->character->level == 30
      && (IS_NPC(d->character) ? 0 : !IS_SET( d->character->act, PLR_NOPLAY ) )
      && d->character->in_room != NULL
      && !in_static_war( d->character ) )
      {
	iCount++;

	d->character->nojoin = 0;
	do_joinwar( d->character, NULL );
	do_restore( d->character, d->character->name );
			
	if( !IS_SET( war_flags, WAR_FLAG_CONTINUOUS ) )
          war_timer += 2;
	else if( iCount > 2 )
          war_timer += 2;
	else 
          war_timer = 12;

	war_timer = UMIN( 30, war_timer );
      }
    }


    if( !IS_SET( war_flags, WAR_FLAG_CONTINUOUS ) && war_timer > 20 ) 
      war_timer = 25;

    war_death = 10;	

    return;
  }

  if (war_status == WAR_ON  )
  {
    int num_alive;
    extern int pc_alive( void );

    num_alive = pc_alive( );

    if (num_alive <= 1)
    {
      end_war();
      return;
    }

    war_total++;

    if( !IS_SET( war_flags, WAR_FLAG_CONTINUOUS ) ) 
    {
      if( war_timer == 0 ) 
      {
        if ( count_waiting_players( ) == 0
        && !IS_SET( war_flags, WAR_FLAG_NOEXTEND ) )
	{
	  do_warmessage( NULL,
	    "No waiting players.  Timer extended 5 minutes." );
	  war_timer = 5;

          if ( number_percent() < 20 && !IS_SET( war_flags, WAR_FLAG_NOEXTEND ) )
          {
	    SET_BIT( war_flags, WAR_FLAG_NOEXTEND );
	    do_warmessage( NULL, "War extensions have been disabled." );
          }

          if(number_percent() < 35 && !IS_SET( war_flags, WAR_FLAG_NOREGEN ) )
	  {
	    SET_BIT( war_flags, WAR_FLAG_NOREGEN );
	    do_warmessage( NULL, "Regeneration has been disabled." );
	  }

          if(number_percent() < 35 && !IS_SET( war_flags, WAR_FLAG_HEALTHLAG ) )
	  {
	    SET_BIT( war_flags, WAR_FLAG_HEALTHLAG );
	    do_warmessage( NULL, "Low health lag has been enabled." );
	  }

          if(number_percent() < 20 && !IS_SET( war_flags, WAR_FLAG_NOQUAFF ) )
	  {
	    SET_BIT( war_flags, WAR_FLAG_NOQUAFF );
	    do_warmessage( NULL, "Potion quaffing has been disabled." );
	  }

          if ( number_percent() < 20 )
          {
	    do_warmessage( NULL, "Zombies of slain players crawl from beneath the earth." );
            for (ch = char_list ; ch != NULL ; ch = ch->next )
            {
              if ( IS_NPC( ch ) )
                continue;

              if ( mortal_in_main_war( ch ) )
                assign_zombie( ch );
            }
          }

	  return;
	}
	end_war();

	for( ch = char_list; ch != NULL; ch = ch_next ) 
	{
	  ch_next = ch->next;

	  if( IS_NPC(ch) ) 
	    continue;

          if ( !mortal_in_main_war( ch ) )
            continue;

	  ch->gold = 0;

	  while ( ch->affected != NULL )
	    affect_remove( ch, ch->affected );

	  ch->affected_by = 0;
	}
	set_war_status( NO_WAR );

	return;
      }

      if( war_timer < 6 ) 
      {
	sprintf( buf, "There %s %d tick%s until Doomsday.",
	  war_timer == 1 ? "is" : "are", war_timer, war_timer == 1 ? "" : "s" );
	do_warmessage( NULL, buf );
	return;
      }
    }
    else
    {
      if( war_timer == 0 )
      {
        int iCount;

	war_timer = 10;

	for( vch = char_list; vch != NULL; vch = vch->next)
	{
          if( IS_NPC(vch) )
	    continue;

          if( mortal_in_main_war( vch ) )
            iCount++;

	  if( iCount > 2 )
	  {
	    if ( war_timer < 15 )
	      war_timer += 4;
	    else
	      war_timer += 2;
	  }
	}

	reset_mud( 0 );
	do_warmessage( NULL, "Rebirth is completed!" );

	for (vch = char_list ; vch != NULL ; vch = vch_next )
	{
	  vch_next = vch->next;

	  if ( mortal_in_main_war( vch ) )
	    teleport_char(vch,TRUE);

	  if (vch->in_room->vnum == 1
	  || IS_SET(war_flags, WAR_FLAG_TELEALL) )
	  {
	    spell_teleport( 63, 50, vch, vch );
	  }
	}

	return;
      }

      if( war_timer < 6 )
      {
	sprintf( buf, "World rebirth in %d tick%s!",
	  war_timer, war_timer == 1 ? "" : "s" );
	do_warmessage( NULL, buf );
	return;
      }
    }

    return;
  }

  if (war_status == POST_WAR && war_timer == 0)
  {
    if ( reboot_on == FALSE )
    {
	do_warmessage( NULL,
	  "The turmoil from the previous war has been cleaned up." );
	set_war_status( NO_WAR );
    }
    else
    {
	if( reboot_on == TRUE )
	{
	  do_warmessage(NULL, str_autoboot );
          append_file( char_list, REBOOT_FILE, str_autoboot );
	  merc_down = TRUE;
	  return;
	}
    }
  }
  return;
}

void reset_mud( unsigned int phase )
{
  CHAR_DATA *ch;
  extern AREA_DATA *area_first;
  AREA_DATA *pArea;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  static char str_reset_contains[19] = "reset_mud contains";
  static char str_reset_mud[10] = "reset_mud";
  extern void reset_area( AREA_DATA *pArea, unsigned int phase );
  extern int making_corpse;
  CharIterator citer;

  making_corpse = TRUE;
  for( obj = object_list; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next;

    if ( obj->carried_by == NULL
    || IS_NPC( obj->carried_by ) )
    {
      while ( obj->contains )
        extract_obj( obj->contains, str_reset_contains );

      obj_next = obj->next;
      extract_obj( obj, str_reset_mud );
    }
  }
  making_corpse = FALSE;

  for ( ch = citer.StartPhase( phase, NULL, CharIterator::WORLD );
        ch != NULL; ch = citer.Next() )
  {

    if ( IS_NPC( ch ) )
      extract_char( ch, TRUE );
    else
      stop_fighting( ch, TRUE );
  }

  for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
  {
    reset_area( pArea, phase );
    pArea->lifespan[phase] = 0;
  }
  return;
}

void end_war( void )
{
  extern int war_timer;
  extern int war_status;
  extern int war_max, war_total;
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  CHAR_DATA *wch;
  int pc_count = 0;
  int i;
  static char str_reset_one_tick[17] = "Reset in 1 tick.";
  static char str_no_one_won[24] = "Strange, no one won....";
  static char str_doom[45] = "No one stopped the doomsday device!!  BOOM!!";
  int has_blood( CHAR_DATA *ch );

  if ( war_status != WAR_ON )
    return;

  if ( char_list == NULL )
    return;

  for (d = descriptor_list ; d != NULL ; d = d->next )
  {
    if (d->connected == CON_PLAYING
    && mortal_in_main_war( d->character ) )
      pc_count++;
  }

  if (pc_count > 1)
  {
    for (wch = char_list ; wch != NULL ; wch = wch->next )
    {
      if ( IS_NPC( wch ) )
	continue;

      if ( mortal_in_main_war( wch ) )
      {
	wch->pcdata->deaths++;
	wch->pcdata->doomsdays++;
#if 0
	wch->pcdata->rating -= 30;
#endif
	save_char_obj(wch);
	remove_from_war( wch );
	send_to_char( "The world explodes beneath your feet and you die!\n\r",
	  wch );
      }
    }

    do_warmessage( NULL, str_doom );
    do_warmessage( NULL, str_reset_one_tick );
    set_war_status( POST_WAR );
    war_timer = 1;
    return;
  }


  if (pc_count == 1)
  {
    CHAR_DATA *winner = NULL;
    extern char last_winner[MAX_NAME_LENGTH+1];

    for (wch = char_list ; wch != NULL ; wch = wch->next )
    {
      if ( IS_NPC( wch ) )
	continue;

      if ( !mortal_in_main_war( wch ) )
	continue;

      if ( wch->desc != NULL )
      {
	winner = wch;
	continue;
      }

      wch->pcdata->ld_at_end++;
/*
      wch->pcdata->rating -= 15;
*/
      wch->pcdata->deaths++;
      remove_from_war( wch );
    }

    send_to_char( "You pick up a win!\n\r" , winner );
    winner->pcdata->wins++;
    save_char_obj( winner );
    remove_from_war( winner );

    i = has_blood( winner );
    if ( i > 0 )
      sprintf( buf, "%s won the war with %d blood%s left!",
        winner->name, i, ( ( i > 1 ) ? "s" : "" ) );
    else
      sprintf( buf, "%s won the war!!!", winner->name );
    sprintf( last_winner, "%s", winner->name );
    do_warmessage( NULL , buf );
    do_warmessage( NULL, str_reset_one_tick );

    sprintf( buf, "%s (%s%s) won the %d plr war in %d ticks! %d hps. %d eqp.",
             winner->name,
             IS_SET( winner->act, PLR_RANDOM_CLASS ) ? "r-" : "",
             class_table[winner->char_class].who_name,
             war_max, war_total,
             winner->hit, winner->eq_points );
    twitter.push( buf, 2 );

    set_war_status( POST_WAR );
    war_timer = 1;
    return;
  }

    if (pc_count == 0)
    {
	do_warmessage( NULL, str_no_one_won );
        do_warmessage( NULL, str_reset_one_tick );
	set_war_status( POST_WAR );
	war_timer = 1;

	return;
    }

    return;
}


void do_start( CHAR_DATA *ch, char *arg )
{
  DESCRIPTOR_DATA *d;
  extern int war_timer;
  extern int war_status;
  char buf[MAX_STRING_LENGTH];
  int pc_count = 0;

  if (war_status > NO_WAR)
  {
    send_to_char( "Timer already started!!\n\r", ch );
    return;
  }

  if (war_status > PRE_WAR)
  {
    send_to_char( "War has already begun!\n\r", ch );
    return;
  }

  for (d = descriptor_list; d != NULL ; d = d->next )
  {
    if (d->connected == CON_PLAYING
    /* add no play flag */
    && (IS_NPC(d->character) ? 0 : !IS_SET( d->character->act, PLR_NOPLAY ) )
    && !in_static_war( d->character )
    && d->character->level == 30)
      pc_count++;
  }

  if (pc_count <= 1)
    return;

  war_timer = 2;
  set_war_status( PRE_WAR );

  sprintf( buf,
    "%s has started the countdown, war will break out in 2 ticks.", ch->name );
  do_warmessage( NULL, buf );

  return;
}

int playing_pcs( )
{
	DESCRIPTOR_DATA *d;
	int pc_count = 0;

	for (d = descriptor_list ; d != NULL ; d = d->next )
	{
	    if (d->connected == CON_PLAYING
	        && d->character->level == 30
		&& (IS_NPC(d->character) ? 0 : !IS_SET( d->character->act, PLR_NOPLAY ) ) )
		 pc_count++;
	}

	return pc_count;
}

void remove_from_war( CHAR_DATA *ch )
{
  if ( IS_NPC( ch ) )
    return;

  if ( ch->desc == NULL )
    return;

  while (  ch->desc->snoop_by != NULL )
    remove_snoop( ch->desc, ch->desc->snoop_by );

  phase_char( ch, WAR_NUM_DEAD );
  ch->pcdata->war_num = WAR_NUM_DEAD;

  REMOVE_BIT( ch->act, PLR_NOPLAY );
}

void set_war_status( int status )
{
    extern time_t war_number;
    extern time_t current_time;
    extern int war_status;

    war_number = current_time;

    war_status = status;
}

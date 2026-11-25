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
#include "CharIterator.h"
#include "ObjIterator.h"



char const dir_name [6][7] =
{
  "north", "east", "south", "west", "up", "down"
};

char const dir_name_2 [6][10] =
{
  "the north", "the east", "the south", "the west", "above", "below"
};

const sh_int rev_dir [6] =
{
    2, 3, 0, 1, 5, 4
};

const	sh_int	movement_loss	[SECT_MAX]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};



/*
 * Local functions.
 */
int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
bool	has_key		args( ( CHAR_DATA *ch, int key ) );
bool    is_door_closed( EXIT_DATA *pexit, CHAR_DATA *ch );
bool    is_door_locked( EXIT_DATA *pexit, CHAR_DATA *ch );
bool    is_door( EXIT_DATA *pexit, CHAR_DATA *ch );


void move_char( CHAR_DATA *ch, int door, bool fForce )
{
  CHAR_DATA *fch;
  ROOM_INDEX_DATA *in_room;
  ROOM_INDEX_DATA *to_room;
  EXIT_DATA *pexit;
  OBJ_DATA *obj;
  AFFECT_DATA *paf;
  INTERACTIVE spec_invis_blocker;
  extern int war_flags;
  CharIterator citer;
  ObjIterator oiter;

  if ( door < 0 || door > 5 )
  {
    bug( "Do_move: bad door %d.", door );
    return;
  }

  if( !IS_NPC(ch)
  && ( paf = is_affected( ch, gsn_entangle ) ) != NULL )
  {
    WAIT_STATE( ch, 2 );

    paf->duration--;

    send_to_char("You struggle against the roots binding your feet...\n\r",
      ch );
    act( "$n struggles against the roots binding $s feet...",
      ch, NULL, NULL, TO_ROOM, TRUE );

    if( paf->duration == 0 )
    {
      send_to_char( "You break the roots binding your feet.\n\r", ch );
      affect_remove( ch, paf );
    }

    return;
  }

  if( !IS_NPC(ch)
  && ( paf = is_affected( ch, gsn_confusion ) ) != NULL )
  {
    WAIT_STATE( ch, 1 );

    paf->duration--;

    if( paf->duration == 0 )
    {
      send_to_char( "You feel far less confused now.\n\r", ch );
      affect_remove( ch, paf );
    }

    send_to_char("The room spins before your eyes...\n\r", ch );
    door = number_range( 0, 5 );
  }

  in_room = ch->in_room;
  if ( ( pexit   = in_room->exit[door] ) == NULL
  ||   ( to_room = pexit->to_room      ) == NULL )
  {
    send_to_char( "Alas, you cannot go that way.\n\r", ch );
    return;
  }

  if ( in_room->area != to_room->area
  && IS_SET( to_room->area->area_flags, AREA_DISABLED )
  && mortal_in_main_war( ch ) )
  {
    send_to_char( "That area is not open this war.\n\r", ch );
    return;
  }

  if ( in_room->area != to_room->area
  && in_private_war( ch ) )
  {
    send_to_char( "You can not leave the zone in a private war.\n\r", ch );
    return;
  }

  if ( is_door_closed( pexit, ch )
  && !IS_AFFECTED(ch, AFF_PASS_DOOR) 
  && in_static_war( ch ) )
  {
    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR, TRUE );
    return;
  }

  if ( IS_AFFECTED(ch, AFF_CHARM)
  &&   ch->master != NULL
  &&   in_room == ch->master->in_room
  &&   fForce == FALSE )
  {
    send_to_char( "What?  And leave your beloved master?\n\r", ch );
    return;
  }

  if ( in_static_war( ch ) )
  {
    if (ch->carry_weight > can_carry_w(ch))
    {
      send_to_char("Your legs buckle under the weight.\n\r", ch);
      return;
    }

    if ( in_room->sector_type == SECT_AIR
    ||   to_room->sector_type == SECT_AIR )
    {
      if ( !can_fly( ch ) )
      {
        send_to_char( "You can't fly.\n\r", ch );
        return;
      }
    }

    if ( in_room->sector_type == SECT_WATER_NOSWIM
    ||   to_room->sector_type == SECT_WATER_NOSWIM )
    {
      if ( !can_traverse_water( ch ) )
      {
        if ( number_percent( ) > ch->pcdata->learned[gsn_swim] )
        {
          send_to_char( "You struggle to swim in your armor...  a boat would help.\n\r", ch );
          WAIT_STATE( ch, 12 );
          return;
        }
        else
        {
          send_to_char( "You swim against the current.\n\r", ch );
          WAIT_STATE( ch, 4 );
        }
      }
    }
  }

    if ( !IS_AFFECTED(ch, AFF_SNEAK)
    || ( IS_NPC(ch) ||
	 (!IS_SET(ch->act, PLR_WIZINVIS)
	      && number_percent( ) > ch->pcdata->learned[gsn_sneak] ) ) )
    {
        if ( is_affected( ch, gsn_illusory_appearance ) )
	  act( "The beastly fido leaves $T.", ch, NULL,
	    dir_name[door], TO_ROOM, FALSE );
	else
	  act( "$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM, FALSE );
    }

    /* track */
    if ( mortal_in_main_war( ch )
    && !IS_AFFECTED( ch, AFF_FLYING ))
    {
	ch->in_room->last_exit_dir = door;
	free_string( ch->in_room->last_char );
	ch->in_room->last_char = str_dup( ch->name );
    }

    char_from_room( ch );
    char_to_room( ch, to_room );

    if ( to_room->smoke > 0 )
      reduce_smoke( ch, 1 );

    for ( obj = oiter.StartList( ch, to_room->contents );
          obj != NULL; obj = oiter.Next( ) )
    {
	if ( obj->special == NULL )
	    continue;

	(*obj->special) (obj, ch, "called_from_move_char", str_null );
    }
    if ( !IS_AFFECTED(ch, AFF_SNEAK) 
    || ( IS_NPC(ch) ||
	 (!IS_SET(ch->act, PLR_WIZINVIS)
	      && number_percent( ) > ch->pcdata->learned[gsn_sneak] ) ) )
    {
        if ( is_affected( ch, gsn_illusory_appearance ) )
	  act( "The beastly fido has arrived.",ch, NULL, NULL, TO_ROOM, FALSE );
	else
	  act( "$n has arrived.", ch, NULL, NULL, TO_ROOM, FALSE );
    }
  
    if (!IS_NPC(ch))
	if (ch->pcdata->learned[gsn_track] > 5
	&& number_percent() > ch->pcdata->learned[gsn_track] )
		ch->in_room->last_exit_dir = -1;
   
    do_look( ch, str_auto );

    while ( number_percent() < ch->move_lag )
	ch->wait++;

    if ( is_affected( ch, gsn_slow ) )
	WAIT_STATE( ch, 2 );

  if( IS_SET(war_flags, WAR_FLAG_HEALTHLAG ) && mortal_in_main_war( ch )
  && !IS_NPC( ch ) )
  {
    int chance;

    chance = ( 100 *
      ( ( ch->max_hit + ch->max_mana ) - ( ch->hit + ch->mana ) )
      / ( ch->max_hit + ch->max_mana ) );

    if ( number_percent() < chance )
      ch->wait++;
  }

  if (fForce == FALSE)
  {
    for ( fch = citer.StartPhase( ch->phase, in_room, CharIterator::ROOM );
          fch != NULL; fch = citer.Next() )
    {
	if ( fch->master == ch && fch->position == POS_STANDING )
	{
	    int fFollow = FALSE;

	    if ( in_static_war( fch )
	    && number_percent( ) < fch->pcdata->learned[gsn_follow] )
	        fFollow = TRUE;

            if ( IS_NPC( fch ) )
              fFollow = TRUE;

	    if ( ( !in_static_war( fch )
	        && !in_static_war( ch ) )
	    || fFollow == TRUE )
	    {
		act( "You follow $N.", fch, NULL, ch, TO_CHAR, TRUE );
		move_char( fch, door, FALSE );
	    }
	}
    }
  }

  for ( fch = citer.Start( ch, CharIterator::ROOM );
        fch != NULL; fch = citer.Next() )
  {
    if( IS_NPC(fch) && fch->interactive != NULL )
    {
      if( fch->interactive == spec_invis_blocker )
        (*fch->interactive) ( fch, ch, NULL, NULL );
      break;
    }
  }

  return;
}


int get_direction( char *arg )
{
    if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) return 0;
    if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) return 1;
    if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) return 2;
    if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) return 3;
    if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) return 4;
    if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) return 5;

    return -1;
}

bool can_traverse_water( CHAR_DATA *ch )
{
  OBJ_DATA *obj;
  bool found;
  ObjIterator oiter;

  if ( IS_AFFECTED( ch, AFF_FLYING ) )
    return TRUE;

  /*
   * Look for a boat.
   */
  found = FALSE;
  for ( obj = oiter.StartList( ch, ch->carrying );
        obj != NULL; obj = oiter.Next( ) )
  {
    if ( obj->item_type == ITEM_BOAT )
    {
      return TRUE;
      break;
    }
  }

  return FALSE;
}


bool can_fly( CHAR_DATA *ch )
{
  if ( !IS_AFFECTED(ch, AFF_FLYING) )
    return FALSE;

  return TRUE;
}


void do_north( CHAR_DATA *ch, char *argument )
{
    if (ch->fighting != NULL)
    {
	send_to_char("You are much too busy for that!\n\r", ch );
	return;
    }

    move_char( ch, DIR_NORTH, FALSE );
    return;
}



void do_east( CHAR_DATA *ch, char *argument )
{
    if (ch->fighting != NULL)
    {
	send_to_char("You are much too busy for that!\n\r", ch );
	return;
    }

    move_char( ch, DIR_EAST, FALSE );
    return;
}



void do_south( CHAR_DATA *ch, char *argument )
{
    if (ch->fighting != NULL)
    {
	send_to_char("You are much too busy for that!\n\r", ch );
	return;
    }

    move_char( ch, DIR_SOUTH, FALSE );
    return;
}



void do_west( CHAR_DATA *ch, char *argument )
{
    if (ch->fighting != NULL)
    {
	send_to_char("You are much too busy for that!\n\r", ch );
	return;
    }

    move_char( ch, DIR_WEST, FALSE );
    return;
}



void do_up( CHAR_DATA *ch, char *argument )
{
    if (ch->fighting != NULL)
    {
	send_to_char("You are much too busy for that!\n\r", ch );
	return;
    }

    move_char( ch, DIR_UP, FALSE );
    return;
}



void do_down( CHAR_DATA *ch, char *argument )
{
    if (ch->fighting != NULL)
    {
	send_to_char("You are much too busy for that!\n\r", ch );
	return;
    }

    move_char( ch, DIR_DOWN, FALSE );
    return;
}



int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;

	 if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else
    {
	for ( door = 0; door <= 5; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL
	    &&   is_door( pexit, ch )
	    &&   pexit->keyword != NULL
	    &&   is_name( arg, pexit->keyword ) )
		return door;
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR, TRUE );
	return -1;
    }

    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	act( "I see no door $T here.", ch, NULL, arg, TO_CHAR, TRUE );
	return -1;
    }

    if ( !is_door( pexit, ch ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return -1;
    }

    return door;
}



void do_open( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int door;
  CharIterator citer;

  if ( !can_interact( ch )
  && !IS_SET( ch->in_room->area->area_flags, AREA_DISABLED ) )
  {
    send_to_char( "If you are not in the main war, you may only affect doors in disabled zones.\n\r", ch );
    return;
  }
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }
    
    if ( IS_NPC(ch) )
	return;

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'open object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	send_to_char( "Ok.\n\r", ch );
	act( "$n opens $p.", ch, obj, NULL, TO_ROOM, TRUE );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !is_door_closed( pexit, ch ) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( is_door_locked( pexit, ch ) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

        open_door( pexit, door, ch );
	act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM, TRUE );
	send_to_char( "Ok.\n\r", ch );

	/* open the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

          for ( rch = citer.StartPhase( ch->phase, to_room, CharIterator::ROOM );
	        rch != NULL; rch = citer.Next() )
            act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR, TRUE );
	}
    }

    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int door;
  CharIterator citer;

  if ( !can_interact( ch )
  && !IS_SET( ch->in_room->area->area_flags, AREA_DISABLED ) )
  {
    send_to_char( "If you are not in the main war, you may only affect doors in disabled zones.\n\r", ch );
    return;
  }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }

	SET_BIT(obj->value[1], CONT_CLOSED);
	send_to_char( "Ok.\n\r", ch );
	act( "$n closes $p.", ch, obj, NULL, TO_ROOM, TRUE );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'close door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( is_door_closed( pexit, ch ) )
	{
	    send_to_char( "It's already closed.\n\r",    ch );
	    check_failed_combat( ch );
	    WAIT_STATE( ch, 2 );
	    return;
	}

        close_door( pexit, door, ch );
	act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM, TRUE );
	send_to_char( "Ok.\n\r", ch );

	/* close the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

          for ( rch = citer.StartPhase( ch->phase, to_room, CharIterator::ROOM );
	        rch != NULL; rch = citer.Next() )
          act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR, TRUE );
	}
    }

    return;
}


bool is_door_closed( EXIT_DATA *pexit, CHAR_DATA *ch )
{
  if ( ch->phase >= NUM_PHASES )
    return TRUE;

  return IS_SET(pexit->exit_info[ch->phase], EX_CLOSED);
}

bool is_door( EXIT_DATA *pexit, CHAR_DATA *ch )
{
  if ( ch->phase >= NUM_PHASES )
    return TRUE;

  return IS_SET(pexit->exit_info[ch->phase], EX_ISDOOR);
}

bool is_door_locked( EXIT_DATA *pexit, CHAR_DATA *ch )
{
  if ( ch->phase >= NUM_PHASES )
    return FALSE;

  return IS_SET(pexit->exit_info[ch->phase], EX_LOCKED);
}

void close_door( EXIT_DATA *pexit, unsigned int door, CHAR_DATA *ch )
{
  ROOM_INDEX_DATA *to_room;
  EXIT_DATA *pexit_rev;

  if ( ch->phase >= NUM_PHASES )
    return;

  SET_BIT(pexit->exit_info[ch->phase], EX_CLOSED);

  /* open the other side */
  if ( ( to_room   = pexit->to_room               ) != NULL
  &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
  &&   pexit_rev->to_room == ch->in_room )
    SET_BIT( pexit_rev->exit_info[ch->phase], EX_CLOSED );
}

void open_door( EXIT_DATA *pexit, unsigned int door, CHAR_DATA *ch )
{
  ROOM_INDEX_DATA *to_room;
  EXIT_DATA *pexit_rev;

  if ( ch->phase >= NUM_PHASES )
    return;

  REMOVE_BIT(pexit->exit_info[ch->phase], EX_CLOSED);

  /* open the other side */
  if ( ( to_room   = pexit->to_room               ) != NULL
  &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
  &&   pexit_rev->to_room == ch->in_room )
    REMOVE_BIT( pexit_rev->exit_info[ch->phase], EX_CLOSED );
}

void lock_door( EXIT_DATA *pexit, unsigned int door, CHAR_DATA *ch )
{
  ROOM_INDEX_DATA *to_room;
  EXIT_DATA *pexit_rev;

  if ( ch->phase >= NUM_PHASES )
    return;

  SET_BIT(pexit->exit_info[ch->phase], EX_LOCKED);

  /* open the other side */
  if ( ( to_room   = pexit->to_room               ) != NULL
  &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
  &&   pexit_rev->to_room == ch->in_room )
    SET_BIT( pexit_rev->exit_info[ch->phase], EX_LOCKED );
}

void unlock_door( EXIT_DATA *pexit, unsigned int door, CHAR_DATA *ch )
{
  ROOM_INDEX_DATA *to_room;
  EXIT_DATA *pexit_rev;

  if ( ch->phase >= NUM_PHASES )
    return;

  REMOVE_BIT(pexit->exit_info[ch->phase], EX_LOCKED);

  /* open the other side */
  if ( ( to_room   = pexit->to_room               ) != NULL
  &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
  &&   pexit_rev->to_room == ch->in_room )
    REMOVE_BIT( pexit_rev->exit_info[ch->phase], EX_LOCKED );
}



bool has_key( CHAR_DATA *ch, int key )
{
  OBJ_DATA *obj;
  ObjIterator oiter;

  for ( obj = oiter.StartList( ch, ch->carrying );
        obj != NULL; obj = oiter.Next( ) )
  {
    if ( obj->pIndexData->vnum == key )
      return TRUE;
  }

  return FALSE;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

  if ( !can_interact( ch )
  && !IS_SET( ch->in_room->area->area_flags, AREA_DISABLED ) )
  {
    send_to_char( "If you are not in the main war, you may only affect doors in disabled zones.\n\r", ch );
    return;
  }
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks $p.", ch, obj, NULL, TO_ROOM, TRUE );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	EXIT_DATA *pexit;

	pexit	= ch->in_room->exit[door];
	if ( !is_door_closed( pexit, ch ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( is_door_locked( pexit, ch ) )
	{
          send_to_char( "It's already locked.\n\r",    ch );
          check_failed_combat( ch );
          WAIT_STATE( ch, skill_table[gsn_pick_lock].beats + 8 );
          return;
        }

        lock_door( pexit, door, ch );
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM, TRUE );
    }

    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

  if ( !can_interact( ch )
  && !IS_SET( ch->in_room->area->area_flags, AREA_DISABLED ) )
  {
    send_to_char( "If you are not in the main war, you may only affect doors in disabled zones.\n\r", ch );
    return;
  }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM, TRUE );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	EXIT_DATA *pexit;

	pexit = ch->in_room->exit[door];
	if ( !is_door_closed( pexit, ch ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !is_door_locked( pexit, ch ) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

        unlock_door( pexit, door, ch );
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM, TRUE );
    }

    return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;
    int skill;
    extern int war_total;

    if ( !can_interact( ch ) )
    {
	send_to_char("You aren't in the war.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

    skill = ch->pcdata->learned[gsn_pick_lock];
    if ( !IS_NPC( ch ) && mortal_in_main_war( ch ) && war_total > 10 )
      skill += war_total - 5;

    if ( !IS_NPC(ch) && number_percent( ) > skill )
    {
	send_to_char( "You failed.\n\r", ch);
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'pick object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
/* removing pick proff */
/*
	if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }
*/

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks $p.", ch, obj, NULL, TO_ROOM, TRUE );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	EXIT_DATA *pexit;

	pexit = ch->in_room->exit[door];
	if ( !is_door_closed( pexit, ch ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !is_door_locked( pexit, ch ) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
/* removing pick proof */
/*
	if ( IS_SET(pexit->exit_info, EX_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }
*/

        unlock_door( pexit, door, ch );
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM, TRUE );
    }

    return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position)
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	    { send_to_char( "You can't wake up!\n\r", ch ); return; }

	send_to_char( "You wake and stand up.\n\r", ch );
	act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM, TRUE );
	ch->position = POS_STANDING;
	break;

    case POS_RESTING:
	send_to_char( "You stand up.\n\r", ch );
	act( "$n stands up.", ch, NULL, NULL, TO_ROOM, TRUE );
	ch->position = POS_STANDING;
	break;

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    int temp;

    temp = ch->position;
    if (ch->fighting != NULL)
	temp = POS_FIGHTING;

    switch ( temp )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_STANDING:
	send_to_char( "You rest.\n\r", ch );
	act( "$n rests.", ch, NULL, NULL, TO_ROOM, TRUE );
	ch->position = POS_RESTING;
	break;

    case POS_FIGHTING:
/*
	send_to_char( "You are already fighting!\n\r", ch );
*/
	send_to_char( "Rest while fighting?   Are you mad??\n\r", ch );
	break;
    }

    return;
}



void do_sleep( CHAR_DATA *ch, char *argument )
{
    int temp;

    temp = ch->position;
    if (ch->fighting != NULL)
	temp = POS_FIGHTING;

    switch ( temp )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
    case POS_STANDING: 
	send_to_char( "You sleep.\n\r", ch );
	act( "$n sleeps.", ch, NULL, NULL, TO_ROOM, TRUE );
	ch->position = POS_SLEEPING;
	break;

    case POS_FIGHTING:
/*
	send_to_char( "You are already fighting!\n\r", ch );
*/
	send_to_char( "Sleep while fighting?   Are you mad???\n\r", ch );
	break;
    }

    return;
}



void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE(ch) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE(victim) )
	{ act( "$N is already awake.", ch, NULL, victim, TO_CHAR, TRUE ); return; }

    if ( IS_AFFECTED(victim, AFF_SLEEP) )
	{ act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR, TRUE );  return; }

    if ( !can_interact( ch ) )
    {
	send_to_char("You are't in the war.\n\r", ch );
	return;
    }

    act( "You wake $M.", ch, NULL, victim, TO_CHAR, TRUE );
    act( "$n wakes you.", ch, NULL, victim, TO_VICT, TRUE );
    victim->position = POS_STANDING;
    return;
}


void do_snare( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int mana;

    if ( IS_NPC( ch ) )
	return;

    if ( !can_interact( ch ) )
    {
	send_to_char("You aren't in the war.\n\r", ch );
	return;
    }

    if ( ch->in_room == NULL )
    {
	send_to_char("Error, you aren't in a room.\n\r", ch );
	return;
    }

    mana = IS_NPC(ch) ? 0
         : ( class_table[ch->char_class].skill_cost[gsn_snare] == 0
	    ? UMAX( skill_table[gsn_snare].min_mana, 50 )
	    : class_table[ch->char_class].skill_cost[gsn_snare] );

    if ( ch->mana < mana )
    {
	send_to_char("You don't have enough mana.\n\r", ch );
	return;
    }

    ch->mana -= mana;

    if ( !IS_NPC( ch )
    && number_percent( ) > ch->pcdata->learned[gsn_snare] )
    {
	send_to_char("Your snare line snapped.\n\r", ch );
	return;
    }

    obj = create_object( get_obj_index( 41 ), ch->phase );
    obj_to_room( obj, ch->in_room );

    send_to_char("You set a snare.\n\r", ch );
    act( "$n sets a snare.", ch, NULL, NULL, TO_ROOM, TRUE );
    return;
}


void do_sneak( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;

  if (IS_NPC(ch))
    return;

  if ( IS_AFFECTED( ch, AFF_SNEAK ) )
  {
    affect_strip( ch, gsn_sneak );
    send_to_char( "You stop trying to sneak.\n\r", ch );
  }
  else
  {
    send_to_char( "You begin trying to sneak.\n\r", ch );

    af.type      = gsn_sneak;
    af.duration  = -1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SNEAK;
    affect_to_char( ch, &af );
  }

  return;
}



void do_hide( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED(ch, AFF_HIDE) )
	REMOVE_BIT(ch->affected_by, AFF_HIDE);

    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_hide] )
	SET_BIT(ch->affected_by, AFF_HIDE);

    WAIT_STATE( ch, 8 );

    return;
}



/*
 * Contributed by Alander.
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
    affect_strip ( ch, gsn_invis			);
    affect_strip ( ch, gsn_mass_invis			);
    affect_strip ( ch, gsn_sneak			);
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_recall( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;

    if (IS_NPC(ch))
	return;

    if ( in_static_war( ch ) )
    {
	send_to_char("Wait till you are dead.\n\r", ch );
	return;
    }
  
    act( "$n prays for transportation!", ch, 0, 0, TO_ROOM, TRUE );

    if ( ( location = get_room_index( ROOM_VNUM_TEMPLE_OF_YS ) ) == NULL )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( in_static_war( ch )
    && ( !IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
	||   IS_AFFECTED(ch, AFF_CURSE) ) )
    {
	send_to_char( "God has forsaken you.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) != NULL )
    {
	if ( number_bits( 1 ) == 0 )
	{
	    WAIT_STATE( ch, 4 );
	    send_to_char( buf, ch );
	    return;
	}

	send_to_char( buf, ch );
	stop_fighting( ch, TRUE );
    }

    act( "$n disappears.", ch, NULL, NULL, TO_ROOM, TRUE );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM, TRUE );
    do_look( ch, str_auto );

    return;
}



void do_tele( CHAR_DATA *ch , char *argument )
{
  extern int war_total;
  bool war_teleport = FALSE;

  if ( in_static_war( ch )
  && ( war_total > 0 || !IS_SET( ch->act, PLR_CAN_TELEPORT ) ) )
  {
    send_to_char( "Sure, but you would have to die first.\n\r", ch );
    return;
  }

  if ( in_private_war( ch ) )
  {
    send_to_char( "Private wars are restricted to a single zone.\n\r", ch );
    return;
  }

  if ( IS_SET( ch->act, PLR_CAN_TELEPORT ) )
  {
    send_to_char( "You use your one in-war teleport.\n\r", ch );
    act( "$n utilizes $s one teleport.", ch, NULL, NULL, TO_ROOM, TRUE );
    REMOVE_BIT( ch->act, PLR_CAN_TELEPORT );
    war_teleport = TRUE;
  }

  teleport_char( ch, war_teleport );

  return;
}


void teleport_char( CHAR_DATA *ch, bool fWar )
{
  ROOM_INDEX_DATA *pRoomIndex;
  EXIT_DATA *pexit;
  int iRoom;
  bool found = FALSE;

  for ( ; ; )
  {
    pRoomIndex = get_room_index( number_range( 0, 65535 ) );

    if ( pRoomIndex == NULL )
      continue;

    if ( !fWar )
      break;

    if ( IS_SET( pRoomIndex->area->area_flags, AREA_DISABLED ) )
      continue;

    if ( IS_SET( pRoomIndex->room_flags, ROOM_NO_START ) )
      continue;

    if ( IS_SET( pRoomIndex->area->area_flags, AREA_NO_START ) )
      continue;

    break;
  }

  char_from_room( ch );
  char_to_room( ch, pRoomIndex );

  for( iRoom = 0;iRoom <= 5;iRoom++ )
  {
    if( ( pexit = ch->in_room->exit[iRoom] ) != NULL 
    &&    pexit->to_room != NULL 
    &&    !is_door_closed( pexit, ch ) )
      found = TRUE;
  }

  if( !found )
  {
    teleport_char( ch, fWar );
    return;
  }

  do_look( ch, str_auto );
  return;
}

void do_scatter( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoomIndex;
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    static char str_scatter[11] = "do_scatter";
    ObjIterator oiter;

    argument = one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
        for ( obj = oiter.StartList( ch, ch->in_room->contents );
              obj != NULL; obj = oiter.Next( ) )
	{
	    if( !CAN_WEAR(obj, ITEM_TAKE) )
		continue;

	    for ( ; ; )	
	    {
		pRoomIndex = get_room_index( number_range( 0, 65535 ) );
	
		if ( pRoomIndex != NULL )
		    if ( !IS_SET( pRoomIndex->room_flags, ROOM_NO_START ) )
		    {
			obj_from_room( obj, str_scatter );
			obj_to_room( obj, pRoomIndex );
			sprintf(buf,"%s is now at %s.\n\r",obj->short_descr, pRoomIndex->name );
			send_to_char(buf, ch );
			act("$p falls down from the sky.",ch,obj,NULL,TO_ROOM, TRUE );
			break;
		    }
	    }
	}
    }

    return;
}

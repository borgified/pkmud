#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "bfs.h"
#include "global.h"

extern sh_int *quiver_colors[28];


/*
 * Assign mob a room to path to, ideally away from players.
 */
void get_new_target_room( CHAR_DATA *ch )
{
  int i;
  int dir;
  int attempts = 20;
  ROOM_INDEX_DATA *room;

#warning This should be rewritten to randomly select a room within 10 rooms.

  while ( attempts-- > 0 )
  {
    while ( 1 )
    {
      i = number_range( 1, 65535 );
      room = get_room_index( i );
      if ( room != NULL )
        break;
    }

    dir = bfs_find_room( ch, ch->in_room, room, 1000 );
    if ( dir >= 0 && dir <= 5 )
    {
      ch->data[0] = i;
      return;
    }
  }

  return;
}


int search_ranged_char( CHAR_DATA *ch, CHAR_DATA *victim, int range )
{
  int direction;
  int i;
  ROOM_INDEX_DATA *in_room;
  ROOM_INDEX_DATA *search_room;
  ROOM_INDEX_DATA *to_room;

  if ( ch->in_room == NULL )
    return -1;

  if ( victim->in_room == NULL )
    return -1;

  in_room = ch->in_room;
  to_room = victim->in_room;

  if ( in_room == to_room )
    return -1;

  for ( direction = DIR_NORTH; direction <= DIR_DOWN; direction++ )
  {
    search_room = in_room;

    for ( i = 0; i < range; i++ )
    {
      if ( search_room->exit[direction] == NULL
      || search_room->exit[direction]->to_room == NULL )
        break;

      search_room = search_room->exit[direction]->to_room;

      if ( search_room == to_room )
        return direction;
    }
  }

  return -1;
}


bool spec_archer_hunter( CHAR_DATA *mob )
{
  OBJ_DATA *wield;
  OBJ_DATA *quiver;
  char buf[MAX_STRING_LENGTH];
  int direction;

  if (mob->wait != 0)
    return FALSE;

  if (mob->hunting == NULL)
    return FALSE;

  if ( !mortal_in_main_war( mob->hunting ) )
    return FALSE;

  mob->mana = UMIN( mob->max_mana, mob->mana + 4 );

  if (mob->position != POS_STANDING)
  {
    if (!number_bits(1))
      do_stand( mob, "" );
    return FALSE;
  }

  if ( (wield = get_eq_char( mob, WEAR_WIELD )) == NULL  )
  {
    if ( (wield = get_obj_carry( mob, "bow" )) == NULL )
    {
      do_say( mob, "Damn.  Lost my weapon.  Good thing I've got another." );
      act("$n draws a long bow from his pack.", mob, NULL, NULL, TO_ROOM, TRUE );
      wield = create_object( get_obj_index(1), mob->phase );  /* long bow */
      obj_to_char( wield, mob );
    }
    else
      do_say( mob, "I hate when I drop my bow!" );

    equip_char( mob, wield, WEAR_WIELD );
    act("$n wields his bow.", mob, NULL, NULL, TO_ROOM, TRUE);
    WAIT_STATE( mob, 4 );
    return FALSE;
  }

  if ( ( quiver = get_eq_char( mob, WEAR_WAIST ) ) == NULL )
  {
    do_say( mob, "Out of arrows, again?" );
    act( "$n quickly makes some arrows.", mob, NULL, NULL, TO_ROOM, TRUE );
    quiver = create_object( get_obj_index( 4 ), mob->phase );  /* basic quiver */
    obj_to_char( quiver, mob );
    equip_char( mob, quiver, WEAR_WAIST );
    WAIT_STATE( mob, 4 );
    return FALSE;
  }

  if ( quiver->value[2] == 0 )
    quiver->value[2] = 25;

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
	act("$n quaffs a virtual swirling potion.",
	  mob, NULL, NULL, TO_ROOM, TRUE);
	spell_pass_door( gsn_pass_door, mob->level, mob, mob );
	WAIT_STATE( mob, 12 );
	return FALSE;
      }

    case 14: case 13: case 12: case 11:
    case 10: case 9: case 8: case 7: case 6:
    case 5:
      if (!IS_AFFECTED( mob, AFF_DETECT_INVIS ) )
      {
	act("$n quaffs a virtual yellow potion.",
	  mob, NULL, NULL, TO_ROOM, TRUE );
	spell_detect_invis( gsn_detect_invis, mob->level, mob, mob );
	WAIT_STATE( mob, 12 );
	return FALSE;
      }

      if (!IS_AFFECTED( mob, AFF_DETECT_HIDDEN ) )
      {
	act("$n quaffs a virtual orange potion.",
	  mob, NULL, NULL, TO_ROOM, TRUE );
	spell_detect_hidden( gsn_detect_hidden, mob->level, mob, mob );
	WAIT_STATE( mob, 12 );
	return FALSE;
      }

    case 4: case 3: case 2: case 1: case 0:
      default:
      break;
  }

  if ( mob->fighting != NULL
  && number_bits(2) == 0 )
  {
    do_say( mob, "Shit!  I can't melee!  I'm outta here!" );
    do_flee( mob, str_empty );
    if ( mob->fighting != NULL )
      do_flee( mob, str_empty );
    return FALSE;
  }

  /*
   * Bow is not ready to fire.
   */
  if ( mob->mana < mob->max_mana )
  {
    ROOM_INDEX_DATA *room = get_room_index( mob->data[0] );
    if ( room == NULL )
      room = get_room_index( 3001 );

    direction = bfs_find_room( mob, mob->in_room, room, 1000 );

    if ( direction < 0 || direction > 5 )
      get_new_target_room( mob );
    else
      move_char( mob, direction, TRUE );

    return FALSE;
  }

  /*
   * Bow is ready to fire!
   */

  /*
   * Track 'em down.
   */
  if ( search_ranged_char( mob, mob->hunting, 4 ) == -1 )
  {
    direction = bfs_find_room( mob, mob->in_room, mob->hunting->in_room, 1000 );

    if (direction < 0 || direction > 5)
    {
	sprintf(buf, "summon %s",
	  mob->hunting == NULL ? "" : mob->hunting->name );
	do_cast( mob, buf );
        return FALSE;
    }

    move_char( mob, direction, TRUE );
    return FALSE;
  }

  /*
   * In range.
   */

  /*
   * Special arrow?
   */
  if ( !number_bits( 3 ) )
    quiver->value[1] = *quiver_colors[number_range(0,27)];
  else
    quiver->value[1] = -1;

  do_shoot( mob, mob->hunting->name );
  get_new_target_room( mob );

  return FALSE;
}

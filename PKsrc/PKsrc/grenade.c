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
#include "global.h"
#include "CharIterator.h"



void do_pull( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;

  one_argument( argument, arg );

  if ( arg[0] == '\0' )
  {
    send_to_char( "Pull what?\n\r", ch );
    return;
  }

  obj = get_eq_char( ch, WEAR_HOLD );

  if ( obj == NULL || obj->item_type != ITEM_GRENADE )
  {
    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
      send_to_char( "You do not have that device.\n\r", ch );
      return;
    }
  }

  if ( obj->item_type != ITEM_GRENADE )
  {
    send_to_char( "You can only activate devices.\n\r", ch );
    return;
  }

  if ( obj->timer > 0 )
  {
    send_to_char( "It is already active!\n\r", ch );
    return;
  }

  act( "$n activates $p.", ch, obj, NULL, TO_ROOM, TRUE );
  act( "You activate $p.", ch, obj, NULL ,TO_CHAR, TRUE );

  obj->timer = obj->value[0];

  obj->killer = ch;

  if ( obj->timer <= 0 )
  {
    explode_grenade( obj );
    return;
  }
}



void explode_grenade( OBJ_DATA *obj )
{
  CHAR_DATA *holder = obj->carried_by;
  CHAR_DATA *victim;
  CHAR_DATA *observer;
  OBJ_DATA *container;
  ROOM_INDEX_DATA *room;
  EXIT_DATA *pexit;
  static char str_explode_grenade[16] = "explode grenade";
  CharIterator citer;
  CharIterator adjacent_citer;
  int door;
  int base_dam;
  int mod_dam;

  if ( obj == NULL )
    return;

  if ( holder != NULL )
  {
    room = holder->in_room;
  }
  else if ( obj->in_obj != NULL )
  {
    container = obj->in_obj;
    while ( container->in_obj != NULL )
      container = container->in_obj;

    if ( container->carried_by != NULL )
    {
      holder = container->carried_by;
      room = holder->in_room;
    }
    else
    {
      room = container->in_room;
    }
  }
  else
  {
    room = obj->in_room;
  } 
  if ( room == NULL )
  {
    bug( "Can't find room for grenade.", 0 );
    extract_obj( obj, str_explode_grenade );
    return;
  }

  victim = citer.StartPhase( obj->phase, room, CharIterator::ROOM );

  if ( victim != NULL )
  {
    if ( holder != NULL )
    {
      act( "$p explodes in $n's hands!", holder, obj, NULL, TO_ROOM, TRUE );
      act( "$p explodes in your hands!", holder, obj, NULL, TO_CHAR, TRUE );
    }
    else
    {
      act( "$p explodes!", victim, obj, NULL, TO_ROOM, TRUE );
      act( "$p explodes!", victim, obj, NULL, TO_CHAR, TRUE );
    }
  }

  for ( door = 0; door < 6; door++ )
  {
    if ( ( pexit = room->exit[door] ) == NULL
      || pexit->to_room == NULL )
      continue;

    observer = adjacent_citer.StartPhase( obj->phase, pexit->to_room,
                                          CharIterator::ROOM );

    if ( observer == NULL )
      continue;

    act( "You hear an explosion from $T.", observer, NULL,
         dir_name_2[rev_dir[door]], TO_ROOM, true );
    act( "You hear an explosion from $T.", observer, NULL,
         dir_name_2[rev_dir[door]], TO_CHAR, true );
  }

  /* Base damage */
  base_dam = obj->value[1] - ( obj->value[2] / 2 )
           + number_range( 1, obj->value[2] );

  if ( base_dam > 0 )
  {
    /* We initialized iterator when checking for players */
    for ( ; victim != NULL; victim = citer.Next() )
    {
      if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS) )
        continue;

      mod_dam = base_dam;
      if ( holder != NULL )
      {
        if ( victim == holder )
          mod_dam *= 2;
        else
          mod_dam /= 2;
      }

      damage( ( obj->killer != NULL ? obj->killer : victim ),
              victim, mod_dam, gsn_explosion, 0 );
    }
  }

  extract_obj( obj, str_explode_grenade );

  return;
}





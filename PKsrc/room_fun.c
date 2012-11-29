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
DECLARE_ROOM_SPECIAL(	spec_boing		);
DECLARE_ROOM_SPECIAL(   spec_dump		);
DECLARE_ROOM_SPECIAL(   spec_sayno		);
DECLARE_ROOM_SPECIAL(   spec_scouts_guild	);


ROOM_SPECIAL *room_fun_lookup( const char *name )
{
    if ( !str_cmp( name, "spec_boing"            ) ) return spec_boing;
    if ( !str_cmp( name, "spec_dump"             ) ) return spec_dump;
    if ( !str_cmp( name, "spec_sayno"		 ) ) return spec_sayno;
    if ( !str_cmp( name, "spec_scouts_guild"	 ) ) return spec_scouts_guild;
    return 0;
}


bool spec_sayno( CHAR_DATA *ch, char *command, char *argument )
{
	ROOM_INDEX_DATA *chatroom;

	if ((chatroom = get_room_index( 1200 )) == NULL )
	{
		return TRUE;
	}

	if (!str_prefix( command, "immtalk") ||
	    !str_prefix( command, ":") )
	{
		do_immtalk( ch, argument );
		if (!str_cmp( argument, "anyone here?"))
		{
			do_recho( ch, "Someone: No!" );
		}
		return FALSE;
	}

	return TRUE;
}

bool spec_boing( CHAR_DATA *ch, char *command, char *argument )
{
	ROOM_INDEX_DATA	*storeroom;

	if ((storeroom = get_room_index( 2452 )) == NULL )
	{
		return TRUE;
	}

    if (!str_prefix( command, "boing" ) )
    {
	char_from_room( ch );
	char_to_room( ch, storeroom );
	do_look( ch, "" );
	return FALSE;
    }
	return TRUE;
}

bool spec_dump( CHAR_DATA *ch, char *command, char *argument )
{
    while ( ch->in_room->contents != NULL )
    {
	act("$p disappears into the rubble.", ch, ch->in_room->contents, NULL, TO_ROOM, TRUE );
	act("$p disappears into the rubble.", ch, ch->in_room->contents, NULL, TO_CHAR, TRUE );
	extract_obj( ch->in_room->contents, "spec_dump" );
    }
    return TRUE;
}

bool spec_scouts_guild( CHAR_DATA *ch, char *command, char *argument )
{
  CHAR_DATA *victim;
  CHAR_DATA *hch;
  int scout_cost = 200;

  if ( str_cmp( command, "send" ) )
    return TRUE;

  if ( argument == NULL || argument[0] == '\0' )
  {
    send_to_char("Send after who?\n\r", ch );
    return FALSE;
  }

  victim = get_char_world(  ch, argument );

  if ( victim == NULL )
  {
    send_to_char("No such character.\n\r", ch );
    return FALSE;
  }

  if ( IS_NPC( victim ) )
  {
    send_to_char("PCs only.\n\r", ch );
    return FALSE;
  }

  if ( !mortal_in_main_war( victim ) )
  {
    send_to_char("In-war characters only.\n\r", ch );
    return FALSE;
  }

  for ( hch = char_list; hch != NULL; hch = hch->next )
  {
    if ( hch->hunting && hch->hunting == victim )
    {
      send_to_char( "There is already something hunting them.\n\r", ch );
      return FALSE;
    }
  }

  if ( ch->gold < scout_cost )
  {
    send_to_char("Scouts cost 200 gold.\n\r", ch );
    return FALSE;
  }

  ch->gold -= scout_cost;

  hch = create_mobile( get_mob_index( 15 ) );

  hch->hunting = victim;
  hch->max_hit = 300;
  hch->hit = hch->max_hit;
  hch->max_mana = 60;
  hch->mana = hch->max_mana;
  hch->level = 20;
  hch->gold = 1;
  hch->char_class = CLASS_THIEF;

  char_to_room( hch, ch->in_room );
  {
    char buf[MAX_STRING_LENGTH];

    sprintf(buf,"%s shall spy on %s!",hch->short_descr,victim->name);
    do_warmessage(ch,buf);
  }

  return FALSE;
}

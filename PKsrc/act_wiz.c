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


BAN_DATA *		ban_free;
BAN_DATA *		ban_list;
BAN_DATA *		restrict_list;
BAN_DATA *		no_note_list;
BAN_DATA *		no_rating_list;

/*
 * Local functions.
 */
ROOM_INDEX_DATA *	find_location	args( ( CHAR_DATA *ch, char *arg ) );





void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
 
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level >= LEVEL_HERO
        &&   cmd_table[cmd].level <= get_trust( ch ) )
	{
	    sprintf( buf, "%d: %-12s", cmd_table[cmd].level, cmd_table[cmd].name );
	    send_to_char( buf, ch );
	    if ( ++col % 5 == 0 )
		send_to_char( "\n\r", ch );
	}
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}



void do_bamfin( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) )
    {
	smash_tilde( argument );
	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
    }
    return;
}



void do_bamfout( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) )
    {
	smash_tilde( argument );
	free_string( ch->pcdata->bamfout );
	ch->pcdata->bamfout = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
    }
    return;
}


void do_warflags( CHAR_DATA *ch, char *argument )
{
  extern int war_flags;
  char arg[MAX_STRING_LENGTH];

  one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "Warflags: bloods continuous loadall doom noregen\n\r", ch );
    send_to_char( "  fountains teleall noquaff \n\r", ch);
    return;
  }

  if( !str_cmp( arg, "healthlag" ) )
  {
    if( !IS_SET(war_flags, WAR_FLAG_HEALTHLAG ) )
    {
      SET_BIT(war_flags, WAR_FLAG_HEALTHLAG);
      do_warmessage(NULL,"Players low on health will now move slower!");
      return;
    }
    else
    {
      REMOVE_BIT(war_flags,WAR_FLAG_HEALTHLAG);
      do_warmessage(NULL,"Players low on health will no longer move slower." );
      return;
    }
    return;
  }

  if( !str_cmp( arg, "noquaff" ) )
  {
    if( !IS_SET(war_flags, WAR_FLAG_NOQUAFF ) )
    {
      SET_BIT(war_flags, WAR_FLAG_NOQUAFF);
      do_warmessage(NULL,"Potion quaffing has been disabled!");
      return;
    }
    else
    {
      REMOVE_BIT(war_flags,WAR_FLAG_NOQUAFF);
      do_warmessage(NULL,"Potion quaffing has been enabled." );
      return;
    }
    return;
  }

  if( !str_cmp( arg, "teleall" ) )
  {
    if( !IS_SET(war_flags, WAR_FLAG_TELEALL ) )
    {
      SET_BIT(war_flags, WAR_FLAG_TELEALL);
      do_warmessage(NULL,"All shall be teleported at rebirth!");
      return;
    }
    else
    {
      REMOVE_BIT(war_flags,WAR_FLAG_TELEALL);
      do_warmessage(NULL,"Only players shall be teleported at rebirth." );
      return;
    }
    return;
  }

	if( !str_cmp( arg, "randomclass" ) )
	{
		if( !IS_SET( war_flags, WAR_FLAG_RANDOMCLASS ) )
		{
			SET_BIT(war_flags, WAR_FLAG_RANDOMCLASS );
			do_warmessage(NULL,"Your profession will be chosen by the gods." );
			return;
		}
		else
		{
			REMOVE_BIT(war_flags, WAR_FLAG_RANDOMCLASS );
			do_warmessage(NULL,"Your profession is now your choice." );
			return;
		}
		return;
	}

	if( !str_cmp( arg, "fountains" ) )
	{
		if( !IS_SET( war_flags, WAR_FLAG_FOUNTAINS ) )
		{
			SET_BIT(war_flags, WAR_FLAG_FOUNTAINS );
			do_warmessage(NULL,"Fountains have loaded!");
			return;
		}
		else
		{
			OBJ_DATA *obj;

			REMOVE_BIT(war_flags, WAR_FLAG_FOUNTAINS );
			do_warmessage(NULL,"The gods have stolen the fountains for their greedy selves!" );
			for(obj = object_list; obj != NULL; obj = obj->next )
			{
				if(obj->item_type != ITEM_FOUNTAIN)
					continue;

				obj_from_room( obj, "do_warflags" );
				extract_obj( obj, "do_warflags" );
			}
			return;
		}
		return;
	}

	if( !str_cmp( arg, "noregen" ) )
	{
		if( !IS_SET( war_flags, WAR_FLAG_NOREGEN ) )
		{
			SET_BIT( war_flags, WAR_FLAG_NOREGEN );
			do_warmessage(NULL,"The gods put down a great burden on mortal bodies." );
			return;
		}
		else
		{
			REMOVE_BIT( war_flags, WAR_FLAG_NOREGEN );
			do_warmessage(NULL,"The gods release their burden." );
			return;
		}
		return;
	}

	if( !str_cmp( arg, "summon" ) )
	{
		if( !IS_SET( war_flags, WAR_FLAG_SUMMON ) )
		{
			SET_BIT( war_flags, WAR_FLAG_SUMMON );
			do_warmessage(NULL,"Summoning players enabled." );
			return;
		}
		else
		{
			REMOVE_BIT( war_flags, WAR_FLAG_SUMMON );
			do_warmessage(NULL,"Summoning players disabled." );
			return;
		}
		return;
	}

	if( !str_cmp( arg, "loadall" ) )
	{
		if( !IS_SET( war_flags, WAR_FLAG_LOADALL ) )
		{
			SET_BIT( war_flags, WAR_FLAG_LOADALL );
			do_warmessage(NULL,"All items are loading!");
			return;
		}
		else
		{
			REMOVE_BIT( war_flags, WAR_FLAG_LOADALL );
			do_warmessage(NULL,"Items will load normally.");
			return;
		}
		return;
	}

	if( !str_cmp( arg, "doom" ) )
	{
		if( !IS_SET( war_flags, WAR_FLAG_DOOM ) )
		{
			SET_BIT( war_flags, WAR_FLAG_DOOM );
			do_warmessage(NULL, "The gods are now paying attention to the wars!" );
		}
		else
		{
			REMOVE_BIT( war_flags, WAR_FLAG_DOOM );
			do_warmessage(NULL, "The gods are now ignoring the wars." );
		}
		return;
	}

	if( !str_cmp( arg, "bloods" ) ) {

		if ( !IS_SET( war_flags, WAR_FLAG_BLOODS ) )
		{
			SET_BIT( war_flags, WAR_FLAG_BLOODS );
			do_warmessage(NULL, "Bloods enabled." );
		}
		else
		{
			REMOVE_BIT( war_flags, WAR_FLAG_BLOODS );
			do_warmessage(NULL, "Bloods disabled." );
		}
		return;
	}

	if( !str_cmp( arg, "continuous" ) ) {

		if( !IS_SET( war_flags, WAR_FLAG_CONTINUOUS ) ) 
		{
			SET_BIT( war_flags, WAR_FLAG_CONTINUOUS );
			set_join_status(ch);
			do_warmessage(NULL, "Continuous war enabled." );
		}
		else
		{
			REMOVE_BIT( war_flags, WAR_FLAG_CONTINUOUS );
			set_join_status(char_list);
			do_warmessage(NULL, "Continous war disabled." );
		}
		return;
	}
	return;

}



void do_deny( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Deny whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    SET_BIT(victim->act, PLR_DENY);
    send_to_char( "You are denied access!\n\r", victim );
    send_to_char( "OK.\n\r", ch );
    do_quit( victim, "" );

    return;
}



void do_disconnect( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Disconnect whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL )
    {
	act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR, TRUE );
	return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d == victim->desc )
	{
	    close_socket( d );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
    return;
}



void do_specho(CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	char arg1[MAX_STRING_LENGTH];
	extern bool write_to_descriptor( int desc, char *txt, int length );

	argument = one_argument( argument, arg1 );

	if( arg1[0] == '\0' )
	{
		send_to_char("Syntax: specho <all/person>\n\r", ch );
		return;
	}

	if( ch->pnote == NULL )
	{
		send_to_char("You must have something written in your note buffer!\n\r", ch );
		return;
	}

	if( !str_cmp(arg1, "all") )
	{
		for( d = descriptor_list; d != NULL; d = d->next )
		{
			if( d->character == NULL )
				continue;
			
			write_to_descriptor( d->descriptor, ch->pnote->text, 0 );
				continue;
		}
		return;
	}

	if( (victim = get_char_world(ch, arg1)) == NULL )
	{
		send_to_char("They aren't on.\n\r", ch );
		return;
	}
	else
	{
		write_to_descriptor( victim->desc->descriptor, ch->pnote->text, 0 );
		send_to_char("They saw:\n\r", ch );
		send_to_char( ch->pnote->text, ch );
		return;
	}

	return;
}

void do_echo( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
/*    char buf[MAX_STRING_LENGTH]; */
    
    if ( argument[0] == '\0' )
    {
	send_to_char( "Echo what?\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
	{
		{
			send_to_char( argument, d->character );
			send_to_char( "\n\r",   d->character );
		}
	}
    }

    return;
}



void do_recho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    
    if ( argument[0] == '\0' )
    {
	send_to_char( "Recho what?\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character->in_room == ch->in_room )
	{
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}
    }

    return;
}

void do_delay( CHAR_DATA *ch , char *argument )
{
    extern int war_status;
   
    if (war_status == WAR_ON)
    {
       send_to_char( "War is on, can't delay any longer!\n\r", ch );
       return;
    }
    set_war_status( NO_WAR );
    do_echo( ch, "War has been delayed!" );
    return;
}

void do_wartime( CHAR_DATA *ch, char *argument ) {

	extern int war_timer;
	extern int war_status;
	extern int war_flags;
	extern int war_death;
	int war_before;
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if( !is_number(arg2) || arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "wartime <war or death> <time>\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "death" ) )
	{
		war_before = war_death;
		war_death = atoi(arg2);
		sprintf( buf,"The gods are %s and choose to give you %d tick%s to kill, or face their wrath!", war_death > war_before ? "happy" : "annoyed",war_death, war_death == 1 ? "" : "s" );
		do_warmessage( NULL, buf );
		return;
	}
	else if ( !str_cmp( arg1, "war" ) )
	{

	if( war_status == NO_WAR ) {

		send_to_char( "War is off, can only set at post, pre, and during war.\n\r", ch );
		return;
	}

	war_timer = atoi(arg2);

	switch( war_status ) {

		case PRE_WAR:
			sprintf( buf, "%d tick%s until war starts.", war_timer, war_timer == 1 ? "" : "s" );
			do_warmessage( NULL, buf );
			break;
		case POST_WAR:
			sprintf( buf, "%d tick%s until reset.", war_timer, war_timer == 1 ? "" : "s" );
			do_warmessage( NULL, buf );
			break;
		case WAR_ON:
			if( !IS_SET( war_flags, WAR_FLAG_CONTINUOUS ) )
				sprintf( buf, "Doomsday device is now set at %d tick%s.", war_timer, war_timer == 1 ? "" : "s" );
			else
				sprintf( buf, "World rebirth is now set at %d tick%s.", war_timer, war_timer == 1 ? "" : "s" );

			do_warmessage( NULL, buf );
			break;
	}
	}
	else
	{
		send_to_char("Syntax: wartime <death/war> <time>\n\r", ch );
		return;
	}

	return;
}

void do_warstart( CHAR_DATA *ch , char *argument )
{
    DESCRIPTOR_DATA *d;
    extern int war_status;
    extern int max_restore_hits;
    extern int max_restore_mana;
    extern int war_flags;
    extern int war_timer;

    max_restore_hits = 80;
    max_restore_mana = 80;

/* make sure war hasnt already begun!! */
    if (war_status > PRE_WAR)
    {
         send_to_char( "The war is already on!!!!\n\r", ch );
         return;
    }

    do_warmessage( NULL, "The war has begun!!!" );

    set_war_status( WAR_ON );

    for ( d = descriptor_list ; d != NULL; d = d->next )
    {
         if ( d->connected == CON_PLAYING
          && d->character->level == 30
          && (IS_NPC(ch) ? 0 : IS_SET( ch->act, PLR_NOPLAY ) )
          && d->character->in_room != NULL )
         {
		d->character->nojoin = 0;
              do_joinwar( d->character, NULL );
	
		if( !IS_SET( war_flags, WAR_FLAG_CONTINUOUS ) )
			war_timer += 4;

		war_timer = UMIN( 30, war_timer );
         }
     }

    if( IS_SET( war_flags, WAR_FLAG_CONTINUOUS ) )
		war_timer = 40;

     return;
}




ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if ( is_number(arg) )
	return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world( ch, arg ) ) != NULL )
    {
	if (IS_NPC(victim) || get_trust(ch) > 30)
	    return victim->in_room;
	else
	    return NULL;
    }

    if (get_trust(ch) < 31)
	return NULL;

    if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
	return obj->in_room;

    return NULL;
}



void do_transfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

	if(IS_NPC(ch)) return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Transfer whom (and where)?\n\r", ch );
	return;
    }

    if( !str_cmp(arg1, "1" ) )
    {
	SET_BIT(ch->act,PLR_TRANS);
	send_to_char("Players may trans you now.\n\r",ch);
	return;
    }
    if( !str_cmp(arg1, "0" ) )
    {
	REMOVE_BIT(ch->act,PLR_TRANS);
	send_to_char("Players may not trans you now.\n\r",ch);
	return;
    }


    if ( !str_cmp( arg1, "all" ) )
    {
	if( get_trust( ch ) < 39 )
	{
		send_to_char("Don't think so...\n\r",ch);
		return;
	}

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   d->character != ch
	    &&   d->character->in_room != NULL
	    &&   can_see( ch, d->character ) )
	    {
		char buf[MAX_STRING_LENGTH];
		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_transfer( ch, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' || get_trust( ch ) < 39 )
    {
	location = ch->in_room;
    }
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}

	if ( room_is_private( location ) )
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "You're already here.\n\r", ch );
	return;
    }

    if ( victim->in_room == NULL )
    {
	send_to_char( "They are in limbo.\n\r", ch );
	return;
    }

    if( get_trust( ch ) < LEVEL_IMMORTAL
    && ( in_static_war( ch )
       || !IS_SET(victim->act,PLR_TRANS)
       || IS_NPC(victim)
       || in_static_war( victim ) ) )
    {
	send_to_char("You cannot:\n\r",ch);
	send_to_char("            trans if you are in war\n\r",ch);
	send_to_char("            trans players that in war\n\r",ch);
	send_to_char("            trans mobs\n\r",ch);
        send_to_char("            trans players that do not have trans on.\n\r",ch);
	send_to_char("\n\rSee: help trans\n\r",ch);
	return;
    }

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    act( "$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM, TRUE );
    char_from_room( victim );
    char_to_room( victim, location );
    act( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM, TRUE );
    if ( ch != victim )
	act( "$n has transferred you.", ch, NULL, victim, TO_VICT, TRUE );
    do_look( victim, "auto" );
    send_to_char( "Ok.\n\r", ch );
}

void do_at( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    CHAR_DATA *wch;
    
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "At where what?\n\r", ch );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    break;
	}
    }

    return;
}



void do_goto( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;


    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Goto where?\n\r", ch );
	return;
    }

    if ( in_static_war( ch ) )
    {
	send_to_char( "You can only use goto when not in the war.\n\r", ch );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( location->vnum == 3 && ( IS_NPC(ch) || str_cmp( ch->name, "rip" ) ) )
    {
	send_to_char("But.. that's Rip's room.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );
    if ( !IS_SET(ch->act, PLR_WIZINVIS) )
    {
	act( "$n $T.", ch, NULL,
	    (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
	    ? ch->pcdata->bamfout : "leaves in a swirling mist",
	    TO_ROOM, FALSE );
    }

    char_from_room( ch );
    char_to_room( ch, location );

    if ( !IS_SET(ch->act, PLR_WIZINVIS) )
    {
	act( "$n $T.", ch, NULL,
	    (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
	    ? ch->pcdata->bamfin : "appears in a swirling mist",
	    TO_ROOM, FALSE );
    }

#if 0
    if ( get_trust( ch ) < LEVEL_IMMORTAL )
    {
      extern time_t war_number;

      ch->pcdata->snoop_war = war_number;
    }
#endif

    do_look( ch, "auto" );
    return;
}



void do_rstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    int door;

    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( location == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( ch->in_room != location && room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    sprintf( buf, "Name: '%s.'\n\rArea: '%s' (%s).\n\r",
	location->name,
	location->area->name, location->area->author );
    send_to_char( buf, ch );

    sprintf( buf, "Area flags: %d.  Area access percent: %d.\n\r",
	location->area->area_flags, location->area->access_percent );
    send_to_char( buf, ch );

    sprintf( buf,
	"Vnum: %d.  Sector: %d.  Light: %d.\n\r",
	location->vnum,
	location->sector_type,
	location->light );
    send_to_char( buf, ch );

    sprintf( buf,
	"Room flags: %d.\n\rDescription:\n\r%s",
	location->room_flags,
	location->description );
    send_to_char( buf, ch );

    if ( location->extra_descr != NULL )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char( "Extra description keywords: '", ch );
	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
		send_to_char( " ", ch );
	}
	send_to_char( "'.\n\r", ch );
    }

    send_to_char( "Characters:", ch );
    for ( rch = location->people; rch; rch = rch->next_in_room )
    {
      if (can_see( ch, rch ) )
      {
	send_to_char( " ", ch );
	one_argument( rch->name, buf );
	send_to_char( buf, ch );
      }
    }

    send_to_char( ".\n\rObjects:   ", ch );
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
	send_to_char( " ", ch );
	one_argument( obj->name, buf );
	send_to_char( buf, ch );
    }
    send_to_char( ".\n\r", ch );

    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = location->exit[door] ) != NULL )
	{
	    sprintf( buf,
		"Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",

		door,
		pexit->to_room != NULL ? pexit->to_room->vnum : 0,
	    	pexit->key,
	    	pexit->exit_info,
	    	pexit->keyword,
	    	pexit->description[0] != '\0'
		    ? pexit->description : "(none).\n\r" );
	    send_to_char( buf, ch );
	}
    }

    return;
}



void do_ostat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
    extern int war_status;

    if ( war_status < WAR_ON && get_trust( ch ) < 40 )
    {
	send_to_char("You can't ostat yet.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Ostat what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    sprintf( buf, "Name: %s.\n\r",
	obj->name );
    send_to_char( buf, ch );

    sprintf( buf, "Vnum: %d.  Type: %s.\n\r",
	obj->pIndexData->vnum, 
	item_type_name( obj->item_type, obj->pIndexData->vnum ) );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s.\n\rLong description: %s\n\r",
	obj->short_descr, obj->description );
    send_to_char( buf, ch );

    sprintf( buf, "Wear bits: %d.  Extra bits: %s.\n\r",
	obj->wear_flags, extra_bit_name( obj->extra_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Number: %d/%d.  Weight: %d/%d.\n\r",
	1,           get_obj_number( obj ),
	obj->weight, get_obj_weight( obj ) );
    send_to_char( buf, ch );

    sprintf( buf, "Cost: %d.  Timer: %d.\n\r",
	obj->cost, obj->timer );
    send_to_char( buf, ch );

    sprintf( buf,
	"In room: %d.  In object: %s.  Carried by: %s.  Wear_loc: %d.\n\r",
	obj->in_room    == NULL    ?        0 : obj->in_room->vnum,
	obj->in_obj     == NULL    ? "(none)" : obj->in_obj->short_descr,
	obj->carried_by == NULL    ? "(none)" : 
	  (can_see( ch, obj->carried_by) ? obj->carried_by->name : ch->name ),
	obj->wear_loc );
    send_to_char( buf, ch );
    
    sprintf( buf, "Values: %d %d %d %d.\n\r",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
    send_to_char( buf, ch );

    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char( "Extra description keywords: '", ch );

	for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
		send_to_char( " ", ch );
	}

	for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
		send_to_char( " ", ch );
	}

	send_to_char( "'.\n\r", ch );
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d.\n\r",
	    affect_loc_name( paf->location ), paf->modifier );
	send_to_char( buf, ch );
    }

    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d.\n\r",
	    affect_loc_name( paf->location ), paf->modifier );
	send_to_char( buf, ch );
    }

    if ( obj->wear_fun != 0 )
	send_to_char( "Object has wear fun.\n\r", ch );

    return;
}



void do_mstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mstat whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

/*
    if( !IS_NPC(victim) && get_trust( ch ) > 39 && victim->desc
    && victim->desc->snoop_by && victim->desc->snoop_by->character )
    {
	sprintf( buf, "Snooped_by: %s\n\r",
	    victim->desc->snoop_by->character->name );
	send_to_char( buf, ch );
    }
*/

    if ( !IS_NPC(victim) && get_trust( victim ) )
	sprintf( buf, "Trust: %d\n\r", get_trust( victim ) );

    sprintf( buf, "Name: %s. Wizinvis: %d  Hunting: %s\n\r",
	victim->name, IS_NPC(victim) ? 0 : victim->pcdata->wizinvis_level,
	IS_NPC(victim) ? victim->hunting == NULL ? "Noone." : victim->hunting->name : "PC." );
    send_to_char( buf, ch );

    sprintf( buf, "Vnum: %d.  Sex: %s.  Room: %d.\n\r",
	IS_NPC(victim) ? victim->pIndexData->vnum : 0,
	victim->sex == SEX_MALE    ? "male"   :
	victim->sex == SEX_FEMALE  ? "female" : "neutral",
	victim->in_room == NULL    ?        0 : victim->in_room->vnum
	);
    send_to_char( buf, ch );

    sprintf( buf, "Str: %d.  Int: %d.  Wis: %d.  Dex: %d.  Con: %d.  Move lag: %d\n\r",
	get_curr_str(victim),
	get_curr_int(victim),
	get_curr_wis(victim),
	get_curr_dex(victim),
	get_curr_con(victim),
	victim->move_lag );
    send_to_char( buf, ch );

    sprintf( buf, "Hp: %d/%d.  Mana: %d/%d.  Practices: %d.\n\r",
	victim->hit,         victim->max_hit,
	victim->mana,        victim->max_mana,
	victim->practice );
    send_to_char( buf, ch );

    sprintf( buf, "Regen bonuses: Hit: %d%%  Mana: %d%%\n\r",
        victim->hit_regen_bonus, victim->mana_regen_bonus );
    send_to_char( buf, ch );
	
    sprintf( buf,
	"Lv: %d.  Class: %s (%d).  AC: %d.  Gold: %d.  Exp: %d.\n\r",
	victim->level,       class_table[victim->char_class].who_name, victim->char_class,
	GET_AC(victim),      victim->gold,         victim->exp );
    send_to_char( buf, ch );

    sprintf( buf, "Hitroll: %d.  Damroll: %d.  Eq p: %d.  Position: %d.\n\r",
	GET_HITROLL(victim), GET_DAMROLL(victim),
	victim->eq_points,
	victim->position );
    send_to_char( buf, ch );
	
    sprintf( buf, "Fighting: %s.\n\r",
	victim->fighting ? victim->fighting->name : "(none)" );
    send_to_char( buf, ch );

    if ( !IS_NPC(victim) )
    {
	sprintf( buf,
	    "Wins: %d.  Deaths: %d.  Kills: %d.  Saving throw: %d.\n\r",
	    victim->pcdata->wins,
	    victim->pcdata->deaths,
	    victim->pcdata->kills,
	    victim->saving_throw );
	send_to_char( buf, ch );
    }

    sprintf( buf, "Carry number: %d.  Carry weight: %d.\n\r",
	victim->carry_number, victim->carry_weight );
    send_to_char( buf, ch );

    sprintf( buf, "Age: %d.  Played: %d.  Timer: %d.  Act: %d.\n\r",
	get_age( victim ), (int) victim->played, victim->timer, victim->act );
    send_to_char( buf, ch );

    sprintf( buf, "Master: %s.  Leader: %s.  Affected by: %s.\n\r",
	victim->master      ? victim->master->name   : "(none)",
	victim->leader      ? victim->leader->name   : "(none)",
	affect_bit_name( victim->affected_by ) );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s.\n\rLong  description: %s",
	victim->short_descr,
	victim->long_descr[0] != '\0' ? victim->long_descr : "(none).\n\r" );
    send_to_char( buf, ch );

    if ( IS_NPC(victim) && victim->spec_fun != 0 )
	send_to_char( "Mobile has spec fun.\n\r", ch );

    if ( IS_NPC(victim) && victim->interactive != 0 )
	send_to_char( "Mobile has interactive spec fun.\n\r", ch );

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
    {
	sprintf( buf,
	    "Spell: '%s' modifies %s by %d for %d hours with bits %s.\n\r",
	    skill_table[(int) paf->type].name,
	    affect_loc_name( paf->location ),
	    paf->modifier,
	    paf->duration,
	    affect_bit_name( paf->bitvector )
	    );
	send_to_char( buf, ch );
    }

    return;
}



void do_mfind( CHAR_DATA *ch, char *argument )
{
    extern int top_mob_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Mfind whom?\n\r", ch );
	return;
    }

    fAll	= !str_cmp( arg, "all" );
    found	= FALSE;
    nMatch	= 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( fAll || is_name( arg, pMobIndex->player_name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\n\r",
		    pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
		send_to_char( buf, ch );
	    }
	}
    }

    if ( !found )
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );

    return;
}



void do_ofind( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    int total = 0;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Ofind what?\n\r", ch );
	return;
    }

    fAll	= !str_cmp( arg, "all" );
    found	= FALSE;
    nMatch	= 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
	if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( fAll || is_name( arg, pObjIndex->name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\n\r",
		    pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
		send_to_char( buf, ch );

		total++;

		if ( total > 20 && !IS_IMMORTAL( ch ) )
		{
		    send_to_char("To prevent abuse, mortals may only find 20 items at a time.\n\r", ch );
		    break;
		}
	    }
	}
    }

    if ( !found )
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );

    return;
}


void do_vostat( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pOI;
    AFFECT_DATA *paf;
    char buf[MAX_STRING_LENGTH];

    if ( !is_number( argument ) )
    {
	send_to_char( "Vnum must be a number.\n\r", ch );
	return;
    }

    pOI = get_obj_index( atoi( argument ) );

    if ( pOI == NULL )
    {
	send_to_char("No such object.\n\r", ch );
	return;
    }

    sprintf( buf, "Namelist: %s.    ",
	pOI->name );
    send_to_char( buf, ch );

    sprintf( buf, "Type: %s.\n\r",
	item_type_name( pOI->item_type, pOI->vnum ) );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s.\n\rLong description: %s\n\r",
	pOI->short_descr, pOI->description );
    send_to_char( buf, ch );

    sprintf( buf,
      "Weight: %d,  Cost: %d.  Load Percent: %d%%.\n\r",
	pOI->weight, pOI->cost, pOI->load_percent );
    send_to_char( buf, ch );

    switch( pOI->item_type )
    {
      default:
	sprintf( buf, "Values: %d  %d  %d  %d\n\r",
	    pOI->value[0], pOI->value[1], pOI->value[2], pOI->value[3] );
	send_to_char( buf, ch );
	break;
      case ITEM_SCROLL:
      case ITEM_POTION:
        sprintf( buf, "Level %d spells of:", pOI->value[0] );
        send_to_char( buf, ch );

        if ( pOI->value[1] >= 0 && pOI->value[1] < MAX_SKILL )
        {
            send_to_char( " '", ch );
            send_to_char( skill_table[pOI->value[1]].name, ch );
            send_to_char( "'", ch );
        }

        if ( pOI->value[2] >= 0 && pOI->value[2] < MAX_SKILL )
        {
            send_to_char( " '", ch );
            send_to_char( skill_table[pOI->value[2]].name, ch );
            send_to_char( "'", ch );
        }

        if ( pOI->value[3] >= 0 && pOI->value[3] < MAX_SKILL )
        {
            send_to_char( " '", ch );
            send_to_char( skill_table[pOI->value[3]].name, ch );
            send_to_char( "'", ch );
        }

        send_to_char( ".\n\r", ch );
        break;

      case ITEM_WAND:
      case ITEM_STAFF:
        sprintf( buf, "Has %d(%d) charges of level %d",
            pOI->value[1], pOI->value[2], pOI->value[0] );
        send_to_char( buf, ch );

        if ( pOI->value[3] >= 0 && pOI->value[3] < MAX_SKILL )
        {
            send_to_char( " '", ch );
            send_to_char( skill_table[pOI->value[3]].name, ch );
            send_to_char( "'", ch );
        }

        send_to_char( ".\n\r", ch );
        break;

      case ITEM_WEAPON:
        sprintf( buf, "Damage is %dd%d (average %d).\n\r",
            pOI->value[1], pOI->value[2],
            ( pOI->value[1] + pOI->value[1]*pOI->value[2] ) / 2 );
        send_to_char( buf, ch );
        break;

      case ITEM_ARMOR:
      case ITEM_JEWELRY:
        sprintf( buf, "Armor class is %d.\n\r", pOI->value[0] );
        send_to_char( buf, ch );
        break;
    }

    for ( paf = pOI->affected; paf != NULL; paf = paf->next )
    {
        sprintf( buf, "Affects %s by %d.\n\r",
            affect_loc_name( paf->location ), paf->modifier );
        send_to_char( buf, ch );
    }

    if ( pOI->wear_fun != NULL )
        send_to_char( "Object has wear fun.\n\r", ch );

    if ( pOI->special != NULL )
        send_to_char( "Object has special function.\n\r", ch );
}


/* New wiz command. to clear out mobs in the mud .. Bruce was here */
/* modified by borlak */

void do_reset( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *victim_next;
  int mob_num;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  one_argument( argument, arg );

  if( arg[0] == '\0' ) 
  {
    send_to_char("Reset: mobs/mud/corpses\n\r", ch );
    return;
  }

  if( !str_cmp( arg, "mobs" ) ) 
  {
    mob_num = 0;
    for ( victim = char_list; victim != NULL; victim = victim_next )
    {
      victim_next = victim->next;

      if ( IS_NPC(victim) && (victim->pIndexData->pShop == NULL) )
      {
	mob_num += 1;
	extract_char( victim, TRUE );
      }
    }
    sprintf( buf, "*lick* %d mobs eaten.", mob_num);
    do_warmessage( NULL, buf );
    return;
  }

  if( !str_cmp( arg, "mud" ) )
  {
    reset_mud( );

    for( victim = char_list; victim != NULL; victim = victim_next )
    {
      victim_next = victim->next;

      if( IS_NPC(victim) ) 
	continue;

      get_start_eq( victim );

      if( victim->level > 30 )
	continue;

      stop_fighting( victim, TRUE );
      victim->hit = victim->max_hit;
      victim->pcdata->potential_dr += victim->max_hit;
      victim->mana = victim->max_mana;
      victim->armor = class_table[victim->char_class].base_armor;
    }
    do_warmessage( NULL, "Mud has been reset!" );
    return;
  }

  if( !str_cmp( arg, "corpses" ) )
  {
    for( obj = object_list; obj != NULL; obj = obj_next ) 
    {
      obj_next = obj->next;

      if( obj->item_type == ITEM_CORPSE_PC )
      {
	obj_from_room( obj, "do_reset" );
	extract_obj( obj, "do_reset" );
      }
    }
    send_to_char( "Poof, pc corpses gone.\n\r", ch );
    return;
  }
  return;
}


void do_mwhere( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Mwhere whom?\n\r", ch );
	return;
    }

    found = FALSE;
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
	if ( IS_NPC(victim)
	&&   victim->in_room != NULL
	&&   is_name( arg, victim->name ) )
	{
	    found = TRUE;
	    sprintf( buf, "[%5d] %-28s [%5d] %s\n\r",
		victim->pIndexData->vnum,
		victim->short_descr,
		victim->in_room->vnum,
		victim->in_room->name );
	    send_to_char( buf, ch );
	}
    }

    if ( !found )
	act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR, TRUE );

    return;
}



void do_reboo( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
    return;
}



void do_reboot( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
    extern bool reboot_on;

    if( !str_cmp( argument, "on" ) )
    {
	sprintf(buf, "Automatic reboot after war.");
	do_echo( ch, buf );
	reboot_on = TRUE;
	return;
    }

    if( !str_cmp(argument, "off" ) )
    {
	sprintf(buf,"Automatic reboot terminated.");
	do_echo( ch, buf );
	reboot_on = FALSE;
	return;
    }

    sprintf( buf, "Reboot by %s.", ch->name );
    append_file( ch, REBOOT_FILE, buf );
    if ( str_cmp( argument, "silent" ) )
      do_echo( ch, buf );
    merc_down = TRUE;
    return;
}



void do_shutdow( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
    return;
}



void do_shutdown( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    extern bool merc_down;
    extern int  shutdown_time;

    one_argument( argument, arg );

    if( is_number(arg) )
    {
	shutdown_time = atoi(arg);

	sprintf(buf, "Auto-shutdown in %d minute%s.",
		shutdown_time,
		shutdown_time == 1 ? "" : "s" );
	do_warmessage( NULL, buf );
	return;
    }

    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf );
    strcat( buf, "\n\r" );
    do_echo( ch, buf );
    merc_down = TRUE;
    return;
}

void remove_snoop( DESCRIPTOR_DATA *d, struct snoop_data *snoop )
{
    struct snoop_data *s;

    if ( d->snoop_by == snoop )
    {
	d->snoop_by = snoop->next;
	free_snoop( snoop );
	return;
    }

    for ( s = d->snoop_by; s != NULL; s = s->next )
    {
	if ( s->next != NULL && s->next == snoop )
	{
	    s->next = snoop->next;
	    free_snoop( snoop );
	    return;
	}
    }

    /* no snoop on that character.. */
    return;
}



void do_snoop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    struct snoop_data *snoop;
    struct snoop_data *snoop_next;
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( IS_NPC( ch ) )
	return;

    if ( arg[0] == '\0' )
    {
	send_to_char( "Snoop whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL )
    {
	send_to_char( "No descriptor to snoop.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Cancelling all snoops.\n\r", ch );
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    for ( snoop = d->snoop_by; snoop != NULL; snoop = snoop_next )
	    {
		snoop_next = snoop->next;

		if ( snoop->d == ch->desc )
		    remove_snoop( d, snoop );
	    }
	}
	return;
    }

    for ( snoop = victim->desc->snoop_by; snoop != NULL; snoop = snoop->next )
    {
	if ( snoop->d == ch->desc )
	{
	    send_to_char("You are already snooping them.\n\r", ch );
	    return;
	}
    }

    if ( get_trust( victim ) >= get_trust( ch )
    && get_trust( victim ) != 30 )
    {
	send_to_char("Maybe next level.\n\r", ch );
	return;
    }

    if ( get_trust( ch ) <= 30 )
    {
	extern time_t war_number;

	if ( mortal_in_main_war( ch ) )
	{
	    send_to_char("Mortals must be out of war to snoop.\n\r", ch );
	    return;
	}

	if ( !mortal_in_main_war( victim ) )
	{
	    send_to_char("Mortals may only snoop mortals that are in war.\n\r", ch );
	    return;
	}

	ch->pcdata->snoop_war = war_number;
    }

/*
 * we hope this is dealt with by our heuristic.
 * Can only snoop people lower level than you, except for 30th levels,
 * who can snoop other 30th levels.
 * 30th levels must be out of war, and their victim(s) must be in-war
 * to snoop.  I probably need to add some code to remove snoops when 
 * character becomes out of war.
 */
    snoop = new_snoop_data( );
    snoop->d = ch->desc;
    snoop->next = victim->desc->snoop_by;
    victim->desc->snoop_by = snoop;
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_switch( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into whom?\n\r", ch );
	return;
    }

    if ( ch->desc == NULL )
	return;
    
    if ( ch->desc->original != NULL )
    {
	send_to_char( "You are already switched.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( victim->desc != NULL )
    {
	send_to_char( "Character in use.\n\r", ch );
	return;
    }

    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    ch->desc            = NULL;
    send_to_char( "Ok.\n\r", victim );
    return;
}



void do_return( CHAR_DATA *ch, char *argument )
{
    if ( ch->desc == NULL )
	return;

    if ( ch->desc->original == NULL )
    {
	send_to_char( "You aren't switched.\n\r", ch );
	return;
    }

    send_to_char( "You return to your original body.\n\r", ch );
    ch->desc->character       = ch->desc->original;
    ch->desc->original        = NULL;
    ch->desc->character->desc = ch->desc; 
    ch->desc                  = NULL;
    return;
}



void do_mload( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;
    
    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number(arg) )
    {
	send_to_char( "Syntax: mload <vnum>.\n\r", ch );
	return;
    }

    if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
    {
	send_to_char( "No mob has that vnum.\n\r", ch );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    act( "$n has created $N!", ch, NULL, victim, TO_ROOM, TRUE );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_oload( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
 
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( in_static_war( ch ) )
    {
      send_to_char( "You may only use oload out of war.\n\r", ch );
      return;
    }

    if ( get_trust( ch ) < LEVEL_IMMORTAL
    && ch->private_war != NULL )
    {
      send_to_char(
        "You may not load equipment after you invite a private war.\n\r", ch );
      return;
    }

    if ( get_trust( ch ) < LEVEL_IMMORTAL
    && ch->carry_number >= 2 )
    {
      send_to_char( "You may only have 2 items in inventory at a time.\n\r", ch );
      return;
    }
 
    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        send_to_char( "Syntax: oload <vnum>.\n\r", ch );
        return;
    }
 
    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	send_to_char( "No object has that vnum.\n\r", ch );
	return;
    }

    if ( get_trust( ch ) < LEVEL_IMMORTAL
    && ( !CAN_WEAR( pObjIndex, ITEM_TAKE )
      || pObjIndex->item_type == ITEM_CONTAINER ) )
    {
      send_to_char( "You can't load that.\n\r", ch );
      return;
    }

    if ( get_trust( ch ) < LEVEL_IMMORTAL
    && ( pObjIndex->item_type == ITEM_POTION
      || pObjIndex->item_type == ITEM_DRINK_CON
      || pObjIndex->item_type == ITEM_PILL
      || pObjIndex->item_type == ITEM_SCROLL
      || pObjIndex->item_type == ITEM_WAND
      || pObjIndex->item_type == ITEM_STAFF ) )
    {
      send_to_char( "Restricted load item.  30 second lag.\n\r", ch );
      WAIT_STATE( ch, 120 );
    }

    obj = create_object( pObjIndex );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
    {
	obj_to_char( obj, ch );
    }
    else
    {
	obj_to_room( obj, ch->in_room );
	act( "$n has created $p!", ch, obj, NULL, TO_ROOM, TRUE );
    }
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_purge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	/* 'purge' */
	CHAR_DATA *vnext;
	OBJ_DATA  *obj_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC(victim) && victim != ch )
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    extract_obj( obj, "do_purge" );
	}

	act( "$n purges the room!", ch, NULL, NULL, TO_ROOM, TRUE );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on PC's.\n\r", ch );
	return;
    }

    act( "$n purges $N.", ch, NULL, victim, TO_NOTVICT, TRUE );
    extract_char( victim, TRUE );
    return;
}



void do_advance( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: advance <char> <level> <password>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ( level = atoi( arg2 ) ) < 1 || level > 40 )
    {
	send_to_char( "Level must be 1 to 40.\n\r", ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust level.\n\r", ch );
	return;
    }

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     */
    if ( level <= victim->level )
    {
	int sn;

	send_to_char( "Lowering a player's level!\n\r", ch );
	send_to_char( "**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", victim );
	victim->level    = 1;
	victim->exp      = 1000;
	victim->max_hit  = 10;
	victim->max_mana = 100;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	    victim->pcdata->learned[sn] = 0;
	victim->practice = 0;
	victim->hit      = victim->max_hit;
	victim->mana     = victim->max_mana;
	advance_level( victim );
    }
    else
    {
	send_to_char( "Raising a player's level!\n\r", ch );
	send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim );
    }

    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
	send_to_char( "You raise a level!!  ", victim );
	victim->level += 1;
	advance_level( victim );
    }
    victim->exp   = 1000 * UMAX( 1, victim->level );
    victim->trust = 0;
    return;
}



void do_trust( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if( ( level = atoi( arg2 ) ) < 0 || level > 41 )
    {
	send_to_char( "Level must be 0 (reset) or 1 to 40.\n\r", ch );
	return;
    }

    
    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust.\n\r", ch );
	return;
    }

    if( get_trust( victim) >= get_trust(ch) )
    {
	send_to_char("snicker\n\r",ch);
	return;
    }

    victim->trust = level;
    return;
}

void do_warstop( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *ch_next;
    extern int war_status;

    if ( war_status < WAR_ON )
    {
	send_to_char( "War hasn't started yet, no need to stop!\n\r", ch );
	return;
    }
    for ( rch = char_list; rch != NULL; rch = ch_next )
    {
	ch_next = rch->next;

	if( IS_NPC(rch) )
		continue;

        /* check level as well to get trusted characters */
	if( mortal_in_main_war( rch )
	&& rch->level == 30 )
	{
	    remove_from_war( rch );

	    if( rch->fighting != NULL )
		do_peace( rch, "" );

	    if ( rch->desc == NULL )
		do_quit( rch, "" );
	}
    }
    set_war_status( NO_WAR );
    do_echo( ch, "War has been stopped!" );
    return;
}

void restore_character( CHAR_DATA *ch )
{
  heal_character( ch, -1 );
  ch->mana = ch->max_mana;
  update_pos( ch );
}
   
void do_restore( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Restore whom?\n\r", ch );
	return;
    }

    /* adding restore all command ...      Bruce was here */
    if ( !str_cmp(arg, "all") )
    {
	for (d = descriptor_list; d != NULL; d = d->next)
	{
	    if ( d->connected == CON_PLAYING && !IS_NPC( d->character ) )
	    {
                restore_character( d->character );
		send_to_char("Someone has restored you.\n\r", d->character );
	    }
	}
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL ) 
    {
       send_to_char( "They aren't here.\n\r", ch );
       return;
    }

    restore_character( victim );
    act( "$n has restored you.", ch, NULL, victim, TO_VICT, TRUE );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_freeze( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Freeze whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_FREEZE) )
    {
	REMOVE_BIT(victim->act, PLR_FREEZE);
	send_to_char( "You can play again.\n\r", victim );
	send_to_char( "FREEZE removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_FREEZE);
	send_to_char( "You can't do ANYthing!\n\r", victim );
	send_to_char( "FREEZE set.\n\r", ch );
    }

    save_char_obj( victim );

    return;
}



void do_log( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Log whom?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char( "Log ALL off.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char( "Log ALL on.\n\r", ch );
	}
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET(victim->act, PLR_LOG) )
    {
	REMOVE_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG set.\n\r", ch );
    }

    return;
}



void do_noemote( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Noemote whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_NO_EMOTE) )
    {
	REMOVE_BIT(victim->act, PLR_NO_EMOTE);
	send_to_char( "You can emote again.\n\r", victim );
	send_to_char( "NO_EMOTE removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_NO_EMOTE);
	send_to_char( "You can't emote!\n\r", victim );
	send_to_char( "NO_EMOTE set.\n\r", ch );
    }

    return;
}



void do_notell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Notell whom?", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_NO_TELL) )
    {
	REMOVE_BIT(victim->act, PLR_NO_TELL);
	send_to_char( "You can tell again.\n\r", victim );
	send_to_char( "NO_TELL removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_NO_TELL);
	send_to_char( "You can't tell!\n\r", victim );
	send_to_char( "NO_TELL set.\n\r", ch );
    }

    return;
}



void do_silence( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
	DESCRIPTOR_DATA *d;
	DESCRIPTOR_DATA *d_next;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Silence whom?", ch );
	return;
    }

	if( str_cmp( arg, "all" ) )
	{
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
	

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_SILENCE) )
    {
	REMOVE_BIT(victim->act, PLR_SILENCE);
	send_to_char( "You can use channels again.\n\r", victim );
	send_to_char( "SILENCE removed.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_SILENCE);
	send_to_char( "You can't use channels!\n\r", victim );
	send_to_char( "SILENCE set.\n\r", ch );
    }
	}
	else
	{
		for( d = descriptor_list; d != NULL; d = d_next )
		{
			d_next = d->next;

			if( d->character == NULL )
				continue;

			if( get_trust(d->character) >= get_trust(ch) )
				continue;

			if( IS_SET(d->character->act, PLR_SILENCE ) )
			{
				sprintf(buf,"%s UNSILENCED.\n\r",d->character->name);
				send_to_char(buf, ch );
				REMOVE_BIT(d->character->act, PLR_SILENCE);
				send_to_char("You can speak now.\n\r",d->character);
			}
			else
			{
				sprintf(buf,"%s SILENCED.\n\r",d->character->name );
				send_to_char(buf, ch );
				SET_BIT(d->character->act, PLR_SILENCE );
				send_to_char("SHUT UP!\n\r",d->character);
			}
		}
		send_to_char("ok.", ch );
	}

    return;
}



void do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch->fighting != NULL )
	    stop_fighting( rch, TRUE );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_ban( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    BAN_DATA *pban;

    if ( IS_NPC(ch) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	strcpy( buf, "Banned sites:\n\r" );
	for ( pban = ban_list; pban != NULL; pban = pban->next )
	{
	    strcat( buf, pban->name );
	    strcat( buf, "\n\r" );
	}
	send_to_char( buf, ch );

	strcpy( buf, "No new char sites:\n\r" );
	for ( pban = restrict_list; pban != NULL; pban = pban->next )
	{
	    strcat( buf, pban->name );
	    strcat( buf, "\n\r" );
	}
	send_to_char( buf, ch );

	strcpy( buf, "No note sites:\n\r" );
	for ( pban = no_note_list; pban != NULL; pban = pban->next )
	{
	    strcat( buf, pban->name );
	    strcat( buf, "\n\r" );
	}
	send_to_char( buf, ch );
	return;
    }

    for ( pban = ban_list; pban != NULL; pban = pban->next )
    {
	if ( !str_cmp( arg, pban->name ) )
	{
	    send_to_char( "That site is already banned!\n\r", ch );
	    return;
	}
    }

    if ( ban_free == NULL )
    {
	pban		= (BAN_DATA *) alloc_perm( sizeof(*pban) );
    }
    else
    {
	pban		= ban_free;
	ban_free	= ban_free->next;
    }

    pban->name	= str_dup( arg );
    pban->next	= ban_list;
    ban_list	= pban;
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_allow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    BAN_DATA *prev;
    BAN_DATA *curr;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove which site from the ban list?\n\r", ch );
	return;
    }

    prev = NULL;
    for ( curr = ban_list; curr != NULL; prev = curr, curr = curr->next )
    {
	if ( !str_cmp( arg, curr->name ) )
	{
	    if ( prev == NULL )
		ban_list   = ban_list->next;
	    else
		prev->next = curr->next;

	    free_string( curr->name );
	    curr->next	= ban_free;
	    ban_free	= curr;
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }

    send_to_char( "Site is not banned.\n\r", ch );
    return;
}

void do_notify( CHAR_DATA *ch, char *argument )
{
	if( !IS_SET(ch->act, PLR_NOTIFY ) )
	{
		SET_BIT(ch->act, PLR_NOTIFY);
		send_to_char("Wiz notifies toggled.\n\r", ch );
		return;
	}
	else
	{
		REMOVE_BIT(ch->act, PLR_NOTIFY);
		send_to_char("You are now ignoring wiz notifies.\n\r", ch );
		return;
	}
	return;
}

void do_wizlock( CHAR_DATA *ch, char *argument )
{
    extern bool wizlock;
    wizlock = !wizlock;

    if ( wizlock )
	send_to_char( "Game wizlocked.\n\r", ch );
    else
	send_to_char( "Game un-wizlocked.\n\r", ch );

    return;
}



void do_slookup( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int sn;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slookup what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    sprintf( buf, "Sn: %4d Slot: %4d Skill/spell: '%s'\n\r",
		sn, skill_table[sn].slot, skill_table[sn].name );
	    send_to_char( buf, ch );
	}
    }
    else
    {
	if ( ( sn = skill_lookup( arg ) ) < 0 )
	{
	    send_to_char( "No such skill or spell.\n\r", ch );
	    return;
	}

	sprintf( buf, "Sn: %4d Slot: %4d Skill/spell: '%s'\n\r",
	    sn, skill_table[sn].slot, skill_table[sn].name );
	send_to_char( buf, ch );
    }

    return;
}



void do_sset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    int sn;
    bool fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: sset <victim> <skill> <value>\n\r",	ch );
	send_to_char( "or:     sset <victim> all     <value>\n\r",	ch );
	send_to_char( "Skill being any skill or spell.\n\r",		ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill or spell.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
	send_to_char( "Value range is 0 to 100.\n\r", ch );
	return;
    }

    if ( fAll )
    {
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name != NULL )
		victim->pcdata->learned[sn]	= value;
	}
    }
    else
    {
	victim->pcdata->learned[sn] = value;
    }

    return;
}



void do_mset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: mset <victim> <field>  <value>\n\r",	ch );
	send_to_char( "or:     mset <victim> <string> <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  str int wis dex con sex class level\n\r",	ch );
	send_to_char( "  gold hp mana move practice\n\r",		ch );
	send_to_char( "  wins kills losses nojoin inwar hunting",	ch );
	send_to_char( "  mlag",		ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "String being one of:\n\r",			ch );
	send_to_char( "  name short long description title spec\n\r",	ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    /*
     * Set something.
     */

    if( !str_cmp( arg2, "hunting" ) )
    {
	CHAR_DATA *tohunt;
	char buf[MAX_STRING_LENGTH];

	if( !IS_NPC(victim ) )
	{
		send_to_char("Not on PC's.\n\r", ch );
		return;
	}

	if( (tohunt = get_char_world(ch, arg3)) == NULL )
	{
		send_to_char("They aren't on to hunt.\n\r", ch );
		return;
	}

	if( IS_NPC(tohunt) )
	{
		send_to_char("Mobs shouldnt hunt mobs.\n\r", ch );
		return;
	}

	victim->hunting = tohunt;
	sprintf(buf, "%s is hunting you.",victim->short_descr);
	send_to_char( buf, tohunt );
	sprintf(buf, "%s hunting %s.",victim->short_descr,tohunt->name );
	mudlog(buf, tohunt->level );
	send_to_char("Ok.\n\r", ch );
	return;
    }

    if( !str_cmp( arg2, "nojoin" ) )
    {
	if( IS_NPC(victim) )
	{
		send_to_char("Duh, not on NPc's.\n\r", ch );
		return;
	}
	victim->nojoin = value;

	if( victim->nojoin == 0 )
		send_to_char( "You may now join.\n\r", victim );

	return;
    }

    if ( !str_cmp( arg2, "str" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 3 || value > 25 )
	{
	    send_to_char( "Strength range is 3 to 18.\n\r", ch );
	    return;
	}

	victim->pcdata->perm_str = value;
	return;
    }

    if ( !str_cmp( arg2, "int" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 3 || value > 25 )
	{
	    send_to_char( "Intelligence range is 3 to 18.\n\r", ch );
	    return;
	}

	victim->pcdata->perm_int = value;
	return;
    }

    if ( !str_cmp( arg2, "wis" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 3 || value > 25 )
	{
	    send_to_char( "Wisdom range is 3 to 18.\n\r", ch );
	    return;
	}

	victim->pcdata->perm_wis = value;
	return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 3 || value > 25 )
	{
	    send_to_char( "Dexterity range is 3 to 18.\n\r", ch );
	    return;
	}

	victim->pcdata->perm_dex = value;
	return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 3 || value > 25 )
	{
	    send_to_char( "Constitution range is 3 to 18.\n\r", ch );
	    return;
	}

	victim->pcdata->perm_con = value;
	return;
    }

    if ( !str_cmp( arg2, "mlag" ) )
    {
	if ( value < 0 || value > 50 )
	{
	    send_to_char( "Mlag range is 0 to 50.\n\r", ch );
	    return;
	}

	victim->move_lag = value;
	return;
    }

    if ( !str_cmp( arg2, "sex" ) )
    {
	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Sex range is 0 to 2.\n\r", ch );
	    return;
	}
	victim->sex = value;
	return;
    }

    if ( !str_cmp( arg2, "class" ) )
    {
	if ( value < 0 || value >= MAX_CLASS )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "Class range is 0 to %d.\n", MAX_CLASS-1 );
	    send_to_char( buf, ch );
	    return;
	}
	victim->char_class = value;
	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 50 )
	{
	    send_to_char( "Level range is 0 to 50.\n\r", ch );
	    return;
	}
	victim->level = value;
	return;
    }

    if ( !str_cmp( arg2, "gold" ) )
    {
	victim->gold = value;
	return;
    }

    if ( !str_cmp( arg2, "hp" ) )
    {
	if ( value < -10 || value > 30000 )
	{
	    send_to_char( "Hp range is -10 to 30,000 hit points.\n\r", ch );
	    return;
	}
	victim->max_hit = value;
	return;
    }

    if ( !str_cmp( arg2, "mana" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Mana range is 0 to 30,000 mana points.\n\r", ch );
	    return;
	}
	victim->max_mana = value;
	return;
    }

    if ( !str_cmp( arg2, "practice" ) )
    {
	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Practice range is 0 to 100 sessions.\n\r", ch );
	    return;
	}
	victim->practice = value;
	return;
    }

    if ( !str_cmp( arg2, "wins" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 1000 )
	{
	    send_to_char( "Win range is 0 to 1000.\n\r", ch );
	    return;
	}

	victim->pcdata->wins = value;
	return;
    }

    if ( !str_cmp( arg2, "kills" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 1000 )
	{
	    send_to_char( "kills range is 0 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->kills = value;
	return;
    }

    if ( !str_cmp( arg2, "deaths" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 1000 )
	{
	    send_to_char( "deaths range is 0 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->deaths = value;
	return;
    }

    if ( !str_cmp( arg2, "inwar" ) )
    {
	bool orig_state;

        if ( IS_NPC(victim) )
        {
            send_to_char( "Now on NPC's.\n\r", ch );
            return;
        }

	orig_state = victim->pcdata->war_num;

        if ( value < 0 || value > 3 )
        {
            send_to_char( "inwar can be set to 0, 1, 2, or 3.\n\r", ch );
            return;
        }

        victim->pcdata->war_num = value;
	if ( value == 0 )
	    remove_from_war( victim );

	if ( orig_state != value )
	{
	    if ( value == FALSE )
		victim->pcdata->potential_dr -= victim->hit;
	    else
		victim->pcdata->potential_dr += victim->hit;
	}
        return;
    }


    if ( !str_cmp( arg2, "name" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	free_string( victim->name );
	victim->name = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
	free_string( victim->short_descr );
	victim->short_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
	free_string( victim->long_descr );
	strcat( arg3, "\n\r" );
	victim->long_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "title" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	set_title( victim, arg3 );
	return;
    }

    if ( !str_cmp( arg2, "spec" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
	{
	    send_to_char( "No such spec fun.\n\r", ch );
	    return;
	}

	return;
    }

    /*
     * Generate usage message.
     */
    do_mset( ch, "" );
    return;
}



void do_oset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: oset <object> <field>  <value>\n\r",	ch );
	send_to_char( "or:     oset <object> <string> <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  value0 value1 value2 value3\n\r",		ch );
	send_to_char( "  extra wear weight cost timer\n\r",		ch );
	send_to_char( "  spec\n\r\n\r",					ch );
	send_to_char( "String being one of:\n\r",			ch );
	send_to_char( "  name short long ed\n\r",			ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {
	obj->value[0] = value;
	return;
    }

    if( !str_cmp( arg2, "spec" ) )
    {
	if( ( obj->special = object_fun_lookup( arg3 ) ) == 0 )
	{
		send_to_char("Thats not a spec.\n\r",ch);
		return;
	}
	return;
    }

    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {
	obj->value[1] = value;
	return;
    }

    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {
	obj->value[2] = value;
	return;
    }

    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {
	obj->value[3] = value;
	return;
    }

    if ( !str_cmp( arg2, "extra" ) )
    {
	obj->extra_flags = value;
	return;
    }

    if ( !str_cmp( arg2, "wear" ) )
    {
	obj->wear_flags = value;
	return;
    }

    if ( !str_cmp( arg2, "weight" ) )
    {
	obj->weight = value;
	return;
    }

    if ( !str_cmp( arg2, "cost" ) )
    {
	obj->cost = value;
	return;
    }

    if ( !str_cmp( arg2, "timer" ) )
    {
	obj->timer = value;
	return;
    }
	
    if ( !str_cmp( arg2, "name" ) )
    {
	free_string( obj->name );
	obj->name = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
	free_string( obj->short_descr );
	obj->short_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
	free_string( obj->description );
	obj->description = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "ed" ) )
    {
	EXTRA_DESCR_DATA *ed;

	argument = one_argument( argument, arg3 );
	if ( argument == NULL )
	{
	    send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r",
		ch );
	    return;
	}

	if ( extra_descr_free == NULL )
	{
	    ed			= (EXTRA_DESCR_DATA *) alloc_perm( sizeof(*ed) );
	}
	else
	{
	    ed			= extra_descr_free;
	    extra_descr_free	= ed;
	}

	ed->keyword		= str_dup( arg3     );
	ed->description		= str_dup( argument );
	ed->next		= obj->extra_descr;
	obj->extra_descr	= ed;
	return;
    }

    /*
     * Generate usage message.
     */
    do_oset( ch, "" );
    return;
}



void do_rset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: rset <location> <field> value\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  flags sector\n\r",				ch );
	return;
    }

    if ( ( location = find_location( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "flags" ) )
    {
	location->room_flags	= value;
	return;
    }

    if ( !str_cmp( arg2, "sector" ) )
    {
	location->sector_type	= value;
	return;
    }

    /*
     * Generate usage message.
     */
    do_rset( ch, "" );
    return;
}


bool is_numeric_address( char *addr )
{
    int len = strlen( addr );
    int i;

    for ( i = 0; i < len; i++ )
	if ( ( addr[i] < '0'
	|| addr[i] > '9' )
	  && addr[i] != '.' )
	    return FALSE;

    return TRUE;
}


char *get_host( CHAR_DATA *ch, char *host_orig )
{
  static char host_out[80];
  int len = strlen( host_orig );
  int i;

  if ( IS_IMMORTAL( ch ) )
    return host_orig;

  len = UMIN( 79, len );
  strncpy( host_out, host_orig, len );
  host_out[len] = '\0';

  if ( is_numeric_address( host_orig ) )
  {
    for ( i = len; host_out[i] != '.'; i-- )
      ;

    for ( i++; host_out[i] != '\0'; i++ )
      host_out[i] = 'x';
  }
  else
  {
    for ( i = 0; host_out[i] != '.'; i++ )
      host_out[i] = 'x';
  }

  return host_out;
}



void do_users( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int count;

    count	= 0;
    buf[0]	= '\0';
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->character != NULL && can_see( ch, d->character ) )
	{
	    if ( get_trust( ch ) < 39 && d->connected != CON_PLAYING )
		continue;
	    count++;
	    if ( IS_IMMORTAL( ch ) )
	      sprintf( buf + strlen(buf), "[%3d %2d] %s@%s\n\r",
		d->descriptor, d->connected,
		d->original  ? d->original->name  :
		d->character ? d->character->name : "(none)",
		get_host( ch, d->host )
		);
	    else
	      sprintf( buf + strlen(buf), "%s@%s\n\r",
		d->original  ? d->original->name  :
		d->character ? d->character->name : "(none)",
		get_host( ch, d->host )
		);
	}
    }

    sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
    send_to_char( buf2, ch );
    send_to_char( buf, ch );
    return;
}



/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Force whom to do what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch ) )
	    {
		act( "$n forces you to '$t'.", ch, argument, vch, TO_VICT, TRUE );
		interpret( vch, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}

	act( "$n forces you to '$t'.", ch, argument, victim, TO_VICT, TRUE );
	interpret( victim, argument );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}



/*
 * New routines by Dionysos.
 */
void do_invis( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( is_number(argument) )
	ch->pcdata->wizinvis_level = UMIN( ch->level, ( atoi( argument ) ) );

    sprintf( buf, "Wizinvis set to %d.\n\r", ch->pcdata->wizinvis_level );
	send_to_char( buf, ch );

    return;
}



void do_holylight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode on.\n\r", ch );
    }

    return;
}




void do_osearch( CHAR_DATA *ch, char *argument )
{
  OBJ_INDEX_DATA *pObjIndex;
  AFFECT_DATA *paf;
  int otype = 0;
  int owear = 0;
  int oaffect = 0;
  int vnum = 0;
  int nMatch = 0;
  int i;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg );
  while ( arg[0] != '\0' )
  {
    if ( !str_prefix( arg, "type" ) )
    {
      argument = one_argument( argument, arg );
      i = -1;
      while( 1 )
      {
        i++;
        if ( !str_prefix( arg, item_type_table[i].name ) )
	  break;
	if ( str_cmp( "last", item_type_table[i].name ) )
	  continue;
	send_to_char( "Bad item type.\n\r", ch );
	return;
      }
      otype = item_type_table[i].ident;
    }
    else if ( !str_prefix( arg, "wear" ) )
    {
      argument = one_argument( argument, arg );
      i = -1;
      while( 1 )
      {
        i++;
        if ( !str_prefix( arg, wear_loc_table[i].loc_name ) )
	  break;
	if ( str_cmp( "last", wear_loc_table[i].loc_name ) )
	  continue;
	send_to_char( "Bad wear loc.\n\r", ch );
	return;
      }
      owear = wear_loc_table[i].loc_bit;
    }
    else if ( !str_prefix( arg, "affect" ) )
    {
      argument = one_argument( argument, arg );
      i = -1;
      while ( 1 )
      {
        i++;
        if ( !str_prefix( arg, affect_loc_table[i].loc_name ) )
	  break;
	if ( str_cmp( "last", affect_loc_table[i].loc_name ) )
	  continue;
	send_to_char( "Bad affect.\n\r", ch );
	return;
      }
      oaffect = affect_loc_table[i].loc_bit;
    }
    else if ( !str_prefix( arg, "vnum" ) )
    {
      argument = one_argument( argument, arg );
      if ( !is_number( arg ) )
      {
        send_to_char( "Vnum argument must be a number.\n\r", ch );
	return;
      }
      vnum = atoi( arg );
    }
    argument = one_argument( argument, arg );
  }

  for ( ; vnum <= MAX_VNUM; vnum++ )
  {
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
      continue;

    if ( owear != 0 && !IS_SET( pObjIndex->wear_flags, owear ) )
      continue;

    if ( otype != 0 && otype != pObjIndex->item_type )
      continue;

    if ( oaffect != 0 )
    {
      for ( paf = pObjIndex->affected; paf != NULL; paf = paf->next )
      {
        if ( paf->location == oaffect )
	  break;
      }

      if ( paf == NULL )
        continue;
    }

    sprintf( buf, "[%5d] %s\n\r",
      pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
    send_to_char( buf, ch );
    nMatch++;

    if ( nMatch >= 20 )
      break;
  }

  return;
}



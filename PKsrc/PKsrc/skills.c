/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD

  With an attempt by Rip to convert to merc 2.1
*/

#include <stdio.h>
#include <time.h>

#include "merc.h"
#include "bfs.h"


void do_whichway( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int direction;

  one_argument( argument, arg );

  if ( mortal_in_main_war( ch ) )
  {
   send_to_char("You can only use whichway out of war.\n\r", ch );
   return;
  }

  if (arg[0] == '\0')
  {
   send_to_char("Which way to WHAT?\n\r", ch );
   return;
  }

  if ( (victim = get_char_phase( ch, arg ) ) == NULL )
  {
    send_to_char("No such victim!\n\r", ch );
    return;
  }

  direction = bfs_find_room( ch, ch->in_room, victim->in_room, 1000 ); 

  switch (direction)
  {
    case DIR_NORTH:
      send_to_char("Your quarry is to the north.\n\r", ch );
      break;
    case DIR_SOUTH:
      send_to_char("Your quarry is to the south.\n\r", ch );
      break;
    case DIR_EAST:
      send_to_char("Your quarry is to the east.\n\r", ch );
      break;
    case DIR_WEST:
      send_to_char("Your quarry is to the west.\n\r", ch );
      break;
    case DIR_UP:
      send_to_char("Your quarry is up.\n\r", ch );
      break;
    case DIR_DOWN:
      send_to_char("Your quarry is down.\n\r", ch );
      break;
    default:
      send_to_char("Unable to find path to your victim.\n\r", ch );
      break;
  }

  return;
}


void do_sic_mob( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
	CHAR_DATA *mob;
	OBJ_DATA *obj;
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];

	argument = one_argument( argument, arg1 );
	one_argument( argument, arg2 );

    if (arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Sicem <assassin/dog/droid/terminator/thief/scout/archer/zombie> who?\n\r", ch );
	return;
    }

	if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
	{
		send_to_char("They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC(victim) )
	{
		send_to_char("Not a player.\n\r", ch );
		return;
	}

	if( !str_prefix(arg1, "terminator") )
	{
		mob = create_mobile( get_mob_index( 10 ), victim->phase );

		mob->hunting = victim;
		mob->max_hit = 600;
		mob->hit = mob->max_hit;
		mob->level = 30;
		mob->gold = 1000;
		mob->char_class = CLASS_WARRIOR;
	
		obj = create_object( get_obj_index(50), mob->phase );
		obj_to_char( obj, mob );
		equip_char( mob, obj, WEAR_WIELD );
	
		obj = create_object( get_obj_index(3620), mob->phase );
		obj_to_char( obj, mob );
		equip_char( mob, obj, WEAR_SHIELD );
	
		char_to_room( mob, get_room_index(ROOM_VNUM_TEMPLE) );
	
		{
			char buf[MAX_STRING_LENGTH];
	
			sprintf(buf,"%s shall hunt %s down!",mob->short_descr,victim->name);
			do_warmessage(ch,buf);
		}	
		return;
	}

    if ( !str_prefix( arg1, "droid" ) )
    {
	mob = create_mobile( get_mob_index( 11 ), victim->phase );

	mob->hunting = victim;
	mob->max_hit = 300;
	mob->hit = mob->max_hit;
	mob->level = 20;
	mob->gold = 100;
	mob->char_class = CLASS_WARRIOR;
	
	obj = create_object( get_obj_index(50), mob->phase );
	/* make it 4d8 for the baby, not 8d8 */
	obj->value[1] = 4;
	obj_to_char( obj, mob );
	equip_char( mob, obj, WEAR_WIELD );
	
	obj = create_object( get_obj_index(3620), mob->phase );
	obj_to_char( obj, mob );
	equip_char( mob, obj, WEAR_SHIELD );
	
	char_to_room( mob, get_room_index(ROOM_VNUM_TEMPLE) );
	{
	    char buf[MAX_STRING_LENGTH];
	
	    sprintf(buf,"%s shall hunt %s down!",mob->short_descr,victim->name);
	    do_warmessage(ch,buf);
	}	
    }

    if ( !str_prefix( arg1, "thief" ) )
    {
	mob = create_mobile( get_mob_index( 13 ), victim->phase );

	mob->hunting = victim;
	mob->max_hit = 280;
	mob->hit = mob->max_hit;
	mob->level = 30;
	mob->gold = 100;
	mob->char_class = CLASS_THIEF;
	
	obj = create_object( get_obj_index(4053), mob->phase );  /* thieve's dirk */
	obj_to_char( obj, mob );
	equip_char( mob, obj, WEAR_WIELD );
	
	char_to_room( mob, get_room_index(ROOM_VNUM_TEMPLE) );
	{
	    char buf[MAX_STRING_LENGTH];
	
	    sprintf(buf,"%s shall hunt %s down!",mob->short_descr,victim->name);
	    do_warmessage(ch,buf);
	}	
    }

    if ( !str_prefix( arg1, "assassin" ) )
    {
	mob = create_mobile( get_mob_index( 14 ), victim->phase );

	mob->hunting = victim;
	mob->max_hit = 150;
	mob->hit = mob->max_hit;
	mob->level = 30;
	mob->gold = 100;
	mob->char_class = CLASS_ASSASSIN;
	
	obj = create_object( get_obj_index(9008), mob->phase );  /* thieve's dirk */
	obj_to_char( obj, mob );
	equip_char( mob, obj, WEAR_WIELD );
	
	char_to_room( mob, get_room_index(ROOM_VNUM_TEMPLE) );
	{
	    char buf[MAX_STRING_LENGTH];
	
	    sprintf(buf,"%s shall hunt %s down!",mob->short_descr,victim->name);
	    do_warmessage(ch,buf);
	}	
    }

    if ( !str_prefix( arg1, "archer" ) )
    {
	mob = create_mobile( get_mob_index( 17 ), victim->phase );

	mob->hunting = victim;
	mob->max_hit = 200;
	mob->hit = mob->max_hit;
	mob->level = 30;
	mob->gold = 100;
	
	char_to_room( mob, get_room_index(ROOM_VNUM_TEMPLE) );
	{
	    char buf[MAX_STRING_LENGTH];
	
	    sprintf(buf,"%s shall hunt %s down!",mob->short_descr,victim->name);
	    do_warmessage(ch,buf);
	}	
    }

    if ( !str_prefix( arg1, "dog" ) )
    {
	mob = create_mobile( get_mob_index( 12 ), victim->phase );

	mob->hunting = victim;
	mob->max_hit = 300;
	mob->hit = mob->max_hit;
	mob->level = 20;
	mob->gold = 100;
	
	char_to_room( mob, get_room_index(ROOM_VNUM_TEMPLE) );
	{
	    char buf[MAX_STRING_LENGTH];
	
	    sprintf(buf,"%s shall hunt %s down!",mob->short_descr,victim->name);
	    do_warmessage(ch,buf);
	}	
    }

    if ( !str_prefix( arg1, "scout" ) )
    {
	mob = create_mobile( get_mob_index( 15 ), victim->phase );

	mob->hunting = victim;
	mob->max_hit = 300;
	mob->hit = mob->max_hit;
	mob->max_mana = 60;
	mob->mana = mob->max_mana;
	mob->level = 20;
	mob->gold = 1;
	mob->char_class = CLASS_THIEF;
	
	char_to_room( mob, get_room_index(ROOM_VNUM_TEMPLE) );
	{
	    char buf[MAX_STRING_LENGTH];
	
	    sprintf(buf,"%s shall spy on %s!",mob->short_descr,victim->name);
	    do_warmessage(ch,buf);
	}
    }

	if( !str_prefix(arg1,"hunter") )
	{
		mob = create_mobile(get_mob_index(8), victim->phase);

		mob->max_hit	= 50;
		mob->hit	= 50;
		mob->level	= 15;

		obj = create_object(get_obj_index(1), mob->phase);
		obj_to_char(obj,mob);
		equip_char(mob,obj,WEAR_WIELD);

		obj = create_object(get_obj_index(4), mob->phase);
		make_random_quiver(obj);
		obj_to_char(obj,mob);
		equip_char(mob,obj,WEAR_WAIST);

		char_to_room(mob,victim->in_room);		

		return;
	}

  if( !str_prefix(arg1,"zombie") )
  {
    assign_zombie( victim );
    return;
  }
  return;
}

void assign_zombie( CHAR_DATA *victim )
{
  CHAR_DATA *mob;

  mob = create_mobile(get_mob_index(18), victim->phase);

  mob->hunting = victim;
  mob->max_hit = 30;
  mob->hit     = 30;
  mob->level   =  5;

  char_to_room( mob, get_room_index(ROOM_VNUM_TEMPLE) );
  {
    char buf[MAX_STRING_LENGTH];
    sprintf(buf,"%s sniffs the air for the brains of %s!",mob->short_descr,victim->name);
    do_warmessage(victim,buf);
  }

  return;
}

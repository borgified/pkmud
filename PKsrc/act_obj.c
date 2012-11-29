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



/*
 * Local functions.
 */
#define CD CHAR_DATA
void	get_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj,
			    OBJ_DATA *container ) );
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void	wear_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
void wield_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
CD *	find_keeper	args( ( CHAR_DATA *ch ) );
int	get_cost	args( ( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );
#undef	CD




void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{

    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	send_to_char( "You can't take that.\n\r", ch );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	act( "$d: you can't carry that many items.",
	    ch, NULL, obj->name, TO_CHAR, TRUE );
	return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
	act( "$d: you can't carry that much weight.",
	    ch, NULL, obj->name, TO_CHAR, TRUE );
	return;
    }

    if ( container != NULL )
    {
	act( "You get $p from $P.", ch, obj, container, TO_CHAR, TRUE );
	act( "$n gets $p from $P.", ch, obj, container, TO_ROOM, FALSE );
	obj_from_obj( obj );
    }
    else
    {
	act( "You get $p.", ch, obj, container, TO_CHAR, TRUE );
	act( "$n gets $p.", ch, obj, container, TO_ROOM, TRUE );
	obj_from_room( obj, "do_get" );
    }

    if ( obj->item_type == ITEM_MONEY )
    {
	ch->gold += obj->value[0];
	extract_obj( obj, "do_get" );
    }
    else
    {
	obj_to_char( obj, ch );
    }

    return;
}

void do_disarmtrap( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	bool bFound = FALSE;
	OBJ_DATA *obj;
	OBJECT_FUN spec_trap;


	if( IS_NPC(ch) )
		return;

	if( ch->pcdata->learned[gsn_disarm_traps] <= 1 )
	{
		send_to_char("You don't know how to disarm traps.\n\r", ch );
		return;
	}

	for( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
	{
		if( obj->special != spec_trap )
		{
			continue;
		}
		else
		{
			bFound = TRUE;
			break;
		}
	}

	if( !bFound )
	{
		send_to_char("There are no traps to disarm here!\n\r", ch );
		return;
	}

	if( number_percent() > ch->pcdata->learned[gsn_disarm_traps] )
	{
		sprintf(buf,"You fail at disarming %s, and it backfires!",obj->short_descr);
		send_to_char(buf,ch);
		(*obj->special) (obj, ch, "called_from_move_char", str_null );
		return;
	}

	sprintf(buf,"You successfully disarm %s.\n\r",obj->short_descr);
	send_to_char(buf,ch);

	obj->value[1]	= FALSE;
	obj->wear_flags = 1;
	obj->killer	= NULL;
	free_string(obj->description);
	sprintf(buf,"%s is lying here, recently disarmed.",obj->short_descr);
	obj->description = str_dup(buf);
	return;
}		

void do_set( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  bool bFound = FALSE;
  OBJ_DATA *obj;
  OBJECT_FUN spec_trap;

  if( IS_NPC(ch) )
    return;

  if( ch->pcdata->learned[gsn_set_traps] <= 1 )
  {
    send_to_char("You don't know how to set traps.\n\r", ch );
    return;
  }

  if( !in_main_war( ch ) )
  {
    send_to_char("Might want to be in a war first.\n\r", ch );
    return;
  }

	for( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{
		if( obj->special != spec_trap )
		{
			continue;
		}
		else
		{
			bFound = TRUE;
			break;
		}
	}

	if( !bFound )
	{
		send_to_char("You don't have any traps to set.\n\r", ch );
		return;
	}

	if( number_percent() > ch->pcdata->learned[gsn_set_traps] )
	{
		sprintf(buf,"You fail at setting %s and it backfires!\n\r",obj->short_descr);
		send_to_char(buf,ch);
		(*obj->special) (obj, ch, "called_from_move_char", str_null );
		return;
	}

	obj_from_char(obj);
	obj_to_room(obj,ch->in_room);

	obj->value[1]	= TRUE;
	obj->wear_flags = 0;
	obj->killer	= ch;

	free_string(obj->description);
	obj->description = str_dup("");		
	
	sprintf(buf,"You hide %s and set it.\n\r",obj->short_descr);
	send_to_char(buf,ch);

	return;
}

void do_get( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    bool found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

/* make sure they are alive */
  if (!IS_NPC(ch))
    if ( !in_main_war( ch ) )
    {
	send_to_char( "You aren't in the war!!\n\r", ch );
	return;
    }

    /* Get type. */
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Get what?\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj' */
	    obj = get_obj_list( ch, arg1, ch->in_room->contents );
	    if ( obj == NULL )
	    {
		act( "I see no $T here.", ch, NULL, arg1, TO_CHAR, TRUE );
		return;
	    }

	    get_obj( ch, obj, NULL );
	}
	else
	{
	    /* 'get all' or 'get all.obj' */
	    found = FALSE;
	    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{

		    found = TRUE;
		    get_obj( ch, obj, NULL );
		}
	    }

	    if ( !found ) 
	    {
		if ( arg1[3] == '\0' )
		    send_to_char( "I see nothing here.\n\r", ch );
		else
		    act( "I see no $T here.", ch, NULL, &arg1[4], TO_CHAR, TRUE );
	    }
	}
    }
    else
    {
	/* 'get ... container' */
	if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}

	if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    act( "I see no $T here.", ch, NULL, arg2, TO_CHAR, TRUE );
	    return;
	}

	switch ( container->item_type )
	{
	default:
	    send_to_char( "That's not a container.\n\r", ch );
	    return;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	    break;

	case ITEM_CORPSE_PC:
            break;
	}

	if ( IS_SET(container->value[1], CONT_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, container->name, TO_CHAR, TRUE );
	    return;
	}

	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj container' */
	    obj = get_obj_list( ch, arg1, container->contains );
	    if ( obj == NULL )
	    {
		act( "I see nothing like that in the $T.",
		    ch, NULL, arg2, TO_CHAR, TRUE );
		return;
	    }
	    get_obj( ch, obj, container );
	}
	else
	{
	    /* 'get all container' or 'get all.obj container' */
	    found = FALSE;
	    for ( obj = container->contains; obj != NULL; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    get_obj( ch, obj, container );
		}
	    }

	    if ( !found )
	    {
		if ( arg1[3] == '\0' )
		    act( "I see nothing in the $T.",
			ch, NULL, arg2, TO_CHAR, TRUE );
		else
		    act( "I see nothing like that in the $T.",
			ch, NULL, arg2, TO_CHAR, TRUE );
	    }
	}
    }

    return;
}



void do_put( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

  if ( !in_main_war( ch ) )
  {
    send_to_char( "You aren't in the war.\n\r", ch );
    return;
  }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Put what in what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
    {
	act( "I see no $T here.", ch, NULL, arg2, TO_CHAR, TRUE );
	return;
    }

    if ( container->item_type != ITEM_CONTAINER )
    {
	send_to_char( "That's not a container.\n\r", ch );
	return;
    }

    if ( IS_SET(container->value[1], CONT_CLOSED) )
    {
	act( "The $d is closed.", ch, NULL, container->name, TO_CHAR, TRUE );
	return;
    }

    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
	/* 'put obj container' */
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( obj == container )
	{
	    send_to_char( "You can't fold it into itself.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	if ( get_obj_weight( obj ) + get_obj_weight( container )
	     > container->value[0] )
	{
	    send_to_char( "It won't fit.\n\r", ch );
	    return;
	}

	obj_from_char( obj );
	obj_to_obj( obj, container );
	act( "$n puts $p in $P.", ch, obj, container, TO_ROOM, TRUE );
	act( "You put $p in $P.", ch, obj, container, TO_CHAR, FALSE );
    }
    else
    {
	/* 'put all container' or 'put all.obj container' */
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   obj->wear_loc == WEAR_NONE
	    &&   obj != container
	    &&   can_drop_obj( ch, obj )
	    &&   get_obj_weight( obj ) + get_obj_weight( container )
		 <= container->value[0] )
	    {
		obj_from_char( obj );
		obj_to_obj( obj, container );
		act( "$n puts $p in $P.", ch, obj, container, TO_ROOM, FALSE );
		act( "You put $p in $P.", ch, obj, container, TO_CHAR, TRUE );
	    }
	}
    }

    return;
}



void do_drop( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  bool found;

  argument = one_argument( argument, arg );

  if ( !in_main_war( ch ) )
  {
    send_to_char( "You aren't in the war.\n\r", ch );
    return;
  }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Drop what?\n\r", ch );
	return;
    }

    if ( is_number( arg ) )
    {
	/* 'drop NNNN coins' */
	int amount;

	amount   = atoi(arg);
	argument = one_argument( argument, arg );
	if ( amount <= 0
	|| ( str_cmp( arg, "coins" ) && str_cmp( arg, "coin" ) ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "You haven't got that many coins.\n\r", ch );
	    return;
	}

	ch->gold -= amount;

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    switch ( obj->pIndexData->vnum )
	    {
	    case OBJ_VNUM_MONEY_ONE:
		amount += 1;
		extract_obj( obj, "do_drop" );
		break;

	    case OBJ_VNUM_MONEY_SOME:
		amount += obj->value[0];
		extract_obj( obj, "do_drop" );
		break;
	    }
	}

	obj_to_room( create_money( amount ), ch->in_room );
	act( "$n drops some gold.", ch, NULL, NULL, TO_ROOM, TRUE );
	send_to_char( "OK.\n\r", ch );
	return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	/* 'drop obj' */
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	act( "$n drops $p.", ch, obj, NULL, TO_ROOM, TRUE );
	act( "You drop $p.", ch, obj, NULL, TO_CHAR, TRUE );
    }
    else
    {
	/* 'drop all' or 'drop all.obj' */
	found = FALSE;
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   obj->wear_loc == WEAR_NONE
	    &&   can_drop_obj( ch, obj ) )
	    {
		found = TRUE;
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		act( "$n drops $p.", ch, obj, NULL, TO_ROOM, TRUE );
		act( "You drop $p.", ch, obj, NULL, TO_CHAR, TRUE );
	    }
	}

	if ( !found )
	{
	    if ( arg[3] == '\0' )
		act( "You are not carrying anything.",
		    ch, NULL, arg, TO_CHAR, TRUE );
	    else
		act( "You are not carrying any $T.",
		    ch, NULL, &arg[4], TO_CHAR, TRUE );
	}
    }

    return;
}



void do_give( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    if ( !in_main_war( ch ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Give what to whom?\n\r", ch );
	return;
    }

    if ( is_number( arg1 ) )
    {
	/* 'give NNNN coins victim' */
	int amount;

	amount   = atoi(arg1);
	if ( amount <= 0
	|| ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" ) ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	argument = one_argument( argument, arg2 );
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Give what to whom?\n\r", ch );
	    return;
	}

	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "You haven't got that much gold.\n\r", ch );
	    return;
	}

	ch->gold     -= amount;
	victim->gold += amount;
	act( "$n gives you some gold.", ch, NULL, victim, TO_VICT, TRUE    );
	act( "$n gives $N some gold.",  ch, NULL, victim, TO_NOTVICT, TRUE );
	act( "You give $N some gold.",  ch, NULL, victim, TO_CHAR, TRUE    );
	send_to_char( "OK.\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "You must remove it first.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
    {
	act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR, TRUE );
	return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
	act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR, TRUE );
	return;
    }

    if ( !can_see_obj( victim, obj ) && !IS_IMMORTAL( ch ) )
    {
	act( "$N can't see it.", ch, NULL, victim, TO_CHAR, TRUE );
	return;
    }

    obj_from_char( obj );
    obj_to_char( obj, victim );
    act( "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT, TRUE );
    act( "$n gives you $p.",   ch, obj, victim, TO_VICT, TRUE    );
    act( "You give $p to $N.", ch, obj, victim, TO_CHAR, TRUE    );
    return;
}




void do_fill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *fountain;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Fill what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    found = FALSE;
    for ( fountain = ch->in_room->contents; fountain != NULL;
	fountain = fountain->next_content )
    {
	if ( fountain->item_type == ITEM_FOUNTAIN )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
    {
	send_to_char( "There is no fountain here!\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "You can't fill that.\n\r", ch );
	return;
    }

    if ( obj->value[1] != 0 && obj->value[2] != 0 )
    {
	send_to_char( "There is already another liquid in it.\n\r", ch );
	return;
    }

    if ( obj->value[1] >= obj->value[0] )
    {
	send_to_char( "Your container is full.\n\r", ch );
	return;
    }

    act( "You fill $p.", ch, obj, NULL, TO_CHAR, TRUE );
    obj->value[2] = 0;
    obj->value[1] = obj->value[0];
    return;
}



void do_drink( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;

    if ( !in_a_war( ch ) )
    {
	send_to_char( "You aren't in a war!!\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	{
	    if ( obj->item_type == ITEM_FOUNTAIN )
		break;
	}

	if ( obj == NULL )
	{
	    send_to_char( "Drink what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if ( obj->item_type == ITEM_POTION )
    {
	do_quaff( ch, argument );
	return;
    }

    switch ( obj->item_type )
    {
    default:
	send_to_char( "You can't drink from that.\n\r", ch );
	break;

    case ITEM_FOUNTAIN:
    case ITEM_DRINK_CON:
	if ( obj->value[1] <= 0 )
	{
	    send_to_char( "It is already empty.\n\r", ch );
	    return;
	}

	if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}

	act( "$n drinks $T from $p.",
	    ch, obj, liq_table[liquid].liq_name, TO_ROOM, TRUE );
	act( "You drink $T from $p.",
	    ch, obj, liq_table[liquid].liq_name, TO_CHAR, TRUE );

	amount = number_range(3, 10);
	amount = UMIN(amount, obj->value[1]);

	ch->mana = UMIN( ch->max_mana, ch->mana +
                                liq_table[liquid].liq_affect[LIQUID_MANA  ]);
        if (!IS_AFFECTED( ch, AFF_BLACKMANTLE )
        && ch->char_class != CLASS_BARBARIAN )
        {
	  if( liq_table[liquid].liq_affect[LIQUID_HITS] > 0 )
	    heal_character( ch, liq_table[liquid].liq_affect[LIQUID_HITS  ] );
	  else
	  {
	    int dam;
	    dam = UMIN( ( ch->hit - 1 ),
	      -1 * liq_table[liquid].liq_affect[LIQUID_HITS] );
	    damage( ch, ch, dam, gsn_poison, 0 );
	  }
        }

        WAIT_STATE( ch, 8 );
	
	if ( obj->value[3] != 0 )
	{
	    /* The shit was poisoned ! */
	    AFFECT_DATA af;

	    act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM, TRUE );
	    send_to_char( "You choke and gag.\n\r", ch );
	    af.type      = gsn_poison;
/* longer duration (was 3 * amount) */
	    af.duration  = 120;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}

/* it uses 1 part, not amount.. */	
	obj->value[1] -= 1;
	if ( obj->value[1] <= 0 )
	{
	    send_to_char( "The empty container vanishes.\n\r", ch );
	    extract_obj( obj, "do_drink" );
	}
	break;
    }

    return;
}



void do_eat( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

/* make sure they are in the war */
    if ( !in_a_war( ch ) )
    {
	send_to_char( "You aren't in a war!!\n\r", ch );
	return;
    }

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Eat what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL(ch) )
    {
	if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL )
	{
	    send_to_char( "That's not edible.\n\r", ch );
	    return;
	}
    }

    WAIT_STATE( ch, 8 );

    act( "$n eats $p.",  ch, obj, NULL, TO_ROOM, TRUE );
    act( "You eat $p.", ch, obj, NULL, TO_CHAR, TRUE );

    switch ( obj->item_type )
    {

    case ITEM_FOOD:

	if ( obj->value[3] != 0 )
	{
	    /* The shit was poisoned! */
	    AFFECT_DATA af;

	    act( "$n chokes and gags.", ch, 0, 0, TO_ROOM, TRUE );
	    send_to_char( "You choke and gag.\n\r", ch );

	    af.type      = gsn_poison;
/* modifying duration from 2* obj->value[0] to 120 */
	    af.duration  = 120;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	break;

    case ITEM_PILL:
	obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
	obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
	obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
	break;
    }

    extract_obj( obj, "do_eat" );
    return;
}



/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	return TRUE;

    if ( !fReplace )
	return FALSE;

    if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
    {
	act( "You can't remove $p.", ch, obj, NULL, TO_CHAR, TRUE );
	return FALSE;
    }

    unequip_char( ch, obj );
    act( "$n stops using $p.", ch, obj, NULL, TO_ROOM, TRUE );
    act( "You stop using $p.", ch, obj, NULL, TO_CHAR, TRUE );
    return TRUE;
}


/* wield one weapon */
void wield_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
    if ( CAN_WEAR( obj, ITEM_WIELD ) )
    {
	if ( !remove_obj( ch, WEAR_WIELD, fReplace ) )
	    return;

	if ( get_obj_weight( obj ) > str_app[get_curr_str(ch)].wield )
	{
	    send_to_char( "It is too heavy for you to wield.\n\r", ch );
	    return;
	}

	act( "$n wields $p.", ch, obj, NULL, TO_ROOM, TRUE );
	act( "You wield $p.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_WIELD );

        if ( IS_SET( obj->extra_flags, ITEM_RANGED_WEAPON ) )
          obj->timer = obj->value[0];

	return;
    }
}


/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
    if ( obj->item_type == ITEM_LIGHT )
    {
	if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
	    return;
	act( "$n lights $p and holds it.", ch, obj, NULL, TO_ROOM, TRUE );
	act( "You light $p and hold it.",  ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_LIGHT );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
    {
	if ( get_eq_char( ch, WEAR_FINGER_L ) != NULL
	&&   get_eq_char( ch, WEAR_FINGER_R ) != NULL
	&&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
	&&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_FINGER_L ) != NULL
	&&   get_eq_char( ch, WEAR_FINGER_R ) != NULL )
	{
		send_to_char("Remove a ring first.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL )
	{
	    act( "$n wears $p on $s left finger.",    ch, obj, NULL, TO_ROOM, TRUE );
	    act( "You wear $p on your left finger.",  ch, obj, NULL, TO_CHAR, TRUE );
	    equip_char( ch, obj, WEAR_FINGER_L );
	    return;
	}

	if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL )
	{
	    act( "$n wears $p on $s right finger.",   ch, obj, NULL, TO_ROOM, TRUE );
	    act( "You wear $p on your right finger.", ch, obj, NULL, TO_CHAR, TRUE );
	    equip_char( ch, obj, WEAR_FINGER_R );
	    return;
	}

	bug( "Wear_obj: no free finger.", 0 );
	send_to_char( "You already wear two rings.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
    {
	if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
	&&   get_eq_char( ch, WEAR_NECK_2 ) != NULL
	&&   !remove_obj( ch, WEAR_NECK_1, fReplace )
	&&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
	&&   get_eq_char( ch, WEAR_NECK_2 ) != NULL )
	{
		send_to_char("Remove some neckwear first.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
	{
	    act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM, TRUE );
	    act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR, TRUE );
	    equip_char( ch, obj, WEAR_NECK_1 );
	    return;
	}

	if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
	{
	    act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM, TRUE );
	    act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR, TRUE );
	    equip_char( ch, obj, WEAR_NECK_2 );
	    return;
	}

	bug( "Wear_obj: no free neck.", 0 );
	send_to_char( "You already wear two neck items.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
    {
	if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
	    return;
	act( "$n wears $p on $s body.",   ch, obj, NULL, TO_ROOM, TRUE );
	act( "You wear $p on your body.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_BODY );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
    {
	if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
	    return;
	act( "$n wears $p on $s head.",   ch, obj, NULL, TO_ROOM, TRUE );
	act( "You wear $p on your head.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_HEAD );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
    {
	if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
	    return;
	act( "$n wears $p on $s legs.",   ch, obj, NULL, TO_ROOM, TRUE );
	act( "You wear $p on your legs.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_LEGS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
    {
	if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
	    return;
	act( "$n wears $p on $s feet.",   ch, obj, NULL, TO_ROOM, TRUE );
	act( "You wear $p on your feet.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_FEET );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
    {
	if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
	    return;
	act( "$n wears $p on $s hands.",   ch, obj, NULL, TO_ROOM, TRUE );
	act( "You wear $p on your hands.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_HANDS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
    {
	if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
	    return;
	act( "$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM, TRUE );
	act( "You wear $p on your arms.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_ARMS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
    {
	if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
	    return;
	act( "$n wears $p about $s body.",   ch, obj, NULL, TO_ROOM, TRUE );
	act( "You wear $p about your body.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_ABOUT );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
    {
	if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
	    return;
	act( "$n wears $p about $s waist.",   ch, obj, NULL, TO_ROOM, TRUE );
	act( "You wear $p about your waist.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_WAIST );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
    {
	if ( get_eq_char( ch, WEAR_WRIST_L ) != NULL
	&&   get_eq_char( ch, WEAR_WRIST_R ) != NULL
	&&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
	&&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL )
	{
	    act( "$n wears $p around $s left wrist.",
		ch, obj, NULL, TO_ROOM, TRUE );
	    act( "You wear $p around your left wrist.",
		ch, obj, NULL, TO_CHAR, TRUE );
	    equip_char( ch, obj, WEAR_WRIST_L );
	    return;
	}

	if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL )
	{
	    act( "$n wears $p around $s right wrist.",
		ch, obj, NULL, TO_ROOM, TRUE );
	    act( "You wear $p around your right wrist.",
		ch, obj, NULL, TO_CHAR, TRUE );
	    equip_char( ch, obj, WEAR_WRIST_R );
	    return;
	}

	bug( "Wear_obj: no free wrist.", 0 );
	send_to_char( "You already wear two wrist items.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
	if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
	    return;
	act( "$n wears $p as a shield.", ch, obj, NULL, TO_ROOM, TRUE );
	act( "You wear $p as a shield.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_SHIELD );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_HOLD ) )
    {
	if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
	    return;
	act( "$n holds $p in $s hands.",   ch, obj, NULL, TO_ROOM, TRUE );
	act( "You hold $p in your hands.", ch, obj, NULL, TO_CHAR, TRUE );
	equip_char( ch, obj, WEAR_HOLD );
	return;
    }

    if ( fReplace )
	send_to_char( "You can't wear that.\n\r", ch );

    return;
}



void do_wield( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if (ch->char_class == CLASS_MONK)
    {
	send_to_char( "Alas, it is against your vows to be so stylish.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Wear, wield, or hold what?\n\r", ch );
	return;
    }
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
		return;
	}

/* don't allow switching of weapons in combat */
	if (ch->fighting != NULL)
		wield_obj( ch, obj, FALSE );
	else
		wield_obj( ch, obj, TRUE );

    return;
}


void do_wear( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;

  if (ch->fighting != NULL)
  {
    send_to_char("Wear equipment while fighting?  Not.\n\r", ch );
    return;
  }

  one_argument( argument, arg );

  if ( arg[0] == '\0' )
  {
    send_to_char( "Wear, wield, or hold what?\n\r", ch );
    return;
  }

  if ( !str_cmp( arg, "all" ) )
  {
    OBJ_DATA *obj_next;

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
      obj_next = obj->next_content;

      /* barbarians can't use magic equipment */
      if( ch->char_class == CLASS_BARBARIAN && obj->pIndexData->affected )
	continue;

      if ( ch->char_class == CLASS_MONK
      && ( !CAN_WEAR( obj, ITEM_WEAR_FINGER )
          && !CAN_WEAR( obj, ITEM_WEAR_WRIST ) ) )
        continue;

	    if( obj->wear_loc != -1 )
		continue;

	    if ( can_see_obj( ch, obj ) )
		wear_obj( ch, obj, TRUE );
    }
    return;
  }
  else
  {
    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
    }

    /* barbarians can't use magic equipment */
    if ( ch->char_class == CLASS_BARBARIAN
    && obj->pIndexData->affected
    && obj->item_type != ITEM_WEAPON )
    {
      send_to_char( "You refuse to use such implements.\n\r", ch );
      return;
    }

    if (ch->char_class == CLASS_MONK
    && obj->item_type != ITEM_LIGHT
    && !CAN_WEAR( obj, ITEM_WEAR_FINGER )
    && !CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
    {
      send_to_char( "But that would weigh you down!\n\r", ch );
      return;
    }

    wear_obj( ch, obj, TRUE );
  }

  return;
}



void do_remove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    if (ch->fighting != NULL)
    {
	send_to_char("Remove equipment in combat? You've got to be kidding.\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_charm_person ) )
    {
      send_to_char("You don't want to strip in your master's presence!\n\r", ch );
      return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    remove_obj( ch, obj->wear_loc, TRUE );
    return;
}



void do_sacrifice( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    extern int war_flags;

    one_argument( argument, arg );

    if (IS_NPC(ch))
	return;

    if ( !in_main_war( ch ) )
	return;

    if ( arg[0] == '\0' || !str_cmp( arg, ch->name ) )
    {
	act( "$n offers $mself to God, who graciously declines.",
	    ch, NULL, NULL, TO_ROOM, TRUE );
	send_to_char(
	    "God appreciates your offer and may accept it later.", ch );
	return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }

/* give players a jar of blood if they kill a player */
    if (obj->item_type == 24)
    {
      if ( IS_SET( war_flags, WAR_FLAG_BLOODS ) )
      {
         OBJ_DATA *jar_blood;
         jar_blood = create_object( get_obj_index( 2288 ) );
         obj_to_char( jar_blood , ch );
         send_to_char( "Rip gives you a nice tasty jar of blood for your sacrifice.\n\r", ch );
    act( "$n sacrifices $p to God.", ch, obj, NULL, TO_ROOM, TRUE );
         extract_obj( obj, "do_sacrifice" );
         return;
      }
      else
      {
	send_to_char("It isn't a blood war.\n\r", ch );
	return;
      }
    }

    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	act( "$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR, TRUE );
	return;
    }

    send_to_char( "God gives you one gold coin for your sacrifice.\n\r", ch );
    ch->gold += 1;

    act( "$n sacrifices $p to God.", ch, obj, NULL, TO_ROOM, TRUE );
    extract_obj( obj, "do_sacrifice" );
    return;
}

void do_use( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *device;
    bool found = FALSE;
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( !in_main_war( ch ) )
	return;

    if ( arg1[0] == '\0' )
    {
	send_to_char("Use what device?\n\r", ch );
	return;
    }

    if ( ( device = get_eq_char( ch, WEAR_HOLD ) ) != NULL )
    {
	if ( is_name( arg1, device->name ) )
	    found = TRUE;
    }

    if ( !found && ( device = get_obj_carry( ch, arg1 ) ) != NULL )
    {
	if ( is_name( arg1, device->name ) )
	    found = TRUE;
    }

    if ( !found )
    {
	send_to_char("You don't have anything like that to use.\n\r", ch );
	return;
    }

    if ( device->item_type == ITEM_WAND )
	use_wand( ch, device, arg2 );
    else if ( device->item_type == ITEM_STAFF )
	use_staff( ch, device, arg2 );
    else if ( device->item_type == ITEM_ARMOR )
	do_wear( ch, arg1 );
    else if ( device->item_type == ITEM_WEAPON )
	do_wield( ch, arg1 );
    else if ( device->item_type == ITEM_LIGHT )
	do_wear( ch, arg1 );
    else
    {
	act("You can't figure out what to do with $p.",
	    ch, device, NULL, TO_CHAR, TRUE );
	return;
    }

    return;
}


void do_quaff( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
  extern int war_flags;

/* barbarians cant */
    if (!IS_NPC(ch) && ch->char_class == CLASS_BARBARIAN )
    {
	send_to_char( "You refuse to use such magic implements.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Quaff what?\n\r", ch );
	return;
    }

    obj = get_eq_char( ch, WEAR_HOLD );

  if ( obj == NULL || obj->item_type != ITEM_POTION )
    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that potion.\n\r", ch );
	return;
    }

    if ( obj->item_type == ITEM_DRINK_CON )
    {
	do_drink( ch, argument );
	return;
    }

    if ( obj->item_type != ITEM_POTION )
    {
	send_to_char( "You can quaff only potions.\n\r", ch );
	return;
    }

    act( "$n quaffs $p.", ch, obj, NULL, TO_ROOM, TRUE );
    act( "You quaff $p.", ch, obj, NULL ,TO_CHAR, TRUE );

  if( !IS_SET(war_flags, WAR_FLAG_NOQUAFF ) )
  {
    obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
    obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
    obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
  }

    extract_obj( obj, "do_quaff" );
    return;
}



void do_recite( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *scroll;
    OBJ_DATA *obj;

/* barbarians cant */
    if (ch->char_class == CLASS_BARBARIAN )
    {
	send_to_char( "You refuse to use such magic implements.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( scroll = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that scroll.\n\r", ch );
	return;
    }

    if ( scroll->item_type != ITEM_SCROLL )
    {
	send_to_char( "You can recite only scrolls.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg2[0] == '\0' )
    {
	victim = ch;
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if ( victim && !in_main_war( ch ) && in_main_war( victim ) )
    {
	send_to_char( "You can't recite scrolls on players in the main war if you are not in the war!\n\r", ch );
	return;
    }

    act( "$n recites $p.", ch, scroll, NULL, TO_ROOM, TRUE );
    act( "You recite $p, which dissolves.", ch, scroll, NULL, TO_CHAR, TRUE );

  if (!IS_NPC(ch) && (number_percent() < ch->pcdata->learned[gsn_read_magic]))
  {
    obj_cast_spell( scroll->value[1], scroll->value[0], ch, victim, obj );
    obj_cast_spell( scroll->value[2], scroll->value[0], ch, victim, obj );
    obj_cast_spell( scroll->value[3], scroll->value[0], ch, victim, obj );
  }
  else
  {
	send_to_char("You failed in the reading!\n\r", ch );
  }

    extract_obj( scroll, "do_recite" );
    return;
}



void use_wand( CHAR_DATA *ch, OBJ_DATA *device, char *argument )
{
    int chance;
    CHAR_DATA *victim;
    OBJ_DATA *obj = NULL;
    OBJ_DATA *wielded = NULL;

    if (ch->char_class == CLASS_BARBARIAN )
    {
	send_to_char( "You refuse to use such magic implements.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' ||
    ( ( victim = get_char_room( ch, argument ) ) == NULL
    && ( obj = get_obj_here( ch, argument ) ) == NULL ) )
    {
	send_to_char("Point the device at whom or what?\n\r", ch );
	act( "$n fiddles with $p.", ch, device, NULL, TO_ROOM, FALSE );
	return;
    }

    if ( IS_NPC( ch ) )
    {
	switch( ch->char_class )
	{
	    default:
		chance = 0;  break;
	    case CLASS_MAGE:
		chance = ch->level * 3;  break;
	    case CLASS_CLERIC:
		chance = ch->level * 2;  break;
	    case CLASS_BARD:
		chance = ch->level * 1;  break;
	}
    }
    else
	chance = ch->pcdata->learned[gsn_use_wands];

    WAIT_STATE( ch, 12 );

    if ( victim != NULL )
    {
	act( "$n points $p at $N.", ch, device, victim, TO_NOTVICT, FALSE );
	act( "$n points $p at you", ch, device, victim, TO_VICT, TRUE );
	act( "You point $p at $N.", ch, device, victim, TO_CHAR, TRUE );
    }
    else
    {
	act( "$n point $p at $P.", ch, device, obj, TO_ROOM, FALSE );
	act( "You point $p at $P.", ch, device, obj, TO_CHAR, TRUE );
    }

    if ( device->value[1] == 0 )
	return;

    if ( device->value[1] > 0 )
	device->value[1] -= 1;

    if ( number_percent() > chance )
    {
	if ( number_percent( ) < ( device->value[0] / 2 ) )
	{
	    bool fSame = FALSE;
	    wielded = get_eq_char( ch, WEAR_WIELD );
	    if ( wielded == device )
		fSame = TRUE;
	    else
	    {
		if ( wielded )
		    unequip_char( ch, wielded );
		unequip_char( ch, device );
		equip_char( ch, device, WEAR_WIELD );
	    }
	    damage( ch, ch,
		dice( 1, (device->value[0]+(device->value[0]/2)) ),
		gsn_use_wands, 0 );
	    extract_obj( device, "use_wand" );
	    if ( !fSame && wielded )
		equip_char( ch, wielded, WEAR_WIELD );
	    return;
	}

	return;
    }

    obj_cast_spell( device->value[3], device->value[0], ch, victim, obj );
    return;
}

void use_staff( CHAR_DATA *ch, OBJ_DATA *device, char *argument )
{
    int chance;
    CHAR_DATA *victim = NULL;
    CHAR_DATA *vch = NULL;
    CHAR_DATA *vch_next = NULL;
    OBJ_DATA *obj = NULL;
    OBJ_DATA *wielded = NULL;
    bool fRoom = FALSE;
    int sn;

    if (ch->char_class == CLASS_BARBARIAN )
    {
	send_to_char( "You refuse to use such magic implements.\n\r", ch );
	return;
    }

    sn = device->value[3];

    if ( argument[0] == '\0' ||
    ( ( victim = get_char_room( ch, argument ) ) == NULL
    && ( obj = get_obj_here( ch, argument ) ) == NULL ) )
	fRoom = TRUE;

    WAIT_STATE( ch, 24 );

    if ( victim != NULL )
    {
	act( "$n points $p at $N.", ch, device, victim, TO_NOTVICT, FALSE );
	act( "$n points $p at you.", ch, device, victim, TO_VICT, FALSE );
	act( "You point $p at $N.", ch, device, victim, TO_CHAR, TRUE );
    }
    else if ( obj != NULL )
    {
	act( "$n point $p at $P.", ch, device, obj, TO_ROOM, FALSE );
	act( "You point $p at $P.", ch, device, obj, TO_CHAR, TRUE );
    }
    else
    {
	act( "$n taps $p on the ground sharply.",
	    ch, device, NULL, TO_ROOM, FALSE );
	act( "You tap $p on the ground sharply.",
	    ch, device, NULL, TO_CHAR, TRUE );
    }

    if ( device->value[1] == 0 )
	return;

    if ( device->value[1] > 0 )
	device->value[1] -= 1;

    if ( IS_NPC( ch ) )
    {
	switch( ch->char_class )
	{
	    default:
		chance = 0;  break;
	    case CLASS_MAGE:
		chance = UMAX( 0, ch->level - 5 ) * 3;  break;
	    case CLASS_CLERIC:
		chance = UMAX( 0, ch->level - 10 ) * 2;  break;
	}
    }
    else
	chance = ch->pcdata->learned[gsn_use_staffs];

    if ( number_percent() > chance )
    {
	if ( number_percent() <(  device->value[0] / 2 ) )
	{
	    bool fSame = FALSE;
	    wielded = get_eq_char( ch, WEAR_WIELD );
	    if ( wielded == device )
		fSame = TRUE;
	    else
	    {
		if ( wielded )
		    unequip_char( ch, wielded );
		unequip_char( ch, device );
		equip_char( ch, device, WEAR_WIELD );
	    }
	    damage( ch, ch,
		dice( 1, (device->value[0]+(device->value[0]/2)) ),
		gsn_use_staffs, 0 );
	    extract_obj( device, "use_staff" );
	    if ( !fSame && wielded )
		equip_char( ch, wielded, WEAR_WIELD );
	    return;
	}

	return;
    }

    if ( !fRoom )
	obj_cast_spell( device->value[3], device->value[0], ch, victim, obj );
    else
    {
	for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
	{
	    vch_next	= vch->next_in_room;

	    switch ( skill_table[sn].target )
	    {
	    default:
		bug( "Do_use (staff room): bad target for sn %d.", sn );
		return;

	    case TAR_IGNORE:
		if ( vch != ch )
		    continue;
		break;

	    case TAR_CHAR_OFFENSIVE:
	    case TAR_CHAR_HINDRANCE:
		if ( vch == ch )
		    continue;
		break;
		
	    case TAR_CHAR_DEFENSIVE:
		if ( vch == ch )
		    continue;
		break;

	    case TAR_CHAR_SELF:
		if ( vch == ch )
		    continue;
		break;
	    }

	    obj_cast_spell( device->value[3], device->value[0], ch, vch, NULL );
	}
    }

    return;
}



void do_steal( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if (ch->fighting != NULL)
    {
	send_to_char("Steal while fighting?  Not bloody likely!\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Steal what from whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if( IS_NPC(victim) && ch->level == 30 )
    {
	send_to_char("No stealing from mobs, flap.\n\r",ch);
	return;
    }

    if ( !in_main_war( ch ) || !in_main_war( victim ) )
    {
	send_to_char( "Either you or they aren't in a war!\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "That's pointless.\n\r", ch );
	return;
    }

    obj = get_obj_carry( victim, arg1 );

    if ( obj == NULL )
    {
	obj = get_obj_wear( victim, arg1 );

	if ( ch->char_class != CLASS_THIEF
	|| obj == NULL
	|| obj->item_type != ITEM_CONTAINER )
	{
	    send_to_char("You can't find it.\n\r", ch );
	    return;
	}
    }

    if ( !str_cmp( arg1, "coin"  )
    ||   !str_cmp( arg1, "coins" )
    ||   !str_cmp( arg1, "gold"  ) )
    {
	int amount;

	amount = victim->gold * number_range(1, 10) / 100;
	if ( amount <= 0 )
	{
	    send_to_char( "You couldn't get any gold.\n\r", ch );
	    return;
	}

	ch->gold     += amount;
	victim->gold -= amount;
	sprintf( buf, "Bingo!  You got %d gold coins.\n\r", amount );
	send_to_char( buf, ch );
	return;
    }

    attempt_steal_object( ch, victim, obj );
}


void attempt_steal_object( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];
    int skill;
    int percent;

    if ( IS_NPC( ch ) )
	skill = ch->level * 4;
    else
	skill = ch->pcdata->learned[gsn_steal];

    WAIT_STATE( ch, skill_table[gsn_steal].beats );
    percent  = number_percent( ) + ( IS_AWAKE(victim) ? 10 : -50 );

    if ( IS_NPC( victim ) && victim->pIndexData->pShop != NULL )
	percent += 50;

    if ( ch->level + 5 < victim->level
    ||   victim->fighting != NULL
    ||   victim->position == POS_FIGHTING
    ||   percent > skill )
    {
	/*
	 * Failure.
 	 */
	send_to_char( "Oops.\n\r", ch );
	act( "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT, TRUE    );
	act( "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT, TRUE );
	sprintf( buf, "%s is a bloody thief!",
	    IS_NPC( ch ) ? ch->short_descr : ch->name );
	do_shout( victim, buf );
	if ( !IS_NPC( ch ) && IS_NPC( victim ) )
	    multi_hit( victim, ch, TYPE_UNDEFINED );

	return;
    }

    if ( !IS_NPC( ch ) && !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't pry it away.\n\r", ch );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	send_to_char( "You have your hands full.\n\r", ch );
	return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
	send_to_char( "You can't carry that much weight.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
	remove_obj( victim, obj->wear_loc, FALSE );
    obj_from_char( obj );
    obj_to_char( obj, ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}



/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch )
{
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;

    pShop = NULL;
    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
    {
	if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
	    break;
    }

    if ( pShop == NULL )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return NULL;
    }

    /*
     * Invisible or hidden people.
     */
/* can see invis */
/*
    if ( !can_see( keeper, ch ) )
    {
	do_say( keeper, "I don't trade with folks I can't see." );
	return NULL;
    }
*/

    return keeper;
}



int get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
    SHOP_DATA *pShop;
    int cost;

    if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL )
	return 0;

    if ( fBuy )
    {
	cost = obj->cost * pShop->profit_buy  / 100;

	if ( obj->item_type == ITEM_CONTAINER )
	    cost *= 100;

	if ( obj->item_type == ITEM_DRINK_CON )
	    cost *= 10;
    }
    else
    {
	int itype;

	cost = 0;
	for ( itype = 0; itype < MAX_TRADE; itype++ )
	{
	    if ( obj->item_type == pShop->buy_type[itype] )
	    {
		cost = obj->cost * pShop->profit_sell / 100;

		cost = ( (cost < 25) ? cost : 25 );
		break;
	    }
	}
    }

    if ( ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
         && obj->value[2] > 0 )
	cost = cost * obj->value[1] / obj->value[2];

    return cost;
}



void do_buy( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    extern int war_status;

    if ( war_status < WAR_ON && get_trust( ch ) < 40 )
    {
	send_to_char("Get some money first you flap.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Buy what?\n\r", ch );
	return;
    }

    {
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	OBJ_DATA *new_obj;
	int cost;

	if ( ( keeper = find_keeper( ch ) ) == NULL )
	    return;

	obj  = get_obj_carry( keeper, arg );
	cost = get_cost( keeper, obj, TRUE );

	if ( cost <= 0 || !can_see_obj( ch, obj ) )
	{
	    act( "$n tells you 'I don't sell that -- try 'list''.",
		keeper, NULL, ch, TO_VICT, TRUE );
	    ch->reply = keeper;
	    return;
	}

	if ( ch->gold < cost )
	{
	    act( "$n tells you 'You can't afford to buy $p'.",
		keeper, obj, ch, TO_VICT, TRUE );
	    ch->reply = keeper;
	    return;
	}
	
	if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
	{
	    send_to_char( "You can't carry that many items.\n\r", ch );
	    return;
	}

	if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
	    send_to_char( "You can't carry that much weight.\n\r", ch );
	    return;
	}

	act( "$n buys $p.", ch, obj, NULL, TO_ROOM, TRUE );
	act( "You buy $p.", ch, obj, NULL, TO_CHAR, TRUE );
	ch->gold     -= cost;
	keeper->gold += cost;

	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	{
	    new_obj = create_object( obj->pIndexData );
            obj_to_char( new_obj, keeper );
            SET_BIT( new_obj->extra_flags, ITEM_INVENTORY );
	}
	    obj_from_char( obj );

	obj_to_char( obj, ch );
	return;
    }
}



void do_list( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern int war_status;

    if ( !in_main_war( ch ) )
    {
	send_to_char("Like dead guys like you can use anything I got.\n\r", ch );
	return;
    }

    if ( war_status < WAR_ON && get_trust( ch ) < 40 )
    {
	send_to_char("Quit scoutin for yer mortal wuss.\n\r", ch );
	return;
    }

    {
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost;
	bool found;

	one_argument( argument, arg );

	if ( ( keeper = find_keeper( ch ) ) == NULL )
	    return;

	found = FALSE;
	for ( obj = keeper->carrying; obj; obj = obj->next_content )
	{
	    if ( obj->wear_loc == WEAR_NONE
	    &&   can_see_obj( ch, obj )
	    && ( cost = get_cost( keeper, obj, TRUE ) ) > 0
	    && ( arg[0] == '\0' || is_name( arg, obj->name ) ) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    send_to_char( "[Lv Price] Item\n\r", ch );
		}

		sprintf( buf, "[%5d] %s.\n\r",
		    cost, capitalize( obj->short_descr ) );
		send_to_char( buf, ch );
	    }
	}

	if ( !found )
	{
	    if ( arg[0] == '\0' )
		send_to_char( "You can't buy anything here.\n\r", ch );
	    else
		send_to_char( "You can't buy that here.\n\r", ch );
	}
	return;
    }
}



void do_sell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    if ( !in_main_war( ch ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Sell what?\n\r", ch );
	return;
    }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
	return;

    if ( IS_AFFECTED( ch, AFF_INVISIBLE ) )
    {
	act( "$n says 'I don't deal with invis scumbags!'.", keeper, NULL, NULL, TO_ROOM, TRUE );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	act( "$n tells you 'You don't have that item'.",
	    keeper, NULL, ch, TO_VICT, TRUE );
	ch->reply = keeper;
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT, TRUE );
	return;
    }

    act( "$n sells $p.", ch, obj, NULL, TO_ROOM, TRUE );
    sprintf( buf, "You sell $p for %d gold piece%s.",
	cost, cost == 1 ? "" : "s" );
    act( buf, ch, obj, NULL, TO_CHAR, TRUE );
    ch->gold     += cost;
    keeper->gold -= cost;

    if ( keeper->gold < 0 )
	keeper->gold = 0;

    if ( obj->item_type == ITEM_TRASH )
    {
	extract_obj( obj, "do_sell" );
    }
    else
    {
	obj_from_char( obj );
	obj_to_char( obj, keeper );
    }

    return;
}



void do_value( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Value what?\n\r", ch );
	return;
    }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
	return;

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	act( "$n tells you 'You don't have that item'.",
	    keeper, NULL, ch, TO_VICT, TRUE );
	ch->reply = keeper;
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT, TRUE );
	return;
    }

    sprintf( buf, "$n tells you 'I'll give you %d gold coins for $p'.", cost );
    act( buf, keeper, obj, ch, TO_VICT, TRUE );
    ch->reply = keeper;

    return;
}



void do_junk( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj;

  if ( strlen( argument ) == 0 )
  {
    send_to_char("Junk what?\n\r", ch );
    return;
  }

  if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
  {
    send_to_char("You don't carry that object.\n\r", ch );
    return;
  }

  if ( IS_SET( obj->extra_flags, ITEM_NODROP  )
  && in_static_war( ch ) )
  {
    send_to_char("You can't seem to get it out of your hands.\n\r", ch );
    return;
  }

  act("$n destroys $p.", ch, obj, NULL, TO_ROOM, TRUE );
  act("You destroy $p.", ch, obj, NULL, TO_CHAR, TRUE );
  extract_obj( obj, "do_junk" );

  return;
}


void do_coinflip( CHAR_DATA *ch, char *arg )
{
  if ( number_bits( 1 ) )
  {
    send_to_char( "The coin comes up heads.\n\r", ch );
    act( "The coin flipped by $n comes up heads.",
      ch, NULL, NULL, TO_ROOM, TRUE );
  }
  else
  {
    send_to_char( "The coin lands tails.\n\r", ch );
    act( "The coin flipped by $n lands tails.",
      ch, NULL, NULL, TO_ROOM, TRUE );
  }

  return;
}

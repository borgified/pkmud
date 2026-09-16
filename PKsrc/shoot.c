/*
 * shoot.c, May 3,4 1996 (mostly) by Robert L. Peckham (rip)
 * for Sword Quest MURPE.
 *
 * ported later May 4, 1996 to PKmud
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "cmod.h"
#include "global.h"

void do_shoot( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *vtemp;
    OBJ_DATA *bow;                /* or crossbow, or sling, etc */
    OBJ_DATA *arrow;
    ROOM_INDEX_DATA *in_room = ch->in_room;
    ROOM_INDEX_DATA *target_room;
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int direction;
    int nSkill;
    int ii;

    if ( IS_NPC( ch ) && ch->master != NULL )
	return;

    if ( !in_main_war( ch ) )
    {
	send_to_char("You aren't in the war.\n\r", ch );
	return;
    }

    nSkill = IS_NPC( ch ) ? 95 : ch->pcdata->learned[gsn_archery];

    if ( ( bow = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| bow->item_type != ITEM_WEAPON
	|| !IS_SET( bow->extra_flags, ITEM_RANGED_WEAPON ) )
    {
	send_to_char("You aren't wielding a ranged weapon.\n\r", ch );
	return;
    }

    if ( bow->timer > 0 )
    {
	send_to_char("Your weapon isn't ready to fire yet.\n\r", ch );
	return;
    }

    if ( ( arrow = get_eq_char( ch, WEAR_WAIST ) ) == NULL
	|| arrow->item_type != ITEM_QUIVER )
    {
	send_to_char("What do you intend on firing?\n\r", ch );
	return;
    }

    if ( arrow->value[2] == 0 )
    {
	send_to_char("Your quiver is empty.\n\r", ch );
	return;
    }

    /* check ammunition type */

    argument = one_argument( argument, arg1 );

    if ( arg1 == NULL || arg1[0] == '\0' )
    {
	send_to_char("Who do you want to shoot at?\n\r", ch );
	return;
    }

    if ( (direction = get_direction( argument ) ) == -1 )
    {
	if ( argument != NULL && argument[0] != '\0' )
	{
	    send_to_char("What direction is that?\n\r", ch );
	    return;
	}

	if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
	{
	    for ( ii = 0; ii < 6; ii++ )
	    {
/* need to change range 5 to based on player's vision */
		if ( (vtemp = search_dir_name( ch, arg1, ii, 5 ) ) != NULL )
		{
		    if ( direction != -1 )
		    {
			send_to_char("Multiple targets like that, which direction?\n\r", ch );
			return;
		    }
		    direction = ii;
		    victim = vtemp;
		}
	    }
	}
    }
    else
    {
	victim = search_dir_name( ch, arg1, direction, 5 );
    }

    if ( victim == NULL )
    {
	send_to_char("You don't see anyone like that around.\n\r", ch );
        check_failed_combat( ch );
	return;
    }

    if ( !in_main_war( victim ) )
    {
	send_to_char("They aren't in the war.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char("Shoot yourself?\n\r", ch );
	return;
    }

    if (!IS_NPC(ch))
        ch->pcdata->failed_combat = 0;

/* we have a victim and a direction.  start the missile off. */

    if ( arrow->value[2] > 0 && !IS_NPC(ch) )
	arrow->value[2]--;

    if ( bow->value[0] > 0 )
    {
        if ( IS_NPC( ch ) )
	    ch->mana -= 2 * bow->value[0];
	else
	    bow->timer = bow->value[0];
    }
    WAIT_STATE( ch , UMAX( skill_table[gsn_archery].beats,
        class_table[ch->char_class].skill_delay[gsn_archery] ) );

    if ( number_percent() > nSkill )
    {
	send_to_char("You fumble your weapon as you attempt to fire it.\n\r", ch );
	act("$n attempts to fire $p, but fumbles.", ch, bow, NULL, TO_ROOM, TRUE );

	unequip_char( ch, bow );
	obj_from_char( bow );
	obj_to_room( bow, ch->in_room );

	return;
    }

    if ( ch->in_room == victim->in_room )
    {
      act("$n turns and fires $p at you!", ch, bow, victim, TO_VICT, TRUE );
      act("$n turns and fires $p at $N!", ch, bow, victim, TO_NOTVICT, TRUE );
      act("You turn and fire $p at $N.", ch, bow, victim, TO_CHAR, TRUE );

/* charm arrow fix */
      if( arrow->value[1] == gsn_charm_person )
      {
	obj_cast_spell( arrow->value[1], ch->level, ch, victim, NULL );
	return;
      }

      if( arrow->value[1] > 0 )
      {
	obj_cast_spell( arrow->value[1], ch->level, ch, victim, NULL );
	if( IS_SET(arrow->extra_flags, ITEM_RANGED_MAGIC) )
	{
	  damage(ch, victim, dice(bow->value[1],bow->value[2]),
	    0, victim->resist_physical );
	}
	return;
      }
      damage( ch, victim, 4*(dice(bow->value[1],bow->value[2])),
        0, victim->resist_physical );
      return;
    }


    /* ranged firing section */
    {
	sprintf( buf, "You fire $p %swards.", dir_name[direction] );
	act( buf, ch, bow, NULL, TO_CHAR, TRUE );
	sprintf( buf, "$n fires $s $p %swards.", dir_name[direction] );
	act( buf, ch, bow, NULL, TO_ROOM, TRUE );

	target_room = ch->in_room->exit[direction]->to_room;

	sprintf( buf, "An arrow flies across the room %swards.", dir_name[direction] );

	/* send the arrow on it's course */
	/* we know the victim is on the path since it was searched above */
	/* test skill in each room */
	while ( 1 )
	{
	    target_room = ch->in_room->exit[direction]->to_room;
	    char_from_room( ch );
	    char_to_room( ch, target_room );

	    if ( target_room == victim->in_room )
		break;

	    if ( number_percent() > nSkill )
	    {
		send_to_char( "Your shot fell short of it's target.\n\r", ch );
		act( "$p flies in from $T and hits the ground.",
		    ch, arrow, dir_name_2[rev_dir[direction]], TO_ROOM, TRUE );

		char_from_room( ch );
		char_to_room( ch, in_room );
		return;
	    }

	    act( buf, ch, arrow, NULL, TO_ROOM, TRUE );
	}

	act( "An arrow flies in from $T...",
	    ch, NULL, dir_name_2[rev_dir[direction]], TO_ROOM, TRUE );

	send_to_char( "Your shot had enough range...\n\r", ch );

	/* enough distance, check aim */
	if ( number_percent() > nSkill )
	{

	    act("...And hits the ground.", ch, NULL, NULL, TO_ROOM, TRUE );

	    char_from_room( ch );
	    char_to_room( ch, in_room );
	    return;
	}

	/* do damage and generate messages */
	act("...And hits $n.", victim, NULL, NULL, TO_ROOM, TRUE );
	send_to_char("...And hits you!\n\r", victim );

	set_fighting( ch, victim );
	if( arrow->value[1] > 0 )
	{
	    obj_cast_spell( arrow->value[1], ch->level, ch, victim, NULL );
	    if( IS_SET(arrow->extra_flags, ITEM_RANGED_MAGIC ) )
		damage( ch, victim, dice(bow->value[1],bow->value[2]),
		  0, victim->resist_physical );
	}
	else
	{
	    damage( ch, victim, 4*(dice(bow->value[1],bow->value[2])),
	      0, victim->resist_physical );
	}

/*
	if ( ch->fighting != NULL && ch->in_room != ch->fighting->in_room )
	    stop_fighting( ch, TRUE ); 
*/
	/*
	 * must check ch->fighting to make sure victim still exists.
	 */
	if ( ch->fighting != NULL
	&& victim->fighting == ch )
	{
	    stop_fighting( ch, FALSE );
	    stop_fighting( victim, FALSE );
	}

	char_from_room( ch );
	char_to_room( ch, in_room );
    }

    return;
}

CHAR_DATA *search_dir_name( CHAR_DATA *ch, char *argument, int direction, int range )
{
    CHAR_DATA *victim = NULL;
    ROOM_INDEX_DATA *in_room = ch->in_room;
    ROOM_INDEX_DATA *search_room = ch->in_room;
    EXIT_DATA *pexit;
    int irange;

    if ( direction < 0 || direction > 5 )
	return NULL;

    if ( argument == NULL || argument[0] == '\0' )
	return NULL;

    for ( irange = 1; irange <= range; irange++ )
    {
	if ( ( pexit = search_room->exit[direction] ) == NULL )
	    break;

	if ( ( search_room = pexit->to_room ) == NULL )
	    break;

	if ( is_door_closed( pexit, ch ) )
	    break;

	char_from_room( ch );
	char_to_room( ch, search_room );

	if ( ( victim = get_char_room( ch, argument ) ) != NULL )
	    break;
    }

    char_from_room( ch );
    char_to_room( ch, in_room );
    return victim;
}

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
#include "global.h"



/*
 * The following special functions are available for objects.
 */
DECLARE_WEAR_FUN(	spec_inviso		);
DECLARE_WEAR_FUN(	spec_fly		);
DECLARE_WEAR_FUN(	spec_sanct		);
DECLARE_WEAR_FUN(	spec_detect_invis	);
DECLARE_WEAR_FUN(	spec_albert_ring	);


WEAR_FUN *wear_fun_lookup( const char *name )
{
    if ( !str_cmp( name, "spec_inviso"            ) ) return spec_inviso;
    if ( !str_cmp( name, "spec_fly"               ) ) return spec_fly;
    if ( !str_cmp( name, "spec_sanct"             ) ) return spec_sanct;
    if ( !str_cmp( name, "spec_detect_invis"      ) ) return spec_detect_invis;
    if ( !str_cmp( name, "spec_albert_ring"       ) ) return spec_albert_ring;
    return 0;
}


void spec_inviso( CHAR_DATA *ch, OBJ_DATA *obj, bool on_off )
{
	AFFECT_DATA af;

	if ( on_off == PUT_ON )
	{
		if ( IS_AFFECTED( ch, AFF_INVISIBLE ) )
			return;
		af.type = gsn_invis;
		af.duration = -1;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_INVISIBLE;
		affect_to_char( ch, &af );
	}
	else
	{
		affect_strip( ch, gsn_invis );
		REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
	}
	return;
}

void spec_detect_invis( CHAR_DATA *ch, OBJ_DATA *obj, bool on_off )
{
	AFFECT_DATA af;

	if ( on_off == PUT_ON )
	{
		if ( IS_AFFECTED( ch, AFF_DETECT_INVIS ) )
			return;
		af.type = gsn_detect_invis;
		af.duration = -1;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_DETECT_INVIS;
		affect_to_char( ch, &af );
	}
	else
	{
		affect_strip( ch, gsn_detect_invis );
		REMOVE_BIT( ch->affected_by, AFF_DETECT_INVIS );
	}
	return;
}

void spec_fly( CHAR_DATA *ch, OBJ_DATA *obj, bool on_off )
{
	AFFECT_DATA af;

	if ( on_off == PUT_ON )
	{
		if ( IS_AFFECTED( ch, AFF_FLYING ) )
			return;
		af.type = gsn_fly;
		af.duration = -1;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_FLYING;
		affect_to_char( ch, &af );
	}
	else
	{
		affect_strip( ch, gsn_fly );
		REMOVE_BIT( ch->affected_by, AFF_FLYING );
	}
	return;
}

void spec_sanct( CHAR_DATA *ch, OBJ_DATA *obj, bool on_off )
{
	AFFECT_DATA af;

	if ( on_off == PUT_ON )
	{
		if ( IS_AFFECTED( ch, AFF_SANCTUARY ) )
			return;
		af.type = gsn_sanct;
		af.duration = -1;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_SANCTUARY;
		affect_to_char( ch, &af );
	}
	else
	{
		affect_strip( ch, gsn_sanct );
		REMOVE_BIT( ch->affected_by, AFF_SANCTUARY );
	}
	return;
}


void spec_albert_ring( CHAR_DATA *ch, OBJ_DATA *obj, bool on_off )
{
	if ( on_off == PUT_ON )
	{
		act("$n fades from existance.", ch, NULL, NULL, TO_ROOM, TRUE );
		char_from_room(ch);
		char_to_room( ch, get_room_index( 12613 ) );
		act("$n appears in the room.", ch, NULL, NULL, TO_ROOM, TRUE );
		do_look( ch, str_auto );
	}

	return;
}

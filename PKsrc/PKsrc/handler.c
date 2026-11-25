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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "global.h"
#include "CharIterator.h"
#include "ObjIterator.h"



AFFECT_DATA *		affect_free;



/*
 * Local functions.
 */
void	affect_modify	args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );



/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust( CHAR_DATA *ch )
{
    if ( ch->desc != NULL && ch->desc->original != NULL )
	ch = ch->desc->original;

    if ( ch->trust != 0 )
	return ch->trust;

    if ( IS_NPC(ch) && ch->level >= LEVEL_HERO )
	return LEVEL_HERO - 1;
    else
	return ch->level;
}



/*
 * Retrieve a character's age.
 */
int get_age( CHAR_DATA *ch )
{
    return 17 + ( ch->played + (int) (current_time - ch->logon) ) / 7200;
}



/*
 * Retrieve character's current strength.
 */
int get_curr_str( CHAR_DATA *ch )
{
    int max;

    if ( IS_NPC(ch) )
	return 13;

    max = 25;

    return URANGE( 3, ch->pcdata->perm_str + ch->pcdata->mod_str, max );
}



/*
 * Retrieve character's current intelligence.
 */
int get_curr_int( CHAR_DATA *ch )
{
    int max;

    if ( IS_NPC(ch) )
	return 13;

    max = 25;

    return URANGE( 3, ch->pcdata->perm_int + ch->pcdata->mod_int, max );
}



/*
 * Retrieve character's current wisdom.
 */
int get_curr_wis( CHAR_DATA *ch )
{
    int max;

    if ( IS_NPC(ch) )
	return 13;

    max = 25;

    return URANGE( 3, ch->pcdata->perm_wis + ch->pcdata->mod_wis, max );
}



/*
 * Retrieve character's current dexterity.
 */
int get_curr_dex( CHAR_DATA *ch )
{
    int max;

    if ( IS_NPC(ch) )
	return 13;

    max = 25;

    return URANGE( 3, ch->pcdata->perm_dex + ch->pcdata->mod_dex, max );
}



/*
 * Retrieve character's current constitution.
 */
int get_curr_con( CHAR_DATA *ch )
{
    int max;

    if ( IS_NPC(ch) )
	return 13;

    max = 25;

    return URANGE( 3, ch->pcdata->perm_con + ch->pcdata->mod_con, max );
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
	return 1000;

    return 2 * get_curr_dex( ch ) / 3;
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
	return 1000000;

    return str_app[get_curr_str(ch)].carry;
}


/*
 * See if a string is one of the names of an object.
 */
bool is_name( const char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_cmp( str, name ) )
	    return TRUE;
    }
}



/*
 * See if a string is one of the names of an object.
 */
bool is_name_prefix( const char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_prefix( str, name ) )
	    return TRUE;
    }
}


/*
 * Apply or remove an affect to a character.
 */
void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
    OBJ_DATA *wield;
    int mod;
    extern int making_corpse;

    mod = paf->modifier;

    if ( fAdd )
    {
	SET_BIT( ch->affected_by, paf->bitvector );
    }
    else
    {
	REMOVE_BIT( ch->affected_by, paf->bitvector );
	mod = 0 - mod;
    }

    if ( IS_NPC(ch) )
	return;

    switch ( paf->location )
    {
    default:
	bug( "Affect_modify: unknown location %d.", paf->location );
	return;

    case APPLY_NONE:						break;
    case APPLY_STR:           ch->pcdata->mod_str	+= mod;	break;
    case APPLY_DEX:           ch->pcdata->mod_dex	+= mod;	break;
    case APPLY_INT:           ch->pcdata->mod_int	+= mod;	break;
    case APPLY_WIS:           ch->pcdata->mod_wis	+= mod;	break;
    case APPLY_CON:           ch->pcdata->mod_con	+= mod;	break;
    case APPLY_SEX:           ch->sex			+= mod;	break;
    case APPLY_CLASS:						break;
    case APPLY_LEVEL:						break;
    case APPLY_AGE:						break;
    case APPLY_HEIGHT:						break;
    case APPLY_WEIGHT:						break;
    case APPLY_MANA:          ch->max_mana		+= mod;	break;
    case APPLY_HIT:           ch->max_hit		+= mod;	break;
/* really dont want objects changing moves... */
/*
    case APPLY_MOVE:          ch->max_move		+= mod;	break;
*/
    case APPLY_MOVE:                                            break;
    case APPLY_GOLD:						break;
    case APPLY_EXP:						break;
    case APPLY_AC:            ch->armor			+= mod;	break;
    case APPLY_HITROLL:       ch->hitroll		+= mod;	break;
    case APPLY_DAMROLL:       ch->damroll		+= mod;	break;
    case APPLY_SAVING_PARA:   ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_ROD:    ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_PETRI:  ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_BREATH: ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_SPELL:  ch->saving_throw		+= mod;	break;
    case APPLY_REGEN_HIT:     ch->hit_regen_bonus	+= mod;	break;
    case APPLY_REGEN_MANA:    ch->mana_regen_bonus	+= mod;	break;
    case APPLY_RESIST_FIRE:   ch->resist_fire	        += mod;	break;
    case APPLY_RESIST_COLD:   ch->resist_cold	        += mod;	break;
    case APPLY_RESIST_LIGHTNING: ch->resist_lightning   += mod;	break;
    case APPLY_RESIST_PHYSICAL:  ch->resist_physical    += mod;	break;
    case APPLY_RESIST_MAGIC:  ch->resist_magic	        += mod;	break;
    case APPLY_RESIST_DRAIN:  ch->resist_drain	        += mod;	break;
    case APPLY_SKILL_PICK:
      if ( IS_NPC( ch ) )
        break;
      ch->pcdata->learned[gsn_pick_lock] += mod;
      break;
    case APPLY_SKILL_BALANCE:
      if ( IS_NPC( ch ) )
        break;
      ch->pcdata->learned[gsn_balance] += mod;
      break;
    case APPLY_SKILL_SIXTH_SENSE:
      if ( IS_NPC( ch ) )
        break;
      ch->pcdata->learned[gsn_sixth_sense] += mod;
      break;
    case APPLY_SKILL_BASH:
      if ( IS_NPC( ch ) )
        break;
      ch->pcdata->learned[gsn_bash] += mod;
      break;
    case APPLY_SKILL_BACKSTAB:
      if ( IS_NPC( ch ) )
        break;
      ch->pcdata->learned[gsn_backstab] += mod;
      break;
    }

    /*
     * Check for weapon wielding.
     * Guard against recursion (for weapons with affects).
     *
     * If we're making a corpse, it doesn't matter.
     */
  if ( !making_corpse )
  {
    if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) != NULL
    &&   get_obj_weight(wield) > str_app[get_curr_str(ch)].wield )
    {
	static int depth;

	if ( depth == 0 )
	{
	    depth++;
	    act( "You drop $p.", ch, wield, NULL, TO_CHAR, TRUE );
	    act( "$n drops $p.", ch, wield, NULL, TO_ROOM, TRUE );
	    if ( mortal_in_main_war( ch ) )
	    {
	      obj_from_char( wield );
	      obj_to_room( wield, ch->in_room );
	    }
	    else
	    {
	      unequip_char( ch, wield );
	    }
	    depth--;
	}
    }
  }

    return;
}



/*
 * Give an affect to a char.
 */
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new;

    if ( affect_free == NULL )
    {
	paf_new		= (AFFECT_DATA *) alloc_perm( sizeof(*paf_new) );
    }
    else
    {
	paf_new		= affect_free;
	affect_free	= affect_free->next;
    }

    *paf_new		= *paf;
    paf_new->next	= ch->affected;
    ch->affected	= paf_new;

    affect_modify( ch, paf_new, TRUE );
    return;
}



/*
 * Remove an affect from a char.
 */
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    if ( ch->affected == NULL )
    {
	bug( "Affect_remove: no affect.", 0 );
	return;
    }

    affect_modify( ch, paf, FALSE );

    if ( paf == ch->affected )
    {
	ch->affected	= paf->next;
    }
    else
    {
	AFFECT_DATA *prev;

	for ( prev = ch->affected; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == paf )
	    {
		prev->next = paf->next;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    bug( "Affect_remove: cannot find paf.", 0 );
	    return;
	}
    }

    paf->next	= affect_free;
    affect_free	= paf->next;
    return;
}



/*
 * Strip all affects of a given sn.
 */
void affect_strip( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
	paf_next = paf->next;
	if ( paf->type == sn )
	    affect_remove( ch, paf );
    }

    return;
}



/*
 * Return true if a char is affected by a spell.
 */
AFFECT_DATA *is_affected( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->type == sn )
	    return paf;
    }

    return NULL;
}



/*
 * Add or enhance an affect.
 */
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_old;
    bool found;

    found = FALSE;
    for ( paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next )
    {
	if ( paf_old->type == paf->type )
	{
	    paf->duration += paf_old->duration;
	    paf->modifier += paf_old->modifier;
	    affect_remove( ch, paf_old );
	    break;
	}
    }

    affect_to_char( ch, paf );
    return;
}



/*
 * Move a char out of a room.
 */
void char_from_room( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( ch->in_room == NULL )
    {
	bug( "Char_from_room: NULL.", 0 );
	return;
    }

    if ( !IS_NPC(ch) )
	--ch->in_room->area->nplayer;

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
    &&   obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room->light > 0 )
	--ch->in_room->light;

    if ( ch == ch->in_room->people )
    {
	ch->in_room->people = ch->next_in_room;
    }
    else
    {
	CHAR_DATA *prev;

	for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
	{
	    if ( prev->next_in_room == ch )
	    {
		prev->next_in_room = ch->next_in_room;
		break;
	    }
	}

	if ( prev == NULL )
	    bug( "Char_from_room: ch not found.", 0 );
    }

    ch->in_room      = NULL;
    ch->next_in_room = NULL;
    return;
}



/*
 * Move a char into a room.
 */
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    OBJ_DATA *obj;

    if ( pRoomIndex == NULL )
    {
	bug( "Char_to_room: NULL.", 0 );
	return;
    }

    ch->in_room		= pRoomIndex;
    ch->next_in_room	= pRoomIndex->people;
    pRoomIndex->people	= ch;

    if ( !IS_NPC(ch) )
	++ch->in_room->area->nplayer;

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
    &&   obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0 )
	++ch->in_room->light;

    return;
}



void char_to_war( CHAR_DATA *ch )
{
  phase_char( ch, WAR_NUM_MAIN );

  if ( !IS_NPC( ch ) )
    ch->pcdata->war_num = WAR_NUM_MAIN;
}



/*
 * Give an obj to a char.
 */
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    ch->eq_points	+= get_equip_points( obj );
    obj->next_content	 = ch->carrying;
    ch->carrying	 = obj;
    obj->carried_by	 = ch;
    obj->in_room	 = NULL;
    obj->in_obj		 = NULL;
    ch->carry_number	+= 1;
    ch->carry_weight	+= get_obj_weight( obj );
}



/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    if ( ( ch = obj->carried_by ) == NULL )
    {
	bug( "Obj_from_char: null ch.", 0 );
	return;
    }

    ch->eq_points -= get_equip_points( obj );

    if ( obj->wear_loc != WEAR_NONE )
	unequip_char( ch, obj );

    if ( ch->carrying == obj )
    {
	ch->carrying = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = ch->carrying; prev != NULL; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( prev == NULL )
	    bug( "Obj_from_char: obj not in list.", 0 );
    }

    obj->carried_by	 = NULL;
    obj->next_content	 = NULL;
    ch->carry_number	-= 1;
    ch->carry_weight	-= get_obj_weight( obj );
    return;
}



/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac( OBJ_DATA *obj, int iWear )
{
    if (!( obj->item_type == ITEM_ARMOR || obj->item_type == ITEM_JEWELRY ) )
	return 0;

    switch ( iWear )
    {
    case WEAR_BODY:	return 3 * obj->value[0];
    case WEAR_HEAD:	return 2 * obj->value[0];
    case WEAR_LEGS:	return 2 * obj->value[0];
    case WEAR_FEET:	return     obj->value[0];
    case WEAR_HANDS:	return     obj->value[0];
    case WEAR_ARMS:	return     obj->value[0];
    case WEAR_SHIELD:	return     obj->value[0];
    case WEAR_FINGER_L:	return     obj->value[0];
    case WEAR_FINGER_R: return     obj->value[0];
    case WEAR_NECK_1:	return     obj->value[0];
    case WEAR_NECK_2:	return     obj->value[0];
    case WEAR_ABOUT:	return 2 * obj->value[0];
    case WEAR_WAIST:	return     obj->value[0];
    case WEAR_WRIST_L:	return     obj->value[0];
    case WEAR_WRIST_R:	return     obj->value[0];
    case WEAR_HOLD:	return     obj->value[0];
    }

    return 0;
}



/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->wear_loc == iWear )
	    return obj;
    }

    return NULL;
}



/*
 * Equip a char with an obj.
 */
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    AFFECT_DATA *paf;

    if ( get_eq_char( ch, iWear ) != NULL )
    {
/* lets try this: */
        bug( "Equip_char: vnum: (%d).", ch->pIndexData->vnum );
	bug( "Equip_char: already equipped (%d).", iWear );
	return;
    }


    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
    {
	/*
	 * Thanks to Morgenes for the bug fix here!
	 */
	act( "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR, TRUE );
	act( "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM, TRUE );
	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	return;
    }

    ch->armor      	-= apply_ac( obj, iWear );
    obj->wear_loc	 = iWear;
    ch->carry_number	-= 1;

    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
	affect_modify( ch, paf, TRUE );
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
	affect_modify( ch, paf, TRUE );

    if (obj->wear_fun )
	(*obj->wear_fun) ( ch, obj, PUT_ON );

    if ( obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room != NULL )
	++ch->in_room->light;

    return;
}



/*
 * Unequip a char with an obj.
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
    AFFECT_DATA *paf;

    if ( obj->wear_loc == WEAR_NONE )
    {
	bug( "Unequip_char: already unequipped.", 0 );
	return;
    }

    ch->armor		+= apply_ac( obj, obj->wear_loc );
    obj->wear_loc	 = -1;
    ch->carry_number	+= 1;

    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
	affect_modify( ch, paf, FALSE );
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
	affect_modify( ch, paf, FALSE );

    if (obj->wear_fun)
	(*obj->wear_fun) (ch, obj, TAKE_OFF);

    if ( obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room != NULL
    &&   ch->in_room->light > 0 )
	--ch->in_room->light;

    return;
}



/*
 * Move an obj out of a room.
 */
void obj_from_room( OBJ_DATA *obj, char *caller )
{
    ROOM_INDEX_DATA *in_room;
    char buf[MAX_STRING_LENGTH];

    if ( ( in_room = obj->in_room ) == NULL )
    {
	sprintf( buf, "obj_from_room: obj %s called by %s.",
	    obj->short_descr, caller );
	bug( buf, 0 );
	return;
    }

    if ( obj == in_room->contents )
    {
	in_room->contents = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = in_room->contents; prev; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    bug( "Obj_from_room: obj not found.", 0 );
	    return;
	}
    }

    if ( obj->pReset != NULL && obj->phase < NUM_PHASES )
    {
      obj->pReset->instance[obj->phase] = NULL;
      obj->pReset = NULL;
    }

    obj->in_room      = NULL;
    obj->next_content = NULL;
    return;
}



/*
 * Move an obj into a room.
 */
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
    obj->next_content		= pRoomIndex->contents;
    pRoomIndex->contents	= obj;
    obj->in_room		= pRoomIndex;
    obj->carried_by		= NULL;
    obj->in_obj			= NULL;
    return;
}



/*
 * Move an object into an object.
 */
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
    obj->next_content		= obj_to->contains;
    obj_to->contains		= obj;
    obj->in_obj			= obj_to;
    obj->in_room		= NULL;
    obj->carried_by		= NULL;

    for ( ; obj_to != NULL; obj_to = obj_to->in_obj )
    {
	if ( obj_to->carried_by != NULL )
	{
	    obj_to->carried_by->carry_weight += get_obj_weight( obj );
	}
    }

    return;
}



/*
 * Move an object out of an object.
 */
void obj_from_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;

    if ( ( obj_from = obj->in_obj ) == NULL )
    {
	bug( "Obj_from_obj: null obj_from.", 0 );
	return;
    }

    if ( obj == obj_from->contains )
    {
	obj_from->contains = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = obj_from->contains; prev; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    bug( "Obj_from_obj: obj not found.", 0 );
	    return;
	}
    }

    if ( obj->pReset != NULL && obj->phase < NUM_PHASES )
    {
      obj->pReset->instance[obj->phase] = NULL;
      obj->pReset = NULL;
    }

    obj->next_content = NULL;
    obj->in_obj       = NULL;

    for ( ; obj_from != NULL; obj_from = obj_from->in_obj )
    {
	if ( obj_from->carried_by != NULL )
	{
	    obj_from->carried_by->carry_weight -= get_obj_weight( obj );
	}
    }

    return;
}



/*
 * Extract an obj from the world.
 */
void extract_obj( OBJ_DATA *obj, char *caller )
{
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *was_in_room;

    if ( obj->in_room != NULL )
    {
	was_in_room = obj->in_room;
	obj_from_room( obj, "extract_obj" );
    }
    else if ( obj->carried_by != NULL )
    {
	was_in_room = obj->carried_by->in_room;
	obj_from_char( obj );
    }
    else if ( obj->in_obj != NULL )
    {
	obj_from_obj( obj );
    }

    while ( obj->contains )
      extract_obj( obj->contains, "extract obj recurse" );

    if ( obj->pReset != NULL && obj->phase < NUM_PHASES )
      obj->pReset->instance[obj->phase] = NULL;

    if ( object_list == obj )
    {
	object_list = obj->next;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = object_list; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == obj )
	    {
		prev->next = obj->next;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    sprintf( buf, "Extract_obj: obj %d not found. (%s)",
	      obj->pIndexData->vnum, caller );
	    bug( buf, 0 );
	    return;
	}
    }

    {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	for ( paf = obj->affected; paf != NULL; paf = paf_next )
	{
	    paf_next    = paf->next;
	    paf->next   = affect_free;
	    affect_free = paf;
	}
    }

    {
	EXTRA_DESCR_DATA *ed;
	EXTRA_DESCR_DATA *ed_next;

	for ( ed = obj->extra_descr; ed != NULL; ed = ed_next )
	{
	    ed_next		= ed->next;
	    free_string( ed->description );
	    free_string( ed->keyword     );
	    extra_descr_free	= ed;
	}
    }

    free_string( obj->name        );
    free_string( obj->description );
    free_string( obj->short_descr );
    --obj->pIndexData->count;
    obj->next	= obj_free;
    obj_free	= obj;
    return;
}



/*
 * Extract a char from the world.
 */
void extract_char( CHAR_DATA *ch, bool fPull )
{
    CHAR_DATA *wch;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    CharIterator citer;
    ObjIterator oiter;

    if ( ch->in_room == NULL )
    {
	bug( "Extract_char: NULL.", 0 );
	return;
    }

    if ( fPull )
	die_follower( ch );

    stop_fighting( ch, TRUE );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	extract_obj( obj, "extract_char" );
    }
   
    if ( fPull || IS_NPC( ch ) ) 
      char_from_room( ch );

    if ( IS_NPC(ch) )
    {
        if ( ch->pReset != NULL && ch->phase < NUM_PHASES )
           ch->pReset->instance[ch->phase] = NULL;

	--ch->pIndexData->count;
    }

    if ( ch->desc != NULL && ch->desc->original != NULL )
	do_return( ch, "" );

    for ( wch = citer.Start( ch, CharIterator::ALL );
          wch != NULL; wch = citer.Next() )
    {
	if ( wch->reply == ch )
	    wch->reply = NULL;

	if ( wch->hunting == ch )
	    wch->hunting = NULL;

	if ( wch->private_war == ch )
	{
          send_to_char(
	    "Your private war opponent has left the game.\n\r", wch );
	  if ( in_private_war( wch ) )
	    remove_from_war( wch );
	  wch->private_war = NULL;
	}
    }

    for ( obj = oiter.Start( ch, ObjIterator::ALL );
          obj != NULL; obj = oiter.Next() )
    {
	if ( obj->killer == ch )
	    obj->killer = NULL;
    }

    if ( ch == char_list )
    {
       char_list = ch->next;
    }
    else
    {
	CHAR_DATA *prev;

	for ( prev = char_list; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == ch )
	    {
		prev->next = ch->next;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    bug( "Extract_char: char not found.", 0 );
	    return;
	}
    }

    if ( ch->desc )
	ch->desc->character = NULL;
    free_char( ch );
    return;
}



/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *rch;
    CHAR_DATA *npch = NULL;
    int number;
    int count;
    CharIterator citer;

    number = number_argument( argument, arg );
    count  = 0;
    if ( !str_cmp( arg, "self" ) )
	return ch;
    for ( rch = citer.Start( ch, CharIterator::ROOM );
          rch != NULL; rch = citer.Next() )
    {
	if ( !can_see( ch, rch ) )
	    continue;
	if ( IS_NPC( rch ) )
	{
	  if ( !IS_SET( rch->act, ACT_ILLUSION )
	  && !IS_SET( rch->act, ACT_TERMINATOR ) )
	  {
	    if ( !is_name( arg, rch->name ) )
	      continue;
	  }
	  else if ( !is_name_prefix( arg, rch->name ) )
	    continue;
	}
	else if ( str_prefix( arg, rch->name ) )
	    continue;

        /* Always match players. */
        if ( number == 1 && !IS_NPC( rch ) )
          return rch;

        /* Check all characters before matching NPC */
	if ( ++count == number ) {
            if ( IS_NPC( rch ) )
            {
              npch = rch;
              continue;
            }
	    return rch;
        }
    }

    return npch;
}




/*
 * Find a char in the world.  This sees all phases.
 */
CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *wch;
    int number;
    int count;
    CharIterator citer;

    if ( ( wch = get_char_room( ch, argument ) ) != NULL )
	return wch;

    number = number_argument( argument, arg );
    count  = 0;
    for ( wch = citer.Start( ch, CharIterator::ALL );
          wch != NULL ; wch = citer.Next() )
    {
	if ( !can_see( ch, wch ) || !is_name( arg, wch->name ) )
	    continue;
	if ( ++count == number )
	    return wch;
    }

    return NULL;
}


/*
 * Find a char in the player's phase.
 */
CHAR_DATA *get_char_phase( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *wch;
    int number;
    int count;
    CharIterator citer;

    if ( ( wch = get_char_room( ch, argument ) ) != NULL )
	return wch;

    number = number_argument( argument, arg );
    count  = 0;
    for ( wch = citer.Start( ch, CharIterator::WORLD );
          wch != NULL ; wch = citer.Next() )
    {
	if ( !can_see( ch, wch ) || !is_name( arg, wch->name ) )
	    continue;
	if ( ++count == number )
	    return wch;
    }

    return NULL;
}



/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;
    ObjIterator oiter;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = oiter.StartList( ch, list );
          obj != NULL; obj = oiter.Next( ) )
    {
	if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->wear_loc == WEAR_NONE
	&&   can_see_obj( ch, obj )
	&&   is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->wear_loc != WEAR_NONE
	&&   can_see_obj( ch, obj )
	&&   is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    obj = get_obj_list( ch, argument, ch->in_room->contents );
    if ( obj != NULL )
	return obj;

    if ( ( obj = get_obj_carry( ch, argument ) ) != NULL )
	return obj;

    if ( ( obj = get_obj_wear( ch, argument ) ) != NULL )
	return obj;

    return NULL;
}



/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;
    ObjIterator oiter;

    if ( ( obj = get_obj_here( ch, argument ) ) != NULL )
	return obj;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = oiter.Start( ch, ObjIterator::WORLD );
          obj != NULL; obj = obj->next )
    {
	if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Create a 'money' obj.
 */
OBJ_DATA *create_money( int amount, unsigned int phase )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;

    if ( amount < 0 )
    {
	bug( "Create_money: zero or negative money %d.", amount );
	amount = 1;
    }

    if ( amount == 0 )
	amount = 1;

    if ( amount == 1 )
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE ), phase );
    }
    else
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), phase );
	sprintf( buf, obj->short_descr, amount );
	free_string( obj->short_descr );
	obj->short_descr	= str_dup( buf );
	obj->value[0]		= amount;
    }

    return obj;
}



/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight( OBJ_DATA *obj )
{
    int weight;

    weight = obj->weight;
    for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
	weight += get_obj_weight( obj );

    return weight;
}



/*
 * True if room is dark.
 */
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
    if ( pRoomIndex->light > 0 )
	return FALSE;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_DARK) )
	return TRUE;

    if ( pRoomIndex->sector_type == SECT_INSIDE
    ||   pRoomIndex->sector_type == SECT_CITY )
	return FALSE;

    if ( weather_info.sunlight == SUN_SET
    ||   weather_info.sunlight == SUN_DARK )
	return TRUE;

    return FALSE;
}


/*
 * True if char can see victim.
 */
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
    ROOM_INDEX_DATA *proom;

    if ( ch == victim )
	return TRUE;

    proom = ch->in_room;

    if ( proom->smoke > 0
    && mortal_in_main_war( ch )
    && ch->char_class != CLASS_NINJA
    && !IS_SET( ch->act, PLR_HOLYLIGHT ) )
      return FALSE;

    if ( !IS_NPC(victim)
	&& get_trust( ch ) < (IS_NPC( victim )
	  ? 0
	  : victim->pcdata->wizinvis_level))

/*
    &&   IS_SET(victim->act, PLR_WIZINVIS)
    &&   get_trust( ch ) < get_trust( victim ) )
*/
	return FALSE;
 	

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;

    if ( ch->char_class == CLASS_DIVINER )
        return TRUE;

    if ( IS_AFFECTED( victim, AFF_IMPROVED_INVIS ) )
	return FALSE;

    if ( IS_AFFECTED(ch, AFF_BLIND) )
	return FALSE;

    if ( room_is_dark( ch->in_room )
    && !IS_AFFECTED(ch, AFF_INFRARED)
    && ch->char_class != CLASS_MONK
    && mortal_in_main_war( ch ) )
	return FALSE;

    if ( IS_AFFECTED(victim, AFF_INVISIBLE)
    &&   !IS_AFFECTED(ch, AFF_DETECT_INVIS)
    &&  mortal_in_main_war( ch ) )
	return FALSE;

    if ( IS_AFFECTED(victim, AFF_HIDE)
    &&	!IS_AFFECTED(ch, AFF_DETECT_HIDDEN)
    &&  !IS_AFFECTED(victim, AFF_FAERIE_FIRE)
    &&	victim->fighting == NULL
    &&	(number_percent() < (IS_NPC(victim)
        ? 50 : victim->pcdata->learned[gsn_hide] ) )
    && mortal_in_main_war( ch ) )
	return FALSE;

    return TRUE;
}



/*
 * True if char can see obj.
 */
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;

/*
    if ( obj->item_type == ITEM_POTION )
	return TRUE;
*/

    if ( obj->wear_loc == WEAR_HOLD && obj->carried_by == ch )
	return TRUE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) )
	return FALSE;

    if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
	return TRUE;

    if ( room_is_dark( ch->in_room )
    && !IS_AFFECTED(ch, AFF_INFRARED) 
    && ch->char_class != CLASS_MONK
    && mortal_in_main_war( ch ) )
	return FALSE;

    if ( IS_SET(obj->extra_flags, ITEM_INVIS)
    &&   !IS_AFFECTED(ch, AFF_DETECT_INVIS) )
	return FALSE;

    return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_SET(obj->extra_flags, ITEM_NODROP) )
	return TRUE;

    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
	return TRUE;

    return FALSE;
}



/*
 * Return ascii name of an item type.
 */
char *item_type_name( int item_type, int vnum )
{
    switch ( item_type )
    {
    case ITEM_LIGHT:		return "light";
    case ITEM_SCROLL:		return "scroll";
    case ITEM_WAND:		return "wand";
    case ITEM_STAFF:		return "staff";
    case ITEM_WEAPON:		return "weapon";
    case ITEM_GRENADE:		return "grenade";
    case ITEM_TREASURE:		return "treasure";
    case ITEM_ARMOR:		return "armor";
    case ITEM_POTION:		return "potion";
    case ITEM_UNKNOWN1:		return "unknown 1";
    case ITEM_FURNITURE:	return "furniture";
    case ITEM_TRASH:		return "trash";
    case ITEM_CONTAINER:	return "container";
    case ITEM_DRINK_CON:	return "drink container";
    case ITEM_KEY:		return "key";
    case ITEM_FOOD:		return "food";
    case ITEM_MONEY:		return "money";
    case ITEM_BOAT:		return "boat";
    case ITEM_CORPSE_NPC:	return "npc corpse";
    case ITEM_CORPSE_PC:	return "pc corpse";
    case ITEM_FOUNTAIN:		return "fountain";
    case ITEM_PILL:		return "pill";
    case ITEM_JEWELRY:		return "jewelry";
    case ITEM_QUIVER:           return "quiver";
    case ITEM_TRAP:             return "trap";
    }

    bug( "Item_type_name: unknown type on Vnum: %d.", vnum );
    return "(unknown)";
}




/*
 * Return ascii name of an affect location.
 */
char *affect_loc_name( int location )
{
  static char str_unknown[10] = "(unknown)";

    switch ( location )
    {
    case APPLY_NONE:		return str_none;
    case APPLY_STR:		return "strength";
    case APPLY_DEX:		return "dexterity";
    case APPLY_INT:		return "intelligence";
    case APPLY_WIS:		return "wisdom";
    case APPLY_CON:		return "constitution";
    case APPLY_SEX:		return "sex";
    case APPLY_CLASS:		return "class";
    case APPLY_LEVEL:		return "level";
    case APPLY_AGE:		return "age";
    case APPLY_MANA:		return "mana";
    case APPLY_HIT:		return "hp";
    case APPLY_MOVE:		return "moves";
    case APPLY_GOLD:		return "gold";
    case APPLY_EXP:		return "experience";
    case APPLY_AC:		return "armor class";
    case APPLY_HITROLL:		return "hit roll";
    case APPLY_DAMROLL:		return "damage roll";
    case APPLY_SAVING_PARA:	return "save vs paralysis";
    case APPLY_SAVING_ROD:	return "save vs rod";
    case APPLY_SAVING_PETRI:	return "save vs petrification";
    case APPLY_SAVING_BREATH:	return "save vs breath";
    case APPLY_SAVING_SPELL:	return "save vs spell";
    case APPLY_REGEN_HIT:	return "hit regen";
    case APPLY_REGEN_MANA:	return "mana regen";
    case APPLY_RESIST_FIRE:	return "resist fire";
    case APPLY_RESIST_COLD:	return "resist cold";
    case APPLY_RESIST_LIGHTNING: return "resist lightning";
    case APPLY_RESIST_PHYSICAL:	return "resist physical";
    case APPLY_RESIST_MAGIC:	return "resist magic";
    case APPLY_RESIST_DRAIN:	return "resist drain";
    case APPLY_SKILL_PICK:	return "pick lock";
    case APPLY_SKILL_BALANCE:	return "balance";
    case APPLY_SKILL_SIXTH_SENSE: return "sixth sense";
    case APPLY_SKILL_BASH:	return "bash";
    case APPLY_SKILL_BACKSTAB:	return "backstab";
    }

    bug( "Affect_location_name: unknown location %d.", location );
    return str_unknown;
}



/*
 * Return ascii name of an affect bit vector.
 */
char *affect_bit_name( int vector )
{
    static char buf[512];

    buf[0] = '\0';
    if ( vector & AFF_BLIND         ) strcat( buf, " blind"         );
    if ( vector & AFF_INVISIBLE     ) strcat( buf, " invisible"     );
    if ( vector & AFF_DETECT_EVIL   ) strcat( buf, " detect_evil"   );
    if ( vector & AFF_DETECT_INVIS  ) strcat( buf, " detect_invis"  );
    if ( vector & AFF_DETECT_MAGIC  ) strcat( buf, " detect_magic"  );
    if ( vector & AFF_DETECT_HIDDEN ) strcat( buf, " detect_hidden" );
/* removing unused AFF_HOLD and replacing with AFF_NO_SUMMON */
/*
    if ( vector & AFF_HOLD          ) strcat( buf, " hold"          );
*/
    if ( vector & AFF_NO_SUMMON     ) strcat( buf, " no_summon"     );
    if ( vector & AFF_SANCTUARY     ) strcat( buf, " sanctuary"     );
    if ( vector & AFF_FAERIE_FIRE   ) strcat( buf, " faerie_fire"   );
    if ( vector & AFF_INFRARED      ) strcat( buf, " infrared"      );
    if ( vector & AFF_CURSE         ) strcat( buf, " curse"         );
/* replaced AFF_FLAMING with AFF_FIRESHIELD */
    if ( vector & AFF_FIRESHIELD    ) strcat( buf, " fireshield"    );
    if ( vector & AFF_POISON        ) strcat( buf, " poison"        );
    if ( vector & AFF_PROTECT       ) strcat( buf, " protect"       );
    if ( vector & AFF_PARALYSIS     ) strcat( buf, " paralysis"     );
    if ( vector & AFF_SLEEP         ) strcat( buf, " sleep"         );
    if ( vector & AFF_SNEAK         ) strcat( buf, " sneak"         );
    if ( vector & AFF_HIDE          ) strcat( buf, " hide"          );
    if ( vector & AFF_CHARM         ) strcat( buf, " charm"         );
    if ( vector & AFF_FLYING        ) strcat( buf, " flying"        );
    if ( vector & AFF_PASS_DOOR     ) strcat( buf, " pass_door"     );
    if ( vector & AFF_IMPROVED_INVIS) strcat( buf, " improved_invis");
    if ( vector & AFF_BLACKMANTLE   ) strcat( buf, " blackmantle"   );
    return ( buf[0] != '\0' ) ? buf+1 : str_none;
}



/*
 * Return ascii name of extra flags vector.
 */
char *extra_bit_name( int extra_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( extra_flags & ITEM_GLOW         ) strcat( buf, " glow"         );
    if ( extra_flags & ITEM_HUM          ) strcat( buf, " hum"          );
    if ( extra_flags & ITEM_DARK         ) strcat( buf, " dark"         );
    if ( extra_flags & ITEM_RENAME       ) strcat( buf, " rename"       );
    if ( extra_flags & ITEM_EVIL         ) strcat( buf, " evil"         );
    if ( extra_flags & ITEM_INVIS        ) strcat( buf, " invis"        );
    if ( extra_flags & ITEM_MAGIC        ) strcat( buf, " magic"        );
    if ( extra_flags & ITEM_NODROP       ) strcat( buf, " nodrop"       );
    if ( extra_flags & ITEM_BLESS        ) strcat( buf, " bless"        );
    if ( extra_flags & ITEM_ANTI_GOOD    ) strcat( buf, " anti-good"    );
    if ( extra_flags & ITEM_ANTI_EVIL    ) strcat( buf, " anti-evil"    );
    if ( extra_flags & ITEM_ANTI_NEUTRAL ) strcat( buf, " anti-neutral" );
    if ( extra_flags & ITEM_NOREMOVE     ) strcat( buf, " noremove"     );
    if ( extra_flags & ITEM_INVENTORY    ) strcat( buf, " inventory"    );
    return ( buf[0] != '\0' ) ? buf+1 : str_none;
}


void heal_character( CHAR_DATA *ch, int amount )
{
    if ( amount < 0 )
	amount = ch->max_hit - ch->hit;

    amount = UMIN( amount, ch->max_hit - ch->hit );

    if ( mortal_in_main_war( ch ) )
	ch->pcdata->potential_dr += amount;

    ch->hit += amount;

    return;
}


int get_affect_points( AFFECT_DATA *paf )
{
  switch( paf->location )
  {
    case APPLY_STR:
    case APPLY_DEX:
    case APPLY_INT:
    case APPLY_WIS:
    case APPLY_CON:
    case APPLY_HITROLL:
    case APPLY_DAMROLL:
    default:
      if ( paf->modifier < 0 )
        return 0;
      else
        return paf->modifier;
      break;

    case APPLY_SEX:
    case APPLY_CLASS:
    case APPLY_LEVEL:
    case APPLY_AGE:
    case APPLY_HEIGHT:
    case APPLY_WEIGHT:
    case APPLY_MOVE:
    case APPLY_GOLD:
    case APPLY_EXP:
      return 0;
      break;

    case APPLY_MANA:
    case APPLY_HIT:
    case APPLY_REGEN_HIT:
    case APPLY_REGEN_MANA:
    case APPLY_RESIST_FIRE:
    case APPLY_RESIST_COLD:
    case APPLY_RESIST_LIGHTNING:
    case APPLY_RESIST_PHYSICAL:
    case APPLY_RESIST_MAGIC:
    case APPLY_RESIST_DRAIN:
    case APPLY_SKILL_PICK:
    case APPLY_SKILL_BALANCE:
    case APPLY_SKILL_SIXTH_SENSE:
    case APPLY_SKILL_BASH:
    case APPLY_SKILL_BACKSTAB:
      if ( paf->modifier <= 0 )
        return 0;
      else
        return ( ( paf->modifier + 4 ) / 5 );
      break;

    case APPLY_AC:
      if ( paf->modifier >= 0 )
        return 0;
      else
        return ( ( ( -1 * paf->modifier ) + 4 ) / 5 );
      break;

    case APPLY_SAVING_PARA:
    case APPLY_SAVING_ROD:
    case APPLY_SAVING_PETRI:
    case APPLY_SAVING_BREATH:
    case APPLY_SAVING_SPELL:
      if ( paf->modifier >= 0 )
        return 0;
      else
        return ( -1 * paf->modifier );
      break;
  }

  return 0;
}


int get_equip_points( OBJ_DATA *obj )
{
  AFFECT_DATA *paf;
  int points = 0;

  if ( obj->item_type == ITEM_SCROLL
  || obj->item_type == ITEM_WAND
  || obj->item_type == ITEM_STAFF )
    points += 5;

  if ( obj->item_type == ITEM_POTION
  || obj->item_type == ITEM_PILL
  || obj->item_type == ITEM_DRINK_CON )
    points += 10;

  for ( paf = obj->affected; paf != NULL; paf = paf->next )
    points += get_affect_points( paf );

  if ( obj->pIndexData )
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
      points += get_affect_points( paf );

  return points;
}


/* phase_obj is recursive to avoid complicated logic in contents */
void phase_obj( OBJ_DATA *obj, unsigned int phase )
{
  OBJ_DATA *in_obj;

  if ( phase > NUM_PHASES )
  {
    phase = 0;
  }

  for ( in_obj = obj->contains; in_obj != NULL; in_obj = in_obj->next_content )
    phase_obj( in_obj, phase );

  obj->phase = phase;
}


void phase_char( CHAR_DATA *ch, unsigned int phase )
{
  CHAR_DATA *wch;
  OBJ_DATA *obj;
  ObjIterator oiter;
  CharIterator citer;

  if ( phase > NUM_PHASES )
  {
    phase = 0;
  }

  for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    phase_obj( obj, phase ); 

  ch->phase = phase;

  /* Do most of extract_char */
  die_follower( ch );

  stop_fighting( ch, TRUE );


  for ( wch = citer.Start( ch, CharIterator::ALL );
        wch != NULL; wch = citer.Next() )
  {
    if ( wch->hunting == ch )
      wch->hunting = NULL;

    if ( wch->private_war == ch )
    {
       send_to_char(
         "Your private war opponent has left the phase.\n\r", wch );
       if ( in_private_war( wch ) )
         remove_from_war( wch );
       wch->private_war = NULL;
    }
  }

  for ( obj = oiter.Start( ch, ObjIterator::ALL );
        obj != NULL; obj = oiter.Next() )
  {
    if ( obj->killer == ch )
      obj->killer = NULL;
  }

  ch->phase = phase;
}






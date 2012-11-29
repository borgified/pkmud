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
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <crypt.h>

#include "merc.h"
#include "global.h"
#include "cmod.h"



char const where_name[][11] =
{
	"<light>   ",
	"<finger>  ",
	"<finger>  ",
	"<neck>    ",
	"<neck>    ",
	"<body>    ",
	"<head>    ",
	"<legs>    ",
	"<feet>    ",
	"<hands>   ",
	"<arms>    ",
	"<shield>  ",
	"<body>    ",
	"<waist>   ",
	"<wrist>   ",
	"<wrist>   ",
	"<wielded> ",
	"<held>    "
};


/* Size of slot array is 11 */
struct wear_loc_names  wear_loc_table [] =
{
  { "take", ITEM_TAKE },
  { "finger", ITEM_WEAR_FINGER },
  { "ring", ITEM_WEAR_FINGER },
  { "neck", ITEM_WEAR_NECK },
  { "body", ITEM_WEAR_BODY },
  { "head", ITEM_WEAR_HEAD },
  { "legs", ITEM_WEAR_LEGS },
  { "feet", ITEM_WEAR_FEET },
  { "boots", ITEM_WEAR_FEET },
  { "hands", ITEM_WEAR_HANDS },
  { "gloves", ITEM_WEAR_HANDS },
  { "arms", ITEM_WEAR_ARMS },
  { "sleeves", ITEM_WEAR_ARMS },
  { "shield", ITEM_WEAR_SHIELD },
  { "about", ITEM_WEAR_ABOUT },
  { "waist", ITEM_WEAR_WAIST },
  { "belt", ITEM_WEAR_WAIST },
  { "wrist", ITEM_WEAR_WRIST },
  { "bracelet", ITEM_WEAR_WRIST },
  { "wield", ITEM_WIELD },
  { "weapon", ITEM_WIELD },
  { "hold", ITEM_HOLD },
  { "last", 0 }
};



/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				    bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );
bool	stringsearch		args( ( char *p, char *a ) );


char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';
    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   strcat( buf, "(invis) "     );

    if ( fShort )
    {
	if ( obj->short_descr != NULL )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( obj->description != NULL )
	    strcat( buf, obj->description );
    }

/*
    if( CAN_WEAR( obj, ITEM_TAKE ) )
    {
	int percent;

	percent = obj->damage == -1 ? -1 : ((float)obj->damage/1500)*100;

	      if(percent<0)	strcat(buf, " (indestructable)"	);
	else  if(percent<5)	strcat(buf, " (falling apart)"	);
	else  if(percent<15)	strcat(buf, " (terrible)"	);
	else  if(percent<35)	strcat(buf, " (poor)"		);
	else  if(percent<50)	strcat(buf, " (fair)"		);
	else  if(percent<70)	strcat(buf, " (very good)"	);
	else  if(percent<95)	strcat(buf, " (excellent)"	);
	else  if(percent<101)	strcat(buf, " (brand new)"	);
	else			strcat(buf, " (artifact)"	);
    }
*/

    if( obj->item_type == ITEM_QUIVER )
    {
	char buf2[MAX_STRING_LENGTH];

	sprintf(buf2," (%d arrows left)",obj->value[2] );
	strcat(buf,buf2);
    }

    if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && IS_OBJ_STAT(obj, ITEM_EVIL)   )   strcat( buf, " ...It glows red!"  );
    if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)
         && IS_OBJ_STAT(obj, ITEM_MAGIC)  )   strcat( buf, " ...It glows blue!"   );
    if ( IS_OBJ_STAT(obj, ITEM_GLOW)      )   strcat( buf, " ...It has a soft glowing aura!"   );
    if ( IS_OBJ_STAT(obj, ITEM_HUM)       )   strcat( buf, " ...It emits a faint humming sound!"   );

    return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if ( ch->desc == NULL )
	return;

    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	count++;
    prgpstrShow	= (char **) alloc_mem( count * sizeof(char *) );
    prgnShow    = (int *) alloc_mem( count * sizeof(int)    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    { 
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		send_to_char( buf, ch );
	    }
	    else
	    {
		send_to_char( "     ", ch );
	    }
	}
	send_to_char( prgpstrShow[iShow], ch );
	send_to_char( "\n\r", ch );
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    send_to_char( "     ", ch );
	send_to_char( "Nothing.\n\r", ch );
    }

    /*
     * Clean up.
     */
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    int temp;

    buf[0] = '\0';

    if ( IS_AFFECTED( victim, AFF_HIDE ) )
    {
	send_to_char("You sense a hidden life form.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "(invis) "      );
    if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) strcat( buf, "(Translucent) ");
    if ( IS_EVIL(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) strcat( buf, "(Red Aura) "   );
    if ( IS_AFFECTED(victim, AFF_BLACKMANTLE) )	strcat( buf, "(Black Aura) " );

    if ( !can_communicate( victim, FALSE ) )
	strcat( buf, "(SNOOP) "   );

    if ( mortal_in_main_war( victim ) ) 	strcat( buf, "(IN_WAR) "   );

    if ( victim->position == POS_STANDING && victim->long_descr[0] != '\0' )
    {
	strcat( buf, victim->long_descr );
    }
    else
    {
    strcat( buf, PERS( victim, ch ) );

    if ( !IS_NPC(victim) && !IS_SET(ch->act, PLR_BRIEF) )
	strcat( buf, victim->pcdata->title );

/* fix for new bash */
    temp = victim->position;
    if (victim->fighting != NULL)
	temp = POS_FIGHTING;

    switch ( temp )
    {
    case POS_DEAD:     strcat( buf, " is dead." );              break;
    case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break;
    case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
    case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
    case POS_SLEEPING: strcat( buf, " is sleeping here." );      break;
    case POS_RESTING:  strcat( buf, " is resting here." );       break;
    case POS_STANDING: strcat( buf, " is here." );               break;
    case POS_FIGHTING:
	strcat( buf, " is here, fighting " );
	if ( victim->fighting == NULL )
	    strcat( buf, "thin air??" );
	else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	else if ( victim->in_room == victim->fighting->in_room )
	{
	    strcat( buf, PERS( victim->fighting, ch ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "somone who left??" );
	break;
    }
		strcat( buf, "\n\r" );
    }

    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
    {
	strcat( buf, PERS(victim, ch ) );
	strcat( buf, " is surrounded by a pink outline.\n\r" );
    }

    if ( IS_AFFECTED(victim, AFF_SANCTUARY)
    || ( is_affected( victim, gsn_illusory_auras ) && !number_bits( 2 ) ) )
    {
	strcat( buf, PERS( victim, ch ) );
	strcat( buf, " glows with a bright light.\n\r" );
    }

    if ( IS_AFFECTED(victim, AFF_FIRESHIELD)
    || ( is_affected( victim, gsn_illusory_auras ) && !number_bits( 2 ) ) )
    {
	strcat( buf, PERS( victim, ch ) );
	strcat( buf, " is surrounded by a flaming sphere.\n\r" );

    }

    if ( IS_AFFECTED(victim, AFF_INVULNERABILITY )
    || ( is_affected( victim, gsn_illusory_auras ) && !number_bits( 2 ) ) )
    {
	strcat( buf, PERS( victim, ch ) );
	strcat( buf, " is surrounded by a creamy white sphere.\n\r" );
    }

    if ( is_affected(victim, gsn_weapon_immunity )
    || ( is_affected( victim, gsn_illusory_auras ) && !number_bits( 2 ) ) )
    {
	strcat( buf, PERS( victim, ch ) );
	strcat( buf, " is surrounded by a metallic aura.\n\r" );
    }

    if ( is_affected(victim, gsn_void_aura )
    || ( is_affected( victim, gsn_illusory_auras ) && !number_bits( 2 ) ) )
    {
	strcat( buf, PERS( victim, ch ) );
	strcat( buf, " is surrounded by a shimmering aura of darkness.\n\r" );
    }

    buf[0] = UPPER(buf[0]);           
    
    if (!IS_NPC(ch) && IS_SET(victim->act, PLR_WIZINVIS) ) {
       buf[0] = '\0';
    }
    
    send_to_char( buf, ch );       
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;

    if (ch == NULL || victim == NULL)
	return;

    if ( !mortal_in_main_war( ch )
    && get_trust( ch ) < LEVEL_IMMORTAL
    && ( mortal_in_main_war( victim ) || !IS_NPC( victim ) ) )
    {
      send_to_char("Dead people can't stand to look at live people silly.\n\r",
        ch );
      return;
    }

    if ( can_see( victim, ch ) && ch->char_class != CLASS_THIEF
			       && ch->level < 36 )
    {
	act( "$n looks at you.", ch, NULL, victim, TO_VICT, FALSE );
	act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT, FALSE );
    }

    if ( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
    }
    else
    {
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR, TRUE );
    }

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    strcpy( buf, PERS(victim, ch) );

         if ( percent >= 100 ) strcat( buf, " is in perfect health.\n\r"  );
    else if ( percent >=  90 ) strcat( buf, " is slightly scratched.\n\r" );
    else if ( percent >=  80 ) strcat( buf, " has a few bruises.\n\r"     );
    else if ( percent >=  70 ) strcat( buf, " has some cuts.\n\r"         );
    else if ( percent >=  60 ) strcat( buf, " has small wounds.\n\r"      );
    else if ( percent >=  50 ) strcat( buf, " has large wounds.\n\r"      );
    else if ( percent >=  40 ) strcat( buf, " has large wounds.\n\r"      );
    else if ( percent >=  30 ) strcat( buf, " has big nasty wounds.\n\r"  );
    else if ( percent >=  20 ) strcat( buf, " has big nasty wounds.\n\r"  );
    else if ( percent >=  10 ) strcat( buf, " is pretty hurt.\n\r"        );
    else if ( percent >=   5 ) strcat( buf, " is in awful condition.\n\r" );
    else                       strcat( buf, " is bleeding to death. \n\r" );

    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );

  if ( in_main_war( ch ) )
  {
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		act( "$N is using:", ch, NULL, victim, TO_CHAR, TRUE );
		found = TRUE;
	    }
	    send_to_char( where_name[iWear], ch );
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
    }

    if ( victim != ch
    &&   !IS_NPC(ch)
    &&   number_percent( ) < ch->pcdata->learned[gsn_peek] )
    {
	send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }
  }

    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;

	if ( !IS_NPC(rch)
	&&   IS_SET(rch->act, PLR_WIZINVIS)
	&&   get_trust( ch ) < get_trust( rch ) )
	    continue;

	if ( IS_NPC(ch)
	|| can_see( ch, rch )
	|| !mortal_in_main_war( ch ) )
	{
	    show_char_to_char_0( rch, ch );
	}
	
    }

    return;
} 



bool check_blind( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;

    if ( IS_AFFECTED(ch, AFF_BLIND) )
    {
	send_to_char( "You can't see a thing!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}


void do_rlog( CHAR_DATA *ch, char *argument )
 {
  FILE *fp;
/*
	char *word;
	int iTimes = 0;
*/
  char arg1[MAX_STRING_LENGTH];

  send_to_char( "Implement in future.\n\r", ch );
  return;

  if( ( fp = fopen( "../PKlog/1000.log", "r" ) ) == NULL )
  {
    send_to_char( "Failed to open log file.\n\r", ch );
    return;
  }

  one_argument( argument, arg1 );

  if( arg1[0] == '\0' ) 
  {
    send_to_char("Invalid perimeter. Syntax: rlog <word to grep for>\n\r", ch );
    return;
  }

/*	while( 1 ) {

		word = fread_word( fp );

		if ( word == '\0' || iTimes == 25 ) {
			
			send_to_char( "Reached end of possible loop.\n\r", ch );
			fclose( fp );
			break;
		}
		
		if ( stringsearch( word, arg1 ) == TRUE ) {

			strcat( word, "\n\r" );
			send_to_char( word, ch );
			iTimes++;
			
		}
	} */

  system( "grep trans ../PKlog/1000.log" );
  return;
}
/* This is the brute force algorithm, won't work with want i want
    so im creating my own below. 

int stringsearch( char *p, char *a ) {

	int i, j, M = strlen(p), N = strlen(a);
	for (i = 0, j = 0; j < M && i < N; i++, j++ ) 
		if ( a[i] != p[j] ) { i -= j-1; j = -1; }
	if( j == M ) return 0; else return 1;
}
*/
bool stringsearch( char *p, char *a ) {

	char *txt;
	int x = strlen(p);
	int n;

	txt = a;

	for( n = 0; n <= x; n++ ) {

		if( *p == *a ) {

			p++; a++;
			if( *a == '\0' )
				return TRUE;
			continue;
		}
		
		a = txt;
		p++;

	}
	return FALSE;
}


void do_look( CHAR_DATA *ch, char *argument )
{
  char buf  [MAX_STRING_LENGTH];
  char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  EXIT_DATA *pexit;
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  char *pdesc;
  int door;
  ROOM_INDEX_DATA *was_in_room;
  ROOM_INDEX_DATA *look_far_room;

  was_in_room = ch->in_room;

  if ( ch->desc == NULL )
    return;

  if ( ch->position < POS_SLEEPING )
  {
    send_to_char( "You can't see anything but stars!\n\r", ch );
    return;
  }

  if ( ch->position == POS_SLEEPING )
  {
    send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
    return;
  }

  if ( !check_blind( ch ) )
    return;

  if ( !IS_NPC(ch)
  &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
  &&   mortal_in_main_war( ch )
  &&   ch->char_class != CLASS_MONK
  &&   room_is_dark( ch->in_room ) )
  {
    send_to_char( "It is pitch black ...\n\r", ch );
    show_char_to_char( ch->in_room->people, ch );
    return;
  }

  if ( ch->in_room->smoke > 0
  && ch->char_class != CLASS_NINJA
  && mortal_in_main_war( ch )
  && !IS_SET( ch->act, PLR_HOLYLIGHT ) )
  {
    send_to_char( "The room is covered in smoke!\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
  {
    /* 'look' or 'look auto' */
    sprintf( buf, "#R%s#0\n\r", ch->in_room->name );
    send_to_char( buf, ch );

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
      do_exits( ch, str_auto );

    if ( arg1[0] == '\0'
    || ( !IS_NPC(ch) && !IS_SET(ch->act, PLR_BRIEF) ) )
      send_to_char( ch->in_room->description, ch );

    show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
    show_char_to_char( ch->in_room->people,   ch );

    /* track */
    if (IS_NPC(ch) || ch->pcdata->learned[gsn_track] > 5)
    {
      char buftrack  [MAX_STRING_LENGTH];

      /* profi check */
      if (IS_NPC(ch)
      || number_percent() <
        ( ch->pcdata->learned[gsn_track]
	  + (IS_AFFECTED( ch, AFF_SNEAK ) ? -50 : 0 ) ) )
        ch->in_room->last_exit_dir = -1;


      if (ch->in_room->last_exit_dir != -1
      && mortal_in_main_war( ch ) )
      {
        sprintf( buftrack,
          "You detect the tracks of %s leading %s.\n\r",
          ch->in_room->last_char, dir_name[ ch->in_room->last_exit_dir ] );
        send_to_char( buftrack, ch );

        /* clear tracks */
        ch->in_room->last_exit_dir = -1;
      }
    }

    return;
  }

    if ( ( !str_cmp( arg1, "i" ) || !str_cmp( arg1, "in" ) )
    && arg2[0] != '\0' )
    {
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
		break;
	    }

	    sprintf( buf, "It's %s full of a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about"     : "more than",
		liq_table[obj->value[2]].liq_color
		);

	    send_to_char( buf, ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	    act( "$p contains:", ch, obj, NULL, TO_CHAR, TRUE );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) != NULL )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if ( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    return;
	}
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj )
	|| !mortal_in_main_war( ch ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if ( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    return;
	}
    }

    pdesc = get_extra_descr( arg1, ch->in_room->extra_descr );
    if ( pdesc != NULL )
    {
	send_to_char( pdesc, ch );
	return;
    }

    if (!str_cmp( arg1, "far" ) || !str_cmp( arg1, "f" ) )
    {
         if ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) ) door = 0;
    else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east"  ) ) door = 1;
    else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) ) door = 2;
    else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west"  ) ) door = 3;
    else if ( !str_cmp( arg2, "u" ) || !str_cmp( arg2, "up"    ) ) door = 4;
    else if ( !str_cmp( arg2, "d" ) || !str_cmp( arg2, "down"  ) ) door = 5;
    else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    if (IS_NPC(ch) ||ch->pcdata->learned[gsn_peek] > 10 || IS_AFFECTED( ch, AFF_FARSEE ) )
    {
    if ((pexit = was_in_room->exit[door]) == NULL
	|| (look_far_room = pexit->to_room) == NULL )
    {
	send_to_char("You don't see much off in that direction.\n\r", ch );
	return;
    }
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    act( "You can't seem to see through the $d.", ch, NULL, pexit->keyword, TO_CHAR, TRUE );
	    return;
	}

	do_look( ch, arg2 );
	char_from_room( ch );
	char_to_room( ch, look_far_room );
	do_look( ch, str_empty );
	char_from_room( ch );
	char_to_room( ch, was_in_room );
	return;
    }
    else
    {
	send_to_char("Your eyesight really isn't that good.\n\r", ch );
	return;
    }
    }

         if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
    else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    /* 'look direction' */
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );

    if ( pexit->keyword    != NULL
    &&   pexit->keyword[0] != '\0'
    &&   pexit->keyword[0] != ' ' )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR, TRUE );
	}
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR, TRUE );
	}
    }

    return;
}



void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

    do_look( ch, arg );

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	switch ( obj->item_type )
	{
	default:
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    send_to_char( "When you look inside, you see:\n\r", ch );
	    sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	}
    }

    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;

    buf[0] = '\0';
    fAuto  = !str_cmp( argument, str_auto );

    if ( !check_blind( ch ) )
	return;

    strcpy( buf, fAuto ? "#E[Exits:" : "Obvious exits:\n\r" );

    found = FALSE;
    for ( door = 0; door <= 5; door++ )
    {
	if ( ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, " " );
		strcat( buf, dir_name[door] );
	    }
	    else
	    {
		sprintf( buf + strlen(buf), "%-5s - %s\n\r",
		    capitalize( dir_name[door] ),
		    room_is_dark( pexit->to_room )
			?  "Too dark to tell"
			: pexit->to_room->name
		    );
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
	strcat( buf, "]#0\n\r" );

    send_to_char( buf, ch );
    return;
}



void do_score( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int temp;

    sprintf( buf,
	"You are %s%s, level %d, %d years old (%d hours).\n\r",
	ch->name,
	IS_NPC(ch) ? "" : ch->pcdata->title,
        ch->level,
	get_age(ch),
	(get_age(ch) - 17) * 2 );
    send_to_char( buf, ch );

    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );
	send_to_char( buf, ch );
    }

    sprintf( buf,
	"You have %d/%d hit, and %d/%d mana.\n\r",

	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana);
    send_to_char( buf, ch );

 if (!IS_NPC(ch))
    sprintf( buf,
        "You have %d kills, %d losses, and %d wins.\n\r",
        ch->pcdata->kills, ch->pcdata->deaths,
        ch->pcdata->wins);
    send_to_char( buf, ch );

/* divide by zero problem */
  if (!IS_NPC(ch))
  {
    if (ch->pcdata->deaths == 0)
    {
      sprintf( buf,
        "Your K/D rating is %5.2f.\n\r",
        ((float) ch->pcdata->kills)); 
      send_to_char( buf, ch );
    }
    else
    {
      sprintf( buf,
        "Your K/D rating is %5.2f.\n\r",
        (((float) ch->pcdata->kills)/((float) ch->pcdata->deaths))); 
      send_to_char( buf, ch );
    }
  }

    sprintf( buf,
	"You are carrying %d/%d items with weight %d/%d kg.\n\r",
	ch->carry_number, can_carry_n(ch),
	ch->carry_weight, can_carry_w(ch) );
    send_to_char( buf, ch );

    sprintf( buf,
	"Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d.\n\r",
	get_curr_str(ch),
	get_curr_int(ch),
	get_curr_wis(ch),
	get_curr_dex(ch),
	get_curr_con(ch) );
    send_to_char( buf, ch );


    sprintf( buf, "Regen bonuses: Hit: %d%%  Mana: %d%%\n\r",
	ch->hit_regen_bonus, ch->mana_regen_bonus );
    send_to_char( buf, ch );


    sprintf( buf,
	"You have scored %d exp, and have %d gold coins.\n\r",
	ch->exp,  ch->gold );
    send_to_char( buf, ch );

/*
    sprintf( buf,
	"Autoexit: %s.  Autoloot: %s.  Autosac: %s.\n\r",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT)) ? "yes" : "no",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOLOOT)) ? "yes" : "no",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOSAC) ) ? "yes" : "no" );
    send_to_char( buf, ch );
*/

    sprintf( buf, "You have %d equipment points.\n\r",
      ch->eq_points );
    send_to_char( buf, ch );
 
/* fix for new bash */
    temp = ch->position;
    if (ch->fighting != NULL)
	temp = POS_FIGHTING;

    switch ( temp )
    {
    case POS_DEAD:     
	send_to_char( "You are DEAD!!\n\r",		ch );
	break;
    case POS_MORTAL:
	send_to_char( "You are mortally wounded.\n\r",	ch );
	break;
    case POS_INCAP:
	send_to_char( "You are incapacitated.\n\r",	ch );
	break;
    case POS_STUNNED:
	send_to_char( "You are stunned.\n\r",		ch );
	break;
    case POS_SLEEPING:
	send_to_char( "You are sleeping.\n\r",		ch );
	break;
    case POS_RESTING:
	send_to_char( "You are resting.\n\r",		ch );
	break;
    case POS_STANDING:
	send_to_char( "You are standing.\n\r",		ch );
	break;
    case POS_FIGHTING:
	send_to_char( "You are fighting.\n\r",		ch );
	break;
    }

    sprintf( buf, "AC: %d.  ", GET_AC(ch) );
    send_to_char( buf, ch );

    sprintf( buf, "Hitroll: %d  Damroll: %d.  Save: %d.\n\r",
	GET_HITROLL(ch), GET_DAMROLL(ch), ch->saving_throw );
    send_to_char( buf, ch );
    
    if ( ch->affected != NULL )
    {
	send_to_char( "You are affected by:\n\r", ch );
	for ( paf = ch->affected; paf != NULL; paf = paf->next )
	{
	    sprintf( buf, "Spell: '%s'", skill_table[paf->type].name );
	    send_to_char( buf, ch );

/*
		sprintf( buf,
		    " modifies %s by %d for %d hours",
		    affect_loc_name( paf->location ),
		    paf->modifier,
		    paf->duration );
		send_to_char( buf, ch );
*/

	    send_to_char( ".\n\r", ch );
	}
    }

    return;
}



char const day_name [][25] =
{
    "Death", "the Player Kill", "Deception", "Darkness", "Blood",
    "the Dead Gods", "the Burned Out Sun"
};

char const month_name [][25] =
{
    "Fire", "the Griffon", "Swords", "Axes",
    "War", "Satan", "the Warhammer", "Poison", "the Dragon",
    "the Elven Assassin", "Hell", "the Battle", "the Dead", "the Shadows",
    "the Noose", "the Ancient Darkness", "the River Styx"
};

void do_time( CHAR_DATA *ch, char *argument )
{
  extern char str_boot_time[];
  char buf[MAX_STRING_LENGTH];
  char *suf;
  int day;
  extern int last_boot_reason;
  static char suf_th[3] = "th";
  static char suf_st[3] = "st";
  static char suf_nd[3] = "nd";
  static char suf_rd[3] = "rd";

  day     = time_info.day + 1;

  if ( day > 4 && day <  20 ) suf = suf_th;
  else if ( day % 10 ==  1       ) suf = suf_st;
  else if ( day % 10 ==  2       ) suf = suf_nd;
  else if ( day % 10 ==  3       ) suf = suf_rd;
  else                             suf = suf_th;

  sprintf( buf,
    "It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r",
    (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
    time_info.hour >= 12 ? "pm" : "am",
    day_name[day % 7],
    day, suf,
    month_name[time_info.month] );
  send_to_char( buf, ch );

  sprintf( buf,
    "Merc started up at %s\n\rThe system time is %s\n\r",
	str_boot_time,
	(char *) ctime( &current_time ));
  send_to_char( buf, ch );

  if ( last_boot_reason == BOOT_CRASH )
    send_to_char("Last boot was caused by a crash.\n\r", ch );
  else
    send_to_char("Last boot from a manual or automatic reboot.\n\r", ch );

  return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];

  static char const sky_look[4][30] =
  {
    "cloudless",
    "cloudy",
    "rainy",
    "lit by flashes of lightning"
  };

  if ( !IS_OUTSIDE(ch) )
  {
    send_to_char( "Looking outside, you see that...\n\r", ch );
  }

  sprintf( buf, "The sky is %s and %s.\n\r",
    sky_look[weather_info.sky],
    weather_info.change >= 0
      ? "a warm southerly breeze blows"
      : "a cold northern gust blows"
    );
  send_to_char( buf, ch );
  return;
}



void do_help( CHAR_DATA *ch, char *argument )
{
  char argall[MAX_INPUT_LENGTH];
  char argone[MAX_INPUT_LENGTH];
  HELP_DATA *pHelp;
  HELP_DATA *pHelpPre = NULL;
  static char str_summary[8] = "summary";

  if ( argument[0] == '\0' )
    argument = str_summary;

  /*
   * Tricky argument handling so 'help a b' doesn't match a.
   */
  argall[0] = '\0';
  while ( argument[0] != '\0' )
  {
    argument = one_argument( argument, argone );
    if ( argall[0] != '\0' )
      strcat( argall, " " );
    strcat( argall, argone );
  }

  for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
  {
    if ( pHelp->level > get_trust( ch ) )
      continue;

    if ( is_name_prefix( argall, pHelp->keyword ) )
      pHelpPre = pHelp;

    if ( is_name( argall, pHelp->keyword ) )
      break;
  }

  if ( pHelp == NULL )
  {
    if ( pHelpPre == NULL )
    {
      send_to_char( "No help on that word.\n\r", ch );
      return;
    }
    else
      pHelp = pHelpPre;
  }

  if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
  {
    send_to_char( pHelp->keyword, ch );
    send_to_char( "\n\r", ch );
  }

  /*
   * Strip leading '.' to allow initial blanks.
   */
  if ( pHelp->text[0] == '.' )
    send_to_char( pHelp->text+1, ch );
  else
    send_to_char( pHelp->text  , ch );

  return;
}



void do_who( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CHAR_DATA *nch;
  int nMatch;
  extern int war_status;
  extern int war_total;
  extern int war_timer;
  extern int war_flags;
  extern int max_restore_hits;
  extern int max_restore_mana;
  extern int war_death;

  extern char *get_host( CHAR_DATA *ch, char *host );

  /* report war status */
  switch(war_status)
  {
    case NO_WAR:
      send_to_char(" There is currently no war on.\n\r", ch);
      break;
    case PRE_WAR:
      send_to_char(" War will begin soon.\n\r", ch);
      break;
    case WAR_ON:
      if( !IS_SET( war_flags, WAR_FLAG_CONTINUOUS ) ) 
        sprintf( buf, "Doomsday device set to go off in %d tick%s.\n\r",
        war_timer, war_timer == 1 ? "" : "s");
      else
        sprintf( buf, "World rebirth in %d tick%s.\n\r",
        war_timer, war_timer == 1 ? "" : "s" );

      send_to_char( buf, ch );
      if ( IS_SET(war_flags,WAR_FLAG_DOOM) )
      {
        sprintf( buf, "[Wrath in: %d] ", war_death );
        send_to_char( buf, ch );
      }
      sprintf(buf,"[Join status: HP:%d%% MANA:%d%%] [War length: %d]\n\r",
        max_restore_hits <= 1           ? 0 : max_restore_hits,
        max_restore_hits <= 1           ? 0 : max_restore_mana,
        war_total );
      send_to_char( buf, ch );
      break;
    case POST_WAR:
      send_to_char(" The war is over, reseting soon.\n\r", ch );
      break;
  }

    sprintf(buf,"War flags:%s%s%s%s%s%s%s%s%s%s\n\r",
	IS_SET(war_flags,WAR_FLAG_CONTINUOUS) ? " [Continuous]" : "",
	IS_SET(war_flags,WAR_FLAG_BLOODS)     ? " [Bloods]" : "",
	IS_SET(war_flags,WAR_FLAG_LOADALL)    ? " [Loadall]" : "",
	IS_SET(war_flags,WAR_FLAG_DOOM)       ? " [Wrath]" : "",
	IS_SET(war_flags,WAR_FLAG_NOREGEN)    ? " [Noregen]" : "",
	IS_SET(war_flags,WAR_FLAG_SUMMON)     ? " [Summon]" : "",
	IS_SET(war_flags,WAR_FLAG_FOUNTAINS)  ? " [Fountains]" : "",
	IS_SET(war_flags,WAR_FLAG_RANDOMCLASS)? " [Randomclass]" : "",
	IS_SET(war_flags,WAR_FLAG_NOQUAFF)    ? " [Noquaff]" : "",
	IS_SET(war_flags,WAR_FLAG_HEALTHLAG)  ? " [Healthlag]" : ""
	);
    send_to_char(buf, ch );

  /*
   * Now show matching chars.
   */
  nMatch = 0;
  buf[0] = '\0';
  for ( nch = char_list; nch != NULL; nch = nch->next )
  {
    CHAR_DATA *wch;
    char const *char_class;
    int total_wars;

    /*
     * Check for match against restrictions.
     * Don't use trust as that exposes trusted mortals.
     */
    if (IS_NPC(nch)
    || (nch->desc == NULL ? 0 : nch->desc->connected != CON_PLAYING)
    || (!can_see(ch, nch ) && nch->level > 30 ) )
      continue;

    if (nch->desc != NULL)
    {
      wch = ( nch->desc->original != NULL ) ? nch->desc->original : nch;
    }
    else
    {
      wch = nch;
    }

    nMatch++;

    /*
     * Figure out what to print for class.
     */
    char_class = class_table[wch->char_class].who_name;
    if ( IS_SET( wch->act, PLR_RANDOM_CLASS ) )
      char_class = "rnd";

    switch ( wch->level )
    {
      default:

        if ( !in_main_war( ch ) )
        {
          if ( IS_SET( wch->act, PLR_RANDOM_CLASS ) )
            char_class = "rand";
          else
            char_class = "flap";
        }
        break;

      case MAX_LEVEL - 0: char_class = "ADMN"; break;
      case MAX_LEVEL - 1: char_class = "STAF"; break;
      case MAX_LEVEL - 2: char_class = "STAF"; break;
      case MAX_LEVEL - 3: char_class = "HELP"; break;
      case MAX_LEVEL - 4: char_class = "HELP"; break;
    }

    if ( !str_cmp( wch->name, "howler" ) )
      char_class = "LORD";
    if ( !str_cmp( wch->name, "terror" ) )
      char_class = "FEAR";   
    if ( !str_cmp( wch->name, "universe" ) )
      char_class = "AHOL";
    /*
     * Format it up.
     */

    /* slight divide by zero problem here */
    total_wars = wch->pcdata->deaths + wch->pcdata->wins;
    if ( total_wars == 0 )
      total_wars = 1;

    sprintf( buf + strlen( buf ), "[" );
    if ( wch->pcdata->kills < 10 )
      sprintf( buf + strlen( buf ), "newb " );
    else if ( check_no_rating_list( wch, FALSE ) )
      sprintf( buf + strlen( buf ), "n/a  " );
    else
      sprintf( buf + strlen( buf ), "%4d ", wch->pcdata->rating );

    sprintf( buf + strlen(buf), "%5.2f %2d %d%% %s] ",
      ((float) wch->pcdata->kills)/(wch->pcdata->deaths
                                    ?((float) wch->pcdata->deaths):1.0),
      (wch->pcdata->wins * 100 / total_wars),
      (wch->pcdata->doomsdays * 100 / total_wars),
      char_class );

    /* and tell if in war */
    if ( wch->desc == NULL )
      sprintf( buf + strlen(buf), "(LINKDEAD) " );

    if ( IS_SET( wch->act, PLR_NOPLAY ) )
      sprintf( buf + strlen( buf ), "(NOPLAY) " );

    if ( mortal_in_main_war( wch ) )
      sprintf( buf + strlen(buf), "(IN WAR) " );
    else if ( in_ffa_war( wch ) && wch->level < LEVEL_IMMORTAL )
      sprintf( buf + strlen( buf ), "(FFA WAR) " );
    else if ( in_private_war( wch ) && wch->level < LEVEL_IMMORTAL )
      sprintf( buf + strlen( buf ), "(PRIVATE WAR) " );
    if ( !can_communicate( wch, FALSE ) )
      strcat( buf, "(SNOOP) "   );

    if (ch->level < 37)
    {
      sprintf( buf + strlen(buf),  "%s%s\n\r",
        wch->name,
        wch->pcdata->title );
    }
    else
    {
      sprintf( buf + strlen(buf), "%s ", wch->name );
      sprintf( buf + strlen(buf), " < %3d %3d > is at: %s\n\r",
        wch->hit,
        wch->mana,
        wch->in_room->name );
    }
  }

  sprintf( buf2, "%d player%s.\n\r", nMatch, nMatch == 1 ? "" : "s" );
  send_to_char( buf2, ch );
  send_to_char( buf, ch );
  return;
}



void do_inventory( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	    continue;

	send_to_char( where_name[iWear], ch );
	if ( can_see_obj( ch, obj )
	|| !mortal_in_main_war( ch ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
	else
	{
	    send_to_char( "something.\n\r", ch );
	}
	found = TRUE;
    }

    if ( !found )
	send_to_char( "Nothing.\n\r", ch );

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;
    static char str_same_item[52]
      = "You compare $p to itself.  It looks about the same.";
    static char str_cant[29]
      = "You can't compare $p and $P.";
    static char str_same[31]
      = "$p and $P look about the same.";
    static char str_better[25]
      = "$p looks better than $P.";
    static char str_worse[24]
      = "$p looks worse than $P.";

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	for ( obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
	    &&   can_see_obj( ch, obj2 )
            &&   mortal_in_main_war( ch )
	    &&   obj1->item_type == obj2->item_type
	    && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if ( obj2 == NULL )
	{
	    send_to_char( "You aren't wearing anything comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
    }
	    
    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = str_same_item;
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = str_cant;
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = str_cant;
	    break;

	case ITEM_ARMOR:
	case ITEM_JEWELRY:
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case ITEM_WEAPON:
	    value1 = obj1->value[1] + obj1->value[1] * obj1->value[2];
	    value2 = obj2->value[1] + obj2->value[1] * obj2->value[2];
	    break;
	}
    }

    if ( msg == NULL )
    {
	     if ( value1 == value2 ) msg = str_same;
	else if ( value1  > value2 ) msg = str_better;
	else                         msg = str_worse;
    }

    act( msg, ch, obj1, obj2, TO_CHAR, TRUE );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
  static char str_diku[5] = "diku";

  do_help( ch, str_diku );
  return;
}



void do_motd( CHAR_DATA *ch, char *argument )
{
  do_help( ch, str_motd );
  return;
}



void do_wizlist( CHAR_DATA *ch, char *argument )
{
  static char str_wizlist[8] = "wizlist";

  do_help( ch, str_wizlist );
  return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int fFree = FALSE;
  bool found;

  one_argument( argument, arg );

  sprintf( buf, "You're in %s, written by %s.\n\r",
    ch->in_room->area->name,
    ch->in_room->area->author );
  send_to_char( buf, ch );

  if ( in_private_war( ch ) )
  {
    if ( ch->private_war == NULL )
    {
      send_to_char( "Bug: You're in a private war with no opponent.\n\r", ch );
      return;
    }
    sprintf( buf, "%-28s %s\n\r",
      ch->private_war->name, ch->private_war->in_room->name );
    send_to_char( buf, ch );
    return;
  }

  if ( !in_main_war( ch ) )
    return;

  if ( IS_NPC( ch )
  || number_percent( ) < ch->pcdata->learned[gsn_free_wheres] )
    fFree = TRUE;

  if ( !IS_NPC(ch) && ch->level < 40
  && ch->gold < 0 && fFree == FALSE )
  {
    send_to_char( "However, you can't afford a where.\n\r", ch );

    WAIT_STATE( ch, 12 );
    return;
  }

  if ( arg[0] == '\0' )
  {
    if ( fFree == FALSE )
      ch->gold -= 0;

    sprintf( buf, "Players in zone %s:\n\r", ch->in_room->area->name );
    send_to_char( buf, ch );
    found = FALSE;
    for ( victim = char_list; victim; victim = victim->next )
    {
      if ( IS_NPC( victim ) )
      {
        if ( !IS_SET( victim->act, ACT_TERMINATOR )
	&& !IS_SET( victim->act, ACT_ILLUSION ) )
	  continue;
      }
      else
      {
        if ( ch->desc == NULL )
	  continue;
      }

      if ( !can_see( ch, victim ) )
        continue;

      if ( victim->in_room == NULL
      || victim->in_room->area != ch->in_room->area )
        continue;

      found = TRUE;
      if ( !IS_NPC( victim ) )
        sprintf( buf, "%s%-28s %s\n\r",
          (mortal_in_main_war( victim ) ? "(IN_WAR)" : " " ),
          victim->name, victim->in_room->name );
      else if ( IS_SET( victim->act, ACT_TERMINATOR  ) )
        sprintf( buf, "%-28s %s\n\r",
	victim->short_descr, victim->in_room->name );
      else if ( IS_SET( victim->act, ACT_ILLUSION  ) )
        sprintf( buf, "(IN_WAR)%-28s %s\n\r",
	  victim->name, victim->in_room->name );
      send_to_char( buf, ch );
    }
    if ( !found )
      send_to_char( "None\n\r", ch );
  }
  else
  {
    if ( !IS_NPC( ch ) && ch->level < 36
    && ch->gold < 30 && fFree == FALSE )
    {
      send_to_char( "You don't have enough gold.\n\r", ch );
      WAIT_STATE( ch, 12 );
      return;
    }

    found = FALSE;
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
      if ( victim->in_room != NULL
      &&   is_name( arg, victim->name )
      &&   victim->level <= ch->level )
      {
	found = TRUE;
	sprintf( buf, "%-28s %s\n\r",
	  PERS(victim, ch), victim->in_room->name );
	send_to_char( buf, ch );

	if ( fFree == FALSE )
	{
	  ch->gold -= 30;
	  send_to_char( "You are charged 30 coins.\n\r", ch );
	}
	break;
      }
    }
    if ( !found )
      act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR, TRUE );
  }

  return;
}


#if 0
#define WHEREIS_COST 100
#endif

void do_whereis( CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA *pRoom;
  char buf[MAX_STRING_LENGTH];
  static int marker = 0;
  bool fFound = FALSE;
  int i;
  extern ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];

  marker++;

#ifdef WHEREIS_COST
  if ( ch->level <= 30 && ch->gold < WHEREIS_COST )
  {
    send_to_char( "You don't have enough gold.\n\r", ch );
    return;
  }
#endif

  for ( i = 0; i < MAX_KEY_HASH; i++ )
    for ( pRoom = room_index_hash[i]; pRoom != NULL; pRoom = pRoom->next )
    {
      if ( pRoom->area->mark == marker )
	continue;

      if ( str_cmp( argument, pRoom->name ) )
	continue;

      sprintf( buf, "%s [%d] - %s\n\r", pRoom->name, pRoom->vnum,
        pRoom->area->name );
      send_to_char( buf, ch );
      pRoom->area->mark = marker;
      fFound = TRUE;
    }

    if ( fFound )
    {
#ifdef WHEREIS_COST
      if ( ch->level <= 30 )
	ch->gold -= WHEREIS_COST;
#endif
    }
    else
    {
      sprintf( buf, "You couldn't find '%s'.\n\r", argument );
      send_to_char( buf, ch );
    }

    return;
}



void do_whereload( CHAR_DATA *ch, char *argument )
{
  RESET_DATA *r;
  AREA_DATA *a;
  int number = 0;
  int marker = (int) current_time;
  int vnum;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  MOB_INDEX_DATA *mIndex;
  OBJ_INDEX_DATA *oIndex;
  ROOM_INDEX_DATA *rIndex;
  extern AREA_DATA *area_first;

  one_argument( argument, arg );

  if ( !is_number( arg )
  || ( get_obj_index( atoi( arg ) ) == NULL ) )
  {
    send_to_char("Syntax:  whereload <object vnum>\n\r", ch );
    return;
  }

  vnum = atoi( arg );

  for ( a = area_first; a != NULL; a = a->next )
  {
    for ( r = a->reset_first; r != NULL; r = r->next )
    {
      if ( number > 20 )
      {
	send_to_char("20 or more matches.  Ending search.\n\r", ch );
	return;
      }

      switch ( r->command )
      {
	case 'G':
	case 'E':
	  if ( r->arg1 == vnum )
	  {
	    if ( mIndex && mIndex->mark != marker )
	    {
	      sprintf( buf, "Loads on mob: %s\n\r", mIndex->short_descr );
	      send_to_char( buf, ch );
	      number++;
	      mIndex->mark = marker;
	    }
	  }
	  else
	    oIndex = get_obj_index( r->arg1 );
	  break;
	case 'M':
	  mIndex = get_mob_index( r->arg1 );
	  break;
	case 'O':
	  if ( r->arg1 == vnum )
	  {
	    rIndex = get_room_index( r->arg3 );
	    if ( rIndex != NULL && rIndex->mark != marker )
	    {
	      sprintf( buf, "Loads in room: %s\n\r", rIndex->name );
	      send_to_char( buf, ch );
	      number++;
	      rIndex->mark = marker;
	    }
	  }
	  break;
	case 'P':
	  if ( r->arg1 == vnum )
	  {
	    oIndex = get_obj_index( r->arg3 );
	    if ( oIndex != NULL && oIndex->mark != marker )
	    {
	      sprintf( buf, "Loads in object: %s\n\r", oIndex->short_descr );
	      send_to_char( buf, ch );
	      number++;
	      oIndex->mark = marker;
	    }
	  }
	  break;
	default:
	  break;
      }
    }
  }
  if ( number == 0 )
    send_to_char("That object doesn't seem to load.\n\r", ch );

  return;
}


void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;
    static char str_consider[40] = "Consider is not supported by this game.";

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    diff = victim->level - ch->level;

#if 0
         if ( diff <= -10 ) msg = "You can kill $N naked and weaponless.";
    else if ( diff <=  -5 ) msg = "$N is no match for you.";
    else if ( diff <=  -2 ) msg = "$N looks like an easy kill.";
    else if ( diff <=   1 ) msg = "The perfect match!";
    else if ( diff <=   4 ) msg = "$N says 'Do you feel lucky, punk?'.";
    else if ( diff <=   9 ) msg = "$N laughs at you mercilessly.";
    else                    msg = "Death will thank you for your gift.";
#endif
    msg = str_consider;

    act( msg, ch, NULL, victim, TO_CHAR, TRUE );
    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( isalpha(title[0]) || isdigit(title[0]) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( !can_communicate( ch, TRUE ) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if ( strlen(argument) > 50 )
	argument[50] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );
/*
	if ( argument[0] == '+' )
	{
	    if ( ch->description != NULL )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace(*argument) )
		argument++;
	}
*/
	if ( strlen(buf) + strlen(argument) >= 65 )
	{
	    send_to_char( "Description too long, 65 character max.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }

    send_to_char( "Your description is:\n\r", ch );
    send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
    return;
}



void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	int col;

	col    = 0;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;

            if ( ch->pcdata->learned[sn] < 1 )
		continue;

	    sprintf( buf, "%18s %3d%%  ",
		skill_table[sn].name, ch->pcdata->learned[sn] );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 )
		send_to_char( "\n\r", ch );
	}

	if ( col % 3 != 0 )
	    send_to_char( "\n\r", ch );

	sprintf( buf, "You have %d practice sessions left.\n\r",
	    ch->practice );
	send_to_char( buf, ch );
    }
    else
    {
	CHAR_DATA *mob;
	int adept;

	if ( !IS_AWAKE(ch) )
	{
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    return;
	}

	for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
	{
	    if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
		break;
	}

	if ( mob == NULL )
	{
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	if ( ch->practice <= 0 )
	{
	    send_to_char( "You have no practice sessions left.\n\r", ch );
	    return;
	}

	if ( ( sn = skill_lookup( argument ) ) < 0 )
	{
	    send_to_char( "You can't practice that.\n\r", ch );
	    return;
	}

	adept = 95;

	if ( ch->pcdata->learned[sn] >= adept )
	{
	    sprintf( buf, "You are already an adept of %s.\n\r",
		skill_table[sn].name );
	    send_to_char( buf, ch );
	}
	else
	{
	    ch->practice--;
	    ch->pcdata->learned[sn] += int_app[get_curr_int(ch)].learn;
	    if ( ch->pcdata->learned[sn] < adept )
	    {
		act( "You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR, TRUE );
		act( "$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM, TRUE );
	    }
	    else
	    {
		ch->pcdata->learned[sn] = adept;
		act( "You are now an adept of $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR, TRUE );
		act( "$n is now an adept of $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM, TRUE );
	    }
	}
    }
    return;
}



void do_tmc( CHAR_DATA *ch, char *argument )
{
}


void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	return;
    }

    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }

    if ( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_socials( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;
    extern int MAX_SOCIAL;

    col = 0;
    for ( iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++ )
    {
	sprintf( buf, "%-12s", social_table[iSocial].name );
	send_to_char( buf, ch );
	if ( ++col % 6 == 0 )
	    send_to_char( "\n\r", ch );
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );

    col = 0;
    for ( iSocial = 0; iSocial < MAX_SOCIAL; iSocial++ ) 
    {
	sprintf( buf, "%-12s", socials[iSocial].name );
	send_to_char( buf, ch );
	if ( ++col % 6 == 0 )
	    send_to_char( "\n\r", ch );
    }

    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );

    return;
}



void do_spells( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;
    int col;

    col = 0;
    for ( sn = 0; sn < MAX_SKILL && skill_table[sn].name != NULL; sn++ )
    {
	sprintf( buf, "%-12s", skill_table[sn].name );
	send_to_char( buf, ch );
	if ( ++col % 6 == 0 )
	    send_to_char( "\n\r", ch );
    }

    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}



/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
 
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level <  LEVEL_HERO
        &&   cmd_table[cmd].level <= get_trust( ch ) )
	{
	    sprintf( buf, "%-12s", cmd_table[cmd].name );
	    send_to_char( buf, ch );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}



void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
	{
	    send_to_char( "You are silenced.\n\r", ch );
	    return;
	}

	send_to_char( "Channels:", ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_AUCTION)
	    ? " +AUCTION"
	    : " -auction",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_CHAT)
	    ? " +GOSSIP"
	    : " -gossip",
	    ch );

	if ( IS_HERO(ch) )
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_IMMTALK)
		? " +IMMTALK"
		: " -immtalk",
		ch );
	}

	send_to_char( !IS_SET(ch->deaf, CHANNEL_MUSIC)
	    ? " +MUSIC"
	    : " -music",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_QUESTION)
	    ? " +QUESTION"
	    : " -question",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_SHOUT)
	    ? " +SHOUT"
	    : " -shout",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_YELL)
	    ? " +YELL"
	    : " -yell",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_INFO)
	    ? " +INFO"
	    : " -info",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_DEATH)
	    ? " +DEATH"
	    : " -death",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_WARMESSAGE)
		? " +WARMESSAGE"
		: " -warmessage",
		ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_SUBWAR)
	    ? " +SUBWAR"
	    : " -subwar",
	    ch );


	send_to_char( ".\n\r", ch );
    }
    else
    {
	bool fClear;
	int bit;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char( "Channels -channel or +channel?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;
        else if ( !str_cmp( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
        else if ( !str_cmp( arg+1, "gossip"   ) ) bit = CHANNEL_CHAT;
	else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
	else if ( !str_cmp( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
	else if ( !str_cmp( arg+1, "question" ) ) bit = CHANNEL_QUESTION;
	else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
	else if ( !str_cmp( arg+1, "info"     ) ) bit = CHANNEL_INFO;
	else if ( !str_cmp( arg+1, "death"    ) ) bit = CHANNEL_DEATH;
	else if ( !str_cmp( arg+1, "warmessage")) bit = CHANNEL_WARMESSAGE;
	else if ( !str_cmp( arg+1, "subwar"   ) ) bit = CHANNEL_SUBWAR;
	else
	{
	    send_to_char( "Set or clear which channel?\n\r", ch );
	    return;
	}

	if ( fClear )
	    REMOVE_BIT (ch->deaf, bit);
	else
	    SET_BIT    (ch->deaf, bit);

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}



/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "[ Keyword  ] Option\n\r", ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOEXIT)
            ? "[+AUTOEXIT ] You automatically see exits.\n\r"
	    : "[-autoexit ] You don't automatically see exits.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_BLANK)
	    ? "[+BLANK    ] You have a blank line before your prompt.\n\r"
	    : "[-blank    ] You have no blank line before your prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_BRIEF)
	    ? "[+BRIEF    ] You see brief descriptions.\n\r"
	    : "[-brief    ] You see long descriptions.\n\r"
	    , ch );
         
	send_to_char(  IS_SET(ch->act, PLR_COMBINE)
	    ? "[+COMBINE  ] You see object lists in combined format.\n\r"
	    : "[-combine  ] You see object lists in single format.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_PROMPT)
	    ? "[+PROMPT   ] You have a prompt.\n\r"
	    : "[-prompt   ] You don't have a prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_TELNET_GA)
	    ? "[+TELNETGA ] You receive a telnet GA sequence.\n\r"
	    : "[-telnetga ] You don't receive a telnet GA sequence.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_SILENCE)
	    ? "[+SILENCE  ] You are silenced.\n\r"
	    : ""
	    , ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_EMOTE)
	    ? ""
	    : "[-emote    ] You can't emote.\n\r"
	    , ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_TELL)
	    ? ""
	    : "[-tell     ] You can't use 'tell'.\n\r"
	    , ch );
    }
    else
    {
	bool fSet;
	int bit;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Config -option or +option?\n\r", ch );
	    return;
	}

             if ( !str_cmp( arg+1, "autoexit" ) ) bit = PLR_AUTOEXIT;
	else if ( !str_cmp( arg+1, "autoloot" ) ) bit = PLR_AUTOLOOT;
	else if ( !str_cmp( arg+1, "autosac"  ) ) bit = PLR_AUTOSAC;
	else if ( !str_cmp( arg+1, "blank"    ) ) bit = PLR_BLANK;
	else if ( !str_cmp( arg+1, "brief"    ) ) bit = PLR_BRIEF;
	else if ( !str_cmp( arg+1, "combine"  ) ) bit = PLR_COMBINE;
        else if ( !str_cmp( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_cmp( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
	else
	{
	    send_to_char( "Config which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	    SET_BIT    (ch->act, bit);
	else
	    REMOVE_BIT (ch->act, bit);

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}


void do_autoexit( CHAR_DATA *ch, char *argument )
{
  if ( IS_NPC( ch ) )
    return;

  if ( !IS_SET( ch->act, PLR_AUTOEXIT ) )
  {
    SET_BIT( ch->act, PLR_AUTOEXIT );
    send_to_char( "Autoexit mode enabled.\n\r", ch );
  }
  else
  {
    REMOVE_BIT( ch->act, PLR_AUTOEXIT );
    send_to_char( "Autoexit mode disabled.\n\r", ch );
  }

  return;
}


void do_brief( CHAR_DATA *ch, char *argument )
{
  if ( IS_NPC( ch ) )
    return;

  if ( !IS_SET( ch->act, PLR_BRIEF ) )
  {
    SET_BIT( ch->act, PLR_BRIEF );
    send_to_char( "Brief mode activated.\n\r", ch );
  }
  else
  {
    REMOVE_BIT( ch->act, PLR_BRIEF );
    send_to_char( "Brief mode disabled.\n\r", ch );
  }

  return;
}


void do_compact( CHAR_DATA *ch, char *argument )
{
  if ( IS_NPC( ch ) )
    return;

  if ( !IS_SET( ch->act, PLR_BLANK ) )
  {
    SET_BIT( ch->act, PLR_BLANK );
    send_to_char( "Compact mode disabled.\n\r", ch );
  }
  else
  {
    REMOVE_BIT( ch->act, PLR_BLANK );
    send_to_char( "Compact mode enabled.\n\r", ch );
  }

  return;
}


void do_randomclass( CHAR_DATA *ch, char *argument )
{
/* simple routine to toggle randomclass */

    if (IS_NPC(ch))
	return;

    if ( in_static_war( ch ) )
    {
	send_to_char("You can't change your random status in war.\n\r", ch );
	return;
    }

	if (!IS_SET( ch->act, PLR_RANDOM_CLASS ))
	{
	    send_to_char("Your class will now be set at random.\n\r", ch );
	    SET_BIT( ch->act, PLR_RANDOM_CLASS );
            return;
	}

	send_to_char("Your class will no longer be set at random.\n\r", ch );
	REMOVE_BIT( ch->act, PLR_RANDOM_CLASS );
	return;
}



void do_noplay( CHAR_DATA *ch, char *argument )
{
/* simple routine to toggle noplay */

  if (IS_NPC(ch))
    return;

  if ( !IS_SET( ch->act, PLR_NOPLAY ) )
  {
    send_to_char("You will not play in the next war.\n\r", ch );
    SET_BIT( ch->act, PLR_NOPLAY );
    return;
  }

  send_to_char("You will play in the next war.\n\r", ch );
  REMOVE_BIT( ch->act, PLR_NOPLAY );
  return;
}


void do_finger( CHAR_DATA *ch, char *argument )
{
  int char_class;
  int col;
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument( argument, arg );

  if ( arg[0] == '\0' )
  {
    send_to_char( "Finger who?\n\r", ch );
    return;
  }

  if ( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if ( IS_NPC( victim ) )
  {
    send_to_char( "You can't finger them.\n\r", ch );
    return;
  }

  sprintf( buf, "%s\n\r", victim->name );
  send_to_char( buf, ch );

  sprintf( buf, "Kills: %d, Deaths: %d, Wins: %d, K/D: %5.2f, Blicks/ed: %d/%d.\n\r",
    victim->pcdata->kills,
    victim->pcdata->deaths,
    victim->pcdata->wins,
    ((float)victim->pcdata->kills /
      ((float)victim->pcdata->deaths
        ? (float) victim->pcdata->deaths : 1.0 ) ),
    victim->pcdata->blicks,
    victim->pcdata->blicked );
  send_to_char( buf, ch );

  if ( victim->pcdata->rated_kills > 0 )
  {
    sprintf( buf, "Average victim K/D: %5.2f, Ave vict/killer/opponent rating: %d/%d/%d.\n",
      ((float) victim->pcdata->total_victim_kds /
        (float) victim->pcdata->kills / 100 ),
      (victim->pcdata->ratings_victims / victim->pcdata->rated_kills),
	(victim->pcdata->rated_deaths ? 
	 (victim->pcdata->ratings_killers / victim->pcdata->rated_deaths) : 0 ),
      ((victim->pcdata->ratings_killers + victim->pcdata->ratings_victims)
	/ ( victim->pcdata->rated_kills + victim->pcdata->rated_deaths ) ) );
    send_to_char( buf, ch );
  }
		

  sprintf( buf, "Damage Inflicted: %d,  ", victim->pcdata->damage_inflicted );
  send_to_char( buf, ch );
  sprintf( buf, "Damage Received:  %d,  ", victim->pcdata->damage_received );
  send_to_char( buf, ch );

  if ( in_static_war( victim ) )
  {
    sprintf( buf, "Potential DR:  %d\n\r", victim->pcdata->potential_dr );
    send_to_char( buf, ch );
  }
  else
    send_to_char( "\n\r", ch );

  sprintf( buf, "Doomsdays: %d,  Linkdead at end:  %d,  Mob deaths:  %d\n\r",
    victim->pcdata->doomsdays,
    victim->pcdata->ld_at_end,
    victim->pcdata->mob_deaths );
  send_to_char( buf, ch );

  send_to_char( "\n\r", ch );

  send_to_char( "Executions:\n\r", ch );

  col = 0;
  for ( char_class = 0; char_class < MAX_CLASS; char_class++ )
  {
    if ( victim->pcdata->class_kills[char_class] > 0 )
    {
      sprintf( buf, "%15s: %3d  ",
	class_table[char_class].class_name,
	victim->pcdata->class_kills[char_class] );
      send_to_char( buf, ch );

      if ( ++col % 3 == 0 )
      {
	send_to_char("\n\r", ch );
	col = 0;
      }
    }
  }

  if ( col % 3 != 0 )
    send_to_char("\n\r", ch );

  col = 0;

  send_to_char("Deaths:\n\r", ch );

  for ( char_class = 0; char_class < MAX_CLASS; char_class++ )
  {
    if ( victim->pcdata->class_deaths[char_class] > 0 )
    {
      sprintf( buf, "%15s: %3d  ",
	class_table[char_class].class_name,
	victim->pcdata->class_deaths[char_class] );
      send_to_char( buf, ch );

      if ( ++col % 3 == 0 )
      {
	send_to_char("\n\r", ch );
	col = 0;
      }
    }
  }

  if ( col % 3 != 0 )
    send_to_char("\n\r", ch );

  return;
}


void do_changeclass( CHAR_DATA *ch, char *argument )
{
  int iClass;
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  extern void set_class_profile( CHAR_DATA *ch );

  while ( isspace(*argument) )
    argument++;

  if (IS_NPC(ch))
    return;

  if ( in_static_war( ch ) )
  {
    send_to_char( "You may not change class while in a war.\n\r", ch );
    return;
  }

  for ( iClass = 0; iClass < MAX_CLASS ; iClass++ )
  {
    if (!strcmp( argument, class_table[iClass].who_name )
    || !strcmp( argument, class_table[iClass].class_name ) )
      break;
  }

  if ( iClass == MAX_CLASS )
  {
    send_to_char( "Invalid class.\n\r", ch );
    send_to_char( "Use the 'cview' command for a list of classes.\n\r", ch );
    return;
  }

  if ( class_table[iClass].enabled == FALSE && ch->level < 36 )
  {
    send_to_char("That class is currently disabled.\n\r", ch );
    return;
  }

  if ( class_table[iClass].min_rating > ch->pcdata->rating )
  {
    send_to_char("That class is for advanced players only.\n\r", ch );
    send_to_char("Classes that require higher ratings are not better\n\r", ch );
    send_to_char("so much as more difficult to play.  Defiler, for\n\r", ch );
    send_to_char("example, has many spells, but only 60 hps.  If you\n\r", ch );
    send_to_char("don't know the MUD pretty well, you'll die.\n\r", ch );
    return;
  }

  send_to_char("Removing your equipment.\n\r", ch );

  for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
  {
    if ( obj->wear_loc != WEAR_NONE )
      unequip_char( ch, obj );
  }

  ch->char_class = iClass;

  set_class_profile( ch );

  sprintf( buf, "Class changed to %s.\n\r",
    class_table[ch->char_class].class_name );
  send_to_char( buf, ch );

  return;
}

void set_class_profile( CHAR_DATA *ch )
{
    int iClass = ch->char_class;
    int sn;

  if( ch->level < 40 ) {
    ch->pcdata->perm_str = class_table[iClass].strength;
    ch->pcdata->perm_dex = class_table[iClass].dexterity;
    ch->pcdata->perm_int = class_table[iClass].intelligence;
    ch->pcdata->perm_wis = class_table[iClass].wisdom;
    ch->pcdata->perm_con = class_table[iClass].constitution;

    ch->max_hit = class_table[iClass].hits;
    ch->max_mana = class_table[iClass].mana;

    ch->armor = class_table[iClass].base_armor;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	ch->pcdata->learned[sn] = class_table[iClass].skill_percent[sn];
    }

    ch->resist_fire = class_table[iClass].resist_fire;
    ch->resist_cold = class_table[iClass].resist_cold;
    ch->resist_lightning = class_table[iClass].resist_lightning;
    ch->resist_physical = class_table[iClass].resist_physical;
    ch->resist_magic = class_table[iClass].resist_magic;
    ch->resist_drain = class_table[iClass].resist_drain;
  }

  if ( iClass == CLASS_DRAGON )
    spell_dragon_power( gsn_dragon_power, 30, ch, ch );

  return;
}


void select_random_class( CHAR_DATA *ch )
{
  char buf[MAX_STRING_LENGTH];
  int iClass;
  int i = 1;

  while ( i++ )
  {
    iClass = number_range( 1, MAX_CLASS ) - 1;

    /* prevent random spec mages and barb */
    if ( class_table[iClass].enabled == TRUE
    && class_table[iClass].random > number_percent( )
    && class_table[iClass].min_rating <= ch->pcdata->rating )
      break;

    if ( i > 50 )
      break;
  }

  ch->char_class = iClass;

  sprintf( buf, "Your class has been set to %s.\n\r",
    class_table[iClass].class_name );
  send_to_char( buf, ch );

  return;
}

void do_joinffa( CHAR_DATA *ch, char *argument )
{
  if ( in_a_war( ch ) )
  {
    send_to_char( "You're already in a war.\n\r", ch );
    return;
  }

  if( IS_SET( ch->act, PLR_RANDOM_CLASS ) )
    select_random_class( ch );

  set_class_profile( ch );

  ch->hit  = ch->max_hit;
  ch->mana = ch->max_mana;

  ch->pcdata->war_num = WAR_NUM_FFA;

  return;
}



void do_join_private( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];

  if ( IS_NPC( ch ) )
  {
    send_to_char( "Only players can run private wars.\n\r", ch );
    return;
  }

  one_argument( argument, arg );
  if ( arg[0] == '\0' )
  {
    send_to_char( "You must invite someone to join a private war.\n\r", ch );
    send_to_char( "private <victim>\n\r", ch );
    send_to_char( "To reject a request, use reject as the victim.\n\r", ch );
    send_to_char( "Reject also removes your invitations.\n\r", ch );
    return;
  }

  if ( !str_cmp( arg, "reject" ) )
  {
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
      if ( victim->private_war == ch )
      {
        send_to_char(
	  "Your private war invitation has been rejected.\n\r", ch );
        victim->private_war = NULL;
      }
    }
    ch->private_war = NULL;

    send_to_char( "You reject all requests for private wars.\n\r", ch );
    return;
  }

  if ( in_a_war( ch ) )
  {
    send_to_char( "You're already in a war.\n\r", ch );
    return;
  }

  if ( ch->eq_points > 25 )
  {
    send_to_char( "You have too many equipment points.  (max 25)\n\r", ch );
    return;
  }

  if ( ch->private_war != NULL )
  {
    send_to_char(
      "You have already invited someone to a private war.\n\r", ch );
    return;
  }

  victim = get_char_world( ch, arg );

  if ( victim == NULL )
  {
    send_to_char( "There is no one in the world by that name.\n\r", ch );
    return;
  }

  if ( IS_NPC( victim ) )
  {
    send_to_char( "NPCs aren't very good at private wars.\n\r", ch );
    return;
  }

  if ( in_a_war( victim ) )
  {
    send_to_char( "They are already in a war.\n\r", ch );
    return;
  }

  if ( ch->in_room == NULL
  || victim->in_room == NULL
  || ch->in_room->area != victim->in_room->area )
  {
    send_to_char( "You must be in the same area.\n\r", ch );
    return;
  }

  if ( victim->private_war == NULL )
  {
    sprintf( buf, "You invite %s to a private war.\n\r", victim->name );
    send_to_char( buf, ch );
    sprintf( buf, "%s has invited you to a private war.\n\r", ch->name );
    send_to_char( buf, victim );
    ch->private_war = victim;
    return;
  }

  if ( victim->private_war != ch )
  {
    send_to_char( "They have invited someone else to a private war.\n\r", ch );
    return;
  }

  ch->private_war = victim;

  if( IS_SET( ch->act, PLR_RANDOM_CLASS ) )
    select_random_class( ch );
  if( IS_SET( victim->act, PLR_RANDOM_CLASS ) )
    select_random_class( victim );

  set_class_profile( ch );
  set_class_profile( victim );

  ch->hit  = ch->max_hit;
  ch->mana = ch->max_mana;
  victim->hit  = victim->max_hit;
  victim->mana = victim->max_mana;

  ch->pcdata->war_num = WAR_NUM_PRIVATE;
  victim->pcdata->war_num = WAR_NUM_PRIVATE;

  send_to_char( "The private war has begun.\n\r", ch );
  send_to_char( "The private war has begun.\n\r", victim );

  return;
}



void do_joinwar( CHAR_DATA *ch, char *argument )
{
    extern int war_flags;
    extern int war_status;
    extern int war_timer;
    extern int war_number;
    extern int war_max;
    char buf[MAX_STRING_LENGTH];
    extern int max_restore_hits;
    extern int max_restore_mana;
    static char str_joinwar[11] = "do_joinwar";

    if (IS_NPC(ch))
	return;

    if ( ch->pcdata->snoop_war == war_number )
    {
	send_to_char("You can't join the current war if you've used certain immortal commands.\n\r", ch );
        send_to_char( "Use the ffa command to join the free for all.\n\r", ch );
	return;
    }

    if ( in_static_war( ch ) )
    {
	send_to_char("You are already in a war silly.\n\r", ch );
	return;
    }

    if (ch->level != 30)
    {
	send_to_char("You don't belong in the war.\n\r", ch );
	return;
    }

    if (war_status != WAR_ON)
    {
	send_to_char("There is no war on!\n\r", ch );
	return;
    }

    if ( max_restore_hits <= 1 )
    {
	send_to_char("Someone has taken too much damage for new players to join.\n\r", ch );
        send_to_char( "Use the ffa command to join the free for all.\n\r", ch );
	return;
    }

    if ( war_status == WAR_ON )
    {
	sprintf( buf, "++    %s has enterred the war.    ++", ch->name );
	do_warmessage( NULL, buf );
    }

    if( IS_SET(war_flags, WAR_FLAG_RANDOMCLASS )
    || IS_SET( ch->act, PLR_RANDOM_CLASS ) )
      select_random_class( ch );

    while ( ch->affected )
      affect_remove( ch, ch->affected );


    teleport_char( ch, TRUE );
    ch->pcdata->war_num = WAR_NUM_MAIN;
    ch->pcdata->potential_dr += ch->hit;

    SET_BIT( ch->act, PLR_CAN_TELEPORT );

    if( ch->char_class != CLASS_MONK )
    {
	get_start_eq( ch );
    }
    else
    {
	OBJ_DATA *obj;
	extern int making_corpse;
	making_corpse = TRUE;
	while ( ch->carrying )
	  extract_obj( ch->carrying, str_joinwar );
	making_corpse = FALSE;
	obj = create_object( get_obj_index(3051) );
	obj_to_char( obj, ch );
    }

    ch->gold = 0;

    set_class_profile( ch );

/* partial restore */
    ch->hit  = UMAX( 1, ch->max_hit *  max_restore_hits / 100 );
    ch->mana = UMAX( 1, ch->max_mana *  max_restore_mana / 100 );

    if( !IS_SET( war_flags, WAR_FLAG_CONTINUOUS ) )
    {
        if ( war_timer < 5 )
	    war_timer = 5;
	else
	    war_timer++;
    }
    war_max++;

    return;
}

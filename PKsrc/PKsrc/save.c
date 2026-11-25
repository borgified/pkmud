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
#include "color.h"
#include "cmod.h"

#if !defined(macintosh)
extern	int	_filbuf		args( (FILE *) );
#endif



/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];



/*
 * Local functions.
 */
void	fwrite_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fwrite_obj	args( ( CHAR_DATA *ch,  OBJ_DATA  *obj,
			    FILE *fp, int iNest ) );
void	fread_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fread_obj	args( ( CHAR_DATA *ch,  FILE *fp ) );



/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch )
{
    char strsave[MAX_INPUT_LENGTH];
    FILE *fp;

    if ( IS_NPC(ch)
      || ch->pcdata->kills < 2 )
	return;

    if ( ch->desc != NULL && ch->desc->original != NULL )
	ch = ch->desc->original;

    ch->save_time = current_time;
    fclose( fpReserve );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
    if ( ( fp = fopen( strsave, "w" ) ) == NULL )
    {
	bug( "Save_char_obj: fopen", 0 );
	perror( strsave );
    }
    else
    {
	fwrite_char( ch, fp );
	if ( ch->carrying != NULL )
	    fwrite_obj( ch, ch->carrying, fp, 0 );
	fprintf( fp, "#END\n" );
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
  int i;

  fprintf( fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER"		);

  fprintf( fp, "Name         %s~\n",	ch->name		);
  if ( !IS_NPC( ch ) )
    fprintf( fp, "Site         %s~\n", ch->pcdata->site	);
  if ( !IS_NPC( ch ) )
    fprintf( fp, "Created      %s~\n", ch->pcdata->createsite	);
  if ( IS_NPC( ch ) )
    fprintf( fp, "ShortDescr   %s~\n",	ch->short_descr		);
  if ( IS_NPC( ch ) )
    fprintf( fp, "LongDescr    %s~\n",	ch->long_descr		);
  if ( !IS_NPC( ch ) )
    fprintf( fp, "Description	%s~\n",	ch->description		);
  fprintf( fp, "Nojoin       %d\n",   ch->nojoin		);
  fprintf( fp, "Lastwar      %d\n",	(int) ch->pcdata->last_war	);
  fprintf( fp, "Lastsnoop    %d\n",	(int) ch->pcdata->snoop_war	);
  fprintf( fp, "Sex          %d\n",	ch->sex			);
  fprintf( fp, "Class        %d\n",	ch->char_class		);
  fprintf( fp, "Level        %d\n",	ch->level		);
  if ( !IS_NPC( ch ) && ch->move_lag != 0 )
    fprintf( fp, "Move_lag    %d\n",	ch->move_lag		);
  fprintf( fp, "Trust        %d\n",	ch->trust		);
  fprintf( fp, "Played       %d\n",
    ch->played + (int) (current_time - ch->logon)		);
  fprintf( fp, "Exp          %d\n",	ch->exp			);
  fprintf( fp, "Act          %d\n",   ch->act			);
  fprintf( fp, "Deaf         %d\n",	ch->deaf		);

  if ( IS_NPC(ch) )
  {
    fprintf( fp, "Vnum         %d\n",	ch->pIndexData->vnum	);
  }
  else
  {
    fprintf( fp, "Password     %s~\n",	ch->pcdata->pwd		);
    fprintf( fp, "Bamfin       %s~\n",	ch->pcdata->bamfin	);
    fprintf( fp, "Bamfout      %s~\n",	ch->pcdata->bamfout	);
    fprintf( fp, "Title        %s~\n",	ch->pcdata->title	);
    fprintf( fp, "Rating       %d\n",	ch->pcdata->rating	);
    fprintf( fp, "Blicks       %d\n",	ch->pcdata->blicks	);
    fprintf( fp, "Blicked      %d\n",	ch->pcdata->blicked	);
    fprintf( fp, "VictRatings  %d %d\n",
      ch->pcdata->ratings_victims,
      ch->pcdata->ratings_killers );
    fprintf( fp, "NumRatings  %d %d\n",
      ch->pcdata->rated_kills,
      ch->pcdata->rated_deaths );
    fprintf( fp, "Stats        %d %d %d %d\n",
      ch->pcdata->kills,
      ch->pcdata->deaths,
      ch->pcdata->wins,
      ch->pcdata->doomsdays );

    for ( i = 0; i < MAX_CLASS; i++ )
    {
      if ( ch->pcdata->class_kills[i] != 0
      || ch->pcdata->class_deaths[i] != 0 )
        fprintf( fp, "Cstat  '%s' %d %d\n",
          class_table[i].class_name,
          ch->pcdata->class_kills[i], ch->pcdata->class_deaths[i] );
    }

    fprintf( fp, "TVKDs             %d\n",
      ch->pcdata->total_victim_kds );

    fprintf( fp, "DamageStats       %d %d %d\n",
      ch->pcdata->damage_inflicted,
      ch->pcdata->damage_received,
      ch->pcdata->potential_dr );
    fprintf( fp, "TwinkStats        %d %d\n",
      ch->pcdata->ld_at_end,
      ch->pcdata->mob_deaths );

    if ( ch->pcdata->color_exits > 0 && ch->pcdata->color_exits < MAX_COLOR )
      fprintf( fp, "ColorExits    %d\n", ch->pcdata->color_exits );
    if ( ch->pcdata->color_gossip > 0 && ch->pcdata->color_gossip < MAX_COLOR )
      fprintf( fp, "ColorGossip    %d\n", ch->pcdata->color_gossip );
    if ( ch->pcdata->color_prompt > 0 && ch->pcdata->color_prompt < MAX_COLOR )
      fprintf( fp, "ColorPrompt    %d\n", ch->pcdata->color_prompt );
    if ( ch->pcdata->color_room_name > 0 && ch->pcdata->color_room_name < MAX_COLOR )
      fprintf( fp, "ColorRoomName    %d\n", ch->pcdata->color_room_name );
    if ( ch->pcdata->color_say > 0 && ch->pcdata->color_say < MAX_COLOR )
      fprintf( fp, "ColorSay    %d\n", ch->pcdata->color_say );
    if ( ch->pcdata->color_tell > 0 && ch->pcdata->color_tell < MAX_COLOR )
      fprintf( fp, "ColorTell    %d\n", ch->pcdata->color_tell );
    if ( ch->pcdata->color_warmessage > 0 && ch->pcdata->color_warmessage < MAX_COLOR )
      fprintf( fp, "ColorWarmessage    %d\n", ch->pcdata->color_warmessage );
  }
  fprintf( fp, "End\n\n" );
  return;
}



/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest )
{
}



/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( DESCRIPTOR_DATA *d, char *name )
{
    static PC_DATA pcdata_zero;
    char strsave[MAX_INPUT_LENGTH];
    CHAR_DATA *ch;
    FILE *fp;
    bool found;

    if ( char_free == NULL )
    {
	ch				= (CHAR_DATA *) alloc_perm( sizeof(*ch) );
    }
    else
    {
	ch				= char_free;
	char_free			= char_free->next;
    }
    clear_char( ch );

    if ( pcdata_free == NULL )
    {
	ch->pcdata			= (PC_DATA *) alloc_perm( sizeof(*ch->pcdata) );
    }
    else
    {
	ch->pcdata			= pcdata_free;
	pcdata_free			= pcdata_free->next;
    }
    *ch->pcdata				= pcdata_zero;

    d->character			= ch;
    ch->desc				= d;
    ch->name				= str_dup( name );
    ch->act				= PLR_BLANK
					| PLR_COMBINE
					| PLR_PROMPT;
    ch->pcdata->site			= str_dup( "" );
    ch->pcdata->createsite		= str_dup( "" );
    ch->pcdata->pwd			= str_dup( "" );
    ch->pcdata->bamfin			= str_dup( "" );
    ch->pcdata->bamfout			= str_dup( "" );
    ch->pcdata->title			= str_dup( "" );
    ch->pcdata->perm_str		= 13;
    ch->pcdata->perm_int		= 13; 
    ch->pcdata->perm_wis		= 13;
    ch->pcdata->perm_dex		= 13;
    ch->pcdata->perm_con		= 13;
    ch->pcdata->kills	= 0;
    ch->pcdata->deaths	= 0;
    ch->pcdata->wins	= 0;
    ch->pcdata->damage_inflicted	= 0;
    ch->pcdata->damage_received	= 0;
    ch->pcdata->potential_dr	= 0;
    ch->pcdata->total_victim_kds	= 0;
    ch->pcdata->rating = 1000;
    ch->pcdata->color_gossip = -1;
    ch->pcdata->color_warmessage = -1;
    ch->pcdata->color_prompt = -1;
    ch->pcdata->color_exits = -1;
    ch->pcdata->color_room_name = -1;
    ch->pcdata->color_tell = -1;
    ch->pcdata->color_say = -1;

    found = FALSE;
    fclose( fpReserve );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_char_obj: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if      ( !str_cmp( word, "PLAYER" ) ) fread_char ( ch, fp );
	    else if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( ch, fp );
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	}
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return found;
}



/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

void fread_char( CHAR_DATA *ch, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    bool ignore_class_stats = FALSE;
   

    for ( ; ; )
    {
        if ( feof( fp ) )
          return;

	word   = fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Act",		ch->act,		fread_number( fp ) );
	    KEY( "AffectedBy",	ch->affected_by,	fread_number( fp ) );
	    KEY( "Alignment",	ch->alignment,		fread_number( fp ) );
	    KEY( "Armor",	ch->armor,		fread_number( fp ) );

	    if ( !str_cmp( word, "Affect" ) || !str_cmp( word, "AffectData" ) )
	    {
		AFFECT_DATA *paf;

		if ( affect_free == NULL )
		{
		    paf		= (AFFECT_DATA *) alloc_perm( sizeof(*paf) );
		}
		else
		{
		    paf		= affect_free;
		    affect_free	= affect_free->next;
		}

		if ( !str_cmp( word, "Affect" ) )
		{
		    /* Obsolete 2.0 form. */
		    paf->type	= fread_number( fp );
		}
		else
		{
		    int sn;

		    sn = skill_lookup( fread_word( fp ) );
		    if ( sn < 0 )
			bug( "Fread_char: unknown skill.", 0 );
		    else
			paf->type = sn;
		}

		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "AttrMod"  ) )
	    {
		ch->pcdata->mod_str  = fread_number( fp );
		ch->pcdata->mod_int  = fread_number( fp );
		ch->pcdata->mod_wis  = fread_number( fp );
		ch->pcdata->mod_dex  = fread_number( fp );
		ch->pcdata->mod_con  = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "AttrPerm" ) )
	    {
		ch->pcdata->perm_str = fread_number( fp );
		ch->pcdata->perm_int = fread_number( fp );
		ch->pcdata->perm_wis = fread_number( fp );
		ch->pcdata->perm_dex = fread_number( fp );
		ch->pcdata->perm_con = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'B':
	    KEY( "Blicks",	ch->pcdata->blicks,	fread_number( fp ) );
	    KEY( "Blicked",	ch->pcdata->blicked,	fread_number( fp ) );
	    KEY( "Bamfin",	ch->pcdata->bamfin,	fread_string( fp ) );
	    KEY( "Bamfout",	ch->pcdata->bamfout,	fread_string( fp ) );
	    break;

	case 'C':
	    KEY( "Class",	ch->char_class,		fread_number( fp ) );

	    KEY( "ColorExits",  ch->pcdata->color_exits, fread_number( fp ) );
	    KEY( "ColorGossip",  ch->pcdata->color_gossip, fread_number( fp ) );
	    KEY( "ColorPrompt",  ch->pcdata->color_prompt, fread_number( fp ) );
	    KEY( "ColorRoomName",  ch->pcdata->color_room_name, fread_number( fp ) );
	    KEY( "ColorSay",  ch->pcdata->color_say, fread_number( fp ) );
	    KEY( "ColorTell",  ch->pcdata->color_tell, fread_number( fp ) );
	    KEY( "ColorWarmessage",  ch->pcdata->color_warmessage, fread_number( fp ) );

            KEY( "Created",     ch->pcdata->createsite, fread_string( fp ) );

	    if ( !str_cmp( word, "Condition" ) )
	    {
		ignore_class_stats = TRUE;
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }

            if ( !str_cmp( word, "Cstat" ) )
            {
              int iClass = class_lookup( fread_word( fp ) );
              if ( iClass != -1 )
              {
                ch->pcdata->class_kills[iClass] = fread_number( fp );
                ch->pcdata->class_deaths[iClass] = fread_number( fp );
              }
              else
              {
                fread_number( fp );
                fread_number( fp );
              }
	      fMatch = TRUE;
	      break;
	    }
	    break;

	case 'D':
	    KEY( "Damroll",	ch->damroll,		fread_number( fp ) );
	    KEY( "Deaf",	ch->deaf,		fread_number( fp ) );
	    KEY( "Description",	ch->description,	fread_string( fp ) );
	    if ( !str_cmp( word, "DamageStats" ) )
	    {
		ch->pcdata->damage_inflicted = fread_number( fp );
		ch->pcdata->damage_received = fread_number( fp );
		ch->pcdata->potential_dr = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "DamageInflicted" ) )
	    {
		if ( ignore_class_stats == TRUE )
		    fread_to_eol( fp );
		else
		    ch->pcdata->damage_inflicted = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "DamageReceived" ) )
	    {
		if ( ignore_class_stats == TRUE )
		    fread_to_eol( fp );
		else
		    ch->pcdata->damage_received = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
		return;
	    KEY( "Exp",		ch->exp,		fread_number( fp ) );
	    break;

	case 'G':
	    KEY( "Gold",	ch->gold,		fread_number( fp ) );
	    break;

	case 'H':
	    KEY( "Hitroll",	ch->hitroll,		fread_number( fp ) );
	    break;

	case 'M':
	    KEY( "Move_lag",	ch->move_lag,		fread_number( fp ) );
	    break;

	case 'L':
	    KEY( "Lastsnoop",	ch->pcdata->snoop_war,	fread_number( fp ) );
	    KEY( "Lastwar",	ch->pcdata->last_war,	fread_number( fp ) );
	    KEY( "Level",	ch->level,		fread_number( fp ) );
	    KEY( "LongDescr",	ch->long_descr,		fread_string( fp ) );
	    break;

	case 'N':
	    if ( !str_cmp( word, "NumRatings" ) )
	    {
		ch->pcdata->rated_kills = fread_number( fp );
		ch->pcdata->rated_deaths = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    KEY( "Nojoin",	ch->nojoin,		fread_number( fp ) );
	    if ( !str_cmp( word, "Name" ) )
	    {
		/*
		 * Name already set externally.
		 */
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'P':
	    KEY( "Password",	ch->pcdata->pwd,	fread_string( fp ) );
	    KEY( "Played",	ch->played,		fread_number( fp ) );
	    KEY( "Position",	ch->position,		fread_number( fp ) );
	    KEY( "Practice",	ch->practice,		fread_number( fp ) );
	    break;

	case 'R':
	    KEY( "Race",        ch->race,		fread_number( fp ) );
	    KEY( "Rating",	ch->pcdata->rating,	fread_number( fp ) );

	    if ( !str_cmp( word, "Room" ) )
	    {
		ch->in_room = get_room_index( fread_number( fp ) );
		if ( ch->in_room == NULL )
		    ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'S':
	    KEY( "SavingThrow",	ch->saving_throw,	fread_number( fp ) );
	    KEY( "Sex",		ch->sex,		fread_number( fp ) );
	    KEY( "ShortDescr",	ch->short_descr,	fread_string( fp ) );

	    if ( !str_cmp( word, "Site" ) )
	    {
		fread_string( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Skill" ) )
	    {
		int sn;
		int value;

		value = fread_number( fp );
		sn    = skill_lookup( fread_word( fp ) );
		if ( sn < 0 )
		    bug( "Fread_char: unknown skill.", 0 );
		else
		    ch->pcdata->learned[sn] = value;
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Stats" ) )
	    {
		ch->pcdata->kills = fread_number( fp );
		ch->pcdata->deaths = fread_number( fp );
		ch->pcdata->wins = fread_number( fp );
		ch->pcdata->doomsdays = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'T':
	    KEY( "Trust",	ch->trust,		fread_number( fp ) );
	    KEY( "TVKDs",	ch->pcdata->total_victim_kds,fread_number( fp ) );
	    if ( !str_cmp( word, "TwinkStats" ) )
	    {
		ch->pcdata->ld_at_end = fread_number( fp );
		ch->pcdata->mob_deaths = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Title" ) )
	    {
		ch->pcdata->title = fread_string( fp );
		if ( isalpha(ch->pcdata->title[0])
		||   isdigit(ch->pcdata->title[0]) )
		{
		    sprintf( buf, " %s", ch->pcdata->title );
		    free_string( ch->pcdata->title );
		    ch->pcdata->title = str_dup( buf );
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'V':
	    if ( !str_cmp( word, "VictRatings" ) )
	    {
		ch->pcdata->ratings_victims = fread_number( fp );
		ch->pcdata->ratings_killers = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Vnum" ) )
	    {
		ch->pIndexData = get_mob_index( fread_number( fp ) );
		fMatch = TRUE;
		break;
	    }
	    break;
	}

	if ( !fMatch )
	{
	    sprintf( buf, "Fread char: no match, %s.", word );
	    bug( buf, 0 );
	    fread_to_eol( fp );
	}
    }
}



void fread_obj( CHAR_DATA *ch, FILE *fp )
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;
    char *word;
    int iNest;
    int done = FALSE;
    bool fMatch;
    bool fNest;
    bool fVnum;

    if ( obj_free == NULL )
    {
	obj		= (OBJ_DATA *) alloc_perm( sizeof(*obj) );
    }
    else
    {
	obj		= obj_free;
	obj_free	= obj_free->next;
    }

    *obj		= obj_zero;
    obj->name		= str_dup( "" );
    obj->short_descr	= str_dup( "" );
    obj->description	= str_dup( "" );

    fNest		= FALSE;
    fVnum		= TRUE;
    iNest		= 0;

    while ( !done )
    {
        if ( feof( fp ) )
          break;

	word   = fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    if ( !str_cmp( word, "Affect" ) || !str_cmp( word, "AffectData" ) )
	    {
		AFFECT_DATA *paf;

		if ( affect_free == NULL )
		{
		    paf		= (AFFECT_DATA *) alloc_perm( sizeof(*paf) );
		}
		else
		{
		    paf		= affect_free;
		    affect_free	= affect_free->next;
		}

		if ( !str_cmp( word, "Affect" ) )
		{
		    /* Obsolete 2.0 form. */
		    paf->type	= fread_number( fp );
		}
		else
		{
		    int sn;

		    sn = skill_lookup( fread_word( fp ) );
		    if ( sn < 0 )
			bug( "Fread_obj: unknown skill.", 0 );
		    else
			paf->type = sn;
		}

		paf->type	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->next	= obj->affected;
		obj->affected	= paf;
		fMatch		= TRUE;
		break;
	    }
	    break;

	case 'C':
	    KEY( "Cost",	obj->cost,		fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Description",	obj->description,	fread_string( fp ) );
	    break;

	case 'E':
	    KEY( "ExtraFlags",	obj->extra_flags,	fread_number( fp ) );

	    if ( !str_cmp( word, "ExtraDescr" ) )
	    {
		EXTRA_DESCR_DATA *ed;

		if ( extra_descr_free == NULL )
		{
		    ed			= (EXTRA_DESCR_DATA *) alloc_perm( sizeof(*ed) );
		}
		else
		{
		    ed			= extra_descr_free;
		    extra_descr_free	= extra_descr_free->next;
		}

		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= obj->extra_descr;
		obj->extra_descr	= ed;
		fMatch = TRUE;
	    }

	    if ( !str_cmp( word, "End" ) )
	    {
                done = TRUE;
	    }
	    break;

	case 'I':
	    KEY( "ItemType",	obj->item_type,		fread_number( fp ) );
	    break;

	case 'N':
	    KEY( "Name",	obj->name,		fread_string( fp ) );

	    if ( !str_cmp( word, "Nest" ) )
	    {
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
		    bug( "Fread_obj: bad nest %d.", iNest );
		}
		else
		{
		    rgObjNest[iNest] = obj;
		    fNest = TRUE;
		}
		fMatch = TRUE;
	    }
	    break;

	case 'S':
	    KEY( "ShortDescr",	obj->short_descr,	fread_string( fp ) );

	    if ( !str_cmp( word, "Spell" ) )
	    {
		int iValue;
		int sn;

		iValue = fread_number( fp );
		sn     = skill_lookup( fread_word( fp ) );
		if ( iValue < 0 || iValue > 3 )
		{
		    bug( "Fread_obj: bad iValue %d.", iValue );
		}
		else if ( sn < 0 )
		{
		    bug( "Fread_obj: unknown skill.", 0 );
		}
		else
		{
		    obj->value[iValue] = sn;
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'T':
	    KEY( "Timer",	obj->timer,		fread_number( fp ) );
	    break;

	case 'V':
	    if ( !str_cmp( word, "Values" ) )
	    {
		obj->value[0]	= fread_number( fp );
		obj->value[1]	= fread_number( fp );
		obj->value[2]	= fread_number( fp );
		obj->value[3]	= fread_number( fp );
		fMatch		= TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Vnum" ) )
	    {
		int vnum;

		vnum = fread_number( fp );
		if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL )
		    bug( "Fread_obj: bad vnum %d.", vnum );
		else
		    fVnum = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "WearFlags",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "WearLoc",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Weight",	obj->weight,		fread_number( fp ) );
	    break;

	}

	if ( !fMatch )
	{
	    bug( "Fread_obj: no match.", 0 );
	    fread_to_eol( fp );
	}
    }

  if ( !fNest || !fVnum )
  {
    bug( "Fread_obj: incomplete object.", 0 );
    free_string( obj->name        );
    free_string( obj->description );
    free_string( obj->short_descr );
    obj->next = obj_free;
    obj_free  = obj;
  }
  else
  {
    obj->next	= object_list;
    object_list	= obj;
    obj->pIndexData->count++;
    if ( iNest == 0 || rgObjNest[iNest] == NULL )
      obj_to_char( obj, ch );
    else
      obj_to_obj( obj, rgObjNest[iNest-1] );
  }
}

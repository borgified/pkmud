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
#include "merc.h"


/*
 * Local functions.
 */
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	note_attach	args( ( CHAR_DATA *ch ) );
void	note_remove	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );



bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( ch->level >= 40)
	return TRUE;

    if ( !str_cmp( ch->name, pnote->sender ) )
	return TRUE;

    if ( is_name( "all", pnote->to_list ) )
	return TRUE;

    if ( IS_HERO(ch) && is_name( "immortal", pnote->to_list ) )
	return TRUE;

    if ( is_name( ch->name, pnote->to_list ) )
	return TRUE;

    return FALSE;
}



void note_attach( CHAR_DATA *ch )
{
    NOTE_DATA *pnote;

    if ( ch->pnote != NULL )
	return;

    if ( note_free == NULL )
    {
	pnote	  = (NOTE_DATA *) alloc_perm( sizeof(*ch->pnote) );
    }
    else
    {
	pnote	  = note_free;
	note_free = note_free->next;
    }

    pnote->next		= NULL;
    pnote->sender	= str_dup( ch->name );
    pnote->date		= str_dup( "" );
    pnote->to_list	= str_dup( "" );
    pnote->subject	= str_dup( "" );
    pnote->text		= str_dup( "" );
    ch->pnote		= pnote;
    return;
}



void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    char to_new[MAX_INPUT_LENGTH];
    char to_one[MAX_INPUT_LENGTH];
    FILE *fp;
    NOTE_DATA *prev;
    char *to_list;

    /*
     * Build a new to_list.
     * Strip out this recipient.
     */
    to_new[0]	= '\0';
    to_list	= pnote->to_list;
    while ( *to_list != '\0' )
    {
	to_list	= one_argument( to_list, to_one );
	if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
	{
	    strcat( to_new, " " );
	    strcat( to_new, to_one );
	}
    }

    /*
     * Just a simple recipient removal?
     */
 if (ch->level < 40)
  if (!((!str_cmp( "all", pnote->to_list) || (pnote->to_list == NULL) || !str_cmp( "immortal", pnote->to_list)) && ch->level >= 40 ))
    if ( str_cmp( ch->name, pnote->sender ) && to_new[0] != '\0' )
    {
	free_string( pnote->to_list );
	pnote->to_list = str_dup( to_new + 1 );
	return;
    }

    /*
     * Remove note from linked list.
     */
    if ( pnote == note_list )
    {
	note_list = pnote->next;
    }
    else
    {
	for ( prev = note_list; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == pnote )
		break;
	}

	if ( prev == NULL )
	{
	    bug( "Note_remove: pnote not found.", 0 );
	    return;
	}

	prev->next = pnote->next;
    }

    free_string( pnote->text    );
    free_string( pnote->subject );
    free_string( pnote->to_list );
    free_string( pnote->date    );
    free_string( pnote->sender  );
    pnote->next	= note_free;
    note_free	= pnote;

    /*
     * Rewrite entire list.
     */
    fclose( fpReserve );
    if ( ( fp = fopen( NOTE_FILE, "w" ) ) == NULL )
    {
	perror( NOTE_FILE );
    }
    else
    {
	for ( pnote = note_list; pnote != NULL; pnote = pnote->next )
	{
	    fprintf( fp, "Sender  %s~\nDate    %s~\nTo      %s~\nSubject %s~\nText\n%s~\n\n",
		pnote->sender,
		pnote->date,
		pnote->to_list,
		pnote->subject,
		pnote->text
		);
	}
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_quietmode( CHAR_DATA *ch, char*argument)
{
	if( !IS_SET( ch->act, PLR_QUIETMODE ) )
	{
		send_to_char("Quietmode on.  You will not receive tells.\n\r", ch );
		SET_BIT( ch->act, PLR_QUIETMODE );
		return;
	}
	else
	{
		send_to_char("You are receiving tells again.\n\r", ch );
		REMOVE_BIT( ch->act, PLR_QUIETMODE );
		return;
	}
	return;
}

void do_nmodify( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *oldb;
    char arg[MAX_INPUT_LENGTH];
    char word[MAX_INPUT_LENGTH];
    int spaces = 0;
    int ii = 0;
    int inew;
    int iline;
    int iw;
    int l_count;

    if ( IS_NPC( ch ) )
	return;

    if ( ch->pnote == NULL || ch->pnote->text[0] == '\0' )
    {
	send_to_char("Nothing in your note buffer to modify.\n\r", ch );
	return;
    }

    oldb = ch->pnote->text;
    argument = one_argument( argument, arg );

    if ( !str_cmp( arg, "format" ) )
    {
	iline = atoi( argument );

	strcpy( buf, oldb );

	inew = 1;
      if ( iline > 1 )
	while ( 1 )
	{
	    ii++;

	    if ( oldb[ii] == '\0' )
		break;

	    if ( oldb[ii] != '\n' )
		continue;

	    ii+=2;
	    inew++;

	    if ( iline == inew )
		break;
	}

	if ( inew < iline )
	{
	    send_to_char("Not that many lines in buffer.\n\r", ch );
	    return;
	}

	buf[ii] = '\0';
	oldb = &oldb[ii];
	word[0] = '\0';
	l_count = 1;

	while( 1 )
	{
	    /* set word length to 0 */
	    iw = 0;

	    /* find beginning of next word */
	    while ( oldb[0] == ' ' || oldb[0] == '\n' || oldb[0] == '\r' )
	    {
		oldb++;
	    }

	    /* if last word, exit */
	    if ( oldb[0] == '\0' )
	    {
		break;
	    }

	    /* if it's a tab, stick in a tab */
	    if ( oldb[0] == '	' /* tab */ )
	    {
		if ( inew != iline )
		    strcat( buf, "\n\r" );
		strcat( buf, "	" );
		l_count = 8;
		oldb++;
		continue;
	    }

	    /* find end of next word */
	    while ( oldb[0] != ' ' && oldb[0] != '\n' )
	    {
		word[iw] = oldb[0];
		iw++;
		oldb++;
	    }

	    /* terminate word */
	    word[iw] = '\0';

	    /* exit if at end */
	    if ( oldb[0] == '\n' )
	    {
		oldb++;
		if ( oldb[0] == '\0' )
		    break;
	    }

	    /* if length is longer than line, new line, otherwise, add word */
	    if ( (l_count + iw) > 72 )
	    {
		inew++;
		strcat( buf, "\n\r" );
		strcat( buf, word );
		l_count = iw;
	    }
	    else
	    {
		if ( spaces > 0 )
		    strcat( buf, " ");
		if ( spaces > 1 )
		    strcat( buf, " ");
		strcat( buf, word );
		l_count += spaces + iw;
	    }

	    /* set # of spaces.  2 if '.', 1 otherwise */
	    if ( word[iw-1] == '.' )
		spaces = 2;
	    else
		spaces = 1;
	}

	strcat( buf, "\n\r\0" );
	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char("Reformatted.\n\r", ch );
	send_to_char( buf, ch );
	return;
    }


    if ( !str_cmp( arg, "sentence" ) || !str_cmp( arg, "sent" ) )
    {
	iline = atoi( argument );

	strcpy( buf, oldb );

	inew = 1;
      if ( iline > 1 )
	while ( 1 )
	{
	    ii++;

	    if ( oldb[ii] == '\0' )
		break;

	    if ( oldb[ii] != '\n' )
		continue;

	    ii+=2;
	    inew++;

	    if ( iline == inew )
		break;
	}

	if ( inew < iline )
	{
	    send_to_char("Not that many lines in buffer.\n\r", ch );
	    return;
	}

	buf[ii] = '\0';
	oldb = &oldb[ii-1];
	ii--;

	while( 1 )
	{
	    oldb++;
	    ii++;

	    /* if last word, exit */
	    if ( oldb[0] == '\0' )
	    {
		buf[ii] = '\0';
		break;
	    }

	    /* dont transfer cr, lf characters */
	    if ( oldb[0] == '\n' || oldb[0] == '\r' )
	    {
		ii--;
		continue;
	    }

	    buf[ii] = oldb[0];
	    if ( oldb[0] == '.' )
	    {
		ii++;  buf[ii] = '\n';
		ii++;  buf[ii] = '\r';
	    }
	}

	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char("Text broken into sentences..\n\r", ch );
	send_to_char( buf, ch );
	return;
    }


    if ( !str_cmp( arg, "delete" ) )
    {
	iline = atoi( argument );

	strcpy( buf, oldb );

	inew = 1;
      if ( iline > 1 )
	while ( 1 )
	{
	    ii++;

	    if ( oldb[ii] == '\0' )
		break;

	    if ( oldb[ii] != '\n' )
		continue;

	    ii+=2;
	    inew++;

	    if ( iline == inew )
		break;
	}

	if ( inew < iline || oldb[ii] == '\0' )
	{
	    send_to_char("Not that many lines in buffer.\n\r", ch );
	    return;
	}

	buf[ii] = '\0';
	oldb = &oldb[ii-1];

	/* find end of next line */
	while( oldb[0] != '\n' )
	{
	    oldb++;
	}

	/* skip \n and \r */
	strcat( buf, &oldb[2] );
	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char("Line Deleted.\n\r", ch );
	send_to_char( buf, ch );
	return;
    }

    if ( !str_cmp( arg, "insert" ) )
    {
	char arg2[MAX_INPUT_LENGTH];

	argument = one_argument( argument, arg2 );

	iline = atoi( arg2 );

	strcpy( buf, oldb );

	iline += 1;

	inew = 1;
      if ( iline > 1 )
	while ( 1 )
	{
	    ii++;

	    if ( oldb[ii] == '\0' )
		break;

	    if ( oldb[ii] != '\n' )
		continue;

	    ii+=2;
	    inew++;

	    if ( iline == inew )
		break;
	}

	if ( inew < iline )
	{
	    send_to_char("Not that many lines in buffer.\n\r", ch );
	    return;
	}

	buf[ii] = '\0';
	oldb = &oldb[ii];

	strcat( buf, argument );
	strcat( buf, "\n\r" );

	strcat( buf, oldb );
	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char("Line Inserted.\n\r", ch );
	send_to_char( buf, ch );
	return;
    }

    if ( !str_cmp( arg, "tab" ) )
    {
	iline = atoi( argument );

	strcpy( buf, oldb );

	inew = 1;
      if ( iline > 1 )
	while ( 1 )
	{
	    ii++;

	    if ( oldb[ii] == '\0' )
		break;

	    if ( oldb[ii] != '\n' )
		continue;

	    ii+=2;
	    inew++;

	    if ( iline == inew )
		break;
	}

	if ( iline < 1 )
	{
	    send_to_char("Must be positve integer.\n\r", ch );
	    return;
	}

	if ( inew < iline )
	{
	    send_to_char("Not that many lines in buffer.\n\r", ch );
	    return;
	}

	buf[ii] = '\0';
	oldb = &oldb[ii];

	strcat( buf, "	" /* tab */ );
	strcat( buf, oldb );

	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char("Tab Added.\n\r", ch );
	send_to_char( buf, ch );
	return;
    }

    if ( !str_cmp( arg, "untab" ) )
    {
	iline = atoi( argument );

	strcpy( buf, oldb );

	inew = 1;
      if ( iline > 1 )
	while ( 1 )
	{
	    ii++;

	    if ( oldb[ii] == '\0' )
		break;

	    if ( oldb[ii] != '\n' )
		continue;

	    ii+=2;
	    inew++;

	    if ( iline == inew )
		break;
	}

	if ( iline < 1 )
	{
	    send_to_char("Must be positve integer.\n\r", ch );
	    return;
	}

	if ( inew < iline )
	{
	    send_to_char("Not that many lines in buffer.\n\r", ch );
	    return;
	}

	buf[ii] = '\0';
	oldb = &oldb[ii];

	if ( oldb[0] == '	' /* tab */ )
	{
	    strcat( buf, &oldb[1] );
	    free_string( ch->pnote->text );
	    ch->pnote->text = str_dup( buf );
	    send_to_char("Tab deleted.\n\r", ch );
	}
	else
	{
	    send_to_char("No tab on that line to remove.\n\r", ch );
	}

	send_to_char( ch->pnote->text, ch );
	return;

    }

    send_to_char("Usage: nmod <format/sent/tab/untab/insert/delete> <line> <text>\n\r", ch );
    return;
}


void do_note( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    NOTE_DATA *pnote;
    int vnum;
    int anum;

    if ( IS_NPC(ch) )
	return;

    argument = one_argument( argument, arg );
    smash_tilde( argument );

    if ( !str_cmp( arg, "list" ) )
    {
	vnum = 0;
	for ( pnote = note_list; pnote != NULL; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) )
	    {
		sprintf( buf, "[%3d] %s: %s\n\r",
		    vnum, pnote->sender, pnote->subject );
		send_to_char( buf, ch );
		vnum++;
	    }
	}
	return;
    }

    if ( !str_cmp( arg, "read" ) )
    {
	bool fAll;

	if ( !str_cmp( argument, "all" ) )
	{
	    fAll = TRUE;
	    anum = 0;
	}
	else if ( is_number( argument ) )
	{
	    fAll = FALSE;
	    anum = atoi( argument );
	}
	else
	{
	    send_to_char( "Note read which number?\n\r", ch );
	    return;
	}

	vnum = 0;
	for ( pnote = note_list; pnote != NULL; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) && ( vnum++ == anum || fAll ) )
	    {
		sprintf( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
		    vnum - 1,
		    pnote->sender,
		    pnote->subject,
		    pnote->date,
		    pnote->to_list
		    );
		send_to_char( buf, ch );
		send_to_char( pnote->text, ch );
		return;
	    }
	}

	send_to_char( "No such note.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "+" ) )
    {
	note_attach( ch );
	strcpy( buf, ch->pnote->text );
	if ( strlen(buf) + strlen(argument) >= MAX_STRING_LENGTH - 4 )
	{
	    send_to_char( "Note too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "subject" ) )
    {
	if( argument[0] == '\0' )
	{
		send_to_char("What should the subject be?\n\r", ch );
		return;
	}
	note_attach( ch );
	free_string( ch->pnote->subject );
	ch->pnote->subject = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "to" ) )
    {
	note_attach( ch );
	free_string( ch->pnote->to_list );
	ch->pnote->to_list = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "clear" ) )
    {
	if ( ch->pnote != NULL )
	{
	    free_string( ch->pnote->text );
	    free_string( ch->pnote->subject );
	    free_string( ch->pnote->to_list );
	    free_string( ch->pnote->date );
	    free_string( ch->pnote->sender );
	    ch->pnote->next	= note_free;
	    note_free		= ch->pnote;
	    ch->pnote		= NULL;
	}

	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "show" ) )
    {
	if ( ch->pnote == NULL )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

	sprintf( buf, "%s: %s\n\rTo: %s\n\r",
	    ch->pnote->sender,
	    ch->pnote->subject,
	    ch->pnote->to_list
	    );
	send_to_char( buf, ch );
	send_to_char( ch->pnote->text, ch );
	return;
    }

    if ( !str_cmp( arg, "post" ) )
    {
	FILE *fp;
	char *strtime;
	BAN_DATA *pban;

	if ( ch->pcdata->kills < 2 && get_trust( ch ) < 36 )
	{
	    send_to_char("Due to idiots making new chars to post, you must have 2 kills to post.\n\r", ch );
	    return;
	}

	for ( pban = no_note_list; pban != NULL; pban = pban->next )
	{
	    if ( !str_suffix( pban->name, ch->pcdata->site ) )
	    {
		send_to_char("Your site may not post notes.\n\r", ch );
		return;
	    }
	}

	if ( !can_communicate( ch, TRUE ) )
	    return;

	if ( ch->pnote == NULL )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

	ch->pnote->next			= NULL;
	strtime				= ctime( &current_time );
	strtime[strlen(strtime)-1]	= '\0';
	ch->pnote->date			= str_dup( strtime );

	if ( note_list == NULL )
	{
	    note_list	= ch->pnote;
	}
	else
	{
	    for ( pnote = note_list; pnote->next != NULL; pnote = pnote->next )
		;
	    pnote->next	= ch->pnote;
	}
	pnote		= ch->pnote;
	ch->pnote	= NULL;

	fclose( fpReserve );
	if ( ( fp = fopen( NOTE_FILE, "a" ) ) == NULL )
	{
	    perror( NOTE_FILE );
	}
	else
	{
	    fprintf( fp, "Sender  %s~\nDate    %s~\nTo      %s~\nSubject %s~\nText\n%s~\n\n",
		pnote->sender,
		pnote->date,
		pnote->to_list,
		pnote->subject,
		pnote->text
		);
	    fclose( fp );
	}
	fpReserve = fopen( NULL_FILE, "r" );

	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "remove" ) )
    {
	if ( !is_number( argument ) )
	{
	    send_to_char( "Note remove which number?\n\r", ch );
	    return;
	}

	anum = atoi( argument );
	vnum = 0;
	for ( pnote = note_list; pnote != NULL; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) && vnum++ == anum )
	    {
		if ( is_name( "rip", pnote->to_list )
		&& str_cmp( ch->name, "rip" ) )
		{
		    send_to_char( "You can't remove that note.\n\r", ch );
		    return;
		}

		note_remove( ch, pnote );
		send_to_char( "Ok.\n\r", ch );
		return;
	    }
	}

	send_to_char( "No such note.\n\r", ch );
	return;
    }

    send_to_char( "Huh?  Type 'help note' for usage.\n\r", ch );
    return;
}



/*
 * Generic channel function.
 */
void talk_channel( CHAR_DATA *ch, char *argument, int channel, const char *verb )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int position;

    if ( !can_communicate( ch, TRUE ) && channel != CHANNEL_WARMESSAGE )
	return;

    if ( argument[0] == '\0' )
    {
	sprintf( buf, "%s what?\n\r", verb );
	buf[0] = UPPER(buf[0]);
	send_to_char( buf, ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE)
    && channel != CHANNEL_WARMESSAGE )
    {
	sprintf( buf, "You can't %s.\n\r", verb );
	send_to_char( buf, ch );
	return;
    }

    REMOVE_BIT(ch->deaf, channel);

    switch ( channel )
    {
    default:
	sprintf( buf, "You %s '%s'\n\r", verb, argument );
	send_to_char( buf, ch );
	sprintf( buf, "$n %ss '$t'",     verb );
	break;

    case CHANNEL_CHAT:
	sprintf( buf, "#GYou gossip '%s'#0\n\r", argument );
	send_to_char( buf, ch );
	sprintf( buf, "#G$n gossips '$t'#0" );
	break;
 
    case CHANNEL_WARMESSAGE:
	for( d = descriptor_list; d != NULL; d = d->next )
	{
		if( d->connected == CON_PLAYING
		&& !IS_NPC(d->character)
		&& !IS_SET(d->character->deaf, CHANNEL_WARMESSAGE ) )
		{
			sprintf(buf,"#W**** %s ****#0\n\r",argument );
			send_to_char( buf, d->character );
		}
	}
	return;

    case CHANNEL_DEATH:
	for ( d = descriptor_list ; d != NULL ; d = d->next )
	{
	 if ( d->connected == CON_PLAYING && !IS_NPC(d->character)
	 && !IS_SET(d->character->deaf, CHANNEL_DEATH))
	 {
	    sprintf( buf, "#h%s#0\n\r", argument );
	    send_to_char( buf, d->character );
	 }
	}
	return;
	break;

    case CHANNEL_IMMTALK:
	sprintf( buf, "$n: $t" );
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch,argument, NULL, TO_CHAR, TRUE );
	ch->position	= position;
	break;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( d->connected == CON_PLAYING
	&&   vch != ch
	&&  !IS_SET(vch->deaf, channel) )
	{
	    if ( channel == CHANNEL_IMMTALK && !IS_HERO(och) )
		continue;
	    if ( (channel == CHANNEL_YELL || channel == CHANNEL_SHOUT )
	    &&   vch->in_room->area != ch->in_room->area )
		continue;

	    position		= vch->position;
	    if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
		vch->position	= POS_STANDING;
	    act( buf, ch,argument, vch, TO_VICT, TRUE );
	    vch->position	= position;
	}
    }

    return;
}



void do_auction( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_AUCTION, "auction" );
    return;
}

void do_warmessage( CHAR_DATA *ch, char *argument )
{
  talk_channel( char_list, argument, CHANNEL_WARMESSAGE, "warmessage" );
  return;
}

void do_chat( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_CHAT, "gossip" );
    return;
}


/*
 * Alander's new channels.
 */
void do_music( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_MUSIC, "music" );
    return;
}



void do_question( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "question" );
    return;
}



void do_answer( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "answer" );
    return;
}



void do_shout( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_SHOUT, "shout" );
    WAIT_STATE( ch, 12 );
    return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_YELL, "yell" );
    return;
}

/** New death channel... Bruce was here */

void do_death( CHAR_DATA *ch, char *argument )
{
	talk_channel( ch, argument, CHANNEL_DEATH, "death" );
	return;
}

void do_info( CHAR_DATA *ch, char *argument )
{
  static char str_army[5] = "Army";

  if( ch == NULL )
  {
    for( ch = char_list; ch != NULL; ch = ch->next )
    {
      if( is_name( ch->name, str_army ) )
      {
        break;
      }
    }
  }
#warning This has seg fault potential.

  if ( !IS_NPC( ch ) && get_trust( ch ) < 39 )
  {
    send_to_char("Mortals can't use info channel.\n\r", ch );
    return;
  }
  talk_channel( ch, argument, CHANNEL_INFO, "info" );
  return;
}



void do_immtalk( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
    return;
}



void do_say( CHAR_DATA *ch, char *argument )
{
    if ( !can_communicate( ch, TRUE ) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }

    act( "#S$n says '$T'#0", ch, NULL, argument, TO_ROOM, TRUE );
    act( "#SYou say '$T'#0", ch, NULL, argument, TO_CHAR, TRUE );
    return;
}


void do_subwar( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_SUBWAR, "subwar" );
    return;
}



void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int position;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     * You can no longer tell to npcs for crash reasons
     */
    if ( ( ( victim = get_char_world( ch, arg ) ) == NULL )
    || IS_NPC(victim))
/*
    || ( IS_NPC(victim) && victim->in_room != ch->in_room ) )
*/
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    

    if ( mortal_in_main_war( victim )
    && get_trust( ch ) < LEVEL_IMMORTAL )
    {
	send_to_char( "You can't tell to them.\n\r", ch );
	return;
    }

    if ( !can_communicate( ch, TRUE )
    && !IS_IMMORTAL(ch)
    && !IS_IMMORTAL(victim) )
	return;

    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR, TRUE );
	return;
    }

    if( IS_SET(victim->act, PLR_QUIETMODE ) && get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char("They aren't receiving tells.\n\r", ch );
	return;
    }


    act( "#TYou tell $N '$t'.#0", ch, argument, victim, TO_CHAR, TRUE );
    position		= victim->position;
    victim->position	= POS_STANDING;
    act( "#T$n tells you '$t'.#0", ch,argument, victim, TO_VICT, TRUE );
    victim->position	= position;
    victim->reply	= ch;

    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int position;

    if( argument[0] == '\0' )
    {
	send_to_char("Reply what?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && (IS_SET(ch->act, PLR_SILENCE) || IS_SET(ch->act, PLR_NO_TELL)))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR, TRUE );
	return;
    }

    if ( !can_communicate( ch, TRUE )
    && !IS_IMMORTAL( ch )
    && !IS_IMMORTAL( victim ) )
	return;


    if ( IS_NPC( victim )
    || ( mortal_in_main_war( victim )
      && get_trust( ch ) < LEVEL_IMMORTAL ) )
    {
	send_to_char( " You can't tell to them!\n\r", ch );
	return;
    }

    if( IS_SET(victim->act, PLR_QUIETMODE ) )
    {
	send_to_char( "They are not receiving tells.\n\r", ch );
	return;
    }


    act( "#TYou reply to $N '$t'.#0", ch, argument, victim, TO_CHAR, TRUE );
    position		= victim->position;
    victim->position	= POS_STANDING;
    act( "#T$n replies to you '$t'.#0", ch, argument, victim, TO_VICT, TRUE );
    victim->position	= position;
    victim->reply	= ch;

    return;
}



void do_emote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *plast;

    if ( !can_communicate( ch, TRUE ) )
	return;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Emote what?\n\r", ch );
	return;
    }

    for ( plast = argument; *plast != '\0'; plast++ )
	;

    strcpy( buf, argument );
    if ( isalpha(plast[-1]) )
	strcat( buf, "." );

    act( "$n $T", ch, NULL, buf, TO_ROOM, TRUE );
    act( "$n $T", ch, NULL, buf, TO_CHAR, TRUE );
    return;
}




void do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_idea( CHAR_DATA *ch, char *argument )
{
    append_file( ch, IDEA_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}



void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}



void do_quit( CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA *d;
  extern int war_flags;
  extern void update_last_logoff( CHAR_DATA *ch );

  if ( IS_NPC(ch) )
    return;

  if (ch->fighting != NULL)
  {
    send_to_char( "No way! You are fighting.\n\r", ch );
    return;
  }
    
  if ( in_static_war( ch )
  && IS_SET( war_flags, WAR_FLAG_CONTINUOUS )  )
  {
    send_to_char("Get out.\n\r", ch );
    act( "$n has left the game.", ch, NULL, NULL, TO_ROOM, FALSE );
    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( log_buf );
    mudlog(log_buf, ch->level);

    update_last_logoff( ch );

    save_char_obj( ch );
    d = ch->desc;
    extract_char( ch, TRUE );

    if ( d != NULL )
      close_socket( d );

    return;
  }
  else
  {
    if( in_static_war( ch ) )
    {
      send_to_char("You must die first.\n\r", ch );
      return;
    }
  }


    if ( ch->position  < POS_STUNNED  )
    {
	send_to_char( "You're not DEAD yet.\n\r", ch );
	return;
    }

    send_to_char(  "Get out.\n\r", ch );
    act( "$n has left the game.", ch, NULL, NULL, TO_ROOM, FALSE );
    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( log_buf );
    mudlog(log_buf, ch->level );

    update_last_logoff( ch );

    /*
     * After extract_char the ch is no longer valid!
     */

    save_char_obj( ch );
    d = ch->desc;
    extract_char( ch, TRUE );

    if ( d != NULL )
	close_socket( d );

    return;
}



void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( ch->pcdata->kills < 2 )
    {
	send_to_char( "You must have at least 2 kills to save.\n\r", ch );
	return;
    }

    save_char_obj( ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL )
    {
	act( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR, TRUE );
	return;
    }

    if ( victim == ch )
    {
	if ( ch->master == NULL )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }
   
    if ( ch->master != NULL )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}



void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master != NULL )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    act( "$n now follows you.", ch, NULL, master, TO_VICT, FALSE );
    act( "You now follow $N.",  ch, NULL, master, TO_CHAR, TRUE );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( ch->master == NULL )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
    }

    act( "$n stops following you.",     ch, NULL, ch->master, TO_VICT, FALSE );
    act( "You stop following $N.",      ch, NULL, ch->master, TO_CHAR, TRUE  );

    ch->master = NULL;
    ch->leader = NULL;
    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master != NULL )
	stop_follower( ch );

    ch->leader = NULL;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = fch;
    }

    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;
    BAN_DATA *pban;

    if ( !can_communicate( ch, TRUE ) )
	return;

    for ( pban = no_note_list; pban != NULL; pban = pban->next )
    {
	if ( !str_suffix( pban->name, ch->pcdata->site ) )
	{
	    send_to_char("Your site may not use order.\n\r", ch );
	    return;
	}
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( !IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}
    }

    if (!str_prefix( argument, "quit" ) || !str_prefix( argument, "cast" ) )
    {
	send_to_char("You can't order that  >:)", ch );
	return;
    }

    found = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
	och_next = och->next_in_room;

	if ( IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	{
	    found = TRUE;
	    act( "$n orders you to '$t'.", ch, argument, och, TO_VICT, TRUE );
	    interpret( och, argument );
	    WAIT_STATE( ch, 12 );
	}
    }

    if ( found )
	send_to_char( "Ok.\n\r", ch );
    else
	send_to_char( "You have no followers here.\n\r", ch );
    return;
}



bool can_communicate( CHAR_DATA *ch, bool fEcho )
{
  extern int war_number;

  if ( IS_NPC( ch ) )
    return TRUE;

  if ( war_number == ch->pcdata->snoop_war )
  {
    if ( fEcho )
      send_to_char("You may not communicate in any way if you have used snoop during the current war.\n\r", ch );
    return FALSE;
  }

  return TRUE;
}

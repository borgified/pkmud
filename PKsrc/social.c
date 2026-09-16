/*  Social OLC created by Borlak */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"



struct social_olc socials[200];

int MAX_SOCIAL;

void read_socials( ) {

	extern int MAX_SOCIAL;
	char *word;
	FILE *fp;
	int social;

	if( ( fp = fopen( "socials.txt", "r" ) ) == NULL ) {

		bug( "Failed opening socials.txt", 0 );
		return;
	}

	for( social = 0; social <= 200; social++ ) {

		word = fread_word( fp );

		if( !str_cmp( word, "NAME" ) ) {

			free_string( socials[social].name	);
			free_string( socials[social].charnv	);
			free_string( socials[social].roomnv	);
			free_string( socials[social].charc	);
			free_string( socials[social].room	);
			free_string( socials[social].vict	);

			socials[social].name = fread_string( fp );
			socials[social].charnv = fread_string( fp );
			socials[social].roomnv = fread_string( fp );
			socials[social].charc  = fread_string( fp );
			socials[social].vict   = fread_string( fp );
			socials[social].room   = fread_string( fp );
			continue;
		}

		if( !str_cmp( word, "ENDFILE" ) )
			break;
	}

	MAX_SOCIAL = social;
			
	fclose( fp );
	return;
}


void do_soulwrite( CHAR_DATA *ch, char *argument ) {

	extern int MAX_SOCIAL;
	FILE *fp;
	int social = 0;
	char arg[MAX_STRING_LENGTH];

	if( ( fp = fopen( "soctempt.txt", "w" ) ) == NULL ) {

		send_to_char("Failed to open txt file.\n\r", ch );
		return;
	}

	if( IS_NPC(ch) ) return;

	if( ch->social.name == '\0' || ch->social.charnv == '\0' 
	||  ch->social.vict == '\0' || ch->social.roomnv == '\0'
	||  ch->social.room == '\0' || ch->social.charc  == '\0' ) {

		send_to_char("One or more of your social slots is empty. Cannot continue.\n\r", ch );
 		send_to_char("Please type 'soul check' to find the emtpy slot.\n\r", ch );
		return;
	}

	one_argument( argument, arg );

	for( social = 0; social < MAX_SOCIAL; social++ ) {

		if( !str_cmp( ch->social.name, socials[social].name ) 
		&& str_cmp( arg, "yes" ) ) {

			send_to_char( "Your social has already been created.\n\r", ch );
			send_to_char( "If you wish to write over it, type 'soulwrite yes'\n\r", ch );
			return;
		}
	}

	fprintf( fp, "NAME   %s~\n", ch->social.name	);
	fprintf( fp, "%s~\n", ch->social.charnv		);
	fprintf( fp, "%s~\n", ch->social.roomnv		);
	fprintf( fp, "%s~\n", ch->social.charc		);
	fprintf( fp, "%s~\n", ch->social.vict		);
	fprintf( fp, "%s~\n\n", ch->social.room		);

	for( social = 0; social < MAX_SOCIAL; social++ ) {

		if( !str_cmp( arg, "yes" ) && !str_cmp( ch->social.name, socials[social].name ) )
			continue;

		fprintf( fp, "NAME   %s~\n", socials[social].name	);
		fprintf( fp, "%s~\n", socials[social].charnv		);
		fprintf( fp, "%s~\n", socials[social].roomnv		);
		fprintf( fp, "%s~\n", socials[social].charc		);
		fprintf( fp, "%s~\n", socials[social].vict		);
		fprintf( fp, "%s~\n\n", socials[social].room		);
	}

	fprintf( fp, "ENDFILE\n" );
	fclose( fp );

	system( "cp soctempt.txt socials.txt" );
	system( "rm soctempt.txt" );
	
	send_to_char("Social added to file.  'soul read' to bring it into game.\n\r", ch );
	return;

}


void do_soul( CHAR_DATA *ch, char *argument ) {

	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	int x;
	bool found = FALSE;

	if ( IS_NPC(ch ) ) return;

	smash_tilde( argument );
	argument = one_argument( argument, arg1 );

        if( !str_cmp( arg1, "check" ) ) {

                sprintf( buf,            "NAME  : %s\n\r", ch->social.name   );
                sprintf( buf+strlen(buf),"CHARNV: %s\n\r", ch->social.charnv );
                sprintf( buf+strlen(buf),"ROOMNV: %s\n\r", ch->social.roomnv );
                sprintf( buf+strlen(buf),"CHAR  : %s\n\r", ch->social.charc  );
                sprintf( buf+strlen(buf),"VICT  : %s\n\r", ch->social.vict   );
                sprintf( buf+strlen(buf),"ROOM  : %s\n\r", ch->social.room   );
                send_to_char( buf, ch );
                return;
        }

	if( !str_cmp( arg1, "load" ) ) {

		send_to_char("Broken.\n\r", ch );
		return;

		one_argument( argument, arg2 );

		for( x = 0; x < MAX_SOCIAL; x++ ) {

			if( !str_cmp( arg2, socials[x].name ) )
				found = TRUE;
		}

		if( found == FALSE ) {

			send_to_char("That social doesn't exist!\n\r", ch );
			return;
		}

		free_string( ch->social.name	);
		free_string( ch->social.charc	);
		free_string( ch->social.charnv	);
		free_string( ch->social.roomnv	);
		free_string( ch->social.room	);
		free_string( ch->social.vict	);

		sprintf( ch->social.name,	socials[x].name		);
		sprintf( ch->social.charc, 	socials[x].charc	);
		sprintf( ch->social.roomnv,	socials[x].roomnv	);
		sprintf( ch->social.room,	socials[x].room		);
		sprintf( ch->social.vict,	socials[x].vict		);
		sprintf( ch->social.charnv,	socials[x].charnv	);

		send_to_char( "Ok.\n\r", ch );
		do_soul( ch, "check" );
		return;

	}

        if( !str_cmp( arg1, "read") ) {

                read_socials( );
		send_to_char("Socials brought into memory.\n\r", ch );
                return;
        }

	if( !str_cmp(arg1, "test" ) ) {

		one_argument( argument, arg2 );

		if( arg2[0] == '\0' || !arg2 ) {

			send_to_char( "You need to supply a victim.\n\r", ch );
			return;
		}

		if( ch->social.name == NULL || ch->social.roomnv == NULL
		||  ch->social.vict == NULL || ch->social.charnv == NULL
		||  ch->social.room == NULL || ch->social.charc  == NULL )
		{

			send_to_char("One or more of your socials are empty.\n\r", ch );
			return;
		}

		if( ( victim = get_char_phase( ch, arg2 ) ) == NULL ) {

			send_to_char( "They aren't here.\n\r", ch );
			return;
		}

		if( victim->in_room != ch->in_room ) {

			send_to_char( "They are not in the room.\n\r", ch );
			return;
		}

		send_to_char( "Without victim...\n\r\n\r", ch );
		act( ch->social.roomnv, ch, NULL, NULL, TO_ROOM, TRUE );
		act( ch->social.charnv, ch, NULL, NULL, TO_CHAR, TRUE );

		send_to_char( "Now with victim...\n\r\n\r", ch );
		act( ch->social.room, ch, NULL, victim, TO_NOTVICT, TRUE );
		act( ch->social.charc, ch, NULL, victim, TO_CHAR, TRUE );
		act( ch->social.vict, ch, NULL, victim, TO_VICT, TRUE );

		return;
	}

	strcpy( arg2, argument );

	if( arg1[0] == '\0' || arg2[0] == '\0' || !arg1 || !arg2 ) {

		send_to_char("Syntax: soul <name/charnv/roomnv/char/vict/room> <NULL/txt>\n\r", ch );
		send_to_char("      : soul <read/check|test/load> <who/social>\n\r", ch );
		return;
	}

	if( ch->level < 38 ) {

		send_to_char( "You cannot make socials without permission.\n\r", ch );
		return;
	}


	if( strlen( arg2 ) > 70 ) {

		send_to_char( "Max length of 70 chars per line.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "name" ) && arg2[0] != '\0' ) {

		if( strlen( arg2 ) > 11 ) {

			send_to_char("Name of social cannot exceed 11 characters.\n\r",ch );
			return;
		}

		free_string( ch->social.name );
		ch->social.name = str_dup( arg2 );
		sprintf( buf, "Socials name: %s\n\r", arg2 );
		send_to_char( buf, ch );
		return;
	}

	if( !str_cmp( arg1, "charnv") ) {

		free_string( ch->social.charnv );
		ch->social.charnv = str_dup( arg2 );
		sprintf( buf, "Socials character with no victims:\n\r%s\n\r", arg2 );
		send_to_char( buf, ch );
		return;
	}

	if( !str_cmp( arg1, "roomnv") ) {

		free_string( ch->social.roomnv );
		ch->social.roomnv = str_dup( arg2 );
		sprintf( buf, "Socials room with no victims:\n\r%s\n\r", arg2 );
		send_to_char( buf, ch );
		return;
	}

	if( !str_cmp( arg1, "char" ) ) {

		free_string( ch->social.charc );
		ch->social.charc = str_dup( arg2 );
		sprintf( buf, "Socials message to character with victim:\n\r%s\n\r", arg2 );
		send_to_char( buf, ch );
		return;
	}

	if( !str_cmp( arg1, "vict" ) ) {

		free_string( ch->social.vict );
		ch->social.vict = str_dup( arg2 );
		sprintf( buf, "Socials message to victim from character:\n\r%s\n\r", arg2 );
		send_to_char( buf, ch );
		return;
	}

	if( !str_cmp( arg1, "room" ) ) {

		free_string( ch->social.room );
		ch->social.room = str_dup( arg2 );
		sprintf( buf, "Socials message to room with a victim from character:\n\r%s\n\r", arg2 );
		send_to_char( buf, ch );
		return;
	}

	send_to_char( "Invalid perimeter.\n\r", ch );
	return;
}




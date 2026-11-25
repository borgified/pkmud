/*
 * color.c
 *
 * (C) 1999 by Rip (Robert L. Peckham - peckham@robpeckham.net)
 *
 * Instructions:  add color.c to your makefile.
 * add color.h to your comm.c includes.
 * add a call to process_color to write_to_descriptor.  create a char
 *  pointer to receive the return value, and propigate this
 *  through write_to_descriptor.
 * add do_color to your interp list and merc.h.
 * add appropriate variables to your pcdata structures, and
 * modify the channel names for values in your game.
 * add appropriate lines to save.c for saving color choices
 * add appropriate filters on IO, both from files and player
 *  input to make it act as desired.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "merc.h"
#include "color.h"

const struct color_info color_data[MAX_COLOR] =
{
  { "black", C_BLACK },
  { "red", C_RED },
  { "green", C_GREEN },
  { "yellow", C_YELLOW },
  { "blue", C_BLUE },
  { "magenta", C_MAGENTA },
  { "cyan", C_CYAN },
  { "white", C_WHITE },
  { "blackbold", C_B_BLACK },
  { "redbold", C_B_RED },
  { "greenbold", C_B_GREEN },
  { "yellowbold", C_B_YELLOW },
  { "bluebold", C_B_BLUE },
  { "magentabold", C_B_MAGENTA },
  { "cyanbold", C_B_CYAN },
  { "whitebold", C_B_WHITE },
  { "bold", ANSI_BOLD },
  { "blink", ANSI_BLINK },
  { "underscore", ANSI_UNDERSCORE },
  { "reverse", ANSI_REVERSE }
};



char *process_color( int desc, char *txt, int length )
{
  bool fANSI = FALSE;
  bool fUseANSI = FALSE;
  bool fSharp = FALSE;
  static char buf[MAX_STRING_LENGTH*2];
  const char *p_in = txt;
  char *p_out = buf;
  int i = 0;
  PC_DATA *pcd;
  DESCRIPTOR_DATA *d;

  for ( d = descriptor_list; d != NULL; d = d->next )
    if ( d->descriptor == desc )
      break;

  if ( d == NULL || d->character == NULL )
    return txt;

  if ( d->original && IS_SET( d->original->act, PLR_COLOUR ) )
  {
    if ( d->original->pcdata )
      pcd = d->original->pcdata;
    fUseANSI = TRUE;
  }
  else if ( d->character && IS_SET( d->character->act, PLR_COLOUR ) )
  {
    if ( d->character->pcdata )
      pcd = d->character->pcdata;
    fUseANSI = TRUE;
  }

  /*
   * Find length in case caller didn't.
   */
  if ( length <= 0 )
    length = strlen(txt);

  if ( length > MAX_STRING_LENGTH )
    length = MAX_STRING_LENGTH;

  while ( *p_in != '\0' && i < length )
  {
    if ( *p_in == '#' )
    {
      if ( fSharp )
      {
        *p_out = '#';
        p_out++;
        fSharp = FALSE;
      }
      else
      {
        fSharp = TRUE;
      }
    }
    else
    {
      if ( fSharp )
      {
        if ( fUseANSI )
        {
          switch( *p_in )
          {
	    case '0':
	      strcpy( p_out, ANSI_OFF );
	      p_out += strlen( ANSI_OFF );
	      break;
            case 'a':
              strcpy( p_out, C_BLACK );
              p_out += strlen( C_BLACK );
              break;
            case 'b':
              strcpy( p_out, C_BLUE );
              p_out += strlen( C_BLUE );
              break;
            case 'c':
              strcpy( p_out, C_CYAN );
              p_out += strlen( C_CYAN );
              break;
            case 'g':
              strcpy( p_out, C_GREEN );
              p_out += strlen( C_GREEN );
              break;
	    case 'h':
	      strcpy( p_out, ANSI_BOLD );
	      p_out += strlen( ANSI_BOLD );
	      break;
            case 'm':
              strcpy( p_out, C_MAGENTA );
              p_out += strlen( C_MAGENTA );
              break;
            case 'r':
              strcpy( p_out, C_RED );
              p_out += strlen( C_RED );
              break;
            case 'w':
              strcpy( p_out, C_WHITE );
              p_out += strlen( C_WHITE );
              break;
            case 'y':
              strcpy( p_out, C_YELLOW );
              p_out += strlen( C_YELLOW );
              break;
	    case 'W':
	      if ( !pcd )
	        break;
	      if ( pcd->color_warmessage < 0
	      || pcd->color_warmessage >= MAX_COLOR )
	        break;
	      strcpy( p_out, color_data[pcd->color_warmessage].sequence );
	      p_out += strlen( color_data[pcd->color_warmessage].sequence );
	      break;
	    case 'E':
	      if ( !pcd )
	        break;
	      if ( pcd->color_exits < 0
	      || pcd->color_exits >= MAX_COLOR )
	        break;
	      strcpy( p_out, color_data[pcd->color_exits].sequence );
	      p_out += strlen( color_data[pcd->color_exits].sequence );
	      break;
	    case 'G':
	      if ( !pcd )
	        break;
	      if ( pcd->color_gossip < 0
	      || pcd->color_gossip >= MAX_COLOR )
	        break;
	      strcpy( p_out, color_data[pcd->color_gossip].sequence );
	      p_out += strlen( color_data[pcd->color_gossip].sequence );
	      break;
	    case 'P':
	      if ( !pcd )
	        break;
	      if ( pcd->color_prompt < 0
	      || pcd->color_prompt >= MAX_COLOR )
	        break;
	      strcpy( p_out, color_data[pcd->color_prompt].sequence );
	      p_out += strlen( color_data[pcd->color_prompt].sequence );
	      break;
	    case 'R':
	      if ( !pcd )
	        break;
	      if ( pcd->color_room_name < 0
	      || pcd->color_room_name >= MAX_COLOR )
	        break;
	      strcpy( p_out, color_data[pcd->color_room_name].sequence );
	      p_out += strlen( color_data[pcd->color_room_name].sequence );
	      break;
	    case 'S':
	      if ( !pcd )
	        break;
	      if ( pcd->color_say < 0
	      || pcd->color_say >= MAX_COLOR )
	        break;
	      strcpy( p_out, color_data[pcd->color_say].sequence );
	      p_out += strlen( color_data[pcd->color_say].sequence );
	      break;
	    case 'T':
	      if ( !pcd )
	        break;
	      if ( pcd->color_tell < 0
	      || pcd->color_tell >= MAX_COLOR )
	        break;
	      strcpy( p_out, color_data[pcd->color_tell].sequence );
	      p_out += strlen( color_data[pcd->color_tell].sequence );
	      break;
            default:
              break;
          }
          fANSI = TRUE;
        }
        fSharp = FALSE;
      }
      else
      {
        *p_out = *p_in;
        p_out++;
      }
    }
    p_in++;
    i++;
  }

  if ( fANSI == TRUE )
  {
    strcpy( p_out, ANSI_OFF );
    p_out += strlen( ANSI_OFF );
  }

  *p_out = '\0';

  return buf;
}


int color_string_to_int( char *name )
{
  int i;

  for ( i = 0; i < MAX_COLOR; i++ )
    if ( !str_prefix( name, color_data[i].name ) )
      return i;

  return -1;
}


void do_color( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int i;

  if ( IS_NPC( ch ) )
    return;

  argument = one_argument( argument, arg );

  if ( ( arg[0] == '\0' && !IS_SET( ch->act, PLR_COLOUR ) )
  || !str_cmp( "on", arg ) )
  {
    SET_BIT( ch->act, PLR_COLOUR );
    send_to_char( "Colour enabled.\n\r", ch );
    send_to_char( "Type 'color help' for information on setting up your color scheme.\n\r", ch );
    return;
  }
  else if ( arg[0] == '\0' || !str_cmp( "off", arg ) )
  {
    REMOVE_BIT( ch->act, PLR_COLOUR );
    send_to_char( "Colour disabled.\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg2 );

  if ( arg2[0] == '\0' )
  {
    arg2[0] = '1';
    arg2[1] = '\0';
  }

  if ( !str_prefix( arg, "warmessage" ) )
  {
    i = color_string_to_int( arg2 );
    ch->pcdata->color_warmessage = i;
    send_to_char( "Warmessage color ", ch );
  }
  else if ( !str_prefix( arg, "exits" ) )
  {
    i = color_string_to_int( arg2 );
    ch->pcdata->color_exits = i;
    send_to_char( "Exits color ", ch );
  }
  else if ( !str_prefix( arg, "gossip" ) )
  {
    i = color_string_to_int( arg2 );
    ch->pcdata->color_gossip = i;
    send_to_char( "Gossip color ", ch );
  }
  else if ( !str_prefix( arg, "prompt" ) )
  {
    i = color_string_to_int( arg2 );
    ch->pcdata->color_prompt = i;
    send_to_char( "Prompt color ", ch );
  }
  else if ( !str_prefix( arg, "room_name" ) )
  {
    i = color_string_to_int( arg2 );
    ch->pcdata->color_room_name = i;
    send_to_char( "Room name color ", ch );
  }
  else if ( !str_prefix( arg, "say" ) )
  {
    i = color_string_to_int( arg2 );
    ch->pcdata->color_say = i;
    send_to_char( "Say color ", ch );
  }
  else if ( !str_prefix( arg, "tell" ) )
  {
    i = color_string_to_int( arg2 );
    ch->pcdata->color_tell = i;
    send_to_char( "Tell color ", ch );
  }
  else
  {
    send_to_char("Valid user-configurable colors:\n\r\
exits, gossip, prompt, room_name, say, tell, warmessage\n\r\
\
Valid colors:\n\r\
black, blue, cyan, green, magenta, red, white, yellow\n\r\
blackbold, bluebold, cyanbold, greenbold, magentabold, redbold,\n\r\
whitebold, yellowbold, bold, blink, underscore, reverse\n\r", ch );
    return;
  }

  if ( i == -1 )
    send_to_char( "cleared.\n\r", ch );
  else
  {
    sprintf( buf, "set to %s.\n\r", color_data[i].name );
    send_to_char( buf, ch );
  }

  return;
}


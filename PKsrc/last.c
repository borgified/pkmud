
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"

struct last_info
{
  struct last_info  *prev;
  struct last_info  *next;
  char              name[14];
  time_t            time;
  time_t            duration;
};

struct last_info *last_head;


struct last_info *new_last_info( void )
{
  struct last_info *li;

  li = (struct last_info *) alloc_perm ( sizeof( struct last_info ) );
  if ( li == NULL )
  {
    log_string( "Could not initialize last logins." );
    exit( 1 );
  }
  li->next = NULL;
  li->name[0] = '\0';
  li->time = 0;
  li->duration = 0;

  return li;
}


void init_last_table( void )
{
  struct last_info *li;
  int i;

  li = new_last_info( );
  last_head = li;

  for ( i = 0; i < 9; i++ )
  {
    li->next = new_last_info( );
    li->next->prev = li;
    li = li->next;
  }

  li->next = last_head;
  last_head->prev = li;

  last_head->time = current_time;
  last_head->duration = 0;
  sprintf( last_head->name, "Reboot" );

  return;
}


void update_last( CHAR_DATA *ch )
{
  if ( ch->level > 30 )
    return;

  last_head = last_head->prev;
  strncpy( last_head->name, ch->name, 14 );
  last_head->time = current_time;

  return;
}

void update_last_logoff( CHAR_DATA *ch )
{
  struct last_info *li = last_head;

  do
  {
    if ( !str_cmp( ch->name, li->name ) )
    {
      li->duration = current_time - li->time;
      break;
    }
    li = li->next;
  }
  while( li != last_head );
}


void do_last( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  extern int war_max, war_total;
  extern time_t war_number;
  extern char last_winner[MAX_NAME_LENGTH+1];
  struct last_info *li;

  sprintf( buf, "Last war: %s\r", ctime( &war_number ) );
  send_to_char( buf, ch );
  sprintf( buf, "Players in last war: %d, Duration: %d, Winner: %s\n\n\r",
    war_max, war_total, last_winner );
  send_to_char( buf, ch );

  li = last_head;

  do
  {
    if ( (int) li->time == 0 )
      break;

    if ( li->name[0] == '\0' )
      break;

    sprintf( buf, "%16s %s (%d minutes)\r", li->name, ctime( &li->time ),
      (int) ( li->duration / 60 ) );
    send_to_char( buf, ch );
    li = li->next;
  }
  while( li != last_head );

  return;
}

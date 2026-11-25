#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#define HIGH_RATING_FILE "ratings.txt"


struct high_score_element *high_rating_list = NULL;

void add_high_element( struct high_score_element *h,
  struct high_score_element **table );


void read_high_list( FILE *fp, struct high_score_element **h )
{
  int i, time;
  int cutoff;
  char *name;
  struct high_score_element *e;

  /* 3 months */
  cutoff = (int) current_time - ( 60 * 60 * 24 * 30 * 3 );

  while ( 1 )
  {
    name = fread_word( fp );

    if ( !str_cmp( name, "End" ) )
      break;

    i = fread_number( fp );
    time = fread_number( fp );

    /*
     * Zap people that haven't logged in.
     */
    if ( cutoff > time )
      continue;

    e = (struct high_score_element *)
        malloc( sizeof( struct high_score_element ) );

    e->value = i;
    e->time = time;
    strcpy( e->name, name );
    add_high_element( e, h );
  }

  return;
}

void write_high_list( FILE *fp, struct high_score_element *h, int elems )
{
  int i = 1;

  for ( ; h != NULL; h = h->next )
  {
    fprintf( fp, "%s %d %d\n", h->name, h->value, (int) h->time );
    i++;
    if ( i > elems )
      break;
  }
  fprintf( fp, "End\n" );

  return;
}

void do_high( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  struct high_score_element *h;
  int i = 1;
  int cur = 1;
  int last = -1;

  send_to_char( "High Rating List:\n\r", ch );

  for ( h = high_rating_list; h != NULL; h = h->next )
  {
    if ( last == h->value )
      cur = cur;
    else
      cur = i;
    sprintf( buf, "  %2d   %4d %12s  %s\r", cur, h->value, h->name,
        ( (char *) ctime( &h->time ) ) );
    send_to_char( buf, ch );
    i++;
    if ( i > 20 )
      break;
    last = h->value;
  }

  return;
}


void add_high_element( struct high_score_element *h,
  struct high_score_element **table )
{
  struct high_score_element *t = *table;
  struct high_score_element *prev;

  /*
   * No entries.
   */
  if ( t == NULL )
  {
    h->next = NULL;
    *table = h;
    return;
  }

  /*
   * Larger than first entry.
   */
  if ( h->value > t->value )
  {
    h->next = t;
    *table = h;
    return;
  }

  /*
   *  Find element it is larger than.
   */
  for ( prev = t; prev->next != NULL; prev = prev->next )
  {
    if ( h->value > prev->next->value )
      break;
  }

  h->next = prev->next;
  prev->next = h;

  return;
}


void remove_high_element( struct high_score_element *h,
  struct high_score_element **table )
{
  struct high_score_element *t = *table;
  struct high_score_element *e;

  /*
   * First element.
   */
  if ( h == t )
  {
    *table = h->next;
    return;
  }

  for ( e = t; e != NULL; e = e->next )
  {
    if ( e->next == h )
      break;
  }

  if ( e == NULL )
    return;

  e->next = h->next;
  return;
}



void adjust_high_entry( struct high_score_element **table,
  CHAR_DATA *ch, int value )
{
  struct high_score_element *t = *table;
  struct high_score_element *e;

  for ( e = t; e != NULL; e = e->next )
  {
    if ( !str_cmp( ch->name, e->name ) )
      break;
  }

  /*
   * already exists.
   */
  if ( e != NULL )
  {
    if ( e->value == value )
      return;

    remove_high_element( e, table );
    e->value = value;
    e->time = current_time;
    add_high_element( e, table );

    return;
  }

  /*
   * Create new structure.
   */
  e = (struct high_score_element *)
        alloc_perm( sizeof( struct high_score_element ) );
  e->value = value;
  e->time = current_time;
  strcpy( e->name, ch->name );
  add_high_element( e, table );

  return;
}


void adjust_high_ratings( CHAR_DATA *ch, CHAR_DATA *victim )
{
  bool fWrite = FALSE;
  FILE *fp;

  if ( ch->level == 30 )
  {
    adjust_high_entry( &high_rating_list, ch, ch->pcdata->rating );
    fWrite = TRUE;
  }

  if ( victim->level == 30 )
  {
    adjust_high_entry( &high_rating_list, victim, victim->pcdata->rating );
    fWrite = TRUE;
  }

  if ( fWrite == FALSE )
    return;

  if ( ( fp = fopen( "ratingtmp.txt", "w" ) ) == NULL )
    return;

  write_high_list( fp, high_rating_list, 30 );
  fclose( fp );
  system( "cp ratingtmp.txt ratings.txt" );

  return;
}


void init_high_scores( void )
{
  FILE *fp;

  if ( ( fp = fopen( "ratings.txt", "r" ) ) == NULL )
  {
    bug( "Error opening ratings.txt", 0 );
  }
  else
  {
    read_high_list( fp, &high_rating_list );
    fclose( fp );
  }

  return;
}









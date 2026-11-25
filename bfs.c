
#include <stdlib.h>
#include "merc.h"
#include "bfs.h"
#include "global.h"

/* Used to randomize BFS search order, to avoid knowing that being
 * east of a monster is always safer than north.  */
int bfs_start_direction = 0;

/* Tag rooms with this identifier to mark already searched rooms. */
int bfs_current_identifier = 1;
int bfs_max_depth = 0;

int bfs_flags = 0;

/* Keep pointer to next room to search. */
ROOM_INDEX_DATA *bfs_top = NULL;
ROOM_INDEX_DATA *bfs_tail = NULL;

void bfs_set_flags( CHAR_DATA *ch )
{
  bfs_flags = 0;

  if ( IS_AFFECTED( ch, AFF_PASS_DOOR ) )
    bfs_flags += BFS_CAN_PASS_WALLS;

  if ( can_traverse_water( ch ) )
    bfs_flags += BFS_CAN_TRAVERSE_WATER;

  if ( can_fly( ch ) )
    bfs_flags += BFS_CAN_FLY;

  return;
}

void bfs_new_search( CHAR_DATA *ch, ROOM_INDEX_DATA *room, int max_depth )
{
  int i, j;
  ROOM_INDEX_DATA *to_room;

  bfs_top = room;
  bfs_current_identifier++;

  if ( room == NULL )
    return;

  bfs_tail = room;
  bfs_max_depth = max_depth;

  bfs_set_flags( ch );

  /* Only use n,e,s,w for start directions */
  bfs_start_direction = number_range( 0, 3 );

  room->bfs_next = NULL;
  room->bfs_identifier = bfs_current_identifier;
  room->bfs_direction = -1;
  room->bfs_depth = 0;

  for ( i = 0; i < 6; i++ )
  {
    j = i + bfs_start_direction;
    if ( j > 5 )
      j -= 6;

    if ( room->exit[j] == NULL )
      continue;

    to_room = room->exit[j]->to_room;

    if ( to_room == NULL )
      continue;

    if ( to_room->bfs_identifier == bfs_current_identifier )
      continue;

    /* Do air check after flagging room */
    if ( to_room->sector_type == SECT_AIR
    && !IS_SET( bfs_flags, BFS_CAN_FLY ) )
      continue;

    /* Do water check after flagging room */
    if ( to_room->sector_type == SECT_WATER_NOSWIM
    && !IS_SET( bfs_flags, BFS_CAN_TRAVERSE_WATER ) )
      continue;

    bfs_add_room( to_room, j, 1 );
  }

  return;
}

ROOM_INDEX_DATA *bfs_walk_next( void )
{
  ROOM_INDEX_DATA *pRoom;

  if ( bfs_top == NULL )
    return NULL;

  pRoom = bfs_top;

  if ( pRoom->bfs_depth < bfs_max_depth )
    bfs_add_adjacent_rooms( pRoom );

  bfs_top = pRoom->bfs_next;

  return pRoom;
}

void bfs_add_room( ROOM_INDEX_DATA *room, int direction, int depth )
{
  bfs_tail->bfs_next = room;
  bfs_tail = room;

  room->bfs_next = NULL;
  room->bfs_identifier = bfs_current_identifier;
  room->bfs_direction = direction;
  room->bfs_depth = depth;

  return;
}


void bfs_add_adjacent_rooms( ROOM_INDEX_DATA *room )
{
  int i, j;
  ROOM_INDEX_DATA *to_room;

  if ( room == NULL )
    return;

  for ( i = 0; i < 6; i++ )
  {
    j = i + bfs_start_direction;
    if ( j > 5 )
      j -= 6;

    if ( room->exit[j] == NULL )
      continue;

    to_room = room->exit[j]->to_room;

    if ( to_room == NULL )
      continue;

    if ( to_room->bfs_identifier == bfs_current_identifier )
      continue;

    /* Do air check after flagging room */
    if ( to_room->sector_type == SECT_AIR
    && !IS_SET( bfs_flags, BFS_CAN_FLY ) )
      continue;

    /* Do water check after flagging room */
    if ( to_room->sector_type == SECT_WATER_NOSWIM
    && !IS_SET( bfs_flags, BFS_CAN_TRAVERSE_WATER ) )
      continue;

    bfs_add_room( to_room, room->bfs_direction, room->bfs_depth + 1 );
  }

  return;
}

int bfs_find_room( CHAR_DATA *ch, ROOM_INDEX_DATA *start_room,
  ROOM_INDEX_DATA *goal_room, int max_depth )
{
  ROOM_INDEX_DATA *room;

  bfs_new_search( ch, start_room, max_depth );

  while ( ( room = bfs_walk_next( ) ) != NULL )
  {
    if ( room == goal_room )
      return room->bfs_direction;
  }

  return -1;
}


void do_rtrack( CHAR_DATA *ch, char *argument )
{
  int rnum;
  int direction;
  ROOM_INDEX_DATA *goal;
  char buf[MAX_STRING_LENGTH];

  if ( ch->in_room == NULL )
  {
    send_to_char( "You can't track if you're not in a room.\r\n", ch );
    return;
  }

  rnum = atoi( argument );
  goal = get_room_index( rnum );

  if ( goal == NULL )
  {
    send_to_char( "No room with that vnum.\r\n", ch );
    return;
  }

  direction = bfs_find_room( ch, ch->in_room, goal, 100 );

  if ( direction == -1 )
  {
    send_to_char( "Unable to find path to room.\r\n", ch );
    return;
  }

  sprintf( buf, "Room %d is to the %s.\r\n", rnum, dir_name[direction] );
  send_to_char( buf, ch );

  return;
}





#ifndef __BFS_UTILITIES_H_
#define __BFS_UTILITIES_H_

void bfs_new_search( CHAR_DATA *CH, ROOM_INDEX_DATA *room, int max_depth );
void bfs_add_room( ROOM_INDEX_DATA *room, int direction, int depth );
void bfs_add_adjacent_rooms( ROOM_INDEX_DATA *room );
int bfs_find_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room,
  ROOM_INDEX_DATA *goal, int max_depth );
ROOM_INDEX_DATA *bfs_walk_next( void );

#define BFS_CAN_PASS_WALLS            1
#define BFS_CAN_TRAVERSE_WATER        2
#define BFS_CAN_FLY                   4

#endif /* __BFS_UTILITIES_H_ */

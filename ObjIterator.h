

#ifndef __OBJ_ITERATOR_H_
#define __OBJ_ITERATOR_H_

#include "merc.h"


class ObjIterator
{
public:

  ObjIterator( void );
  ~ObjIterator( void );

  OBJ_DATA *Start( CHAR_DATA *ch,
                   unsigned int type );
  OBJ_DATA *StartPhase( unsigned int phase,
                        OBJ_DATA *list,
                        unsigned int type );

  /* Searches a passed in list */
  /* Used for inventory, bags, etc. */
  OBJ_DATA *StartList( CHAR_DATA *ch,
                       OBJ_DATA *obj );
  OBJ_DATA *Next( void );

  /* Searches char list in phase */
  static const unsigned int WORLD = 0;

  /* Searches room in phase */
  static const unsigned int ROOM = 1;

  /* Searches char list, any phase */
  static const unsigned int ALL = 2;

private:
  OBJ_DATA *next;
  unsigned int type;
  unsigned int phase;
};

#endif /* __CHAR_ITERATOR_H_ */

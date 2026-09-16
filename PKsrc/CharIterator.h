

#ifndef __CHAR_ITERATOR_H_
#define __CHAR_ITERATOR_H_

#include "merc.h"


class CharIterator
{
public:

  CharIterator( void );
  ~CharIterator( void );

  CHAR_DATA *Start( CHAR_DATA *ch,
                    unsigned int type );
  CHAR_DATA *StartPhase( unsigned int phase,
                         ROOM_INDEX_DATA *room,
                         unsigned int type );
  CHAR_DATA *Next( void );

  /* Searches char list in phase */
  static const unsigned int WORLD = 0;

  /* Searches room in phase */
  static const unsigned int ROOM = 1;

  /* Searches char list, any phase */
  static const unsigned int ALL = 2;

private:
  CHAR_DATA *next;
  unsigned int type;
  unsigned int phase;
};

#endif /* __CHAR_ITERATOR_H_ */

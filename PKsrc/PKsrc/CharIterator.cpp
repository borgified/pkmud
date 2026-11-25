
#include "CharIterator.h"


CharIterator::CharIterator( void )
  : next(NULL), type(WORLD), phase(0)
{
}

CharIterator::~CharIterator( void )
{
}

CHAR_DATA *CharIterator::Start( CHAR_DATA *ch,
                          unsigned int __type )
{
  if ( ch == NULL || __type > ALL )
    return NULL;

  type = __type;

  if ( type == WORLD || type == ALL )
    next = char_list;
  else if ( ch->in_room == NULL )
    next = NULL;
  else
    next = ch->in_room->people;

  phase = ch->phase;
  if ( phase >= NUM_PHASES )
    phase = 0;

  return Next();
}


CHAR_DATA *CharIterator::StartPhase( unsigned int __phase,
                                     ROOM_INDEX_DATA *room,
                                     unsigned int __type )
{
  if ( __type > ALL )
    return NULL;

  type = __type;

  if ( type == WORLD || type == ALL )
    next = char_list;
  else if ( room == NULL )
    next = NULL;
  else
    next = room->people;

  phase = __phase;
  if ( phase >= NUM_PHASES )
    phase = 0;

  return Next();
}


CHAR_DATA *CharIterator::Next( void )
{
  CHAR_DATA *retchar = next;
  
  while ( retchar != NULL )
  {
    if ( type == WORLD || type == ALL )
      next = retchar->next;
    else
      next = retchar->next_in_room;

    if ( retchar->phase == phase || type == ALL )
      return retchar;

    retchar = next;
  }

  return retchar;
}


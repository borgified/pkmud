
#include "ObjIterator.h"


ObjIterator::ObjIterator( void )
  : next(NULL), type(WORLD), phase(0)
{
}

ObjIterator::~ObjIterator( void )
{
}

OBJ_DATA *ObjIterator::Start( CHAR_DATA *ch,
                          unsigned int __type )
{
  if ( ch == NULL || __type > ALL )
    return NULL;

  type = __type;

  if ( type == WORLD || type == ALL )
    next = object_list;
  else if ( ch->in_room == NULL )
    next = NULL;
  else
    next = ch->in_room->contents;

  phase = ch->phase;
  if ( phase >= NUM_PHASES )
    phase = 0;

  return Next();
}


OBJ_DATA *ObjIterator::StartPhase( unsigned int __phase,
                                   OBJ_DATA *list,
                                   unsigned int __type )
{
  if ( __type > ALL )
    return NULL;

  type = __type;

  if ( type == WORLD || type == ALL )
    next = object_list;
  else if ( list == NULL )
    next = NULL;
  else
    next = list;

  phase = __phase;
  if ( phase >= NUM_PHASES )
    phase = 0;

  return Next();
}


OBJ_DATA *ObjIterator::StartList( CHAR_DATA *ch,
                                  OBJ_DATA *list )
{
  if ( ch == NULL || list == NULL )
    return NULL;

  type = ROOM;

  next = list;

  phase = ch->phase;
  if ( phase >= NUM_PHASES )
    phase = 0;

  return Next();
}


OBJ_DATA *ObjIterator::Next( void )
{
  OBJ_DATA *retchar = next;
  
  while ( retchar != NULL )
  {
    if ( type == WORLD || type == ALL )
      next = retchar->next;
    else
      next = retchar->next_content;

    if ( retchar->phase == phase || type == ALL )
      return retchar;

    retchar = next;
  }

  return retchar;
}


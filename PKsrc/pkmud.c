
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"


bool can_interact( CHAR_DATA *ch )
{
  if ( IS_NPC( ch ) )
    return TRUE;

  if ( ch->phase != WAR_NUM_MAIN )
    return TRUE;

  if ( ch->pcdata->war_num == WAR_NUM_MAIN )
    return TRUE;

  if ( get_trust( ch ) >= LEVEL_IMMORTAL )
    return TRUE;

  return FALSE;
}


bool in_main_war( CHAR_DATA *ch )
{
  if ( IS_NPC( ch ) )
    return TRUE;

  if ( ch->pcdata->war_num == WAR_NUM_MAIN )
    return TRUE;

  if ( get_trust( ch ) >= LEVEL_IMMORTAL )
    return TRUE;

  return FALSE;
}

bool mortal_in_main_war( CHAR_DATA *ch )
{
  if ( IS_NPC( ch ) )
    return FALSE;

  if ( get_trust( ch ) >= LEVEL_IMMORTAL )
    return FALSE;

  return in_main_war( ch );
}

bool in_ffa_war( CHAR_DATA *ch )
{
  if ( !in_a_war( ch ) )
    return FALSE;

  if ( IS_NPC( ch ) )
    return FALSE;

  if ( ch->pcdata->war_num == WAR_NUM_FFA )
    return TRUE;

  return FALSE;
}


bool in_private_war( CHAR_DATA *ch )
{
  if ( !in_a_war( ch ) )
    return FALSE;

  if ( IS_NPC( ch ) )
    return FALSE;

  if ( ch->pcdata->war_num == WAR_NUM_PRIVATE )
    return TRUE;

  return FALSE;
}

bool in_static_war( CHAR_DATA *ch )
{
  if ( mortal_in_main_war( ch ) )
    return TRUE;

  if ( in_private_war( ch ) )
    return TRUE;

  return FALSE;
}


bool in_same_war( CHAR_DATA *ch, CHAR_DATA *victim )
{
  if ( get_trust( ch ) >= LEVEL_IMMORTAL
  || get_trust( victim ) >= LEVEL_IMMORTAL )
    return TRUE;

  if ( ch->phase == victim->phase )
    return TRUE;

#if 0
  if ( IS_NPC( ch ) )
  {
    if ( in_main_war( victim ) )
      return TRUE;
    else
      return FALSE;
  }

  if ( IS_NPC( victim ) )
  {
    if ( in_main_war( ch ) )
      return TRUE;
    else
      return FALSE;
  }

  if ( ch->pcdata->war_num != victim->pcdata->war_num
  || ch->pcdata->war_num == WAR_NUM_DEAD )
    return FALSE;

  if ( in_private_war( ch ) )
  {
    if ( ch->private_war != victim )
      return FALSE;
  }
#endif

  return FALSE;
}


bool in_a_war( CHAR_DATA *ch )
{
  if ( IS_NPC( ch ) )
    return TRUE;

  if ( get_trust( ch ) >= LEVEL_IMMORTAL )
    return TRUE;

  if ( ch->pcdata->war_num != WAR_NUM_DEAD )
    return TRUE;

  return FALSE;
}



int count_waiting_players( void )
{
  DESCRIPTOR_DATA *desc;
  int count = 0;

  for ( desc = descriptor_list; desc != NULL; desc = desc->next )
  {
    if ( desc->connected != CON_PLAYING )
      continue;
    if ( desc->character == NULL )
      continue;
    if ( desc->character->level != 30 )
      continue;

    if ( IS_SET( desc->character->act, PLR_NOPLAY ) )
      continue;

//    if ( !mortal_in_main_war( desc->character ) )
    if ( desc->character->phase != WAR_NUM_MAIN
    && !IS_SET( desc->character->act, PLR_NOPLAY ) )
      count++;
  }

  return count;
}




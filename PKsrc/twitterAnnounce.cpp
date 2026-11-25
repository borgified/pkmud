#include "merc.h"
#include "twitterAnnounce.h"

#define DEBUG 1
#define MAX_TWITTER_LENGTH 140

twitterAnnounce::twitterAnnounce( void )
  : outprio(0), last_time(0), delay(0), initialized(0), enabled(1)
{
}

twitterAnnounce::~twitterAnnounce( void )
{
}

void twitterAnnounce::init( int _lastTime, int _delay )
{
  last_time = _lastTime;
  delay = _delay;
}

void twitterAnnounce::push( char *buf, int priority )
{
  /* Twitter disabled / twitcurl removed */
}

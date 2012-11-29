

#include "merc.h"
#include "twitterAnnounce.h"

#define DEBUG 1

#define MAX_TWITTER_LENGTH 140




twitterAnnounce::twitterAnnounce( void )
  : outprio(0), last_time(0)
{
}


twitterAnnounce::~twitterAnnounce( void )
{
}


void twitterAnnounce::init( int _lastTime, int _delay )
{
  char logbuf[1024];
  last_time = _lastTime;
  delay = _delay;

  std::string pkuser( "PKMudAnnounce" );
  std::string pkpwd( "NPcywfWAXf8LYQuq" );

  twitterObj.setTwitterUsername( pkuser );
  twitterObj.setTwitterPassword( pkpwd );

  /* oauth */
  twitterObj.getOAuth().setConsumerKey( std::string( "KpUnplr0r1Tk8IYmqMmOQ" ) );
  twitterObj.getOAuth().setConsumerSecret( std::string( "wLEu6y65xS8HYYns4yRm0jtd8FKg4xJ2M6MeKKOiGk" ) );

  std::string authtokkey( "963439207-tOsVy0sHVWkP3sdrDz4plpZZFHmXex3h9P5jBCbp" );
  std::string authtoksec( "FBRduEfWdeVaYYn1NMJbgLjx5oB21jk35g5X0aEs" );

  twitterObj.getOAuth().setOAuthTokenKey( authtokkey );
  twitterObj.getOAuth().setOAuthTokenSecret( authtoksec );

  if( twitterObj.accountVerifyCredGet() )
  {
    sprintf( logbuf, "Successfully verified Twitter account.\n\r" );
    log_string( logbuf );
  }
  else
  {
    sprintf( logbuf, "ERROR: Failed to verify Twitter account.\n\r" );
    log_string( logbuf );
    exit( 1 );
  }
}


void twitterAnnounce::push( char *buf, int priority )
{
  char logbuf[1024];

  /* Calculate time since last message */
  int delta = current_time - last_time;

#if 0
sprintf( logbuf, "cur: %d, last: %d, delta: %d, delay: %d, prio: %d, buf: '%s'\n\r",
  current_time, last_time, delta, delay, priority, buf );
log_string( logbuf );
#endif

  /* Don't spam twitter! */
  if ( delta < delay && priority <= outprio )
  {
    return;
  }

  /* Nothing to send. */
  if ( buf[0] == '\0' )
  {
    return;
  }

  if ( strlen( buf ) > MAX_TWITTER_LENGTH )
    buf[MAX_TWITTER_LENGTH] = '\0';

  std::string tmpstr = buf;
  twitterObj.statusUpdate( tmpstr );

  sprintf( logbuf, "Generated twitter message: '%s'", buf );
  log_string( logbuf );

  last_time = current_time;
  outprio = priority;
}


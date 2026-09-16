

#include "merc.h"
#include "twitterAnnounce.h"

#define DEBUG 1

#define MAX_TWITTER_LENGTH 140




twitterAnnounce::twitterAnnounce( void )
  : outprio(0), last_time(0), initialized(0), enabled(1)
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

#if TWITTER_DISABLE
  return;
#endif

  std::string pkUser("");
  std::string pkPassword("");
  std::string pkConsumerKey("");
  std::string pkConsumerSecret("");
  std::string pkOAuthToken("");
  std::string pkOAuthTokenSecret("");
  std::ifstream fileStream;
  char tmpBuf[1024];

  fileStream.open( "../PKauth/pk_user.txt" );
  memset( tmpBuf, 0, 1024 );
  fileStream >> tmpBuf;
  pkUser = tmpBuf;
  fileStream.close( );

  fileStream.open( "../PKauth/pk_password.txt" );
  memset( tmpBuf, 0, 1024 );
  fileStream >> tmpBuf;
  pkPassword = tmpBuf;
  fileStream.close( );

  fileStream.open( "../PKauth/pk_consumer_key.txt" );
  memset( tmpBuf, 0, 1024 );
  fileStream >> tmpBuf;
  pkConsumerKey = tmpBuf;
  fileStream.close( );

  fileStream.open( "../PKauth/pk_consumer_secret.txt" );
  memset( tmpBuf, 0, 1024 );
  fileStream >> tmpBuf;
  pkConsumerSecret = tmpBuf;
  fileStream.close( );

  fileStream.open( "../PKauth/pk_oauth_token.txt" );
  memset( tmpBuf, 0, 1024 );
  fileStream >> tmpBuf;
  pkOAuthToken = tmpBuf;
  fileStream.close( );

  fileStream.open( "../PKauth/pk_oauth_token_secret.txt" );
  memset( tmpBuf, 0, 1024 );
  fileStream >> tmpBuf;
  pkOAuthTokenSecret = tmpBuf;
  fileStream.close( );


  twitterObj.setTwitterUsername( pkUser );
  twitterObj.setTwitterPassword( pkPassword );

  /* oauth */
  twitterObj.getOAuth().setConsumerKey( pkConsumerKey );
  twitterObj.getOAuth().setConsumerSecret( pkConsumerSecret );

  twitterObj.getOAuth().setOAuthTokenKey( pkOAuthToken );
  twitterObj.getOAuth().setOAuthTokenSecret( pkOAuthTokenSecret );

  if( twitterObj.accountVerifyCredGet() )
  {
    sprintf( logbuf, "Successfully verified Twitter account.\n\r" );
    log_string( logbuf );
    initialized = 1;
  }
  else
  {
    sprintf( logbuf, "ERROR: Failed to verify Twitter account.\n\r" );
    log_string( logbuf );
  }
}



void twitterAnnounce::push( char *buf, int priority )
{
  char logbuf[1024];

  if ( initialized == 0 )
    return;

  if ( enabled == 0 )
    return;

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
  twitterObj.statusUpdate( tmpstr, "" );

  sprintf( logbuf, "Generated twitter message: '%s'", buf );
  log_string( logbuf );

  last_time = current_time;
  outprio = priority;
}


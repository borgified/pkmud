

#include "../twitcurl/libtwitcurl/twitcurl.h"

#define TWITTER_DISABLE TRUE

class twitterAnnounce
{
  public:
    twitterAnnounce( void );
    ~twitterAnnounce( void );

    void init( int _lastTime, int _delay );

    void push( char *buf, int priority );

    void enable( void ) { enabled = 1; }
    void disable( void ) { enabled = 0; }

  private:
    twitCurl twitterObj;
    int outprio;
    int last_time;
    int delay;
    int initialized;
    int enabled;
};


extern twitterAnnounce twitter;

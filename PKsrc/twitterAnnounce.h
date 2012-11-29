

#include "../twitcurl/libtwitcurl/twitcurl.h"

class twitterAnnounce
{
  public:
    twitterAnnounce( void );
    ~twitterAnnounce( void );

    void init( int _lastTime, int _delay );

    void push( char *buf, int priority );

  private:
    twitCurl twitterObj;
    int outprio;
    int last_time;
    int delay;
};


extern twitterAnnounce twitter;

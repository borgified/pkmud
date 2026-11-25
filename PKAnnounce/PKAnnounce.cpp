

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "includes/twitcurl.h"

#define DEBUG 1

#define MAX_TWITTER_LENGTH 140
#define MAX_INCOMING_MSG 170

#define PORT_NUM 5000

void get_data( char *buf, int *priority );
void send_to_twitter( twitCurl &to, char *const buf );
void config_twitter( twitCurl &to );

char terminator[5] = "####";
char authmsg[19] = "twitterserver1234\n";

char lettera[2] = "a";



int main( int argc, char **argv )
{
  char c;
  char buffer[1024];
  int numread;
  int last_time = 0;
  int now_time = 0;
  int time_threshold = 600;
  int last_priority = 0;
  int now_priority = 0;
  twitCurl twitterObj;

  config_twitter( twitterObj );

  while ( 1 )
  {
    if ( DEBUG )
      printf( "in main loop\n\r" );

    /* Get more Data */
    get_data( buffer, &now_priority );

    if ( DEBUG )
      printf( "past get data\n\r" );

    if ( strlen( buffer ) < 0 )
      continue;

    if ( strlen( buffer ) > 120 )
      buffer[120] = '\0';

    /* Check timer */
    now_time = (int) time( NULL );

    if ( ( now_time - last_time ) < time_threshold
       &&  now_priority <= last_priority )
    {
      printf( "Aborting send.  Too close to most recent send.\n\r" );
      continue;
    }

    last_time = now_time;
    last_priority = now_priority;

    if ( DEBUG )
      printf( "past abort: t: %d, prio: %d\n\r", now_time, now_priority );

    /* send to Twitter */
    send_to_twitter( twitterObj, buffer );
  }

  return 0;
}


int connect_to_socket( int port )
{
  int sockfd;
  struct hostent *server;
  struct sockaddr_in serv_addr;
  int optval, optlen;

  sockfd = socket( AF_INET, SOCK_STREAM, 0 );
  if ( sockfd < 0 )
  {
    printf( "Unable to open socket.  Aborting.\n\r" );
    exit( 1 );
  }


  /* Set the option active */
  optval = 1;
  optlen = sizeof(optval);
  if(setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
     printf("SO_KEEPALIVE set failed\n");
     close(sockfd);
     exit(1);
  }

  server = gethostbyname( "pkmud.net" );
  if ( server == NULL )
  {
    printf( "Unable to look up localhost. Aborting.\n\r" );
    exit(1);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *) server->h_addr,
        (char *) &serv_addr.sin_addr.s_addr,
        server->h_length );
  serv_addr.sin_port = htons(port);

  while ( 1 )
  {
    if ( connect( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
    {
      printf( "Error connecting to server.  Sleeping 60 seconds.\n\r" );
      sleep( 60 );
      continue;
    }

printf( "Successfully connected. Socket = %d\n\r", sockfd );
    write( sockfd, authmsg, strlen( authmsg ) );
    break;
  }

  return sockfd;
}


void get_data( char *buf, int *priority )
{
  static struct timeval null_time;
  static int sockfd = 0;
  char readbuf[1024];
  int port = PORT_NUM;
  int position;
  int nRead = 0;

  readbuf[0] = '\0';
  buf[0] = '\0';
  *priority = 0;

  while ( 1 )
  {
    sleep( 1 );

    if ( !sockfd )
      sockfd = connect_to_socket( port );

    while ( sockfd )
    {
      sleep( 1 );

      fd_set in_set;
      fd_set out_set;
      fd_set exc_set;
      FD_ZERO( &in_set  );
      FD_ZERO( &out_set );
      FD_ZERO( &exc_set );
      FD_SET( sockfd, &in_set );
      FD_SET( sockfd, &exc_set );

//      write( sockfd, lettera, 1 );

      if ( select( sockfd+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
      {
        printf( "Error calling select.  Aborting.\n\r" );
        exit( 1 );
      }

      if ( FD_ISSET( sockfd, &exc_set ) )
      {
printf( "Exception set, closing.\n\r" );
        close( sockfd );
        sockfd = 0;
        break;
      }

      if ( !FD_ISSET( sockfd, &in_set ) )
        continue;

      position = 0;
      if ( readbuf[0] != '\0' )
        position = strlen( readbuf );
      if ( position > MAX_INCOMING_MSG )
        position = 0;

      nRead = read( sockfd, &readbuf[position], MAX_INCOMING_MSG );

      if ( nRead > MAX_INCOMING_MSG
       ||  readbuf[1] != ':' )
      {
        readbuf[0] = '\0';
        continue;
      }

      readbuf[nRead] = '\0';
printf( "Got a message: %s\n\r", readbuf );

      /* Valid message received of format priority:message#### */
      if ( !strcmp( &readbuf[nRead - 4], terminator ) )
      {
        *priority = (int) readbuf[0] - (int) '0';

        if ( *priority > 9 ) *priority = 0;
        if ( *priority < 0 ) *priority = 0;

printf( "priority = %d\n\r", *priority );

        strncpy( buf, &readbuf[2], nRead - 6 );
        buf[nRead - 6] = '\0';
printf( "message processed\n\r" );
        return;
      }
printf( "failed strcmp\n\r" );
    }
  }

  return;
}


void send_to_twitter( twitCurl &twitterObj, char *buf )
{
  std::string tmpstr = buf;

  twitterObj.statusUpdate( tmpstr );

  return;
}


void config_twitter( twitCurl &twitterObj )
{

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
    printf( "Successfully verified Twitter account.\n\r" );
  }
  else
  {
    printf( "ERROR: Failed to verify Twitter account.\n\r" );
    exit( 1 );
  }
}





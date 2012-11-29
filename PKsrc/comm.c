/**************************************************************************m
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "merc.h"
#include "global.h"

#include "twitterAnnounce.h"


/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif



/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#endif

#if defined(apollo)
#undef __attribute
#endif



/*
 * Socket and TCP/IP stuff.
 */
#if	defined(macintosh) || defined(MSDOS)
const	char	echo_off_str	[] = { '\0' };
const	char	echo_on_str	[] = { '\0' };
const	char 	go_ahead_str	[] = { '\0' };
#endif

#if	defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };
#endif



/*
 * OS-dependent declarations.
 */
#if	defined(_AIX)
#include <sys/select.h>
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(apollo)
#include <unistd.h>
void	bzero		args( ( char *b, int length ) );
#endif

#if	defined(__hpux)
int	accept		args( ( int s, void *addr, int *addrlen ) );
int	bind		args( ( int s, const void *addr, int addrlen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, void *addr, int *addrlen ) );
int	getsockname	args( ( int s, void *name, int *addrlen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname,
 				const void *optval, int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(interactive)
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

#if	defined(linux)
/*int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
*/
int	close		args( ( int fd ) );
/*
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
*/
/*
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
*/
#endif

#if	defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct	timeval
{
	time_t	tv_sec;
	time_t	tv_usec;
};
#if	!defined(isascii)
#define	isascii(c)		( (c) < 0200 )
#endif
static	long			theKeys	[4];

int	gettimeofday		args( ( struct timeval *tp, void *tzp ) );
#endif

#if	defined(MIPS_OS)
extern	int		errno;
#endif

#if	defined(MSDOS)
int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
int	kbhit		args( ( void ) );
#endif

#if	defined(NeXT)
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined(sequent)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
int	listen		args( ( int s, int backlog ) );
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, caddr_t optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

/*
 * This includes Solaris SYSV as well.
 */
#if defined(sun)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
#if defined(SYSV)
int	setsockopt	args( ( int s, int level, int optname, 
                            const char *optval, int optlen ) );
#else 
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
#endif
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif



/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_free;	/* Free list for descriptors	*/
DESCRIPTOR_DATA *   descriptor_list;	/* All open descriptors		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    god;		/* All new chars are gods!	*/
bool		    merc_down;		/* Shutdown			*/
bool		    wizlock;		/* Game is wizlocked		*/
char		    str_boot_time[MAX_INPUT_LENGTH];
time_t		    current_time;	/* Time of this pulse		*/
time_t		    war_number = 1;		/* time this war started */

/* external war variables */
int                 war_status = NO_WAR;  /* contains status of war */
int                 alive_players = 0;	/* alive players... */
int                 war_timer  = 0;	/* keeps track of various war
					   functions */
int		    war_flags = 0;
int		    war_total = 0; /* elapsed time of current war */
int		    war_death = 0;
int		    war_max = 0;   /* max number of players this/last war */
char		    last_winner[MAX_NAME_LENGTH+1] = "(reboot)";
bool		    reboot_on = FALSE;
int		    pulse_point2;
int		    pulse_area2;
int		    shutdown_time = -1;
int		    last_boot_reason = BOOT_CRASH;

/* Declare twitter announcer */
twitterAnnounce twitter;

/*
 * local function
 */




/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void	game_loop_mac_msdos	args( ( void ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif

#if defined(unix)
void	game_loop_unix		args( ( int control ) );
int	init_socket		args( ( int port ) );
void	new_descriptor		args( ( int control ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif




/*
 * Other local functions (OS-independent).
 */
bool	check_parse_name	args( ( char *name ) );
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name ) );
int	main			args( ( int argc, char **argv ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	process_output		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void	permanent_bans		args( ( ) );



int main( int argc, char **argv )
{
    struct timeval now_time;
    int port;

#if defined(unix)
    int control;
#endif

    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

    /*
     * Init time.
     */
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    strcpy( str_boot_time, ctime( &current_time ) );

    /*
     * Check boot reason.
     */
    if ( !access( REBOOT_FILE, F_OK ) )
    {
	system( "rm -f reboot.txt" );
	last_boot_reason = BOOT_REBOOT;
    }

    /*
     * Macintosh console initialization.
     */
#if defined(macintosh)
    console_options.nrows = 31;
    cshow( stdout );
    csetmode( C_RAW, stdin );
    cecho2file( "log file", 1, stderr );
#endif

    /*
     * Reserve one channel for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
    port = 4000;
    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}
    }

    /*
     * Run the game.
     */
#if defined(macintosh) || defined(MSDOS)
    boot_db( );
    log_string( "Merc is ready to rock." );
    game_loop_mac_msdos( );
#endif

#if defined(unix)
    control = init_socket( port );

    boot_db( );
    permanent_bans( );

    twitter.init( 0, 900 );

    sprintf( log_buf, "Merc is ready to rock on port %d.", port );
    log_string( log_buf );
    game_loop_unix( control );
    close( control );
#endif

    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game." );
    exit( 0 );
    return 0;
}



#if defined(unix)
int init_socket( int port )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
/*
    int x;
*/
/* bug fix to stop port binds posted by rtaylor on r.g.m.admin */
    int x = 1;
    int fd;

    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof(x) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	close( fd );
	exit( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    close( fd );
	    exit( 1 );
	}
    }
#endif

    sa		    = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
	perror( "Init_socket: bind" );
	close( fd );
	exit( 1 );
    }

    if ( listen( fd, 3 ) < 0 )
    {
	perror( "Init_socket: listen" );
	close( fd );
	exit( 1 );
    }

    return fd;
}
#endif



#if defined(unix)
void game_loop_unix( int control )
{
    static struct timeval null_time;
    struct timeval last_time;

    signal( SIGPIPE, SIG_IGN );
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	DESCRIPTOR_DATA *d;
	OBJ_DATA *wield;
	int maxdesc;

#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );

	maxdesc	= control;

	for ( d = descriptor_list; d; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	}

	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    perror( "Game_loop: select: poll" );
	    exit( 1 );
	}

	/*
	 * New connection?
	 */
	if ( FD_ISSET( control, &in_set ) )
	    new_descriptor( control );

	/*
	 * Kick out the freaky folks.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;   
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character )
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d );
	    }
	}

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
		--d->character->wait;
		if ( !IS_NPC( d->character )
		&& !IS_SET( d->character->act, PLR_CMD_SUCCESS ) )
		  continue;
	    }

	    /* This is a lame place to put the archer stuff, but we
	     * want to avoid an extra loop every pulse, and we come
	     * through here once per pulse.
	     */
	    if ( d->character != NULL
		&& (wield = get_eq_char( d->character, WEAR_WIELD ) ) != NULL
		&& wield->item_type == ITEM_WEAPON
		&& IS_SET( wield->extra_flags, ITEM_RANGED_WEAPON )
		&& wield->timer > 0 )
	    {
		wield->timer--;
		if ( wield->timer == 0 )
		    send_to_char("Your weapon is ready to fire.\n\r", d->character );
	    }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

		if ( d->connected == CON_PLAYING )
		    interpret( d->character, d->incomm );
		else
		    nanny( d, d->incomm );

		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler( );

	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 )
	    &&   FD_ISSET(d->descriptor, &out_set) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}

	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta	= ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
		    perror( "Game_loop: select: stall" );
		    exit( 1 );
		}
	    }
	}

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif



#if defined(unix)
void new_descriptor( int control )
{
    static DESCRIPTOR_DATA d_zero;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    BAN_DATA *pban;
    struct sockaddr_in sock;
    struct hostent *from;
    int desc;
    int size;
    int i;
    bool is_numeric_address( char *addr );

    size = sizeof(sock);
    getsockname( control, (struct sockaddr *) &sock, (socklen_t *) &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, (socklen_t *) &size) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	return;
    }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
	perror( "New_descriptor: fcntl: FNDELAY" );
	return;
    }

    /*
     * Cons a new descriptor.
     */
    if ( descriptor_free == NULL )
    {
	dnew		= (DESCRIPTOR_DATA *) alloc_perm( sizeof(*dnew) );
    }
    else
    {
	dnew		= descriptor_free;
	descriptor_free	= descriptor_free->next;
    }

    *dnew		= d_zero;
    dnew->descriptor	= desc;
    dnew->connected	= CON_GET_NAME;
    dnew->outsize	= 2000;
    dnew->outbuf	= (char *) alloc_mem( dnew->outsize );

    size = sizeof(sock);
    if ( getpeername( desc, (struct sockaddr *) &sock, (socklen_t *) &size ) < 0 )
    {
	perror( "New_descriptor: getpeername" );
	dnew->host = str_dup( "(unknown)" );
    }
    else
    {
	/*
	 * Would be nice to use inet_ntoa here but it takes a struct arg,
	 * which ain't very compatible between gcc and system libraries.
	 */
	int addr;

	addr = ntohl( sock.sin_addr.s_addr );
	sprintf( buf, "%d.%d.%d.%d",
	    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
	    );
	sprintf( log_buf, "Sock.sinaddr:  %s", buf );
	log_string( log_buf );
	from = gethostbyaddr( (char *) &sock.sin_addr,
	    sizeof(sock.sin_addr), AF_INET );
	dnew->host = str_dup( from ? from->h_name : buf );
    }

    for ( i = 0; i < strlen( dnew->host ); i++ )
	dnew->host[i] = LOWER( dnew->host[i] );


    /*
     * Block sites without reverse DNS
     */
    if ( 0 && is_numeric_address( dnew->host ) )
    {
      write_to_descriptor( desc,
        "We only allow connections from sites with reverse DNS enabled.\n\r", 0 );
      write_to_descriptor( desc,
        "Please speak with your system administrator to fix this problem.\n\r", 0 );
      close( desc );
      free_string( dnew->host );
      free_mem( dnew->outbuf, dnew->outsize );
      dnew->next		= descriptor_free;
      descriptor_free	= dnew;
      return;
    }
	
    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    for ( pban = ban_list; pban != NULL; pban = pban->next )
    {
	if ( !str_suffix( pban->name, dnew->host )
	|| !str_prefix( pban->name, dnew->host ) )
	{
	    write_to_descriptor( desc,
		"Your site has been banned from this Mud.\n\r", 0 );
	    close( desc );
	    free_string( dnew->host );
	    free_mem( dnew->outbuf, dnew->outsize );
	    dnew->next		= descriptor_free;
	    descriptor_free	= dnew;
	    return;
	}
    }

    /*
     * Init descriptor data.
     */
    dnew->next			= descriptor_list;
    descriptor_list		= dnew;

    /*
     * Send the greeting.
     */
    {
	extern char * help_greeting;
	extern char * help_greeting2;
	extern char * help_greeting3;
        extern char * help_greeting4;
        extern char * help_greeting5;

	/*
	 * 2-5 disabled
	 */
	switch (dice(1,1))
	{
	default:
	case 1:
	if ( help_greeting[0] == '.' )
	    write_to_buffer( dnew, help_greeting+1, 0 );
	else
	    write_to_buffer( dnew, help_greeting  , 0 );
	break;

	case 2:
	if ( help_greeting2[0] == '.' )
	    write_to_buffer( dnew, help_greeting2+1, 0 );
	else
	    write_to_buffer( dnew, help_greeting2  , 0 );
	break;

	case 3:
	if ( help_greeting3[0] == '.' )
	    write_to_buffer( dnew, help_greeting3+1, 0 );
	else
	    write_to_buffer( dnew, help_greeting3  , 0 );
	break;

	case 4:
	if ( help_greeting4[0] == '.' )
	    write_to_buffer( dnew, help_greeting4+1, 0 );
	else
	    write_to_buffer( dnew, help_greeting4  , 0 );
	break;

	case 5:
	if ( help_greeting5[0] == '.' )
	    write_to_buffer( dnew, help_greeting5+1, 0 );
	else
	    write_to_buffer( dnew, help_greeting5  , 0 );
	break;
	}
    }

    return;
}
#endif



void close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch;
    struct snoop_data *snoop;

    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by != NULL )
    {
	for ( snoop = dclose->snoop_by; snoop != NULL; snoop = snoop->next )
	write_to_buffer( snoop->d,
	    "Your victim has left the game.\n\r", 0 );
    }

    {
      DESCRIPTOR_DATA *d;

      for ( d = descriptor_list; d != NULL; d = d->next )
      {
	for ( snoop = d->snoop_by; snoop != NULL; snoop = snoop->next )
	    if ( snoop->d == dclose )
	    {
		remove_snoop( d, snoop );
	    }
      }
    }

    if ( ( ch = dclose->character ) != NULL )
    {
	sprintf( log_buf, "Closing link to %s.", ch->name );
	log_string( log_buf );
	if ( dclose->connected == CON_PLAYING )
	{
	   act( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM, FALSE );
	   sprintf( log_buf, "%s has lost link.", ch->name );
	   mudlog( log_buf, ch->level );
	   ch->desc = NULL;
           /* ok, now we want to boot people who are not in the war */
	   if ( !in_main_war( ch ) )
		do_quit( ch, "" );
	}
	else
	{
	    free_char( dclose->character );
	}
    }

    if ( d_next == dclose )
	d_next = d_next->next;   

    if ( dclose == descriptor_list )
    {
	descriptor_list = descriptor_list->next;
    }
    else
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d != NULL )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }

    close( dclose->descriptor );
    free_string( dclose->host );
    dclose->next	= descriptor_free;
    descriptor_free	= dclose;
#if defined(MSDOS) || defined(macintosh)
    exit(1);
#endif
    return;
}



bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf );
	write_to_descriptor( d->descriptor,
	    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	return FALSE;
    }

    /* Snarf input. */
#if defined(macintosh)
    for ( ; ; )
    {
	int c;
	c = getc( stdin );
	if ( c == '\0' || c == EOF )
	    break;
	putc( c, stdout );
	if ( c == '\r' )
	    putc( '\n', stdout );
	d->inbuf[iStart++] = c;
	if ( iStart > sizeof(d->inbuf) - 10 )
	    break;
    }
#endif

#if defined(MSDOS) || defined(unix)
    for ( ; ; )
    {
	int nRead;

	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart );
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string( "EndOF encountered on read." );
	    return FALSE;
	}
	else if ( errno == EWOULDBLOCK )
	    break;
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }
#endif

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( d->inbuf[i] == '\0' )
	    return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( k >= MAX_INPUT_LENGTH - 2 )
	{
	    write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

	    /* skip the rest of the line */
	    for ( ; d->inbuf[i] != '\0'; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];

	/* double color code symbol */
	if ( d->inbuf[i] == '#' )
	    d->incomm[k++] = '#';
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */
    if ( k > 1 || d->incomm[0] == '!' )
    {
    	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	{
	    d->repeat = 0;
	}
	else
	{
	    if ( ++d->repeat >= 30 )
	    {
		sprintf( log_buf, "%s input spamming!", d->host );
		log_string( log_buf );
/* temporary disabling ..... Bruce was here
		write_to_descriptor( d->descriptor,
		    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
		strcpy( d->incomm, "quit" );
 */
	    }
	}
    }

    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
    return;
}



/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    extern bool merc_down;

    /*
     * Bust a prompt.
     */
    if ( fPrompt && !merc_down && d->connected == CON_PLAYING )
    {
	CHAR_DATA *ch;

	ch = d->original ? d->original : d->character;
	if ( IS_SET(ch->act, PLR_BLANK) )
	    write_to_buffer( d, "\n\r", 2 );

	if ( IS_SET(ch->act, PLR_PROMPT) )
	{
	    char buf[40];

	    ch = d->character;

	    sprintf( buf, "#P<%dhp %dm> #0", ch->hit, ch->mana );
	    send_to_char( buf, ch );
/*
	    sprintf( buf, "<%d%%hp %d%%m> ", percent, m_percent );

	    write_to_buffer( d, buf, 0 );
*/
	}

	if ( IS_SET(ch->act, PLR_TELNET_GA) )
	    write_to_buffer( d, go_ahead_str, 0 );
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by != NULL )
    {
	char buf[MAX_STRING_LENGTH];
	struct snoop_data *snoop;
	sprintf( buf, "%%(%s) ", d->character == NULL ? "null" : d->character->name );
	for ( snoop = d->snoop_by; snoop != NULL; snoop = snoop->next )
	{
	    send_to_char( buf, snoop->d->character );
	    write_to_buffer( snoop->d, d->outbuf, d->outtop );
	    write_to_buffer( snoop->d, "%% %%\n\r", 2 );
	}
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}


/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
  /*
   * Find length in case caller didn't.
   */
  if ( length <= 0 )
    length = strlen(txt);

  if ( length > MAX_STRING_LENGTH )
    length = MAX_STRING_LENGTH;

  /*
   * Initial \n\r if needed.
   */
  if ( d->outtop == 0 && !d->fcommand )
  {
    d->outbuf[0]	= '\n';
    d->outbuf[1]	= '\r';
    d->outtop	= 2;
  }

  /*
   * Expand the buffer as needed.
   */
  while ( d->outtop + length >= d->outsize )
  {
    char *outbuf;

    outbuf      = (char *) alloc_mem( 2 * d->outsize );
    strncpy( outbuf, d->outbuf, d->outtop );
    free_mem( d->outbuf, d->outsize );
    d->outbuf   = outbuf;
    d->outsize *= 2;
  }

  /*
   * Copy.
   */
  strcpy( d->outbuf + d->outtop, txt );
  d->outtop += length;
  return;
}



/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;
    char *p_text;

#if defined(macintosh) || defined(MSDOS)
    if ( desc == 0 )
	desc = 1;
#endif

    p_text = process_color( desc, txt, length );

    length = strlen(p_text);

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 4096 );
	if ( ( nWrite = write( desc, p_text + iStart, nBlock ) ) < 0 )
	    { perror( "Write_to_descriptor" ); return FALSE; }
    } 

    return TRUE;
}



/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    BAN_DATA *pban;
    char *pwdnew;
    char *p;
/*
    int iClass;
*/
    bool fOld;
    extern void set_class_profile( CHAR_DATA *ch );
    extern void update_last( CHAR_DATA *ch );

    while ( isspace(*argument) )
	argument++;

    ch = d->character;

    switch ( d->connected )
    {

    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d );
	return;

    case CON_GET_NAME:
	if ( argument[0] == '\0' )
	{
	    close_socket( d );
	    return;
	}

	argument[0] = UPPER(argument[0]);
	if ( !check_parse_name( argument ) )
	{
	    write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
	    return;
	}

	fOld = load_char_obj( d, argument );
	ch   = d->character;

	if ( IS_SET(ch->act, PLR_DENY) )
	{
	    sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
	    log_string( log_buf );
	    write_to_buffer( d, "You are denied access.\n\r", 0 );
	    close_socket( d );
	    return;
	}

	if ( check_reconnect( d, argument, FALSE ) )
	{
	    fOld = TRUE;
	}
	else
	{
	    if ( wizlock && (ch->level < 36 ) )
	    {
		write_to_buffer( d, "The game is wizlocked.\n\r", 0 );
		close_socket( d );
		return;
	    }
	}

	if ( fOld )
	{
	    /* Old player */
	    write_to_buffer( d, "Password: ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	}
	else
	{
	    /* New player */
	    sprintf( buf, "Did I get that right, %s (Y/N)? ", argument );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;

    case CON_GET_OLD_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Wrong password.  One more try.\n\r", 0 );

	    write_to_buffer( d, "Password: ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD2;

	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );

	check_playing( d, ch->name );

	if ( check_reconnect( d, ch->name, TRUE ) )
	    return;

	sprintf( log_buf, "%s@%s has connected.", ch->name, d->host );
	log_string( log_buf );
	if ( IS_HERO(ch) )
	    do_help( ch, str_imotd );
	do_help( ch, str_motd );
	d->connected = CON_READ_MOTD;
	break;

    case CON_GET_OLD_PASSWORD2:
	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
		write_to_buffer( d, "Wrong password.\n\rShutting down.\n\r", 0 );
		close_socket( d );
	
		return;
	}

	write_to_buffer( d, echo_on_str, 0 );

	check_playing( d, ch->name );

	if ( check_reconnect( d, ch->name, TRUE ) )
		return;

	sprintf( log_buf, "%s@%s has connected.", ch->name, d->host );
	log_string( log_buf );
	if ( IS_HERO(ch) )
		do_help( ch, str_imotd );
	do_help( ch, str_motd );
	d->connected = CON_READ_MOTD;
	break;

    case CON_CONFIRM_NEW_NAME:
	switch ( *argument )
	{
	case 'y': case 'Y':

    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    for ( pban = restrict_list; pban != NULL; pban = pban->next )
    {
	if ( !str_suffix( pban->name, ch->desc->host )
	|| !str_prefix( pban->name, ch->desc->host ) )
	{
	    write_to_descriptor( ch->desc->descriptor,
		"Due to abuse, no new characters from your site can be created.\n\r", 0 );
	    write_to_descriptor( ch->desc->descriptor,
		"Email: pkmud@pkmud.net for a character.\n\r", 0 );
	    extract_char( ch, TRUE );
	    close_socket( ch->desc );
	    return;
	}
    }

	    sprintf( buf, "New character.\n\rGive me a password for %s: %s",
		ch->name, echo_off_str );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;

	case 'n': case 'N':
	    write_to_buffer( d, "Ok, what IS it, then? ", 0 );
	    free_char( d->character );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer( d, "Please type Yes or No? ", 0 );
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strlen(argument) < 5 )
	{
	    write_to_buffer( d,
		"Password must be at least five characters long.\n\rPassword: ",
		0 );
	    return;
	}

	pwdnew = crypt( argument, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
		write_to_buffer( d,
		    "New password not acceptable, try again.\n\rPassword: ",
		    0 );
		return;
	    }
	}

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd	= str_dup( pwdnew );
	write_to_buffer( d, "Please retype password: ", 0 );
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Passwords don't match.\n\rRetype password: ",
		0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );
	write_to_buffer( d, "What is your sex (M/F/N)? ", 0 );
	d->connected = CON_GET_NEW_SEX;
	break;

    case CON_GET_NEW_SEX:
	switch ( argument[0] )
	{
	case 'm': case 'M': ch->sex = SEX_MALE;    break;
	case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
	case 'n': case 'N': ch->sex = SEX_NEUTRAL; break;
	default:
	    write_to_buffer( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
	    return;
	}

	ch->char_class = CLASS_MAGE;
	strcpy( buf, "All characters are set to mages initially.\n\rChange your class within the game with the CHANGECLASS command.\n\r**PRESS RETURN**\n\r" );
	write_to_buffer( d, buf, 0 );
	d->connected = CON_GET_NEW_CLASS;

/* all this is unnessessary, we will set all to mages and let
   change later */
/*
	strcpy( buf, "Select a class [" );
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    if ( iClass > 0 )
		strcat( buf, " " );
	    strcat( buf, class_table[iClass].who_name );
	}
	strcat( buf, "]: " );
	write_to_buffer( d, buf, 0 );
	d->connected = CON_GET_NEW_CLASS;
*/
	break;

    case CON_GET_NEW_CLASS:
/* unneccessary at the moment */
/*
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    if ( !str_cmp( argument, class_table[iClass].who_name ) )
	    {
		ch->class = iClass;
		break;
	    }
	}

	if ( iClass == MAX_CLASS )
	{
	    write_to_buffer( d,
		"That's not a class.\n\rWhat IS your class? ", 0 );
	    return;
	}
*/

	sprintf( log_buf, "%s@%s new player.", ch->name, d->host );
	ch->pcdata->createsite	= str_dup( d->host );
	log_string( log_buf );
	write_to_buffer( d, "\n\r", 2 );
	do_help( ch, str_motd );
	d->connected = CON_READ_MOTD;

        sprintf( log_buf, "New Player %s created at %s.",ch->name,
                 (char *) ctime( &current_time ));
        twitter.push( log_buf, 1 );
	break;

    case CON_READ_MOTD:
	write_to_buffer( d, 
    "\n\rWelcome to PK Mud.  May your visit here be ... Deadly.\n\r",
	    0 );
	ch->next	= char_list;
	char_list	= ch;
	d->connected	= CON_PLAYING;
	ch->pcdata->site	= str_dup( d->host );

        if ( ch->level >= -1)
        {
	    REMOVE_BIT( ch->act, PLR_NOPLAY );
            ch->gold = 0;
            ch->hitroll = 0;
            ch->damroll = 0;
            ch->saving_throw = 0;
            ch->pcdata->perm_str = 14;
            ch->pcdata->perm_int = 14;
            ch->pcdata->perm_wis = 14;
            ch->pcdata->perm_dex = 14;
            ch->pcdata->perm_con = 14;
            ch->pcdata->mod_str  = 0;
            ch->pcdata->mod_int  = 0;
            ch->pcdata->mod_wis  = 0;
            ch->pcdata->mod_dex  = 0;
            ch->pcdata->mod_con  = 0;
            ch->armor = 100;
            ch->pcdata->war_num = WAR_NUM_DEAD;
	}

/* initialize class statistics */
	set_class_profile( ch );

	if ( ch->level == 0 )
	{

	    ch->level	= 30;
	    ch->exp	= 0;
            ch->hit	= 1;
            ch->mana	= 1;
/* to stop multiple plays */
            ch->pcdata->last_war = 1;
/* everybody starts with one no kills, no deaths and no wins */
            ch->pcdata->kills = 0;
            ch->pcdata->deaths = 0;
            ch->pcdata->wins = 0;
	    sprintf( buf, "the Title-less Player Killer" );
	    set_title( ch, buf );

/* er, ill send em to temple rather than school... */
/* which is now map room */
	    char_to_room( ch, get_room_index( ROOM_VNUM_MAP_ROOM ) );
            SET_BIT( ch->act, PLR_AUTOEXIT );
            SET_BIT( ch->act, PLR_BRIEF );
            REMOVE_BIT( ch->act, PLR_BLANK );
	}
	else if ( IS_IMMORTAL(ch) )
	{
	  ch->pcdata->wizinvis_level = ch->level;
	  char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
          if( ch->level >= 40 )
          {
            ch->max_hit = 1000;
            ch->max_mana = 1000;
            do_sset( ch, "self all 100"  );
          }
          restore_character( ch  );
	}
	else
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_MAP_ROOM ) );
	}

/*	if( ch->nojoin < 3 ) */
		ch->nojoin = -1;

	act( "$n has entered the game.", ch, NULL, NULL, TO_ROOM, FALSE );
	sprintf( log_buf, "%s@%s has entered the game.",ch->name,ch->desc->host);
	mudlog(log_buf, ch->level );

        if ( ch->level == 30 )
        {
	  sprintf( log_buf, "%s logged on at %s.",ch->name,
                   (char *) ctime( &current_time ));
          twitter.push( log_buf, 0 );
        }

	do_look( ch, "auto" );
	update_last( ch );
	break;
    }

    return;
}



/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
    /*
     * Reserved words.
     */
    if ( is_name( name, "all auto end immortal self someone" ) )
	return FALSE;

    /*
     * Length restrictions.
     */
    if ( strlen(name) <  3 )
	return FALSE;

#if defined(MSDOS)
    if ( strlen(name) >  8 )
	return FALSE;
#endif

#if defined(macintosh) || defined(unix)
    if ( strlen(name) > MAX_NAME_LENGTH )
	return FALSE;
#endif

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;
	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;
    }

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
	extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	MOB_INDEX_DATA *pMobIndex;
	int iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
	    for ( pMobIndex  = mob_index_hash[iHash];
		  pMobIndex != NULL;
		  pMobIndex  = pMobIndex->next )
	    {
		if ( is_name( name, pMobIndex->player_name ) )
		    return FALSE;
	    }
	}
    }

    return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	if ( !IS_NPC(ch)
	&& ( !fConn || ch->desc == NULL )
	&&   !str_cmp( d->character->name, ch->name ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
       
		send_to_char( "Reconnecting.\n\r", ch );
		act( "$n has reconnected.", ch, NULL, NULL, TO_ROOM, FALSE );
		sprintf( log_buf, "%s@%s reconnected.", ch->name, d->host );
		log_string( log_buf );
		mudlog( log_buf, ch->level );
		d->connected = CON_PLAYING;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



/*
 * Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold; dold = dold->next )
    {
	if ( dold != d
	&&   dold->character != NULL
	&&   dold->connected != CON_GET_NAME
	&&   dold->connected != CON_GET_OLD_PASSWORD
	&&   !str_cmp( name, dold->original
	         ? dold->original->name : dold->character->name ) )
	{
            write_to_buffer( dold, "Your body has been usurped.\n\r", 0 );
            write_to_buffer( d, "Stealing old body.\n\r", 0 );

            close_socket( dold );

	    return TRUE;
	} 
  }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if ( ch == NULL
    ||   ch->desc == NULL
    ||   ch->desc->connected != CON_PLAYING
    ||   ch->was_in_room == NULL 
    ||   ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) )
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );
    ch->was_in_room	= NULL;
    act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM, FALSE );
    return;
}


/*
 * Write to one char.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    if ( txt != NULL && ch->desc != NULL )
	write_to_buffer( ch->desc, txt, strlen(txt) );
    return;
}

 
void act( const char *format, CHAR_DATA *ch, const void *arg1,
  const void *arg2, int type, bool fAlways )
{
    static char * const he_she	[] = { "it",  "he",  "she" };
    static char * const him_her	[] = { "it",  "him", "her" };
    static char * const his_her	[] = { "its", "his", "her" };
    static char * const boy_girl [] = { "thing", "boy", "girl" };

    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *point;

    if ( format == NULL || format[0] == '\0' )
	return;

    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
	if ( vch == NULL )
	{
	    bug( "Act: null vch with TO_VICT.", 0 );
	    return;
	}
	to = vch->in_room->people;
    }
    
    for ( ; to != NULL; to = to->next_in_room )
    {
	if ( to->desc == NULL || !IS_AWAKE(to) )
	    continue;

	if ( type == TO_CHAR && to != ch )
	    continue;
	if ( type == TO_VICT && ( to != vch || to == ch ) )
	    continue;
	if ( type == TO_ROOM && to == ch )
	    continue;
	if ( type == TO_NOTVICT && (to == ch || to == vch) )
	    continue;

	if ( !fAlways && !can_see( to, ch ) )
	    continue;

	point	= buf;
	str	= format;
	while ( *str != '\0' )
	{
	    if ( *str != '$' )
	    {
		*point++ = *str++;
		continue;
	    }
	    ++str;

	    if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' )
	    {
		bug( "Act: missing arg2 for code %d.", *str );
		i = " <@@@> ";
	    }
	    else
	    {
		switch ( *str )
		{
		default:  bug( "Act: bad code %d.", *str );
			  i = " <@@@> ";				break;
		case 't': i = (char *) arg1;				break;
		case 'T': i = (char *) arg2;          			break;
		case 'n': i = PERS( ch,  to  );				break;
		case 'N': i = PERS( vch, to  );				break;
		case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];	break;
		case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];	break;
		case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];	break;
		case 'M': i = him_her [URANGE(0, vch ->sex, 2)];	break;
		case 's': i = his_her [URANGE(0, ch  ->sex, 2)];	break;
		case 'S': i = his_her [URANGE(0, vch ->sex, 2)];	break;
		case 'y': i = boy_girl [URANGE(0, ch  ->sex, 2)];	break;
		case 'Y': i = boy_girl [URANGE(0, vch ->sex, 2)];	break;

		case 'p':
		    i = can_see_obj( to, obj1 )
			    ? obj1->short_descr
			    : "something";
		    break;

		case 'P':
		    i = can_see_obj( to, obj2 )
			    ? obj2->short_descr
			    : "something";
		    break;

		case 'd':
		    if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
		    {
			i = "door";
		    }
		    else
		    {
			one_argument( (char *) arg2, fname );
			i = fname;
		    }
		    break;
		}
	    }
		
	    ++str;
	    while ( ( *point = *i ) != '\0' )
		++point, ++i;
	}

	*point++ = '\n';
	*point++ = '\r';
	buf[0]   = UPPER(buf[0]);
	write_to_buffer( to->desc, buf, point - buf );
    }

    return;
}

/*
 * Macintosh support functions.
 */
#if defined(macintosh)
int gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif

void set_banned_site( char *s_name )
{
    BAN_DATA * pban = NULL;

    pban = (BAN_DATA *) alloc_perm( sizeof( *pban));
    pban->name = str_dup(s_name);
    pban->next = ban_list;
    ban_list = pban;
}

void set_restricted_site( char *s_name )
{
    BAN_DATA * pban = NULL;

    pban = (BAN_DATA *) alloc_perm( sizeof( *pban));
    pban->name = str_dup(s_name);
    pban->next = restrict_list;
    restrict_list = pban;
}

void set_no_rating_site( char *s_name )
{
    BAN_DATA * pban = NULL;

    pban = (BAN_DATA *) alloc_perm( sizeof( *pban));
    pban->name = str_dup(s_name);
    pban->next = no_rating_list;
    no_rating_list = pban;
}


void permanent_bans( )
{
#if 0
    set_banned_site( "ox.ac.uk" );  /* god hacker */
    set_restricted_site( "ip68-99-203-150.ph.ph.cox.net" ); /* merlin */
    set_restricted_site( ".sttl.uswest.net" ); /* synge */
    set_restricted_site( ".aol.com" ); /* synge */
    set_restricted_site( "www.whiteriver.wednet.edu" ); /* synge */
    set_restricted_site( "64-93-34-186.client.dsl.net" ); /* cold */
    set_banned_site( "neteze.com" ); /* attax */
    set_banned_site( "216-174-250-98.atgi.net" ); /* attax */
    set_banned_site( "dsl-65-184-88-225.telocity.com" ); /* attax */
    set_banned_site( "sirius.net" );  /* artemis.. annoying */
    set_banned_site( "ted.net" ); /* attax */
    set_no_rating_site( "aol.com" );
#endif
    return;
}


void get_start_eq( CHAR_DATA *ch )
{
  OBJ_DATA *obj;
  extern int making_corpse;

  making_corpse = TRUE;
  while ( ch->carrying != NULL )
  {
    if ( ch->carrying->wear_loc != WEAR_NONE )
      unequip_char( ch, ch->carrying );

    extract_obj( ch->carrying, "get_start_eq" );
  }
  making_corpse = FALSE;


  obj = create_object( get_obj_index(OBJ_VNUM_BRONZE_HELMET) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  obj = create_object( get_obj_index(OBJ_VNUM_BRONZE_BREAST) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  obj = create_object( get_obj_index(OBJ_VNUM_BRONZE_LEGGINGS) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  obj = create_object( get_obj_index(OBJ_VNUM_BRONZE_BOOTS) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  obj = create_object( get_obj_index(OBJ_VNUM_GLINTING_RING) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  obj = create_object( get_obj_index(OBJ_VNUM_GLINTING_RING) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  obj = create_object( get_obj_index(OBJ_VNUM_PURPLE_CLOAK) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  obj = create_object( get_obj_index(OBJ_VNUM_PURPLE_CLOAK) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  obj = create_object( get_obj_index(OBJ_VNUM_SWORDSMANS_GLOVES) );
  obj->cost = 0;
  obj_to_char( obj, ch );


  obj = create_object( get_obj_index(OBJ_VNUM_WEAK_LANTERN) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  obj = create_object( get_obj_index(3051) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  obj = create_object( get_obj_index(OBJ_VNUM_BLACK_DEMON_BLADE) );
  obj->cost = 0;
  obj_to_char( obj, ch );

  return;
}




bool check_no_rating_list( CHAR_DATA *ch, bool fMessage )
{
  BAN_DATA *pban;

  if ( ch->desc == NULL || ch->desc->host == NULL )
    return FALSE;

  if ( IS_SET( ch->act, PLR_RATING_OVERRIDE ) )
    return FALSE;

  for ( pban = no_rating_list; pban != NULL; pban = pban->next )
  {
    if ( !str_suffix( pban->name, ch->desc->host )
    || !str_prefix( pban->name, ch->desc->host ) )
    {
      if ( fMessage == TRUE )
        send_to_char( "Characters from your site can not have ratings.\n\r", ch );
      return TRUE;
    }
  }

  return FALSE;
}

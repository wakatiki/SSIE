
#ifdef OS_WINDOWS

#ifdef MODE_CONSOLE
char * to_ascii_dos( char *in )
	{
	static char t_ascii_dos[256];
	static bool t_ascii_dos_init = false;
	char *out;
	long int i, l;
	
	if ( !in ) return null;
	
	BeginCriticalSection();
	if ( t_ascii_dos_init == false )
		{
		for( i = 0 ; i < 256 ; i++ ) t_ascii_dos[i] = 0;
		t_ascii_dos[(uchar)'à'] = 133;
		t_ascii_dos[(uchar)'â'] = 131;
		t_ascii_dos[(uchar)'ä'] = 132;
		t_ascii_dos[(uchar)'é'] = 8*16+2;
		t_ascii_dos[(uchar)'è'] = 8*16+10;
		t_ascii_dos[(uchar)'ê'] = 8*16+8;
		t_ascii_dos[(uchar)'ë'] = 8*16+9;
		t_ascii_dos[(uchar)'î'] = 8*16+12;
		t_ascii_dos[(uchar)'ï'] = 8*16+11;
		t_ascii_dos[(uchar)'ô'] = 9*16+3;
		t_ascii_dos[(uchar)'ö'] = 9*16+4;
		t_ascii_dos[(uchar)'ù'] = 9*16+7;
		t_ascii_dos[(uchar)'û'] = 9*16+6;
		t_ascii_dos[(uchar)'ü'] = 8*16+1;
		t_ascii_dos[(uchar)'ç'] = 8*16+7;
		t_ascii_dos[(uchar)'°'] = 248;
		
		out = alloc(200);
		
		t_ascii_dos_init = true;
		}
	EndCriticalSection();
	
	l = strlen(in);
	out = memget( l+1 );
	for( i = 0 ; i < l ; i++ )
		out[i] = ( t_ascii_dos[(uchar)(in[i])] ) ? t_ascii_dos[(uchar)(in[i])] : in[i];
	out[i] = 0;
	return out;
	}
#else

#define to_ascii_dos( in )	in

#endif
/*
#define elib_print( s ) \
	if ( s ) \
		{\
		if ( elib_print_socket ) strdcat( &elib_socket_buffer, s, _ );\
		else { \
			if ( elib_print_to_screen ) printf( "%s", to_ascii_dos(s) );\
			if ( ( elib_print_fp )&&( elib_print_fp != stdout ) )\
				fprintf( elib_print_fp, "%s", to_ascii_dos(s) ); \
		}}
*/
#define elib_print( s ) \
	if ( s ) \
		{\
		if ( elib_print_socket ) strdcat( &elib_socket_buffer, s, _ );\
		else { \
			if ( elib_print_to_screen ) printf( "%s", to_ascii_dos(s) );\
			if ( ( elib_print_fp )&&( elib_print_fp != stdout ) )\
				fprintf( elib_print_fp, "%s", s ); \
		}}

#define elib_printn( s, n ) \
	if ( ( s )&&( n > 0 ) ) \
		{\
		if ( elib_print_socket ) strdcat( &elib_socket_buffer, s, _ );\
		else { \
			if ( elib_print_to_screen ) fwrite( stdout, to_ascii_dos(s), n, _ );\
			if ( ( elib_print_fp )&&( elib_print_fp != stdout ) )\
				fwrite( elib_print_fp, to_ascii_dos(s), n, _ ); \
		}}

#else // OS_UNIX

#define elib_print( s ) \
	if ( s ) \
		{\
		if ( elib_print_socket ) strdcat( &elib_socket_buffer, s, _ );\
		else { \
			if ( elib_print_to_screen ) printf( "%s", s );\
			if ( ( elib_print_fp )&&( elib_print_fp != stdout ) )\
				fprintf( elib_print_fp, "%s", s ); \
		}}

#define elib_printn( s, n ) \
	if ( s ) \
		{\
		if ( elib_print_socket ) strdcat( &elib_socket_buffer, s, _ );\
		else { \
			if ( elib_print_to_screen ) fwrite( stdout, s, n, _ );\
			if ( ( elib_print_fp )&&( elib_print_fp != stdout ) )\
				fwrite( elib_print_fp, s, n, _ ); \
		}}

#endif



//------------------------------------------------------------------------------
FILE *elib_print_fp = NULL;
bool elib_print_to_screen = true;
handle elib_print_socket = 0;
char *elib_socket_buffer = null;

//------------------------------------------------------------------------------
void	print_to_file( char *fname )
//------------------------------------------------------------------------------
	{
	if ( ( elib_print_fp != NULL )&&( elib_print_fp != stdout ) )
		fclose( elib_print_fp );
	if ( elib_print_fp = fopen( fname, "ab" ) )
		elib_print_to_screen = false;
	else
		elib_print_to_screen = true;
	return;
	}



//------------------------------------------------------------------------------
void	print_to_socket( handle hSock )
//------------------------------------------------------------------------------
	{
	elib_print_socket = hSock;
	strdcpy( &elib_socket_buffer, "", _ );
	}
	
	
//------------------------------------------------------------------------------
void	print_to_file_and_screen( char *fname )
//------------------------------------------------------------------------------
	{
	if ( ( elib_print_fp != NULL )&&( elib_print_fp != stdout ) )
		fclose( elib_print_fp );
	elib_print_fp = fopen( fname, "ab" );
	elib_print_to_screen = true;
	}

//------------------------------------------------------------------------------
FILE *	print_get_fp( void )
//------------------------------------------------------------------------------
	{
	return ( elib_print_fp ) ? elib_print_fp : stdout;
	}

//------------------------------------------------------------------------------
bool	print_to_screen( void )
//------------------------------------------------------------------------------
	{
	return elib_print_to_screen;
	}

//------------------------------------------------------------------------------
void	print( char *msg, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg;

	if ( ! elib_print_fp ) elib_print_fp = stdout;

	if ( msg == _ ) return;
	if ( msg == __ )
		{
		if ( !elib_print_socket )
			{
			fflush( stdout );
			if ( elib_print_fp ) fflush( elib_print_fp );
			}
		else
			{
			tcp_ssend( elib_print_socket, elib_socket_buffer, -1 );
			strdcpy( &elib_socket_buffer, "", _ );
			}
		return;
		}
	

	elib_print( msg );
	va_start(args, msg); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) return;
		if ( arg == __ )
			{
			if ( !elib_print_socket )
				{
				fflush( stdout );
				if ( elib_print_fp ) fflush( elib_print_fp );
				}
			else
				{
				tcp_ssend( elib_print_socket, elib_socket_buffer, -1 );
				strdcpy( &elib_socket_buffer, "", _ );
				}
			return;
			}
		elib_print( arg );
		}
	}
	


//------------------------------------------------------------------------------
void	printn( char *msg, int len )
//------------------------------------------------------------------------------
	{
//	if ( ! elib_print_fp ) elib_print_fp = stdout;

	elib_printn( msg, len );
	}
	


//------------------------------------------------------------------------------
void	_print_ind( int ind, int maxlen, char *msg )
//------------------------------------------------------------------------------
	{
	char *line, *blank = "                                                     "
						 "                                                     ";
	int i, p, p0;

	
//	if ( ! elib_print_fp ) elib_print_fp = stdout;
	
	for( p0 = p = 0 ; msg[p0] ; )
		{
		p = strpos( msg, "\n", p0 );
		if ( p >= 0 )
			{
			if ( ( p )&&( msg[p-1] == '\r' ) )
				line = substr( msg, p0, p-p0-1 );
			else
				line = substr( msg, p0, p-p0 );
			p0 = p+1;
			}
		else
			{
			line = &(msg[p0]);
			p0 = strlen( msg );
			}
		
		while( ind + strlen( line ) > maxlen )
			{
			printn( blank, ind );
			printn( line, maxlen-ind );
			print( BR, _ );
			line = &(line[maxlen-ind]);
			}
		
		printn( blank, ind );
		print( line, _ );
		if ( p >= 0 ) print( BR, _ );
		}
	}
	
	

//------------------------------------------------------------------------------
void	print_ind( int ind, int maxlen, char *msg, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg;
	int bFlush = false;
	char *s = null;

//	if ( ! elib_print_fp ) elib_print_fp = stdout;

	strdcpy( &s, msg, _ );
	va_start(args, msg); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		if ( arg == __ )
			{
			bFlush = true;
			break;
			}
		strdcat( &s, arg, _ );
		}

	_print_ind( ind, maxlen, s );
	print( BR, _ );
	if ( bFlush )
		{
		if ( !elib_print_socket )
			{
			fflush( stdout );
			if ( elib_print_fp ) fflush( elib_print_fp );
			}
		else
			{
			tcp_ssend( elib_print_socket, elib_socket_buffer, -1 );
			strdcpy( &elib_socket_buffer, "", _ );
			}
		}
	
	free( s );
	}


	
	
//------------------------------------------------------------------------------

#ifdef OS_UNIX
#include <termios.h>

static struct termios   save_termios;
static int              term_saved;
int tty_raw( void )
	{
	int fd = fileno(stdin);
	
	// RAW! mode
	struct termios  buf; 
	
	if (tcgetattr(fd, &save_termios) < 0) // get the original state
		return -1;
		
	buf = save_termios;
	buf.c_lflag &= ~(ECHO | ICANON /*| IEXTEN*/ | ISIG);	// echo off, canonical mode off, extended input processing off,
														// signal chars off
	buf.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);	// no SIGINT on BREAK, CR-toNL off, input parity
														//   check off, don't strip the 8th bit on input,
														//   ouput flow control off 
	
	buf.c_cflag &= ~(CSIZE | PARENB);	// clear size bits, parity checking off
	buf.c_cflag |= CS8;					// set 8 bits/char
	buf.c_oflag &= ~(OPOST);			// output processing off
	buf.c_cc[VMIN] = 1;					// 1 byte at a time
	buf.c_cc[VTIME] = 0;				// no timer on input
	if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
		return -1;
		
	term_saved = 1;
	return 0;
	}
	
int tty_reset( void )
	{
	int fd = fileno(stdin);

	// set it to normal!
	if (term_saved)
		if (tcsetattr(fd, TCSAFLUSH, &save_termios) < 0)
			return -1;
	return 0;
	} 
	
#endif


//------------------------------------------------------------------------------
int kbget( void )
//------------------------------------------------------------------------------
	{
	
	#ifdef OS_WINDOWS
	int n;
	
	n = getch();
	if ( n == 224 )
		return 500 + (uint)getch();
	else
	if ( n == 0 )
		return 1000 + (uint)getch() - 58;
	return n;
	#else
	static int n, p = -1;
	
	if ( p != -1 )
		{
		n = p;
		p = -1;
		return n;
		}
	tty_raw( );
	n = getchar();
	if ( n == 127 )
		{
		tty_reset( );
		return 8;
		}

	if ( n == 27 )
		{
		n = getchar();
		if ( n != 91 )
			{
			tty_reset( );
			p = n;
			return 27;
			}

		n = getchar();
//		print( "coucou:", itoa(n), BR, _ );
		switch( n )
			{
			case 65 : // Flèche haute
				tty_reset( );
				return 572;

			case 66 : // Flèche bas
				tty_reset( );
				return 580;

			case 67 : // Flèche droite
				tty_reset( );
				return 577;

			case 68 : // Flèche gauche
				tty_reset( );
				return 575;
				
			case 49 : 
				n = getchar();
				switch( n )
					{
					case 126 :
						tty_reset( );
						return 571; // Home

					case 49 :
						getchar();
						tty_reset( );
						return 1001; // F1

					case 50 :
						getchar();
						tty_reset( );
						return 1002; // F2

					case 51 :
						getchar();
						tty_reset( );
						return 1003; // F3

					case 52 :
						getchar();
						tty_reset( );
						return 1004; // F4

					case 53 :
						getchar();
						tty_reset( );
						return 1005; // F5

					case 55 :
						getchar();
						tty_reset( );
						return 1006; // F6

					case 56 :
						getchar();
						tty_reset( );
						return 1007; // F7

					case 57 :
						getchar();
						tty_reset( );
						return 1008; // F8
					}
				return( n );
				
			case 50 : 
				n = getchar();
				switch( n )
					{
					case 126 :
						tty_reset( );
						return 582; // Inser

					case 48 :
						getchar();
						tty_reset( );
						return 1009; // F9

					case 49 :
						getchar();
						tty_reset( );
						return 1010; // F10

					case 51 :
						getchar();
						tty_reset( );
						return 633; // F11

					case 52 :
						getchar();
						tty_reset( );
						return 634; // F12
					}
				tty_reset( );
				return( n );

			case 51 : // Suppr
				getchar();
				tty_reset( );
				return 583;

			case 52 : // Fin
				getchar();
				tty_reset( );
				return 579;

			case 53 : // Pg Up
				getchar();
				tty_reset( );
				return 573;

			case 54 : // Pg down
				getchar();
				tty_reset( );
				return 581;
			}
		
		}
		
	
	tty_reset( );
	return n;
	#endif	


/*
Flèche haute	: 27 91 65 (572)
Flèche bas		: 27 91 66 (580)
Flèche gauche	: 27 91 68 (575)
Flèche droite	: 27 91 67 (577)
Home			: 27 91 49 126 (571)
Inser			: 27 91 50 126 (582)
Suppr			: 27 91 51 126 (583)
Fin				: 27 91 52 126 (579)
Pg up			: 27 91 53 126 (573)
Pg down			: 27 91 54 126 (581)
F1				: 27 91 49 49 126 (1001)
F2				: 27 91 49 50 126 (1002)
F3				: 27 91 49 51 126 (1003)
F4				: 27 91 49 52 126 (1004)
F5				: 27 91 49 53 126 (1005)
F6				: 27 91 49 55 126 (1006)
F7				: 27 91 49 56 126 (1007)
F8				: 27 91 49 57 126 (1008)
F9				: 27 91 50 48 126 (1009)
F10				: 27 91 50 49 126 (1010)
F11				: 27 91 50 51 126 (633)
F12				: 27 91 50 52 126 (634)
*/
	}

void cursor_pos( int col, int line )
	{
	#ifdef OS_WINDOWS	
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD  Pos;
	
	Pos.X = col;
	Pos.Y = line;
	
	SetConsoleCursorPosition(hConsole, Pos);
	#else
	printf( "\033[%u;%uH", line, col );
	#endif
	}












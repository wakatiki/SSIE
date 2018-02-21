
#ifdef OS_WINDOWS

HMODULE hDLL = NULL;
WSADATA WSAData;

#define WSAStartup _WSAStartup
int PASCAL (*WSAStartup)(WORD,LPWSADATA);
#define WSACleanup _WSACleanup
int PASCAL (*WSACleanup)(void);

#define socket _socket
SOCKET PASCAL (*socket)(int,int,int);
#define closesocket _closesocket
int PASCAL (*closesocket)(int);
#define shutdown _shutdown
int PASCAL (*shutdown)(int,int);

#define htons _htons
u_short PASCAL (*htons)(u_short);
#define ntohs _ntohs
u_short PASCAL (*ntohs)(u_short);

#define accept _accept
SOCKET PASCAL (*accept)(SOCKET,struct sockaddr*,int*);
#define bind _bind
int PASCAL (*bind)(SOCKET,const struct sockaddr*,int);
#define listen _listen
int PASCAL (*listen)(SOCKET,int);
#define recv _recv
int PASCAL (*recv)(SOCKET,char*,int,int);
#define send _send
int PASCAL (*send)(SOCKET,const char*,int,int);
#define connect _connect
int PASCAL (*connect)(SOCKET,const struct sockaddr*,int);
#define inet_addr _inet_addr
unsigned long PASCAL (*inet_addr)(const char*);
#define select _select
int PASCAL (*select)(int nfds,fd_set*,fd_set*,fd_set*,const struct timeval*);

#define getsockname _getsockname
int PASCAL (*getsockname)(SOCKET, SOCKADDR_IN*, int*);
#define gethostbyname _gethostbyname
struct hostent * PASCAL (*gethostbyname)(char*);

#define gethostname _gethostname
int PASCAL (*gethostname)(char*, int);


#define tcp_init 	if ( !hDLL ) _tcp_init()

#define socklen_t int

//------------------------------------------------------------------------------				
void _tcp_init( void )
//------------------------------------------------------------------------------				
    {
    hDLL = LoadLibrary("ws2_32.dll");
    
    WSAStartup = (int PASCAL (*)(WORD,LPWSADATA))GetProcAddress(hDLL, "WSAStartup");
    WSAStartup(MAKEWORD(2,0), &WSAData);
    WSACleanup = (int PASCAL (*)(void))GetProcAddress(hDLL, "WSACleanup");

    socket = (SOCKET PASCAL (*)(int,int,int))GetProcAddress(hDLL, "socket");
    closesocket = (int PASCAL (*)(int))GetProcAddress(hDLL, "closesocket");
    shutdown = (int PASCAL (*)(int,int))GetProcAddress(hDLL, "shutdown");

    htons = (u_short PASCAL (*)(u_short))GetProcAddress(hDLL, "htons");
    ntohs = (u_short PASCAL (*)(u_short))GetProcAddress(hDLL, "ntohs");
    connect = (int PASCAL (*)(SOCKET,const struct sockaddr*,int))GetProcAddress(hDLL, "connect");
    accept = (SOCKET PASCAL (*)(SOCKET,struct sockaddr*,int*))GetProcAddress(hDLL, "accept");
    bind = (int PASCAL (*)(SOCKET,const struct sockaddr*,int))GetProcAddress(hDLL, "bind");
    listen = (int PASCAL (*)(SOCKET,int))GetProcAddress(hDLL, "listen");
    recv = (int PASCAL (*)(SOCKET,char*,int,int))GetProcAddress(hDLL, "recv");
    send = (int PASCAL (*)(SOCKET,const char*,int,int))GetProcAddress(hDLL, "send");
    select = (int PASCAL (*)(int nfds,fd_set*,fd_set*,fd_set*,const struct timeval*))GetProcAddress(hDLL, "select");
    inet_addr = (unsigned long PASCAL (*)(const char*))GetProcAddress(hDLL, "inet_addr");

	getsockname = (int PASCAL (*)(SOCKET, SOCKADDR_IN*, int*))GetProcAddress(hDLL, "getsockname");
	gethostbyname = (struct hostent * PASCAL (*)(char*))GetProcAddress(hDLL, "gethostbyname");
	gethostname = (int PASCAL (*)(char*, int))GetProcAddress(hDLL, "gethostname");
    }

//------------------------------------------------------------------------------				
char * tcp_myIP( uint nOrder )
//------------------------------------------------------------------------------				
	{
	static char my_ip[16][16], n = 0;
	int i, p = 0, p0;
	char *s;

	if ( !n ) 
		{
		BeginCriticalSection();
	
		if ( !n ) 
			{
			if ( s = System( null, "ipconfig" ) )
				{
				while(1)
					{
					p = strpos( s, "Adresse IPv4", p0=p );
					if ( p < 0 )	p = strpos( s, "Adresse IP", p0 );
					if ( p < 0 )	p = strpos( s, "IPv4 Address", p0 );
					if ( p < 0 )
						{
						if ( !n ) strcpy( my_ip[n++], "IP not found" );
						break;
						}
					p = strpos( s, ":", (uint)( p0=p+1 ) );
					if ( p < 0 ) { p = p0; continue; }
		
					for( p+=2, i = 0 ; ( s[p] != 0 )&&(s[p] != 10)&&(s[p] != 13) ; p++, i++ )
						{
						my_ip[n][i] = s[p];
						}
					my_ip[n][i] = 0;
		
					if ( strcmp( my_ip[n], "0.0.0.0" ) ) n++;
					continue;
					}
				}
			else
				strcpy( my_ip[n++], "IP not found" );
			}

		EndCriticalSection();
		}

	return ( nOrder < n ) ? my_ip[nOrder] : NULL;
	}

//------------------------------------------------------------------------------				
#else	// OS_UNIX
//------------------------------------------------------------------------------				

#define tcp_init

//------------------------------------------------------------------------------				
char * tcp_myIP( uint nOrder )
//------------------------------------------------------------------------------				
	{
	static char n = 0, my_ip[16][30];
	char *s = null, *w = null;
	handle h;
	
	if ( ! n )
		{
		#ifdef OS_LINUX
//		System( &s, "/sbin/ifconfig -a|grep \"inet ad\"|"
//					"awk '{print substr($2,5,50);}'|grep -v \"127.0.0.1\"" );

		System( &s, "/sbin/ifconfig -a|grep \"inet \"|"
					"awk '{print $2;}'|grep -v \"127.0.0.1\"" );


/* RHEL 5-6
eth0      Link encap:Ethernet  HWaddr 00:50:56:BF:5A:45
          inet addr:10.200.253.35  Bcast:10.200.253.255  Mask:255.255.255.0
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:5353768 errors:0 dropped:0 overruns:0 frame:0
          TX packets:71201 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:477053746 (454.9 MiB)  TX bytes:17302818 (16.5 MiB)

lo        Link encap:Local Loopback
          inet addr:127.0.0.1  Mask:255.0.0.0
          UP LOOPBACK RUNNING  MTU:16436  Metric:1
*/


/* RHEL 7 :

kpw36hr3.kqg.egd.edf.fr:ssie:/appli/ssie7/conf $ ifconfig -a
ens32: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 10.202.11.80  netmask 255.255.255.192  broadcast 10.202.11.127
        ether 00:50:56:bc:78:fa  txqueuelen 1000  (Ethernet)
        RX packets 3455942  bytes 247453291 (235.9 MiB)
        RX errors 0  dropped 164  overruns 0  frame 0
        TX packets 846310  bytes 97365575 (92.8 MiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

ens34: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 10.202.14.78  netmask 255.255.255.192  broadcast 10.202.14.127
        ether 00:50:56:bc:1f:d5  txqueuelen 1000  (Ethernet)
        RX packets 959524  bytes 97423559 (92.9 MiB)
        RX errors 0  dropped 81  overruns 0  frame 0
        TX packets 17327  bytes 1040456 (1016.0 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        loop  txqueuelen 0  (Local Loopback)
        RX packets 14  bytes 1400 (1.3 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 14  bytes 1400 (1.3 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
*/

		#else
		System( &s, "ifconfig -a|grep \"inet \"|"
					"awk '{print $2;}'|grep -v \"127.0.0.1\"" );
		#endif

		if ( s )
			{
			h = text_init( s, "" );
			while( text_getword( &w, h ) != NULL )
				{
				trim(w);
				if ( !w[0] ) continue;
				if ( !strnicmp( w, "addr:", 5 ) )
					strcpy( my_ip[n++], &(w[5]) );
				else
				if ( w[0] == ':' )
					strcpy( my_ip[n++], &(w[1]) );
				else
					strcpy( my_ip[n++], w );
				}
			}
		else
			strcpy( my_ip[n++], "127.0.0.1" );
		}
	
	if ( s ) free( s );
	if ( w ) free( w );
	
	return ( nOrder < n ) ? my_ip[nOrder] : NULL;
	}
	
#define SOCKET int
#define SOCKADDR_IN struct sockaddr_in
#define SOCKADDR struct sockaddr

#endif

//------------------------------------------------------------------------------				
// UNIX ET WINDOWS
//------------------------------------------------------------------------------				

//------------------------------------------------------------------------------				
bool is_ip_valide( char *ip )
//------------------------------------------------------------------------------				
	{
	int p1, p2, n;
	char s[16];
	
	if ( !ip ) return false;
	p1 = 0;
	p2 = strpos( ip, ".", 0 );
	if ( p2 < 0 ) return false;
	strncpy( s, ip, p2 );
	if ( ! is_numeric( s ) ) return false;
	n = atoi( s );
	if ( n > 255 ) return false;
	
	p1 = p2+1;
	p2 = strpos( ip, ".", p1 );
	if ( p2 < 0 ) return false;
	strncpy( s, &(ip[p1]), p2-p1 );
	if ( ! is_numeric( s ) ) return false;
	n = atoi( s );
	if ( n > 255 ) return false;
	
	p1 = p2+1;
	p2 = strpos( ip, ".", p1 );
	if ( p2 < 0 ) return false;
	strncpy( s, &(ip[p1]), p2-p1 );
	if ( ! is_numeric( s ) ) return false;
	n = atoi( s );
	if ( n > 255 ) return false;
	
	p1 = p2+1;
	n = strpos( ip, ":", p1 );
	if ( n < 0 )
		strcpy( s, &(ip[p1]) );
	else
		{
		if ( ! is_numeric( &(ip[n+1]) ) ) return false;
		strncpy( s, &(ip[p1]), n-p1 );
		n = atoi( &(ip[n+1]) );
		if ( ( n < 10 )||( n > 65535 ) ) return false;
		}
	if ( ! is_numeric( s ) ) return false;
	n = atoi( s );
	if ( n > 255 ) return false;
	
	return true;
	}

//------------------------------------------------------------------------------				
char * tcp_dns_address( char * dns )
//------------------------------------------------------------------------------				
	{
	#ifdef OS_WINDOWS		

	char *out = null;
	struct hostent *host;
	char **adr;

	if ( ( !dns )||( ! (dns[0]) ) ) return "";
	if ( ! stricmp( dns, "localhost" ) ) return "127.0.0.1";

	tcp_init;

	if ((host = (struct hostent *)gethostbyname( dns )) != NULL)
		{
		for( adr = (char **)(host->h_addr_list) ; *adr ; adr++ )
			{
			out = str( itoa((uchar)((*adr)[0])), ".", itoa((uchar)((*adr)[1])),
					".", itoa((uchar)((*adr)[2])), ".", itoa((uchar)((*adr)[3])), _ );
			if ( !strcmp( out, "127.0.0.1" ) ) continue;
			return out;
			}
		}

	return null;

	#else // OS_UNIX

	static char *s = null;
	char *out = null;
	int li, lj;

	if ( ! stricmp( dns, "localhost" ) ) return "127.0.0.1";
	
	strdcpy( &s, "nslookup ", dns, _ );
	trim( s );
	if ( !( s[8] ) ) return "";
	
	System( &s, s );
	li = stripos( s, "Address", 0 );
	if ( li >= 0 )
		{
		li = stripos( s, "Address", li+8 );
		if ( li >= 0 )
			{
			lj = strpos( s, "\n", li+7 );
			if ( lj >= 0 )
				out = trim(substr( s, li+8, lj - li - 8 ));
			else
				out = trim(substr( s, li+8, -1 ));
			}
		}
	
	return out;

	#endif
	}


//------------------------------------------------------------------------------				
bool is_free_port( char *port )
//------------------------------------------------------------------------------				
	{
	#ifdef OS_WINDOWS
	char **s1 = memptr(0), **s2 = memptr(0);
	#else
	static char *s1 = null, *s2 = null;
	#endif
	int p1, p2, p3, b, i;
	
	if ( ! is_numeric( port ) ) return false;
	
	#ifdef OS_WINDOWS
	System( s1, "netstat -an" );

	strdcpy( s2, " 0.0.0.0:", port, " ", _);
	for( p1 = -1, b = false ; ; )
		{
		p1 = strpos( *s1, *s2, p1+1);
		if ( p1 < 0 )
			{
			if ( !b )	break;
			return false;
			}
		b = true;
		p2 = strpos( *s1, "LISTEN", p1);
		if ( p2 > 0 )
			{
			p3 = strpos( *s1, "\n", p1);
			if ( ( p3 < 0 )||( p3 > p2 ) )
				return -1;
			}
		}

	#else
	#ifdef OS_AIX
		#define prefix_port "*."
	#else
		#define prefix_port ":"
	#endif

	System( &s1,
		strdcpy( &s2, 
			"netstat -tan | awk '{print $4 \" \" $6}' | grep \"", prefix_port, port, " \"", _ ) );

	strdcpy( &s2, prefix_port, port, " ", _);
	for( p1 = -1, b = false ; ; )
		{
		p1 = strpos( s1, s2, p1+1 );
		if ( p1 < 0 )	return ( !b ) ? true : false;
		b = true;
		p2 = strpos( s1, "LISTEN", p1);
		if ( p2 > 0 )
			{ // Vérifie que listen est sur la même ligne
			p3 = strpos( s1, "\n", p1);
			if ( ( p3 < 0 )||( p3 > p2 ) )
				return -1;	// Le port est pris => pas la peine d'attendre qu'il se libère
			}
		}

	#endif

	return ( !b ) ? true : false;
	}


//------------------------------------------------------------------------------				
handle tcp_connect( char * ipport )
//------------------------------------------------------------------------------				
	// errno = 1 : parametre invalide			
	// errno = 2 : adresse ip non valide			
	// errno = 3 : n° port non valide			
	// errno = 4 : pb connexion	
	// error_msg = message d'erreur renseigné		
    {
    int sock, i=0, p, n, nb, port;
    SOCKADDR_IN sin;
    char *ip, **ts;

	tcp_init;

	if ( !ipport )
		{
		strdcpy( &error_msg, "tcp_connect : invalid parameter", _ );
		errno = 1;
		return ERROR;
		}
	
	ts = str_explode( ipport, ";" );
	for( nb = 0 ; ts[nb] ; nb++ );
	if ( stripos( ipport, "random", 0 ) >= 0 )
		i = rnd( 0, nb-1 );

	strdcpy( &error_msg, "tcp_connect :", _ );

	for( n = 0 ; n <= nb ; n++, i++ )
		{
		if ( i >= nb ) { i = -1; continue; }
		if ( ! (ts[i][0]) ) continue;
		if ( ! stricmp( ts[i], "random" ) ) continue;

		p = strpos( ts[i], ":", 0 );
		if ( p < 0 )
			{
			errno = 1;
			strdcat( &error_msg, (char*)((n) ? ";" : "" ),
				(char*)((nb) ? itoa(i) : "" ), " ",
				ts[i], " : is a invalid parameter (format=ip:port)", _ );
			continue;
			}
		
		port = atoi( trim(&(ts[i][p+1])) );
		if ( port <= 0 )
			{
			strdcat( &error_msg, (char*)((n) ? ";" : "" ),
				(char*)((nb) ? itoa(i) : "" ), " ",
				ts[i], " : is a invalid port", _ );
			errno = 2;
			continue;
			}
		

		ts[i][p] = 0;
		if ( ! is_ip_valide( ts[i] ) )
			{
			ip = tcp_dns_address( ts[i] );	
			if ( !ip )
				{
				strdcat( &error_msg, (char*)((n) ? ";" : "" ),
					(char*)((nb) ? itoa(i) : "" ), " ",
					ts[i], " : is a invalid address", _ );
				errno = 3;
				continue;
				}
			}
		else
			ip = ts[i];
		

	    sock = (int)socket(AF_INET, SOCK_STREAM, 0);
	    sin.sin_addr.s_addr			= inet_addr(ip);
	    sin.sin_family				= AF_INET;
	    sin.sin_port				= htons(port);
	
	    if (  connect(sock, (SOCKADDR *)&sin, sizeof(sin)) < 0 )
			{
			errno = 4;
			strdcat( &error_msg, (char*)((n) ? ";" : "" ),
				(char*)((nb) ? itoa(i) : "" ), " ",
				ip, ":", itoa(port), " : connexion error", _ );
			continue;
			}
		

		strdcat( &error_msg, " ",
			ip, ":", itoa(port), " : connexion ok", _ );
			
		free( ts );
		return (handle)sock;
		}
	
	return ERROR;
	}






//------------------------------------------------------------------------------				
void tcp_disconnect( handle hCnx )
//------------------------------------------------------------------------------				
	{
	#ifdef OS_UNIX
	close( hCnx );
	shutdown( hCnx, 2 );
	#else
	closesocket( hCnx );
	#endif
	}


//------------------------------------------------------------------------------				
int tcp_send( handle hCnx, char *in, int len )
//------------------------------------------------------------------------------				
    {
	if ( len == -1 ) len = strlen( in );
	return send(hCnx, in, len, 0);
	}


//------------------------------------------------------------------------------				
int tcp_recv( handle hCnx, char **p_out, int maxlen )
//------------------------------------------------------------------------------				
	{
	char *out;
	
	if ( !p_out ) return ERROR;	
	out = *p_out = (char*)realloc( *p_out, maxlen );

	return recv( hCnx, out, maxlen, 0 );
	}


//------------------------------------------------------------------------------				
int	tcp_nrecv( handle hCnx, char **p_out, int len )
//------------------------------------------------------------------------------				
	{
	int n, t = 0, ne = 0;
	char *out;
	
	if ( ( p_out )&&( *p_out ) ) out = *p_out = (char*)realloc( *p_out, len+1 );
	else { out = alloc( len+1 ); if ( p_out ) *p_out = out; }
	
	while( t < len )
		{
//		n = recv( hCnx, &(out[t]), (((len-t)>4096)?4096:len-t), 0 );
		n = recv( hCnx, &(out[t]), len-t, 0 );
		if ( ( n == ERROR )||( ne > 500 ) )
			return ERROR;
		if ( n ) { t += n; ne = 0; }
		else ne++;
		}
	out[len] = 0;
	return t;
	}
	

//------------------------------------------------------------------------------				
int	tcp_ssend( handle hCnx, char *in, int len )
//------------------------------------------------------------------------------				
	{
	int n;
	char **s = memptr(0);
	
	// CHIFFREMENT DU MESSAGE
	if ( len == -1 ) len = strlen( in );
	else if ( len == -2 ) len = strlen( in )+1;
	n = crypte( s, in , len );

	// ENVOI LA LONGUEUR DU MESSAGE
	if ( send(hCnx, itona(n,10), 10, 0) == ERROR ) return ERROR;

	// ENVOI LE MESSAGE
	return send(hCnx, *s, n, 0);
	}

	
//------------------------------------------------------------------------------				
int	tcp_srecv( handle hCnx, char **p_out )
//------------------------------------------------------------------------------				
	{
	int n, t;
	#ifdef OS_WINDOWS
	char *s1 = null, *s2 = null;
	#else
	static char *s1 = null, *s2 = null;
	#endif

	// RECEPTION DE LA PREMIERE TRAME
	
	if ( tcp_nrecv( hCnx, &s1, 10 ) == ERROR )
		#ifdef OS_UNIX
		return ERROR;
		#else
		goto tcp_srecv_fin_error;
		#endif
	
	if ( !is_numeric( s1 ) )
		return ERROR;
		
	n = atoi( s1 );
	if ( tcp_nrecv( hCnx, &s1, n ) == ERROR )
		#ifdef OS_UNIX
		return ERROR;
		#else
		goto tcp_srecv_fin_error;
		#endif

	n = uncrypte( p_out, s1, n );
	if ( strcmp( *p_out, "ELIB TCP_FSEND START" ) )
		#ifdef OS_UNIX
		return n;
		#else
		goto tcp_srecv_fin_ok;
		#endif
		
	// RECEPTIONS DES TRAMES SUIVANTES
	
	for( n = 0, *p_out[0] = 0 ; ; )
		{
		if ( tcp_nrecv( hCnx, &s1, 10 ) == ERROR )
			#ifdef OS_UNIX
			return ERROR;
			#else
			goto tcp_srecv_fin_error;
			#endif
		t = atoi( s1 );
		if ( tcp_nrecv( hCnx, &s1, t ) == ERROR )
			#ifdef OS_UNIX
			return ERROR;
			#else
			goto tcp_srecv_fin_error;
			#endif
		t = uncrypte( &s2, s1, t );
		if ( ! strcmp( s2, "ELIB TCP_FSEND END" ) ) break;
		*p_out = (char*)realloc( *p_out, n+t+1 );
		memcpy( &((*p_out)[n]), s2, t );
		n += t;
		}
	(*p_out)[n] = 0;

	#ifdef OS_UNIX
	return n;
	#else

	tcp_srecv_fin_ok :
		if ( s1 ) free( s1 );
		if ( s2 ) free( s2 );
		return n;
	
	tcp_srecv_fin_error :
		if ( s1 ) free( s1 );
		if ( s2 ) free( s2 );
		return ERROR;

	#endif
	}
	

//------------------------------------------------------------------------------				
int	tcp_ssend0( handle hCnx, char *in, int len )
//------------------------------------------------------------------------------				
	{
	// ENVOI LA LONGUEUR DU MESSAGE
	if ( len == -1 ) len = strlen( in );
	else if ( len == -2 ) len = strlen( in )+1;
	
	if ( send(hCnx, itona(len,10), 10, 0) == ERROR ) return ERROR;

	// ENVOI LE MESSAGE
	return send(hCnx, in, len, 0);
	}

	
//------------------------------------------------------------------------------				
int	tcp_srecv0( handle hCnx, char **p_out )
//------------------------------------------------------------------------------				
	{
	int n;

	// RECEPTION DE LA PREMIERE TRAME
	
	if ( tcp_nrecv( hCnx, p_out, 10 ) == ERROR )
		return ERROR;
	
	if ( !is_numeric( *p_out ) )
		return ERROR;
		
	n = atoi( *p_out );
	if ( tcp_nrecv( hCnx, p_out, n ) == ERROR )
		return ERROR;

	return n;
	}
	

//------------------------------------------------------------------------------				
int	tcp_srecv9( handle hCnx, char **p_out )
//------------------------------------------------------------------------------				
	{
	int n, t;
	#ifdef OS_WINDOWS
	char *s1 = null, *s2 = null;
	#else
	static char *s1 = null, *s2 = null;
	#endif

	// LECTURE DE LA LONGUEUR DU MESSAGE
	if ( tcp_nrecv( hCnx, &s1, 9 ) == ERROR )
		#ifdef OS_UNIX
		return ERROR;
		#else
		goto tcp_srecv9_fin_error;
		#endif
	n = atoi( s1 );
	
	// LECTURE DU MESSAGE
	if ( tcp_nrecv( hCnx, &s1, n ) == ERROR )
		#ifdef OS_UNIX
		return ERROR;
		#else
		goto tcp_srecv9_fin_error;
		#endif

	// DECHIFFREMENT DU MESSAGE ET RETOUR
	
	n = uncrypte( p_out, s1, n );
	if ( strcmp( *p_out, "ELIB TCP_FSEND START" ) )
		return n;
		
	// RECEPTIONS DES TRAMES SUIVANTES
	
	for( n = 0, *p_out[0] = 0 ; ; )
		{
		if ( tcp_nrecv( hCnx, &s1, 10 ) == ERROR )
			#ifdef OS_UNIX
			return ERROR;
			#else
			goto tcp_srecv9_fin_error;
			#endif
		t = atoi( s1 );
		if ( tcp_nrecv( hCnx, &s1, t ) == ERROR )
			#ifdef OS_UNIX
			return ERROR;
			#else
			goto tcp_srecv9_fin_error;
			#endif
		t = uncrypte( &s2, s1, t );
		if ( ! strcmp( s2, "ELIB TCP_FSEND END" ) ) break;
		*p_out = (char*)realloc( *p_out, n+t+1 );
		memcpy( &((*p_out)[n]), s2, n );
		n += t;
		}
	(*p_out)[n] = 0;

	#ifdef OS_UNIX
	return n;
	#else

	tcp_srecv9_fin_ok :
		if ( s1 ) free( s1 );
		if ( s2 ) free( s2 );
		return n;
	
	tcp_srecv9_fin_error :
		if ( s1 ) free( s1 );
		if ( s2 ) free( s2 );
		return ERROR;

	#endif
	}
	


//------------------------------------------------------------------------------				
int	tcp_fsend( handle hCnx, char *fname )
//------------------------------------------------------------------------------				
	{
	static char *buf = null;
	FILE *fp;
	int n;
	
	if ( ! buf )
		buf = alloc( 64 Ko );
	
	fp = fopen( fname, "rb" );
	if ( fp == NULL )
		{
		error_set( 1, "problème à l'ouverture du fichier : ", fname, _ );
		return ERROR;
		}
	
	if ( tcp_ssend( hCnx, "ELIB TCP_FSEND START", -2 ) == ERROR )
		{
		fclose( fp );
		return ERROR;
		}
	
	while( ( n = fread( fp, buf, 64 Ko ) ) > 0 )
		if ( tcp_ssend(hCnx, buf, n) == ERROR )
			{
			fclose( fp );
			error_set( 2, "problème émission des données", _ );
			return ERROR;
			}
	fclose( fp );

	if ( tcp_ssend( hCnx, "ELIB TCP_FSEND END", -2 ) == ERROR ) return ERROR;

	return OK;
	
	}
	
	
//------------------------------------------------------------------------------				
int	tcp_frecv( handle hCnx, char *fname, int len )
//------------------------------------------------------------------------------				
	{
	static char *s1 = null, *s2 = null;
	int n, t;
	FILE *fp;

	// CREATION DU FICHIER
	
	fp = fopen( fname, "wb" );
	if ( fp == NULL )
		{
		error_set( 1, "problème création du fichier : ", fname, _ );
		return ERROR;
		}
	
	// RECEPTION DE LA PREMIERE TRAME
	
	if ( tcp_nrecv( hCnx, &s1, 10 ) == ERROR ) return ERROR;
	n = atoi( s1 );
	if ( tcp_nrecv( hCnx, &s1, n ) == ERROR ) return ERROR;

	n = uncrypte( &s2, s1, n );
	if ( strcmp( s2, "ELIB TCP_FSEND START" ) )
		{
		if ( ! fwrite( fp, s2, n, _ ) ) { fclose( fp ); return ERROR; }
		fclose( fp );
		return n;
		}
		
	// RECEPTIONS DES TRAMES SUIVANTES
	
	for( t = 0 ; ; )
		{
		if ( tcp_nrecv( hCnx, &s1, 10 ) == ERROR ) return ERROR;
		n = atoi( s1 );
		if ( tcp_nrecv( hCnx, &s1, n ) == ERROR ) return ERROR;
		n = uncrypte( &s2, s1, n );
		if ( ! strcmp( s2, "ELIB TCP_FSEND END" ) ) break;
		if ( ! fwrite( fp, s2, n, _ ) ) { fclose( fp ); return ERROR; }
		t += n;
		}
	fclose( fp );
	return t;
	}

	
//------------------------------------------------------------------------------				
char * 	tcp_request( char *ipport, char *cmd, char **p_out )
//------------------------------------------------------------------------------				
	{
	handle hCnx;
	int n;

	for( n = 0 ; n < 2 ; n++, Sleep(10) )
		{
		if ( ( hCnx = tcp_connect( ipport ) ) == ERROR )
			{
			error_set( 1, "request: problème de connexion : ", ipport, _ );
			continue;
			}
		
		if ( tcp_ssend( hCnx, cmd, -1 ) == ERROR )
			{
			tcp_disconnect( hCnx );
			error_set( 2, "request: problème d'émission de la commande.", _ );
			continue;
			}

		if ( tcp_srecv( hCnx, p_out ) == ERROR )
			{
			tcp_disconnect( hCnx );
			error_set( 2, "request: problème de réception.", _ );
			continue;
			}

		tcp_disconnect( hCnx );
		return *p_out;
		}
	
	return null;
	}


//------------------------------------------------------------------------------				
char * 	tcp_crequest( handle hCnx, char *cmd, char **p_out )
//------------------------------------------------------------------------------				
	{
	if ( tcp_ssend( hCnx, cmd, -1 ) == ERROR )
		{
		tcp_disconnect( hCnx );
		error_set( 2, "request: problème d'émission de la commande.", _ );
		return sERROR;
		}

	if ( tcp_srecv( hCnx, p_out ) == ERROR )
		{
		tcp_disconnect( hCnx );
		error_set( 2, "request: problème de réception.", _ );
		return sERROR;
		}

	return *p_out;
	}


//------------------------------------------------------------------------------				
int	http_recv( handle hCnx, char **p_url, char **p_header, char **p_data,
							long long *p_data_length )
//------------------------------------------------------------------------------				
	{
	int n, ne, p, p1 = -1, c = 0, p2;
	char *s = null, meth;
	
	// p_out doit commencer par 'G' ou 'P'
	if ( ( (*p_header)[0] != 'G' )&&( (*p_header)[0] != 'P' ) )
		return ERROR;

	p = strlen( *p_header );
	
	// LECTURE DU HEADER
	s = alloc0(5000);
	do
		{
		n = tcp_recv( hCnx, &s, 5000 - p );
		if ( n < 0 ) break;
		if ( n == 0 ) { c++; if ( c > 500 ) break; else continue; }
		p += n;
		strdncat( p_header, s, n, _ );
		}
	while( ( p < 5000 )&&( ( p1 = strpos( *p_header, "\r\n\r\n", 0 ) ) < 0 ) );

	free( s );
	if ( ( p >= 5000 )||( p1 < 0 ) )
		return ERROR;
	
	
	// LECTURE DE L'URL
	p1 = strpos( *p_header, " ", 5 );
	strdncpy( p_url, &((*p_header)[4]), p1-4, _ );
	strd_replace( p_url, "%20", " " );
	strd_replace( p_url, "%22", "\"" );
	strd_replace( p_url, "%27", "'" );
	if ( (p2 = strpos( *p_url, "&", 0 )) > 0 )
		{
		p1 = strpos( *p_url, " ", 0 );
		if ( ( p1 < 0 )||( p1 > p ) ) (*p_url)[p2] = ' ';
		}


	// METHODE GET

	if ( ! strncmp( *p_header, "GET ", 4 ) )
		{
		*p_data_length = 0;
		return 1;
		}


	// POST OU PUT : RECUPERATION DES DATA

	p2 = strpos( *p_header, "Content-Length: ", 6 );
	*p_data_length = atoi( &((*p_header)[p2+16]) );

	p2 = strpos( *p_header, "\r\n\r\n", p2+16 ) + 4;


	if ( *p_data )
		*p_data = realloc( *p_data, (*p_data_length)+1 );
	else
		*p_data = alloc0( (*p_data_length)+1 );

	memcpy( *p_data, &((*p_header)[p2]), n = p - p2 );

	for( ne = 0 ; n < *p_data_length ; )
		{
		n += recv( hCnx, &((*p_data)[n]), (*p_data_length)-n, 0 );
		if ( n ) ne = 0;
		else
			{
			ne++;
			if ( ne > 200 )
				return ERROR;
			}
		}


	(*p_data)[ *p_data_length ] = 0;
	
	if (( stripos( *p_header, "utf-8", 0 ) >= 0 )||( stripos( *p_header, "Windows", 0 ) >= 0 ))
		{
		str_from_utf8( p_data );
		*p_data_length = strlen( *p_data );
		}

	return 2;
	}



	


//------------------------------------------------------------------------------				
handle tcp_server_start( char * port )
//------------------------------------------------------------------------------				
	{
	tcp_init;
       
    SOCKET sock;
    SOCKADDR_IN sin;
/*
    #ifdef OS_UNIX
	int optval = 1;
	#endif
*/

    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    sin.sin_addr.s_addr		= INADDR_ANY;
    sin.sin_family			= AF_INET;
    sin.sin_port			= htons(atoi(port));
    if ( bind(sock, (SOCKADDR *)&sin, sizeof(sin)) ) return ERROR;

/*
    #ifdef OS_UNIX
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
	#endif
*/

    if ( listen(sock, 0) ) return ERROR;
    
    return sock;
	}


//------------------------------------------------------------------------------				
handle tcp_cnx_wait( handle hSrv, char *ip )
//------------------------------------------------------------------------------				
    {
    SOCKET csock;
    SOCKADDR_IN csin;
    socklen_t sinsize = sizeof(csin);

    csock = accept(hSrv, (SOCKADDR *)&csin, &sinsize);
    if ( ip != NULL )
	    {
		sprintf( ip, "%d.%d.%d.%d",
			((unsigned char *)&csin.sin_addr)[0],
			((unsigned char *)&csin.sin_addr)[1],
			((unsigned char *)&csin.sin_addr)[2],
			((unsigned char *)&csin.sin_addr)[3]
			);
		}
    return csock;
	}	
		























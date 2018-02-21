#include "ssie7lib.h"


// FONCTIONS DE GESTION DE LA PASSERELLE SYNCHRONE
//------------------------------------------------------------------------------

char *gateway_port_local = null;
char *gateway_dns_dest = null;
char *gateway_port_dest = null;
char *gateway_ip_list = null;

#define hThread1	h1
#define hThread2	h2
#define hCnxDest	h3

extern long int hSrv;


//------------------------------------------------------------------------------
int gateway_in_out( T_WORK *t_work )

// ATTEND UNE TRAME DE L'INITIATEUR ET LA TRANSMET VERS LE DESTINATAIRE
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, dirwrk, DIR_CHAR "gateway_", gateway_port_dest, ".mft", _ );

	n3 = 0;
	for(;;)
		{
		n1 = tcp_recv( hCnx, &s1, 8 Ko );
		if ( n1 == ERROR )
			{
			tcp_disconnect( hCnxDest );
			#ifdef OS_WINDOWS
			ExitThread( 0 ); return;
			#else
			ssie7exit(0);
			#endif
			}			
		if ( !n1 )
			{
			n3++;
			if ( n3 > 1000 )
				{
				tcp_disconnect( hCnx );
				tcp_disconnect( hCnxDest );
				#ifdef OS_WINDOWS
				ExitThread( 0 ); return;
				#else
				ssie7exit(0);
				#endif
				}
			continue;
			}
		
		n3 = 0;
		if ( fp = fopen( fname, "wb" ) )	fclose( fp );
		
		if ( tcp_send( hCnxDest, s1, n1 ) == ERROR )
			{
			tcp_disconnect( hCnx );
			#ifdef OS_WINDOWS
			ExitThread( 0 ); return;
			#else
			ssie7exit(0);
			#endif
			}
		}
	}

//------------------------------------------------------------------------------
int gateway_out_in( T_WORK *t_work )

// ATTEND UNE TRAME DE REPONSE ET LA TRANSMET VERS L'INITIATEUR
//------------------------------------------------------------------------------
	{
	for(;;)
		{
		n2 = tcp_recv( hCnxDest, &s2, 8 Ko );
		if ( n2 == ERROR )
			{
			tcp_disconnect( hCnx );
			#ifdef OS_WINDOWS
			ExitThread( 0 ); return;
			#else
			ssie7exit(0);
			#endif
			}

		if ( !n2 )
			{
			n4++;
			if ( n4 > 1000 )
				{
				tcp_disconnect( hCnx );
				tcp_disconnect( hCnxDest );
				#ifdef OS_WINDOWS
				ExitThread( 0 ); return;
				#else
				ssie7exit(0);
				#endif
				}
			continue;
			}
		
		n4 = 0;
		
		if ( tcp_send( hCnx, s2, n2 ) == ERROR )
			{
			tcp_disconnect( hCnxDest );
			#ifdef OS_WINDOWS
			ExitThread( 0 ); return;
			#else
			ssie7exit(0);
			#endif
			}
		}
	}


int hGSrv;

//------------------------------------------------------------------------------
int gateway_wait_request( T_WORK *t_work )

// ATTEND UNE CONNEXION ET DEMARRE UNE SESSION DE PASSERELLE SYNCHRONE
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, dirwrk, DIR_CHAR "gateway_", gateway_port_dest, ".mft", _ );
	ip_cli = alloc( 20 );
	
	for(;;)
		{
		hCnx = tcp_cnx_wait( hGSrv, ip_cli );

		if ( hCnx == ERROR )
			{
			logw( "GATEWAY : ", gateway_port_local, " : ", ip_cli, " : tcp_cnx_wait ERROR", _ );
			ssie7exit(0);
			}
			

		if ( !strcmp( ip_cli, "127.0.0.1" ) )
			{
			tcp_recv( hCnxDest, &s2, 1 );
			tcp_disconnect( hCnx );
			continue;
			}

		
		logw( "GATEWAY : ", gateway_port_local, " : ", ip_cli, " : Connexion", _ );


		if ( gateway_ip_list[0] )
			{
			strdcpy( &s, ",", ip_cli, ",", _ );
			if ( strpos( gateway_ip_list, s, 0 ) < 0 )
				{
				logw( "GATEWAY : ", gateway_port_local, " : ", ip_cli, " : Connexion refusée", _ );
				tcp_disconnect( hCnx );
				continue;
				}
			}
		

		// CONNEXION AU DESTINATAIRE
		
		strdcpy( &s, tcp_dns_address(gateway_dns_dest), ":", gateway_port_dest, _ );		
		logw( "GATEWAY : ", gateway_port_local, " : ", ip_cli, " : connexion à : ", s, _ );
	    hCnxDest = tcp_connect( s );
		if ( hCnxDest == ERROR )
			{
			logw( "GATEWAY : ", gateway_port_local, " : ", ip_cli, " : ERREUR : impossible de se connecter à ", s, " (", strerror(errno), ").", _ );
			tcp_disconnect( hCnx );
			continue;
			}
			
//        logw( "GATEWAY : ", gateway_port_local, " : ", ip_cli, " : connexion à ", s, " effectuée.", _ );
			

		// LANCEMENT DE 2 PROCESS EN PARALLELE, 1 pour les réceptions et 1 pour les émissions
		hThread1 = thread_start( (int (*)(void *))gateway_in_out, t_work );
		hThread2 = thread_start( (int (*)(void *))gateway_out_in, t_work );

	
		thread_waitend( hThread1, 0 );
		thread_waitend( hThread2, 0 );

		tcp_disconnect( hCnx );
		tcp_disconnect( hCnxDest );
		
		
		logw( "GATEWAY : ", gateway_port_local, " : ", ip_cli, " : Fin connexion", _ );
		}
	
	}



//------------------------------------------------------------------------------
int gateway_run( T_WORK *t_work, char *in_port, char *ip_list, char *dest, char *maxcnx )

// DEMARRAGE D'UNE PASSERELLE SYNCHRONE
//------------------------------------------------------------------------------
	{
	T_WORK *t_wrk;
	
	//--------------------------------------------------------------------------
	// VERIFIE QUE LE PORT EST LIBRE
	
	for( n = 0 ; ( ! is_free_port( in_port ) )||( n > 60 ) ; n++, sleep(1) )
		{
		if ( ! n )
			{
			print( str( currdate("log"), " : ",
				product_name, " : Le port ", in_port, " n'est pas libre." BR
					"On attend qu'il se libère",
				_ ), -1 );
			}
		}


	if ( is_free_port( in_port ) <= 0 )
		{
		logw( "GATEWAY : ", in_port, " : ERREUR : Le port n'est pas libre", _ );
		ssie7exit(1);
		}

	
	//--------------------------------------------------------------------------
	// INITIALISE LE SERVEUR TCP
	
	strdcpy( &gateway_port_local, in_port, _ );
	
	hGSrv = tcp_server_start( in_port );
	if ( ! hGSrv )
		{
		logw( "GATEWAY : ", in_port,
			" : ERREUR : Problème démarrage serveur tcp", _ );
		ssie7exit(1);
		}

	if ( ( ip_list )&&( trim(ip_list)[0] ) )
		{
		strdcpy( &gateway_ip_list, ",", ip_list, ",", _ );
		strd_replace( &gateway_ip_list, " ", "" );
		}
	else
		gateway_ip_list = "";
	
	strdcpy( &fname, dirwrk, DIR_CHAR "gateway_", in_port, ".mft", _ );
	if ( ! file_exists( fname ) ) file_save_str( fname, "", -1 );

	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_gateway_", in_port, ".pid", _ );
	file_save_str( fname, itoa(getpid()), -1 );
	
	
	//--------------------------------------------------------------------------
	// DEMARRE LES PROCESS D'ATTENTES ET DE TRAITEMENTS DES REQUETES
	
	fclose( stderr );
	fflush( stdout );
	
//	trace_close();
	
	if ( ( p = strpos( dest, ":", 0 ) ) < 0 )
		{
		gateway_dns_dest = dest;
		gateway_port_dest = "1761";
		}
	else
		{
		gateway_dns_dest = null;
		strdcpy( &gateway_dns_dest, substr( dest, 0, p ), _ );
		gateway_port_dest = null;
		strdcpy( &gateway_port_dest, substr( dest, p+1, -1 ), _ );
		}
	
	if ( maxcnx ) n = atoi( maxcnx );
	else n = 3;
	
	logw( "GATEWAY : ", in_port, " : maxcnx=", itoa(n), ", dest=", dest, ", ip_list = '", gateway_ip_list, "' : démarrage : OK", _ );
	
	for( ; n > 1 ; n-- )
		{
		t_wrk = (T_WORK*)alloc0( sizeof(T_WORK) );
		thread_start( (int (*)(void *))gateway_wait_request, t_wrk );
		}
	
	gateway_wait_request( t_work );
	}



#undef hThread1
#undef hThread2
#undef hCnxDest

















//#include "ssie7lib.h"

extern FILE * fpd;
#ifdef OS_AIX
//char bMemoryAlert = false;
#endif

//------------------------------------------------------------------------------
int ssie7run( )

// FONCTION APPELE PAR LE MAIN POUR GERER TOUS LES MODES (CONSOLE, LIGNE DE COMMANDE, SERVEUR, ...)
//------------------------------------------------------------------------------
	{
	T_WORK_ALLOC;	// Strucutre de travail

	strdcpy( &user, sysuser(), _ );

	ssie7_init( t_work );
	

	// DISPATCH SELON LA DEMANDE
	
	//----------------------------------------------------------------------
	if ( argc < 2 )	// PAS DE PARAMETRE => CONSOLE
	//----------------------------------------------------------------------
		{
		request_mode = MODE_CONSOLE;
		console_run( t_work );
		}
	else

	//----------------------------------------------------------------------
	if ( !strcmp( argv[1], "robot" ) ) // PROCESS ROBOT
	//----------------------------------------------------------------------
		{
		request_mode = MODE_ROBOT;
		robot_run( t_work );
		}
	else


	//----------------------------------------------------------------------
	if ( ( !strcmp( argv[1], "server" ) )	// PROCESS SERVEUR TCP
		&&( ( argc >= 2 )&&( is_numeric( argv[2] ) ) ) )
	//----------------------------------------------------------------------
		{
		request_mode = MODE_TCP;
		server_run( t_work );
		}
	else		

	//----------------------------------------------------------------------
		{
		// EXECUTION D'UNE REQUETE PASSE EN LIGNE DE COMMANDE
		
		if ( argv[1][0] == '@' )
			{
			if ( cmd ) cmd[0] = 0;
			file_load( &cmd, &(argv[1][1]) );
			}
		else
			{
			strdcpy( &cmd, argv[1], _ );
			for( i = 2 ; i < argc ; i++ )
				{
				if ( (p=strpos( argv[i], " ", 0 )) < 0 )
					{
					strdcat( &cmd, " ", argv[i], _ );
					}
				else
					{
					if (( (p1=strpos( argv[i], "=", 0 )) > 0 )&&( p1 < p ))
						strdcat( &cmd, " ", substr( argv[i], 0, p1+1 ), str_quote( substr( argv[i], p1+1, -1 ) ), _ );
					else
						strdcat( &cmd, " ", str_quote( argv[i] ), _ );
					}
				}
			}
		
		strdcpy( &(ip_cli), "local", _ );
		r = 0;
		request_mode = MODE_CMDE;

		if ( hReq ) var_free( hReq );
		hReq = var_from_cmd( 0, cmd );

		if ( !hReq )
			{
			print( "ERREUR de syntaxe" BR, cmd, _ );
			return;
			}
		v = var_getn( hReq, 0, &what );

		ssie7_cmd( t_work );
//		print( BR, _ );

		ssie7free( t_work );
		if ( argv[1][0] == '@' )
			file_delete( &(argv[1][1]) );
		}
	
	return r;	
	}



//------------------------------------------------------------------------------
int console_run( T_WORK * t_work )

// EXECUTION DANS LE MODE CONSOLE
//------------------------------------------------------------------------------
	{
	//----------------------------------------------------------------------
	print( product_name, " - Console d'exploitation." BR BR, _ );
	//----------------------------------------------------------------------

	server_conf_load();

	if ( !cmd ) cmd = alloc( 4096 );

	ip_cli = null;

	for( ; ; )
		{
		print( BR BR, var_get( hSrvConf, "partname" ), " > ", _ );
		cmd = realloc( cmd, 4096 );

		fgets( cmd, 4096, stdin );

		if ( trim( cmd )[0] == 0 ) continue;
		
		//------------------------------------------------------------------
		if ( strmicmp( cmd, "q|quit|exit" ) )
		//------------------------------------------------------------------
			return;

		//------------------------------------------------------------------
		
		//if ( hReq ) var_free( hReq );  provoque des crash dans le cas console
		hReq = var_from_cmd( 0, cmd );
		if ( !hReq )
			{
			print( "ERREUR de syntaxe" BR, cmd, _ );
			return;
			}
		v = var_getn( hReq, 0, &what );
		
		ssie7_cmd( t_work );
		ssie7free( t_work );
//		trace_close();
		}
	}


//------------------------------------------------------------------------------




long int hSrv;

#ifdef HTTP_AUTHORIZED

//------------------------------------------------------------------------------
int server_wait_request( T_WORK * t_work )

// ATTENTE D'UNE REQUETE TCP
//------------------------------------------------------------------------------
	{
	char *fcnx = null;
	int dt = sysdate;
	
	ip_cli = alloc( 20 );
	hCnx = ERROR;

	for( ; ; )
		{
//		if ( ( hCnx != ERROR )&&( hCnx ) )	tcp_disconnect( hCnx );

		if ((sysdate - dt) > (3 Heures)) ssie7exit(0); // Redémarrage toutes les 3 heures
		
		
		// ATTEND UNE REQUETE

		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
		if ( file_exists( fname ) )
			{
			tcp_disconnect( hSrv );
			ssie7exit(0);
			}

		#ifdef TR
		unlink( tr_fname );
		#endif
		
		while ( ( hCnx = tcp_cnx_wait( hSrv, ip_cli ) ) <= 0 )
			Sleep(10); // LAISSE LE TEMPS AU SYSTEME DE RESPIRER

		if ( file_exists( fname ) )
			{
			tcp_disconnect( hCnx );
			tcp_disconnect( hSrv );
			ssie7exit(0);
			}
		
		
		is_admin = false;
		if ( profile ) profile[0] = 0;
		
		
		// LECTURE DU PROTOCOLE
		
		if ( tcp_nrecv( hCnx, &s, 1 ) == ERROR )
			continue;
			

		if ( s[0] == '0' )
			{
			// LECTURE DE LA REQUETE
	
			if ( ( n = tcp_srecv9( hCnx, &cmd ) ) == ERROR )
				continue;
	
			cmd[n] = 0;
			if ( (trim(cmd))[0] == 0 ) continue;

			
			hReq = var_from_cmd( 0, cmd );
			if ( !hReq )
				{
				tcp_ssend( hCnx, "ERREUR de syntaxe", -1 );
				continue;
				}
	
			var_getn( hReq, 0, &what );
			if ( ! stricmp( what, "ping" ) ) { tcp_ssend( hCnx, "OK", -1 ); goto suite; }
			
			if ( v = var_get( hReq, "user" ) )
				{
				strdcpy( &user, v, _ );
				var_delete( hReq, "user" );
				}
			else
			if ( user )
				user[0] = 0;
			else
				strdcpy( &user, "", _ );

			protocol = 0;
			
			
			// TRAITEMENT DE LA REQUETE
			
			if ( ssie7out ) ssie7out[0] = 0;
			else	
				strdcpy( &ssie7out, "", _ );
			
			ssie7_cmd( t_work );
			
			
			// ENVOI LA REPONSE
			
	//		if (( ssie7out )&&( ssie7out[0] ))
			if ( ssie7out )
				tcp_ssend( hCnx, ssie7out, -1 );
			}
		else
			{
			if ( (p = http_recv( hCnx, &s1 /*url*/, &s /*header*/, &cmd, &ll )) == ERROR )
				continue;

			if ( p == 1 ) // GET
				{
				if ( strpos( s1, "..", 0 ) >= 0 ) goto error_get;
				strdcpy( &fname, dirwww, s1, _ );
				
				if ( dir_exists( fname ) )	strdcat( &fname, s1, ".html", _ );
				if ( ! file_exists( fname ) )
					{
					error_get:
//logw( "HTTP : Fichier inconnu : ", fname, _ );
					strdcpy( &s1, "HTTP/1.0 200 OK\nContent-Type : text/HTML; charset=iso-8859-1\n\n",
								"Fichier inconnu : ", fname, "<br>", _ );
					tcp_send( hCnx, s1, -1 );
					tcp_disconnect( hCnx );
					continue;
					}
					
				if ( file_load( &s, fname ) == sERROR )
					{
					strdcpy( &s1, "HTTP/1.0 200 OK\nContent-Type : text/HTML; charset=iso-8859-1\n\n",
								"Pb lecture fichier : ", fname, "<br>", _ );
					tcp_send( hCnx, s1, -1 );
					tcp_disconnect( hCnx );
					continue;
					}
					
				n = file_size( fname );
				v = file_getext( fname );
				if ( ( !stricmp( v, "bmp" ) )||( !stricmp( v, "gif" ) )||( !stricmp( v, "ico" ) )
					||( !stricmp( v, "jpg" ) )||( !stricmp( v, "jpeg" ) )||( !stricmp( v, "png" ) )
					||( !stricmp( v, "mpg" ) )||( !stricmp( v, "mpeg" ) ) )
					{
					strdcpy( &s1, "HTTP/1.0 200 OK\nContent-Type : image/x-xbitmap\nContent-Length : ",
									itoa( n ), "\n\n", _ );
					tcp_send( hCnx, s1, -1 );
					}
				else
					{
					tcp_send( hCnx, "HTTP/1.0 200 OK\nContent-Type : text/HTML; charset=iso-8859-1\n\n", -1 );
//					n = strlen(str_to_utf8( &s ));
					}
	
				tcp_send( hCnx, s, n );
				tcp_disconnect( hCnx );
				continue;
				}
			
			// POST
			tcp_send( hCnx, "HTTP/1.0 200 OK\nContent-Type : text/HTML; charset=utf-8\n\n", -1 );

			hReq = var_from_cmd( 0, cmd );
			if ( !hReq )
				{
				tcp_send( hCnx, "ERREUR de syntaxe", -1 );
				tcp_disconnect( hCnx );
				continue;
				}
	
			var_getn( hReq, 0, &what );
			if ( v = var_get( hReq, "user" ) )
				{
				strdcpy( &user, v, _ );
				var_delete( hReq, "user" );
				}
			else
			if ( user )
				user[0] = 0;
			else
				strdcpy( &user, "", _ );
			
			protocol = 1;
			
			// TRAITEMENT DE LA REQUETE
			
			if ( ssie7out ) ssie7out[0] = 0;
			else	
				strdcpy( &ssie7out, "", _ );
			
			ssie7_cmd( t_work );
			
			
			// ENVOI LA REPONSE
			
			if ( ssie7out )
				tcp_send( hCnx, str_to_utf8( &ssie7out ), -1 );
			
			}
		
		suite:
				
		tcp_disconnect( hCnx );
		ssie7free( t_work );
	
		#ifdef OS_AIX
//		if ( !bMemoryAlert )
			{
			strdcpy( &cmd, "svmon -P ", itoa( getpid() ), " -O summary=basic,unit=MB,sortentity=inuse | grep ", itoa( getpid() ), " | grep -v grep | sed -e \"s/^ //\" | tr -s \" \" \":\" | cut -d \":\" -f6 | cut -d \".\" -f1", _ );
			System( &s, cmd );
			if ( atoi( s ) > 512 )
				{
//				bMemoryAlert = true;
				logw( module_name, " : ATTENTION dépacement de la mémoire max autorisée : " BR, cmd, BR, s, _ );
				ssie7exit( OK );
				}
			}
		#endif
		}
	}

#else

//------------------------------------------------------------------------------
int server_wait_request( T_WORK * t_work )

// ATTENTE D'UNE REQUETE TCP
//------------------------------------------------------------------------------
	{
	char *fcnx = null;
	
	ip_cli = alloc( 20 );
	hCnx = ERROR;

	for( ; ; )
		{
//		if ( ( hCnx != ERROR )&&( hCnx ) )	tcp_disconnect( hCnx );
		
		// ATTEND UNE REQUETE

		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
		if ( file_exists( fname ) )
			{
			tcp_disconnect( hSrv );
			ssie7exit(0);
			}

		while ( ( hCnx = tcp_cnx_wait( hSrv, ip_cli ) ) <= 0 )
			Sleep(10); // LAISSE LE TEMPS AU SYSTEME DE RESPIRER

		if ( file_exists( fname ) )
			{
			tcp_disconnect( hCnx );
			tcp_disconnect( hSrv );
			ssie7exit(0);
			}
		
		
		is_admin = false;
		if ( profile ) profile[0] = 0;
		
		
		// LECTURE DU PROTOCOLE
		
		if ( tcp_nrecv( hCnx, &s, 1 ) == ERROR )
			continue;
			
		if ( s[0] != '0' ) continue;


		// LECTURE DE LA REQUETE

		if ( ( n = tcp_srecv9( hCnx, &cmd ) ) == ERROR )
			continue;

		cmd[n] = 0;
		if ( (trim(cmd))[0] == 0 ) continue;
		

		hReq = var_from_cmd( 0, cmd );
		if ( !hReq )
			{
//			trace( "ERREUR de syntaxe", _ );
			tcp_ssend( hCnx, "ERREUR de syntaxe", -1 );
			continue;
			}

		var_getn( hReq, 0, &what );
		if ( ! stricmp( what, "ping" ) ) { tcp_ssend( hCnx, "OK", -1 ); goto suite; }
		
		if ( v = var_get( hReq, "user" ) )
			{
			strdcpy( &user, v, _ );
			var_delete( hReq, "user" );
			}
		else
		if ( user )
			user[0] = 0;
		else
			strdcpy( &user, "", _ );
		
		
		// TRAITEMENT DE LA REQUETE
		
		if ( ssie7out ) ssie7out[0] = 0;
		else	
			strdcpy( &ssie7out, "", _ );
		
		ssie7_cmd( t_work );
		
		
		// ENVOI LA REPONSE
		
//		if (( ssie7out )&&( ssie7out[0] ))
		if ( ssie7out )
			tcp_ssend( hCnx, ssie7out, -1 );
	

		suite:
			
		
		ssie7free( t_work );
	

		#ifdef OS_AIX
//		if ( !bMemoryAlert  )
			{
			strdcpy( &cmd, "svmon -P ", itoa( getpid() ), " -O summary=basic,unit=MB,sortentity=inuse | grep ", itoa( getpid() ), " | grep -v grep | sed -e \"s/^ //\" | tr -s \" \" \":\" | cut -d \":\" -f6 | cut -d \".\" -f1", _ );
			System( &s, cmd );
			if ( atoi( s ) > 512 )
				{
				logw( module_name, " : ATTENTION dépacement de la mémoire max autorisée : " BR, cmd, BR, s, _ );
//				bMemoryAlert = true;
				ssie7exit( OK );
				}
			}
		#endif
		}
	}

#endif

#ifdef OS_UNIX

//------------------------------------------------------------------------------
int server_survey( T_WORK * t_work )

// SURVEILLANCE DU SERVEUR HTTP
//------------------------------------------------------------------------------
	{
	//--------------------------------------------------------------------------
	// SURVEILLANCE DES PROCESS

	strdcpy( &fname1, dirwrk, DIR_CHAR, module_name, "_server_", srv_port, ".pid", _ );
	file_save_str( fname1, itoa( getpid() ), -1 );

	strdcpy( &cmd, module_name, " server ", argv[2], _ );
	for( n2 = 0, Sleep(50) ; ; sleep(1), n2++ )
		{
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
		if ( file_exists( fname ) )
			{
			sleep(3);
			tcp_disconnect( hSrv );
/*{
logw( "server_survey : fichier stop présent", _ );
ssie7exit(0);
}*/			
			ssie7exit(0);
			}
			
		if ( (n+1 - process_count( cmd )) > 0 )
			{
			// Redémarre un process d'attente de requête
			pid = fork();
			if (pid == 0) /* Processus fils      */
		    	{
				server_wait_request( t_work );
				ssie7exit(0);
				}


			// Redémarre un autre process de surveillance (toujours à cause des zombies)
			pid = fork();
			if (pid == 0) /* Processus fils      */
		    	{
				server_survey( t_work );
				ssie7exit(0);
				}
			ssie7exit(0);
			}
		
		//----------------------------------------------------------------------
		ssie7free( t_work );
//		trace_close();
		}
	
	}
	
#endif

//------------------------------------------------------------------------------
int server_run( T_WORK * t_work )

// EXECUTION DU SERVEUR HTTP
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	T_WORK *t_wrk;
	#endif

	#ifdef OS_UNIX
	strdcpy( &fname, dirtmp, DIR_CHAR, module_name, "_server.out", _ );
	fpd = fopen( fname, "ab" );
	if ( fpd ) fprint( fpd, "server_run", BR, __ );
	#endif
	
	//--------------------------------------------------------------------------
	// INITIALISATION
	
	strdcpy( &fname1, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
	file_delete( fname1 );
	
	server_conf_load();
	
	srv_port = argv[2];
	
	strdcpy( &fname1, dirwrk, DIR_CHAR, module_name, "_server_", srv_port, ".pid", _ );


	//--------------------------------------------------------------------------
	// INITIALISE LE FICHIER PID
	
	file_save_str( fname1, itoa( getpid() ), -1 );
		
	
	//--------------------------------------------------------------------------
	// INITIALISE LE FICHIER STATUS
	
	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server_", srv_port, ".status", _ );
	file_save_str( fname, str( currdate("log"),
		" : ", product_name, " : En cour de démarrage",
		_ ), -1 );



	
	
	//--------------------------------------------------------------------------
	// VERIFIE QUE LE PORT EST LIBRE
	
	for( n = 0 ; ( ! is_free_port( srv_port ) )&&( n < 5*60 ) ; n++, sleep(1) )
		{
		if ( ! n )
			{
			file_save_str( fname, str( currdate("log"), " : ",
				product_name, " : Le port ", srv_port, " n'est pas libre." BR
					"On attend qu'il se libère",
				_ ), -1 );
			}
		}
		

		

	if ( is_free_port( srv_port ) <= 0 )
		{
		logw( "SERVEUR : démarrage sur le port ", srv_port,
			" : ERREUR : Le port n'est pas libre", _ );
		file_save_str( fname, str( currdate("log"), " : ",
			product_name, " : ERREUR : Le port ", srv_port, " n'est pas libre",
			_ ), -1 );

		ssie7exit(1);
		}



		
	
	//--------------------------------------------------------------------------
	// INITIALISE LE SERVEUR TCP
	
	hSrv = tcp_server_start( srv_port );
	if ( ! hSrv )
		{
		logw( "SERVEUR : démarrage sur le port ", srv_port,
			" : ERREUR : Problème démarrage serveur tcp", _ );
		file_save_str( fname, str( currdate("log"), " : ",
			 product_name, " : ERREUR : Problème démarrage serveur tcp", _ ), -1 );

		ssie7exit(1);
		}

	logw( "SERVEUR : démarrage sur le port ", srv_port, " : OK", _ );
	file_save_str( fname, str( currdate("log"), " : ",
		product_name, " : est démarré sur le port ", srv_port,
		_ ), -1 );
	
	file_save_str( fname1, itoa(getpid()), -1 ); // Enregistre le pid
	

	
	//--------------------------------------------------------------------------
	// DEMARRE LES PROCESS D'ATTENTES ET DE TRAITEMENTS DES REQUETES
	
	fclose( stderr );
	
	v = var_get( hSrvConf, "maxcnx" );
	if ( ( !v )||( (n = atoi( v )) == 0 ) ) n = 10;

	strdcpy( &dirwww, var_get( hSrvConf, "dirwww" ), _ );
	
	#ifdef OS_WINDOWS
	
	for( ; n > 1 ; n-- )
		{
		t_wrk = (T_WORK*)alloc0( sizeof(T_WORK) );
		thread_start( (int (*)(void *))server_wait_request, t_wrk );
		}
	
	server_wait_request( t_work );
	
	#else	// OS_UNIX
	
	fflush( stdout );
	
	for( i = 0 ; i < n ; i++ )
		{
		pid = fork();
		if (pid == 0) /* Processus fils      */
	    	{
			server_wait_request( t_work );
			ssie7exit(0);
			}
		}


	// Démarre la surveillance des processus
	
	pid = fork();
	if (pid == 0) /* Processus fils      */
    	{
		server_survey( t_work );
		ssie7exit(0);
		}

	ssie7exit(0);

	#endif
	}


























#define MAIN

#include "elib4.c"
#define MFT
#include "ssie7lib.c"

//------------------------------------------------------------------------------
// PARAMETRAGE DU MODULE SSIE7NAT

#define PRODUCT_NAME		"SSIE Serveur v7.2.a"


#include "ssie7lib_cmd_connect.c"
#include "ssie7lib_vdisk.c"
#include "ssie7lib_vdisk_server.c"

//#include "ssie7c_cmd_gw_inst.c"
//#include "ssie7c_cmd_initialize.c"



// main , ssie7_init , server_run , robot_run

long int hSrv;
int server_survey( T_WORK * t_work );
int server_wait_request( T_WORK * t_work );


//------------------------------------------------------------------------------
int main(int ac, char *av[])
//------------------------------------------------------------------------------
	{
	T_WORK_ALLOC;
	#ifdef OS_WINDOWS
	T_WORK *t_wrk;
	#endif

	if ( ac < 2 ) exit(1);
	
	argc = ac;
	argv = av;

	strdcpy( &module_name, getenv("SSIE_MODULE_NAME"), _ );
	strdcpy( &dirbin	, getenv("SSIE_DIRBIN"), _ );
	strdcpy( &dirconf	, getenv("SSIE_DIRCONF"), _ );
	strdcpy( &dirwrk	, getenv("SSIE_DIRWRK"), _ );
	strdcpy( &dirlog	, getenv("SSIE_DIRLOG"), _ );
	strdcpy( &server_conf_fname	, getenv("SSIE_CONF_FNAME"), _ );
	
	hSrvConf = var_load( server_conf_fname );
	var_set( hSrvConf, "srvnat_list", "none" );
	var_set( hSrvConf, "part_auto_crea", "Non" );

	request_mode = MODE_TCP;

	#ifdef OS_UNIX
	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.out", _ );
	fpd = fopen( fname, "ab" );
	if ( fpd ) fprint( fpd, "server_run", BR, __ );
	#endif

	// RECHERCHE D'OPENSSL (OBLIGATOIRE)
	
	#ifdef OS_WINDOWS
	// POUR WINDOWS C'EST $DIRBIN/SSIE7SEC.EXE
	strdcpy( &fname, dirbin, DIR_CHAR "ssie7sec.exe", _ );
	if ( ssl_pgm( fname ) == null )
	#else
	// POUR UNIX, LE PROGRAMME EST RECHERCHE DANS LE PATH
	if ( ssl_pgm( null ) == null )
	#endif
		{
		print( "ERREUR : openssl n'est pas installé ou est inaccessible.(",fname,")" BR, _ );
		wexit(1);
		}
		
	
	
	//--------------------------------------------------------------------------
	// INITIALISATION
	
	strdcpy( &fname1, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
	file_delete( fname1 );
	
//	server_conf_load();
	srv_port = ( argc == 3 ) ? argv[2] : argv[1];
	
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
		logw( module_name, " : démarrage sur le port ", srv_port,
			" : ERREUR : Le port n'est pas libre", _ );
		file_save_str( fname, str( currdate("log"), " : ",
			product_name, " : ERREUR : Le port ", srv_port, " n'est pas libre",
			_ ), -1 );

		exit(1);
		}



		
	
	//--------------------------------------------------------------------------
	// INITIALISE LE SERVEUR TCP
	
	hSrv = tcp_server_start( srv_port );
	if ( ! hSrv )
		{
		logw( module_name, " : démarrage sur le port ", srv_port,
			" : ERREUR : Problème démarrage serveur tcp", _ );
		file_save_str( fname, str( currdate("log"), " : ",
			 product_name, " : ERREUR : Problème démarrage serveur tcp", _ ), -1 );

		exit(1);
		}

	logw( module_name, " : démarrage sur le port ", srv_port, " : OK", _ );
	file_save_str( fname, str( currdate("log"), " : ",
		"[product_name] : est démarré sur le port ", srv_port,
		_ ), -1 );
	
//	print( product_name, " démarré sur le port ", srv_port, BR, __ );
	file_save_str( fname1, itoa(getpid()), -1 ); // Enregistre le pid
	

	
	//--------------------------------------------------------------------------
	// DEMARRE LES PROCESS D'ATTENTES ET DE TRAITEMENTS DES REQUETES
	
	fclose( stderr );
	
	v = getenv("SSIE_SERVER_MAXCNX");
	if ( ( !v )||( (n = atoi( v )) == 0 ) ) n = 10;
	
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
			exit(0);
			}
		}


	// Démarre la surveillance des processus
	
	pid = fork();
	if (pid == 0) /* Processus fils      */
    	{
		server_survey( t_work );
		exit(0);
		}

	exit(0);

	#endif

	}
	


	
//------------------------------------------------------------------------------
int ssie7_cmd( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	if (( stricmp( what, "log" ) )&&( stricmp( what, "connect" ) ))
		{
		if ( request_mode == MODE_TCP )
			{
			strdcpy( &s, cmd, _ );
			if ( session_cnx <= 0 )
				logw( ip_cli, " > ", strd_replace( &s, NNI_ADM, "SPENCER" ), _ );
			else
				logw( ip_cli, " (", session_partname, ") > ", strd_replace( &s, NNI_ADM, "SPENCER" ), _ );
			}
		#ifdef OS_UNIX
		else
		if ( strcmp( user, "ssie" ) )
			logw( user, " : ", cmd, _ );
		#endif
		else
		if ( request_mode == MODE_CONSOLE )		logw( "CONSOLE : ", cmd, _ );
		}
	
	if ( !stricmp( what, "connect" ) )		return cmd_connect( t_work );

	if ( !stricmp( what, "vdisk_server" ) )	return cmd_vdisk_server( t_work );

	strdcpy( &fname, dirbin, DIR_CHAR, what, EXE, _ );
	if ( !file_exists( fname ) )
		{
		strdcpy( &fname, dirbin, DIR_CHAR, module_name, "_", what, EXE, _ );
		if ( !file_exists( fname ) )
			strdcpy( &fname, dirbin, DIR_CHAR, module_name, EXE, _ );
		}
		
	if ( file_exists( fname ) )
		{
		file_tmpname(&fname1);
		file_save_str( fname1, cmd, -1 );
		strdcat( &fname, " @", fname1, _ );
		System( &s, fname );
		print( s, _ );
		unlink( fname1 );
		return OK;
		}

	}


//------------------------------------------------------------------------------
void ssie7_init( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	}

//------------------------------------------------------------------------------
int robot_run( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	}


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
			exit(0);
			}
			
		if ( (n+1 - process_count( cmd )) > 0 )
			{
			// Redémarre un process d'attente de requête
			pid = fork();
			if (pid == 0) /* Processus fils      */
		    	{
				server_wait_request( t_work );
				exit(0);
				}


			// Redémarre un autre process de surveillance (toujours à cause des zombies)
			pid = fork();
			if (pid == 0) /* Processus fils      */
		    	{
				server_survey( t_work );
				exit(0);
				}
			exit(0);
			}
		
		//----------------------------------------------------------------------
		ssie7free( t_work );
		trace_close();
		}
	
	}
	
#endif




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
		if ( ( hCnx != ERROR )&&( hCnx ) )	tcp_disconnect( hCnx );
		
		// ATTEND UNE REQUETE

		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
		if ( file_exists( fname ) )
			{
			tcp_disconnect( hSrv );
			exit(0);
			}

		while ( ( hCnx = tcp_cnx_wait( hSrv, ip_cli ) ) <= 0 )
			Sleep(10); // LAISSE LE TEMPS AU SYSTEME DE RESPIRER

		if ( file_exists( fname ) )
			{
			tcp_disconnect( hCnx );
			tcp_disconnect( hSrv );
			exit(0);
			}
		
		// VERIFIE LE NOMBRE DE CNX POUR UNE MEME IP
		
		
		
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
			trace( "ERREUR de syntaxe", _ );
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
			
		#ifdef OS_WINDOWS
//		unlink( fcnx );
		#endif
		
		ssie7free( t_work );
	
//		trace_close();
		}
	}




	
	















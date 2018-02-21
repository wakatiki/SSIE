//------------------------------------------------------------------------------
//
// SSIE7.C
//
//------------------------------------------------------------------------------

// OPTIONS DE COMPILATIONS

#define SSIE_VERSION		"SSIE v7.2.a.7 - 2018/01/18"

#ifdef OS_WINDOWS
//#define MODE_CONTROLEUR	// Force le mode ihm locale et tests
#endif

//#define IHM_LOCALE	// Mode windows avec IHM locale
//#define TR			// Mode trace pour les tests
//#define TR_DEL		// Mode trace avec suppression auto du fichier trace à l'exit (pour détecter les plantages)
//#define FORCE_SSIE7	// force module_name = ssie7 même si le binaire s'appel autrement (pour les tests)
//#define TESTS			// Commande "test" pour le robot de test

//------------------------------------------------------------------------------

#ifndef MODE_CONTROLEUR

	#define PRODUCT_VERSION		SSIE_VERSION

#else

	#define PRODUCT_VERSION		SSIE_VERSION " - Contrôleur"

	#ifdef OS_WINDOWS
	#define IHM_LOCALE	// Mode windows avec IHM locale
	#endif
	#define TESTS		// Avec commandes tests

#endif

#ifdef IHM_LOCALE
#undef MODE_CONSOLE
#endif

//------------------------------------------------------------------------------


#ifdef OS_WINDOWS
#define TR_DIR "c:/e/ssie7traces"
//#define TR_DIR "d:\\r2000\\mft\\ssie\\traces"
#else
#define TR_DIR LOGI_DIR "/ssie7/traces"
#endif

#ifdef IHM_LOCALE
	#define PRODUCT_OPTION1		"IHM Locale"
#else
	#define PRODUCT_OPTION1		""
#endif

#ifdef TESTS
	#define PRODUCT_OPTION2		"Tests"
#else
	#define PRODUCT_OPTION2		""
#endif

#ifdef TR
	#define PRODUCT_OPTION3		"Traces"
#else
	#define PRODUCT_OPTION3		""
#endif

//#define HTTP_AUTHORIZED // faille web
#define MAIN

#include "ssie7.h"


//------------------------------------------------------------------------------
int main(int ac, char *av[])
//------------------------------------------------------------------------------
	{
	argc = ac;	argv = av;

	tr_init(TR_DIR);

	error_no = ssie7run();
	exit( error_no ); // exit() est une macro
	}



//------------------------------------------------------------------------------
void ssie7_init( T_WORK* t_work )
//------------------------------------------------------------------------------
	{

	#ifdef OS_UNIX


	// IGNORE LES SIGNAUX KILL (Pb stty erase sous AIX)
	//putenv( "SSIE_IGNORE_SIGNAL=Oui" );
	if ( getenv( "SSIE_IGNORE_SIGNAL" ) )
		for( i = 1 ; i < NSIG ; i++ )
			signal( i, SIG_IGN );

	#endif

//	tr( "ssie7_init_1" );

	// INIT MODULE_NAME

	strdcpy( &module_name, file_getname( argv[0] ), _ );
	if ( ( p = stripos( module_name, ".exe", 0 ) ) >= 0 )
		module_name[p] = 0;

#ifdef FORCE_SSIE7
strdcpy( &module_name, "ssie7", _ );
#endif


	// INIT PRODUCT_NAME

	strdcpy( &s1, module_name, "_PRODUCTNAME", _ );
	v = getenv( upper(s1) );
	if (( !v )||( v[0] == 0 ))
		{
		strdcpy( &product_name, PRODUCT_VERSION, _ );

		if (( (PRODUCT_OPTION1)[0] )||( (PRODUCT_OPTION2)[0] )||( (PRODUCT_OPTION3)[0] ))
			{
			b = false;
			strdcat( &product_name, " (avec " PRODUCT_OPTION1, _ );
			if ( (PRODUCT_OPTION1)[0] ) b = true;
			if ( (PRODUCT_OPTION2)[0] )
				{
				strdcat( &product_name, (char*)((b)?", ":""), PRODUCT_OPTION2, _ );
				b = true;
				}
			if ( (PRODUCT_OPTION3)[0] )
				{
				strdcat( &product_name, (char*)((b)?", ":""), PRODUCT_OPTION3, _ );
				b = true;
				}
			if ( b )
				strdcat( &product_name, ")", _ );
			}
		putenv( strdcpy( &s, s1, "=",  product_name, _ ) );
		}
	else
		strdcpy( &product_name, v, _ );


	// INIT DIRBIN

	strdcpy( &s1, module_name, "DIRBIN", _ );
	v = getenv( upper(s1) );
	strdcpy( &s2, module_name, "DIRDAT", _ );
	v1 = getenv( upper(s2) );
	if (( !v )||( v[0] == 0 )||( !v1 )||( v1[0] == 0 ))
		{
		#ifdef OS_WINDOWS
		// SI LES VARIABLES D'ENVIRONNEMENTS N'ONT PAS ETE INITIALISEES
		// => RECHERCHE LE FICHIER DE CONF SSIE7DIR.CFG DANS LE REPERTOIRE COURANT

		strdcpy( &fname, module_name, "dir.cfg", _ );

		if ( !file_exists( fname ) )
			{
			// PAS TROUVE => ON CHERCHE DANS LE MEME REPERTOIRE QUE LE PROGRAMME EN COURS
			strdcpy( &fname, file_getdir( argv[0] ), DIR_CHAR, module_name, "dir.cfg", _ );
			if ( !file_exists( fname ) )
				{
				strdcpy( &dirbin, file_getdir( file_get_fullpath( argv[0] ) ), _ );
				goto init_suite;
				}
			}

		// LE FICHIER EXISTE => ON LE CHARGE EN MEMOIRE

		file_load( &s, fname );
		h = var_from_cmd( 0, s );

		// CHARGE LE REPERTOIRE DES BINAIRES
		strdcpy( &dirbin, var_get( h, "dirbin" ), _ );

		// CHARGE LE REPERTOIRE DES FICHIERS DE CONFIGURATION
		strdcpy( &dirconf, var_get( h, "dirconf" ), _ );

		// CHARGE LE REPERTOIRE DES FICHIERS DE DONNEES
		strdcpy( &dirdat, var_get( h, "dirdat" ), _ );

		// CHARGE LE REPERTOIRE DES LOG
		strdcpy( &dirlog, var_get( h, "dirlog" ), _ );

		// CHARGE LE REPERTOIRE DE TRANSITE DES FICHIERS A ENVOYER
		// s'il n'est pas fourni => dirtemp = dirdat + '/temp'
		v = var_get( h, "dirtemp" );
		if ( v ) strdcpy( &dirtemp, v, _ );
		else strdcpy( &dirtemp, dirdat, DIR_CHAR "temp", _ );

		// CHARGE LE REPERTOIRE DES FICHIERS DE TRAVAIL
		// s'il n'est pas fourni => dirwrk = dirdat + '/wrk'
		v = var_get( h, "dirwrk" );
		if ( v ) strdcpy( &dirwrk, v, _ );
		else strdcpy( &dirwrk, dirdat, DIR_CHAR "wrk", _ );

		// CHARGE LE REPERTOIRE DES FICHIERS TEMPORAIRES
		// s'il n'est pas fourni => dirtmp = dirwrk + '/tmp'
		v = var_get( h, "dirtmp" );
		if ( v ) strdcpy( &dirtmp, v, _ );
		else strdcpy( &dirtmp, dirwrk, DIR_CHAR "tmp", _ );

		// CHARGE LE REPERTOIRE D'ARRIVE DES FICHIERS PAR MFT
		// s'il n'est pas fourni => dirrecv = dirdat + '/recv'
		v = var_get( h, "dirrecv" );
		if ( v ) strdcpy( &dirrecv, v, _ );
		else strdcpy( &dirrecv, dirdat, DIR_CHAR "recv", _ );

		// CHARGE LE REPERTOIRE DE TRACE DES FICHIERS ENVOYES PAR MFT
		// s'il n'est pas fourni => dirsend = dirdat + '/send'
/*
		v = var_get( h, "dirsend" );
		if ( v ) strdcpy( &dirsend, v, _ );
		else strdcpy( &dirsend, dirdat, DIR_CHAR "send", _ );
*/
		// CHARGE LE REPERTOIRE DE TRACE DES EXECUTIONS DES FLUX
		// s'il n'est pas fourni => dirflux = dirdat + '/flux'
		v = var_get( h, "dirflux" );
		if ( v ) strdcpy( &dirflux, v, _ );
		else strdcpy( &dirflux, dirdat, DIR_CHAR "flux", _ );

		// CHARGE LE REPERTOIRE OU SE TROUVE LE PROGRAMME MFTUTIL
		// S'il n'est pas dans dirbin, il doit être fourni sous peine de non
		// fonctionnement.
		v = var_get( h, "dirmftutil" );
		if ( v )	putenv( str( "SSIE7MFTUTIL=",  v, _ ) );

		// CHARGE LE REPERTOIRE DES IDF MFT
		// (pour simulation des réveils MFT)
		// s'il n'est pas fourni => diridf = dirconf + '/mftidf'
		v = var_get( h, "diridf" );
		if ( v ) strdcpy( &diridf, v, _ );
		else strdcpy( &diridf, dirconf, DIR_CHAR "mftidf", _ );


		// POSITIONNE LES VARIABLES D'ENVIRONNEMENT AVEC LES INFO
		// PRECEDEMMENT CHARGEES

		putenv( str( "SSIE7DIRBIN=",  dirbin, _ ) );
		putenv( str( "SSIE7DIRDAT=",  dirdat, _ ) );
		putenv( str( "SSIE7DIRCONF=", dirconf, _ ) );
		putenv( str( "SSIE7DIRLOG=",  dirlog, _ ) );
		putenv( str( "SSIE7DIRTMP=",  dirtmp, _ ) );
		putenv( str( "SSIE7DIRWRK=",  dirwrk, _ ) );
		putenv( str( "SSIE7DIRRECV=", dirrecv, _ ) );
//		putenv( str( "SSIE7DIRSEND=", dirsend, _ ) );
		putenv( str( "SSIE7DIRFLUX=", dirflux, _ ) );
		putenv( str( "SSIE7DIRIDF=",  diridf, _ ) );

		// FIN DE CHARGEMENT DU FICHIER DE CONF SSIE7DIR.CFG (WINDOWS UNIQUEMENT)

		#else
		strdcpy( &dirbin, BIN_DIR, _ );
		putenv( strdcpy( &s, s1, "=",  dirbin, _ ) );
		#endif
		}
	else
		strdcpy( &dirbin, v, _ );

	// INIT DIRAPP

	init_suite:

	if ( ! dirapp )
		{
		strdcpy( &s1, module_name, "DIRAPP", _ );
		v = getenv( upper(s1) );
		if (( !v )||( v[0] == 0 ))
			{
			#ifndef IHM_LOCALE
			strdcpy( &dirapp, APPLI_DIR, DIR_CHAR, module_name, _ );
			#else
			strdcpy( &dirapp, getenv( "LOCALAPPDATA"), DIR_CHAR, module_name, _ );
			#endif
			putenv( strdcpy( &s, s1, "=",  dirapp, _ ) );
			}
		else
			strdcpy( &dirapp, v, _ );
		}


	// INIT DIRDAT

	if ( ! dirdat )
		{
		strdcpy( &s1, module_name, "DIRDAT", _ );
		v = getenv( upper(s1) );
		if (( !v )||( v[0] == 0 ))
			{
			strdcpy( &dirdat, dirapp, DIR_CHAR "data", _ );
			putenv( strdcpy( &s, s1, "=",  dirdat, _ ) );
			}
		else
			strdcpy( &dirdat, v, _ );
		}


	// INIT DIRLOG

	if ( ! dirlog )
		{
		strdcpy( &s1, module_name, "DIRLOG", _ );
		v = getenv( upper(s1) );
		if (( !v )||( v[0] == 0 ))
			{
			strdcpy( &dirlog, dirapp, DIR_CHAR "log", _ );
			putenv( strdcpy( &s, s1, "=",  dirlog, _ ) );
			}
		else
			strdcpy( &dirlog, v, _ );
		}


	// INIT DIRCONF

	if ( ! dirconf )
		{
		strdcpy( &s1, module_name, "DIRCONF", _ );
		v = getenv( upper(s1) );
		if (( !v )||( v[0] == 0 ))
			{
			strdcpy( &dirconf, dirapp, DIR_CHAR "conf", _ );
			putenv( strdcpy( &s, s1, "=",  dirconf, _ ) );
			}
		else
			strdcpy( &dirconf, v, _ );
		}


	// INIT DIRWRK

	if ( ! dirwrk )
		{
		strdcpy( &s1, module_name, "DIRWRK", _ );
		v = getenv( upper(s1) );
		if (( !v )||( v[0] == 0 ))
			{
			strdcpy( &dirwrk, dirdat, DIR_CHAR "wrk", _ );
			putenv( strdcpy( &s, s1, "=",  dirwrk, _ ) );
			}
		else
			strdcpy( &dirwrk, v, _ );
		}


	// INIT DIRIDF

	if ( ! diridf )
		{
		strdcpy( &s1, module_name, "DIRIDF", _ );
		v = getenv( upper(s1) );
		if (( !v )||( v[0] == 0 ))
			{
			strdcpy( &diridf, dirconf, DIR_CHAR "mftidf", _ );
			putenv( strdcpy( &s, s1, "=",  diridf, _ ) );
			}
		else
			strdcpy( &diridf, v, _ );
		}


	// INIT DIRTMP

	if ( ! dirtmp )
		{
		strdcpy( &s1, module_name, "DIRTMP", _ );
		v = getenv( upper(s1) );
		if (( !v )||( v[0] == 0 ))
			{
			strdcpy( &dirtmp, dirwrk, DIR_CHAR "tmp", _ );
			putenv( strdcpy( &s, s1, "=",  dirtmp, _ ) );
			}
		else
			strdcpy( &dirtmp, v, _ );
		}

	if ( !dir_exists( dirtmp ) )	mkdir( dirtmp );


	// INIT DIRTEMP

	if ( ! dirtemp )
		{
		strdcpy( &s1, module_name, "DIRTEMP", _ );
		v = getenv( upper(s1) );
		if (( !v )||( v[0] == 0 ))
			{
			strdcpy( &dirtemp, dirdat, DIR_CHAR "temp", _ );
			putenv( strdcpy( &s, s1, "=",  dirtemp, _ ) );
			}
		else
			strdcpy( &dirtemp, v, _ );
		}


	// INIT DIRPART

	strdcpy( &s1, module_name, "DIRPART", _ );
	v = getenv( upper(s1) );
	if (( !v )||( v[0] == 0 ))
		{
		strdcpy( &dirpart, dirconf, DIR_CHAR "part", _ );
		putenv( strdcpy( &s, s1, "=",  dirpart, _ ) );
		}
	else
		strdcpy( &dirpart, v, _ );


	// CALCUL LES REPERTOIRES QUI DECOULENT DE CEUX PRECEDEMMENT CHARGES.

		strdcpy( &dirrecv, dirdat, DIR_CHAR "recv", _ );
//		strdcpy( &dirsend, dirdat, DIR_CHAR "send", _ );
		strdcpy( &dirflux, dirdat, DIR_CHAR "flux", _ );

		// REPERTOIRE DE CONFIGURATION DES FLUX
		strdcpy( &dircflux		, dirconf , 	DIR_CHAR "flux", _ );

		// REPERTOIRE DES FICHIERS DES FLUX EXPORTES
		strdcpy( &direxport		, dirdat , 		DIR_CHAR "export", _ );

		// REPERTOIRE DES FICHIERS DES FLUX A IMPORTER
		strdcpy( &dirimport		, dirdat , 		DIR_CHAR "import", _ );

		// REPERTOIRE DES PROCESSUS A SURVEILLER
//		strdcpy( &dirpsurv		, dirwrk , 		DIR_CHAR "psurv", _ );

		// REPERTOIRE DES PROCESSUS A DECLANCHER A UNE DATE PRECISE
//		strdcpy( &dirwdate		, dirwrk , 		DIR_CHAR "wdate", _ );

		// REPERTOIRE DES PROCESSUS EN ATTENTE DE RECEPTION MFT
//		strdcpy( &dirwrecv		, dirwrk , 		DIR_CHAR "wrecv", _ );

		// REPERTOIRE DES PROCESSUS EN ATTENTE DE FICHIER DANS UN REPERTOIRE
//		strdcpy( &dirwfile		, dirwrk , 		DIR_CHAR "wfile", _ );

		// REPERTOIRE DES FLUX EN ATTENTE DE QUELQUE CHOSE
//		strdcpy( &dirrflux		, dirwrk , 		DIR_CHAR "rflux", _ );

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


	// CHARGE LE FICHIER DE CONFIGURATION DU SERVEUR

	server_conf_load( );


	#ifdef OS_UNIX

	if ( var_get( hSrvConf, "est_PEPSIS" ) )
		{
		strdcpy( &ssie_user, "mersi", _ );
		bPEPSIS = true;
		strdcpy( &dirtmp, "/appli/ssie7/data/tmp", _ );
		}
	else
		{
		strdcpy( &ssie_user, var_get( hSrvConf, "ssie_user" ), _ );
		if ( ssie_user[0] == 0 ) strdcpy( &ssie_user, "ssie", _ );
		}

	if ( stricmp( sysuser(), ssie_user ) )
		{
		if ( argc < 2 )
			// MODE CONSOLE INTERDIT SOUS UN AUTRE LOGIN QUE SSIE
			ssie7exit(1);
		strdcpy( &dirtmp, "/tmp", _ );
		}
	#else

	ssie_user = "ssie";

	#endif
	}




//------------------------------------------------------------------------------
int ssie7_cmd( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	char **mts, *sep = null;
	int _i;


	if ( what[0] == '[' )
		{
		p = strpos( cmd, "]", 0 );
		if ( p > 0 )
			{
			strdncpy( &sep, cmd, p+1, _ );
			trim(sep);
			mts = str_explode( &(cmd[p+1]), sep );
			for( _i = 0 ; mts[_i] ; _i++ )
				{
				if ( mts[_i][0] == 0 ) continue;
				strdcpy( &cmd, mts[_i], _ );
				print( "<b>", cmd, "</b><br>", _ );
				var_free( hReq );
				hReq = var_from_cmd( 0, cmd );
				var_getn( hReq, 0, &what );
				ssie7_cmd( t_work );
				print( "<br><br><br>", _ );
				}
			free( mts );
			return OK;
			}
		}


//tr_init(TR_DIR);

	#ifdef OS_UNIX
	if ( ( request_mode != MODE_TCP )&&( stricmp( sysuser(), ssie_user ) )
		&&( stricmp( sysuser(), "root" ) )&&( stricmp( what, "request" ) ) )
		{
		// RECHERCHE LE N° PORT DU SERVEUR SSIE

		System( &s, "ps -ef | grep \"ssie7 server \" | grep -v grep" );
		p = strpos( s, "ssie7 server ", 0 );
		if ( p >= 0 )
			v = itoa( atoi( &(s[p+13]) ) );
		else
			{
			print( "ERREUR : ssie7 server n'est pas démarré" BR, _ );
			return ERROR;
			}


		// ENVOI LA REQUETE

		strdcpy( &s1, "127.0.0.1:", v, _ );
		strdcat( &cmd, " user=", sysuser(), _ );

		if ( tcp_request( s1, cmd, &s ) == sERROR )
			{
			print( "ERREUR : ", error_msg, BR, _ );
			return ERROR;
			}

		print( s, BR, _ );

		return OK;
		}
	#endif


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
		if ( strcmp( user, ssie_user ) )
			logw( user, " : ", cmd, _ );
		#endif
		else
		if ( request_mode == MODE_CONSOLE )		logw( "CONSOLE (s) : ", cmd, _ );
		}



	if ( !stricmp( what, "connect" ) )		return cmd_connect( t_work );

//	if ( !stricmp( what, "vdisk_server" ) )	return cmd_vdisk_server( t_work );

	if (( !stricmp( what, "mft_server_recv" ) )||( !stricmp( what, "send_server" ) ))
											return cmd_mft_server_recv( t_work );

	if (( !stricmp( what, "mft_server_send" ) )||( !stricmp( what, "recv_server" ) ))
											return cmd_mft_send_part( t_work );

	strdcpy( &fname, dirbin, DIR_CHAR, module_name, "_", what, EXE, _ );
	if ( file_exists( fname ) )
		{
		file_tmpname( &fname1 );
		strdcpy( &cmd1, cmd, " ip_cli='", ip_cli, "' user='", user, "'", _ );
		file_save_str( fname1, cmd1, -1 );
		strdcpy( &cmd1, fname, " @", fname1, _ );
		System( &s, cmd1 );
		print( s, _ );
		return OK;
		}


	#ifdef OS_WINDOWS
	if ( !stricmp( what, "bdoc" ) )				return cmd_bdoc( t_work );
	#endif

	if ( !stricmp( what, "mftcat" ) )			return cmd_mft_cat( t_work );

	if ( !stricmp( what, "mft" ) )	{log_id=0;	return cmd_mft( t_work );}

	if ( !stricmp( what, "mersi_emission" ) )	return cmd_mersi_emission( t_work );

	if ( !stricmp( what, "mersi_reception" ) )	return cmd_mersi_reception( t_work );

	if ( !stricmp( what, "mersi_liste" ) )		return cmd_mersi_liste( t_work );


	if ( !stricmp( what, "mftutil" ) )		return cmd_mftutil( t_work );
	if ( !stricmp( what, "mftmsg" ) )		return cmd_mftmsg( t_work );

	if ( !stricmp( what, "listcat" ) )		{
											var_free( hReq ); strdcpy( &cmd, "mftutil ", cmd, _ );
											hReq = var_from_cmd( 0, cmd );
											return cmd_mftutil( t_work );
											}


	if ( !stricmp( what, "request" ) )		return cmd_request( t_work );


	if (( request_mode == MODE_TCP )&&( ! is_authorized( null, "visiteur" ) ))
		{
		print( "ERREUR : Accès non autorisé.", BR, _ );
		return ERROR;
		}



	if ( !stricmp( what, "version" ) )
		{	print( product_name, BR, _ );	return OK;	}

	if ( !stricmp( what, "log" ) )			return cmd_log( t_work );

	if ( !stricmp( what, "flux" ) )			return cmd_flux( t_work );

	if ( !stricmp( what, "part" ) )			return cmd_part( t_work );

	if ( !stricmp( what, "export" ) )		return cmd_export( t_work );

	if ( !stricmp( what, "import" ) )		return cmd_import( t_work );

	if ( !stricmp( what, "login" ) )		return cmd_login( t_work );

	if ( !stricmp( what, "app" ) )			return cmd_app( t_work );

	if ( !stricmp( what, "report" ) )		return cmd_report( t_work );

	if ( !stricmp( what, "report_compute" )) return report_compute( t_work );

/*
	if ( ( !stricmp( what, "mft" ) )&&( mft_exists() ) )
											return cmd_mft( t_work );
*/

	if ( !stricmp( what, "status" ) )		return cmd_ssie7_status( t_work );

	if ((!stricmp( what, "server" ) )||( !stricmp( what, "srv" ) ))
											return cmd_server( t_work );

//	if ( !stricmp( what, "vdisk" ) )		return cmd_vdisk_client( t_work );


	if ( ! is_authorized( "is_admin", null ) )
		{
		print( "ERREUR : Commande non autorisée." BR, _ );
		return ERROR;
		}



	#ifdef OS_WINDOWS

	if ( !stricmp( what, "start" ) )		return cmd_ssie7_ask_start( t_work );

	if ( !stricmp( what, "stop" ) )			return cmd_ssie7_ask_stop( t_work );

	if ( !stricmp( what, "ds" ) )			return cmd_ssie7_start( t_work );

	if ( !stricmp( what, "sq" ) )			exit( cmd_ssie7_stop( t_work ) );
	#else


	if ( !stricmp( what, "start" ) )		return cmd_ssie7_start( t_work );

	if ( !stricmp( what, "stop" ) )			return cmd_ssie7_stop( t_work );


	#endif

	if ( !stricmp( what, "do_start" ) )		return cmd_ssie7_start( t_work );
	if ( !stricmp( what, "do_stop" ) )		return cmd_ssie7_stop( t_work );


	if ( !stricmp( what, "LockServer" ) )
		{
		if ( request_mode == MODE_TCP ) return ERROR;
		strdcpy( &fname, dirwrk, DIR_CHAR "LockServer", _ );
		file_save_str( fname, "", 0 );
		return OK;
		}

	if ( !stricmp( what, "gateway" ) )		return cmd_ssie7_gateway( t_work );

	if ((!stricmp( what, "srvannu" ) )||( !stricmp( what, "srvnat" ) ))
											return cmd_srvannu( t_work );

	if ( !stricmp( what, "nat_request" ) )	return cmd_natrequest( t_work );

//	if ( !stricmp( what, "upgrade" ) )		return cmd_upgrade( t_work );
//	if ( !stricmp( what, "uprc" ) )			return cmd_uprc( t_work );


	if ( !stricmp( what, "print" ) )		return cmd_var_print( t_work );

	if ( !stricmp( what, "varset" ) )		return cmd_var_set( t_work );

	if ( !stricmp( what, "ssiedir" ) )
		{
		print(
			"dirbin = ", dirbin,  BR
			"dirapp = ", dirapp,  BR
			"dirdat = ", dirdat,  BR
			"dirtmp = ", dirtmp,  BR
			"dirconf= ", dirconf, BR
			"diridf = ", diridf,  BR
			"dirwrk = ", dirwrk,  BR
			"dirpart= ", dirpart, BR
			"dirflux= ", dirflux, BR
			"dirwww = ", var_get( hSrvConf, "dirwww" ), BR
			#ifdef OS_UNIX
			"ssie_user=", ssie_user, BR
			#endif
			,_ );
		if ( bPEPSIS )
			printf( "est_PEPSIS=Oui" BR, _ );

		return OK;
		}


	#ifdef TESTS
	if ( !stricmp( what, "test" ) )			return cmd_test( t_work );
	if ( !stricmp( what, "t" ) )			return cmd_test( t_work );
	#endif

	#ifdef OS_WINDOWS
	if ( !stricmp( what, "sysuser" ) )
		{
		print( var_get( hSrvConf, "partname" ), _ );
//		print( sysuser(), _ );
		return OK;
		}

	if ( !stricmp( what, "ihm_server_list" ) )
		{
		strdcpy( &fname, dirconf, DIR_CHAR "srv_list.cfg", _ );
		if ( !file_exists( fname ) ) print( "[]", _ );
		else print( file_load( &s, fname ), _ );
		return OK;
		}

	if ( !stricmp( what, "ihm_server_list_save" ) )
		{
		strdcpy( &fname, dirconf, DIR_CHAR "srv_list.cfg", _ );
		v = var_get( hReq, "srv_list" );
		if ( !v )
			{
			print( "ERREUR : srv_list non fourni", _ );
			return ERROR;
			}
		file_save_str( fname, v, -1 );
		print( "OK", _ );
		return OK;
		}
	#endif


	return cmd_system( t_work );
	}


















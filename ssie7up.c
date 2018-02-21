#define MAIN

#define PRODUCT_NAME                "SSIE UPGRADE v7.2.a.5 - 2017/01/03"
#define SSIE_VERSION				"SSIE v7.2.a.6 - 2017/08/25"
#define MODULE_NAME                 "ssie7"
#define DEFAULT_PORT                "1765"

#include "elib4.c"
#define MFT
#include "ssie7lib.c"


#undef print

#ifdef OS_WINDOWS
	#define APPLI_DIR		"c:\\edf\\appli"
	#define BIN_DIR			"d:\\r2000\\mft"
	#define OPENSSL			"c:\\e\\Apps\\OpenSSL\\openssl.exe"
	#define LOGICIELS
//	#define SAUVE_DIR		"c:\\e\\apps\\t0_ssie\\ssie\\data\\ssie7up_save"
	#define SAUVE_DIR		"d:\\r2000\\mft\\ssie\\data\\ssie7up_save"
#endif

#ifdef OS_AIX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/ssie7"
	#define OPENSSL			null
	#define LOGICIELS
	#define SAUVE_DIR		"/appli/ssie7/data/ssie7up_save"
#endif

#ifdef OS_LINUX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/logiciels/ssie7"
	#define OPENSSL			null
	#define LOGICIELS		"/logiciels"
	#define SAUVE_DIR		"/appli/ssie7/data/ssie7up_save"
#endif

#ifdef OS_SOLARIS
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/logiciels/ssie7"
	#define OPENSSL			null
	#define LOGICIELS		"/logiciels"
	#define SAUVE_DIR		"/appli/ssie7/data/ssie7up_save"
#endif



//------------------------------------------------------------------------------
// DEFINITION DES VARIABLES GLOBALES

EXTERN char * appname			init_null;
EXTERN char * dircflux			init_null;
EXTERN char * dirflux			init_null;
EXTERN char * dirrflux			init_null;
EXTERN char * flux_id			init_null;
EXTERN char * fluxname			init_null;
EXTERN char * direxport			init_null;
EXTERN char * dirimport			init_null;
EXTERN char * export_id			init_null;
EXTERN char * import_id			init_null;

EXTERN char * diridf			init_null;

#define hFlux	hConf
#define hEtape	hConf1


bool bMuet = false;
bool bDMZ = false;
bool bGO = 0;

bool bKeepUserMersi = false;
bool bRoot = false;
bool bMigrPEPSIS = false;

#define PackageCode	argv[1+bGO]

//------------------------------------------------------------------------------

typedef struct
	{
	int parent_id;
	int first_child_id;
	int last_child_id;
	int next_id;
	char when[21];
	char status[200 + 1];
	char log[500 + 1];
	}
	t_elib_log1;

t_elib_log1 *elib_log1;
t_elib_log  *elib_log;


//------------------------------------------------------------------------------
#include "ssie7lib_mft.c"
#include "ssie7up_lib.c"

//#ifdef OS_UNIX
#include "ssie7up_migration_mft.c"
#include "ssie7up_migration_mersi.c"
//#endif

#include "ssie7up_back.c"
#include "ssie7up_run.c"


//------------------------------------------------------------------------------
int main(int ac, char *av[])
//------------------------------------------------------------------------------
	{
	T_WORK_ALLOC;

	argc = ac;
	argv = av;


	ssie_user = "ssie";

	System( &s, "whoami" );
	trim(s);
	if ( ( !strcmp( s, "root" ) )||( !strcmp( s, "rootmin" ) ) )
		bRoot = true;

	if (( argc >= 4 )&&( !stricmp( argv[1], "-add" ) ))
		{
		// ssie7up -add {resultname} {fname} {dirdest}
		if ( !file_exists( argv[2] ) )
			{
			file_copy( argv[0], argv[2] );
			}

		if ( !file_exists( argv[2] ) )
			{
			print( "ERREUR : le fichier n'existe pas : ", argv[2], _ );
			}

		if ( argc > 4 ) v = argv[4];
		else v = null;
		autoextract_add_files( argv[2], argv[3], v );
		exit(0);
		}

	if (( argc >= 3 )&&( !stricmp( argv[1], "-list" ) ))
		{
		// ssie7up -list {fname}

		if ( !file_exists( argv[2] ) )
			{
			print( "ERREUR : le fichier n'existe pas : ", argv[2], _ );
			}

		autoextract_list( argv[2] );
		exit(0);
		}

	if (( argc > 1 )&&( !stricmp( argv[1], "go" ) )) bGO = 1;

	// CALCUL LE MODULE_NAME
	// C'est le nom du programme en train de tourner.
	// Il permet de nommer certain fichier

		module_name = MODULE_NAME;

	// CALCUL PRODUCT_NAME
	// C'est nom du produit avec sa version.
	// Il sert à l'affichage et à la transmission des informations du serveur
		product_name	= PRODUCT_NAME;

	// CALCUL DEFAULT_PORT
	// C'est le numéro de port par défaut
		default_port	= DEFAULT_PORT;


	// OBTENTION DES REPERTOIRES PRINCIPAUX

		v = getenv( "SSIE7DIRBIN" );

		#ifdef OS_WINDOWS

		if ( ( !v )||( ! (v[0]) ) )
			{
			// SI LES VARIABLES D'ENVIRONNEMENTS N'ONT PAS ETE INITIALISEES
			// => RECHERCHE LE FICHIER DE CONF SSIE7DIR.CFG DANS LE REPERTOIRE COURANT

			strdcpy( &fname, "ssie7dir.cfg", _ );
			if (( !file_exists( fname ) )&&( dir_exists( "D:\\R2000\\MFT" ) ))
				{

				// PAS TROUVE => ON CHERCHE DANS LE MEME REPERTOIRE QUE LE PROGRAMME EN COURS
				strdcpy( &fname, file_getdir( argv[0] ), DIR_CHAR "ssie7dir.cfg", _ );
				if ( !file_exists( fname ) )
					{
					// TOUJOURS PAS TROUVE => ERREUR
					file_save_str( "SSIE7.ERREUR", "ERREUR : fichier ssie7dir.cfg non trouvé" BR, -1 );
					print( "ERREUR : fichier ssie7dir.cfg non trouvé : ", fname, BR, _ );
					wexit(1);
					}
				}

			if ( file_exists( fname ) )
				{
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
				}
			else
				{
				if ( dir_exists( "c:\\e\\bin\\windows" ) )
					{
					strdcpy( &dirbin, "c:\\e\\bin\\windows", _ );
					strdcpy( &dirconf, "c:\\e\\apps\\ssie7\\conf", _ );
					strdcpy( &dirdat, "c:\\e\\apps\\ssie7\\data", _ );
					strdcpy( &dirlog, "c:\\e\\apps\\ssie7\\log", _ );
					strdcpy( &dirtemp, dirdat, DIR_CHAR "temp", _ );
					strdcpy( &dirwrk, dirdat, DIR_CHAR "wrk", _ );
					strdcpy( &dirtmp, dirwrk, DIR_CHAR "tmp", _ );
					strdcpy( &dirrecv, dirdat, DIR_CHAR "recv", _ );
//					strdcpy( &dirsend, dirdat, DIR_CHAR "send", _ );
//					strdcpy( &dirflux, dirdat, DIR_CHAR "flux", _ );
					strdcpy( &diridf, dirconf, DIR_CHAR "mftidf", _ );
					}
				else
					{
					// PAS TROUVE => ON CHERCHE DANS LE MEME REPERTOIRE QUE LE PROGRAMME EN COURS
					strdcpy( &fname, file_getdir( argv[0] ), DIR_CHAR "ssie7dir.cfg", _ );
					if ( !file_exists( fname ) )
						{
						// TOUJOURS PAS TROUVE => ERREUR
						file_save_str( "SSIE7.ERREUR", "ERREUR : fichier ssie7dir.cfg non trouvé" BR, -1 );
						print( "ERREUR : fichier ssie7dir.cfg non trouvé : ", fname, BR, _ );
						wexit(1);
						}
					}
				}

			// POSITIONNE LES VARIABLES D'ENVIRONNEMENT AVEC LES INFO
			// PRECEDEMMENT CHARGEES

			putenv( str( "SSIE7DIRBIN=",  dirbin, _ ) );
			putenv( str( "SSIE7DIRDAT=",  dirdat, _ ) );
			putenv( str( "SSIE7DIRCONF=", dirconf, _ ) );
			putenv( str( "SSIE7DIRLOG=",  dirlog, _ ) );
			putenv( str( "SSIE7DIRTMP=",  dirtmp, _ ) );
			putenv( str( "SSIE7DIRWRK=",  dirwrk, _ ) );
			putenv( str( "SSIE7DIRRECV=", dirrecv, _ ) );
//			putenv( str( "SSIE7DIRSEND=", dirsend, _ ) );
			putenv( str( "SSIE7DIRFLUX=", dirflux, _ ) );
			putenv( str( "SSIE7DIRIDF=",  diridf, _ ) );

			// FIN DE CHARGEMENT DU FICHIER DE CONF SSIE7DIR.CFG (WINDOWS UNIQUEMENT)
			}


		else

		#endif

			{
			// SI NOUS ARRIVONS LA, NOUS SOMMES SOUS UNIX
			// OU SOUS WINDOWS AVEC LES VARIABLES D'ENVIRONNEMENTS QUI ONT ETE POSITIONNEES

			// CHARGE LE REPERTOIRE DES BINAIRES.
			// Soit c'est $SSIE7DIRBIN soit c'est selon l'OS : /ssie7 (AIX) ou /logiciels/ssie7 (Linux et Solaris)
			strdcpy( &dirbin, (char*)( ( ( !v )||( !(v[0]) ) ) ? BIN_DIR : v ), _ );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &dirbin, BIN_DIR, _ );
				putenv( str( "SSIE7DIRBIN=",  dirbin, _ ) );
				}
			else
				strdcpy( &dirbin, v, _ );


			// CHARGE LE REPERTOIRE DES FICHIERS DE CONFIGURATION.
			// Soit c'est $SSIE7DIRCONF soit c'est /appli/ssie7/conf
			v = getenv( "SSIE7DIRCONF" );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &dirconf, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "conf", _ );
				putenv( str( "SSIE7DIRCONF=",  dirconf, _ ) );
				}
			else
				strdcpy( &dirconf, v, _ );


			// CHARGE LE REPERTOIRE DES FICHIERS DE DONNEES.
			// Soit c'est $SSIE7DIRDAT soit c'est /appli/ssie7/data
			v = getenv( "SSIE7DIRDAT" );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &dirdat, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "data", _ );
				putenv( str( "SSIE7DIRDAT=",  dirdat, _ ) );
				}
			else
				strdcpy( &dirdat, v, _ );


			// CHARGE LE REPERTOIRE DES FICHIERS DE LOG.
			// Soit c'est $SSIE7DIRLOG soit c'est /appli/ssie7/log
			v = getenv( "SSIE7DIRLOG" );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &dirlog, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "log", _ );
				putenv( str( "SSIE7DIRLOG=",  dirlog, _ ) );
				}
			else
				strdcpy( &dirlog, v, _ );


			// CHARGE LE REPERTOIRE DE TRANSITE DES FICHIERS A ENVOYER.
			// Soit c'est $SSIE7DIRTEMP soit c'est $dirdat/temp
			v = getenv( "SSIE7DIRTEMP" );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &dirtemp, dirdat, DIR_CHAR "temp", _ );
				putenv( str( "SSIE7DIRTEMP=",  dirtemp, _ ) );
				}
			else
				strdcpy( &dirtemp, v, _ );


			// CHARGE LE REPERTOIRE DES FICHIERS DE TRAVAIL.
			// Soit c'est $SSIE7DIRWRK soit c'est $dirdat/wrk
			v = getenv( "SSIE7DIRWRK" );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &dirwrk, dirdat, DIR_CHAR "wrk", _ );
				putenv( str( "SSIE7DIRWRK=",  dirwrk, _ ) );
				}
			else
				strdcpy( &dirwrk, v, _ );


			// CHARGE LE REPERTOIRE DES FICHIERS TEMPORAIRES.
			// Soit c'est $SSIE7DIRTMP soit c'est $dirwrk/tmp
			v = getenv( "SSIE7DIRTMP" );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &dirtmp, dirwrk, DIR_CHAR "tmp", _ );
				putenv( str( "SSIE7DIRTMP=",  dirtmp, _ ) );
				}
			else
				strdcpy( &dirtmp, v, _ );


			// CHARGE LE REPERTOIRE D'ARRIVE DES FICHIERS RECU PAR MFT.
			// Soit c'est $SSIE7DIRRECV soit c'est $dirdat/recv
			v = getenv( "SSIE7DIRRECV" );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &dirrecv, dirdat, DIR_CHAR "recv", _ );
				putenv( str( "SSIE7DIRRECV=",  dirrecv, _ ) );
				}
			else
				strdcpy( &dirrecv, v, _ );


			// CHARGE LE REPERTOIRE DE TRACE DES FICHIERS ENVOYES PAR MFT.
			// Soit c'est $SSIE7DIRSEND soit c'est $dirdat/send
/*
			v = getenv( "SSIE7DIRSEND" );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &dirsend, dirdat, DIR_CHAR "send", _ );
				putenv( str( "SSIE7DIRSEND=",  dirsend, _ ) );
				}
			else
				strdcpy( &dirsend, v, _ );
*/

			// CHARGE LE REPERTOIRE DE TRACE DES EXECUTIONS DES FLUX.
			// Soit c'est $SSIE7DIRFLUX soit c'est $dirdat/flux
			v = getenv( "SSIE7DIRFLUX" );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &dirflux, dirdat, DIR_CHAR "flux", _ );
				putenv( str( "SSIE7DIRFLUX=",  dirflux, _ ) );
				}
			else
				strdcpy( &dirflux, v, _ );


			// CHARGE LE REPERTOIRE DES IDF MFT
			// (pour simulation des réveils MFT)
			// Soit c'est $SSIE7DIRIDF soit c'est $dirconf/mftidf
			v = getenv( "SSIE7DIRIDF" );
			if ( ( !v )||( !(v[0]) ) )
				{
				strdcpy( &diridf, dirconf, DIR_CHAR "mftidf", _ );
				putenv( str( "SSIE7DIRIDF=",  diridf, _ ) );
				}
			else
				strdcpy( &diridf, v, _ );
			}


		// CALCUL LES REPERTOIRES QUI DECOULENT DE CEUX PRECEDEMMENT CHARGES.

			// CHARGE LE REPERTOIRE DE CONFIGURATION DES FLUX
			strdcpy( &dircflux		, dirconf , 	DIR_CHAR "flux", _ );

			// CHARGE LE REPERTOIRE DES FICHIERS DES FLUX EXPORTES
			strdcpy( &direxport		, dirdat , 		DIR_CHAR "export", _ );

			// CHARGE LE REPERTOIRE DES FICHIERS DES FLUX A IMPORTER
			strdcpy( &dirimport		, dirdat , 		DIR_CHAR "import", _ );

			// CHARGE LE REPERTOIRE DES PROCESSUS A SURVEILLER
//			strdcpy( &dirpsurv		, dirwrk , 		DIR_CHAR "psurv", _ );

			// CHARGE LE REPERTOIRE DES PROCESSUS A DECLANCHER A UNE DATE PRECISE
//			strdcpy( &dirwdate		, dirwrk , 		DIR_CHAR "wdate", _ );

			// CHARGE LE REPERTOIRE DES PROCESSUS EN ATTENTE DE RECEPTION MFT
//			strdcpy( &dirwrecv		, dirwrk , 		DIR_CHAR "wrecv", _ );

			// CHARGE LE REPERTOIRE DES PROCESSUS EN ATTENTE DE FICHIER DANS UN REPERTOIRE
//			strdcpy( &dirwfile		, dirwrk , 		DIR_CHAR "wfile", _ );

			// CHARGE LE REPERTOIRE DES FLUX EN ATTENTE DE QUELQUE CHOSE
//			strdcpy( &dirrflux		, dirwrk , 		DIR_CHAR "rflux", _ );

		// FIN DE CHARGEMENT DES REPERTOIRES PRINCIPAUX

	mkdir( dirtmp );

	#ifdef OS_UNIX
	strdcpy( &dirtmp, "/tmp", _ );
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
		exit(1);
		}



/*
	#ifdef OS_UNIX

	if ( is_sudo_conf_ok() != true )
		{
		print( "ERREUR : sudo ssie7 non paramétré !!!" BR BR, _ );
		exit(1);
		}


	if ( strcmp( sysuser(), "ssie" ) )
		{
		print( "ERREUR : Ce programme doit obligatoirement être démarré sous le login ssie" BR BR, _ );
		exit(1);
		}

	#endif
*/

    strdcpy( &ssie7_prefix, dirbin, DIR_CHAR "ssie7" EXE " ", _ );
	#ifdef OS_UNIX
    strdcpy( &sudo_ssie7_prefix, "sudo ", ssie7_prefix, _ );

    #else // OS_WINDOWS
    strdcpy( &sudo_ssie7_prefix, ssie7_prefix, _ );
    #endif


	if (( argc >= 2 )&&( !stricmp( argv[1], "-pepsis_conv" ) ))
		{
		strdcpy( &fname, dirlog, DIR_CHAR "ssie7up_", itod(0,"flog"), ".log", _ );
		print_to_file_and_screen( fname );
		migr_pepsis( t_work );
		// Mise à jour config serveur
		strdcpy( &fname, dirconf, DIR_CHAR "ssie7.dat", _ );
		h = var_load( fname );
		var_set( h, "est_PEPSIS", "Oui" );
		var_save( h, fname );

		print( BR "CONVERSION TERMINEE" BR BR, _ );
		exit(0);
		}

	if (( argc >= 2 )&&( !stricmp( argv[1], "-pepsis_unconv" ) ))
		{
		strdcpy( &fname, dirlog, DIR_CHAR "ssie7up_", itod(0,"flog"), ".log", _ );
		print_to_file_and_screen( fname );
		back_pepsis( t_work );

		print( BR "CONVERSION TERMINEE" BR BR, _ );
		exit(0);
		}

	if (
		(( argc >= 3 )&&( !stricmp( argv[2], "-pepsis_conv" ) ))
	  ||(( argc >= 4 )&&( !stricmp( argv[3], "-pepsis_conv" ) ))
	  ||(( argc >= 5 )&&( !stricmp( argv[4], "-pepsis_conv" ) ))
		)
		{
		bMigrPEPSIS = true;
		bKeepUserMersi = true;
		ssie_user = "mersi";
		}

	if (( argc >= 3 )&&( !stricmp( argv[2], "-keep_user_mersi" ) ))
		{
		// Option acceptée
		bKeepUserMersi = true;
		ssie_user = "mersi";
		}


	if (( argc >= (3+bGO) )&&( !stricmp( argv[2+bGO], "-noquestion" ) ))
		bMuet = true;

	// LECTURE DE LA VERSION ACTUELLE DE SSIE v7.

	strdcpy( &cmd, module_name, EXE, _ );
	if ( !strcmp( file_getname( argv[0] ), cmd ) )
		{
		print( "ERREUR : ce programme (ssie7up) ne doit pas s'appeler ", module_name, EXE, BR, _ );
		exit(1);
		}

	strdcpy( &cmd, dirbin, DIR_CHAR, module_name, EXE, _ );
	if ( file_exists( cmd ) )
		{
		strdcat( &cmd, " version", _ );
		System( &u15, cmd );
		trim( u15 );
		}
	else
		strdcpy( &u15, "SSIE v7 non encore installé", _ );

    print( BR, PRODUCT_NAME, ".", BR BR,
		"   Version ssie actuelle   : ", u15, BR, _ );

/*
	strdcpy( &cmd, dirbin, DIR_CHAR, module_name, EXE " nat_request \"part '" , _ );
	for( i = 0 ; v = tcp_myIP(i) ; i++ )
		strdcat( &cmd, (char*)((i)?",":""), v, _ );
	strdcat( &cmd, v, "' get\"", _ );
	System( &s, cmd );
	trim(s);
	if ( stripos( s, "erreur", 0 ) < 0 )
		{
		var_free( h );
		h = var_from_str( 0, s );
		v = var_get( h, "product_name" );
		}
	else
		v = s;

    print( "   Version dans l'annuaire : ", v, BR, _ );
*/



	b1 = ( argc == (1+bGO) );
	b2 = false; // Migration MFT (2) ou mersi3 (1)
	strdcpy( &s1, "", _ );

	u14 = null;


	print_list:

	if ( b1 ) print( BR "   LISTE DES MIGRATIONS POSSIBLES ET DES PATCH" BR BR, _ );
	b = false;


	#ifdef OS_UNIX
	if (( check_file( LOGICIELS "/mersi3/mersi3", t_work ) )
		&&( ! check_file( SAUVE_DIR "/mersi3", t_work ) )
		)
		{
		b = true; b2 = 1;
		if ( !b1 ) strdcat( &s1, ",last", _ );
		else print( "   - last : migration mersi v3 => ", SSIE_VERSION, BR, _ );
		}
	else
	if (( check_file( LOGICIELS "/mft/2.4.1/filexe/MFTMAIN", t_work ) )
		&&( ! check_file( SAUVE_DIR "/MFTMAIN", t_work ) )
		)
		{
		b = true; b2 = 2;
		if ( !b1 ) strdcat( &s1, ",last", _ );
		else print( "   - last : migration MFT v2.4.1 vers ", SSIE_VERSION, BR, _ );
		}
	else
	#endif
	if ( stricmp( u15, SSIE_VERSION ) )
		{
		b = true;
		if ( !b1 ) strdcat( &s1, ",last", _ );
		else print( "   - last : installation de " SSIE_VERSION BR BR, _ );
		}


	#ifdef OS_UNIX
	if ( check_file( SAUVE_DIR "/mersi3", t_work ) )
		{
		b = true; b2 = -1;
		if ( !b1 ) strdcat( &s1, ",backM", _ );
		else print( "   - backM : retour à mersi v3", BR, _ );
		}
//	else
	if ( check_file( SAUVE_DIR "/MFTMAIN", t_work ) )
		{
		b = true; b2 = -2;
		if ( !b1 ) strdcat( &s1, ",backMFT", _ );
		else print( "   - backMFT : retour à MFT v2.4.1", BR, _ );
		}
/*
	else
	if ( check_file( "/appli/ssie7/data/wrk/sauvegardes/MFTMAIN230", t_work ) )
		{
		b = true; b2 = 2;
		if ( !b1 ) strdcat( &s1, ",back", _ );
		else print( "   - back : retour à MFT v2.3.0", BR, _ );
		}
*/
	#endif
//	else
	if ( file_exists( SAUVE_DIR DIR_CHAR "ssie7" EXE ) )
		{
		strdcpy( &cmd, SAUVE_DIR DIR_CHAR "ssie7" EXE " version", _ );
		System( &u14, cmd );
		trim( u14 );
		b = true;
		if ( !b1 ) strdcat( &s1, ",back", _ );
		else print( "   - back : retour à la version : ", u14, BR BR, _ );
		}


	if ( b1 )
		{
		if ( b )
			print( BR "   Pour démarrer une migration ou une mise à jour, exécutez la commande :" BR
			"      ssie7up {code action} [-noquestion] [-keep_user_mersi]" BR
			"      (code action = code en début de chacune des lignes.)" BR BR, _ );

		else
			print( BR, "   Aucune mise à jour disponible." BR BR, _ );

		exit(1);
		}


	strdcat( &s1, ",", _ );
	strdcpy( &s, ",", PackageCode, ",", _ );

	if ( stripos( s1, s, 0 ) < 0 )
		{
		print( "ERREUR : Code action inconnu" BR BR, _ );
		b1 = true;
		goto print_list;
		}


	chdir( dirbin );
	mkdir( dirlog );

	if ( ! stricmp( PackageCode, "back" ) )
		{
		b2 = false;
		exit( upgrade_back( t_work ) );
		}
	else
	if ( ! stricmp( PackageCode, "backM" ) )
		{
		b2 = -1;
		exit( upgrade_back( t_work ) );
		}
	else
	if ( ! stricmp( PackageCode, "backMFT" ) )
		{
		b2 = -2;
		exit( upgrade_back( t_work ) );
		}
	else
		exit( upgrade_run( t_work ) );

	}

//-----------------------------------------------------------------------------------------





















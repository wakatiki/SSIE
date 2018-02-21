#define MAIN
#include "elib4.c"
#define MFT
#include "ssie7lib.c"

//------------------------------------------------------------------------------
// PARAMETRAGE DU MODULE SSIE7NAT

#define MODULE_NAME			"ssie7c"
#define PRODUCT_NAME		"SSIE Controleur v7.2.a"
#define DEFAULT_PORT		"1770"



#ifdef OS_WINDOWS
	#define APPLI_DIR		"c:\\e\\apps"
	#define BIN_DIR			"c:\\e\\bin\\windows"
	#define OPENSSL			"c:\\e\\bin\\windows\\ssie7sec.exe"
#endif

#ifdef OS_AIX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/"
	#define OPENSSL			null
#endif

#ifdef OS_LINUX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/logiciels/ssie7"
	#define OPENSSL			null
#endif

#ifdef OS_SOLARIS
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/logiciels/ssie7"
	#define OPENSSL			null
#endif


#define HTTP_AUTHORIZED
char * dirwww = "c:/e/src/ssie7/tbot";

#include "ssie7lib_cmd.c"
#include "ssie7lib_cmd_part.c"
#include "ssie7lib_cmd_connect.c"
#include "ssie7lib_cmd_exploit.c"
#include "ssie7lib_cmd_request.c"
#include "ssie7lib_cmd_server.c"
#include "ssie7lib_run.c"
#include "ssie7lib_robot.c"
#include "ssie7lib_vdisk.c"
#include "ssie7lib_vdisk_client.c"
#include "ssie7lib_vdisk_server.c"

//#include "ssie7c_cmd_gw_inst.c"
//#include "ssie7c_cmd_initialize.c"


// main , ssie7_init , server_run , robot_run

//------------------------------------------------------------------------------
int main(int ac, char *av[])
//------------------------------------------------------------------------------
	{
	argc = ac;
	argv = av;

	return( ssie7run() );
	}




//------------------------------------------------------------------------------
void ssie7_init( T_WORK* t_work )
	// Cette fonction est appelée immédiatement par ssie7run()
	// Elle permet d'initialiser le programme
//------------------------------------------------------------------------------
	{
	strdcpy( &module_name, file_getname( argv[0] ), _ );
	if ( ( p = stripos( module_name, ".exe", 0 ) ) >= 0 )
		module_name[p] = 0;

	strdcpy( &module_short_name, "Controleur", _ );
		
	product_name	= PRODUCT_NAME;
	default_port	= DEFAULT_PORT;
	
	strdcpy( &dirbin	, BIN_DIR, _ );
//	strdcpy( &dirappli	, APPLI_DIR	DIR_CHAR, module_name, _ );
	strdcpy( &dirconf	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "conf", _ );
	strdcpy( &dirdat	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "data", _ );
	strdcpy( &dirtmp	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "tmp", _ );
	strdcpy( &dirwrk	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "data" DIR_CHAR "wrk", _ );
	strdcpy( &dirlog	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "log", _ );
	
	// INITIALISATION D'OPENSSL
	
	if ( ssl_pgm( OPENSSL ) == null )
		{
		print( "ERREUR : openssl n'est pas installé ou est inaccessible." BR, __ );
		wexit(1);
		}

	
	// CHARGE LA CONFIGURATION
		
	server_conf_load();
	if ( !var_get( hSrvConf, "srvnat_list" ) )
		{
		var_set( hSrvConf, "srvnat_list", "none" );
		server_conf_save( );
		}
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

	if ( !stricmp( what, "vdisk" ) )		return cmd_vdisk_client( t_work );

	strdcpy( &fname, dirbin, DIR_CHAR, what, EXE, _ );
	if ( file_exists( fname ) )
		{
		if ( getenv( "SSIE_MODULE_NAME" ) == null )
			{
			putenv( strdcpy( &s, "SSIE_MODULE_NAME=",  module_name, _ ) );
			putenv( strdcpy( &s, "SSIE_DIRBIN=",  dirbin, _ ) );
			putenv( strdcpy( &s, "SSIE_DIRWRK=",  dirwrk, _ ) );
			putenv( strdcpy( &s, "SSIE_DIRTMP=",  dirtmp, _ ) );
			putenv( strdcpy( &s, "SSIE_DIRLOG=",  dirlog, _ ) );
			putenv( strdcpy( &s, "SSIE_DIRCONF=", dirconf, _ ) );
			putenv( strdcpy( &s, "SSIE_DIRDAT=",  dirdat, _ ) );
			putenv( strdcpy( &s, "SSIE_SERVER_MAXCNX=",  var_get(hSrvConf, "maxcnx"), _ ) );
			putenv( strdcpy( &s, "SSIE_CONF_FNAME=",  server_conf_fname, _ ) );
			}

		file_tmpname(&fname1);
		p = strpos( cmd, " ", 0 );
		file_save_str( fname1, &(cmd[p+1]), -1 );
		strdcat( &fname, " @", fname1, _ );
		System( &s, fname );
		print( s, _ );
		unlink( fname1 );
		return OK;
		}

	if ( !stricmp( what, "version" ) )
		{ print( product_name, BR, _ );	return OK; }
		
	if ( !stricmp( what, "log" ) )			return cmd_log( t_work );

	if ( !stricmp( what, "request" ) )		return cmd_request( t_work );

	if ( !stricmp( what, "part" ) )			return cmd_part( t_work );

	if ( !stricmp( what, "status" ) )		return cmd_status( t_work );

	if ( !stricmp( what, "start" ) )		return cmd_start( t_work );

	if ( !stricmp( what, "stop" ) )			return cmd_stop( t_work );

	if ( !stricmp( what, "sq" ) )			{ cmd_stop( t_work ); exit(0); }
	
	if ( !stricmp( what, "server" ) )		return cmd_server( t_work );

	if ( !stricmp( what, "print" ) )		return cmd_var_print0( t_work );

	if ( !stricmp( what, "setvar" ) )		return cmd_var_set( t_work );

/*
	if ( !stricmp( what, "initialize" ) )	return cmd_initialize( t_work );

	if ( !stricmp( what, "ssie7c_part" ) )	return cmd_ssie7c_part( t_work );

	if ( !stricmp( what, "gw_inst" ) )		return cmd_gw_inst( t_work );

	if ( !stricmp( what, "gw_sync" ) )		return cmd_gw_sync( t_work );
*/

	return cmd_system( t_work );
	}







	
	















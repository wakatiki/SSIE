#define MAIN
#include "elib4.c"
#define MFT
#include "ssie7lib.c"

//------------------------------------------------------------------------------
// PARAMETRAGE DU MODULE SSIE7NAT

#define MODULE_NAME			"ssie7c"
#define PRODUCT_NAME		"SSIE Robot de test"
#define DEFAULT_PORT		"1770"



#ifdef OS_WINDOWS
	#define APPLI_DIR		"c:\\e\\apps"
	#define BIN_DIR			"c:\\e\\bin\\windows"
	#define OPENSSL			"c:\\e\\bin\\windows\\ssie7sec.exe"
#endif

#ifdef OS_AIX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/ssie7"
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

char *ssie_dirlog = null;
char *ssie_dirbin = null;
char *ssie_dirconf = null;
char *ssie_dirdat = null;
char *ssie_dirwrk = null;


#include "ssie7lib_cmd.c"
#include "ssie7lib_cmd_connect.c"
#include "ssie7lib_cmd_exploit.c"
#include "ssie7lib_cmd_request.c"
//#include "ssie7lib_cmd_server.c"
#include "ssie7lib_run.c"
#include "ssie7lib_robot.c"
#include "ssie7lib_vdisk.c"
#include "ssie7lib_vdisk_client.c"
#include "ssie7lib_vdisk_server.c"

#include "ssie7tbot_cmd_test.c"


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
	strdcpy( &dirconf	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "conf", _ );
//	strdcpy( &dirdat	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "data", _ );
	strdcpy( &dirdat	, "c:/e/src/ssie7/tbot/data", _ );
	strdcpy( &dirtmp	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "tmp", _ );
	strdcpy( &dirwrk	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "data" DIR_CHAR "wrk", _ );
	strdcpy( &dirlog	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "log", _ );


	strdcpy( &ssie_dirbin	, BIN_DIR, _ );
	strdcpy( &ssie_dirconf	, APPLI_DIR	DIR_CHAR, "ssie7", DIR_CHAR "conf", _ );
	strdcpy( &ssie_dirdat	, APPLI_DIR	DIR_CHAR, "ssie7", DIR_CHAR "data", _ );
//	strdcpy( &dirtmp	, APPLI_DIR	DIR_CHAR, module_name, DIR_CHAR "tmp", _ );
	strdcpy( &ssie_dirwrk	, APPLI_DIR	DIR_CHAR, "ssie7", DIR_CHAR "data" DIR_CHAR "wrk", _ );
	strdcpy( &ssie_dirlog	, APPLI_DIR	DIR_CHAR, "ssie7", DIR_CHAR "log", _ );
	
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


// RECUPERE LES PROCEDURES EXISTANTES :
/*	
tf = null;
strdcpy( &fname, dirdat, DIR_CHAR "articles" DIR_CHAR "*.dat", _ );
while( tf = file_list( tf, fname, "absdir" ) )
	{
	strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
	var_free( h );
	h = var_load( fname );
	v = var_get( h, "name" );
	strdcpy( &fname1, fname, ".name.txt", _ );
	file_save_str( fname1, v, -1 );
	v = var_get( h, "descr" );
	strdcpy( &fname1, fname, ".desrc.txt", _ );
	file_save_str( fname1, v, -1 );
//	v = var_get( h, "script" );
//	strdcpy( &fname1, fname, ".script.txt", _ );
//	file_save_str( fname1, v, -1 );
	}
*/
	}


	
//------------------------------------------------------------------------------
int ssie7_cmd( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	if (( stricmp( what, "log" ) )&&( stricmp( what, "connect" ) ))
		{
		logw( cmd, _ );
/*
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
*/
		}
	
//	if ( !stricmp( what, "connect" ) )		return cmd_connect( t_work );

//	if ( !stricmp( what, "vdisk_server" ) )	return cmd_vdisk_server( t_work );

//	if ( !stricmp( what, "vdisk" ) )		return cmd_vdisk_client( t_work );


	if ( !stricmp( what, "version" ) )
		{ print( product_name, BR, _ );	return OK; }
		
	if ( !stricmp( what, "tests_log" ) )	return tbot_tests_log( t_work );

	if ( !stricmp( what, "tests_set_status" ) )	return tbot_tests_set_status( t_work );

	if ( !stricmp( what, "tests_load" ) )	return tbot_tests_load( t_work );

	if ( !stricmp( what, "tests_list" ) )	return tbot_tests_list( t_work );

	if ( !stricmp( what, "proc_list" ) )	return tbot_proc_list( t_work );

	if ( !stricmp( what, "proc_load" ) )	return tbot_proc_load( t_work );

	if ( !stricmp( what, "proc_save" ) )	return tbot_proc_save( t_work );

	if ( !stricmp( what, "proc_delete" ) )	return tbot_proc_delete( t_work );

	if ( !stricmp( what, "func_load" ) )	return tbot_func_load( t_work );

	if ( !stricmp( what, "proc_func_load" ) )	return tbot_proc_func_load( t_work );

	if ( !stricmp( what, "arti_list" ) )	return tbot_arti_list( t_work );

	if ( !stricmp( what, "arti_load" ) )	return tbot_arti_load( t_work );

	if ( !stricmp( what, "arti_save" ) )	return tbot_arti_save( t_work );

	if ( !stricmp( what, "arti_delete" ) )	return tbot_arti_delete( t_work );

	if ( !stricmp( what, "wdoc_list" ) )	return tbot_wdoc_list( t_work );

	if ( !stricmp( what, "wdoc_load" ) )	return tbot_wdoc_load( t_work );

	if ( !stricmp( what, "wdoc_save" ) )	return tbot_wdoc_save( t_work );

	if ( !stricmp( what, "wdoc_delete" ) )	return tbot_wdoc_delete( t_work );

	if ( !stricmp( what, "purge_log" ) )	return tbot_purge_log( t_work );

	if ( !stricmp( what, "purge_cat" ) )	return tbot_purge_cat( t_work );

	if ( !stricmp( what, "log" ) )			return cmd_log( t_work );

	if ( !stricmp( what, "request" ) )		return cmd_request( t_work );

	if ( !stricmp( what, "print" ) )		return cmd_var_print0( t_work );

	if ( !stricmp( what, "setvar" ) )		return cmd_var_set( t_work );


	return cmd_system( t_work );
	}







	
	















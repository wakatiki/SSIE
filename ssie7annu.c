#define MAIN
#include "ssie7annu.h"

// 10.122.0.81:1765(NOEY758);10.200.205.74:1765(PCYYANNU);10.122.47.38:1765(NOEY4L9)
// main , ssie7_init , server_run , cmd_srvannu_list

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

/*
file_load( &s1, "c:/e/etoa" );
file_load( &s2, "c:/e/etoa_271" );
for( i = 0 ; i < file_size( "c:/e/etoa_271" ) ; i++ )
	{
//	if ( s1[i] != s2[i] )
//if ( s1[i] == 0 )
		print( itoa(s1[i]), " : ", itoa(s2[i]), BR, _ );
	if (( i % 40 ) == 0) pause();
	}
wexit(0);
*/


	module_name		= MODULE_NAME;
	product_name	= PRODUCT_NAME;
	default_port	= DEFAULT_PORT;
	
	dirbin		= BIN_DIR;
//	dirappli	= APPLI_DIR	DIR_CHAR MODULE_NAME;
	dirconf		= APPLI_DIR	DIR_CHAR MODULE_NAME DIR_CHAR "conf";
	dirdat		= APPLI_DIR	DIR_CHAR MODULE_NAME DIR_CHAR "data";
	dirtmp		= APPLI_DIR	DIR_CHAR MODULE_NAME DIR_CHAR "data" DIR_CHAR "tmp";
	dirwrk		= APPLI_DIR	DIR_CHAR MODULE_NAME DIR_CHAR "data" DIR_CHAR "wrk";
	dirpart		= APPLI_DIR	DIR_CHAR MODULE_NAME DIR_CHAR "data" DIR_CHAR "part";
	dirlog		= APPLI_DIR	DIR_CHAR MODULE_NAME DIR_CHAR "log";
	
	// INITIALISATION D'OPENSSL
	
	if ( ssl_pgm( OPENSSL ) == null )
		{
		print( "ERREUR : openssl n'est pas installé ou est inaccessible." BR, __ );
		wexit(1);
		}


	server_conf_load( );
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
	if (( stricmp( what, "log" ) )&&( !var_get( hReq, "no_log" ) ))
		{
		if ( request_mode == MODE_TCP )			logw( ip_cli, " : ", cmd, _ );
		else
		if ( request_mode == MODE_CONSOLE )		logw( "CONSOLE : ", cmd, _ );
		else
		if ( request_mode == MODE_ROBOT )		logw( "ROBOT : ", cmd, _ );
		else
												logw( "CMD : ", cmd, _ );
		}
	

//	if ( !stricmp( what, "connect" ) )		return cmd_connect( t_work );

	if ( !stricmp( what, "part" ) )			return cmd_part( t_work );

	if ( !stricmp( what, "maj_part_ssie" ) )return cmd_maj_part_ssie( t_work );

	if ( !stricmp( what, "maj_part_ons" ) )	return cmd_maj_part_ons( t_work );

	if ( !stricmp( what, "login" ) )		return cmd_login( t_work );

	if ( !stricmp( what, "get_ssl_certif" ) )
											return cmd_get_ssl_certif( t_work );

	if ( !stricmp( what, "srvannu_list" ) )	return cmd_srvannu_list( t_work );

	if ( !stricmp( what, "mftkey" ) )		return cmd_mftkey( t_work );

	if ( !stricmp( what, "srvnat_load" ) )	return cmd_srvnat_load( t_work );


//	if ( !stricmp( what, "log_agrega" ) )	return cmd_log_agregate( t_work );

	if ( !stricmp( what, "log" ) )			return cmd_log( t_work );

//	if ( !stricmp( what, "request" ) )		return cmd_request( t_work );

	if ( !stricmp( what, "status" ) )		return cmd_status( t_work );

	if ( !stricmp( what, "start" ) )		return cmd_start( t_work );

	if ( !stricmp( what, "do_start" ) )		return cmd_start( t_work );

	if ( !stricmp( what, "stop" ) )			return cmd_stop( t_work );

	if ( !stricmp( what, "do_stop" ) )		return cmd_stop( t_work );

	if ( !stricmp( what, "server" ) )		return cmd_server( t_work );

/*
	if ((!stricmp( what, "srvannu" ) )||( !stricmp( what, "srvnat" ) ))
											return cmd_srvannu( t_work );
*/

	if ( !stricmp( what, "print" ) )		return cmd_var_print( t_work );

	print( "ERREUR : Commande inconnue." BR, _ );
	}





// GESTION DE LA LISTE DES SERVEURS D'ANNUAIRE

//------------------------------------------------------------------------------
int cmd_srvannu_list( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &vn );
	if ( ( !v )||( ! (vn[0]) ) )
		{
		file_load( &s, "/appli/ssie7annu/conf/srvannu_list.cfg" );
		print( s, BR, _ );
		return OK;
		}
	
	file_save_str( "/appli/ssie7annu/conf/srvannu_list.cfg", vn, -1 );
	
	if ( var_get( hReq, "no_replique" ) ) return OK;
	
	strdcpy( &cmd, "srvannu_list '", vn, "' no_replique", _ );
	ts = str_explode( vn, "," );
	for( i = 0 ; ts[i] ; i++ )
		{
		if ( ! (ts[i][0]) ) continue;
		if ( ! strncmp( ts[i], tcp_myIP(0), -1 ) ) continue;
		tcp_request( ts[i], cmd, &s );
//print( ts[i], " => ", cmd, BR, s, BR, _ );
		}
	
	print( "OK", _ );
	return OK;
	}
	
	















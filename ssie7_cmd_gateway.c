#include "ssie7.h"


//------------------------------------------------------------------------------
int cmd_ssie7_gateway( T_WORK *t_work )

// DEMARRAGE DE LA PASSERELLE SYNCHRONE
//------------------------------------------------------------------------------
	{
	if ( !var_getn( hReq, 1, &v1 ) )
		{
		for( i = 0 ; v = var_getn( hSrvConf, i, &vn ) ; i++ )
			{
			if ( strnicmp( vn, "gateway_", 8 ) ) continue;
			print( vn, " : ", v, BR, _ );
			}
		
		print( "OK" BR, _ );
		return OK;
		}
		
	if ( !stricmp( v1, "set" ) )
		{
		if ( !var_getn( hReq, 2, &v2 ) )
			{
			print( "ERREUR : usage : gateway set {n° port} {ip_list} {ip|dns:port} [max_cnx]" BR, _ );
			return ERROR;
			}
		if ( !var_getn( hReq, 3, &v3 ) )
			{
			print( "ERREUR : usage : gateway set {n° port} {ip_list} {ip|dns:port} [max_cnx]" BR, _ );
			return ERROR;
			}
		if ( !var_getn( hReq, 4, &v4 ) )
			{
			print( "ERREUR : usage : gateway set {n° port} {ip_list} {ip|dns:port} [max_cnx]" BR, _ );
			return ERROR;
			}
		
		v5 = null;
		var_getn( hReq, 5, &v5 );
		
		
		strdcpy( &s, "gateway_", v2, _ );
		strdcpy( &s1, "'", v3, "' ", v4, " ", v5, _ );
		
		var_set( hSrvConf, s, s1 );
		server_conf_save( );
		print( "OK" BR, _ );
		return OK;		
		}
		
	if ( !stricmp( v1, "delete" ) )
		{
		if ( !var_getn( hReq, 2, &v2 ) )
			{
			print( "ERREUR : usage : gateway delete {n° port}" BR, _ );
			return ERROR;
			}
		strdcpy( &s, "gateway_", v2, _ );
		
		var_delete( hSrvConf, s );
		server_conf_save( );
		print( "OK" BR, _ );
		return OK;
		}
	
	
	if ( !stricmp( v1, "run" ) )
		{
		if ( !var_getn( hReq, 2, &v2 ) )
			{
			print( "ERREUR : usage : gateway run {n° port} {ip_list} {ip|dns:port} [max_cnx]" BR, _ );
			return ERROR;
			}
		if ( !var_getn( hReq, 3, &v3 ) )
			{
			print( "ERREUR : usage : gateway run {n° port} {ip_list} {ip|dns:port} [max_cnx]" BR, _ );
			return ERROR;
			}
		if ( !var_getn( hReq, 4, &v4 ) )
			{
			print( "ERREUR : usage : gateway run {n° port} {ip_list} {ip|dns:port} [max_cnx]" BR, _ );
			return ERROR;
			}

		v5 = null;
		var_getn( hReq, 5, &v5 );
			
		gateway_run( t_work, v2, v3, v4, v5 );
		}
	}




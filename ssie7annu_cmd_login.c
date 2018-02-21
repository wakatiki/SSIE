#include "ssie7lib.h"


// GESTION DES LOGIN

#define login	(t_work->u1)

//------------------------------------------------------------------------------
int cmd_login( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	logw( ip_cli, " : ", cmd, _ );
	
	v = var_getn( hReq, 1, &login );
	if ( !v ) goto syntax_error;

	v = var_getn( hReq, 2, &vn );
	if ( !v )	goto syntax_error;

	if ( h ) var_free( h );
	h = var_load( strdcpy( &fname, dirdat, DIR_CHAR "login" DIR_CHAR, upper(login), ".dat", _ ) );

	//--------------------------------------------------------------------------
	// RETOURNE LA LISTE DES SERVEURS D'UN LOGIN
	//--------------------------------------------------------------------------
	
	if ( !strcmp( vn, "get" ) )
		{
		if ( ! var_get( hReq, "of" ) )
			{
			// AFFICHAGE FACON MERSI3 TEMPORAIREMENT
			v = var_get( h, "srv_liste" );
			if ( !v ) return OK;
			
			ts = str_explode( &(v[1]), "," );
			b = false;
			for( i = 0 ; ( ts[i] )&&( ts[i][0] ) ; i++ )
				{
				var_free( h1 );
				h1 = var_load( strdcpy( &fname, dirpart, DIR_CHAR, upper(ts[i]), ".dat", _ ) );
				
				v1 = var_get( h1, "ip_ssie" );
				if ( ( !v1 )||( !(v1[0]) ) )
					{
					v1 = var_get( h1, "ip" );
					if ( ( !v1 )||( !(v1[0]) ) )
						v1 = var_get( h1, "part_ip" );
					}
				
				b1 = false;
				v2 = var_get( h1, "port" );
				if ( !v2 )
					{
					b1 = true;
					v2 = var_get( h1, "part_port_mersi" );
					if ( !v2 )
						v2 = var_get( h1, "port_mersi" );
					}
					
/*
				v1 = var_get( hConf1, "part_ip" );
				v2 = var_get( hConf1, "part_port_mersi" );
*/
				if ( !v2 ) v2 = "1765";
				
				if ( v = var_get( h1, "part_descr" ) )
					strdcpy( &s2, quote(v), _ );
				else
				    {
       				if ( v = var_get( h1, "descr" ) )
					   strdcpy( &s2, quote(v), _ );
                    else
					   strdcpy( &s2, "''", _ );
                    }
					
				if ( ( !v1 )||( !v2 ) ) continue;
				print( (char*)((b)?",":""), "[", quote( ts[i] ), ",",
					quote(v1), ",", v2, ",", s2, "]", _ );
				b = true;
				}
				
			return OK;			
			}
		
		
			
		v = var_get( h, "srv_liste" );
		if ( !v )
			{
			print( "[]", _ );
			return OK;
			}
		
		ts = str_explode( &(v[1]), "," );
		print( "[", _ );
		b = false;
		for( i = 0 ; ( ts[i] )&&( ts[i][0] ) ; i++ )
			{
			if ( h1 ) var_free( h1 );
			h1 = var_load( strdcpy( &fname, dirpart, DIR_CHAR, upper(ts[i]), ".dat", _ ) );

			v1 = var_get( h1, "ip_ssie" );
			if ( ( !v1 )||( !(v1[0]) ) )
				{
				v1 = var_get( h1, "ip" );
				if ( ( !v1 )||( !(v1[0]) ) )
					v1 = var_get( h1, "part_ip" );
				}
			
			if ( !v1 ) continue;
			
			b1 = false;
			v2 = var_get( h1, "port" );
			if ( !v2 )
				{
				b1 = true;
				v2 = var_get( h1, "part_port_mersi" );
				if ( !v2 )
					v2 = var_get( h1, "port_mersi" );
				if ( !v2 )
					v2 = "1765";
				}
				
			v3 = var_get( h1, "descr" );
			if ( !v3 ) 	v3 = var_get( h1, "part_descr" );
			
			print( (char*)((b)?",":""), "[", str_quote( ts[i] ), ",'",
				v1, ":", v2, "',", str_quote(v3),
				",", (char*)((b1)? "'mersi3'" : "'ssie7'" ),
				"]", _ );
			b = true;
			}
		print( "]", _ );
		return OK;
		}
		

	//--------------------------------------------------------------------------
	// AJOUTE UN SERVEUR A UN LOGIN
	//--------------------------------------------------------------------------
	
	if ( !stricmp( vn, "set" ) )
		{
		v = var_getn( hReq, 3, &v2 );
		if ( !v ) return OK;

		v = var_get( h, "srv_liste" );
		if ( v )
			{
			if ( stripos( v, strdcpy( &s, ",", v2, ",", _ ), 0 ) >= 0 )
				{
				print( "OK", _ );
				return OK;
				}
			if ( v[0] )
				var_set( h, "srv_liste", strdcpy( &s, v, v2, ",", _ ) );
			else
				var_set( h, "srv_liste", strdcpy( &s, v, ",", v2, ",", _ ) );
			}
		else
			var_set( h, "srv_liste", strdcpy( &s, ",", v2, ",", _ ) );
			
		var_save( h, fname );
		#ifdef OS_UNIX
		system( strdcpy( &s, "chmod 744 ", fname, _ ) );
		#endif
		
		print( "OK", _ );

		// REPLIQUE SUR LES AUTRES SERVEURS
		
		if ( var_get( hReq, "no_replique" ) ) return OK;
		
		file_load( &s, "/appli/ssie7annu/conf/srvannu_list.cfg" );
		strdcat( &cmd, " no_replique", _ );
		
		ts = str_explode( s, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			if ( ! strncmp( ts[i], tcp_myIP(0), -1 ) ) continue;
			tcp_request( ts[i], cmd, &s );
//print( ts[i], " => ", cmd, BR, s, BR, _ );
			}
		
		return OK;
		}


	//--------------------------------------------------------------------------
	// ENLEVE L'AUTORISATION D'UN SERVEUR A UN LOGIN
	//--------------------------------------------------------------------------
	
	if ( !stricmp( vn, "delete" ) )
		{
		v = var_getn( hReq, 3, &v2 );
		v = var_get( h, "srv_liste" );
		if ( v )
			{
			v = str_replace( v, strdcpy( &s, ",", v2, ",", _ ), "," );
			var_set( h, "srv_liste", v );
			var_save( h, fname );
			}
		print( "OK", _ );
		return OK;
		}
	

	//--------------------------------------------------------------------------
	// MAUVAISE SYNTAXE
	//--------------------------------------------------------------------------

	if ( v )
		{
		syntax_error:
		print( "ERREUR : syntaxe : login NNI [get|set|delete]" BR, _ );
		return ERROR;
		}


	//--------------------------------------------------------------------------
		
	return OK;
	}





















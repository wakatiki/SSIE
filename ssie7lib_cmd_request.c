#include "ssie7lib.h"


//------------------------------------------------------------------------------
int cmd_request( T_WORK *t_work )

// COMMANDE REQUEST
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &v1 );
	if ( ! v ) return ERROR;
	var_getn( hReq, 2, &vn );
	unquote(vn);
	strdcpy( &cmd1, vn, _ );
	
/*
	strdcpy( &s, " user=", var_get( hSrvConf, "partname" ), _ );
	strd_replace( &cmd1, s, "" );
	strdcat( &cmd1, " user=", var_get( hSrvConf, "partname" ), _ );
*/
//print( "debug request 00 : ", cmd1, BR, _ );

	if ( request_mode == MODE_TCP )
		{
		if ( session_partname )
			{
			// Transmet les droits admin si autorisé
//			strdcpy( &cmd1, vn, " user=", _ );
			if ( ! is_authorized( "is_admin", null ) )
				strdcat( &cmd1, " user=", session_partname, _ );
			else
				{
				v = var_get( hSrvConf, "partname" );
				strdcat( &cmd1, " user=", v, _ );
				}
			}
		else
		if ( strcmp( ip_cli, "127.0.0.1" ) )
			{
			print( "ERREUR : Vous devez être connecté", _ );
			return ERROR;
			}
		else
			strdcpy( &cmd1, vn, " user=", var_get( hSrvConf, "partname" ), _ );
		
		strdcpy( &cmd, "request ", v1, " ", quote(cmd1), " ip_cli=", ip_cli, _ );

//print( "debug request 0 : ", cmd, BR, _ );
				
		file_tmpname( &fname );
		file_save_str( fname, cmd, -1 );
		strdcpy( &cmd1, argv[0], " @", fname, _ );
		System( &s, cmd1 );
		print( s, _ );
		return OK;
		}
	
//print( "debug1", BR, __ );
	// Mode ligne de commande ou console
	
	b4 = ( ( p5 = strpos( v1, "/", 0 ) ) >= 0 ); // Via un intermédiaire ?
	if ( b4 )
		{
		v1[p5] = 0;
		strdcpy( &cmd1, "request ", &(v1[p5+1]), " ", quote(cmd1), _ );
		}
	
	
	// Destinataire = Nom de partenaire
//print( "debug2", BR, __ );
	
	var_free( hPart ); hPart = 0;
	partname = v1;
	if (( ! is_ip_valide( partname ) )&&( part_exists( t_work ) ))
		{
//print( "debug3", BR, __ );

		hPart = var_load( fname );

//print( "debug3a", BR, __ );
		
		v = var_get( hPart, "product_name" );

//print( "debug3b", BR, __ );

		if ( ( v )&&( !strnicmp( v, "SSIE v7", 7 ) ) )
			{
//			strdcpy( &cmd1, vn, _ );
			
			// Request mode connecté (request PARTSSIE7 ...)
	
//print( "debug request 1 : ", partname, " : ", cmd1, BR, _ );
				
			if ( ssie7connect( t_work, false ) == ERROR )
				{
				// PROBLEME DE CONNEXION
				print( "ERREUR : Problème de connexion à ", partname, BR, error_msg, _ );
				return ERROR;
				}
	
	
			if ( ! tcp_crequest( hCnx, cmd1, &s ) )
				print( "ERREUR : ", error_msg, _ );
			else
				print( s, _ );
			
			return OK;
			}

//print( "debug request 2 : ", partname, " : ", cmd1, BR, _ );

		v = var_get( hPart, "dns" );
		if (( !v )||( !(v[0]) )) v = var_get( hPart, "ip" );
		v1 = strdcpy( &s1, v, ":", var_get( hPart, "port_mersi" ), _ );

		if ( !stricmp( var_get( hSrvConf, "partname" ), "MFTCCWEX" ) )
			{
			if ( stripos( cmd1, " user=MFTCCWEX", 0 ) > 0 )
				strdcat( &cmd1, " user=" NNI_ADM, _ );
			}
/*
		else
		if ( ! var_get( hReq, "ip_cli" ) )
			strdcat( &cmd1, " user=", var_get( hSrvConf, "partname" ), _ );
*/
//print( "debug request 2 : ", v1, " : ",cmd1, BR, _ );
		}
/*	else
	if ( ! var_get( hReq, "ip_cli" ) )
		strdcat( &cmd1, " user=", var_get( hSrvConf, "partname" ), _ );
*/
	


	
	// Destinataire = IP:Port
	
//print( "debug request 2 : ", v1, " : ", cmd1, BR, _ );
		
	if ( ! tcp_request( v1, cmd1, &s ) )
		print( "ERREUR : ", error_msg, _ );
	else
		print( s, _ );

	return OK;
	}



/*
//------------------------------------------------------------------------------
int cmd_request_log( T_WORK *t_work )

// COMMANDE REQUEST AND LOG
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &v1 );
	if ( ! v ) return ERROR;
	strdcpy( &partname, v1, _ );
	if ( part_exists( t_work ) )
		{
		hPart = var_load( fname );
		strdcpy( &partname, var_get( hPart, "ip_ssie" ), ":", var_get( hPart, "port" ), _ );
		}
	
	v = var_getn( hReq, 2, &v2 );
	v1 = var_getn( hReq, 3, &v3 );
	if (( ! v )||( !v1 ))
		{
		print( "ERREUR : syntaxe : request&log {ip:por|partname} \"{cmd}\" {base_name} folder=..., type=..., class=...", _ );
		return ERROR;
		}

	if ( ( p = strpos( partname, "/", 0 ) ) < 0 )
		{
		v = tcp_request( partname, v2, &s );

		if ( ! v )
			strdcpy( &w, "ERREUR : ", error_msg, _ );
		else
			strdcpy( &w, v, _ );
		}
	else
		{
		v1[p] = 0;
		strdcpy( &s, "request ", &(partname[p+1]), " ", quote( v2 ), _ );
		logw( "request : ", s, _ );
		v = tcp_request( partname, s, &s );
		if ( ! v )
			strdcpy( &w, "ERREUR : ", error_msg, _ );
		else
			strdcpy( &w, v, _ );
		}

	// Création d'un document bdoc
	
	strdcpy( &cmd, "bdoc ", v3, " docsave ",
		" folder=", var_get( hReq, "folder" ),
		", type=", var_get( hReq, "folder" ),
		", class=", var_get( hReq, "class" ),
		", title=", quote( v2 ),
		", body=", quote( w ), _ );
		
	var_free( hReq );
	hReq = var_from_cmd( 0, cmd );
	cmd_bdoc( t_work );
	
	print( w, _ );

	return OK;
	}



//------------------------------------------------------------------------------
int cmd_lrequest_log( T_WORK *t_work )

// COMMANDE REQUEST AND LOG
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &v2 );
	v1 = var_getn( hReq, 2, &v3 );
	if (( ! v )||( !v1 ))
		{
		print( "ERREUR : syntaxe : lrequest&log \"{cmd}\" {base_name} folder=..., type=..., class=...", _ );
		return ERROR;
		}
	
	if ( !strncmp( v2, "ssie7 ", 6 ) )
		{
		file_tmpname( &fname );
		file_save_str( fname, &(v2[6]), -1 );
		strdcpy( &cmd1, "ssie7 @", fname, _ );
		System( &w, cmd1 );
		}
	else
		System( &w, v2 );
	
	// Création d'un document bdoc
	
	strdcpy( &cmd, "bdoc ", v3, " docsave ",
		" folder=", var_get( hReq, "folder" ),
		", type=", var_get( hReq, "folder" ),
		", class=", var_get( hReq, "class" ),
		", title=", quote( v2 ),
		", body=", quote( w ), _ );
		
	var_free( hReq );
	hReq = var_from_cmd( 0, cmd );
	cmd_bdoc( t_work );
	
	print( w, _ );

	return OK;
	}




//------------------------------------------------------------------------------
int cmd_ls( T_WORK *t_work )

// COMMANDE LS
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &vn );

	#ifdef OS_WINDOWS
	strdcpy( &cmd, "dir ", (char*)( ( v ) ? str_replace(vn, "/", "\\") : "" ), _ );
	#else
	strdcpy( &cmd, "ls -l ", (char*)( ( v ) ? vn : "" ), _ );
	#endif
	print( System( &s, cmd ), _ );
	}



//------------------------------------------------------------------------------
int cmd_rm( T_WORK *t_work )

// COMMANDE RM
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &vn );

	#ifdef OS_WINDOWS
	file_delete( str_replace(vn, "/", "\\") );
	#else
	file_delete( vn );
	#endif
	}



//------------------------------------------------------------------------------
int cmd_cp( T_WORK *t_work )

// COMMANDE CP
//------------------------------------------------------------------------------
	{
	v1 = null;
	var_getn( hReq, 1, &v1 );
	v2 = null;
	var_getn( hReq, 2, &v2 );

	#ifdef OS_WINDOWS
	file_copy( str_replace(v1, "/", "\\"), str_replace(v1, "/", "\\") );
	#else
	file_copy( v1, v2 );
	#endif
	}



//------------------------------------------------------------------------------
int cmd_mv( T_WORK *t_work )

// COMMANDE MV
//------------------------------------------------------------------------------
	{
	v1 = null;
	var_getn( hReq, 1, &v1 );
	v2 = null;
	var_getn( hReq, 2, &v2 );

	#ifdef OS_WINDOWS
	file_move( str_replace(v1, "/", "\\"), str_replace(v1, "/", "\\") );
	#else
	file_move( v1, v2 );
	#endif
	}




//------------------------------------------------------------------------------
int cmd_mkdir( T_WORK *t_work )

// COMMANDE MKDIR
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &vn );

	#ifdef OS_WINDOWS
	if ( v )	mkdir( str_replace(vn, "/", "\\") );
	#else
	if ( v )	mkdir( vn );
	#endif
	}



//------------------------------------------------------------------------------
int cmd_rmdir( T_WORK *t_work )

// COMMANDE RMDIR
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &vn );

	#ifdef OS_WINDOWS
	if ( v )	file_delete( str_replace(vn, "/", "\\") );
	#else
	if ( v )	file_delete( vn );
	#endif
	}



//------------------------------------------------------------------------------
int cmd_chown( T_WORK *t_work )

// COMMANDE CHOWN
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	return OK;
	#else
	system( cmd );
	#endif
	}



//------------------------------------------------------------------------------
int cmd_chmod( T_WORK *t_work )

// COMMANDE CHMOD
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	return OK;
	#else
	system( cmd );
	#endif
	}



//------------------------------------------------------------------------------
int cmd_cat( T_WORK *t_work )

// COMMANDE CAT
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &vn );

	if ( !v ) return ERROR;
	if ( !file_exists( vn ) )
		{
		print( "ERREUR : Fichier inexistant", _ );
		return ERROR;
		}
	file_load( &s, vn );
	print( s, BR, _ );
	
	return OK;
	}

*/















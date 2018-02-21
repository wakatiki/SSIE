
// ssie7connect, cmd_connect

//------------------------------------------------------------------------------				
int ssie7disconnect( T_WORK *t_work )
//------------------------------------------------------------------------------				
	{
	if ( session_cnx > 0 ) { tcp_disconnect( session_cnx ); session_cnx = ERROR; }
	if ( session_partname ) { free( session_partname ); session_partname = null; }
	if ( session_ipport ) { free( session_ipport ); session_ipport = null; }
	hCnx = hCnxOld;

	for( i = 0 ; i < 200 ; i++ )
		{
		strdcpy( &w, dirtmp, DIR_CHAR, partname, ".cnx", itoa( i ), _ );
		if ( !file_exists( w ) ) break;
		if ( s ) s[0] = 0;
		if ( ( file_load( &s, w ) == sERROR )||( atoi( s ) == getpid() ) )
			file_delete( w );
		}
	}




//------------------------------------------------------------------------------				
int 	cmd_connect( T_WORK *t_work )
//------------------------------------------------------------------------------				
	{
	// Syntaxe :
	// connect [protocole_version] [partname] [session_id ou auth_coded_string] [session_id value]
	
	// LECTURE DE LA VERSION
	v = var_getn( hReq, 1, &v1 );
	if ( !v )
		{
		print( "ERREUR : connexion refusée (0:syntaxe)" BR, _ );
		return ERROR;
		}
	// On garde la version la plus ancienne
	if ( atoi( v1 ) < atoi( PROTOCOLE_VERSION ) )
		strdcpy( &session_version, v1, _ );
	else
		strdcpy( &session_version, PROTOCOLE_VERSION, _ );
	
	
	// RECHERCHE LE PARTENAIRE INITIATEUR
	
	v = var_getn( hReq, 2, &partname ); // Récupère le nom de partenaire
	if ( !v )
		{
		print( "ERREUR : connexion refusée (1:syntaxe)" BR, _ );
		return ERROR;
		}
		
	strdcpy( &fname3, dirconf, DIR_CHAR "part" DIR_CHAR, partname, ".dat", _ );
	if ( !file_exists( fname3 ) )
		{
		b = false;
		v = var_get( hSrvConf, "part_auto_crea" );
		if ( !strcmp( v, "Oui" ) )
			{
			// RECHERCHE LE PARTENAIRE SUR LE SERVEUR D'ANNUAIRE
			strdcpy( &cmd, argv[0], " part ", partname, " loadnat", _ );
			System( &s, cmd );
			b = file_exists( fname3 );
			}
		if (  !b )
			{
			print( "ERREUR : connexion refusée (2:unknow part)", BR, _ );
			return ERROR;
			}
		}
		
	if ( hPart ) var_free( hPart );
	hPart = var_load( fname3 );
	strdcpy( &session_partname, partname, _ );
			
	
	// LECTURE MODE DE CONNEXION
	
	v = var_getn( hReq, 3, &v1 ); // Récupère la chaine d'authentification à décoder
	if ( !v )
		{
		print( "ERREUR : connexion refusée (4:syntaxe)" BR, _ );
		return ERROR;
		}
	
	
	// CONNEXION RAPIDE
	
	if ( !strcmp( v1, "session_id" ) )
		{
		v = var_getn( hReq, 4, &v1 ); 
		if ( !v )
			{
			print( "ERREUR : connexion refusée (5:syntaxe)" BR, _ );
			return ERROR;
			}
		
		v = var_get( hPart, "session_id" );
		if ( ( !v )||( strcmp( v, v1 ) ) )
			{
			print( "ERREUR : connexion refusée (6:syntaxe)" BR, _ );
			return ERROR;
			}
		
		
		v = var_get( hPart, "session_key" );
		if ( !v )
			{
			print( "ERREUR : connexion refusée (6a:syntaxe)" BR, _ );
			return ERROR;
			}
		
		strdcpy( &session_key, v, _ );
		
		goto connect_ok;
		}
	
	
	// AUTHENTIFICATION DU SERVEUR

	v2 = ssl_decrypt( &s1, hSrvConf, v1 );


	if ( v2 == null )
		{
		print( "ERREUR : connexion refusée (7:auth SSL)" BR, _ );
		
        // RECHERCHE LE PARTENAIRE SUR LE SERVEUR D'ANNUAIRE POUR RECHARGER LES CERTIFICATS SSL.
		strdcpy( &cmd, argv[0], " part ", partname, " loadnat", _ );
		System( &s, cmd );
		
		return ERROR;
		}
		
	
	// GENERE UN CODE SESSION ET DEMANDE L'AUTHENTIFICATION DU CLIENT
	
	v3 = srnd( 200 );
	v = ssl_encrypt( &s, hPart, v3 );
	if ( !v ) // Le retour n'est pas correct
		{
		print( "ERREUR : connexion refusée (8:auth SSL)" BR, _ );
		return ERROR;
		}
	
	strdcpy( &cmd, PROTOCOLE_VERSION " ", substr(v2,0,100), "<ssie7>", v, _ );

	if ( tcp_ssend( hCnx, cmd, -1 ) == ERROR )
		{
		return ERROR;
		}
		
	if ( tcp_srecv( hCnx, &s ) == ERROR )
		{
		return ERROR;
		}

	if ( strcmp( s, substr(v3,0,100) ) ) // Le retour n'est pas correct
		{
		print( "ERREUR : connexion refusée (9:auth SSL)" BR BR, s, BR BR, v3, _ );
		return ERROR;
		}
	
	

	// LA CONNEXION EST ACCEPTEE => ENREGISTRE LE SESSION_ID
	
	strdcpy( &s, substr(v2, 0, 100), substr(v3, 0, 100), _ );
	var_set( hPart, "session_id", s );
	strdcpy( &session_id, s, _ );

	strdcpy( &s, substr(v2, 100, -1), substr(v3, 100, -1), _ );
	var_set( hPart, "session_key", s );
	strdcpy( &session_key, s, _ );
	

	var_save( hPart, fname3 );
	
	//----------------------------------------------------------------------
	connect_ok :
	//----------------------------------------------------------------------

	session_cnx = hCnx;
		
	// LECTURE REPERTOIRE DE RECEPTION
	
	v = var_get( hPart, "files_dir" );
	strdcpy( &session_files_dir, v, _ );
	if ( ( !v )||( !(v[0]) ) )
		if ( ( session_files_dir[n=(strlen(session_files_dir)-1)] != '/' )
			&&( session_files_dir[n] != '\\' ) )
			strdcat( &session_files_dir, DIR_CHAR, _ );


	print( "OK:" PROTOCOLE_VERSION, __ );


	//----------------------------------------------------------------------
	// ATTEND ET TRAITE LES REQUETES
	//----------------------------------------------------------------------
	
	for( ; ; )
		{
		if ( ( hCnx == ERROR )||( !hCnx ) ) return ERROR;
		if ( ( n = tcp_srecv( hCnx, &cmd ) ) == ERROR )
			{
			free( ssie7out );
			ssie7out = null;
			hCnx = ERROR;
			return OK;
			}	
		cmd[n] = 0;
		if ( (trim(cmd))[0] == 0 ) continue;
		
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd );
		if ( !hReq )
			{
			tcp_ssend( hCnx, "ERREUR de syntaxe", -1 );
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
		
		
		// TRAITEMENT DE LA REQUETE
		
		if ( ssie7out ) ssie7out[0] = 0;
		else	
			strdcpy( &ssie7out, "", _ );
		

		ssie7_cmd( t_work );
		
		
		// ENVOI LA REPONSE
		
		tcp_ssend( hCnx, ssie7out, -1 );
		ssie7free( t_work );
		}
	}





















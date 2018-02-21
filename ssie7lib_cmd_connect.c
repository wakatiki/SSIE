#include "ssie7lib.h"

// ssie7connect, cmd_connect



//------------------------------------------------------------------------------				
int ssie7connect( T_WORK *t_work, char bNoAuth )

// CONNEXION A UN SERVEUR SSIE v7 DISTANT
//------------------------------------------------------------------------------				
	{
	static char *trc = null;
	long int fic_part_date=0;

	hCnxOld = hCnx;
		
#undef print	// On prend le print standard ELIB

	// VERIFIE SI ON ETAIT DEJA CONNECTE

tr( "ssie7connect_1" );

	if ( ( session_partname )&&( session_ipport )
		&& (
			( !partname )
			||( ! stricmp( session_partname, partname ) )
			)
		)
		{
		if ( session_cnx != ERROR )
			{ hCnx = session_cnx; return OK; } // La connexion est encore en cours => OK
			
		if ( hPart ) var_free( hPart );
		hPart = hNULL;

		// RECHARGE LA CONFIG DU PARTENAIRE
		
		strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, session_partname, ".dat", _ );
		if ( !file_exists( fname ) )
			{
			b = false;
			v = var_get( hSrvConf, "part_auto_crea" );
			if ( ( !v )||( !strcmp( v, "Oui" ) ) )
				{
				// RECHERCHE LE PARTENAIRE SUR LE SERVEUR D'ANNUAIRE
				strdcpy( &cmd, argv[0], " part ", session_partname, " loadnat", _ );
				System( &s, cmd );
				b = file_exists( fname );
				}
			if (  !b )
				{
				error_set( 1, "ERREUR : Le partenaire ", session_partname, " est inconnu (1)", _ );
				hCnx = hCnxOld;
				return ERROR;
				}

			}
		
		hPart = var_load( fname );
		
		goto connect;	// La connexion n'existe plus mais la session est OK => Reconnexion
		}
	

tr( "ssie7connect_2" );
	if ( !partname ) partname = session_partname;
	if ( !partname )
		{
		error_set( 1, "ERREUR : aucune connexion en cours", _ );
		hCnx = hCnxOld;
		return ERROR;
		}
//	upper( partname );	
	

	// TERMINE LA SESSION EN COURS
tr( "ssie7connect_3" );

	if ( session_partname )
		{
		if ( ( session_ipport )&&( strcmp( partname, session_partname ) ) )
			{
			free( session_ipport );
			session_ipport = null;	// Force la recherche de l'IP:PORT
			}
		free( session_partname );
		session_partname = null;	// Invalide la session en cours
		}
		

	if ( session_cnx > 0 )
		{
		tcp_disconnect( session_cnx );
		session_cnx = ERROR;
		}
	

tr( "ssie7connect_4" );
	
	// CHARGE LA CONFIG DU PARTENAIRE
	
	if ( hPart ) var_free( hPart );
	hPart = hNULL;
	
	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, partname, ".dat", _ );
	if ( !file_exists( fname ) )
		{
		b = false;
		v = var_get( hSrvConf, "part_auto_crea" );
		if ( ( !v )||( !strcmp( v, "Oui" ) ) )
			{
			// RECHERCHE LE PARTENAIRE SUR LE SERVEUR D'ANNUAIRE
			strdcpy( &cmd, argv[0], " part ", partname, " loadnat", _ );
			System( &s, cmd );
			b = file_exists( fname );
			}
		if ( !b )
			{
			error_set( 1, "ERREUR : Le partenaire ", partname, " est inconnu (2)", _ );
			hCnx = hCnxOld;
			return ERROR;
			}
		}

tr( "ssie7connect_5" );
	
	part_refresh( t_work );
	var_free( hPart );
	hPart = var_load( fname );
	fic_part_date = file_date(fname);
	
	
	// DETERMINE SON ADRESSE IP:PORT
	
	if ( !session_ipport )
		{
tr( "ssie7connect_6" );

		v = trim(var_get( hPart, "dns_ssie" ));
		if ( ( v )&&( v[0] ) )
			v = tcp_dns_address( v );

		if ( ( !v )||( !(v[0]) ) )
			v = var_get( hPart, "ip_ssie" );
           
		if ( ( !v )||( !(v[0]) )||( ! is_ip_valide( v ) ) )
			{
			error_set( 1, "ERREUR : L'adresse du partenaire ", partname, " n'est pas valide (", v, ")", _ );
			hCnx = hCnxOld;
			return ERROR;
			}
	
		v1 = var_get( hPart, "port" );
		if ( !v1 )
			{
			error_set( 1, "ERREUR : Le partenaire ", partname, " n'est pas correctement configuré", _ );
			hCnx = hCnxOld;
			return ERROR;
			}

		strdcpy( &session_ipport, v, ":", v1, _ );

		// VERIFIE SI UNE SESSION ENREGISTREE EXISTE POUR CE PARTENAIRE
	
		v = var_get( hPart, "session_id" );
		if ( v )
			strdcpy( &session_id, v, _ );
		}
	
	
	
	//--------------------------------------------------------------------------
	connect:
	//--------------------------------------------------------------------------

tr( "ssie7connect_7" );

	// LIMITE LE NOMBRE DE CONNEXION VERS UN MEME PARTENAIRE
/*	
	v = var_get( hPart, "max_tran" );
	if ( !v ) { if ( stricmp( partname, "MFTCCWEX" ) ) n = 3; else n = 10; } else n = atoi( v );
	if ( n <= 0 ) n = 3;
	

	for( i = 0 ; i < n ; i++ )
		{
		strdcpy( &w, dirtmp, DIR_CHAR, partname, ".cnx", itoa( i ), _ );
		if ( s ) s[0] = 0;
		if ( ( file_load( &s, w ) == sERROR )||( !process_is_alive( atoi( s ) ) ) )
			{
			file_save_str( w, itoa( getpid() ), -1 );
			break;
			}
		}
	if ( i >= n )
		{
		error_set( 1, "ALERTE : Pb connexion : trop de connexion pour ce partenaire : ", partname, " (", session_ipport, ")", _ );
		hCnx = hCnxOld;
		return ERROR;
		}
*/
	
	// CONNEXION
	
	hCnx = tcp_connect( session_ipport );
	if ( hCnx == ERROR )
		{
		error_set( 1, "Pb connexion à ", partname, " (", session_ipport, ")", _ );
		ssie7disconnect( t_work );
		if ( ( time(NULL) - fic_part_date ) > 3*60 )
			{
			strdcpy( &cmd, argv[0], " part ", partname, " loadnat", _ );
			System( &s1, cmd );
			}
		return ERROR;
		}
	
	if ( bNoAuth )
		{
		session_cnx = hCnx;
		return OK;
		}
	if ( session_id )		goto reconnect;
	
	
	//--------------------------------------------------------------------------
	new_session: // DEMARRAGE D'UNE NOUVELLE SESSION
	//--------------------------------------------------------------------------

tr( "ssie7connect_8" );

		
	// PREPARE DEMANDE AU DISTANT DE S'AUTHENTIFIER
	
	v2 = srnd( 200 );
	v = ssl_encrypt( &s, hPart, v2, "ssl" );
	if ( !v )
		{
		error_set( 1, "ERREUR : Le partenaire ", partname, " n'est pas correctement configuré", _ );
		ssie7disconnect( t_work );
		if ( ( time(NULL) - fic_part_date ) > 3*60 )
			{
			strdcpy( &cmd, argv[0], " part ", partname, " loadnat", _ );
			System( &s1, cmd );
//			logw("ERREUR : Authentification (certificat serveur non valide) : rechargement des propriétés du partenaire '", partname, "' (cmd='", cmd, "', retour='", s1, "').", _ );
			error_set( 1, "ERREUR : Authentification (certificat serveur non valide) de ", partname, " (", session_ipport, ") => ", cmd, " ==> ", s1, _ );
			}
		return ERROR;
		}


tr( "ssie7connect_9" );

	strdcpy( &cmd, "connect " PROTOCOLE_VERSION " ", var_get( hSrvConf, "partname" ), " ", quote( v ), _ );

	if ( tcp_ssend( hCnx, cmd, -1 ) == ERROR )
		{
		error_set( 1, "ERREUR : Pb connexion (1:cnx lost) à ", partname, " (", session_ipport, ")", _ );
		ssie7disconnect( t_work );
		return ERROR;
		}
		
	if ( tcp_srecv( hCnx, &s ) == ERROR )
		{
		error_set( 1, "ERREUR : Pb connexion (2:cnx lost) à ", partname, " (", session_ipport, ")", _ );
		ssie7disconnect( t_work );
		return ERROR;
		}

	if ( stripos( s, "ERREUR", 0 ) >= 0 )
		{
		error_set( 1, "ERREUR : Pb connexion (3:rejected) à ", partname, " (", session_ipport, ")" BR, s, _ );
		ssie7disconnect( t_work );
		return ERROR;
		}

	
	// VERIFIE L'AUTHENTIFICATION DU DISTANT
	
	p = strpos( s, "<ssie7>", 0 );
	if ( p < 0 )
		{
		error_set( 1, "ERREUR : Authentification (1) de ", partname, " (", session_ipport, ")" BR, s, _ );
		ssie7disconnect( t_work );
		return ERROR;
		}
		

	v1 = substr( s, 4, p-4 );
	if ( strncmp( v2, v1, 100 ) )
		{
		error_set( 1, "ERREUR : Authentification (certificat serveur non valide) de ", partname, " (", session_ipport, ")", _ );
		ssie7disconnect( t_work );
		if ( ( time(NULL) - fic_part_date ) > 3*60 )
			{
			strdcpy( &cmd, argv[0], " part ", partname, " loadnat", _ );
			System( &s1, cmd );
//			logw("ERREUR : Authentification (certificat serveur non valide) : rechargement des propriétés du partenaire '", partname, "' (cmd='", cmd, "', retour='", s1, "').", _ );
			error_set( 1, "ERREUR : Authentification (certificat serveur non valide) de ", partname, " (", session_ipport, ") => ", cmd, " ==> ", s1, _ );
			}
		return ERROR;
		}
	
	

	// ON GARDE LA VERSION DE PROTOCOLE LA PLUS ANCIENNE
	
	if ( atoi( v = substr( s, 0, 3 ) ) < atoi( PROTOCOLE_VERSION ) )
		strdcpy( &session_version, v, _ );
	else
		strdcpy( &session_version, PROTOCOLE_VERSION, _ );
	
	
	// RECUPERE LA DEMANDE D'AUTHENTIFICATION DE NOUS PAR LE DISTANT
	
	v = substr( s, p+7, -1 );
	v3 = ssl_decrypt( &s1, hSrvConf, v, "ssl" );
/*
	if ( v3 == null )
		{
		error_set( 1, "ERREUR : Authentification (3:rejected) de ", partname, " (", session_ipport, ")", _ );
		ssie7disconnect( t_work );
		return ERROR;
		}
*/
	if ( v3 == null )
		{
		for( i = 2 ; i < 5 ; i++ )
			{
			switch( i )
				{
				case 2 : vn = "ssl2"; break;
				case 3 : vn = "ssl3"; break;
				case 4 : vn = "ssl4"; break;
				}
			strdcpy( &w, vn, "_config_ok", _ );
			if ( !var_get( hSrvConf, w ) ) break;
			if ( v3 = ssl_decrypt( &s1, hSrvConf, v, vn ) ) break;
			}
		if ( !v3 )
			{	
			error_set( 1, "ERREUR : Authentification (3:rejected) de ", partname, " (", session_ipport, ")", _ );
			ssie7disconnect( t_work );
			return ERROR;
			}
		}

	

	if ( tcp_ssend( hCnx, v3, 100 ) == ERROR )
		{
		error_set( 1, "ERREUR : Pb connexion (4:lost) à ", partname, " (", session_ipport, ")", _ );
		ssie7disconnect( t_work );
		return ERROR;
		}

	// ATTEND QUE LE DISTANT NOUS AUTHENTIFIE OU PAS
	
	if ( tcp_srecv( hCnx, &s ) == ERROR )
		{
		error_set( 1, "ERREUR : Pb connexion (5:lost) à ", partname, " (", session_ipport, ")", _ );
		ssie7disconnect( t_work );
		return ERROR;
		}


	if ( strncmp( s, "OK:", 3 ) )
		{
		error_set( 1, "ERREUR : Authentification de (4) ", partname, " (", session_ipport, ")" BR, s, _ );
		ssie7disconnect( t_work );
		return ERROR;
		}
	
	

	// ENREGISTRE LA SESSION_ID DANS LE FICHIER PARTENAIRE
	
	strdcpy( &s, substr(v2, 0, 100), substr(v3, 0, 100), _ );
	var_set( hPart, "session_id", s );
	strdcpy( &session_id, s, _ );

	strdcpy( &s, substr(v2, 100, -1), substr(v3, 100, -1), _ );
	var_set( hPart, "session_key", s );
	strdcpy( &session_key, s, _ );

	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, partname, ".dat", _ );
	var_save( hPart, fname );
	
	strdcpy( &session_partname, partname, _ );

	session_cnx = hCnx;
	
	return OK;
	
	
	
	//--------------------------------------------------------------------------
	reconnect:
	//--------------------------------------------------------------------------


	// ENVOI LA REQUETE D'AUTHENTIFICATION RAPIDE
	
	strdcpy( &cmd, "connect " PROTOCOLE_VERSION " ", var_get( hSrvConf, "partname" ), " session_id ", quote( session_id ), _ );

	if ( tcp_ssend( hCnx, cmd, -1 ) == ERROR )
		{
		error_set( 1, "ERREUR : Pb connexion à ", partname, " (", session_ipport, ")", _ );
		ssie7disconnect( t_work );
		return ERROR;
		}
	
	
	// RECUPERE L'ACCORD OU LE REFUS
	
	if ( tcp_srecv( hCnx, &s ) == ERROR )
		{
		error_set( 1, "ERREUR : Pb connexion à ", partname, " (", session_ipport, ")", _ );
		ssie7disconnect( t_work );
		return ERROR;
		}



	if ( strncmp( s, "OK:", 3 ) )
		{
		tcp_disconnect( hCnx );
		free( session_id ); session_id = null;
		goto connect;	// REFUS => on ré-ouvre une session
		}
	
	
	// CONNEXION ACCEPTEE
	
	// On garde la version la plus ancienne
	if ( atoi( v = substr( s, 3, -1 ) ) < atoi( PROTOCOLE_VERSION ) )
		strdcpy( &session_version, v, _ );
	else
		strdcpy( &session_version, PROTOCOLE_VERSION, _ );

		
	strdcpy( &session_key, var_get( hPart, "session_key" ), _ );

	session_cnx = hCnx;

	if ( ! session_partname )
		strdcpy( &session_partname, partname, _ );

	
	return OK;
	}

#define print( msg, ... )		ssie7print( t_work, msg, __VA_ARGS__ )



//------------------------------------------------------------------------------				
int ssie7disconnect( T_WORK *t_work )
//------------------------------------------------------------------------------				
	{
	if ( session_cnx > 0 ) { tcp_disconnect( session_cnx ); session_cnx = ERROR; }
	if ( session_partname ) { free( session_partname ); session_partname = null; }
	if ( session_ipport ) { free( session_ipport ); session_ipport = null; }
	session_id = null;
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

	//--------------------------------------------------------------------------
	if ( request_mode != MODE_TCP )	// MODE CLIENT : CONSOLE OU LIGNE DE COMMANDE
	//--------------------------------------------------------------------------
		{
		v = var_getn( hReq, 1, &partname ); // Récupère le nom de partenaire
		if ( !v )
			{
			print( "ERREUR : syntaxe : connect [partname]" BR, _ );
			return ERROR;
			}
			
		if ( ssie7connect( t_work, false ) == ERROR )
			return ERROR;
		
		ssie7disconnect( t_work );
		
		print( "OK", _ );
		return OK;		
		}
	
	
	
	
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	
	else	// CONNECT MODE SERVEUR : TCP
	
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
		{
		// Syntaxe :
		// connect [protocole_version] [partname] [session_id ou auth_coded_string] [session_id value]

		// LECTURE DE LA VERSION
		v = var_getn( hReq, 1, &v1 );
		if ( !v )
			{
			print( "ERREUR : connexion refusée (0:syntaxe)" BR,
			cmd, BR,
			 _ );
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
				print( "ERREUR : connexion refusée (2:unknow part ", partname, ")", BR, _ );
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

		v2 = ssl_decrypt( &s1, hSrvConf, v1, "ssl" );
		if ( v2 == null )
			{
			for( i = 2 ; i < 5 ; i++ )
				{
				switch( i )
					{
					case 2 : vn = "ssl2"; break;
					case 3 : vn = "ssl3"; break;
					case 4 : vn = "ssl4"; break;
					}
				strdcpy( &w, vn, "_config_ok", _ );
				if ( !var_get( hSrvConf, w ) ) break;
				if ( v2 = ssl_decrypt( &s1, hSrvConf, v1, vn ) ) break;
				}
			if ( !v2 )
				{	
				print( "ERREUR : connexion refusée (7:auth SSL)" BR, _ );
				return ERROR;
				}
			}
			
		
		// GENERE UN CODE SESSION ET DEMANDE L'AUTHENTIFICATION DU CLIENT
		
		v3 = srnd( 200 );
		v = ssl_encrypt( &s, hPart, v3, "ssl" );
		if ( !v ) // Le retour n'est pas correct
			{
			print( "ERREUR : connexion refusée (8:auth SSL)" BR, _ );
			
            // RECHERCHE LE PARTENAIRE SUR LE SERVEUR D'ANNUAIRE POUR RECHARGER LES CERTIFICATS SSL.
			strdcpy( &cmd, argv[0], " part ", partname, " loadnat", _ );
			System( &s, cmd );
			
			return ERROR;
			}
		
		strdcpy( &cmd, PROTOCOLE_VERSION " ", substr(v2,0,100), "<ssie7>", v, _ );

		if ( tcp_ssend( hCnx, cmd, -1 ) == ERROR )
			return ERROR;
			
		if ( tcp_srecv( hCnx, &s ) == ERROR )
			return ERROR;

		if ( strncmp( s, v3, 100 ) ) // Le retour n'est pas correct
			{
			print( "ERREUR : connexion refusée (9:auth SSL)", _ );
			
            // RECHERCHE LE PARTENAIRE SUR LE SERVEUR D'ANNUAIRE POUR RECHARGER LES CERTIFICATS SSL.
			strdcpy( &cmd, argv[0], " part ", partname, " loadnat", _ );
			System( &s, cmd );
			
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


		print( "OK:" PROTOCOLE_VERSION, __ /*flush*/ );


		//----------------------------------------------------------------------
		// ATTEND ET TRAITE LES REQUETES
		//----------------------------------------------------------------------
		
		for( ; ; )
			{
			if ( ( hCnx == ERROR )||( !hCnx ) ) return ERROR;
			if ( ( n = tcp_srecv( hCnx, &cmd ) ) == ERROR )
				{
				if ( session_cnx > 0 ) { tcp_disconnect( session_cnx ); session_cnx = ERROR; }
				if ( session_partname ) { free( session_partname ); session_partname = null; }
				if ( session_ipport ) { free( session_ipport ); session_ipport = null; }
				if ( ssie7out ) free( ssie7out );
				ssie7out = null;
				hCnx = ERROR;
				ssie7free( t_work );
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

			if (( hCnx == 0 )||( hCnx == ERROR ))
				{
				ssie7free( t_work );
				return OK;
				}
			
			// ENVOI LA REPONSE
			
			tcp_ssend( hCnx, ssie7out, -1 );
			ssie7free( t_work );
			}
		}
	}





















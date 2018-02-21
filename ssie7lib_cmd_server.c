#include "ssie7lib.h"


// cmd_server


//------------------------------------------------------------------------------
int cmd_server( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	server_conf_reload( );

	if ( h ) { var_free( h ); h = 0; }
	

	//--------------------------------------------------------------------------
	// VALEURS PAR DEFAUT
	//--------------------------------------------------------------------------
	
	if ( ( ! (v=var_get( hSrvConf, "ip_ssie" )) )||( ! (trim(v)[0]) ) )
		{
		// INITIALISE L'ADRESSE IP
		
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "ip_ssie", tcp_myIP( 0 ) );
		}
	

	if ( ( ! (v=var_get( hSrvConf, "port" )) )||( ! (trim(v)[0]) ) )
		{
		// INITIALISE LE PORT
		
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "port", default_port );
		}
	

	if ( ( ! (v=var_get( hSrvConf, "maxcnx" )) )||( ! (trim(v)[0]) ) )
		{
		// INITIALISE LE MAXCNX
		
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "maxcnx", "10" );
		}
	

	//--------------------------------------------------------------------------
	// LECTURE DES PARAMETRES
	//--------------------------------------------------------------------------


	for( i = 1 ; v = var_getn( hReq, i, &vn ) ; i++ )
		{
		//----------------------------------------------------------------------
		if ( !stricmp( vn, "partname" ) )
		//----------------------------------------------------------------------
			{
			if ( ! is_alphanum( v, "_" ) )
				{
				print( "ERREUR : Le nom de partenaire doit être alphanumérique" BR, _ );
				return ERROR;
				}
				
			if ( strlen( v ) > 30 )
				{
				print( "ERREUR : Le nom de partenaire doit 30 caractères maximum" BR, _ );
				return ERROR;
				}
				
			// Vérifie que le partenaire n'est pas déjà pris par quelqu'un d'autre

			strdcpy( &cmd, "part get ", v, _ );
			if ( ( nat_request( &s, cmd ) )&&( stripos( s, "ERREUR", 0 ) < 0 ) )
				{
				if ( h1 ) var_free( h1 );
				h1 = var_from_str( 0, s );
				v1 = var_get( h1, "ip_ssie" );
				if ( strcmp( v1, var_get( hSrvConf, "ip_ssie" ) ) )
					{
					print( "ERREUR : Ce partenaire est déjà pris par un autre serveur", _ );
					return ERROR;
					}
				}
			
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "old_partname", var_get( h, "partname" ) );
			var_set( h, "partname", upper(v) );
			
			continue;
			}

		//----------------------------------------------------------------------
		if ( !stricmp( vn, "descr" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "descr", v );
			
			continue;
			}


		//------------------------------------------------------------------
		if ( ! strcmp( vn, "ip" ) )
		//------------------------------------------------------------------
			{
			if ( ( v )&&( !(v[0]) ) )
				v = tcp_myIP( 0 );
				
			if ( ! is_ip_valide( v ) )
				{
				print( "ERREUR : ip non valide", _ );
				return ERROR;
				}
				
			for( j = 0 ; v1 = tcp_myIP( j ) ; j++ )
				if ( !strcmp( v, v1 ) ) break;
			
			if ( !v1 )
				{
				print( "ERREUR : L'adresse IP fournie n'est pas valide" BR, _ );
				return ERROR;
				}
			
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "old_ip", var_get( h, "ip_ssie" ) );
			var_set( h, "ip_ssie", v );
			
			continue;
			}
		
		//------------------------------------------------------------------
		if ( ! strcmp( vn, "dns" ) )
		//------------------------------------------------------------------
			{
			if ( ! (v1 = tcp_dns_address( v )) )
				{
				print( "ERREUR : dns non valide", _ );
				return ERROR;
				}

			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "dns", v );
			
			continue;
			}
		
		//----------------------------------------------------------------------
		if ( !stricmp( vn, "port" ) )
		//----------------------------------------------------------------------
			{
			if ( ( ! is_numeric( v ) )
				||( (p = atoi( v )) < 10 )||( p > 65535 ) )
				{
				print( "ERREUR : Le port n'est pas valide" BR, _ );
				var_free( hSrvConf );
				hSrvConf = 0;
				server_conf_load( );
				return ERROR;
				}

			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "port", v );
			
			continue;
			}


		//----------------------------------------------------------------------
		if ( !stricmp( vn, "maxcnx" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "maxcnx", v );
			
			continue;
			}


		//----------------------------------------------------------------------
		if ( !stricmp( vn, "adm_email" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "adm_email", v );
			
			continue;
			}


		//----------------------------------------------------------------------
		if (( !stricmp( vn, "ssl" ) )||( !stricmp( vn, "ssl_config" ) ))
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			strdcpy( &s1, "", _ );

			//------------------------------------------------------------------
			// DEMANDE LA GENERATION D'UNE NOUVELLE CLEF PRIVE ET LA GENERATION D'UN CSR
			// La conf SSL actuelle reste dans le même état
			//------------------------------------------------------------------

			if ( !stricmp( v, "gencsr" ) )
				{
				
				strdcpy( &fname, dirconf, DIR_CHAR, module_name, ".new", _ );	// Contient la nouvelle clef privée
				strdcpy( &fname1, dirconf, DIR_CHAR, module_name, ".csr", _ );	// Fichier requête de certification
				if ( ( file_exists( fname1 ) )
					&&( ( file_date( fname1 ) + (60*60*24*30) ) > time(NULL) )
					&&( file_exists( fname ) )
					&&( ( file_date( fname ) + (60*60*24*30) ) > time(NULL) )
					)
					{
					print( "ERREUR : Une demande est déjà en cours." BR, _ );
					return ERROR;
					}
				if ( h1 ) var_free( h1 );
				h1 = var_new();
				ssl_conf_load( h1, var_get( hSrvConf, "partname" ), null );
				var_save( h1, fname );
				v = var_get( h1, "ssl_csr" );
				if ( ! file_exists( fname1 ) )
					file_save_str( fname1, v, -1 );	
				strdcat( &s1, "Un CSR a été généré dans ", fname1, BR, _ );
				var_set( h, "ssl_msg", s1 );
				}
			else
			
			
			
			//------------------------------------------------------------------
			// REINITIALISE LA CONF SSL (La conf SSL actelle est supprimée)
			//------------------------------------------------------------------

			if ( !stricmp( v, "newcer" ) )
				{
				// SUPPRIME LA CONF ACTUELLE,
				// UNE NOUVELLE SERA INITIALISEE PLUS LOIN DANS LE PROGRAMME
				
				var_delete( h, "ssl_csr" );
				var_delete( h, "ssl_private_key" );
				}
			else
			
			
			
			//------------------------------------------------------------------
			// COMPLETE LA CONFIG SSL ACTUELLE
			//------------------------------------------------------------------

				{
				strdcpy( &s2, dirconf, DIR_CHAR "certif_lib", _ );
				if ( ssl_conf_load( h, v, s2 ) == ERROR )
					{
/*
					strdcpy( &s2, dirconf, DIR_CHAR "certif_lib", _ );
					if ( ( ! var_get( h, "ssl_certif_server" ) )
						||( !dir_exists( s2 ) )
						||( ssl_conf_load( h, s2, null ) == ERROR )
						)
*/
						var_set( h, "ssl_msg", error_msg );
					}
				}
			
			
			//------------------------------------------------------------------
			// CONSTRUIT LE LIBELLE A AFFICHER
			//------------------------------------------------------------------
			
			v = var_get( h, "ssl_certif_server" );
			if ( v )
				{
				v = var_get( h, "ssl_certif_server_owner" );
				strdcat( &s1, var_get( h, "ssl_certif_server_enddate" ), " : ", v, BR, _ );
				for( n = 1 ; ; n++ )
					{
					strdcpy( &s2, "ssl_certif_ca", itoa(n), _ );
//					if ( !var_get( hSrvConf, s2 ) ) break;
					if ( !var_get( h, s2 ) ) break;
					strdcpy( &s2, "ssl_certif_ca", itoa(n), "_owner", _ );
					v = var_get( h, s2 );
					if ( !v ) break;
					strdcpy( &s2, "ssl_certif_ca", itoa(n), "_enddate", _ );
					strdcat( &s1, var_get( h, s2 ), " : ", v, BR, _ );
					}
				var_set( h, "ssl_config", s1 );
				var_delete( h, "ssl_msg" );
				}
			else
				{
				v = var_get( h, "ssl_csr" );
				if ( !v )
					strdcat( &s1, "Aucune configuration SSL.", BR, _ );
				else
					{
					strdcpy( &fname, dirconf, DIR_CHAR, module_name, ".csr", _ );
					if ( ! file_exists( fname ) )
						file_save_str( fname, v, -1 );	
					strdcat( &s1, "Un CSR a été généré dans ", fname, BR, _ );
					}
				var_set( h, "ssl_msg", s1 );
				}
				
			continue;
			}

		//----------------------------------------------------------------------

		if ( !stricmp( vn, "of" ) ) continue;
		
		print( "ERREUR : paramètre ", vn, " inconnu", _ );
		return ERROR;
		}
	
	

	//--------------------------------------------------------------------------
	// VERIFIE LA CONFIGURATION SSL
	//--------------------------------------------------------------------------

	v = var_get( ((h)?h:hSrvConf), "ssl_csr" );
	if ( ( !v )||( v[0] == 0 ) )
		{
		//------------------------------------------------------------------
		// Aucune conf SSL => GENERE UNE CLEF PRIVEE ET UN CSR ET DEMANDE DE CERTIF
		//------------------------------------------------------------------


		if (( partname = var_get( ((h)?h:hSrvConf), "partname" ) )&&( trim(partname)[0] ))
			{

			if ( !h ) h = var_dup( hSrvConf );
			ssl_conf_load( h, partname, null );
			v = var_get( h, "ssl_csr" );
			if ( ( !v )||( v[0] == 0 ) )
				var_set( h, "ssl_msg", error_msg );
			else
				{
				strdcpy( &fname1, dirconf, DIR_CHAR, module_name, ".csr", _ );
				file_save_str( fname1, v, -1 );	
				
				// DEMANDE UN CERTIFICAT A UN SERVEUR NAT
				
				strdcpy( &cmd, "get_ssl_certif ", partname, " ", quote( v ), _ );
					
				if ( nat_request( &s, cmd ) )
					{
					if ( strpos( s, "-----BEGIN CERTIFICATE", 0 ) >= 0 )
						{
						if ( ssl_conf_load( h, s, null ) != ERROR )
							{
							v = var_get( h, "ssl_certif_server_owner" );
							strdcpy( &s1, var_get( h, "ssl_certif_server_enddate" ), " : ", v, BR, _ );
							for( n = 1 ; ; n++ )
								{
								strdcpy( &s2, "ssl_certif_ca", itoa(n), _ );
								if ( !var_get( h, s2 ) ) break;
								strdcpy( &s2, "ssl_certif_ca", itoa(n), "_owner", _ );
								v = var_get( h, s2 );
								if ( !v ) break;
								strdcpy( &s2, "ssl_certif_ca", itoa(n), "_enddate", _ );
								strdcat( &s1, var_get( h, s2 ), " : ", v, BR, _ );
								}
							var_set( h, "ssl_config", s1 );
							var_delete( h, "ssl_msg" );
							}
						else
							var_set( h, "ssl_msg", str(error_msg, BR BR, s, BR, _) );
						}
					else
						{
						var_set( h, "ssl_msg", s );
						}
					}
				else
					{
					strdcpy( &s, "Un CSR a été généré dans ", fname1, BR, _ );
					var_set( h, "ssl_msg", s );
					}
				}
			}
		else
			{
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "ssl_msg", "Aucune configuration SSL" );
			}
		}
	


	//--------------------------------------------------------------------------
	if ( h )	// IL Y A EU DES CHANGEMENTS => ON ENREGISTRE
	//--------------------------------------------------------------------------
		{
		var_free( hSrvConf );
		hSrvConf = h;
		server_conf_save( );

		h = var_new();
		
		//----------------------------------------------------------------------
		// VERIFIE LA VALIDITE DE LA CONF
		// ET GENERE LE FICHIER DE CONF PARTENAIRE DE CE SERVEUR SI OK
		//----------------------------------------------------------------------
		
		for( ; ; )	// Ceci n'est pas une boucle. C'est une technique pour interrompre 
					// un traitement dès qu'un paramètre n'est pas correcte et continuer la suite
			{
			var_set( h, "product_name", product_name );
			var_set( h, "os", OS_NAME );
			v = var_get( hSrvConf, "ip_ssie" );
			if ( ! v ) break;
			var_set( h, "ip_ssie", v );
			var_set( h, "dns", var_get( hSrvConf, "dns" ) );
// Ajout ip et dns_ssie.
			var_set( h, "ip", var_get( hSrvConf, "ip" ) );
			var_set( h, "dns_ssie", var_get( hSrvConf, "dns_ssie" ) );
			v = var_get( hSrvConf, "port" );
			if ( ! v ) break;
			var_set( h, "port", v );
			v = var_get( hSrvConf, "ssl_config_ok" );
			if ( ! v ) break;
			var_set( h, "ssl_config_ok", v );
			v = var_get( hSrvConf, "ssl_certif_server" );
			if ( ! v ) break;
			var_set( h, "ssl_certif_server", v );
			v = var_get( hSrvConf, "ssl_certif_server_endtime" );
			if ( ! v ) break;
			var_set( h, "ssl_certif_server_endtime", v );
			for( i = 1 ; ; i++ )
				{
				strdcpy( &s, "ssl_certif_ca", itoa( i ), _ );
				v = var_get( hSrvConf, s );
				if ( !v ) break;
				var_set( h, s, v );
				}
			v = var_get( hSrvConf, "ssl_config" );
			if ( ! v ) break;
			var_set( h, "ssl_config", v );

			var_set( h, "old_partname", v = var_get( hSrvConf, "old_partname" ) );
			if ( v )
				{
				strdcpy( &fname, dirconf, DIR_CHAR, v, ".conf", _ );
				file_delete( fname );
				}

			var_set( h, "descr", v = var_get( hSrvConf, "descr" ) );

			v = var_get( hSrvConf, "partname" );
			if ( ! v ) break;
			var_set( h, "partname", v );

			strdcpy( &fname, dirconf, DIR_CHAR, v, ".conf", _ );
			var_save( h, fname );
			var_set( h, "enable", "Oui" );
			strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, v, ".dat", _ );
			var_save( h, fname );
			
			// TRANSFERT LES INFORMATIONS AU SERVEUR D'ANNUAIRE
			
			var_to_str( h, &s );
			strdcpy( &cmd, "part save ", s, _ );
			nat_request( &s, cmd );
			 
			break;
			}
		}
	
	
	

	//--------------------------------------------------------------------------
	// VERIFIE LA DATE DE VALIDITE DU CERTIFICAT SSL
	//--------------------------------------------------------------------------

	n = ssl_end_delay( hSrvConf );
	if ( ( var_get( hSrvConf, "ssl_config_ok" ) )&&( n < (30 Jours) ) )
		{
		var_set( hSrvConf, "ssl_msg", str(
			"ATTENTION : La date de validité de votre certificat "
			"arrive à terme dans ", itoa( n/(1 Jour) ), " jour",
			(char*)(((n/(1 Jour))>1)?"s":""), _ )
			);
		}


	
	//--------------------------------------------------------------------------
	// AFFICHE LES INFO SERVEUR
	//--------------------------------------------------------------------------

	
	if ( var_get( hReq, "of" ) )
		{
		print( "{", var_to_js_array( hSrvConf, &s ), "}", _ );
		}
	else
		{
		print_ind( 3, 80,
			"partname     = ", var_get( hSrvConf, "partname" ), BR,
			"descr        = ", var_get( hSrvConf, "descr" ), BR,
//			"dns          = ", var_get( hSrvConf, "dns" ), BR,
			"dns          = ", var_get( hSrvConf, "dns_ssie" ), BR,
			"ip           = ", var_get( hSrvConf, "ip_ssie" ), BR,
			"port         = ", var_get( hSrvConf, "port" ), BR,
			"maxcnx       = ", var_get( hSrvConf, "maxcnx" ), BR,
			"adm_email    = ", var_get( hSrvConf, "adm_email" ), BR,
			"ssl_config   = ", BR,
			_ );

		print_ind( 6, 80, var_get( hSrvConf, "ssl_config" ), BR, BR, var_get( hSrvConf, "ssl_msg" ), BR, _ );
		}
	
	
	
	//--------------------------------------------------------------------------
	}


























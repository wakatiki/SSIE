#include "ssie7.h"


// cmd_server


//------------------------------------------------------------------------------
int cmd_server( T_WORK * t_work )

// COMMANDE SERVER : Configuration du serveur SSIE
//------------------------------------------------------------------------------
	{
tr( "cmd_server_1" );
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
		#ifndef IHM_LOCALE
		var_set( h, "port", DEFAULT_PORT );
		#else
		var_set( h, "port", "1770" );
		var_set( h, "rflux_nb", "0" );
		#endif
		}
	

	#ifdef IHM_LOCALE
	if ( ( ! (v=var_get( hSrvConf, "dirwww" )) )||( ! (trim(v)[0]) ) )
		{
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "dirwww", dirbin );
		}
	#endif

	if ( ( ! (v=var_get( hSrvConf, "maxcnx" )) )||( ! (trim(v)[0]) ) )
		{
		// INITIALISE LE MAXCNX
		
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "maxcnx", "10" );
		}
	

	if ( ( ! (v=var_get( hSrvConf, "maxtask" )) )||( ! (trim(v)[0]) ) )
		{
		// INITIALISE LE MAXCNX
		
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "maxtask", "50" );
		}
	

	if ( ( ! (v=var_get( hSrvConf, "part_auto_crea" )) )||( ! (trim(v)[0]) ) )
		{
		// INITIALISE LE PART_AUTO_CREA
		
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "part_auto_crea", "Oui" );
		}


	v = var_get( hSrvConf, "product_name" );
	if ( ( ! (v) )||( ! (trim(v)[0]) )||( strcmp( v, product_name ) ) )
		{
		// INITIALISE LA VERSION
		
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "product_name", product_name );
		}

	
	#ifdef OS_UNIX
	
	mkdir( diridf );
	mkdir( v = str( dirconf, DIR_CHAR "reveils", _ ) );

	if ( ! dir_exists( dirtemp ) )
		{
		mkdir( dirtemp );
		// DONNE LES DROITS D'ACCES A TOUT LE MONDE
		p = 1;
		while( ( p = strpos( dirtemp, "/", p ) ) > 0 )
			{
			strdcpy( &cmd, "chmod 755 ", substr( dirtemp, 0, p ), _ );
			System( &s, cmd );
			p++;
			}
		strdcpy( &cmd, "chmod 777 ", dirtemp, _ );
		System( &s, cmd );
		}
	
	// VERIFIE L'EXISTANCE DES FICHIERS MFTIDF

	
	strdcpy( &fname, diridf, DIR_CHAR "RECV_R2MMSB.cfg", _ );
	if ( !file_exists( fname ) )
		{
		strdcpy( &s, "EXEC = '", dirconf, DIR_CHAR "reveils" DIR_CHAR "&9.RAPPL.&1.8RAPPL.AVR'", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2MMS.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2MMA.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2MMAB.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2MFS.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2MFSB.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2MFA.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2MFAB.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2IS.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2ISB.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2IA.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "RECV_R2IAB.cfg", _ );
		file_save_str( fname, s, -1 );
		}

	strdcpy( &fname, diridf, DIR_CHAR "SEND_R2MMSB.cfg", _ );
	if ( !file_exists( fname ) )
		{
		strdcpy( &s, "EXEC = '", dirconf, DIR_CHAR "reveils" DIR_CHAR "&9.SAPPL.&1.8SAPPL.ACR'", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2MMS.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2MMA.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2MMAB.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2MFS.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2MFSB.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2MFA.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2MFAB.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2IS.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2ISB.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2IA.cfg", _ );
		file_save_str( fname, s, -1 );
		strdcpy( &fname, diridf, DIR_CHAR "SEND_R2IAB.cfg", _ );
		file_save_str( fname, s, -1 );
		}
	
	
	#endif
	


	//--------------------------------------------------------------------------
	// CHERCHE LES MISES A JOUR de partname, ip ou adl, SUR LE SERVEUR D'ANNUAIRE
	//--------------------------------------------------------------------------

	strdcpy( &cmd, "part '", _ );

	if ( ! ( v = var_get( hSrvConf, "ip_ssie" ) ) )
		{
		// PREMIER DEMARRAGE
		for( i = 0 ; v = tcp_myIP(i) ; i++ )
			strdcat( &cmd, (char*)((i)?",":""), v, _ );
		}
	else
		strdcat( &cmd, v, _ );

	strdcat( &cmd, "' get", _ );

tr( "cmd_server_2" );

	if ( nat_request( &s, cmd ) )
		{
		if ( stripos( s, "erreur", 0 ) < 0 )
			{
			for( ; ; )
				{
				if ( h1 ) var_free( h1 );
				h1 = var_from_str( 0, s );
				if ( !h1 ) break;

				v1 = var_get( h1, "partname" );
				if ( !v1 ) break;
				v2 = var_get( hSrvConf, "partname" );

				if ( ( !v2 )||( !(v2[0]) )||( strcmp( v1, v2 ) ) )
					{
					if ( !h ) h = var_dup( hSrvConf );
					var_set( h, "partname", v1 );
					if ( ( v1 )&&( v1[0] )&&( v2 )&&( v2[0] ) )
						{
						print( "ATTENTION : Votre nom de partenaire à changé : ", v2, " => ", v1, BR, _ );
						logw( "server : Changement de nom de partenaire sur l'annuaire national : ", v2, " => ", v1, _ );
						}
					}

				v1 = var_get( h1, "ip_ssie" );
				if ( !v1 )
				{
					v1 = var_get( h1, "ip" );
					if ( !v1 )	break;

					if ( !h ) h = var_dup( hSrvConf );
					var_set( h, "ip_ssie", v1 );
					var_set( h, "ancien_mft", "Oui" );
				}
				else
				{
					v2 = var_get( hSrvConf, "ip_ssie" );
					if ( ( !v2 )&&( h ) )
						v2 = var_get( h, "ip_ssie" );
					if ( ( !v2 )||( !(v2[0]) )||( strcmp( v1, v2 ) ) )
					{
						if ( !h ) h = var_dup( hSrvConf );

						for( i = 0 ; v = tcp_myIP( i ) ; i++ )
							if ( !strcmp( v, v2 ) ) break;

						if ( !v )
						{
							print( "ATTENTION : L'adresse IP dans l'annuaire national n'est pas valide !!! : ", v2, BR, _ );
							logw( "server : ATTENTION : L'adresse IP dans l'annuaire national n'est pas valide !!! : ", v2, _ );
							break;
						}
						else
						{
//							var_set( h, "ip_ssie", v2 );
							var_set( h, "ip_ssie", v1 );

							if ( ( v1 )&&( v1[0] )&&( v2 )&&( v2[0] ) )
								print( "ATTENTION : Votre IP à changé : '", v2, "' => '", v1, "'.", BR, _ );
							logw( "server : Changement d'IP sur l'annuaire national : '", v2, "' => '", v1, "'.", _ );
						}
					}
				}

				v1 = var_get( h1, "dns_ssie" );
				if ( !v1 )
				{
					v1 = var_get( h1, "dns" );
					if ( !v1 )	break;

					if ( !h ) h = var_dup( hSrvConf );
					var_set( h, "dns_ssie", v1 );
					var_set( h, "ancien_mft", "Oui" );
                }
				else
				{
					v2 = var_get( hSrvConf, "dns_ssie" );
					if ( ( !v2 )&&( h ) )
						v2 = var_get( h, "dns_ssie" );
					if ( strcmp( v1, nvl(v2,"") ) )
					{
						if ( !h ) h = var_dup( hSrvConf );

						var_set( h, "dns_ssie", v1 );

						if ( ( v1 )&&( v1[0] )&&( v2 )&&( v2[0] ) )
							{
							print( "ATTENTION : Votre DNS à changé : '", v2, "' => '", v1, "'.", BR, _ );
							logw( "server : Changement de DNS sur l'annuaire national : '", v2, "' => '", v1, "'.", _ );
							}
					}
				}

				v1 = var_get( h1, "adl_liste" );
				if ( ( !v1 )||( !(v1[0]) ) ) break;
				v2 = var_get( hSrvConf, "adl_liste" );
				if ( ( !v2 )||( !(v2[0]) )
							||( ( v2[0] != '#' )&&( strcmp( v1, v2 ) ) ) )
					{
					if ( !h ) h = var_dup( hSrvConf );
					var_set( h, "adl_liste", v1 );
					if ( ( v2 )&&( v2[0] ) )
						{
         				print( "ATTENTION : Vos ADLs ont changé : ", v2, " => ", v1, BR, _ );
						logw( "server : les ADLs ont changées sur l'annuaire national : ", v2, " => ", v1, _ );
						}
					}

				break;
				}
			}
		else
			if ( !h ) h = var_dup( hSrvConf );
		}


	//--------------------------------------------------------------------------
	// LECTURE DES PARAMETRES
	//--------------------------------------------------------------------------

tr( "cmd_server_3" );

	for( i = 1 ; v = var_getn( hReq, i, &vn ) ; i++ )
		{
		//----------------------------------------------------------------------
		if ( !stricmp( vn, "partname" ) )
		//----------------------------------------------------------------------
			{
			v1 = var_get( hSrvConf, "partname" );
			if ( ( v1 )&&( !stricmp( v, v1 ) ) ) continue;
			
			#ifdef IHM_LOCALE
			if ( !(v[0]) ) goto partname_ok;
			#endif
			
			if ( ! is_alphanum( v, "_" ) )
				{
				print( "ERREUR : Le nom de partenaire doit être alphanumérique" BR, _ );
				return ERROR;
				}

				
			if ( strlen( v ) != 8 )
				{
				print( "ERREUR : Le nom de partenaire doit être sur 8 caractères" BR, _ );
				return ERROR;
				}
				
			// Vérifie que le partenaire n'est pas déjà pris par quelqu'un d'autre

			strdcpy( &cmd, "part ", v, " get ", _ );

			if ( nat_request( &s, cmd ) )
				{
				if ( stripos( s, "ERREUR", 0 ) < 0 )	
					{
					if ( h1 ) var_free( h1 );
					h1 = var_from_str( 0, s );
					v1 = var_get( h1, "ip" );
					v2 = var_get( h1, "ip_ssie" );
					if ( !v2 ) v2 = v1;
					v3  = var_get( hSrvConf, "ip_ssie" );
					if ( ( !v3 )&&( h ) ) v3 = var_get( h, "ip_ssie" );
					if ( !v3 )
						{
						print( "ERREUR : Votre adresse IP n'est pas paramétré", BR, _ );
						return ERROR;
						}
						
					if ( ( v2 )&&( strcmp( v2, v3 ) ) )
						{
						print( "ERREUR : Ce partenaire est déjà pris par un autre serveur", BR,
							_ );
						return ERROR;
						}
					}
				else
				if ( stripos( s, "non trouvé", 0 ) < 0 )
					{
					print( s, BR, _ );
					return ERROR;
					}
				}
			
			partname_ok:
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "old_partname", var_get( h, "partname" ) );
			var_set( h, "partname", upper(v) );
			
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! stricmp( vn, "descr" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "descr", v );
			
			continue;
			}


		//----------------------------------------------------------------------
		if ( ! stricmp( vn, "mft_log_delay" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			n = atoi(v);
			if ( n <= 0 ) n = 30;
			if ( n != 30 )
				var_set( h, "mft_log_delay", itoa(n) );
			else
				var_delete( h, "mft_log_delay" );
			
			continue;
			}


		//----------------------------------------------------------------------
		if ( ! stricmp( vn, "ssl_warn_mail_delay" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			n = atoi(v);
			if ( n <= 0 ) n = 30;
			if ( n != 30 )
				var_set( h, "ssl_warn_mail_delay", itoa(n) );
			else
				var_delete( h, "ssl_warn_mail_delay" );
			
			continue;
			}


		//------------------------------------------------------------------
		if ( ! stricmp( vn, "ip" ) )
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
		if ( ! stricmp( vn, "dns" ) )
		//------------------------------------------------------------------
			{
			if ( ( v )&&( v[0] )&&( ! (v1 = tcp_dns_address( v )) ) )
				{
				print( "ERREUR : dns non valide", _ );
				return ERROR;
				}

			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "dns_ssie", v );
			
			continue;
			}
		
		//----------------------------------------------------------------------
		if ( !stricmp( vn, "port" ) )
		//----------------------------------------------------------------------
			{
			if ( ( ! is_numeric( v ) )
				||( (p = atoi( v )) < 1 )||( p > 65535 ) )
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

/*
		//----------------------------------------------------------------------
		if ( !stricmp( vn, "port_ssl" ) )
		//----------------------------------------------------------------------
			{
			if ( ( ! is_numeric( v ) )
				||( (p = atoi( v )) < 1 )||( p > 65535 ) )
				{
				print( "ERREUR : Le port ssl n'est pas valide" BR, _ );
				var_free( hSrvConf );
				hSrvConf = 0;
				server_conf_load( );
				return ERROR;
				}

			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "port_ssl", v );
			
			continue;
			}


		//----------------------------------------------------------------------
		if ( !stricmp( vn, "port_ssl2" ) )
		//----------------------------------------------------------------------
			{
			if ( ( ! is_numeric( v ) )
				||( (p = atoi( v )) < 1 )||( p > 65535 ) )
				{
				print( "ERREUR : Le port ssl2 n'est pas valide" BR, _ );
				var_free( hSrvConf );
				hSrvConf = 0;
				server_conf_load( );
				return ERROR;
				}

			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "port_ssl2", v );
			
			continue;
			}
*/

		//----------------------------------------------------------------------
		if ( !stricmp( vn, "maxcnx" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "maxcnx", v );
			
			continue;
			}


		//----------------------------------------------------------------------
		if ( !stricmp( vn, "maxtask" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "maxtask", v );
			
			continue;
			}


		//----------------------------------------------------------------------
		if ( !stricmp( vn, "dirwww" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "dirwww", v );
			
			continue;
			}


		//----------------------------------------------------------------------
		if ( !stricmp( vn, "rflux_nb" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "rflux_nb", v );
			
			continue;
			}


		//----------------------------------------------------------------------
		if ( !stricmp( vn, "part_auto_crea" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			if ( strmicmp( v, "o|oui|y|yes" ) )
				var_set( h, "part_auto_crea", "Oui" );
			else
				var_set( h, "part_auto_crea", "Non" );
			
			continue;
			}


		#ifdef OS_LINUX
		//----------------------------------------------------------------------
		if ( !stricmp( vn, "no_drop_caches" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			if ( strmicmp( v, "o|oui|y|yes" ) )
				var_set( h, "no_drop_caches", "Oui" );
			else
				var_set( h, "no_drop_caches", "Non" );
			
			continue;
			}
		#endif


		//----------------------------------------------------------------------
		if ( ( !stricmp( vn, "mft_gateway" ) )&&( !mft_exists() ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			if ( v[0] )
				{
				var_set( h, "mft_gateway", v );
	            }
			else
				{
				var_delete( h, "mft_gateway" );
	            }
			
			continue;
			}


		//----------------------------------------------------------------------
		if ( !stricmp( vn, "trf_timeout" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			if ( atoi( v ) != 0 )
				var_set( h, "trf_timeout", v );
			else
				var_delete( h, "trf_timeout" );
			
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
		if ( !stricmp( vn, "adl_liste" ) )
		//----------------------------------------------------------------------
			{
			if ( !h ) h = var_dup( hSrvConf );
			var_set( h, "adl_liste", v );
			
			continue;
			}


		//----------------------------------------------------------------------
		if (( !stricmp( vn, "ssl2" ) )||( !stricmp( vn, "ssl3" ) )
			||( !stricmp( vn, "ssl4" ) ))
		//----------------------------------------------------------------------
			goto traite_param_ssl;
			
			
		//----------------------------------------------------------------------
		if (( !stricmp( vn, "ssl" ) )||( !stricmp( vn, "ssl1" ) )||( !stricmp( vn, "ssl_config" ) ))
		//----------------------------------------------------------------------
			{
			vn = "ssl";
			
			traite_param_ssl:
				
			if ( !h ) h = var_dup( hSrvConf );
			strdcpy( &s1, "", _ );
			strdcpy( &w, vn, "_msg", _ );
			var_delete( h, w );

			//------------------------------------------------------------------
			// DEMANDE LA GENERATION D'UNE NOUVELLE CLEF PRIVE ET LA GENERATION D'UN CSR
			// La conf SSL actuelle reste dans le même état
			//------------------------------------------------------------------

			if ( !stricmp( v, "gencsr" ) )
				{
				strdcpy( &fname, dirconf, DIR_CHAR, module_name, "_", vn, ".new", _ );	// Contient la nouvelle clef privée
				strdcpy( &fname1, dirconf, DIR_CHAR, module_name, "_", vn, ".csr", _ );	// Fichier requête de certification
				strdcpy( &w, vn, "_csr", _ );
				if ( ( var_get( hSrvConf, w ) )
					&&( file_exists( fname1 ) )
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
				ssl_conf_load2( h1, var_get( h, "partname" ), null, vn );
				var_save( h1, fname );
				strdcpy( &w, vn, "_csr", _ );
				v = var_get( h1, w );
				if ( ! file_exists( fname1 ) )
					file_save_str( fname1, v, -1 );	
				strdcat( &s1, "Un CSR a été généré dans ", fname1, BR, _ );
				strdcpy( &w, vn, "_msg", _ );
				var_set( h, w, s1 );
				}
			else
			
			
			
			//------------------------------------------------------------------
			// REINITIALISE LA CONF SSL (La conf SSL actelle est supprimée)
			//------------------------------------------------------------------

			if ( !stricmp( v, "newcer" ) )
				{
				// VERIFI QU'UN SERVEUR D'ANNUAIRE EST DISPONIBLE
				
				if ( !nat_search( ) )
					{
					print( "ERREUR : aucun serveur d'annuaire disponible." BR, _ );
					return ERROR;
					}
					
				// SUPPRIME LA CONF ACTUELLE,
				// UNE NOUVELLE SERA INITIALISEE PLUS LOIN DANS LE PROGRAMME
				
				strdcpy( &w, vn, "_csr", _ );
				var_delete( h, w );
				strdcpy( &w, vn, "_config", _ );
				var_delete( h, w );
				strdcpy( &w, vn, "_private_key", _ );
				if ( !strcmp( vn, "ssl" ) )
					var_delete( h, w );
				else
					var_set( h, w, "generate" );
				}
			else
			
			
			
			//------------------------------------------------------------------
			// REINITIALISE LA CONF SSL (La conf SSL actelle est supprimée)
			//------------------------------------------------------------------

			if ( !stricmp( v, "none" ) )
				{
				if ( strcmp( vn, "ssl" ) )
					{
					strdcpy( &w, vn, "_primary_key", _ );
					var_delete( h, w );
					strdcpy( &w, vn, "_csr", _ );
					var_delete( h, w );
					strdcpy( &w, vn, "_certif_server", _ );
					var_delete( h, w );
					strdcpy( &w, vn, "_config", _ );
					var_delete( h, w );
					strdcpy( &w, vn, "_config_ok", _ );
					var_delete( h, w );
					strdcpy( &w, vn, "_msg", _ );
					var_delete( h, w );
					continue;
					}
				}
			else
			
			
			
			//------------------------------------------------------------------
			// REINITIALISE LA CONF SSL (La conf SSL actelle est supprimée)
			//------------------------------------------------------------------

			if ( !strnicmp( v, "swap", 4 ) )
				{
				v1 = strdcpy( &s4, vn, "_", _ );
				i = atoi( &(v[4]) );
				switch(i)
					{
					case 0 : v2 = v1, v1 = "ssl_"; break;
					case 1 : v2 = v1, v1 = "ssl_"; break;
					case 2 : v2 = "ssl2_"; break;
					case 3 : v2 = "ssl3_"; break;
					case 4 : v2 = "ssl4_"; break;
					default:
						print( "ERREUR : ssl swap invalide", _ );
						return ERROR;
					}
					
				if ( !strcmp( v1, v2 ) )
					{	
					print( "ERREUR : ssl swap invalide", _ );
					return ERROR;
					}
				
				if ( !strcmp( v1, "ssl_" ) )
					{
					strdcpy( &w, v2, "config_ok", _ );
					if (( !(v=var_get( h, w )) )||( strcmp( v, "true" ) ))
						{
						print( "ERREUR : ssl swap d'une configuration non valide : ", v2, _ );
						return ERROR;
						}
					}

				for( i = 0 ; v = var_getn( h, i, &vn ) ; i++ )
					{
					if ( !strncmp( vn, v1, -1 ) )
						{
						strdcpy( &s, v2, &(vn[strlen(v1)]), _ );
						var_setn( h, i, s, null );
						}
					else
					if ( !strncmp( vn, v2, -1 ) )
						{
						strdcpy( &s, v1, &(vn[strlen(v2)]), _ );
						var_setn( h, i, s, null );
						}
					}
				continue;
				}
			else
			
			
			
			//------------------------------------------------------------------
			// COMPLETE LA CONFIG SSL ACTUELLE
			//------------------------------------------------------------------

				{
				strdcpy( &s2, dirconf, DIR_CHAR "certif_lib", _ );
				if ( ssl_conf_load2( h, v, s2, vn ) == ERROR )
					{
					strdcpy( &w, vn, "_msg", _ );
					var_set( h, w, error_msg );
					}
				}
			
			
			//------------------------------------------------------------------
			// CONSTRUIT LE LIBELLE A AFFICHER
			//------------------------------------------------------------------
			
			strdcpy( &w, vn, "_certif_server", _ );
			v = var_get( h, w );
			if ( v )
				{
				strdcpy( &w, vn, "_certif_server_owner", _ );
				v = var_get( h, w );
				strdcpy( &w, vn, "_certif_server_enddate", _ );
				strdcat( &s1, var_get( h, w ), " : ", v, BR, _ );
				for( n = 1 ; ; n++ )
					{
					strdcpy( &w, vn, "_certif_ca", itoa(n), _ );
					if ( !var_get( h, w ) ) break;
					strdcpy( &w, vn, "_certif_ca", itoa(n), "_owner", _ );
					v = var_get( h, w );
					if ( !v ) break;
					strdcpy( &w, vn, "_certif_ca", itoa(n), "_enddate", _ );
					strdcat( &s1, var_get( h, w ), " : ", v, BR, _ );
					}
				strdcpy( &w, vn, "_config", _ );
				var_set( h, w, s1 );
				}
			else
				{
				strdcpy( &w, vn, "_csr", _ );
				v = var_get( h, w );
				if ( !v )
					{
					strdcpy( &w, vn, "_private_key", _ );
					if ( ! var_get( h, w ) )
						{
						strdcat( &s1, "Aucune configuration SSL.", BR, _ );
						strdcpy( &w, vn, "_msg", _ );
						var_set( h, w, s1 );
						}
					}
				else
					{
					strdcpy( &fname, dirconf, DIR_CHAR, module_name, "_", vn, ".csr", _ );
					if ( ! file_exists( fname ) )
						file_save_str( fname, v, -1 );	
					strdcat( &s1, "Un CSR a été généré dans ", fname, BR, _ );
					strdcpy( &w, vn, "_msg", _ );
					var_set( h, w, s1 );
					}
				}
				
			continue;
			}

		//----------------------------------------------------------------------

		if ( !stricmp( vn, "of" ) ) continue;
		
		print( "ERREUR : paramètre ", vn, " inconnu", _ );
		return ERROR;
		}
	
	
	#ifdef IHM_LOCALE
	v1 = var_get( ((h)?h:hSrvConf), "partname" );
	if ( ( !v1 )||( !(v1[0]) ) )
		{
		strdcpy( &s, sysuser(), _ );
		for( i = strlen(s) ; i < 7 ; i++ )
			strdcat( &s, "_", _ );
		if ( strlen(s) == 7 )
			strdcat( &s, "U", _ );
		
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "partname", upper(s) );
		}
	#endif

	//--------------------------------------------------------------------------
	// VERIFIE LA CONFIGURATION SSL
	//--------------------------------------------------------------------------

tr( "cmd_server_4" );

	partname = strdcpy( &part, var_get( ((h)?h:hSrvConf), "partname" ), _ );
	if ( trim(partname)[0] )
	for( i = 1 ; i < 5 ; i++ )
		{
		switch( i )
			{
			case 1 : vn = "ssl"; break;
			case 2 : vn = "ssl2"; break;
			case 3 : vn = "ssl3"; break;
			case 4 : vn = "ssl4"; break;
			}
		strdcpy( &w, vn, "_private_key", _ );

		v = var_get( ((h)?h:hSrvConf), w );
		if ( i == 1 )
			{
			if ( ( v )&&( v[0] ) ) continue; // certif principal déjà OK ou en erreur
			}
		else
		if ( ( !v )||( strcmp( v, "generate" ) ) ) continue; // Pas de demande


		//------------------------------------------------------------------
		// Aucune conf SSL => GENERE UNE CLEF PRIVEE ET UN CSR ET DEMANDE UN CERTIF
		//------------------------------------------------------------------

		if ( !h ) h = var_dup( hSrvConf );
		var_delete( h, w );
		ssl_conf_load2( h, partname, null, vn );
		strdcpy( &w, vn, "_csr", _ );
		v = var_get( h, w );
		if ( ( !v )||( v[0] == 0 ) )
			{
			strdcpy( &w, vn, "_msg", _ );
			var_set( h, w, error_msg );
			}
		else
			{
			strdcpy( &fname1, dirconf, DIR_CHAR, partname, "_", vn, ".csr", _ );
			strdcpy( &w, vn, "_csr", _ );
			file_save_str( fname1, v = var_get( h, w ), -1 );	
			
			// DEMANDE UN CERTIFICAT A UN SERVEUR NAT
			
			strdcpy( &cmd, "get_ssl_certif ", partname, " ", quote( v ), _ );

			if ( nat_request( &s, cmd ) )
				{
				if ( strpos( s, "-----BEGIN CERTIFICATE", 0 ) >= 0 )
					{
					if ( ssl_conf_load2( h, s, null, vn ) != ERROR )
						{
						strdcpy( &w, vn, "_certif_server_owner", _ );
						v = var_get( h, w );
						strdcpy( &w, vn, "_certif_server_enddate", _ );
						strdcpy( &s1, var_get( h, w ), " : ", v, BR, _ );
						for( n = 1 ; ; n++ )
							{
							strdcpy( &w, vn, "_certif_ca", itoa(n), _ );
							if ( !var_get( h, w ) ) break;
							strdcpy( &w, vn, "_certif_ca", itoa(n), "_owner", _ );
							v = var_get( h, w );
							if ( !v ) break;
							strdcpy( &w, vn, "_certif_ca", itoa(n), "_enddate", _ );
							strdcat( &s1, var_get( h, w ), " : ", v, BR, _ );
							}
						strdcpy( &w, vn, "_config", _ );
						var_set( h, w, s1 );
						strdcpy( &w, vn, "_msg", _ );
						var_delete( h, w );
						}
					else
						{
						strdcpy( &w, vn, "_msg", _ );
						var_set( h, w, str(error_msg, BR BR, s, BR, _) );
						}
					}
				else
					{
					strdcpy( &w, vn, "_msg", _ );
					var_set( h, w, s );
					}
				}
			else
				{
				strdcpy( &s, "Un CSR a été généré dans ", fname1, BR, _ );
				strdcpy( &w, vn, "_msg", _ );
				var_set( h, w, s );
				}
			}
		}


tr( "cmd_server_5" );

	if (( !(v=var_get( ((h)?h:hSrvConf), "ssl_config_ok" )) )
		||( strcmp( v, "true" ) ))
		{
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "ssl_msg", "Aucune configuration SSL" );
		}


	v = var_get( hSrvConf, "dns_ssie" );
	v1 = var_get( hSrvConf, "partname" );

	if (( v )&&( !stricmp( v, "si-ssie.edf.fr" ) )
		&&( ( !v1 )||( stricmp( v1, "MFTCCWEX" ) ) )
		)
		{
		if ( !h ) h = var_dup( hSrvConf );
		var_set( h, "dns_ssie", "" );
		}
		

	//--------------------------------------------------------------------------
	if (( h )&&( is_authorized( "is_admin", null ) ))	// IL Y A EU DES CHANGEMENTS => ON ENREGISTRE
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
			
			// On revérifie la validité de l'ip pour le cas d'un changement de machine
			// par recopie de la conf de l'ancienne machine.
			#ifndef IHM_LOCALE
//			#ifdef OS_UNIX
			for( j = 0 ; v1 = tcp_myIP( j ) ; j++ )
				if ( !strcmp( v, v1 ) ) break;
			if ( !v1 )
				v = tcp_myIP( 0 );
			#else // Windows avec IHM Locale
			v = "127.0.0.1";
			#endif
			
			var_set( h, "ip_ssie", v );
			v = var_get( hSrvConf, "dns_ssie" );
			if ( v )	var_set( h, "dns_ssie", v );

			if ( mft_exists() )
				{
				var_set( h, "ip", v );
				var_set( h, "dns", var_get( hSrvConf, "dns_ssie" ) );
				}
			else
			if ( v = var_get( hSrvConf, "mft_gateway" ) )
				{
				var_set( h, "mft_gateway", upper(v) );
				strdcpy( &fname1, dirconf, DIR_CHAR "part" DIR_CHAR, v, ".dat", _ );
				strdcpy( &cmd, argv[0], " part ", v, " loadnat", _ );
				System( &s, cmd ); 
         		if ( h1 ) var_free( h1 );
        		h1 = var_load( fname1 );
        		v1 = var_get( h1, "ip_ssie" );
        		v = var_get( h1, "dns_ssie" );
        		if (( !v )||( !(v[0]) ))
					v = v1;
				
				var_set( h, "ip", v );
				var_set( h, "dns", v );
				}
			else
				{
				var_set( h, "ip", "si-ssie.edf.fr" );
				var_set( h, "dns", "si-ssie.edf.fr" );
				}

			
			var_set( h, "adl_liste", var_get( hSrvConf, "adl_liste" ) );
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

			var_set( h, "descr", var_get( hSrvConf, "descr" ) );
			var_set( h, "adm_email", var_get( hSrvConf, "adm_email" ) );

			v = var_get( hSrvConf, "partname" );
			if ( ! v ) break;
			var_set( h, "partname", v );
			var_set( h, "enable", "Oui" );

			strdcpy( &fname, dirconf, DIR_CHAR, v, ".conf", _ );
			var_save( h, fname );
			
			strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, v, ".dat", _ );
			var_free( h1 );
			h1 = var_load( fname );
			v = var_get( h1, "files_dir" );
			if ( v ) var_set( h, "files_dir", v );
			v = var_get( h1, "trf_direction" );
			if ( v ) var_set( h, "trf_direction", v );
			var_save( h, fname );
			
			// TRANSFERT LES INFORMATIONS AU SERVEUR D'ANNUAIRE
			
			var_to_str( h, &s );
			strdcpy( &cmd, "part save ", s, _ );

			if ( s ) s[0] = 0;
			nat_request( &s, cmd );
			
			if ( stripos( s, "ERREUR", 0 ) >= 0 )
				var_set( hSrvConf, "ssl_msg", s );
			else
				var_set( hSrvConf, "ssl_msg", "" );


            server_conf_save( );
            			
			break;
			}
		}
	
	

	//--------------------------------------------------------------------------
	// VERIFIE LA DATE DE VALIDITE DU CERTIFICAT SSL
	//--------------------------------------------------------------------------

	for( i = 1 ; i < 5 ; i++ )
		{
		switch( i )
			{
			case 1 : vn = "ssl"; break;
			case 2 : vn = "ssl2"; break;
			case 3 : vn = "ssl3"; break;
			case 4 : vn = "ssl4"; break;
			}
		
		strdcpy( &w, vn, "_config_ok", _ );	
		if ( ( var_get( hSrvConf, w ) )
				&&( (n=ssl_end_delay( hSrvConf, vn )) < (30 Jours) ) )
			{
			strdcpy( &w, vn, "_msg", _ );	
			if ( n >= 0 )
				var_set( hSrvConf, w, str(
					"ATTENTION : La date de validité de votre certificat "
					"arrive à terme dans ", itoa( n/(1 Jour) ), " jour",
					(char*)(((n/(1 Jour))>1)?"s":""), _ )
					);
			else
				var_set( hSrvConf, w, 
					"ATTENTION : Votre certificat SSL n'est plus valide" );
			}
		}



	//--------------------------------------------------------------------------
	// AFFICHE LES INFO SERVEUR
	//--------------------------------------------------------------------------
	
	if ( var_get( hReq, "of" ) )
		{
		print( "{", var_to_js_array( hSrvConf, &s ),
				",product_name:", str_quote( product_name ),
				",os:", str_quote( OS_NAME ),
				"}", _ );
		}
	else
		{
		print_ind( 3, 80,
			"partname       = ", var_get( hSrvConf, "partname" ), BR,
			"descr          = ", var_get( hSrvConf, "descr" ), BR,
			"dns            = ", var_get( hSrvConf, "dns_ssie" ), BR,
			"ip             = ", var_get( hSrvConf, "ip_ssie" ), BR,
			"port           = ", var_get( hSrvConf, "port" ), BR,
			"adl_liste      = ", var_get( hSrvConf, "adl_liste" ), BR,
			"maxcnx         = ", var_get( hSrvConf, "maxcnx" ), BR,
			"maxtask        = ", var_get( hSrvConf, "maxtask" ), BR,
			"part_auto_crea = ", var_get( hSrvConf, "part_auto_crea" ), BR,
			"adm_email      = ", var_get( hSrvConf, "adm_email" ), _ );

			
		if ( v = var_get( hSrvConf, "trf_timeout" ) )
			print_ind( 3, 80,
			"trf_timeout    = ", v, _ );
			
		if ( v = var_get( hSrvConf, "mft_log_delay" ) )
			print_ind( 3, 80,
			"mft_log_delay  = ", v, _ );
			
		if ( v = var_get( hSrvConf, "mft_gateway" ) )
			print_ind( 3, 80,
			"mft_gateway    = ", v, _ );
			
		if ( v = var_get( hSrvConf, "ssl_warn_mail_delay" ) )
			print_ind( 3, 80,
			"ssl_warn_mail_delay = ", v, _ );
			
		
		for( i = 1 ; i < 5 ; i++ )
			{
			switch( i )
				{
				case 1 : vn = "ssl"; break;
				case 2 : vn = "ssl2"; break;
				case 3 : vn = "ssl3"; break;
				case 4 : vn = "ssl4"; break;
				}
			strdcpy( &w, vn, "_config", _ );
			v1 = var_get( hSrvConf, w );
			strdcpy( &w, vn, "_msg", _ );
			v2 = var_get( hSrvConf, w );
			
			if ( ( v1 )&&( v1[0] ) )
				{
				print_ind( 3, 80,
					vn,  "_config    ", (char*)((i==1)?" ":""), "= ", _ );
				print_ind( 6, 80, v1, _ );
				
				if ( ( v2 )&&( v2[0] ) )
					print_ind( 6, 80, v2, BR, _ );
				}
			else
			if ( ( v2 )&&( v2[0] ) )
				{
				print_ind( 3, 80,
					vn,  "_config    = ", _ );
				print_ind( 6, 80, v2, BR, _ );
				}
			}
		}
	
	
	
	//--------------------------------------------------------------------------
	}


























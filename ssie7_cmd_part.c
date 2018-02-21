#include "ssie7.h"


// GESTION DES LOGINS POUR AVOIR LES DROITS DE TRANSFERER DES FICHIERS AVEC LE SERVEUR

#define part_fname			fname3
#define bModif				b
#define bModeJS				b1
#define bIndexADL			b2

//------------------------------------------------------------------------------
int cmd_part( T_WORK *t_work )

// COMMANDE PART : GESTION/CONFIGURATION DES PARTENAIRES DISTANTS
//------------------------------------------------------------------------------
	{
	bModeJS = false; // Affichage type écran
	v = var_get( hReq, "of" );
	if ( ( v )&&( !strcmp( v, "js" ) ) ) bModeJS = true; // Affichage sous forme de tableau javascript

	//--------------------------------------------------------------------------
	// LECTURE DU PARTNAME
	//--------------------------------------------------------------------------
	
	partname = null;
	v = var_getn( hReq, 1, &partname );
//logw( "cmd_part:", partname, _ );
	if (( v )&&( strcmp( partname, "of" ) ))
		{
		strdcpy( &part_fname, dirconf, DIR_CHAR "part" DIR_CHAR, upper(partname), ".dat", _ );
//logw( "cmd_part1", _ );

		if ( h ) var_free( h );
		h = var_load( part_fname );
		
		//----------------------------------------------------------------------
		// LECTURE DES PARAMETRES
		//----------------------------------------------------------------------
		
		bModif = false;
		
		for( i = 2 ; v = var_getn( hReq, i, &vn ) ; i++ )
			{
			if ( ! strcmp( vn, "of" ) ) continue;
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "enable" ) )
			//------------------------------------------------------------------
				{
				if ( strmicmp( v, "o|oui|y|yes|true|vrai" ) )
					var_set( h, "enable", "Oui" );
				else
					var_set( h, "enable", "Non" );
					
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "ip" ) )
			//------------------------------------------------------------------
				{
				if ( ( v[0] )&&( ! is_ip_valide( v ) ) )
					{
					print( "ERREUR : ip non valide", _ );
					return ERROR;
					}

				var_set( h, "ip_ssie", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "ip_mft" ) )
			//------------------------------------------------------------------
				{
				if ( ( v[0] )&&( ! is_ip_valide( v ) ) )
					{
					print( "ERREUR : ip non valide", _ );
					return ERROR;
					}

				var_set( h, "ip", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "dns" ) )
			//------------------------------------------------------------------
				{
				if ( ( v[0] )&&( ! tcp_dns_address( v ) ) )
					{
					print( "ERREUR : dns non valide", _ );
					return ERROR;
					}

				var_set( h, "dns_ssie", v );
				if ( mft_exists() )
					var_set( h, "dns", v );
				
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "port" ) )
			//------------------------------------------------------------------
				{
				if ( ( v[0] )&&( ( !is_numeric( v ) )||( (p = atoi(v)) < 10 )||( p > 65535 ) ) )
					{
					print( "ERREUR : port non valide", _ );
					return ERROR;
					}
					
				var_set( h, "port", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "product_name" ) )
			//------------------------------------------------------------------
				{
				var_set( h, "product_name", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "descr" ) )
			//------------------------------------------------------------------
				{
				var_set( h, "descr", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "os" ) )
			//------------------------------------------------------------------
				{
				var_set( h, "os", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "max_tran" ) )
			//------------------------------------------------------------------
				{
				var_set( h, "max_tran", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "adl_liste" ) )
			//------------------------------------------------------------------
				{
				var_set( h, "adl_liste", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "no_maj_nat" ) )
			//------------------------------------------------------------------
				{
				var_set( h, "no_maj_nat", v );
				bModif = true;
				continue;
				}

			//------------------------------------------------------------------
			if ( ! strcmp( vn, "mft_gateway" ) )
			//------------------------------------------------------------------
				{
				if ( ! v[0] )
					var_delete( h, "mft_gateway" );
				else
				{
					var_set( h, "mft_gateway", v );
                } 
	            bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "repository" ) )
			//------------------------------------------------------------------
				{
				var_set( h, "repository", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "email" ) )
			//------------------------------------------------------------------
				{
				var_set( h, "email", v );
				bModif = true;
				continue;
				}
			
			
/*
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "rexec" ) )
			//------------------------------------------------------------------
				{
				if ( strcmp( v, "true" ) )
					{
					var_set( h, "rexec", v );
					bModif = true;
					continue;
					}
				
				// SIMULATION D'UN REXEC PAR CE PARTENAIRE
				
				hPart = h; h = 0;
				cmd_rexec_execute( t_work );
				return OK;
				}
*/
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "files_dir" ) )
			//------------------------------------------------------------------
				{
				if ( ! v[0] )
					v = strdcpy( &fname, dirdat, DIR_CHAR "files" DIR_CHAR, partname, _ );

/*
				mkdir( v );
				if ( ! dir_exists( v ) )
					{
					print( "ERREUR : le répertoire files_dir n'existe pas : ", v, _ );
					return ERROR;
					}
*/
				var_set( h, "files_dir", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "trf_direction" ) )
			//------------------------------------------------------------------
				{
				if ( ! v[0] )
					v = "both";

				if ( strmicmp( v, "send,recv,both" ) )
					{
					print( "ERREUR : trf_direction = send ou recv ou both", _ );
					return ERROR;
					}

				var_set( h, "trf_direction", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "ssl_enable" ) ) // Pour les partenaire MFT uniquement
			//------------------------------------------------------------------
				{
				var_set( h, "ssl_enable", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "mft_retry" ) ) // Pour les partenaire MFT uniquement
			//------------------------------------------------------------------
				{
				var_set( h, "mft_retry", v );
				bModif = true;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ( i == 2 )&&( ! strcmp( vn, "load" ) ) )
			//------------------------------------------------------------------
				{
				v = var_getn( hReq, 3, &vn );
				if ( ( !v )||( !file_exists( vn ) ) )
					{
					print( "ERREUR : Le fichier n'existe pas", _ );
					return ERROR;
					}
				
				var_free( h1 );
				h1 = var_load( vn );

				if ( v = var_get( h, "is_nat" ) )
                   var_set( h1, "is_nat", v );

				var_set( h1, "partname", partname );

				if ( v = var_get( h, "enable" ) )
					var_set( h1, "enable", v );
				else
					var_set( h1, "enable", "Oui" );
					
				if ( v = var_get( h1, "part_os" ) )
					{
					if ( ( v )&&( !stricmp( v, "Linux64" ) ) )
						var_set( h, "os", "Linux" );
					else
						var_set( h1, "os", v );
					}
				var_delete( h1, "part_os" );

				if ( v = var_get( h1, "part_mersi_version" ) )
					{
					if ( v[0] )
						strdcpy( &s, "mersi v", v, _ );
					else
						strdcpy( &s, "", _ );
					var_set( h1, "product_name", s );
					}
				var_delete( h1, "part_mersi_version" );

				if ( v = var_get( h1, "part_ip" ) )
					var_set( h1, "ip", v );	// IP MFT
				var_delete( h1, "part_ip" );

				if ( v = var_get( h1, "part_dns" ) )
					var_set( h1, "dns", v );
				var_delete( h1, "part_dns" );

				if ( v = var_get( h1, "part_adl_liste" ) )
					var_set( h1, "adl_liste", v );
				var_delete( h1, "part_adl_liste" );

				if ( !var_get( h1, "port" ) )
					{
					if ( v = var_get( h1, "part_port_mft" ) )
						var_set( h1, "port_mft", v );
					else
						var_set( h1, "port_mft", "1761" );
					var_delete( h1, "part_port_mft" );
					}

				if ( v = var_get( h, "files_dir" ) )
					var_set( h1, "files_dir", v );

				if ( v = var_get( h, "trf_direction" ) )
					var_set( h1, "trf_direction", v );
			
				if ( v = var_get( h, "mft_retry" ) )
					var_set( h1, "mft_retry", v );

				var_free( h );
				h = h1;
				h1 = 0;
				
				bModif = true;
				i++;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ( i == 2 )&&( ! strncmp( vn, "loadnat", 7 ) ) )
			//------------------------------------------------------------------
				{
				v = var_get( h, "no_maj_nat" );
				if ( ( v )&&( ( ! stricmp( v, "true" ) )||( ! stricmp( v, "Oui" ) )||( ! stricmp( v, "1" ) ) ) ) return OK;

				// CHARGE LES INFO D'UN SERVEUR NAT
				if ( nat_request( &s1, strdcpy( &s, "part ", partname, " get", _ ) ) )
					{
					logw( "part ", partname, " ", vn, _ );

					if ( ( strpos( s1, "partname", 0 ) < 0 )&&( strpos( s1, "part_name", 0 ) < 0 ) )
						{
						print( "ERREUR : partenaire inconnu du national." BR, _ );
						return ERROR;
						}

					var_free( h1 );
					h1 = var_from_str( 0, s1 );
					
					partname = var_get( h1, "partname" );
					
					if ( !partname )
						partname = var_get( h1, "part_name" );
						
					if ( v = var_get( h, "enable" ) )
						var_set( h1, "enable", v );
					else
						var_set( h1, "enable", "Oui" );
						
					if ( v = var_get( h1, "os" ) )
						{
						if ( ( v )&&( !stricmp( v, "Linux64" ) ) )
							var_set( h, "os", "Linux" );
						else
							var_set( h1, "os", v );
						}
	
					if ( v = var_get( h, "files_dir" ) )
						var_set( h1, "files_dir", v );
	
					if ( v = var_get( h, "trf_direction" ) )
						var_set( h1, "trf_direction", v );
				
					if ( v = var_get( h, "mft_retry" ) )
						var_set( h1, "mft_retry", v );
	
					if ( v = var_get( h, "mft_gateway" ) )
						var_set( h1, "mft_gateway", v );

					var_set( h1, "is_nat", "Oui" );
					
					var_free( h );
					h = h1;
					h1 = 0;
						
					bModif = true;
					continue;
					}
				else
					{
					print( "ERREUR : Pb connexion à un serveur d'annuaire" BR, _ );
					return ERROR;
					}
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "delete" ) )
			//------------------------------------------------------------------
				{
				strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR "ip" DIR_CHAR,
					var_get( h, "ip_ssie" ), "_", var_get( h, "port" ), ".ipp", _ );
				unlink( fname );
				unlink( part_fname );
				if ( mft_exists() )
					{
					strdcpy( &cmd, mft_exe(), " confpart ", partname, " delete", _ );
					System( &s, cmd );
					}
				print( "OK", _ );
				return OK;
				}
			
			
			//------------------------------------------------------------------
			if (( !stricmp( vn, "ssl" ) )||( !stricmp( vn, "ssl_config" ) ))
			//------------------------------------------------------------------
				{
				bModif = true;
				
				strdcpy( &s1, "", _ );

				// CHARGE ET VERIFIE LA CONFIG SSL FOURNIE
				
				strdcpy( &s2, dirconf, DIR_CHAR "certif_lib", _ );
				if ( ssl_certif_load( h, v, s2, "ssl" ) == ERROR )
					{
					strdcpy( &s1, error_msg, BR, _ );
					print( s1, BR, _ );
					}

			
				// CONSTRUIT LE LIBELLE A AFFICHER
				
				v = var_get( h, "ssl_certif_server" );
				if ( v )
					{
					v = var_get( h, "ssl_certif_server_to" );
					strdcat( &s1, var_get( h, "ssl_certif_server_enddate" ), " : ", v, BR, _ );
					for( n = 1 ; ; n++ )
						{
						strdcpy( &s2, "ssl_certif_ca", itoa(n), _ );
						if ( !var_get( h, s2 ) ) break;
						strdcpy( &s2, "ssl_certif_ca", itoa(n), "_to", _ );
						v = var_get( h, s2 );
						if ( !v ) break;
						strdcpy( &s2, "ssl_certif_ca", itoa(n), "_enddate", _ );
						strdcat( &s1, var_get( h, s2 ), " : ", v, BR, _ );
						}
					}
				else
				if ( v = var_get( h, "ssl_certif_ca1" ) )
					{
					v = var_get( h, "ssl_certif_ca1_to" );
					strdcat( &s1, var_get( h, "ssl_certif_ca1_enddate" ), " : ", v, BR, _ );
					}
				else
					{
					v = var_get( h, "product_name" );
					if (( v )&&( stripos( v, "ssie v7", 0 ) >= 0 ))
						strdcat( &s1, "La configuration SSL n'est pas valide", BR, _ );
					}

				v = s1;
				
				var_set( h, "ssl_config", v );
					
				continue;
				}
			
			//------------------------------------------------------------------
			
			if ( strmicmp( vn, "ip|descr|dns|adl_liste|port_mersi|port_mft|port_ssl|port_ssl2|"
							"port_shttp|mft_relai|mft_key|mft_key_type|mft_npart|no_maj_nat|"
							"os|network|is_onr|mft_version|mft_about|mft_maxtran|mersi_version|"
							"direction|mft_ssl_authorized|ssl_enable|ssl_certifu|ssl_certifr|"
							"ssl_certifi1|ssl_certifi2|ssl_certifi3|ssl_certifi4|ftp_ssl_certifu|"
							"ftp_ssl_certifr|ftp_ssl_certifi1|ftp_ssl_certifi2|ftp_ssl_certifi3|"
							"ftp_ssl_certifi4|port_ftp|sftp_sshkey|ftp_type|of|mft_retry|rescue_part" ) == false )
				{			
				print( "ERREUR : le paramètre ", vn, " est inconnu (cmd_part)." BR, _ );
				return ERROR;
				}
			}
		
			
		//----------------------------------------------------------------------
		if ( bModif )
		//----------------------------------------------------------------------
			{
			if ( ! is_alphanum( partname, "_" ) )
				{
				print( "ERREUR : Le nom de partenaire doit être alphanumérique", _ );
				return ERROR;
				}
			
			if ( ! stricmp( partname, "list" ) )
				{
				print( "ERREUR : Le nom de partenaire invalide", _ );
				return ERROR;
				}
			
			var_set( h, "partname", partname );
			partname = var_get( h, "partname" );
			
			strdcpy( &part_fname, dirconf, DIR_CHAR "part" DIR_CHAR, upper(partname), ".dat", _ );
				
			// VERIFIE LA VALIDITE DE TOUTES LES INFO
			
			var_delete( h, "error_msg" );
			for( b = false ; ; )
				{
				v = var_get( h, "ip_ssie" );
				if ( !v )
					{
					v = var_get( h, "dns_ssie" );
					if ( !v )
						{
						v = var_get( h, "dns" );
						if ( !v )
							{
							v = var_get( h, "ip" );
							if ( !v )
								{
								var_set( h, "error_msg", "ERREUR: ip ou dns obligatoire" );
								break;
								}
							}
						}
					}

				
				v = var_get( h, "product_name" );

				if ( ( !v )||( strnicmp( v, "ssie", 4 ) ) )
					{
					v = var_get( h, "ip_ssie" );
					if ( v )
						{
						var_set( h, "ip", v );
						var_delete( h, "ip_ssie" );
						}
					}
				else
					{	
					v = var_get( h, "port" );
					if ( !v )
						{
						var_set( h, "error_msg", "ERREUR: port obligatoire" );
						break;
						}
					}
				
				if ( var_get( h, "ip_ssie" ) )
					{ // uniquement pour les ssie v7
					v = var_get( h, "ssl_config_ok" );
					if ( !v )
						{
						var_set( h, "error_msg", "ERREUR: config SSL valide obligatoire" );
						break;
						}
					}
					
				b = true;
				break;
				}
			
			v = var_get( h, "enable" );
			if ( ( !v )||( strcmp( v, "Non" ) ) )
				var_set( h, "enable", "Oui" );

			var_save( h, part_fname );
			
			// INDEXATION PAR ADL (POUR MERSI_EMISSION)
			
			strdcpy( &fname1, dirconf, DIR_CHAR "part" DIR_CHAR "adl", _ );
			mkdir( fname1 );
			v = var_get( h, "adl_liste" );
			if ( ( v )&&( v[0] ) )
				{
				ts = str_explode( v, "," );
				for( i = 0 ; ts[i] ; i++ )
					{
					if ( strlen( ts[i] ) != 8 ) continue;
					strdcpy( &fname, fname1, DIR_CHAR, ts[i], ".dat", _ );
					file_save_str( fname, partname, -1 );
					}
				}
			
			// MISE A JOUR DU PARAMETRAGE MFT
			
			if ( ( b )&&( mft_exists() )
//				&&( v = var_get( h, "port_mft" ) )&&( v[0] )
				)
				{
				strdcpy( &cmd, mft_exe(), " confpart ", partname, _ );
				System( &s, cmd );
				if ( stripos( s, "error", 0 ) >= 0 )
					{
					var_set( h, "error_msg", s );
					var_save( h, part_fname );
					}
				}
			
			strd_replace( &part_fname, ".dat", ".send" );
			unlink( part_fname );
			v = var_get( h, "trf_direction" );
			if ( ( v )&&( !strcmp( v, "send" ) ) )
				file_save_str( part_fname, "", 0 );
			
			print( "OK", _ );
			return OK;
			}
		
		
		
		//----------------------------------------------------------------------
		// AFFICHE LES INFO DU PARTENAIRE DEMANDE
		//----------------------------------------------------------------------
		
		if ( var_get( h, "ssl_config_ok" ) )
			{
			n = ssl_end_delay( h, "ssl" );
			if ( n < (30 Jours) )
				{
				strdcpy( &s, "ATTENTION : La date de validité de ce certificat "
						"arrive à terme dans ", itoa( n/(1 Jour) ), " jour",
						(char*)(((n/(1 Jour))>1)?"s":""), BR, _ );
				var_set( h, "error_msg", s );
				}
			}
		
		
		if ( file_exists( part_fname ) )
			{
			if ( ! bModeJS )
				{
				if ( var_get( h, "ip_ssie" ) )
					print_ind( 3, 80,
						"partname      = ", var_get( h, "partname" ), BR,	
						"os            = ", var_get( h, "os" ), BR,
						"enable        = ", var_get( h, "enable" ), BR,
						"ip            = ", var_get( h, "ip_ssie" ), BR,	
						"dns           = ", var_get( h, "dns_ssie" ), BR,	
						"port          = ", var_get( h, "port" ), BR,	
//						"email         = ", var_get( h, "email" ), BR,	
						"product_name  = ", var_get( h, "product_name" ), BR,	
						"adl_liste     = ", var_get( h, "adl_liste" ), BR,
						"trf_direction = ", var_get( h, "trf_direction" ),
						_ );
				else
					print_ind( 3, 80,
						"partname      = ", var_get( h, "partname" ), BR,	
						"os            = ", var_get( h, "os" ), BR,
						"enable        = ", var_get( h, "enable" ), BR,
						"ip_mft        = ", var_get( h, "ip" ), BR,	
						"dns           = ", var_get( h, "dns" ), BR,	
						"port_mft      = ", (char*)((v=var_get( h, "port_mft" ))?v:(char*)((v=var_get( h, "port" ))?v:"1761")), BR,	
						"product_name  = ", var_get( h, "product_name" ), BR,	
						"adl_liste     = ", var_get( h, "adl_liste" ), BR,
						"trf_direction = ", var_get( h, "trf_direction" ),
						_ );

				v = var_get( h, "mft_gateway" );
				if ( ( v )&&( v[0] ) )
					print_ind( 3, 80, "mft_gateway   = ", v, _ );
				
				v = var_get( h, "max_tran" );
				if ( ( v )&&( v[0] ) )
					print_ind( 3, 80, "max_tran      = ", v, _ );
				
				v = var_get( h, "no_maj_nat" );
				if ( ( v )&&( ( !stricmp( v, "true" ) )||( !stricmp( v, "Oui" ) )||( !stricmp( v, "1" ) ) ) )
					print_ind( 3, 80, "no_maj_nat    = Oui", _ );

				print_ind( 3, 80, "ssl_config    = ", BR, _ );
				print_ind( 6, 80, var_get( h, "ssl_config" ), BR, _ );
				print_ind( 3, 80, var_get( h, "error_msg" ), BR, _ );
				}
			else
				print( "{", var_to_js_array( h, &s ), "}", _ );
			
			return OK;
			}
		}

	//--------------------------------------------------------------------------
	// AFFICHE LA LISTE DES PARTENAIRES
	//--------------------------------------------------------------------------
		
	n = 0;
	tf = NULL;
	
	strdcpy( &fname1, dirconf, DIR_CHAR "part" DIR_CHAR "adl", _ );
	if ( !dir_exists( fname1 ) )
		{
		mkdir( fname1 );
		bIndexADL = true;
		}
	else
		bIndexADL = false;
	
	
	if ( !bModeJS )
		print( " partname      | Actif | ip              | port | product", BR BR, _ );
	else
		print( "[", _ );

	if (( partname )&&
			(( !stricmp( partname, "of" ) )||( !stricmp( partname, "list" ) ))
		)
		partname[0] = 0;


	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, "*", _ );
	while( tf = file_list( tf, fname, "nodir" ) )
		{
		if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;

		if ( h ) var_free( h );
		h = var_load( strdcpy( &fname, dirconf, "/part/", tf->name, _ ) );
		p = strpos( tf->name, ".", 0 );
		tf->name[p] = 0;

		if ( var_get( h, "ip_ssie" ) )
			strdcpy( &s, " ", strpad( tf->name, 14, " " ), "| ",
				strpad( var_get( h, "enable" ), 6, " " ), "| ",
				strpad( substr(var_get( h, "ip_ssie" ),0, 16), 16, " " ), "| ",
				strpad( var_get( h, "port" ), 5, " " ), "| ", var_get( h, "product_name" ),
				BR, _ );
		else
			strdcpy( &s, " ", strpad( tf->name, 14, " " ), "| ",
				strpad( var_get( h, "enable" ), 6, " " ), "| ",
				strpad( substr(var_get( h, "ip" ),0, 16), 16, " " ), "| ",
				strpad( var_get( h, "port_mft" ), 5, " " ), "| ", var_get( h, "product_name" ),
				BR, _ );

		if ( ( partname )&&( stripos( s, partname, 0 ) < 0 ) ) continue;

		// INDEXATION PAR ADL (POUR MERSI_EMISSION)
		if ( bIndexADL )
			{
			v = var_get( h, "adl_liste" );
			if ( ( v )&&( v[0] ) )
				{
				ts = str_explode( v, "," );
				for( i = 0 ; ts[i] ; i++ )
					{
					if ( strlen( ts[i] ) != 8 ) continue;
					strdcpy( &fname, fname1, DIR_CHAR, ts[i], ".dat", _ );
					file_save_str( fname, tf->name, -1 );
					}
				}
			}
		

		// AFFICHAGE
		if ( ! bModeJS )
			{
			print( s, _ );
			}
		else
			{
			if ( var_get( h, "ip_ssie" ) )
				print( (char*)((n)?",[":"["), quote(tf->name),
					",", quote(var_get( h, "ip_ssie" )),
					",", quote(var_get( h, "os" )),
					",", quote(var_get( h, "dns_ssie" )),
					",'',", quote(var_get( h, "product_name" )),
					"]", _ );
			else
				print( (char*)((n)?",[":"["), quote(tf->name),
					",", quote(var_get( h, "ip" )),
					",", quote(var_get( h, "os" )),
					",", quote(var_get( h, "dns" )),
					",'',", quote(var_get( h, "product_name" )),
					"]", _ );
			}
		n++;
//		h = 0;
		}
		
	if ( ! bModeJS )
		print( BR, " ", itoa(n), " partenaire", (char*)((n>1)?"s":""), BR, _ );	
	else
		print( "]", _ );
	
	if ( h ) var_free( h );
	h = 0;
	
	//--------------------------------------------------------------------------

	return OK;
	}


#undef part_fname


















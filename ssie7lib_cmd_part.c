//#include "ssie7lib.h"


// GESTION DES LOGINS POUR AVOIR LES DROITS DE TRANSFERER DES FICHIERS AVEC LE SERVEUR

#define part_fname			fname3
#define bModif				b
#define bModeJS				b1

//------------------------------------------------------------------------------
int cmd_part( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
//	if ( send_private_port( t_work ) ) return OK;
	
	
	bModeJS = false; // Affichage type écran
	v = var_get( hReq, "of" );
	if ( ( v )&&( !strcmp( v, "js" ) ) ) bModeJS = true; // Affichage sous forme de tableau javascript

	//--------------------------------------------------------------------------
	// LECTURE DU PARTNAME
	//--------------------------------------------------------------------------
	
	partname = null;
	v = var_getn( hReq, 1, &partname );
	if (( v )&&( strcmp( partname, "of" ) ))
		{
		strdcpy( &part_fname, dirconf, DIR_CHAR "part" DIR_CHAR, upper(partname), ".dat", _ );

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
			if ( ! strcmp( vn, "port" ) )
			//------------------------------------------------------------------
				{
				if ( ( !is_numeric( v ) )||( (p = atoi(v)) < 1 )||( p > 65535 ) )
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
			if ( ! strcmp( vn, "os" ) )
			//------------------------------------------------------------------
				{
				var_set( h, "os", v );
				bModif = true;
				continue;
				}

/*			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "repository" ) )
			//------------------------------------------------------------------
				{
				var_set( h, "repository", v );
				bModif = true;
				continue;
				}
*/
			
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
				
				// SIMULATION D'UN REXEC
				
				cmd_rexec_execute( t_work );
				return OK;
				}
*/

/*			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "files_dir" ) )
			//------------------------------------------------------------------
				{
				if ( ! v[0] )
					v = strdcpy( &fname, dirdat, DIR_CHAR "files" DIR_CHAR, partname, _ );

				mkdir( v );
				if ( ! dir_exists( v ) )
					{
					print( "ERREUR : le répertoire files_dir n'existe pas : ", v, _ );
					return ERROR;
					}

				var_set( h, "files_dir", v );
				bModif = true;
				continue;
				}
*/
			
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
				
				if ( h1 ) var_free( h1 );
				h1 = var_load( vn );
				if ( v = var_get( h, "enable" ) )
					var_set( h1, "enable", v );
				else
					var_set( h1, "enable", "Oui" );
				
				var_set( h1, "files_dir", var_get( h, "files_dir" ) );
				
				var_free( h );
				h = h1;
				
				bModif = true;
				i++;
				continue;
				}
			
			//------------------------------------------------------------------
			if ( ! strcmp( vn, "delete" ) )
			//------------------------------------------------------------------
				{
				unlink( part_fname );
				print( "OK", __ );
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
				if ( ssl_certif_load( h, v, s2 ) == ERROR )
					{
/*
					if ( ( ! var_get( h, "ssl_certif_server" ) )
						||( !dir_exists( s2 ) )
						||( ssl_certif_load( h, s2 ) == ERROR )
						)
*/
						strdcpy( &s1, error_msg, BR, _ );
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
					strdcat( &s1, "La configuration SSL n'est pas valide", BR, _ );

				v = s1;
				
				var_set( h, "ssl_config", v );
					
				continue;
				}
			
			//------------------------------------------------------------------
			
			print( "ERREUR : le paramètre ", vn, " est inconnu." BR, __ );
			return ERROR;
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
			
/*
			if ( ( ! (v=var_get( h, "files_dir" )) )||( ! (trim(v)[0]) ) )
				{
				strdcpy( &fname, dirdat, DIR_CHAR "files" DIR_CHAR, partname, _ );
				mkdir( fname );
				var_set( h, "files_dir", fname );
				}
*/
				
			// VERIFIE LA VALIDITE DE TOUTES LES INFO
			
			var_delete( h, "error_msg" );
			for( b = false ; ; )
				{
				v = var_get( h, "ip_ssie" );
				if ( !v )
					{
					var_set( h, "error_msg", "ERREUR: ip obligatoire" );
					break;
					}
				v = var_get( h, "port" );
				if ( !v )
					{
					var_set( h, "error_msg", "ERREUR: port obligatoire" );
					break;
					}
				v = var_get( h, "ssl_config_ok" );
				if ( !v )
					{
					var_set( h, "error_msg", "ERREUR: config SSL valide obligatoire" );
					break;
					}
/*
				v = var_get( h, "files_dir" );
				if ( ( !v )||( ! dir_exists( v ) ) )
					{
					var_set( h, "error_msg", "ERREUR: répertoire files_dir n'existe pas" );
					break;
					}
*/
				b = true;
				break;
				}
			
/*
			if ( !b )
				var_set( h, "enable", "Non" );
			else
*/
				if ( !var_get( h, "enable" ) )
					var_set( h, "enable", "Oui" );

			
			
			var_save( h, part_fname );
			
/*
			// GENERE UN FICHIER AVEC IP_PORT pour les recherches par ip_port
			
			strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR,
				var_get( h, "ip" ), "_", var_get( h, "port" ), ".ipp", _ );
			unlink( fname );
			
			if ( ( !var_get( h, "error_msg" ) )&&( !strcmp( var_get( h, "enable" ), "Oui" ) ) )
				{
				var_set( h, "partname", partname );
				var_save( h, fname );
				}
*/				
			}
		
		
		
		//----------------------------------------------------------------------
		// AFFICHE LES INFO DU PARTENAIRE DEMANDE
		//----------------------------------------------------------------------
		
		if ( file_exists( part_fname ) )
			{
			if ( ! bModeJS )
				{
				print_ind( 3, 80,
					"os           = ", var_get( h, "os" ), BR,
					"enable       = ", var_get( h, "enable" ), BR,
					"ip           = ", var_get( h, "ip_ssie" ), BR,	
					"port         = ", var_get( h, "port" ), BR,	
					"product_name = ", var_get( h, "product_name" ), BR,	
//					"repository    = ", var_get( h, "repository" ), BR,
//					"rexec         = ", var_get( h, "rexec" ), BR,
//					"files_dir    = ", var_get( h, "files_dir" ), BR,
					"ssl_config   = ", BR,
					_ );

				n = ssl_end_delay( h );
				
				v = var_get( h, "ssl_config" );
				if ( ( v )&&( v[0] ) )
					{
					print_ind( 6, 80, v, _ );
					
					if ( ( var_get( h, "error_msg" ) )&&( n < (30 Jours) ) )
						print_ind( 6, 80, "ATTENTION : La date de validité de ce certificat "
							"arrive à terme dans ", itoa( n/(1 Jour) ), " jour",
							(char*)(((n/(1 Jour))>1)?"s":""), BR, _ );
					}
				
				print_ind( 3, 80, var_get( h, "error_msg" ), BR, _ );
				}
			else
				print( var_to_js_array( h, &s ), _ );
			
			print( __, __ );
			
			return OK;
			}
		}

	//--------------------------------------------------------------------------
	// AFFICHE LA LISTE DES PARTENAIRES
	//--------------------------------------------------------------------------
		
	n = 0;
	tf = NULL;
	
	if ( !bModeJS )
		print( " partname      | Actif | IP               | port", BR BR, _ );
	else
		print( "[", _ );

	if (( partname )&&( !strcmp( partname, "of" ) )) partname[0] = 0;

	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, partname, "*", _ );
	while( tf = file_list( tf, fname, "nodir" ) )
		{
		if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;

		if ( h ) var_free( h );
		h = var_load( strdcpy( &fname, dirconf, "/part/", tf->name, _ ) );
		p = strpos( tf->name, ".", 0 );
		tf->name[p] = 0;

		v = var_get( h, "ip_ssie" );
		if ( !v ) v = var_get( h, "ip_ssie" );
		if ( ! bModeJS )
			print( " ", strpad( tf->name, 14, " " ), "| ",
					strpad( var_get( h, "enable" ), 6, " " ), "| ",
					strpad( substr(v,0, 17), 17, " " ), "| ",
					var_get( h, "port" ),
					BR, _ );
		else
			print( (char*)((n)?",{":"{"), var_to_js_array( h, &s ), "}", _ );
		n++;
		var_free( h );
		h = 0;
		}
	if ( ! bModeJS )
		print( BR, " ", itoa(n), " partenaire", (char*)((n>1)?"s":""), BR, __ );	
	else
		print( "]", __ );
	
	
	//--------------------------------------------------------------------------

	return OK;
	}





















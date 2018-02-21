#include "ssie7.h"

//------------------------------------------------------------------------------
int cmd_flux( T_WORK *t_work )

// COMMANDE FLUX
//------------------------------------------------------------------------------
	{
	tr( "cmd_flux_1" );
	
	v = var_getn( hReq, 1, &vn );
	if ( ( v )&&( is_numeric( vn ) ) )
		{
		tr( "cmd_flux_2" );
		strdcpy( &flux_id, vn, _ );
		flux_exec_services( t_work );
		return;
		}
	
	if ( ( v )&&( ! strcmp( vn, "pki" ) ) )
		{
		tr( "cmd_flux_3" );
		v = var_getn( hReq, 2, &s );
		if ( ! v ) { print( "ERREUR (1)", _ ); return; }
		
		tr( "cmd_flux_4" );
		strdcpy( &fname, dirflux, DIR_CHAR, s, ".run", _ );
//		strdcpy( &fname, dirrflux, DIR_CHAR, s, _ );
		if ( ! file_exists( fname ) ) { print( "ERREUR (2)", _ ); return; }

		tr( "cmd_flux_5" );
		strdcpy( &fname, dirflux, DIR_CHAR, s, ".dat", _ );
		if ( ! file_exists( fname ) ) { print( "ERREUR (3):", fname, _ ); return; }
		
		tr( "cmd_flux_6" );
		if ( hConf ) var_free( hConf );
		hConf = var_load( fname );

		v = var_getn( hReq, 3, &s );
		if ( ! v ) { print( "ERREUR (4)", _ ); return; }

		tr( "cmd_flux_7" );
		v1 = var_get( hConf, strdcpy( &s1, "pki_", s, _ ) );
		if ( ! v1 ) { print( "ERREUR (5)", _ ); return; }
		
		tr( "cmd_flux_8" );
		print( v1, _ );
		return;
		}

	//--------------------------------------------------------------------------
	// RECHERCHE SI C'EST UNE MISE A JOUR DU FLUX
	//--------------------------------------------------------------------------
	
	tr( "cmd_flux_9" );
	
	b = false;
	for( i = var_count( hReq )-1 ; ( i > 0 )&&( v = var_getn( hReq, i, &vn ) ) ; i-- )
		{
		if ( strcmp( v, "true" ) )
			{
			if ( strmicmp( vn, "descr|actif|version|etape|type|dest_liste|adle|appe"
							"|appr|mft_options|appl_info|bin_files|txt_files|before_script"
							"|planif_enable|waitfile|waitack|sendack|restart|restart_onrecv"
							"|one_by_one|planif|ftp|ftp_dest|ftp_login|ftp_pw|ftp_prot|ftp_sshkey"
							"|ftp_cmd|emet_liste|start_cond|script|locked|lock_fname"
							"|ack_fname|wait_fname|timeout|horaire|isolate_files|flux_name"
							"|log|c|of|filter|date|status|max_item|name|one_by_one" ) == false )
				{
				print( "ERREUR : le paramètre ", vn, " est inconnu (cmd_flux)." BR, _ );
				tr( "cmd_flux_10" );
				return ERROR;
				}
			if ( ( i > 2 )&&( strcmp( vn, "of" ) )&&( strcmp( vn, "filter" ) )
					&&( strcmp( vn, "date" ) )&&( strcmp( vn, "version" ) )
					&&( strcmp( vn, "status" ) )&&( strcmp( vn, "max_item" ) )
					)
				b = true;
			}
		}
	
	//--------------------------------------------------------------------------
	// LECTURE DES PARAMETRES
	//--------------------------------------------------------------------------
	
	tr( "cmd_flux_11" );
	
	appname = null;
	fluxname = null;
		
	if (( b )||( var_get( hReq, "start" ) )) // Modification ou démarrage
		{
		tr( "cmd_flux_12" );
		v = var_getn( hReq, 1, &appname );
		if ( ( ! v )||( strcmp( v, "true" ) ) )
			{
			print( "ERREUR : syntaxe : flux {app_name} {fluxname} ...", BR, _ );
			return ERROR;
			}
		
		tr( "cmd_flux_13" );
		upper( appname );
		if ( !is_authorized( appname, "editeur" ) ) return ERROR;

		tr( "cmd_flux_14" );
		v = var_getn( hReq, 2, &fluxname );
		if ( ( ! v )||( strcmp( v, "true" ) ) )
			{
			print( "ERREUR : syntaxe : flux {app_name} {fluxname} ...", BR, _ );
			return ERROR;
			}
		tr( "cmd_flux_15" );
		
		upper( fluxname );
		
        for( i = strlen( fluxname ) - 1 ; i >=0 ; i-- )
            {
            if ( ( fluxname[i] >= 'A' )&&( fluxname[i] <= '_' ) ) continue;
            if ( ( fluxname[i] >= '0' )&&( fluxname[i] <= '9' ) ) continue;
            switch( fluxname[i] )
                {
                case ' ' :
                    fluxname[i] = '_';
                case '_' :
                case '-' :
                    break;
                case '/' :
                    fluxname[i] = '@';
                    break;
                default :
					strdcpy( &s, " ", _ );
					s[0] = fluxname[i];
					print( "ERREUR : Le nom du flux contient des caractères interdits : '", s, "'" BR, _ );
					var_print( hReq, ", " );
                    tr( "cmd_flux_16" );
					return ERROR;
                }
            }
		
		tr( "cmd_flux_17" );
		if ( strmicmp( fluxname, "start|startinside|stop|delete|create|version|up|down|"
							"descr|actif|version|etape|type|dest_liste|adle|appe"
							"|appr|mft_options|appl_info|bin_files|txt_files"
							"|planif_enable|waitfile|waitack|sendack|restart|one_by_one"
							"|planif|ftp|ftp_dest|ftp_login|ftp_pw|ftp_prot|ftp_sshkey"
							"|ftp_cmd|emet_liste|start_cond|script|locked|comment"
							"|ack_fname|wait_fname|timeout|horaire|isolate|flux_name"
							"|log|c|of|filter|date|name|duplique|pki|restore|noreport"
							"|desactive|insert|no_verify|encours|termine|execute" ) )
			{
			print( "ERREUR : Ce nom de flux n'est pas permis" BR, _ );
			return ERROR;
			}
		
		tr( "cmd_flux_18" );
		strdcpy( &fname, dircflux, "/", appname, "_flux_", fluxname, ".dat", _ );
		
		if (hConf ) var_free( hConf );
		hConf = var_load( fname );

		v = var_getn( hReq, 3, &s );
		n = 0;
		if ( ( v )&&( !strcmp( v, "true" ) )&&( is_numeric( s ) ) )
			n = atoi( s );

		if ( !n )
			{
			tr( "cmd_flux_19" );
			
			//------------------------------------------------------------------
			// CREATION D'UN NOUVEAU FLUX
			//------------------------------------------------------------------
			
			if ( var_get( hReq, "create" ) )
				{
				tr( "cmd_flux_20" );
				if ( ! is_authorized( appname, "gestionnaire" ) ) return ERROR;

				tr( "cmd_flux_21" );
				if ( file_exists(
						strdcpy( &s, dircflux, "/", appname, "_flux_",
									fluxname, ".dat", _ ) ) )
					{
					print( "ERREUR : Un flux de même nom existe déjà", _ );
					return;
					}
				}
			else
			
			//------------------------------------------------------------------
			// MODIFICATION DES INFORMATIONS GENERALES D'UN FLUX
			//------------------------------------------------------------------
			
			
			if ( ! file_exists( fname ) )
				{
				tr( "cmd_flux_22" );
				print( "ERREUR : Ce flux n'existe pas" BR, _ );
				return ERROR;
				}
		
			b3 = false;
			if ( ( v = var_get( hConf, "flux_locked" ) )
				&&( !strcmp( v, "Oui" ) ) )
				b3 = true;
				
			if ( v = var_get( hReq, "descr" ) )
				var_set( hConf, "flux_descr", v );
			if ( v = var_get( hReq, "desactive" ) )
				{
				if ( strmicmp( v, "o|oui|y|yes|true|vrai" ) )
					var_set( hConf, "flux_disabled", "Oui" );
				else
					var_set( hConf, "flux_disabled", "Non" );
				}
			if ( v = var_get( hReq, "restart" ) )
				{
				if ( strmicmp( v, "o|oui|y|yes|true|vrai" ) )
					var_set( hConf, "flux_restart", "10" );
				else
				if ( is_numeric( v ) )
					var_set( hConf, "flux_restart", v );
				else
					var_set( hConf, "flux_restart", "Non" );
				}
			if ( v = var_get( hReq, "locked" ) )
				{
				if ( strmicmp( v, "o|oui|y|yes|true|vrai" ) )
					var_set( hConf, "flux_locked", "Oui" );
				else
					{
					var_set( hConf, "flux_locked", "Non" );
					b3 = false;
					}
				}

			tr( "cmd_flux_23" );
			
			if ( v = upper(var_get( hReq, "name" )) )
				{
				tr( "cmd_flux_24" );
				if ( b3 )
					{
					print( "ERREUR : Vous ne pouvez pas modifier un flux verrouillé." BR, _ );
					return ERROR;
					}
				
				tr( "cmd_flux_25" );
				unlink( fname );
		        for( i = strlen( v ) - 1 ; i >=0 ; i-- )
		            {
		            if ( ( v[i] >= 'A' )&&( v[i] <= '_' ) ) continue;
		            if ( ( v[i] >= '0' )&&( v[i] <= '9' ) ) continue;
		            switch( v[i] )
		                {
		                case ' ' :
		                    v[i] = '_';
		                case '_' :
		                case '-' :
		                    break;
		                case '/' :
		                    v[i] = '@';
		                    break;
		                default :
		                    tr( "cmd_flux_26" );
							print( "ERREUR : Le nom du flux contient des caractères interdits" BR, _ );
		                    return ERROR;
		                }
		            }


/*
				strdcpy( &fname1, dirdat, "/flux/", appname, "_", fluxname, ".idx", _ );
				strdcpy( &fname2, dirdat, "/flux/", appname, "_", v, ".idx", _ );
				file_move( fname1, fname2 );
*/
				n = 0;
				tf = NULL;
				while( tf = file_list( tf, dircflux, "nodir" ) )
					{
					tr( "cmd_flux_27" );
					if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;
					if ( ( appname )&&( strpos( tf->name, appname, 0 ) < 0 ) ) continue;
					if ( ( p = strpos( tf->name, "_flux_", 0 ) ) < 0 ) continue;
					if ( ( p1=strpos( tf->name, "_etape_", p ) ) < 0 ) continue;
					if ( ( p2 = strpos( tf->name, fluxname, p ) ) < 0 ) continue;
					if ( p2 + strlen( fluxname ) != p1 ) continue;

					tr( "cmd_flux_28" );
					strdcpy( &fname1, dircflux, "/", tf->name, _ );
					strdcpy( &fname2, dircflux, "/", appname, "_flux_", v, substr( tf->name, p1, -1 ), _ );
//					file_move( fname1, fname2 );
					if ( file_move( fname1, fname2 ) == ERROR )
						logw( "flux execute : ", error_msg, _ );
					}
				fluxname = v;
				strdcpy( &fname, dircflux, "/", appname, "_flux_", fluxname, ".dat", _ );
				}

			//------------------------------------------------------------------
			// SAUVEGARDE DU FLUX
			//------------------------------------------------------------------
			
			tr( "cmd_flux_29" );
			
			if ( ! var_get( hReq, "start" ) )
				{
				tr( "cmd_flux_30" );
				if ( !is_authorized( appname, "gestionnaire" ) ) return ERROR;
				
				tr( "cmd_flux_31" );
				if ( v1 = var_get( hReq, "version" ) )
					{
					tr( "cmd_flux_32" );
					v2 = upper(var_get( hConf, "flux_version" ));
					if ( !v2 ) v2 = "1";
					if ( strcmp( v1, v2 ) )
						{
						tr( "cmd_flux_33" );
						// CHANGEMENT DE VERSION
						
						if ( file_exists( strdcpy( &fname1, dircflux, "/", appname,
								"_flux", v1, "_", fluxname, ".dat", _ ) ) )
							{
							print( "ERREUR : Vous ne pouvez pas modifier une version archivée.", BR, _ );
							return ERROR;
							}

						tr( "cmd_flux_34" );
						// SUPPRIME UNE EVENTUELLE VERSION IDENTIQUE
						
						file_delete(
							strdcpy( &s, dircflux, "/", appname, "_flux", v2, "_", fluxname, ".dat", _ ) );
						file_delete(
							strdcpy( &s, dircflux, "/", appname, "_flux", v2, "_", fluxname, "_etape_*", _ ) );
						
						// SAUVEGARDE LA VERSION ACTUELLE
						
						strdcpy( &fname1, dircflux, "/", appname, "_flux", v2, "_", fluxname, ".dat", _ );
						file_copy( fname, fname1 );

						for( i = 1 ; file_exists( strdcpy( &fname1,
										dircflux, "/", appname, "_flux_", fluxname,
										"_etape_", itoa(i), ".dat", _ ) ); i++ )
							{
							strdcpy( &fname2, dircflux, "/", appname, "_flux", v2, "_", fluxname,
										"_etape_", itoa(i), ".dat", _ );
							file_copy( fname1, fname2 );
							}
								
						var_set( hConf, "flux_version", v1 );
						var_set( hConf, "flux_locked", "Non" );
						b3 = false;
						}
					}
					
				tr( "cmd_flux_35" );
				if ( b3 )
					{
					print( "ERREUR : Vous ne pouvez pas modifier un flux verrouillé." BR, _ );
					return ERROR;
					}
				
				tr( "cmd_flux_36" );
				var_save( hConf, fname );
				}
			}
		else
			{
			tr( "cmd_flux_37" );
			
			//------------------------------------------------------------------
			// MODIFICATION D'UNE ETAPE
			//------------------------------------------------------------------
			
			if ( !is_authorized( appname, "gestionnaire" ) ) return ERROR;
			
			if ( ( v = var_get( hConf, "flux_locked" ) )
				&&( !strcmp( v, "Oui" ) ) )
				{
				print( "ERREUR : Vous ne pouvez pas modifier un flux verrouillé.", BR, _ );
				return ERROR;
				}
			
			v1 = var_get( hReq, "version" );
			if ( v1 )
				{
				v2 = var_get( hConf, "flux_version" );
				if ( !v2 ) v2 = "1";
				if ( !strcmp( v1, v2 ) )
					v1 = null;
				}
			if ( v1 )
				{
				print( "ERREUR : Vous ne pouvez pas modifier une version archivée.", BR, _ );
				return ERROR;
				}
			
			if ( var_get( hReq, "insert" ) )
				{
				for( i = 1000 ; i >= n ; i-- )
					{
					strdcpy( &fname1, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(i), ".dat", _ );
					if ( ! file_exists( fname1 ) ) continue;
					strdcpy( &fname2, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(i+1), ".dat", _ );
//					file_move( fname1, fname2 );
					if ( file_move( fname1, fname2 ) == ERROR )
						logw( "flux insert : ", error_msg, _ );
					}
				}
				
			strdcpy( &fname1, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(n), ".dat", _ );
			if ( hConf1 ) var_free( hConf1 );
			hConf1 = var_load( fname1 );
			
			if ( v = var_get( hReq, "descr" ) )
				var_set( hConf1, "etape_descr", v );
			if ( v = var_get( hReq, "type" ) )
				{
				if ( ! strmicmp( v, "emi|rec|ftp|script|sendack|waitack|waitend|on_error|start|startinside|waitfile|planif" ) )
					{
					print( "ERREUR : le type de l'étape n'est pas connu : ", v, BR, _ );
					return ERROR;
					}
					
				var_set( hConf1, "etape_type", lower(v) );
				}
			else
				v = var_get( hConf1, "etape_type" );

			tr( "cmd_flux_38" );
			
			//------------------------------------------------------------------
			if ( ! strcmp( v, "emi" ) )
			//------------------------------------------------------------------
				{
				tr( "cmd_flux_39" );
				if ( v = var_get( hReq, "dest_liste" ) )
					var_set( hConf1, "age_dest_liste", v );
				if ( v = var_get( hReq, "adle" ) )
					var_set( hConf1, "age_adle", v );
				if ( v = var_get( hReq, "appe" ) )
					var_set( hConf1, "age_appe", v );
				if ( v = var_get( hReq, "appr" ) )
					var_set( hConf1, "age_appr", v );
				if ( v = var_get( hReq, "mft_options" ) )
					var_set( hConf1, "age_mft_options", v );
				if ( v = var_get( hReq, "appl_info" ) )
					var_set( hConf1, "age_appl_info", v );
				if ( v = var_get( hReq, "bin_files" ) )
					var_set( hConf1, "age_bin_files", v );
				if ( v = var_get( hReq, "txt_files" ) )
					var_set( hConf1, "age_txt_files", v );
				if ( v = var_get( hReq, "before_script" ) )
					{
					if ( v[0] == '@' )
						{
						v1 = file_load( &s, &(v[1]) );
						if ( !v1 )
							{
							print( "ERREUR : age_before_script : lecture du fichier ", &(v[1]), BR, _ );
							return ERROR;
							}
						v = v1;
						}
					var_set( hConf1, "age_before_script", v );
					}
				if ( v = var_get( hReq, "script" ) )
					{
					if ( v[0] == '@' )
						{
						v1 = file_load( &s, &(v[1]) );
						if ( !v1 )
							{
							print( "ERREUR : age_script : lecture du fichier ", &(v[1]), BR, _ );
							return ERROR;
							}
						v = v1;
						}
					var_set( hConf1, "age_script", v );
					}
				}
			else
			//------------------------------------------------------------------
			if ( ! strcmp( v, "rec" ) )
			//------------------------------------------------------------------
				{
				tr( "cmd_flux_40" );
				if ( v = var_get( hReq, "emet_liste" ) )
					var_set( hConf1, "agr_emet_liste", v );
				if ( v = var_get( hReq, "start_cond" ) )
					var_set( hConf1, "agr_start_cond", v );
				if ( v = var_get( hReq, "dest_name" ) )
					var_set( hConf1, "agr_dest_name", v );
				if ( v = var_get( hReq, "horaire" ) )
					var_set( hConf1, "agr_horaire", v );
				if ( v = var_get( hReq, "script" ) )
					{
					if ( v[0] == '@' )
						{
						v1 = file_load( &s, &(v[1]) );
						if ( !v1 )
							{
							print( "ERREUR : age_script : lecture du fichier ", &(v[1]), BR, _ );
							return ERROR;
							}
						v = v1;
						}
					var_set( hConf1, "agr_script", v );
					}
				if ( v = var_get( hReq, "one_by_one" ) )
					{
					if ( strmicmp( v, "o|oui|y|yes|true|vrai" ) )
						var_set( hConf1, "agr_one_by_one", "Oui" );
					else
						var_set( hConf1, "agr_one_by_one", "Non" );
					}
				}
			else
			//------------------------------------------------------------------
			if ( ! strcmp( v, "ftp" ) )
			//------------------------------------------------------------------
				{
				if ( v = var_get( hReq, "ftp_dest" ) )
					var_set( hConf1, "ftp_dest", v );
				if ( v = var_get( hReq, "ftp_login" ) )
					var_set( hConf1, "ftp_login", v );
				if ( v = var_get( hReq, "ftp_pw" ) )
					var_set( hConf1, "ftp_pw", v );
				if ( v = var_get( hReq, "ftp_prot" ) )
					var_set( hConf1, "ftp_prot", v );
				if ( v = var_get( hReq, "ftp_sshkey" ) )
					var_set( hConf1, "ftp_sshkey", v );
				if ( v = var_get( hReq, "ftp_cmd" ) )
					var_set( hConf1, "ftp_cmd", v );
				}
			else
			//------------------------------------------------------------------
			if ( ! strcmp( v, "script" ) )
			//------------------------------------------------------------------
				{
				tr( "cmd_flux_41" );
				if ( v = var_get( hReq, "script" ) )
					{
					tr( "cmd_flux_41.1" );
					if ( v[0] == '@' )
						{
						v1 = file_load( &s1, &(v[1]) );
						if ( !v1 )
							{
							print( "ERREUR : script : lecture du fichier ", &(v[1]), BR, _ );
							return ERROR;
							}
						v = v1;
						}
					
					tr( "cmd_flux_41.1" );
					
					// RECHERCHE S'IL Y A DES PKI
					p0 = 0;
					strdcpy( &s, "", _ );
					while( ( p = stripos( v, "pki ", p0 ) ) >= 0 )
						{
						tr( "cmd_flux_41.2" );
						if ( ( p )&&( v[p-1] != '\n' ) )
							{
							p0 = p+1;	continue;
							}
						tr( "cmd_flux_41.3" );
						p3 = strpos( v, "\n", p+4 );
						if ( p3 < 0 ) break;
						p2 = strpos( v, "=", p+4 );
						if ( ( p2 < 0 )||( p2 >= p3 ) )
							{ p0 = p3; continue; }
						
						v1 = trim( substr( v, p+4, p2 - p - 4 ) );
						if ( v1[0] == 0 )
							{ p0 = p3; continue; }
						
						v2 = trim( substr( v, p2+1, p3 - p2 - 1 ) );
						if ( v2[0] == 0 )
							{ p0 = p3; continue; }
							
						if ( ! strcmp( v2, "########" ) )
							{ p0 = p3; continue; }
						
						strdncat( &s, &(v[p0]), p - p0, _ );
						strdcat( &s, "pki ", v1, "=########", _ );
						p0 = p3;

						var_set( hConf, strdcpy(&s1, "pki_", v1, _ ), v2 );
						
						v3 = var_get( hConf, "pki_list" );
						if ( !v3 )
							{
							var_set( hConf, "pki_list", strdcpy( &s3, ",", v1, ",", _ ) );
							}
				        else
				            {
							if ( stripos( v3, strdcpy( &s3, ",", v1, _ ), 0 ) < 0 )
				               	var_set( hConf, "pki_list", strdcpy( &s3, ",", v1, v3, _ ) );
				            else
				               	{
								if ( stripos( v3, strdcpy( &s3, ",", v1, ",", _ ), 0 ) < 0 )
					               	var_set( hConf, "pki_list", strdcpy( &s3, v3, v1, ",", _ ) );
								}
				            }
				
						// TRI LA LISTE PAR ORDRE DU PLUS LONG AU PLUS COURT
						v3 = var_get( hConf, "pki_list" );
						if ( v3 )
							{
							b = true;
							b1 = false;
							ts = str_explode( v3, "," );
							while( b )
								for( i = 0, b = false ; ts[i+1] ; i++ )
									{
									if ( strlen( ts[i] ) < strlen( ts[i+1] ) )
										{
										v1 = ts[i];
										ts[i] = ts[i+1];
										ts[i+1] = v1;
										b = b1 = true;
										}
									}
									
							if ( b1 )
								{
								strdcpy( &s3, "", _ );
								for( i = 0 ; ts[i] ; i++ )
									{
									strdcat( &s3, ",", ts[i], _ );
									}
								var_set( hConf, "pki_list", s3 );
								}
							}
						
						var_save( hConf, fname );
						}
					tr( "cmd_flux_41.4" );
					if ( s[0] )
						{
						strdcat( &s, &(v[p0]), _ );
						v = s;
						}	
					var_set( hConf1, "etape_script", v );
					tr( "cmd_flux_41.5" );
					}
				tr( "cmd_flux_41.6" );
				}
			else
			//------------------------------------------------------------------
			if ( ! strcmp( v, "on_error" ) )
			//------------------------------------------------------------------
				{
				tr( "cmd_flux_42" );
				if ( v = var_get( hReq, "script" ) )
					{
					if ( v[0] == '@' )
						{
						v1 = file_load( &s, &(v[1]) );
						if ( !v1 )
							{
							print( "ERREUR : script : lecture du fichier ", &(v[1]), BR, _ );
							return ERROR;
							}
						v = v1;
						}
					var_set( hConf1, "etape_script", v );
					}
				}
			else
			//------------------------------------------------------------------
			if ( ! strcmp( v, "waitfile" ) )
			//------------------------------------------------------------------
				{
				tr( "cmd_flux_43" );
				if ( v = var_get( hReq, "wait_fname" ) )
					var_set( hConf1, "etape_wait_fname", v );
				if ( v = var_get( hReq, "horaire" ) )
					var_set( hConf1, "etape_horaire", v );
				if ( v = var_get( hReq, "isolate_files" ) )
					{
					if ( strmicmp( v, "o|oui|y|yes|true|vrai" ) )
						var_set( hConf1, "etape_isolate_files", "Oui" );
					else
						var_set( hConf1, "etape_isolate_files", "Non" );
					}
				if ( v = var_get( hReq, "one_by_one" ) )
					{
					if ( strmicmp( v, "o|oui|y|yes|true|vrai" ) )
						var_set( hConf1, "etape_one_by_one", "Oui" );
					else
						var_set( hConf1, "etape_one_by_one", "Non" );
					}
				}
			else
			//------------------------------------------------------------------
			if ( ! strcmp( v, "sendack" ) )
			//------------------------------------------------------------------
				{
				v = var_get( hReq, "ack_fname" );
				var_set( hConf1, "etape_ack_fname", v );
				}
			else
			//------------------------------------------------------------------
			if ( ! strcmp( v, "waitack" ) )
			//------------------------------------------------------------------
				{
				v = var_get( hReq, "timeout" );
				var_set( hConf1, "etape_timeout", v );
				}
			else
			//------------------------------------------------------------------
			if ( ! strcmp( v, "start" ) )
			//------------------------------------------------------------------
				{
				v = var_get( hReq, "flux_name" );
				var_set( hConf1, "etape_flux_name", v );
				}
			else
			//------------------------------------------------------------------
			if ( ! strcmp( v, "startinside" ) )
			//------------------------------------------------------------------
				{
				v = var_get( hReq, "flux_name" );
				var_set( hConf1, "etape_flux_name", v );
				}
			else
			//------------------------------------------------------------------
			if ( ! strcmp( v, "planif" ) )
			//------------------------------------------------------------------
				{
				if ( v = var_get( hReq, "planif" ) )
					var_set( hConf1, "etape_planif", v );
				if ( v = var_get( hReq, "planif_enable" ) )
					{
					if ( strmicmp( v, "n|no|non|false" ) )
						{
						var_set( hConf1, "etape_planif_enable", "Non" );
						}
					else
						var_set( hConf1, "etape_planif_enable", "Oui" );
					}
				}
			
			//------------------------------------------------------------------
			var_save( hConf1, fname1 );
			}

		//----------------------------------------------------------------------
		if ( var_get( hReq, "start" ) )
		//----------------------------------------------------------------------
			{
			tr( "cmd_flux_43a" );
			if ( ! var_get( hReq, "no_verify" ) )
				{
				// VERIFIE QUE LE FLUX N'EST PAS DEJA DEMARRE
				
				tr( "cmd_flux_43a.1" );
				h = 0;
//				strdcpy( &s, dirrflux, DIR_CHAR "*", _ );
				strdcpy( &s, dirflux, DIR_CHAR, "*.run", _ );
				tf = NULL;
				tr( "cmd_flux_43a.2" );
				while( ( tf = file_list( tf, s, "nodir adir" ) ) != NULL )
					{
					tr( "cmd_flux_43a.3" );
//					if ( ! is_numeric( tf->name ) ) continue;
					strdncpy( &fname, dirflux, -1, DIR_CHAR, 1,
								tf->name, strlen(tf->name)-4, ".dat", 4, _ );
					if ( h ) var_free( h );
					h = var_load( fname );
					if ( ( v = var_get( h, "app_name" ) ) == null ) continue;
					if ( stricmp( appname, v ) ) continue;
					if ( ( v = var_get( h, "flux_name" ) ) == null ) continue;
					if ( stricmp( fluxname, v ) ) continue;
					print( "ATTENTION : Ce flux est déjà en cours d'exécution (flux_id=", tf->name, ").", _ );
					tr( "cmd_flux_43a.4" );
					return OK;
					}
				}
			// DEMARRAGE DU FLUX
			
			tr( "cmd_flux_43a.5" );
			flux_start( t_work );
			tr( "cmd_flux_43a.6" );
			return;
			}
		}
	else
		{
		tr( "cmd_flux_44" );
		
		//----------------------------------------------------------------------
		// ACTIONS GENERALES SUR LES FLUX
		//----------------------------------------------------------------------

		appname = null;
		fluxname = null;
		v = var_getn( hReq, 1, &appname );
		if ( ( v )&&( ! strcmp( v, "true" ) )&&( stricmp( appname, "log" ) ) )
			{
			tr( "cmd_flux_45" );
			upper( appname );
			v = var_getn( hReq, 2, &fluxname );
			if ( ( v )&&( ! strcmp( v, "true" ) )&&( stricmp( fluxname, "log" ) ) )
				{
				tr( "cmd_flux_44" );
				upper( fluxname );
		        for( i = strlen( fluxname ) - 1 ; i >=0 ; i-- )
		            {
		            if ( ( fluxname[i] >= 'A' )&&( fluxname[i] <= '_' ) ) continue;
		            if ( ( fluxname[i] >= '0' )&&( fluxname[i] <= '9' ) ) continue;
		            switch( fluxname[i] )
		                {
		                case ' ' :
		                    fluxname[i] = '_';
		                case '_' :
		                case '-' :
		                    break;
		                case '/' :
		                case ':' :
		                    fluxname[i] = '@';
		                    break;
		                }
		            }
				if ( hConf ) var_free( hConf );
				hConf = var_load( strdcpy( &fname, dircflux, "/", appname, "_flux_",
									fluxname, ".dat", _ ) );
				}
			else
				fluxname = null;
			}
		else
			appname = null;

		tr( "cmd_flux_46" );
		
		v = var_getn( hReq, 3, &s );
		n = 0;
		if ( ( v )&&( !strcmp( v, "true" ) )&&( is_numeric( s ) ) )
			n = atoi( s );
		
		//----------------------------------------------------------------------
		// LISTE DES FLUX EXECUTES (LOG)
		//----------------------------------------------------------------------

		if ( var_get( hReq, "log" ) )
			{
			tr( "cmd_flux_47" );
			
			w = var_get( hReq, "date" );
//			if ( !w )	w = currdate("yyyy/mm/dd");
			if ( w )	for( j = 0 ; w[j] ; j++ ) if ( w[j] == '_' ) w[j] = '/';
			
			v = var_get( hReq, "max_item" );
			if ( !v ) n = 100;
			else n = atoi( v );
			
			v1 = upper(var_get( hReq, "status" ));
			v2 = var_get( hReq, "filter" );

			v = var_get( hReq, "of" );

			tr( "cmd_flux_47a" );
			if ( ( v )&&( !strcmp( v, "js" ) ) )
				print( hlog_index( &s, dirflux, w, v1, v2, n, 1 ), BR, _ );
			else
			if ( bPEPSIS )
				{
				if ( !v1 ) { v1 = "RUN"; n = 0; }
				print( hlog_index( &s, dirflux, w, v1, v2, n, 2 ), BR, _ );
				}
			else
				print( hlog_index( &s, dirflux, w, v1, v2, n, 0 ), BR, _ );

			return;
			}
	
		//----------------------------------------------------------------------
		// ACTIONS SUR UN FLUX
		//----------------------------------------------------------------------

		if ( ( appname )&&( fluxname ) )
			{
			tr( "cmd_flux_48" );
			
			//------------------------------------------------------------------
			// ACTIONS SUR LES ETAPES
			//------------------------------------------------------------------
	
			if ( n )
				{
				// SUPPRESSION D'UNE ETAPE

				if ( var_get( hReq, "delete" ) )
					{
					if ( ! is_authorized( appname, "gestionnaire" ) ) return ERROR;
					strdcpy( &fname1, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(n), ".dat", _ );

					unlink( fname1 );
					for( i = n+1 ;  ; i++ )
						{
						strdcpy( &fname2, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(i), ".dat", _ );
						if ( ! file_exists( fname2 ) ) break;
						file_move( fname2, fname1 );
						v = fname1;
						fname1 = fname2;
						fname2 = v;
						}
					print( "OK", _ );
					return;
					}
					
				// MONTE UNE ETAPE D'UN CRAN

				if ( var_get( hReq, "up" ) )
					{
					if ( ! is_authorized( appname, "gestionnaire" ) ) return ERROR;
					if ( n > 1 )
						{
						strdcpy( &fname1, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(n), ".dat", _ );
						strdcpy( &fname2, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(n-1), ".dat", _ );
						strdcpy( &s, dircflux, "/", appname, "_flux_", fluxname, "_etape_0.dat", _ );
						file_move( fname1, s );
						file_move( fname2, fname1 );
						file_move( s, fname2 );
						}
					print( "OK", _ );
					return;
					}
				
				// DESCEND UNE ETAPE D'UN CRAN
				
				if ( var_get( hReq, "down" ) )
					{
					if ( ! is_authorized( appname, "gestionnaire" ) ) return ERROR;

					strdcpy( &fname1, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(n), ".dat", _ );
					strdcpy( &fname2, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(n+1), ".dat", _ );
					if ( file_exists( fname2 ) )
						{
						strdcpy( &s, dircflux, "/", appname, "_flux_", fluxname, "_etape_0.dat", _ );
						file_move( fname1, s );
						file_move( fname2, fname1 );
						file_move( s, fname2 );
						}
					print( "OK", _ );
					return;
					}
				}
				
			//------------------------------------------------------------------
			// ACTIONS SUR UN FLUX
			//------------------------------------------------------------------
	
			else
				{
				//--------------------------------------------------------------
				// SUPPRESSION DU FLUX
				//--------------------------------------------------------------
		
				if ( var_get( hReq, "delete" ) )
					{
					if ( ! is_authorized( appname, "gestionnaire" ) ) return ERROR;
					file_delete( strdcpy( &s, dircflux, "/", appname, "_flux_", fluxname, ".dat", _ ) );
					file_delete( strdcpy( &s, dircflux, "/", appname, "_flux_", fluxname, "_etape_*", _ ) );
					print( "OK", _ );
					return;
					}
				else

				//--------------------------------------------------------------
				// DUPLICATION DU FLUX
				//--------------------------------------------------------------
		
				if ( var_get( hReq, "duplique" ) )
					{
					if ( ! is_authorized( appname, "gestionnaire" ) ) return ERROR;
					
					// CALCUL UN NOM DE FLUX

					for( n = 0 ; ; n++ )
						{
						if ( !n )
							strdcpy( &s, fluxname, "_COPIE", _ );
						else
							strdcpy( &s, fluxname, "_COPIE_", itoa(n), _ );
						strdcpy( &fname1, dircflux, "/", appname, "_flux_", s, ".dat", _ );
						if( ! file_exists( fname1 ) ) break;
						}
					
					// DUPLIQUE LES FICHIERS

					strdcpy( &fname2, dircflux, "/", appname, "_flux_", fluxname, ".dat", _ );
					file_copy( fname2, fname1 );
					
					for( n = 1 ; ; n++ )
						{
						strdcpy( &fname2, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(n), ".dat", _ );
						if ( ! file_exists( fname2 ) ) break;
						strdcpy( &fname1, dircflux, "/", appname, "_flux_", s, "_etape_", itoa(n), ".dat", _ );
						file_copy( fname2, fname1 );
						}
					
					print( "OK:", s, _ );
					return;
					}
				}
			}
		}
	
	//--------------------------------------------------------------------------
	
	if ( ( appname )&&( ! is_authorized( appname, "visiteur" ) ) ) return ERROR;
		
	if ( fluxname )
		{
		//----------------------------------------------------------------------
		// LECTURE D'UNE VARIABLE PROTEGEE
		//----------------------------------------------------------------------

		tr( "cmd_flux_49" );
		
		if ( var_get( hReq, "pki" ) )
			{
			v = var_getn( hReq, 4, &s );
			if ( ! v ) { print( "ERREUR (1)", _ ); return; }
			v1 = var_get( hConf, strdcpy( &s1, "pki_", s, _ ) );
			if ( ! v1 ) { print( "ERREUR (2)", _ ); return; }
			print( v1, _ );
			return;
			}

		//----------------------------------------------------------------------
		// GESTION DES VERSIONS
		//----------------------------------------------------------------------

		tr( "cmd_flux_50" );
		
		v1 = var_get( hReq, "version" );
		if ( v1 )
			{
			v2 = var_get( hConf, "flux_version" );
			if ( !v2 ) v2 = "1";
			if ( !strcmp( v1, v2 ) )
				v1 = null;
			}
		
		tr( "cmd_flux_51" );
		if ( ( v1 )&&( var_get( hReq, "restore" ) ) )
			{
			// SAUVEGARDE LA VERSION ACTUELLE
			
			tr( "cmd_flux_52" );
			
			strdcpy( &fname, dircflux, "/", appname, "_flux_", fluxname, ".dat", _ );
			file_delete( strdcpy( &s, dircflux, "/", appname, "_flux", v2, "_", fluxname, ".dat", _ ) );
			file_delete( strdcpy( &s, dircflux, "/", appname, "_flux", v2, "_", fluxname, "_etape_*", _ ) );
			file_copy( fname, strdcpy( &s,
						dircflux, "/", appname, "_flux", v2, "_", fluxname, ".dat", _ ) );

			for( i = 1 ; file_exists( strdcpy( &fname1,
							dircflux, "/", appname, "_flux_", fluxname,
							"_etape_", itoa(i), ".dat", _ ) ); i++ )
				{
				file_copy( fname1, strdcpy( &fname2,
							dircflux, "/", appname, "_flux", v2, "_", fluxname,
							"_etape_", itoa(i), ".dat", _ ) );
				}

			// RESTAURATION DE L'ANCIENNE VERSION
			
			tr( "cmd_flux_53" );
			
			file_delete( strdcpy( &s, dircflux, "/", appname, "_flux_", fluxname, ".dat", _ ) );
			file_delete( strdcpy( &s, dircflux, "/", appname, "_flux_", fluxname, "_etape_*", _ ) );
			
			file_copy( strdcpy( &s, 
								dircflux, "/", appname, "_flux", v1, "_", fluxname, ".dat", _ ),
						fname );

			for( i = 1 ; file_exists( strdcpy( &fname1,
							dircflux, "/", appname, "_flux", v1, "_", fluxname,
							"_etape_", itoa(i), ".dat", _ ) ); i++ )
				file_copy( fname1, strdcpy( &s,
							dircflux, "/", appname, "_flux_", fluxname,
							"_etape_", itoa(i), ".dat", _ ) );

			file_delete( strdcpy( &s,
					dircflux, "/", appname, "_flux", v1, "_", fluxname, ".dat", _ ) );
			file_delete( strdcpy( &s,
					dircflux, "/", appname, "_flux", v1, "_", fluxname, "_etape_*", _ ) );

			print( "OK" BR, _ );
			tr( "cmd_flux_54" );
			return OK;
			}
			
		b1 = false;
		v = var_get( hReq, "of" );
		if ( ( v )&&( ! strcmp( v, "js" ) ) )
			b1 = true;
		
		tr( "cmd_flux_55" );
		
		v3 = var_getn( hReq, 3, &vn );
		n = 0;
		if ( ( v3 )&&( !strcmp( v3, "true" ) )&&( is_numeric( vn ) ) )
			n = atoi( vn );

		tr( "cmd_flux_56" );
		
		// CALCUL LE NOMBRE DE VERSION
		for( i = 2 ; i < 1000 ; i++ )
			{
			if ( ! file_exists( strdcpy( &s, 
				dircflux, "/", appname, "_flux", itoa(i), "_", fluxname, ".dat", _ ) ) )
				{
				if ( ! file_exists( strdcpy( &s, 
					dircflux, "/", appname, "_flux", itoa(i+1), "_", fluxname, ".dat", _ ) ) )
					{
					i--;
					break;
					}
				}
			}

		tr( "cmd_flux_57" );
		v = var_get( hConf, "flux_version" );
		if ( ( v )&&( (n1=atoi( v )) > i ) ) i = n1;
		var_set( hConf, "flux_version_nb", itoa( i ) );
		
		v = v3;
		
		//----------------------------------------------------------------------
		// AFFICHE LES INFO SUR LE FLUX
		//----------------------------------------------------------------------

		tr( "cmd_flux_58" );
		if ( !n )
			{
			if ( !b1 )
				{
				print( "   descr           = ", var_get( hConf, "flux_descr" ), BR, _ );
				print( "   desactivé       = ", var_get( hConf, "flux_disabled" ), BR, _ );
				print( "   version         = ", var_get( hConf, "flux_version" ), BR, _ );
				print( "   verrouillé      = ", var_get( hConf, "flux_locked" ), BR, _ );
				print( "   version_nb      = ", var_get( hConf, "flux_version_nb" ), BR, _ );
				}
			else
				{
				print( v2 = var_to_js_array( hConf, &s ), _ );
				print( (char*)((v2[0])?",":""), "etapes:[", _ );
				}
			
			tr( "cmd_flux_59" );
			tf = NULL;
			while( tf = file_list( tf, dircflux, "nodir" ) )
				{
				if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;
				if ( strpos( tf->name, strdcpy(&s, appname, "_flux", _), 0 ) < 0 ) continue;
				b2 = false;
				if ( ( p = strpos( tf->name, "_flux_", 0 ) ) < 0 )
					{
					if ( !v1 ) continue;
					if ( ( p = strpos( tf->name, strdcpy( &s, "_flux", v1, "_", _ ), 0 ) ) < 0 )
						continue;
					b2 = true;
					}
				if ( ( p1 = strpos( tf->name, "_etape_", 0 ) ) < 0 ) continue;
				if ( ( p2 = strpos( tf->name, fluxname, p ) ) < 0 ) continue;
				if ( p2 + strlen( fluxname ) != p1 ) continue;
				p2 = strpos( tf->name, ".", p+6 );
				v = substr( tf->name, p1+7, p2-p1-7 );
				if ( hConf1 ) var_free( hConf1 );
				hConf1 = var_load( strdcpy( &fname, dircflux, "/", tf->name, _ ) );
				var_to_js_array( hConf1, &s1 );
				strd_quote( &s1 );
				if ( !b2 )
					{
					if ( ! b1 )
						print( "   Etape ", v, " : ", var_get( hConf1, "etape_type" ),
							" : ", var_get( hConf1, "etape_descr" ), BR, _ );
					else
						{
						if ( n ) print( ",", _ );
						strdcpy( &s, var_get( hConf1, "etape_descr" ), _ );
						print( "['", v, "','", var_get( hConf1, "etape_type" ), "', ",
							quote(s), ",0,", s1, "]", _ );
						}
					}
				else
					{
					if ( ! b1 )
						print( "   Etape ", v, " : v", v1, " : ", var_get( hConf1, "etape_type" ),
							" : ", var_get( hConf1, "etape_descr" ), BR, _ );
					else
						{
						if ( n ) print( ",", _ );
						strdcpy( &s, var_get( hConf1, "etape_descr" ), _ );
						print( "['", v, "','", var_get( hConf1, "etape_type" ), "', ",
							quote(s), ",", v1, ",", s1, "]", _ );
						}
					}
				n++;
				}
			if ( b1 )
				{
				print( "],", _ );
				if ( is_authorized( appname, "gestionnaire" ) )
					print( "profile:'gestionnaire'", _ );
				else
				if ( is_authorized( appname, "editeur" ) )
					print( "profile:'editeur'", _ );
				else
				if ( is_authorized( appname, "visiteur" ) )
					print( "profile:'visiteur'", _ );
				else
					print( "profile:'none'", _ );
				}
			}
		else
			{
			tr( "cmd_flux_60" );
			if ( !v1 )
				strdcpy( &fname1, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(n), ".dat", _ );
			else
				strdcpy( &fname1, dircflux, "/", appname, "_flux", v1, "_", fluxname, "_etape_", itoa(n), ".dat", _ );
			if ( hConf1 ) var_free( hConf1 );
			hConf1 = var_load( fname1 );
			if ( ! b1 )
				{
				print( "   type           = ", v = var_get( hConf1, "etape_type" ), BR, _ );
				print( "   descr          = ", var_get( hConf1, "etape_descr" ), BR, _ );
				if ( ! strcmp( v, "emi" ) )
					{
					print( "   dest_liste   = ", var_get( hConf1, "age_dest_liste" ), BR, _ );
					print( "   adle         = ", var_get( hConf1, "age_adle" ), BR, _ );
					print( "   appe         = ", var_get( hConf1, "age_appe" ), BR, _ );
					print( "   mft_options  = ", var_get( hConf1, "age_mft_options" ), BR, _ );
					print( "   appl_info    = ", var_get( hConf1, "age_appl_info" ), BR, _ );
					print( "   bin_files    = ", var_get( hConf1, "age_bin_files" ), BR, _ );
					print( "   txt_files    = ", var_get( hConf1, "age_txt_files" ), BR, _ );
					print( "   script       = ", var_get( hConf1, "age_script" ), BR, _ );
					}
				else
				if ( ! strcmp( v, "rec" ) )
					{
					print( "   start_cond     = ", var_get( hConf, "agr_start_cond" ), BR, _ );	
					print( "   on_receive     = ", var_get( hConf, "agr_script" ), BR, _ );
					print( "   horaire        = ", var_get( hConf, "agr_horaire" ), BR, _ );
					print( "   restart_onrecv = ", var_get( hConf, "agr_restart_onrecv" ), BR, _ );
//					print( "   recv_wait      = ", var_get( hConf, "recv_wait" ), BR, _ );
					}
				else
				if ( ! strcmp( v, "ftp" ) )
					{
					print( "   ftp_dest  = ", var_get( hConf, "ftp_dest" ), BR, _ );	
					print( "   ftp_login = ", var_get( hConf, "ftp_login" ), BR, _ );
					print( "   ftp_pw    = ", var_get( hConf, "ftp_pw" ), BR, _ );
					print( "   ftp_cmd   = ", var_get( hConf, "ftp_cmd" ), BR, _ );
					}
				else
					{
					print( "   script = ", var_get( hConf, "etape_script" ), BR, _ );	
					}
						
				}
			else
				print( var_to_js_array( hConf1, &s ), _ );
			}
		
		tr( "cmd_flux_61" );	
		return OK;
		}	
	
	//--------------------------------------------------------------------------
	// AFFICHE LA LISTE DES FLUX
	//--------------------------------------------------------------------------

	b1 = false;
	v = var_get( hReq, "of" );
	if ( ( v )&&( !strcmp( v, "js" ) ) ) b1 = true;

	if ( b1 ) print( "[", _ );
	n = 0;
	tf = NULL;
	while( tf = file_list( tf, dircflux, "nodir" ) )
		{
		if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;
		if ( ( appname )&&( strpos( tf->name, strdcpy(&s, appname, "_flux_", _), 0 ) < 0 ) ) continue;
		if ( ( p = strpos( tf->name, "_flux_", 0 ) ) < 0 ) continue;
		if ( ( strpos( tf->name, "_etape_", 0 ) ) > 0 ) continue;
		if ( !appname )
			{
			v = substr( tf->name, 0 , p );
			if ( ! is_authorized( v, "visiteur" ) ) continue;
			}
		p1 = strpos( tf->name, ".", p+6 );

		fluxname = substr( tf->name, p+6, p1-p-6 );
        for( i = strlen( fluxname ) - 1 ; i >=0 ; i-- )
			if ( fluxname[i] == '@' ) fluxname[i] = '/';

		v = substr( tf->name, 0, p );
		if ( hConf ) var_free( hConf );
		hConf = var_load( strdcpy( &fname, dircflux, "/", tf->name, _ ) );
		if ( ! b1 )
			print( v, " ", fluxname, " ", var_get( hConf, "flux_descr" ),
				BR, _ );
		else
			{
			if ( n ) print( ",", _ );
			print( "['", v, "','", fluxname, "']", _ );
			}
		n++;
		}
	if ( ! b1 ) print( itoa(n), " flux", BR, _ );
	else print( "]", _ );
	}






//------------------------------------------------------------------------------
int flux_exec_services( T_WORK* t_work )

// COMMANDE FLUX SUR LES FLUX EN COURS D'EXECUTION
//------------------------------------------------------------------------------
	{
    handle wrk_hLog;

	strdcpy( &fname, dirflux, "/", flux_id, ".dat", _ );
	if ( hFlux ) var_free( hFlux );
	hFlux = var_load( fname );
	if ( !hFlux )
		{
		print( "ERREUR : Le flux demandé n'existe pas.", BR, _ );
		return ERROR;
		}
		
	if ( v = var_getn( hReq, 2, &vn ) )
		{
		if ( ( ! strcmp( vn, "execute" ) )
				||( ( ! strcmp( vn, "dupli_start" ) )&&( ! strcmp( argv[4], "timeout" ) ) )
			)
			{
			hLog = hlog_init( dirflux, flux_id );
			flux_execute( t_work );
			ssie7exit(0);
			}
		

		if ( ! strcmp( vn, "exec_button" ) )
			{
			button_execute( t_work );
			strdcpy( &fname, dirflux, "/", flux_id, ".dat", _ );
			var_save( hFlux, fname );
			return OK;
			}

		
		if ( ! strcmp( vn, "dupli_start" ) )
			{
			flux_dupli_start( t_work );
			return OK;
			}
		
		if ( ! strcmp( vn, "c" ) )
			{
			return OK;
			}
			
		if ( ! strcmp( vn, "stop" ) )
			{
			flux_stop( t_work );
			return OK;
			}

		if (( ! strcmp( vn, "noreport" ) )||( ! strcmp( vn, "no_report" ) ))
			{
			v = var_get( hFlux, "noreport" );
			if ( ( v )&&( !strcmp( v, "true" ) ) )
				{
				print( "ERREUR : no_report déjà fait" BR, _ );
				return OK;
				}

			if ( ! var_get( hFlux, "report_version" ) )
				{
//				v = var_get( hFlux, "status" );
        		wrk_hLog = hlog_init( dirflux, flux_id );
        		hlogr( wrk_hLog, 0, &s4, null );
        		free( (char*)wrk_hLog );
        		v = s4;
				if ( ! strnicmp( v, "OK", -1 ) )
					var_set( hFlux, "report_set_ok", "true" );
				else
				if ( ! strnicmp( v, "ALERT", -1 ) )
					var_set( hFlux, "report_alert", "true" );
				else	
					var_set( hFlux, "report_error", "true" );
				var_set( hFlux, "report_version", "2" );	
				}
			
			var_delete( hFlux, "ask_report_error" );
			var_delete( hFlux, "ask_report_ok" );
			var_set( hFlux, "ask_noreport", "true" );
			if ( v = var_get( hReq, "comment" ) )
				var_set( hFlux, "comment", v );


			var_delete( hFlux, "report_ok" );
			var_save( hFlux, fname );
			print( "OK (veuillez patienter 3 minutes maxi)" BR, _ );
			
			return OK;
			}

		if (( ! strcmp( vn, "report_error" ) )||( ! strcmp( vn, "reporterror" ) ))
			{
			v = var_get( hFlux, "report_error" );
			if ( ( v )&&( !strcmp( v, "true" ) ) )
				{
				print( "ERREUR : report_error déjà fait" BR, _ );
				return OK;
				}

			if ( ! var_get( hFlux, "report_version" ) )
				{
//				v = var_get( hFlux, "status" );
        		wrk_hLog = hlog_init( dirflux, flux_id );
        		hlogr( wrk_hLog, 0, &s4, null );
        		free( (char*)wrk_hLog );
        		v = s4;
				if ( ! strnicmp( v, "OK", -1 ) )
					var_set( hFlux, "report_set_ok", "true" );
				else
				if ( ! strnicmp( v, "ALERT", -1 ) )
					var_set( hFlux, "report_alert", "true" );
				else	
					var_set( hFlux, "report_error", "true" );
				var_set( hFlux, "report_version", "2" );	
				}
			
			var_delete( hFlux, "ask_report_ok" );
			var_delete( hFlux, "ask_noreport" );
			var_set( hFlux, "ask_report_error", "true" );
			if ( v = var_get( hReq, "comment" ) )
				var_set( hFlux, "comment", v );
			
			var_delete( hFlux, "report_ok" );
			var_save( hFlux, fname );
			print( "OK (veuillez patienter 3 minutes maxi)" BR, _ );
			
			return OK;
			}

		if (( ! strcmp( vn, "report_ok" ) )||( ! strcmp( vn, "reportok" ) ))
			{
			v = var_get( hFlux, "report_set_ok" );
			if ( ( v )&&( !strcmp( v, "true" ) ) )
				{
				print( "ERREUR : report_ok déjà fait" BR, _ );
				return OK;
				}

			if ( ! var_get( hFlux, "report_version" ) )
				{
//				v = var_get( hFlux, "status" );
        		wrk_hLog = hlog_init( dirflux, flux_id );
        		hlogr( wrk_hLog, 0, &s4, null );
        		free( (char*)wrk_hLog );
        		v = s4;
				if ( ! strnicmp( v, "OK", -1 ) )
					var_set( hFlux, "report_set_ok", "true" );
				else
				if ( ! strnicmp( v, "ALERT", -1 ) )
					var_set( hFlux, "report_alert", "true" );
				else	
					var_set( hFlux, "report_error", "true" );
				var_set( hFlux, "report_version", "2" );	
				}
			
			var_delete( hFlux, "ask_report_error" );
			var_delete( hFlux, "ask_noreport" );
			var_set( hFlux, "ask_report_ok", "true" );
			if ( v = var_get( hReq, "comment" ) )
				var_set( hFlux, "comment", v );
			
			var_delete( hFlux, "report_ok" );
			var_save( hFlux, fname );
			print( "OK (veuillez patienter 3 minutes maxi)" BR, _ );
			
			return OK;
			}
		}
	
	v = var_get( hReq, "of" );
	b = ( ( v )&&( !strcmp( v, "js" ) ) ) ? true : false;
	
	if (b)
		{
		print( hlog_export( &s, dirflux, flux_id, 1 ), _ );

/*
ssie_buttons=[
	[ "btn_name", etape_no, "label", "Confirm Msg",
			[
			{
			input
			type
			values
			...
			}
			]
		...
		],
	...
	]

*/
		v = var_get( hFlux, "ssie_buttons" );
		if ( v )
			{
			print( ";ssie_buttons=[", _ );
			ts = str_explode( v, "," );
			for( i = 0, b = false ; ts[i] ; i++ )
				{
				if ( ts[i][0] == 0 ) continue;
				v1 = var_get( hFlux, ts[i] );
				if ( !v1 ) continue;
				p = strpos( ts[i], ":", 0 );
				if ( p < 0 ) continue;
				p1 = strpos( v1, "::", 0 );
				if ( p1 < 0 ) continue;
				
				if ( b ) print( ",", _ ); else b = true;
				
				print( "[", quote( v4 = &(ts[i][p+1]) ),
						",", itoa(atoi(ts[i])), _ );

				// CHARGE LE SCRIPT
				v1[p1] = 0;
				var_free( h );
				h = var_load( v1 );
				v = var_get( h, "etape_script" );
				if ( !v ) v = var_get( h, "age_script" );
				if ( !v ) v = var_get( h, "agr_script" );
				if ( !v ) continue;
				
				// RECHERCHE LA DEFINITION DU BOUTON DU SCRIPT
				
				p0 = 0;
				while( sgets( &s, v, &p0 ) )
					{
					if ( (p=stripos( s, "ssie_button ", 0 )) < 0 ) continue;
					if ( trim(substr( s, 0, p ))[0] != 0 ) continue;
					
					var_free( h1 );
					h1 = var_from_cmd( 0, s );
					if ( !var_getn( h1, 1, &vn ) ) continue;
					if ( stricmp( vn, v4 ) ) continue;
					vn = null;
					var_getn( h1, 2, &vn );
					
					// BOUTON TROUVE
					if (( !vn )||( vn[0] == 0 ))
						continue;	// Label obligatoire

					strdcpy( &s, vn, _ );
					var_list_replace( hFlux, &s );
					print( ",", quote(s), _ ); // LABEL

					vn = null;
					var_getn( h1, 3, &vn );
					strdcpy( &s, vn, _ );
					var_list_replace( hFlux, &s );
					print( ",", quote(s), _ ); // CONFIRM MSG
					
/*
					while( sgets( &s, v, &p0 ) )
						{
						
						}
					
*/					
					break;
					}
					
//				print( "[]", _ );
				print( "]", _ );
				}
			print( "];", _ );
			}

		}
	else
		{
		print( hlog_export( &s, dirflux, flux_id, 0 ), BR, _ );
		}
	
	}

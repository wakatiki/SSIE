#include "ssie7.h"

//------------------------------------------------------------------------------
int cmd_import( T_WORK *t_work )

// COMMANDE IMPORT D'UN FLUX SSIE7
//------------------------------------------------------------------------------
	{
	mkdir( dirimport );

	// LECTURE DES PARAMETRES
	
	v = var_getn( hReq, 1, &appname );
	if ( ( ! v )||( strcmp( v, "true" ) ) )
		{
		print( "ERREUR : syntaxe : import {app_name} [import_id] {fname=...} load" BR, _ );
		return ERROR;
		}
	upper( appname );
	if ( ! file_exists( strdcpy( &fname, dircflux, DIR_CHAR, appname, ".dat", _ ) ) )
		{
		print( "ERREUR : L'application demandée n'existe pas.", BR, _ );
		return ERROR;
		}
	
	if ( !is_authorized( appname, "gestionnaire" ) )
		{
		print( "ERREUR : Vous n'êtes pas habilité." BR, _ );
		return ERROR;
		}

	
	v = var_getn( hReq, 2, &import_id );
	if ( ( ! v )||( strcmp( v, "true" ) )||( !is_numeric(import_id) ) )
		import_id = null;
	
	
	
	// INCORPORE UN FICHIER DANS LES IMPORTS (CAS DE TRANSFERT PAR AUTRE MOYEN QUE SSIE7)
	
	if ( v = var_get( hReq, "fname" ) )
		{
		// CALCUL UN NOUVEL ID
		strdcpy( &fname, dirimport, "/", appname, ".id", _ );
		import_id = itoa(get_uid( fname ));
		if ( !import_id )
			{
			print( "ERREUR : obtention d'un identifiant d'import." BR, _ );
			return ERROR;
			}

		if ( hConf ) var_free( hConf );
		hConf = var_load( v );
		var_set( hConf, "log", "" );
		strdcpy( &fname, dirimport, "/", appname, "_", import_id, ".dat", _ );
		var_save( hConf, fname );
		
		print( "OK" BR, _ );
		import_id = null;
		return OK;
		}

	
	b = false;
	v = var_get( hReq, "of" );
	if ( ( v )&&( !stricmp( v, "js" ) ) )
		b = true;
		
		
	// AFFICHE LA LISTE DES IMPORTS
	
	if ( import_id == null )
		{
		n = 0;
		
		if ( file_exists( strdcpy( &fname, dirimport, "/", appname, ".id", _ ) ) )
			{
			i = read_uid( fname );
			if ( b ) print( "[", _ );
			for( ; i > 0 ; i-- )
				{
				strdcpy( &fname, dirimport, "/", appname, "_", itoa(i), ".dat", _ );
				if ( !file_exists( fname ) ) continue;
				if ( hConf ) var_free( hConf );
				hConf = var_load( fname );
				if ( !b )
					print( itoa(i), " : ", var_get( hConf, "export_date" ),
						", ", var_get( hConf, "flux_liste" ), BR, _ );
				else
					print( (char*)((n)?",":""), "{import_id:", itoa(i),
						",export_date:", str_quote( var_get( hConf, "export_date" ) ),
						",export_descr:", str_quote( var_get( hConf, "export_descr" ) ),
						",export_pti:", str_quote( var_get( hConf, "export_pti" ) ),
						",flux_liste:", str_quote( var_get( hConf, "flux_liste" ) ),
						"}", _ );
				n++;
				}
			if ( b ) print( "]", _ );
			}
		if ( !b )
			print( itoa(n), " import", (char*)((n>1)?"s":""), _ );
		return OK;
		}
	
	
	// TRAITEMENT DES DELETE
	
	if ( var_get( hReq, "delete" ) )
		{
		unlink( strdcpy( &fname, dirimport, "/", appname, "_", import_id, ".dat", _ ) );
		print( "OK" BR, _ );
		return OK;
		}


	// CHARGEMENT DE L'IMPORT DANS L'APPLICATION
	
	if ( var_get( hReq, "load" ) )
		{
		strdcpy( &fname, dirimport, "/", appname, "_", import_id, ".dat", _ );
		if ( !file_exists( fname ) )
			{
			print( "ERREUR : L'import demandé n'existe pas." BR, _ );
			return ERROR;
			}
		if ( hConf ) var_free( hConf );
		hConf = var_load( fname );
		v = var_get( hConf, "log" );
		strdcpy( &s4, v, BR, itod(0,null), " : Début Importation" BR, _ );
		
		v = var_get( hConf, "flux_liste" );
		ts = str_explode( v, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			v = var_get( hConf, ts[i] );
			if ( hConf1 ) var_free( hConf1 );
			hConf1 = var_from_str( 0, v );
			strdcpy( &fluxname, ts[i], _ );
			strd_replace( &fluxname, "/", "@" );
			strdcpy( &fname, dircflux, DIR_CHAR,
					appname, "_flux_", fluxname, ".dat", _ );
			if ( file_exists( fname ) )
				{
				// LE FLUX EXISTE, ON VERIFIE LES VERSIONS
				
				if ( hConf2 ) var_free( hConf2 );
				hConf2 = var_load( fname );
				v1 = var_get( hConf1, "flux_version" );
				if ( !v1 ) v1 = "1";
				v2 = var_get( hConf2, "flux_version" );
				if ( !v2 ) v2 = "1";
				if ( strcmp( v1, v2 ) )
					{
					// ARCHIVE LA VERSION COURANTE
					strdcat( &s4, fluxname, " : Archive la version ", v2, " : ", _ );
					System( &s, strdcpy( &s1, "mersi4 flux ", appname, " ", ts[i], " version=", v1, " descr=''", _ ) );
					}
				else
					{
					// EXPORTE LA VERSION COURANTE
					strdcat( &s4, fluxname, " : Export la version ", v2, " : ", _ );
					System( &s, strdcpy( &s1, "mersi4 export ", appname, " flux_liste='", ts[i],
							"' descr='Export_de_sauvegarde_avant_import' user=", user, _ ) );
					}
				}

			// IMPORT DU FLUX
			var_save( hConf1, fname );
			strdcat( &s4, fluxname, " : Import" BR, _ );
			
			// IMPORT DES ETAPES
			
			#ifdef OS_UNIX
			system( strdcpy( &s, "rm -f ", dircflux, DIR_CHAR,
					appname, "_flux_", fluxname, "_etape_*", _ ) );
			#else
			system( strdcpy( &s, "del /Q ", dircflux, DIR_CHAR,
					appname, "_flux_", fluxname, "_etape_*", _ ) );
			#endif
				
			for( j = 1 ; ; j++ )
				{
				strdcpy(&s1, ts[i], "_etape_", itoa(j), _ );
				v = var_get( hConf, s1 );
				if ( !v ) break;
				if ( hConf1 ) var_free( hConf1 );
				hConf1 = var_from_str( 0, v );
				strdcpy( &fname1, dircflux, DIR_CHAR,
					appname, "_flux_", fluxname, "_etape_", itoa(j), ".dat", _ );
				var_save( hConf1, fname1 );
				strdcat( &s4, fluxname, " : Import étape ", itoa(j), BR, _ );
				}
			}

		strdcat( &s4, itod(0,null), " : FIN de l'import." BR, _ );
		var_set( hConf, "log", s4 );
		strdcpy( &fname, dirimport, "/", appname, "_", import_id, ".dat", _ );
		var_save( hConf, fname );
		
		print( "OK" BR, _ );
		return OK;
		}


	// AFFICHE LES INFO SUR L'IMPORT DEMANDE
	
	strdcpy( &fname, dirimport, "/", appname, "_", import_id, ".dat", _ );
	if ( hConf ) var_free( hConf );
	hConf = var_load( fname );
	
	if ( !b )
		{
		print( "import_id = ", import_id, BR, _ );
		print( "export_date = ", var_get( hConf, "export_date" ), BR, _ );
		print( "export_user = ", var_get( hConf, "export_user" ), BR, _ );
		print( "flux_liste  = ", v=var_get( hConf, "flux_liste" ), BR, _ );
		print( "descr       = ", var_get( hConf, "export_descr" ), BR, _ );
		print( "pti         = ", var_get( hConf, "export_pti" ), BR, _ );
		print( "log         = ", var_get( hConf, "log" ), BR, _ );

		ts = str_explode( v, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			print( ts[i], " = ", var_get( hConf, ts[i] ), BR, _ );
			for( j = 1 ; ; j++ )
				{
				strdcpy(&s1, ts[i], "_etape_", itoa(j), _ );
				v = var_get( hConf, s1 );
				if ( !v ) break;
				print( s1, " = ", v, BR, _ );
				}
			}
		}
	else
		{
		print( "{export_date:", str_quote( var_get( hConf, "export_date" )),
			",export_user:", str_quote( var_get( hConf, "export_user" )),
			",flux_liste:", str_quote( v=var_get( hConf, "flux_liste" )),
			",export_descr:", str_quote( var_get( hConf, "export_descr" )),
			",export_pti:", str_quote( var_get( hConf, "export_pti" )),
			",export_log:", str_quote( var_get( hConf, "log" )),
			_ );

		print( ",flux:[", _ );
		if ( v )
			{
			ts = str_explode( v, "," );
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( ! (ts[i][0]) ) continue;
				print( (char*)((i)?",":""), "{flux_name:", str_quote(ts[i]), ",flux_attrib:{", _ );
				v = var_get( hConf, ts[i] );
				if ( hConf1 ) var_free( hConf1 );
				hConf1 = var_from_str( 0, v );
				for( j = 0 ; v = var_getn( hConf1, j, &vn ) ; j++ )
					{
					print( (char*)((j)?",":""), vn, ":",
						(char*)((is_numeric(v))? v : str_quote( v )), _ );
					}
				print( "},etapes:[", _ );
				for( j = 1 ; ; j++ )
					{
					strdcpy(&s1, ts[i], "_etape_", itoa(j), _ );
					v = var_get( hConf, s1 );
					if ( !v ) break;
					
					print( (char*)((j>1)?",":""), "{", _ );
					if ( hConf1 ) var_free( hConf1 );
					hConf1 = var_from_str( 0, v );
					for( k = 0 ; v = var_getn( hConf1, k, &vn ) ; k++ )
						{
						print( (char*)((k)?",":""), vn, ":",
							(char*)((is_numeric(v))? v : str_quote( v )), _ );
						}
					print("}", _ );
					}
				print("]}", _ );
				}
			}
		print("]}", _ );
		}
	}




















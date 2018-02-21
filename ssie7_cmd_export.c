#include "ssie7.h"

extern char  /* *etape,*/ *flux_etape;
extern int etape_no, etape_log_id;

#define send_fname			fname2
#define cat_fname			fname3
#define log_fname			fname4


//------------------------------------------------------------------------------
int cmd_export( T_WORK *t_work )

// COMMANDE EXPORT D'UN FLUX SSIE7
//------------------------------------------------------------------------------
	{
	tr( "cmd_exporte_1" );
	mkdir( direxport );
	
	// RECHERCHE SI C'EST UNE MISE A JOUR
	
	b = false;
	for( i = var_count( hReq )-1 ; ( i > 0 )&&( v = var_getn( hReq, i, &vn ) ) ; i-- )
		{
		if ( strcmp( v, "true" ) )
			{
			if ( strmicmp( vn, "descr|flux_liste|pti|of" ) == false )
				{
				tr( "cmd_exporte_2" );
				print( "ERREUR : le paramètre ", vn, " est inconnu (cmd_export)." BR, _ );
				return ERROR;
				}
			if ( strcmp( vn, "of" ) )
				b = true;
			}
		}
	
	// LECTURE DES PARAMETRES
	
	v = var_getn( hReq, 1, &appname );
	if ( ( ! v )||( strcmp( v, "true" ) ) )
		{
		print( "ERREUR : syntaxe : export <app_name> [export_id] {descr=...} {flux_liste=...}", BR, _ );
		return ERROR;
		}
		
	tr( "cmd_exporte_3" );
	upper( appname );
	if ( ! file_exists( strdcpy( &fname, dircflux, DIR_CHAR, appname, ".dat", _ ) ) )
		{
		print( "ERREUR : L'application demandée n'existe pas.", BR, _ );
		return ERROR;
		}
	
	tr( "cmd_exporte_4" );
	if ( !is_authorized( appname, "gestionnaire" ) )
		{
		print( "ERREUR : Vous n'êtes pas habilité." BR, _ );
		return ERROR;
		}

	tr( "cmd_exporte_5" );
	v = var_getn( hReq, 2, &export_id );
	
	if ( ( ! v )||( strcmp( v, "true" ) )||( !is_numeric(export_id) ) )
		export_id = null;
	
	
	// CREATION OU MODIFICATION D'UN EXPORT
	
	if ( b )
		{
		tr( "cmd_exporte_6" );
		if ( export_id == null )
			{
			tr( "cmd_exporte_7" );
			// CALCUL UN NOUVEL ID
			strdcpy( &fname, direxport, "/", appname, ".id", _ );
			export_id = itoa(get_uid( fname ));
			if ( !export_id )
				{
				print( "ERREUR : obtention d'un identifiant d'export." BR, _ );
				return ERROR;
				}
			strdcpy( &fname, direxport, "/", appname, "_",
							export_id, ".dat", _ );
			}
		else
			{
			if ( !file_exists( strdcpy( &fname, direxport, "/", appname, "_",
							export_id, ".dat", _ ) ) )
				{
				print( "ERREUR : L'export demandé n'existe pas." BR, _ );
				return ERROR;
				}
			}
		
		tr( "cmd_exporte_8" );
		
		if ( hConf ) var_free( hConf );
		hConf = var_load( fname );

		var_set( hConf, "export_user", user );
		var_set( hConf, "export_date", itod(0, null ) );
		
		if ( v = var_get( hReq, "descr" ) )
			var_set( hConf, "export_descr", v );

		if ( v = var_get( hReq, "pti" ) )
			var_set( hConf, "export_pti", v );

		if ( v = var_get( hReq, "flux_liste" ) )
			var_set( hConf, "flux_liste", upper(v) );
		else
			v = var_get( hConf, "flux_liste" );
			
		if ( !v )
			{
			print( "ERREUR : Vous devez donner une liste de flux à exporter." BR, _ );
			return ERROR;
			}
		
		// EXPORTE TOUS LES FLUX DEMANDES ET TOUTES LES ETAPES
		
		tr( "cmd_exporte_9" );
			
		ts = str_explode( v, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			tr( "cmd_exporte_10" );
			strdcpy( &s2, ts[i], _ );
			strd_replace( &s2, "/", "@" );
			strdcpy( &fname1, dircflux, DIR_CHAR,
				appname, "_flux_", s2, ".dat", _ );
			if ( !file_exists( fname1 ) )
				{
				print( "ERREUR : Le flux ", ts[i], " n'existe pas.", _ );
				return ERROR;
				}
			tr( "cmd_exporte_11" );
			if ( hConf1 ) var_free( hConf1 );
			hConf1 = var_load( fname1 );
			var_to_str( hConf1, &s );
			var_set( hConf, ts[i], s );
			for( j = 1 ; ; j++ )
				{
				tr( "cmd_exporte_12" );
				strdcpy( &fname1, dircflux, DIR_CHAR,
				appname, "_flux_", s2, "_etape_", itoa(j), ".dat", _ );
				if ( ! file_exists( fname1 ) ) break;
				tr( "cmd_exporte_13" );
				if ( hConf1 ) var_free( hConf1 );
				hConf1 = var_load( fname1 );
				var_to_str( hConf1, &s );
				var_set( hConf, strdcpy(&s1, ts[i], "_etape_", itoa(j), _ ), s );
				}
			}
		
		var_save( hConf, fname );
		}
	
	// AFFICHE LES INFO
	
	tr( "cmd_exporte_14" );
	
	b = false;
	v = var_get( hReq, "of" );
	if ( ( v )&&( !stricmp( v, "js" ) ) )
		b = true;
		
	if ( export_id == null )
		{
		tr( "cmd_exporte_15" );
		// LISTE DES EXPORTS DE L'APPLICATION
		
		n = 0;
		
		if ( file_exists( strdcpy( &fname, direxport, "/", appname, ".id", _ ) ) )
			{
			tr( "cmd_exporte_16" );
			i = read_uid( fname );
			if ( b ) print( "[", _ );
			for( ; i > 0 ; i-- )
				{
				tr( "cmd_exporte_17" );
				strdcpy( &fname, direxport, "/", appname, "_", itoa(i), ".dat", _ );
				if ( !file_exists( fname ) ) continue;
				tr( "cmd_exporte_18" );
				if ( hConf ) var_free( hConf );
				hConf = var_load( fname );
				if ( !b )
					print( itoa(i), " : ", var_get( hConf, "export_date" ),
						", ", var_get( hConf, "flux_liste" ), BR, _ );
				else
					print( (char*)((n)?",":""), "{export_id:", itoa(i),
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
			print( itoa(n), " export", (char*)((n>1)?"s":""), _ );
		tr( "cmd_exporte_19" );
		return OK;
		}


	tr( "cmd_exporte_20" );
	
	// DELETE DEMANDE ?	
	if ( var_get( hReq, "delete" ) )
		{
		tr( "cmd_exporte_21" );
		if ( !export_id )
			{
			print( "ERREUR : Vous devez précisez un identifiant d'export" BR, _ );
			return ERROR;
			}
		tr( "cmd_exporte_22" );
		unlink( strdcpy( &fname, direxport, "/", appname, "_", export_id, ".dat", _ ) );
		print( "OK" BR, _ );
		return OK;
		}


	//--------------------------------------------------------------------------
	// TRANSFERT DE L'EXPORT VERS UN PARTENAIRE
	//--------------------------------------------------------------------------

	tr( "cmd_exporte_23" );
	v = var_getn( hReq, 3, &vn );
	if ( ( v )&&( ! stricmp( vn, "mft_send" ) ) )
		{
		tr( "cmd_exporte_24" );
		v = var_getn( hReq, 4, &partname );
		if ( !v )
			{
			print( "ERREUR : syntaxe : export [app_name] [export_id] mft_send [partname] [app_name_dest]" BR, _ );
			return ERROR;
			}
		tr( "cmd_exporte_25" );
		upper( partname );
		v = var_getn( hReq, 5, &v1 );
		if ( !v )
			v1 = appname;
			
		if ( ! part_exists( t_work ) )
			{
			print( "ERREUR : Le partenaire demandé est inconnu du serveur." BR, _ );
			return ERROR;
			}
	
		tr( "cmd_exporte_26" );
		
		//--------------------------------------------------------------
		// PREPARE LA REQUETE SEND POUR L'ENVOI VERS UN PARTENAIRE
		//--------------------------------------------------------------
		
		strdcpy( &fname, direxport, "/", appname, "_", export_id, ".dat", _ );
		
		strdcpy( &cmd, argv[0], " mft send immediat part=", partname, " fname=", fname,
			" src_fname=", quote( v = file_getname( fname ) ),
			" EXPORT_SSIE appname=", quote(v1),
			_ );
		
		//--------------------------------------------------------------
		// REALISE LE TRANSFERT
		//--------------------------------------------------------------

		System( &s1, cmd );

		if ( hConf ) var_free( hConf );
		hConf = var_load( fname );
		v = var_get( hConf, "log" );
		strdcpy( &s, v, itod(0, null), " : Envoi vers ", partname, ":", v1, " : OK", BR, _ );
/*
		strdcpy( &s, v, itod(0, null), " : Envoi vers ", partname, ":", v1, BR,
					cmd, BR, s1, BR BR,
					 _ );
*/		var_set( hConf, "log", s );
		var_save( hConf, fname );

		print( "OK", _ );
		
		tr( "cmd_exporte_27" );
		
		//--------------------------------------------------------------
		return OK;
		}
	
	
	
	
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// A ADAPTER SANS MFT
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
/*	
	v = var_getn( hReq, 3, &vn );
	if ( ( v )&&( ! stricmp( vn, "mft_send" ) ) )
		{
		v = var_getn( hReq, 4, &partname );
		if ( !v )
			{
			print( "ERREUR : syntaxe : export [app_name] [export_id] mft_send [partname] [app_name_dest]" BR, _ );
			return ERROR;
			}
		v = var_getn( hReq, 5, &v1 );
		if ( !v )
			v1 = appname;
			
		// VERIFIE QUE MFT EST DEMARRE
		
		if ( mft_status( t_work, false ) == ERROR )
			{
			print( "ERREUR : MFT n'est pas démarré." BR, _ );
			return ERROR;
			}
		
		// VERIFIE QUE PARTENAIRE EXISTE
		
		strdcpy( &fname, dirpart, "/", upper(partname), ".dat", _ );
		if ( ! file_exists( fname ) )
			{
			print( "ERREUR : Le partenaire demandé est inconnu du serveur." BR, _ );
			return ERROR;
			}
		
		// SOUMET LE TRANSFERT A MFT
		
		strdcpy( &fname, direxport, "/", appname, "_", export_id, ".dat", _ );
		strdcpy( &s2, substr(appname,0,2), substr(partname,0,2), export_id, _ );
		strdcpy( &s, "send part=", partname, ",idf=MEXPORT,",
					"parm='EXPORT MERSI POUR IMPORTATION DANS : ", v1,
					"',fname=", fname, ",ida=", s2, _ );
		r = mft_util( s, &s1 );
		if ( r == ERROR )
			{
			print( "ERREUR : mftutil : ", s1, BR, s, _ );
			return;
			}
		
		// ATTEND L'APPARITION DU TRANSFERT DANS LE CATALOGUE
		
		for( n = 120 ; n > 0 ; n-- )
			{
			sleep(1);
			r = mft_util( strdcpy( &s, "listcat part=*,ida=", s2, "*", _ ), &s1 );
			if ( r == ERROR ) continue;
			if ( strpos( s1, " 0 record(s) selected", 0 ) >= 0 ) continue;
			if ( stripos( s1, "warning", 0 ) >= 0 ) continue;
			break;
			}
		if ( n == 0 )
			{
			print( "ERREUR : Le transfert MFT ne démarre pas" BR, _ );
			return ERROR;
			}
		// ATTEND LA FIN DU TRANSFERT
		
		for( n1 = 300 ; n1 > 0 ; n1-- )
			{
			if ( n1 != 300 )
				{
				sleep( 1 );
				r = mft_util( strdcpy( &s, "listcat part=*,ida=", s2, _ ), &s1 );
				if ( r == ERROR ) continue;
				if ( strpos( s1, " 0 record(s) selected", 0 ) >= 0 ) continue;
				if ( stripos( s1, "warning", 0 ) >= 0 ) continue;
				}

			p = strpos( s1, " -------- ", 0 );
			p = strpos( s1, "\n", p ) + 1;
			b = false;
			while( sgets( &s, s1, &p ) )
				{
				if ( strlen( s ) < 40 ) continue;
				n = atoi( trim( substr( s, 56, 3 ) ) );
				if ( ( !n )&&( s[13] != 'T' ) ) continue;
		
				// SUPPRIME LE TRANSFERT
				
				r = mft_util( strdcpy( &s, "delete part=*,direct=send,ida=", s2, _ ), &s3 );
				
				if ( n ) // ERREUR DE TRANSFERT
					{
					print( "ERREUR MFT : ", mft_errlib( n ), _ );
					return ERROR;
					}
					
				strdcpy( &fname, direxport, "/", appname, "_", export_id, ".dat", _ );
				hConf = var_load( fname );
					
				v = var_get( hConf, "log" );
				strdcpy( &s, v, itod(0, null), " : Envoi par MFT vers ", partname, ":", v1, " OK" BR, _ );
				var_set( hConf, "log", s );
				var_save( hConf, fname );
				print( "OK" BR, _ );
				return OK;
				}
			}
		print( "ERREUR : Le transfert ne se termine pas" BR, _ );
		r = mft_util( strdcpy( &s, "delete part=*,direct=send,ida=", s2, _ ), &s3 );
		return ERROR;
		}
*/
	
	// AFFICHE LES INFO SUR L'EXPORT DEMANDE
	
	tr( "cmd_exporte_28" );
	
	strdcpy( &fname, direxport, "/", appname, "_", export_id, ".dat", _ );
	if ( hConf ) var_free( hConf );
	hConf = var_load( fname );
	
	if ( !b )
		{
		tr( "cmd_exporte_29" );
		
		print( "export_id   = ", export_id, BR, _ );
		print( "export_date = ", var_get( hConf, "export_date" ), BR, _ );
		print( "export_user = ", var_get( hConf, "export_user" ), BR, _ );
		print( "flux_liste  = ", v=var_get( hConf, "flux_liste" ), BR, _ );
		print( "descr       = ", var_get( hConf, "export_descr" ), BR, _ );
		print( "pti         = ", var_get( hConf, "export_pti" ), BR, _ );
		print( "log         = ", var_get( hConf, "log" ), BR, _ );

		ts = str_explode( v, "," );
		for( i = 0 ; (ts)&&(ts[i]) ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			print( ts[i], " = ", var_get( hConf, ts[i] ), BR, _ );
			tr( "cmd_exporte_30" );
			
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
		tr( "cmd_exporte_31" );
		print( "{export_date:", str_quote( var_get( hConf, "export_date" )),
			",export_user:", str_quote( var_get( hConf, "export_user" )),
			",flux_liste:", str_quote( v=var_get( hConf, "flux_liste" )),
			",export_descr:", str_quote( var_get( hConf, "export_descr" )),
			",export_pti:", str_quote( var_get( hConf, "export_pti" )),
			",export_log:", str_quote( var_get( hConf, "log" )),
			_ );

		print( ",flux:[", _ );
		ts = str_explode( v, "," );
		for( i = 0 ; (ts)&&(ts[i]) ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			tr( "cmd_exporte_32" );
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
		print("]}", _ );
		}
	
	tr( "cmd_exporte_33." );
	}


















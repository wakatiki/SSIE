#include "ssie7.h"

char   *flux_etape = null;
int		etape_no = 1, etape_log_id;
char	bFluxStarted = false;

// FONCTIONS D'EXPLOITATION DES FLUX SSIE7

//------------------------------------------------------------------------------
int flux_start( T_WORK* t_work )

// DEMARRAGE D'UN FLUX SSIE
//------------------------------------------------------------------------------
	{
	// OBTENTION D'UN FLUX_ID ET INITIALISATION DE LA LOG
	
	tr( "flux_start1" );
	mkdir( dirflux );
	
	hLog = hlog_init( dirflux, null );
	tr( "flux_start2" );
	if ( hLog == ERROR )
		{
		tr( "flux_start2a" );
		print( "ERREUR : init hlog" BR, _ );
		return ERROR;
		}
	tr( "flux_start2b" );
	
	strdcpy( &flux_id, hlog_getid( hLog ), _ );
	tr2( "flux_start2c:",flux_id );

	hlogu( hLog, 0, "Démarrage du flux", flux_id, ":", appname, "/", fluxname, _ );
	tr( "flux_start3" );
	hlogc( hLog, 0, "", "Démarrage demandé par ", user, ", flux_id = ", flux_id, _ );
	
	tr( "flux_start4" );
	
	// INITIALISATION DU FLUX
	
	var_set( hFlux, "app_name", appname );
	var_set( hFlux, "flux_name", fluxname );
	var_set( hFlux, "logr", "0" );
	logroot = 0;
	
	var_set( hFlux, "_FLUX_ID", flux_id );
	var_set( hFlux, "var_list", ",FLUX_ID," );
	var_set( hFlux, "dt_start", itoa(time(null)) );
	
	tr( "flux_start5" );
	
	if ( h ) var_free( h );

	tr( "flux_start6" );

	h = var_load( strdcpy( &fname, dircflux, "/", appname, ".dat", _ ) );
	v = var_get( h, "app_login" );
	var_set( hFlux, "app_login", (char*)((v)?v:"") );

	tr( "flux_start7" );
	
	strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".dat", _ );
	var_save( hFlux, fname );


	// DEMARRE LE FLUX

	strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".run", _ );
	file_save_str( fname, "", 0 );
	
	print( "OK: flux_id = ", flux_id, BR, _ );
	bFluxStarted = true;
	
	return OK;
	}




//------------------------------------------------------------------------------
int flux_stop( T_WORK * t_work )

// FORCE L'ARRET D'UN FLUX SSIE
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".run", _ );
	strdcpy( &fname1, fname, ".lock", _ );
	hLock = file_lock( fname1 );
	
	if ( s ) s[0] = 0;
	file_load( &s, fname );
	for( file_delete( fname ) ; file_exists( fname ) ; Sleep(100) )
		file_delete( fname );
	
	file_unlock( hLock );
	unlink( fname1 );

	strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".log.*.RUN", _ );
	file_delete( fname );
	
	p = atoi( s );
	if ( p ) process_kill( p );


	hLog = hlog_init( dirflux, flux_id );
	// SUPPRIME LES EVENTUELS PROCESS D'EMISSION
	v = var_get( hFlux, "send_to_surv" );
	if ( v )
		{
		if ( ts ) free( ts );
		ts = str_explode( v, "|" );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			if ( ts2 ) free( ts2 );
			ts2 = str_explode( ts[i], "," );
			id = atoi( ts2[0] );
			strdcpy( &ida, ts2[1], _ );
			
			
			hlogr( hLog, id, &s, null );
			if ( !strnicmp( s, "ERREUR", 6 ) )	continue;
			if ( !strnicmp( s, "ERROR", 5 ) )	continue;
			if ( !strnicmp( s, "OK", 2 ) )		continue;
			strdcpy( &s, argv[0], " mft cancel ida=", ida, _ );
			#ifdef OS_UNIX
			//strdcpy(&ida,"E4C00060",_);//sni
			if (strlen(ida) < 8) logw("FLUX: ida vide",_);
					else {
					System( &s1, s );
					//on recupere le pid
					System( &s, "echo $(($$-1))" );
					trim(s);
					process_kill( atoi(s) );
					}
			//logw("FLUX: cancel de ida=",ida,"/res=",s1,_);//sni a enlever
			#else
			System( &s1, s );	
			#endif
			}
		}



	// SUPPRIME LES EVENTUELS FICHIERS DE TRAVAIL
	
	
	// MET A JOUR LA LOG
	
	hlogc( hLog, 0, "", "ARRET DEMANDE PAR ", user, _ );

	n = 1;

	if ( v = var_get( hFlux, "err_nb" ) ) n = atoi(v) + 1;
	var_set( hFlux, "flux_canceled", "true" );
	var_set( hFlux, "err_nb", itoa(n) );
	var_save( hFlux, strdcpy( &fname, dirflux, "/", flux_id, ".dat", _ ) );

	hlogc( hLog, 0, "ANNULE", "FIN DES TRAITEMENTS", _ );
	hlogu( hLog, 0, "ANNULE", _ );

	return OK;
	}





//------------------------------------------------------------------------------
int flux_dupli_start( T_WORK* t_work )

// DEMARRAGE RAPIDE D'UN FLUX PAR DUPLICATION D'UN AUTRE
//------------------------------------------------------------------------------
	{
	// flux_id contient lID du flux à dupliquer.
	
	if ( var_get( hReq, "process_restarted" ) )
		{
		flux_execute( t_work );
		return OK;
		}
	
	// OBTENTION D'UN FLUX_ID

	hLog = hlog_init( dirflux, null );
	v1 = hlog_getid( hLog );
	
	// DUPLICATION DU FLUX
	
	strdcpy( &fname1, dirflux, DIR_CHAR, flux_id, ".dat", _ );
	strdcpy( &fname2, dirflux, DIR_CHAR, v1, ".dat", _ );
	file_copy( fname1, fname2 );
	
	strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".lo*", _ );

	tf = NULL;
	while( tf = file_list( tf, fname, "nodir absdir" ) )
		{
		strdcpy( &fname1, dirflux, DIR_CHAR, tf->name, _ );
		strdcpy( &fname2, dirflux, DIR_CHAR, v1, &(tf->name[strlen(flux_id)]), _ );
		file_delete( fname2 );
		file_copy( fname1, fname2 );
		}
	
	
	// MISE A JOUR DE CERTAINES INFORMATIONS
	
	strdcpy( &flux_id, v1, _ );
	strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".dat", _ );
	if ( hFlux ) var_free( hFlux );
	hFlux = var_load( fname );
	var_set( hFlux, "_FLUX_ID", flux_id );
	var_set( hFlux, "flux_do_restart", "false" );
	var_set( hFlux, "dt_start", itoa(time(null)) );
	var_save( hFlux, fname );

	hLog = hlog_init( dirflux, flux_id );
	hlogu( hLog, 0, null, flux_id, ":",
			var_get( hFlux, "app_name" ), "/", var_get( hFlux, "flux_name" ), _ );
	hlogu( hLog, 1, "", "Démarrage demandé par ", user, ", flux_id = ", flux_id, _ );

	
	// PLACE LE FLUX DANS LA LISTE DES FLUX EN COURS
	
	strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".run", _ );
	file_save_str( fname, "", 0 );
	
	return OK;
	}




//------------------------------------------------------------------------------
int flux_execute( T_WORK* t_work )

// EXECUTION ETAPE PAR ETAPE D'UN FLUX
//------------------------------------------------------------------------------
	{
	// CHARGE LES INFO DU FLUX
	tr("flux_execute_1"); tr2( "date=", currdate("log") ); tr2( "flux_id=", flux_id );
	strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".run", _ );
	if ( !file_exists( fname ) ) return OK;

	strdcpy( &fname1, dirflux, DIR_CHAR, flux_id, ".log.*.RUN", _ );
	if ( file_count( fname1 ) == 0 )
		{
tr("flux_execute_1a");
		unlink( fname );
		return OK;
		}
	
	strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".dat", _ );
	if ( hFlux ) var_free( hFlux );
	hFlux = var_load( fname );
	if ( !hFlux )
		{
		tr("flux_execute_2");
		print( "ERREUR : Le flux demandé n'existe pas.", BR, _ );
		return ERROR;
		}
	
	if ( var_get( hFlux, "flux_canceled" ) )
		{
tr("flux_execute_2a");
		strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".run", _ );
		for( file_delete( fname ) ; file_exists( fname ) ; Sleep(100) )
			file_delete( fname );
		return ERROR;
		}
		
	// FLUX EN ATTENTE D'UNE DATE ?

	if ( v = var_get( hFlux, "wait_date" ) )
		{
		tr2("flux_execute_3:", itoa(sysdate) );
		tr2("flux_execute_3a:", v );
		if ( sysdate < atoi(v) ) return OK;
		var_delete( hFlux, "wait_date" );
		}
	
	
	// LECTURE INFO GENERALES
	
	tr("flux_execute_4");
	appname = var_get( hFlux, "app_name" );
	fluxname = var_get( hFlux, "flux_name" );
	logroot = atoi(var_get( hFlux, "logr" ));
	
	
	// BOUCLE SUR LES ETAPES

	v = var_get( hFlux, "etape_no" );
	if ( v ) etape_no = atoi( v );
	else etape_no = 1;
	
	for( ; ; etape_no++ )
		{
		tr("flux_execute_5");

		// CHARGE LES INFORMATIONS SUR L'ETAPE EN COURS
		
		strdcpy( &flux_etape, itoa(etape_no), _ );

		v = var_get( hFlux, strdcpy( &s1, "etape_", fluxname, "_", flux_etape, "_conf_fname", _ ) );

//hlogc( hLog, 0, "", "etape_begin 0 : ", flux_etape, " : ", s1, _ );
		if ( !v )
			{
			tr("flux_execute_6");
			// L'ETAPE N'A PAS ENCORE ETE INITIALISEE
//hlogc( hLog, 0, "", "etape_begin 1", _ );
			
			var_set( hFlux, "etape_no", flux_etape );
			var_set( hFlux, strdcpy( &s, "etape_", flux_etape, "_dt_start", _ ), itoa(time(null)) );
			
			strdcpy( &fname, dircflux, "/", appname, "_flux_", fluxname, "_etape_", flux_etape, ".dat", _ );
			if ( ! file_exists( fname ) )
				{
				tr("flux_execute_7");
				// PLUS D'ETAPES
				v = var_get( hFlux, "startinside_stack" );
				if ( ( !v )||( !( v[0]) ) )
					{
					flux_fin:
					tr("flux_execute_8");	
					// ON EST ARRIVE A LA FIN
					
					var_set( hFlux, "etape_type", "" );
					strdcpy( &s4, hlogc_last_status( hLog, logroot ), _ );

					
					// ENVOI LES AR S'IL Y EN A ET SI UNE ETAPE SENDACK EXISTE
					
					v = var_get( hFlux, "sendack_list" );
					if ( ( v )&&( (v[0]) ) )
						{
						tr("flux_execute_9");
						// RECHERCHE UNE ETAPE D'ENVOI D'AR
						for( i = etape_no+1 ; i < 10000 ; i++ )
							{
							tr("flux_execute_10");
							strdcpy( &fname, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(i), ".dat", _ );
							if ( ! file_exists( fname ) )
								break;
							tr("flux_execute_11");
							if ( hEtape ) var_free( hEtape );
							hEtape = var_load( fname );
							v = var_get( hEtape, "etape_type" );
							if ( strcmp( v, "sendack" ) ) continue;
							
							tr("flux_execute_12");
							strdcpy( &flux_etape, itoa(i), _ );
							var_set( hFlux, "etape_no", flux_etape );
							v1 = var_get( hEtape, "etape_descr" );
							var_set( hFlux, strdcpy( &s, "etape_", flux_etape, "_type", _ ), v );
							var_set( hFlux, strdcpy( &s, "etape_", flux_etape, "_conf_fname", _ ), fname );
							var_set( hFlux, strdcpy( &s, "etape_", flux_etape, "_retry_nb", _ ), "0" );
							etape_log_id = hlogc( hLog, logroot, "En cours", "Etape ", flux_etape, " : ", v1, _ );
							var_set( hFlux, strdcpy( &s, "etape_", flux_etape, "_log_id", _ ), itoa(etape_log_id) );
							hlogu_status( hLog, logroot, "En cours : ", v1, _ );

							var_set( hFlux, "stop_after_sendack", "true" );
							var_set( hFlux, "etape_next", "" );
							var_set( hFlux, "status_after_sendack", s3 );
							tr("flux_execute_13");
							if ( !sendack_execute( t_work ) ) goto fin;
							tr("flux_execute_14");
							break;
							}
						}

					tr("flux_execute_15");

					// CALCUL LE STATUS DE FIN
					
					if ( v = var_get( hFlux, "status_after_sendack" ) )
						if ( strnicmp( s4, "ERREUR", -1 ) ) strdcpy( &s4, v, _ );
					
					if ( !strnicmp( s4, "OK", 2 ) )
						{
						strdcpy( &s, "OK : ", _ );
						n1 = atoi( var_get( hFlux, "send_file_nb" ) );
						if ( n1 > 0 )
							{
							strdcat( &s, "emi[", itoa(n1), ",taille:", var_get( hConf, "send_file_size" )," (", _ );
//							strdncat( &s, v=var_get( hFlux, "send_file_slist" ), 100, _ );
//							strdcat( &s, (char*)(( strlen(v) > 100 )?"...":""),
							strdcat( &s, v=var_get( hFlux, "send_file_slist" ),
								")]", _ );
							}
						n2 = atoi( var_get( hFlux, "recv_file_nb" ) );
						if ( n2 > 0 )
							{
							strdcat( &s, (char*)((n1>0)?"<br>":""),
								"reçu[", itoa(n2), ",taille:", var_get( hConf, "recv_file_size" ), " (", _ );
//							strdncat( &s, v=var_get( hFlux, "recv_file_slist" ), 30, _ );
//							strdcat( &s, (char*)(( strlen(v) > 30 )?"...":""),
							strdcat( &s, v=var_get( hFlux, "recv_file_slist" ),
								")]", _ );
							}
						if ( n1+n2 )
							{
							strdcat( &s, "<br>part(", var_get( hConf, "part_list" ), ")", _ );
							v = s; s = s4; s4 = v;
							}
						}

					// REDEMARRE LE FLUX SI DEMANDE
					flux_restart( t_work );
					

					// FIN
					hlogc( hLog, logroot, "", "FIN DES TRAITEMENTS", _ );
				
					var_save( hFlux, strdcpy( &fname, dirflux, "/", flux_id, ".dat", _ ) );
					
					hlogu( hLog, logroot, s4, _ );
					
					// ENLEVE LE FLUX DES FLUX EN COURS
					strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".run", _ );
					for( unlink( fname ) ; file_exists( fname ) ; Sleep(1) )
						unlink( fname );
						
					return OK;
					}

				else
					{
					tr("flux_execute_16");
					// FIN D'UN START INSIDE
//hlogc( hLog, 0, "", "Fin exécute flux : ", fluxname, _ );

					// CALCUL LE STATUS DE FIN
					
					v1 = hlogc_last_status( hLog, logroot );
					hlog_raise_status( hLog, logroot, v1, _ );
					hlogu_status( hLog, hLog, 0, "Etape ", fluxname, " ", flux_etape, " : ", v1, _ );
					
					// DEPILE
					if ( ts ) free( ts );
					ts = str_explode( v, "|" );
					strdcpy( &s, "", _ );
					for( i = 1 ; ts[i] ; i++ )
						{
						if ( ts[i][0] == 0 ) continue;
						tr("flux_execute_17");
						strdcat( &s, ts[i], "|", _ );
						}
					var_set( hFlux, "startinside_stack", s );
//hlogc( hLog, 0, "", "startinside_stack = '", s, "'", _ );

					// RECUPERE LES INFORMATIONS DU FLUX APPELANT

					if ( ts2 ) free( ts2 );
					ts2 = str_explode( ts[0], "," );
					
					fluxname = str( ts2[0], _ );
					var_set( hFlux, "flux_name", fluxname );

					etape_log_id = logroot;
					logroot = atoi( ts2[1] );
//					etape = str( ts2[2], _ );
					flux_etape = str( ts2[3], _ );
					etape_no = atoi( flux_etape );
					var_set( hFlux, "flux_etape", flux_etape );

					hlogc( hLog, etape_log_id, "", "Retour à ", fluxname, ", etape=", flux_etape, _ );
					
					// ORIENTATION POUR LA SUITE
					
					if ( ( !strnicmp( v1, "ERREUR", -1 ) )
						||( !strnicmp( v1, "ALERT", -1 ) ) )
						on_error_execute( t_work );

					v1 = var_get( hFlux, "etape_next" );
			
					if ( ! strcmp( v1, "stop" ) )
						var_set( hFlux, "flux_etape", "-1" );
					
					continue;	
					}

				}
				
			// INITIALISATION DE L'ETAPE
//hlogc( hLog, 0, "", "etape_begin 3", _ );
			tr("flux_execute_18");
			
			if ( hEtape ) var_free( hEtape );
			hEtape = var_load( fname );
			
			v = var_get( hEtape, "etape_type" );
			if ( ! strcmp( v, "on_error" ) ) continue;
			tr("flux_execute_19");
			if ( ! strcmp( v, "waitend" ) ) continue;
			tr("flux_execute_20");
//			if ( ! strcmp( v, "waitack" ) ) continue;
			
			tr("flux_execute_21");
			var_set( hFlux, strdcpy( &s, "etape_", flux_etape, "_type", _ ), v );
			var_set( hFlux, strdcpy( &s, "etape_", fluxname, "_", flux_etape, "_conf_fname", _ ), fname );
			var_set( hFlux, strdcpy( &s, "etape_", flux_etape, "_retry_nb", _ ), "0" );
			var_set( hFlux, "etape_next", "" );

			v1 = var_get( hEtape, "etape_descr" );
			v2 = var_get( hFlux, "startinside_stack" );
			if ( ( !v2 )||( ! (v2[0]) ) )
				{
				tr("flux_execute_22");
				etape_log_id = hlogc( hLog, logroot, "En cours", "Etape ", flux_etape, " ", v, " : ", v1, _ );
				}
			else
				etape_log_id = hlogc( hLog, logroot, "En cours", "Etape ", fluxname, " ", flux_etape, " ", v, " : ", v1, _ );

			var_set( hFlux, strdcpy( &s, "etape_", flux_etape, "_log_id", _ ), itoa(etape_log_id) );
			var_save( hFlux, strdcpy( &fname, dirflux, "/", flux_id, ".dat", _ ) );\

			hlogu_status( hLog, logroot, "En cours : Etape ", flux_etape, " ", v, " : ", v1, _ );
			}
		else
			{
			tr("flux_execute_23");
			// L'ETAPE EST DEJA EN COURS

			if ( hEtape ) var_free( hEtape );
			hEtape = var_load( v );

			etape_log_id = atoi(var_get( hFlux, strdcpy( &s, "etape_", flux_etape, "_log_id", _ ) ));

//			if ( var_get( hReq, "process_restarted" ) )
//				hlogc( hLog, etape_log_id, "", "REPRISE DES TRAITEMENTS SUITE A INCIDENT", _ );
				
			v = var_get( hFlux, strdcpy( &s, "etape_", flux_etape, "_type", _ ) );
			}
		
		tr("flux_execute_24");
//		hlogu_status( hLog, etape_log_id, "En cours", _ );

		// EXECUTE L'ETAPE
		
		if ( ! strcmp( v, "script" ) )		{ if ( !script_execute( t_work ) ) goto fin; }
		else
		if ( ! strcmp( v, "planif" ) )		{ if ( !planif_execute( t_work ) ) goto fin; }
		else
		if ( ! strcmp( v, "waitfile" ) )	{ if ( !waitfile_execute( t_work ) ) goto fin;}
		else
		if ( ! strcmp( v, "start" ) )		start_execute( t_work );
		else
		if ( ! strcmp( v, "startinside" ) )	startinside_execute( t_work );
		else
		if ( ! strcmp( v, "emi" ) )			{ if ( ! emi_execute( t_work ) ) goto fin; }
		else
		if ( ! strcmp( v, "rec" ) )			{ if ( ! rec_execute( t_work ) ) goto fin; }
		else
		if ( ! strcmp( v, "waitack" ) )		{ if ( !waitack_execute( t_work ) ) goto fin;}
		else
		if ( ! strcmp( v, "sendack" ) )		{ if ( ! sendack_execute( t_work ) ) goto fin; }

		tr("flux_execute_25");
		v1 = var_get( hFlux, "etape_next" );
//hlogc( hLog, 0, "", "etape_next ", v1, _ );
		
		if ( ! strcmp( v1, "stop" ) )
			goto flux_fin;
		else
		if ( ! strncmp( v1, "retry", -1 ) )
			{
			tr("flux_execute_26");
			strdcpy( &s3, "etape_", flux_etape, "_timeout", _ );
			var_delete( hFlux, s3 );
			
			etape_no--;
			}
		else
		if ( v1[0] == '+' )
			{
			tr("flux_execute_27");
			var_set( hFlux, "etape_no", flux_etape = itoa(etape_no + atoi(&(v1[1]))) );
			var_delete( hFlux, strdcpy( &s1, "etape_", flux_etape, "_conf_fname", _ ) );
			etape_no = atoi( flux_etape ) - 1 ;
			}
		else
		if ( v1[0] == '-' )
			{
			tr("flux_execute_28");
			var_set( hFlux, "etape_no", flux_etape = itoa(etape_no - atoi(&(v1[1]))) );
			var_delete( hFlux, strdcpy( &s1, "etape_", flux_etape, "_conf_fname", _ ) );
			etape_no = atoi( flux_etape ) - 1 ;
			}
		else
		if ( is_numeric( v1 ) )
			{
			tr("flux_execute_29");
			var_set( hFlux, "etape_no", flux_etape = itoa(atoi(v1)) );
			var_delete( hFlux, strdcpy( &s1, "etape_", fluxname, "_", flux_etape, "_conf_fname", _ ) );
			etape_no = atoi( flux_etape ) - 1 ;
			}
			
		var_save( hFlux, strdcpy( &fname, dirflux, "/", flux_id, ".dat", _ ) );
		continue;
		}
	
	//--------------------------------------------------------------------------
	fin :
		tr("flux_execute_30");
		var_save( hFlux, strdcpy( &fname, dirflux, "/", flux_id, ".dat", _ ) );

		// ENLEVE LE PROCESS EN COURS DE LA LISTE DES PROCESS A SURVEILLER
//		unlink( strdcpy( &fname, dirpsurv, DIR_CHAR, itoa( getpid() ), _ ) );
					
		return OK;
		
	}


//------------------------------------------------------------------------------
int flux_restart( T_WORK * t_work )

// REDEMARRAGE DU FLUX EN COURS
//------------------------------------------------------------------------------
	{
	tr("flux_restart_1");
	
	// VERIFIE SI LE FLUX DOIT ETRE REDEMARRE

	v = var_get( hFlux, "flux_restarted" );
	if ( ( v )&&( ! strcmp( v, "true" ) ) )
		return OK;

	v = var_get( hFlux, "waitfile_verify" );
	if ( ( v )&&( ! strcmp( v, "true" ) ) )
		{
		tr("flux_restart_2");
		strdcpy( &fname, dirtemp, DIR_CHAR "WF_", flux_id, _ );
		
		#ifdef OS_UNIX
		v2 = trim( var_get( hFlux, "app_login" ) );
		if (( v2 )&&( (p=strpos( v2, ":", 0 )) >= 0 ))
			v2 = strdncpy( &u6, v2, p, _ );
				
		if (( v2 )&&( v2[0] )&&( strcmp( v2, ssie_user ) ))
			{
			strdcpy( &s, "sudo ", dirbin, "/ssie7 chown -R ", ssie_user, ":", ssie_user,
							 " ", fname, _ );
			System( &s1, s );
			}
		#endif
				
		if ( file_count( fname ) > 0 )
			{
			tr("flux_restart_3");
			strdcpy( &s, "ALERTE : WaitFile : Fichiers reçus toujours présents dans : ", fname, _ );
			hlogc( hLog, logroot, s, "", _ );
			if ( !strnicmp( s4, "OK", -1 ) ) strdcpy( &s4, s, _ );
			}
		else
			file_delete( fname );
			
//		return OK;
		goto restart;
		}

	v = var_get( hFlux, "flux_do_restart" );
	if ( ( !v )||( ! strcmp( v, "false" ) ) ) return OK;
	
	tr("flux_restart_4");	
	
	v1 = var_get( hFlux, "app_login" );
	if (( v1 )&&( (p=strpos( v1, ":", 0 )) >= 0 ))
		v1 = strdncpy( &u6, v1, p, _ );
		
	// VERIFIE SI ETAPE WAITFILE QUE LES FICHIERS ONT ETE SUPPRIMES
	
	if ( ( v = var_get( hFlux, "wait_file_list" ) ) != null )
		{
		tr("flux_restart_5");
		ts = str_explode( v, "\n" );
		b1 = true;
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( !( ts[i][0] ) ) continue;
			tr("flux_restart_6");
			ts2 = str_explode( ts[i], "|" );
			for( j = 0 ; ts2[j] ; j++ )
				{
				if ( !( ts2[j][0] ) ) continue;
				
				tr("flux_restart_7");

				#ifdef OS_UNIX
				if ( ( v1 )&&( v1[0] )&&( strcmp( v1, sysuser() ) ) ) // sudo
					{
					tr("flux_restart_8");
					System( &s3, strdcpy( &s2, "sudo ", dirbin, "/ssie7 ls -l ", ts2[j], _ ) );

					if ( ( stripos( s3, "No such file or directory", 0 ) >= 0 )||( stripos( s3, "not found", 0 ) >= 0 ) )
						{ tr("flux_restart_9"); b1 = false; break; }
					
					continue;
					}
				#endif

				if ( ( strpos( ts2[j], "*", 0 ) >= 0 )||( dir_exists( ts2[j] ) ) )
					{
					tr("flux_restart_10");
					if ( file_list( NULL, ts2[j], "nodir adir nohide" ) == NULL )
						{ tr("flux_restart_11"); b1 = false; break; }
					}
				else
					if ( ! file_exists( ts2[j] ) )
						{ tr("flux_restart_12");b1 = false; break; }
				}
			if ( b1 == false ) break;
			tr("flux_restart_13");
			}
		
		if ( b1 == true )
			{
			tr("flux_restart_14");
			hlogc( hLog, 0, "ALERTE : Les fichiers reçus par le premier waitfile "
					"n'ont pas été supprimés ou déplacés.\n"
					"Le redémarrage du flux n'est pas possible.", "", _ );
			if ( !strnicmp( s4, "OK", -1 ) )
				{
				tr("flux_restart_15");
				strdcpy( &s4, "ALERTE : Fichiers non supprimés => Redémarrage impossible", _ );
				}
			return OK;
			}
		}
	
	// REDEMARRAGE
	
	restart:
		
	v = hlogc_last_status( hLog, 0 );
	
	v1 = var_get( hFlux, "startinside_stack" );
	if (( v1 )&&( v1[0] ))
		{
		tr("flux_restart_16");
		ts = str_explode( v1, "|" );
		for( i = 0 ; ts[i] ; i++ );
		i--;
		if ( ts[i][0] == 0 ) i--;
		p =strpos( ts[i], ",", 0 );
		strdncpy( &s2, ts[i], p, _ );
		}
	else
		strdcpy( &s2, fluxname, _ );
	strd_replace( &s2, "@", "/" );
	
	id = hlogc( hLog, 0, "", "Redémarre le flux", ":", s2, _ );
	
/*
	strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".run", _ );
	for( file_delete( fname ) ; file_exists( fname ) ; Sleep(100) )
		file_delete( fname );
*/
		
	bFluxStarted = true;
	n = 10;
	while(1)
		{
		tr("flux_restart_17");
		System( &s1, strdcpy( &s3, argv[0], " flux ", appname, " ", s2,
								" restart=", itoa(n), " start" " no_verify", _ ) );
		hlogu( hLog, id, s1, _ );
		if (( strpos( s1, "ERREUR", 0 ) < 0 )&&( strpos( s1, "ERROR", 0 ) < 0 ))
			{
			var_set( hFlux, "flux_restarted", "true" );
			return OK;
			}
		tr("flux_restart_18");
		n--;
		if ( !n )
			return OK;

		sleep(1);
		}
	}


























#include "ssie7.h"

extern char  /* *etape,*/ *flux_etape;
extern int etape_no, etape_log_id;



//------------------------------------------------------------------------------
int waitfile_execute( T_WORK* t_work )

// EXECUTE UNE ETAPE DE FLUX DE TYPE WAITFILE (Attente de fichier dans un répertoire)
//------------------------------------------------------------------------------
	{
	tr("waitfile_execute_1");
	//--------------------------------------------------------------------------
	// INITIALISATION DE L'ETAPE WAIT_FILE SI PAS DEJA FAIT
	//--------------------------------------------------------------------------

	if ( v = var_get( hFlux , "wait_file_list" ) )
		strdcpy( &s4, v, _ );
	else
		{
		tr("waitfile_execute_2");
		
		// LECTURE DES FICHIERS A ATTENDRE
		
		v = var_get( hEtape, "etape_wait_fname" );
		if ( ( !v )||( !( v[0] ) ) )
			{
			error_bad_param:
			tr("waitfile_execute_3");
			hlog_raise_status( hLog, etape_log_id, "ERREUR : aucun fichier à attendre n'a été définit", _ );
			hlogu_status( hLog, 0, "Etape ", flux_etape, " : ERREUR : aucun fichier à attendre n'a été définit", _ );
			var_set( hFlux, "etape_next", "stop" );
			var_set( hFlux, "flux_do_restart", "false" );
			return true;
			}
			
		strdcpy( &s4, v, _ );
		var_list_replace( hFlux, &s4 );
		trim( s4 );
		if ( s4[0] == '[' )
			{
			p = strpos( s4, "]", 1 );
			if ( p >= 0 )
				{
				n = atoi( &(s4[1]) );
				if ( n > 0 )
					{
					var_set( hFlux, "wait_file_interval", itoa(n) );
					strdcpy( &s3, &(s4[p+1]), _ );
					trim( s3 );
					v = s4; s4 = s3; s3 = v;
					}
				}
			}
		
		if ( s4[0] == 0 ) goto error_bad_param;
		
		hlogc( hLog, etape_log_id, "", "Attend : <b>", s4, "</b>", _ );
		var_set( hFlux, "wait_file_list", s4 );

		// CALCUL L'HEURE LIMITE
		
		v4 = var_get( hEtape, "etape_horaire" );
		if ( ( v4 )&&( v4[0] ) )
			{
			tr("waitfile_execute_4");
			v1 = var_get( hFlux, strdcpy( &s, "etape_", flux_etape, "_dt_start", _ ) );
			n = atoi( v1 );
			n1 = n+(2*24*60*60);
	
			strdcpy( &s2, v4, _ );
			var_list_replace( hFlux, &s2 );
			strd_replace( &s2, ",", " " );
			strd_ireplace( &s2, "h", ":" );
			ts = str_explode( s2, " " );
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( ! ts[i][0] ) continue;
				tr("waitfile_execute_5");
				if ( ts2 ) free( ts2 );
				ts2 = str_explode( ts[i], ":" );
				
				strdcpy( &s, itod( time(NULL), "jj/mm/aaaa" ), " ",
					ts2[0], ":", (char*)((ts2[1]) ? ts2[1] : "00" ), ":00", _ );
				
				n2 = dtoi( s, "jj/mm/aaaa hh:mm:ss" );
				
				if ( n2 <= n ) // date limite antérieure à la date de démarrage
					n2 += (24*60*60); // On ajoute une journée;
				
				if ( n1 > n2 ) n1 = n2;
				}
			
			if ( n1 < (n+(2*24*60*60)) )
				{
				tr("waitfile_execute_6");
				v4 = itoa( n1 );
				var_set(  hFlux, "date_timeout", v4 );
				hlogc( hLog, etape_log_id, "", "heure limite=", itod( n1, "jj/mm/aaaa hh:mm:ss" ), _ );
				}
			else
				var_set(  hFlux, "date_timeout", "0" );
			}
		else
			var_set(  hFlux, "date_timeout", "0" );
		

		// DEMANDE LE REDEMARRAGE AUTOMATIQUE
		
		tr("waitfile_execute_7");
		
		v = var_get( hFlux, "flux_do_restart" );
		if ( !v )
			var_set( hFlux, "flux_do_restart", "true" );


		
		// PLACE LE FLUX EN ATTENTE
	
//		hlog_raise_status( hLog, etape_log_id, "ATTEND le(s) fichier(s) : ", s4, _ );
		hlogu_status( hLog, 0, "Etape ", flux_etape, " : waitfile : ATTEND le(s) fichier(s) : ", s4, _ );
/*			
		strdcpy( &fname, dirrflux, DIR_CHAR, flux_id, _ );
		fp = fopen( fname, "wb" );
		if ( !fp )
			{
			hlog_raise_status( hLog, etape_log_id, "ERREUR : Création fichier ", fname, _ );
			var_set( hFlux, "etape_next", "stop" );
			return true;
			}
	
		fprint( fp, "wait_file:", fname1, _ ); // L'écriture fichier permettra d'enlever le flux dans la liste des process en attente
		fclose( fp );
*/		
//		return false;
		}


	//--------------------------------------------------------------------------
	// VERIFIE LA PRESENCE DES FICHIERS DEMANDES
	//--------------------------------------------------------------------------
	// s4 contient la liste des fichiers à attendre
	
	tr("waitfile_execute_8");
	
	if ( ts1 ) free( ts1 );
	ts1 = str_explode( s4, "\n" );
	for( i = 0 ; ts1[i] ; i++ )
		{
		if ( ts1[i][0] == 0 ) continue;
		
		tr("waitfile_execute_9");tr2( "file_count:", ts1[i] );
		#ifdef OS_WINDOWS
		if ( file_count( ts1[i] ) == 0 ) break;
		#else
		v = trim(var_get( hFlux, "app_login" ));
		if (( v )&&( (p=strpos( v, ":", 0 )) >= 0 ))
			v = strdncpy( &u6, v, p, _ );
		if (( v )
			&&( v[0] )&&( strcmp( v, sysuser() ) ))
			{ // sudo
			tr("waitfile_execute_10");
			strdcpy( &s, "sudo ", dirbin, "/ssie7 su - ", v, " -c \"\\\""
				"ls -l ", ts1[i],
				" | awk '{ if ((substr(\\\\\\$1,1,1) != \\\\\\\"d\\\\\\\")"
				"&&(substr(\\\\\\$1,1,1) != \\\\\\\"l\\\\\\\")&&( \\\\\\$9 != \\\\\\\"\\\\\\\" )) print \\\\\\$9 }'"
				"\\\"\"", _ );
			//strdcpy( &s,"tail -f /tmp/hhh &" ,_);
			System( &u5, s );
			// on recupere le pid
			System( &s, "echo $(($$-1))" );
			trim(s);
			trim( u5 );
			//Kill les process une fois l'information recuperee
			process_kill( atoi(s) );

			if (( stripos( u5, "No such file", 0 ) >= 0 )
					||( stripos( u5, "not found", 0 ) >= 0 ) )
				break;
			logw("waitfile: pki_test pid=",s,_);
            }
		else
		if ( file_count( ts1[i] ) == 0 ) break;

		if ( !file_is_free( ts1[i] ) ) break;	// Fichier occupé par un autre processus
		#endif
		tr("waitfile_execute_11");
		}
	
	
	//--------------------------------------------------------------------------
	// Pas de fichier ? => Vérifie le timeout
	//--------------------------------------------------------------------------
	
	if ( ts1[i] )
		{
		traite_timeout:
		tr("waitfile_execute_12");
			
		n = atoi( var_get( hFlux, "date_timeout" ) );
		if ( ( !n )||( n >= time(NULL) ) )
			{
			tr("waitfile_execute_13");
			if ( v = var_get( hFlux, "wait_file_interval" ) )
				var_set( hFlux, "wait_date", itoa( sysdate + (atoi(v)*60) ) );
			else
				var_set( hFlux, "wait_date", itoa( sysdate + 5 ) ); // Scanne toutes les 5 secondes
			return false;
			}

		hlogu_status( hLog, etape_log_id, "ERREUR : La date limite de réception est dépassée", _ );
		on_error_execute( t_work );
		v1 = var_get( hFlux, "etape_next" );
		if ( ( !v1 )||( !(v1[0]) ) )
			{
			tr("waitfile_execute_14");
			var_set( hFlux, "etape_next", "stop" );
			}

		return true;
		}


	//--------------------------------------------------------------------------
	// Tous les fichiers sont arrivés
	// ISOLEMENT DES FICHIERS ?
	//--------------------------------------------------------------------------

	hlogc( hLog, etape_log_id, "", "Fichiers reçus", _ );
		
	if (( v = var_get( hEtape, "etape_isolate_files" ) )
			&&( !stricmp( v, "Oui" ) ))
		{
		tr("waitfile_execute_15");
		// CREATION REPERTOIRE D'ISOLEMENT
		v1 = strdcpy( &fname1, dirtemp, DIR_CHAR "WF_", flux_id, _ );
		mkdir( fname1 );
		if ( !dir_exists( fname1 ) )
			{
			tr("waitfile_execute_16");
			hlogc( hLog, etape_log_id, "ERREUR : Création répertoire d'isolement",
							fname, " : ", strerror( errno), _ );
			hlogu_status( hLog, etape_log_id, "ERREUR : WaitFile : Création répertoire d'isolement", _ );
			var_set( hFlux, "etape_next", "stop" );
			var_set( hFlux, "flux_do_restart", "false" );
			return true;
			}
		
		#ifdef OS_UNIX	
		v2 = strdcpy( &s4, trim( var_get( hFlux, "app_login" ) ), _ );
		if ( (!v2)||( !(v2[0]) ) ) v2 = v3 = ssie_user;
		else
		if ( (p=strpos(v2,":",0)) >= 0 )
			{
			v2[p] = 0;
			v3 = &(v2[p+1]);
			}
		else
			v3 = v2;
/*
		strdcpy( &cmd, "sudo ", dirbin, "/ssie7 chown -R ", v2, ":", v3, " ", fname1, _ );
		system( cmd );
		strdcpy( &cmd, "sudo ", dirbin, "/ssie7 chmod -R 700 ", fname1, _ );
		system( cmd );
*/
		#endif
		
		// Crée la variable $WF_DIR

		var_set( hFlux, "_WF_DIR", fname1 );

		v = var_get( hFlux, "var_list" );
		if ( !v )
			{
			tr("waitfile_execute_17");
			var_set( hFlux, "var_list", strdcpy( &s1, ",WF_DIR,", _ ) );
			}
        else
            {
			tr("waitfile_execute_18");
			if ( stripos( v, strdcpy( &s1, ",WF_DIR", _ ), 0 ) < 0 )
               	var_set( hFlux, "var_list", strdcpy( &s1, ",WF_DIR", v, _ ) );
            else
               	{
				tr("waitfile_execute_19");
				if ( stripos( v, strdcpy( &s1, ",WF_DIR,", _ ), 0 ) < 0 )
	               	var_set( hFlux, "var_list", strdcpy( &s1, v, "WF_DIR,", _ ) );
				}
            }
		
		var_list_prepare( hFlux );

		// ISOLEMENT DES FICHIERS
		
		for( i = 0 ; ts1[i] ; i++ )
			{
			if ( ts1[i][0] == 0 ) continue;
			tr("waitfile_execute_20");
 /*
            #ifdef OS_UNIX
			if ( ts[3][0] )
				{ // sudo
				strdcpy( &cmd, "sudo ", dirbin, "/ssie7 su - ", ts[3], " -c \"\\\""
					"ls -ld ", ts1[i],
					" | wc -l\\\"\"", _ );
    			System( &s, cmd );
    
    			if ( atoi(s) == 0 ) break;
				}
			else
            #endif
   				if ( file_count( ts1[i] ) == 0 ) break;
*/
			tr2( "file_move ", ts1[i] );
			tr2( "=> ", fname1 );
			
			#ifdef OS_WINDOWS
			file_move( ts1[i], fname1 );
			#else
			strdcpy( &cmd, "sudo ", dirbin, "/ssie7 mv -f ", ts1[i], " ", fname1, _ );
			system( cmd );
//			System( &s, cmd );
			#endif
			}
			
		#ifdef OS_UNIX
		// Change les droits pour pouvoir les compter
		strdcpy( &cmd, "sudo ", dirbin, "/ssie7 chown -R ", sysuser(), ":", sysuser(), " ", fname1, _ );
		system( cmd );
//		System( &s, cmd );
		#endif

		
		tr("waitfile_execute_21");
		
		if ( file_count( fname1 ) == 0 ) goto traite_timeout;

		#ifdef OS_UNIX
		if ( strcmp( v2, sysuser() ) )
			{
			tr("waitfile_execute_22");
			strdcpy( &cmd, "sudo ", dirbin, "/ssie7 chown -R ", v2, ":", v3, " ", fname1, _ );
			system( cmd );
//			System( &s, cmd );
			}
		#endif
		
		
		// REDEMARRAGE DU FLUX DES MAINTENANT ?
/*	
		if (( v = var_get( hFlux, "flux_do_restart" ) )
				&&( !stricmp( v, "true" ) ))
			{
			tr("waitfile_execute_23");
			System( &s1, strdcpy( &s, argv[0], " flux \"",
					var_get( hFlux, "app_name" ), "\" \"",
					str_replace( var_get( hFlux, "flux_name"), "@", "/" ),
					"\" start no_verify", _ ) );
			hlogc( hLog, 0, "", "Redémarrage du flux :", s1, _ );
			if ( stripos( s1, "flux_id = ", 0 ) >= 0 )
				var_delete( hFlux, "flux_do_restart" );
			var_set( hFlux, "waitfile_verify", "true" );
			}
*/

		var_set( hFlux, "waitfile_verify", "true" );
		}

	
	//--------------------------------------------------------------------------
	// FIN ETAPE
	//--------------------------------------------------------------------------
	tr("waitfile_execute_24");
	
	hlogu_status( hLog, etape_log_id, "OK", _ );
	return true;
	}






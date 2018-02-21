
	//--------------------------------------------------------------------------
	// ROBOT DE TRAITEMENT DES TACHES RECURENTES
	//--------------------------------------------------------------------------
	
	if ( ! strcmp( argv[2], "rtask" ) )
	
	{
	#ifdef OS_UNIX
	int bSudoOk;
	bSudoOk = is_sudo_conf_ok();
	if ( !bSudoOk ) logw( "ROBOT : bSudo KO !!!", _ );
//	else logw( "ROBOT : bSudo OK", _ );
	#endif

	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_rtask.pid", _ );
	if ( file_load( &s, fname ) != sERROR )
		{
		if ( process_is_alive( atoi(s) ) ) ssie7exit(0);
		}
		
	file_save_str( fname, itoa(getpid()), -1 );
	
	for( n5 = n4 = 0 ; ; sleep(1), n5++ )
		{
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
		if ( file_exists( fname ) ) ssie7exit(0);
		
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_rtask.pid", _ );
		file_save_str( fname, itoa(getpid()), -1 ); // Pour surveillance de blocage
		
		if ( ! ((n5+1)%(45*60)) ) ssie7exit(0); // Redémarrage toutes les 45 minutes
		
		// server_conf_load( ); // Recharge la conf si le fichier de conf à changé

		//------------------------------------------------------------------
		// SURVEILLANCE DU SERVEUR HTTP
		//------------------------------------------------------------------

		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server_", v = var_get( hSrvConf, "port" ), ".pid", _ );
		if ( s ) s[0] = 0;
		file_load( &s, fname );
		if ( ( s )&&( s[0] )&&( !process_is_alive( atoi( s ) ) ) )
			{
			strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
			if ( !file_exists( fname ) )
				{
				// REDEMARRE LE PROCESS PRINCIPAL
				strdcpy( &cmd, argv[0], " server ", v, _ );
				StartH( cmd );
//				logw("ROBOT : serveur anormalement arrêté => Redémarrage : cmd=", cmd, _ );		
				}
			
			// SOUS UNIX, LES PROCESS SECONDAIRES SONT DIRECTEMENT SURVEILLES
			// PAR LE PROCESS PRINCIPAL
			}
		
		
		// SURVEILLANCE SERVEUR HTTP : Essai de se connecter
		
		if ( ( !(n5 % 60) )	// Toutes les minutes
				||( n4 ) )  // ou s'il y a eu un échec
			{
			strdcpy( &s3, dirwrk, DIR_CHAR "ping.log", _ );
			unlink( s3 );
			strdcpy( &s2, argv[0], " request 127.0.0.1:",
						v=var_get( hSrvConf, "port" ), " ping >", s3, _ );
			strdcpy( &s, dirwrk, DIR_CHAR "ping.bat", _ );
			file_save_str( s, s2, -1 );
			strdcpy( &s2, s, _ );
			#ifdef OS_UNIX
			strdcpy( &s, "chmod 700 ", s2, _ );
			system( s );
			#endif
			p = StartH( s2 );
			sleep(2);
			for( ; ; )
				{
				if ( ! process_is_alive( p ) )
					{
					if ( file_load( &s, s3 ) )
						{
						if ( strpos( s, "OK", 0 ) >= 0 )
							{
							n4 = 0;
							break; // Connexion OK
							}
						}
					}
				
				// La connexion n'a pas pu se faire
				process_kill( p );
				n4++;
				if ( n4 < 60 ) break; // Tentative pendant 2 minutes
				n4 = -30; // Laisse 3 minutes pour le redémarrage
				
				// Envoi email d'alerte
				
				//logw( "ROBOT : serveur : Pb de connexion ==> kill des process et redémarrage", _ );
				logw( "ROBOT : serveur : etat du serveur => rechargement de la configuration", _ );

				#ifdef OS_UNIX
				#if 0
				if ( v1 = var_get( hSrvConf, "adm_email" ) )
					{
					strdcpy( &fname, dirwrk, DIR_CHAR "sendmail", _ );
					if ( ( !file_exists( fname ) )||( (sysdate - file_date( fname )) > 15*60 ) )
						{
						strdcpy( &cmd, "echo \"\" | mail -s \"", itod( 0, "jj/mm/aaaa hh:mm:ss" ), " : ALERTE PROBLEME " MODULE_NAME
//										" $HOSTNAME ==> Veuillez vérifier, MERCI\" ", v1, _
								" $HOSTNAME ==> Redemarrage\" ", v1, _
								 );
					
						system( cmd );
						file_save_str( fname, "", 0 );
						}
					}
				#endif
			
				strdcpy( &cmd, argv[0], " server ", v, _ );
				while( process_count( cmd ) > 0 )
					process_kill_all( cmd );
					
				#else // OS_WINDOW
				
				strdcpy( &fname, dirwrk, DIR_CHAR, module_name,  "_server_", v, ".pid", _ );
				if ( s ) s[0] = 0;
				file_load( &s, fname );
				p1 = atoi( s );
				process_kill( p1 );
				
				#endif
				
				// ARRET DES PROCESSUS SERVER POUR REDEMARRAGE
				#ifdef OS_AIX
				sleep(15);// le temps de faire un A/R de ssie
				#endif

				break;
				}
			}



		//------------------------------------------------------------------
		// SURVEILLANCE DES PASSERELLES SYNCHRONE
		//------------------------------------------------------------------
		
		for( i = 0 ; v = var_getn( hSrvConf, i, &vn ) ; i++ )
			{
			if ( strnicmp( vn, "gateway_", 8 ) ) continue;
			p = atoi( &(vn[8]) );
			
			strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_gateway_", v=itoa( p ), ".pid", _ );
			
			for( ; ; )
				{
				if ( file_load( &s, fname ) == sERROR ) break;
				pid = atoi( s );
				if ( ! pid ) break;
				
				if ( ! process_is_alive( pid ) )
					{
					file_delete( fname );
					logw( "ROBOT : la passerelle synchrone ", v, " est anormalement arrêtée => Redémarrage", _ );

					strdcpy( &fname, dirwrk, DIR_CHAR "cmd_gateway_", v, ".mft", _ );
					if ( file_load( &cmd, fname ) )
						StartH( cmd );
					break;
					}
					
				if ( ! ( n5 % (30) ) )
					{
					strdcpy( &s, "127.0.0.1:", v, _ );
					if ( ( hCnx = tcp_connect( s ) ) != ERROR )
						{
						tcp_disconnect( hCnx );
						break;
						}
						
					process_kill( pid );
					#ifdef OS_LINUX
					sleep( 60 );
					#else
					sleep( 5 );
					#endif
					logw( "ROBOT : la passerelle synchrone ", v, " est inacessible => Redémarrage", _ );
					strdcpy( &fname, dirwrk, DIR_CHAR "cmd_gateway_", v, ".mft", _ );
					if ( file_load( &cmd, fname ) )
						StartH( cmd );
					}
				
				break;
				}
			}
		
		
		//------------------------------------------------------------------
		// CALCUL DES STATISTIQUES
		//------------------------------------------------------------------
		
		if ( ! ( (n5+2) % (3*60) ) ) // TOUTES LES 3 MINUTES
			{
			// CALCUL DES STATISTIQUES TOUTES LES 3 MINUTES
			if ( process_count( "report_compute" ) == 0 )
				StartH( strdcpy( &s2, argv[0], " report_compute", _ ) );
			}


		
		//------------------------------------------------------------------
		// PURGE TOUTES LES 30 MINUTES
		//------------------------------------------------------------------
		
		if ( !( (n5+3) % (30 Minutes) ) ) 
			{
			// PURGE DES LOG
			file_purge( dirlog, 30 );

			// PURGE DES FICHIERS RECUS NON TRAITE DEPUIS PLUS DE 30 JOURS
			
			v = var_get( hSrvConf, "mft_log_delay" );
			n = 30;
			if ( v ) n = atoi( v );
			file_purge( dirrecv, n );
			
			// PURGE DES FLUX
			strdcpy( &fname, dirflux, DIR_CHAR "*.*.END*", _ );
			strdcpy( &w, itod( sysdate - (30 Jours), "AAAAMMJJ" ), _ );
			tf = NULL;
			while( tf = file_list( tf, fname, "absdir nodir" ) )
				{
				//p = strpos( tf->name, ".", 0 );//bug
				p = strpos( tf->name, "log", 0 );
				if ( strncmp( &(tf->name[p+4]), w, 8 ) >= 0 ) continue;//4=strlen("log.")
				strdcpy( &s, itoa(atoi(tf->name)), _ );
				strdcpy( &fname, dirflux, DIR_CHAR, s, ".run", _ );
				if ( file_exists( fname ) ) continue; // Ne purge pas les flux encore en cours
				strcpy( &(fname[n = (strlen(fname)-3)]), "*" );
				logw("ROBOT: (purge des flux) Suppression des fichiers: ",fname,_);
				file_delete( fname );
				
				// Suppression répertoire
				if ( strlen( s ) > 3 )
					strdncpy( &fname, dirflux, -1, DIR_CHAR, 1,
							s, 3, DIR_CHAR, 1, &(s[3]), -1, _ );
				else
					strdcpy( &fname, dirflux, DIR_CHAR, s, _ );
				logw("ROBOT: (purge des flux) Supression du repertoire: ",fname,_);
				file_delete( fname ); // Suppression répertoire de log
				}
			
			// PURGE DU REPERTOIRE TEMP (SUR 3 JOURS)
			
			file_purge( dirtemp, 3 );


			// PURGE DES REPERTOIRE WF_{flux_id}
			
			strdcpy( &fname, dirtemp, DIR_CHAR "WF_*", _ );
			tf = NULL;
			while( tf = file_list( tf, fname, "absdir" ) )
				{
				strdcpy( &fname1, dirflux, DIR_CHAR, substr(tf->name, 3, -1 ), ".dat", _ );
				strdcpy( &fname2, fname1, ".tmp", _ );
				if (( ! file_exists( fname1 ) )&&( ! file_exists( fname2 ) ))
					{ // Purge si la log du flux propriétaire n'existe plus
					#ifdef OS_WINDOWS
					strdcpy( &fname1, tf->dir, DIR_CHAR, tf->name, _ );
					file_delete( fname1 );
					#else
					if ( bSudoOk )
					strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
							"rm -Rf ", tf->dir, DIR_CHAR, tf->name, _ );
					system( s );
					#endif
					}
				}
				
			#ifdef OS_UNIX
			
			// PURGE DES FICHIERS CORE
			//System( &s, "rm /home/ssie/core" );
			//System( &s, "rm " BIN_DIR "/core" );
			
			#endif
			
			
			// PURGE MEMOIRE
			
			#ifdef OS_LINUX
			if (( bSudoOk )&&( ( !( v=var_get( hSrvConf, "no_drop_caches" ) ) )
				||( stricmp( v, "Oui" ) ) ))
				{
				logw( "ROBOT : drop_caches", _ );
				system( "sudo /logiciels/ssie7/ssie7 sysctl -w vm.drop_caches=1" );
				}
			#endif
			}
		
		
		
		//------------------------------------------------------------------
		// TOUTES LES 24 HEURES
		//------------------------------------------------------------------

		strdcpy( &fname, dirwrk, DIR_CHAR, "robot_rtask_24.tim", _ );
		if (( ! file_exists( fname ) )
				||( (time(NULL) - file_date(fname)) >= (24 Heures) ))
			{
			// SURVEILLANCE VALIDITE DES CERTIFICATS SSL
			v = var_get( hSrvConf, "adm_email" );
			if (( v )&&( var_get( hSrvConf, "ssl_config_ok" ) ))
				{
				n1 = ssl_end_delay( hSrvConf, "ssl" );
				v2 = var_get( hSrvConf, "ssl_warn_mail_delay" );
				if ( v2 ) n2 = atoi( v2 ); else n2 = 30;
				if ( n1 <= (n2 Jours) )
					{
//					logw( "ROBOT : SSL ALERTE", _ );
					if ( n1 >= 0 )
						var_set( hSrvConf, "ssl_msg", v1 = str(
							"ATTENTION : La date de validité de votre certificat "
							"arrive à terme dans ", itoa( n1/(1 Jour) ), " jour",
							(char*)(((n1/(1 Jour))>1)?"s":""), _ )
							);
					else
						var_set( hSrvConf, "ssl_msg", 
							v1 = "ATTENTION : Votre certificat SSL n'est plus valide" );

					strdcpy( &cmd,
						"echo \"\" | mail -s \"ALERTE SSIE v7 : $HOSTNAME (",
						var_get(hSrvConf, "partname"), ") : ", v1, "\" ", v, _ );
					system( cmd );
					}
				}
			fp = fopen( fname, "wb" );
			if ( fp ) fclose( fp );

			
			// PURGE DU CATALOGUE MFT TOUTE LES 24 HEURES

			file_save_str( fname, "", 0 );
			
//			mftcat_purge( n );
			mftcat_purge( t_work );
			}

		//------------------------------------------------------------------

//			ssie7free( t_work );
//			trace_close( );
		}
	}
		
	

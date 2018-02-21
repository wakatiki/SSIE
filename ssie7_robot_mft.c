
	//--------------------------------------------------------------------------
	// ROBOT DE SURVEILLANCE DES TRANSFERTS MFT
	//--------------------------------------------------------------------------
	
	if ( ! strcmp( argv[2], "mft" ) )
	
	{
	tr("robot_mft_1");

	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_mft.pid", _ );
	if ( file_load( &s, fname ) != sERROR )
		{
		if ( process_is_alive( atoi(s) ) ) ssie7exit(0);
		}
		
	file_save_str( fname, itoa(getpid()), -1 );
	

	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
	
	for( n5 = 1 ; ; sleep(1), n5++ )
		{
		tr("robot_mft_2");
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
		if ( file_exists( fname ) ) ssie7exit(0);
		
		if ( ! (n5%(4 Heures)) ) exit(0); // Redémarrage toutes les 4 heures

		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_mft.pid", _ );
		file_save_str( fname, itoa(getpid()), -1 ); // Pour surveillance de blocage


		//sni begin 14/12
		//-----------------------------------------------------------------
		// SURVEILLANCE DES TRANSFERTS AU STATUS K
		// AVEC ERREUR 7XX TOUTES LES 5 MNS
		//------------------------------------------------------------------
		if ( ! ((n5)%(5*60)) ) //test toutes les 5 minutes
		if ( bd_select( hMftCat, "direct=send status=K", "part" , &s4 ) != ERROR )
		{
		v = u10; u10 = s4; s4 = v;
                if ( u10[1] )
                	for( p6 = 0, l = strlen(u10) ; p6 < l ; p6 += 8 )
                 	{
                    	strdncpy( &ida, &(u10[p6]), 8, _ );
			
			// recuperer le code erreur
			var_free( h );
			if ((h = mftcat_load( ida )) == ERROR) logw("ROBOT MFT : verif du le mftload",_);
			//if ( (h = mftcat_load( ida )) != ERROR ) logw("ROBOT MFT : ida(KO)=", ida, "=>err=",var_get( h, "errno" ),_ );
			//ajouter 'un case of' dans le cas de plusieurs errno
			if (atoi(var_get( h, "errno" )) == 730)
			{
				strdcpy( &cmd, argv[0], " mft retry ida=", ida, _ );
                        	System( &s, cmd );
				logw( "ROBOT MFT : transfert relance ida =>",ida,_);//on peut ajouter le s=res de la cmd
				Sleep(1);
			}
			}
		

		}
		//sni end 14/12



		//------------------------------------------------------------------
		// SURVEILLE LES SEND_PART SANS ACTIVITE DEPUIS PLUS DE 3 MINUTES
		//------------------------------------------------------------------
		
		tr2( BR "robot_mft_3:", itod( sysdate, "flog") );

		tf = NULL;
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_send_part_*.pid", _ );
		while( tf = file_list( tf, fname, "nodir absdir" ) )
			{
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			if ( (sysdate - file_date( fname )) < (3 Minutes) ) continue;
			
			if ( file_load( &s, fname ) == sERROR ) continue;
			process_kill( atoi( s ) );
			unlink( fname );
			tf->name[strlen(tf->name)-4] = 0;
			logw( "ROBOT MFT : send_part ", tf->name, " : semble bloqué => transfert relance", _ );
			}
		
		
		//------------------------------------------------------------------
		// DEMARRE LES SEND_PART
		//------------------------------------------------------------------
		
		tr2(BR "robot_mft_4:", itod( sysdate, "flog") );
		for( ; ; )
			{
			tr(BR "robot_mft_4a");
			if ( bd_select( hMftCat, "direct=send status=C", "part", &s4 ) == ERROR )
				{
				logw( "robot mft : pb lecture mftcat", _ );
				ssie7exit(0);
//				break;
				}


			tr2(BR "robot_mft_4b:", itoa(strlen(s4)/8));
			if ( bd_load( hMftCat, s4, strlen(s4)/8, ",:part", &s3 ) == ERROR )
				break;
			
			tr(BR "robot_mft_4c");
			strdcpy( &part, "", _ );
			
			#ifdef OS_WINDOWS
			strdcpy( &cmd, module_name, EXE, _ ); 
//			n = atoi(var_get( hSrvConf, "maxcnx" ))-1;
			n4 = 3;
			#else
			strdcpy( &cmd, "send_part", _ ); 
			n4 = 10 - process_count( cmd );	// Nb process max que l'on peut démarrer
			#endif
					
			
			tr(BR "robot_mft_4d");
			for( i = 0, l = strlen(s3) ; ( i < l )&&( n4 > 0 ) ; i = j+1 )
				{
				tr(BR "robot_mft_4e");
				j = strpos( s3, "\n", i );
				if ( j < 0 ) j = l;
				strdncpy( &s2, &(s3[i]), j-i, _ );
				trim( s2 );
				if ( !( s2[0] ) ) continue;
				if ( !strcmp( part, s2 ) ) continue;
				v = part; part = s2; s2 = v;
				tr(BR "robot_mft_4f");
				strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_send_part_", part, ".pid", _ );
				if ( file_load( &s, fname ) )
					if ( process_is_alive( atoi( s ) ) ) continue;
				
				strdcpy( &cmd, argv[0], " mft send_part ", part, _ );
				logw( "ROBOT MFT : ", cmd, _ );
				//logw( "sni: jenvois pas le tf", _ );
				StartH( cmd );
				n4--;
				}

			tr(BR "robot_mft_4g");
//fprint( fpd, BR "robot_mft_4g", __ );
			break;
			}
			
		
		
		//------------------------------------------------------------------
		// SURVEILLANCE DES TRANSFERTS AU STATUS W
		// DEMARRAGE DES REVEILS
		//------------------------------------------------------------------
		
//fprint( fpd, "robot_mft_5", __ );
		tr2(BR "robot_mft_5:", itod( sysdate, "flog") );
		if ( bd_select( hMftCat, "status=W", null, &s4 ) != ERROR )
			{
			v = u10; u10 = s4; s4 = v;
			if ( u10[1] )
			for( p6 = 0, l = strlen(u10) ; p6 < l ; p6 += 8 )
				{
				strdncpy( &ida, &(u10[p6]), 8, _ );
				
				// CHARGE LE TRANSFERT
				
				var_free( h );
				if ( (h = mftcat_load( ida )) == ERROR )
					{
					logw( "ROBOT : Surveillance status=W : ERREUR : Transfert inexistant : ", ida, BR, _ );
					continue;
					}
				
				// VERIFIE LE STATUS
				v = var_get( h, "status" );
				if ( !v ) continue;
				if ( v[0] != 'W' )
					{
					// LANCE UNE REPARATION D'INDEXE POUR LE CAS OU
					mftcat_repair( ida );
					continue;
					}
				
				// TRAITEMENTS SEND TERMINES
				
				v = var_get( h, "direct" );
				if ( !v ) continue;
				if ( !stricmp( v, "send" ) )
					{
					if ( atoi( var_get( h, "errno" ) ) )
						{
						mftcat_upd( h, "K", null, null );
						v = trim(var_get( h, "exec_on_error" ));
						if ( ( v )&&( v[0] ) )
							{
							}
						}
					else
						{
						mftcat_upd( h, "T", null, null );
						v = trim(var_get( h, "exec_on_finish" ));
						if ( ( v )&&( v[0] ) )
							{
							}
						else
							mft_reveil_compute( t_work, "SEND" );
						}
					continue;
					}
				
				// TRAITEMENT DES IMPORTS
				
				v = var_get( h, "EXPORT_SSIE" );
				if ( ( v )&&( !strcmp( v, "true" ) ) )
					{
					v1 = var_get( h, "appname" );
					if ( v1 )
						{
						strdcpy( &fname1, dirrecv, DIR_CHAR, tf->name, _ );
						strdcpy( &cmd, argv[0], " import ", v1, " fname=", var_get( h, "fname" ), _ );
						System( &s, cmd );
						
						// PASSE LE STATUS A X
						mftcat_upd( h, "X", null, null );
						
						continue;
						}
					}
					
				// RECV TERMINE

				if ( atoi( var_get( h, "errno" ) ) )
					mftcat_upd( h, "K", null, null );
				else
					{
					if ( mft_reveil_compute( t_work, "RECV" ) == ERROR )
						{
						strdcpy( &fname, dirdat, DIR_CHAR "flux" DIR_CHAR "*.run", _ );
						if ( file_count( fname ) > 0 )
							mftcat_upd( h, "F", null, null );
						else
							mftcat_upd( h, "T", null, null );
						}
					}
				}
			}
			
		
		

		//------------------------------------------------------------------
		// MFT RECV REGULIER
		//------------------------------------------------------------------
		
//		tr("robot_mft_6");
		if ( ! ( (n5+2) % (3*60) ) ) // TOUTES LES 3 MINUTES
			{
			// DECLENCHE LES TRANSFERTS LES FICHIERS EN ATTENTE
			// CHEZ LES PARTENAIRES EN EMISSION UNIQUEMENT
			
			tr(BR "robot_mft_6a");
			strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR "*.dat", _ );
			tf = NULL;
			while( tf = file_list( tf, fname, "nodir absdir" ) )
				{
				strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
				var_free( hPart );
				hPart = var_load( fname );
				
				// Retient les partenaires avec trf_direction=send
				v = var_get( hPart, "trf_direction" );
				if ( ( !v )||( stricmp( v, "send" ) ) ) continue;
				v = var_get( hPart, "enable" );
				if ( ( !v )||( stricmp( v, "Oui" ) ) ) continue;
				v = var_get( hPart, "product_name" );
				if ( ( !v )||( strnicmp( v, "SSIE v7", 7 ) ) ) continue;
				
				// Partenaire ssie7
				strdcpy( &cmd, argv[0], " mft recv part=", var_get( hPart, "partname" ), _ );
				System( &s, cmd );
				#ifdef OS_UNIX
				// verifier la commande avant de faire le kill
				logw( cmd, " : ", s, _ );//sni a enlever
				System( &cmd, "echo $(($$-1))" );
				trim(cmd);
				process_kill( atoi(cmd) );
				#endif				
				}

			//------------------------------------------------------------------
			// DEMARRE LES SEND_PART MODE PUBLISH (pour surveillance des timeout)
			//------------------------------------------------------------------
			
			for( ; ; )
				{
				if ( bd_select( hMftCat, "direct=send status=H", "part", &s4 ) == ERROR )
					break;
	
				if ( bd_load( hMftCat, s4, strlen(s4)/8, ",:part", &s3 ) == ERROR )
					break;
				
				strdcpy( &part, "", _ );
				
				#ifdef OS_WINDOWS
				strdcpy( &cmd, module_name, EXE, _ ); 
				n = atoi(var_get( hSrvConf, "maxcnx" ))-1;
				#else
				strdcpy( &cmd, module_name, " ", _ ); 
				n = 0;
				#endif
				n += process_count( cmd );
	
				n4 = atoi(var_get( hSrvConf, "maxtask" )) - 10 - n;
						// Laisse une marge pour 10 autres proccessus
				
				for( i = 0, l = strlen(s3) ; ( i < l )&&( n4 > 0 ) ; i = j+1 )
					{
					j = strpos( s3, "\n", i );
					if ( j < 0 ) j = l;
					strdncpy( &s2, &(s3[i]), j-i, _ );
					trim( s2 );
					if ( !( s2[0] ) ) continue;
					if ( !strcmp( part, s2 ) ) continue;
					v = part; part = s2; s2 = v;
					
					strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_send_part_", part, ".pid", _ );
					if ( file_load( &s, fname ) )
						if ( process_is_alive( atoi( s ) ) ) continue;
					
					strdcpy( &cmd, argv[0], " mft send_part ", part, _ );
					logw( "ROBOT MFT : ", cmd, _ );
					StartH( cmd );
					n4--;
					}
	
				break;
				}
				
			}
		
		tr2("robot_mft_7:", itod( sysdate, "flog") );
		}

	}
















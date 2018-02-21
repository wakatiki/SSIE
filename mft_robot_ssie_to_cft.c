#define cat_fname			fname3
#define log_fname			fname4

#define spart	(t_work->u1)
#define rpart	(t_work->u2)
#define fpLog	fp1

//------------------------------------------------------------------------------
int robot_ssie_to_mft( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
	
	tr("robot_ssie_to_mft_1");
	
	strdcpy( &module_name, "mft_robot", _ );
	strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot_ssie_to_mft.pid", _ );
	if ( file_load( &s, fname ) != sERROR )
		{
		if ( process_is_alive( atoi(s) ) )
			exit(0);
		}
		
	file_save_str( fname, itoa(getpid()), -1 );
	
	// BOUCLE DE SURVEILLANCE
	tr("robot_ssie_to_mft_2");
	
	for( n5 = 1 ; ; sleep(5), n5++ )
		{
		if ( ! (n5%(30*12)) ) exit(0); // Redémarrage toutes les 30 minutes

		tr("robot_ssie_to_mft_3");
		strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot_ssie_to_mft.pid", _ );
		file_save_str( fname, itoa(getpid()), -1 );
		
		// LECTURE DES TRANSFERTS AU STATUS D

		strdcpy( &s, "status=D", _ );
		
		if ( bd_select( hMftCat, s, null, &s4 ) == ERROR )
			continue;
			
		tr("robot_ssie_to_mft_4");
		v = u10; u10 = s4; s4 = v;
		if ( u10[1] )
		for( p6 = 0, l = strlen(u10) ; p6 < l ; p6 += 8 )
			{
			tr("robot_ssie_to_mft_5");
			strdncpy( &ida, &(u10[p6]), 8, _ );
			robot_mft_send( t_work );

			strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot_ssie_to_mft.pid", _ );
			file_save_str( fname, itoa(getpid()), -1 );
			}
		}
	}
	


//------------------------------------------------------------------------------
int robot_mft_send( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	// Transfert vers un MFT : ida contient l'IDA du transfert
	
	//--------------------------------------------------------------------------
	// CHARGE LES INFO SUR LE TRANSFERT
	//--------------------------------------------------------------------------

//logw( "ssie_to_mft : Traitement du transfert ", ida, _ );

	tr("robot_mft_send_1");
	
	var_free( hReq );
	if ( (hReq = mftcat_load( ida )) == ERROR )
		{
		logw( "ssie_to_mft : ERREUR : Le transfert n'existe pas : ", ida, _ );
		return 1; // Suppression de la file d'attente
		}
	

	v = var_get( hReq, "partname" );
	if ( !v ) v = var_get( hReq, "rpart" );
	strdcpy( &rpart, v, _ );
	upper( rpart );

	
	
	//--------------------------------------------------------------------------
	// OUVRE LA LOG DU TRANSFERT
	//--------------------------------------------------------------------------
	
	tr("robot_mft_send_2");
	
//	log_id = atoi(var_get( hReq, "_log_id" ));
//	if ( log_id )
//		hLog = hlog_init( var_get( hReq, "_hlog_dir" ), var_get( hReq, "_hlog_id" ) );
	
	log_id = 0;	
	
	//--------------------------------------------------------------------------
	// VERIFIE SI UNE ANNULATION N'A PAS ETE DEMANDEE
	//--------------------------------------------------------------------------

	v = var_get( hReq, "status" );
	if ( v[0] != 'D' )
		{
		// REPARATION INDEXE POUR LE CAS OU
		mftcat_repair( ida );
		return 1;
		}
	
	if ( atoi(var_get( hReq, "_transfert_canceled" )) == 1 )
		{
		// TRANSFERT SUIVANT
		tr("robot_mft_send_3");
		
		if ( var_get( hReq, "_mftutil" ) )
			{
			tr("robot_mft_send_3a");
			logw( "ssie_to_mft : ", ida, " : Transfert annulé => purge MFT", _ );
			strdcpy( &cmd, "delete part=", rpart, ",ida=", ida, _ );
			tr("robot_mft_send_3b");
			mftutil( cmd, &s );
			tr("robot_mft_send_3c");
			mftcat_logw( hReq, "Transfert annulé => purge MFT", _ );
			tr("robot_mft_send_3d");
			if ( log_id )  hlogc( hLog, log_id, "Transfert annulé => purge MFT", _ );
			tr("robot_mft_send_3e");
			}
		else
			logw( "ssie_to_mft : ", ida, " : Transfert annulé", _ );
		
		mftcat_upd( hReq, "K", null, null );
		if ( log_id )
			hlogu_status( hLog, log_id, "ANNULE", _ );
			
		tr("robot_mft_send_4");
		
		if ( log_id ) hlog_free( hLog );
		return 1; // Enlève le transfert de la file d'attente
		}

	
	//--------------------------------------------------------------------------
	// VERIFIE TIMEOUT
	//--------------------------------------------------------------------------
	
	tr("robot_mft_send_5");
	
	v = var_get( hReq, "_dt_ref_timeout" );
	if ( !v ) var_set( hReq, "_dt_ref_timeout", itoa(sysdate) );
	else
		{
		tr("robot_mft_send_6");
		
		v1 = var_get( hReq, "errno" );
		if ( atoi( v1 ) )
			{ // Transfert en erreur et toujours en cours
			tr("robot_mft_send_7");
			v1 = var_get( hReq, "timeout" );
			if ( !v1 ) v1 = var_get( hSrvConf, "trf_timeout" );
			if ( atoi( v1 ) == 0 ) v1 = "120"; // 2 heures par défaut
			
			if ( (sysdate - atoi(v)) > (atoi( v1 )*60) )
				{
				tr("robot_mft_send_8");
				// TIMEOUT
				v = var_get( hReq, "status" );
				if ( ( !v )||( v[0] == 'D' ) ) // Uniquement si pas déjà fait
					{
					if ( var_get( hReq, "_mftutil" ) )
						{
						tr("robot_mft_send_9");
						strdcpy( &cmd, "delete part=", rpart, ",ida=", ida, _ );
						mftutil( cmd, &s );
						}
					strdcpy( &s, "TIMEOUT:", var_get( hReq, "errmsg" ), _ );
					mftcat_logw( hReq, s, _ );
					mftcat_upd( hReq, "W", null, s );
					tr("robot_mft_send_10");
					if ( log_id )
						{
						hlogc( hLog, log_id, s, _ );
						hlogu_status( hLog, log_id, "ERREUR:", s, _ );
						}
					logw( "ssie_to_mft : ", ida, " : ", s, _ );
					}

				// TRANSFERT SUIVANT
				
				if ( log_id ) hlog_free( hLog );
				tr("robot_mft_send_11");
				return 1; // Enlève le transfert de la file d'attente
				}
			}
		}
	
		
	//--------------------------------------------------------------------------
	// ACTIONS SELON LE N° DE L'ETAPE
	//		0 = transfert à soumettre
	//		1 = transfert en cours par MFT
	//--------------------------------------------------------------------------
	
	tr("robot_mft_send_12");
	e = atoi( var_get( hReq, "_mft_send_etape_no" ) );
	
	switch( e )
		{
		//----------------------------------------------------------------------
		// TRANSFERT A SOUMETTRE A MFT
		//----------------------------------------------------------------------

		case 0 : 
			
			tr("robot_mft_send_13");
			
			var_set( hReq, "_mft_send_etape_no", "0" );
			var_set( hReq, "errmsg", "MFT : Transfert en préparation" );

			//--------------------------------------------------------------------------
			// VERIFIE PARTENAIRE DESTINATAIRE
			//--------------------------------------------------------------------------
						
			strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, rpart, ".dat", _ );
			if (( !file_exists( fname ) )||( (sysdate - file_date( fname )) > 4 Heures ))
				{
				tr("robot_mft_send_14");
				strdcpy( &s, dirbin, DIR_CHAR "ssie7" EXE " part ", rpart, " loadnat", _ );
				System( &s1, s );
				if ( !file_exists( fname ) )
					{
					tr("robot_mft_send_15");
					
					strdcpy( &s, "ERREUR : Partenaire inconnu : ", rpart, _ );
					mftcat_logw( hReq, s, _ );
					mftcat_upd( hReq, "K", "408", s );
					if ( log_id )
						{
						hlogc( hLog, log_id, s, _ );
						hlogu_status( hLog, log_id, s, _ );
						}
					if ( log_id ) hlog_free( hLog );
					return 1; // Enlève le transfert de la file d'attente
					}
				}
			
			var_free( hPart );
			hPart = var_load( fname );
			v = var_get( hReq, "product_name" );
			if (( v )&&( !strnicmp( v, "ssie v7", 7 ) ))
				{
				tr("robot_mft_send_16");
				strdcpy( &s, "ERREUR : Partenaire non MFT", _ );
				mftcat_logw( hReq, s, _ );
				mftcat_upd( hReq, "K", "409", s );
				if ( log_id )
					{
					hlogc( hLog, log_id, s, _ );
					hlogu_status( hLog, log_id, s, _ );
					}
				if ( log_id ) hlog_free( hLog );
				logw( "ssie_to_mft : ", ida, " : ", s, _ );
				return 1; // Enlève le transfert de la file d'attente
				}
			
			
			
			
			//------------------------------------------------------------------
			// ENVOI DE MESSAGE
			//------------------------------------------------------------------
			
			if ( ( v = var_get( hReq, "type" ) )&&( !stricmp( v, "message" ) ) )
				{
				tr("robot_mft_send_17");
				
				// PREPARE LA COMMANDE MFTUTIL ET LA SOUMET

				strdcpy( &cmd, "send "
					"part=", rpart, ","
					"ida=", ida, ","
					"type=message", ","
					"idm=", (char*)( ( (v=var_get( hReq, "idm" )) && (v[0]) ) ? v : "msg" ), ","
					"msg=\"", (char*)( ( v=var_get( hReq, "msg" ) ) ? quote( str_replace(v, "\"", "") ) : "''" ), "\"",
					_ );
				
/*
				// ANNULER CAR MFT NE RECUPERE PAS LES MESSAGES EN MODE STATE=HOLD
				
				if ( var_get( hReq, "publish" ) )
					strdcat( &cmd, ",state=hold", _ );
				else
				if (( v = var_get( hReq, "state" ) )&&( ! stricmp( v, "hold" ) ))
					{
					strdcat( &cmd, ",state=hold", _ );
					var_set( hReq, "publish", "true" );
					}
				else
*/
				if ( v = var_get( hReq, "spart" ) )
					strdcat( &cmd, ",spart=", v, _ );
				
				goto soumission;
				}



			//------------------------------------------------------------------
			// ENVOI DE FICHIER : PREPARE LA COMMANDE MFTUTIL ET LA SOUMET
			//------------------------------------------------------------------
			
			strdcpy( &fname, v = var_get( hReq, "fname" ), _ );
			if (( !v )||( ! file_exists( fname ) ))
				{
				tr("robot_mft_send_18");
				
				strdcpy( &s, v="ERREUR : Le fichier à envoyer par MFT n'existe pas ou plus", _ );
				mftcat_logw( hReq, s, _ );
				mftcat_upd( hReq, "K", "110", v );
				if ( log_id )
					{
					hlogc( hLog, log_id, s, _ );
					hlogu_status( hLog, log_id, s, _ );
					}
				
				if ( log_id ) hlog_free( hLog );
				return 1; // Supprime le transfert de la file d'attente
				}

			strdcpy( &cmd, "send "
				"part=", rpart, ","
				"ida=", ida, ","
				"fname=", quote( fname ),
				_ );

			tr("robot_mft_send_19");
			
			if ( var_get( hReq, "publish" ) )
				strdcat( &cmd, ",state=hold", _ );
			else
			if (( v = var_get( hReq, "state" ) )&&( ! stricmp( v, "hold" ) ))
				{
				tr("robot_mft_send_20");
				strdcat( &cmd, ",state=hold", _ );
				var_set( hReq, "publish", "true" );
				}
			else	
			if ( v = var_get( hReq, "spart" ) ) // On ne met pas spart si state=hold (bogue de MFT)
				strdcat( &cmd, ",spart=", v, _ );

			
			if ( v = var_get( hReq, "idf" ) )
				{
				tr("robot_mft_send_21");
				strdcat( &cmd, ",idf=", v, _ );
				if ( upper(v)[strlen(v)-1] == 'B' )	b1 = 0; else b1 = 1;
				}
			else
			if ( var_get( hReq, "ascii" ) )
				{ tr("robot_mft_send_22"); strdcat( &cmd, ",idf=R2MMS", _ ); b1 = 1; }
			else
				{ strdcat( &cmd, ",idf=R2MMSB", _ ); b1 = 0; }
			
			if ( v = var_get( hReq, "nidf" ) )
				strdcat( &cmd, ",nidf=", v, "", _ );
				
			if ( v = var_get( hReq, "sappl" ) )
				strdcat( &cmd, ",sappl='", v, "'", _ );
			else
				{
				tr("robot_mft_send_23");
				v1 = var_get( hReq, "adle" );
				if ( !v1 ) v1 = var_get( hReq, "spart" );
				if ( !v1 ) v1 = var_get( hSrvConf, "partname" );
				v2 = var_get( hReq, "appe" );
				if ( !v2 ) v2 = "";
				strdcat( &cmd, ",sappl='", strpad( v1, 8, " " ), strpad( v2, 8, " " ), "'", _ );
				}

			if ( v = var_get( hReq, "suser" ) )
				strdcat( &cmd, ",suser='", v, "'", _ );
			else
				{
				tr("robot_mft_send_24");
				v1 = var_get( hReq, "adle" );
				if ( !v1 ) v1 = var_get( hReq, "spart" );
				if ( !v1 ) v1 = var_get( hSrvConf, "partname" );
				strdcat( &cmd, ",suser='", ida, strpad( v1, 8, " " ), "'", _ );
				}

			if ( v = var_get( hReq, "rappl" ) )
				strdcat( &cmd, ",rappl='", v, "'", _ );
			else
				{
				tr("robot_mft_send_25");
				v1 = var_get( hReq, "adlr" );
				if ( !v1 ) v1 = rpart;
				v2 = var_get( hReq, "appr" );
				if ( !v2 ) v2 = "";
				strdcat( &cmd, ",rappl='", strpad( v1, 8, " " ), strpad( v2, 8, " " ), "'", _ );
				}

			if ( v = var_get( hReq, "ruser" ) )
				strdcat( &cmd, ",ruser='", v, "'", _ );
			else
				{
				tr("robot_mft_send_26");
				strdcat( &cmd, ",ruser='", ida, "'", _ );
				}

			
			if ( v = var_get( hReq, "parm" ) )
				strdcat( &cmd, ",parm=\"", quote( str_replace(v, "\"", "") ), "\"", _ );
			
			
			// PARAMETRAGE SPECIFIQUE POUR LES MVS
			tr("robot_mft_send_27");
			
			strdcpy( &fname1, dirconf, DIR_CHAR "part" DIR_CHAR, rpart, ".dat", _ );
			hPart= var_load( fname1 );
			v = var_get( hPart, "os" );
			if ( ( v )&&( !stricmp( v, "mvs" ) ) )
				{
				tr("robot_mft_send_28");
				if ( ! ( v = var_get( hReq, "nspace" ) ) )
					strdcat( &cmd, ",nspace=", itoa( (file_size( fname )/1024) + 1 ), _ );
				else
					strdcat( &cmd, ",nspace=", v, _ );
				
				if ( ! ( v = var_get( hReq, "nrecfm" ) ) )
					strdcat( &cmd, ",nrecfm=V", _ );
				else
					strdcat( &cmd, ",nrecfm=", v, _ );
					
				if ( ! ( v = var_get( hReq, "nlrecl" ) ) )
					{
					tr("robot_mft_send_29");
					if ( !b1 )
						strdcat( &cmd, ",nlrecl=4000", _ );
					else
						{
						n1 = 0;
						fp = fopen( fname, "rb" );
						if ( fp )
							{
							s = ( char*)realloc( s, 8 Ko );
							j = 0;
							while( fgets( s, 8 Ko, fp ) )
								{
								n2 = strlen( s ) - 1;
								if ( n1 < n2 ) n1 = n2;
								j++;
								if ( j > 50 ) break;
								}
							fclose( fp );
							}
						if ( n1 == 0 ) n1 = 4000;
						strdcat( &cmd, ",nlrecl=", itoa( n1 ), _ );
						if ( ! var_get( hReq, "nblksize" ) )
							{
							if ( n1*10 < 15000 ) n1 = n1 * 10;
							else if ( n1*5 < 15000 ) n1 = n1 * 5;
							else if ( n1*3 < 15000 ) n1 = n1 * 3;
							else if ( n1*2 < 15000 ) n1 = n1 * 2;
							
							var_set( hReq, "nblksize", itoa( n1 ) );
							}
						}
					}
				else
					strdcat( &cmd, ",nlrecl=", v, _ );
				
				if ( !( v = var_get( hReq, "nblksize" )  ))
					{
					if ( !b1 )
						strdcat( &cmd, ",nblksize=12000", _ );
					}
				else
				strdcat( &cmd, ",nblksize=", v, _ );
				}
					
			if ( ( v = var_get( hReq, "faction" ) )&&( !stricmp( v, "delete" ) ) )
				strdcat( &cmd, ",faction=delete", _ );
			
			
			//--------------------------------------------------------------------------
			soumission:
			
			tr("robot_mft_send_30");
			
			strd_ireplace( &cmd, "[$ida]", ida );
	
			var_set( hReq, "_mftutil", cmd );
			
			mftcat_logw( hReq, "MFTUTIL ", cmd, BR, _ );
			
			if ( mftutil( cmd, &s1 ) == ERROR )
				{
				tr("robot_mft_send_31");
				logw( "ssie_to_mft : ", ida, " : ERREUR : MFTUTIL : ", s1, _ );
				strdcpy( &s, "MFTUTIL ERREUR : ", BR, s1, _ );
				mftcat_logw( hReq, s, _ );
				mftcat_upd( hReq, null, "201", "ERREUR MFTUTIL" );
				if ( log_id )  hlogc( hLog, log_id, s, _ );
				
				if ( log_id ) hlog_free( hLog );
				return ERROR;
				}

			
			
			//------------------------------------------------------------------
			// SE MET EN ATTENTE
			//------------------------------------------------------------------
			
			tr("robot_mft_send_32");
			
			var_set( hReq, "_dt_ref_timeout", itoa(sysdate) );
			var_set( hReq, "_mft_send_etape_no", "1" );
			mftcat_upd( hReq, null, "0", "En cours : MFT : Transfert soumis" );
			
			if ( log_id ) hlog_free( hLog );
			return OK;

		
		//----------------------------------------------------------------------
		// TRANSFERT EN ATTENTE DE FIN DU TRANSFERT
		//----------------------------------------------------------------------

		case 1 : 
			
			tr("robot_mft_send_33");
			
			//------------------------------------------------------------------
			// VERIFIE QUE MFT EST DEMARRE
			//------------------------------------------------------------------
			
			if ( process_count( "MFTMAIN" ) == 0 )
				{
				if ( log_id ) hlog_free( hLog );
				return ERROR;
				}
			
			
			//------------------------------------------------------------------
			// RECHERCHE LE TRANSFERT DANS MFT
			//------------------------------------------------------------------
			
			tr("robot_mft_send_34");
			
			var_free( h1 );
			strdcpy( &s, "direct=send,part=", rpart, ",ida=", ida, _ );
			h1 = mft_listcat_full( s );
			if ( h1 == ERROR )
				{
				// TRANSFERT NON TROUVE ==> RELANCE
				tr("robot_mft_send_35");
				
				strdcpy( &s, "ERREUR : Transfert MFT non trouvé", _ );
				logw( "ssie_to_mft : ", ida, " : ERREUR : MFTUTIL.", s, _ );
				mftcat_logw( hReq, s, _ );

/*
				strdcpy( &cmd, "delete part=", rpart, ",ida=", ida, _ );

				mftcat_logw( hReq, "SUPPRIME LE TRANSFERT MFT (POUR RELANCE)", _ );

				if ( mftutil( cmd, &s1 ) == ERROR )
					{
					tr("robot_mft_send_36");
					
					strdcpy( &s, "ERREUR MFTUTIL : ", BR, cmd, BR, s1, _ );
					logw( "ssie_to_mft : ", ida, " : ", s, _ );
					mftcat_logw( hReq, s, _ );
					mftcat_upd( hReq, null, "201", "ERREUR MFTUTIL" );
					if ( log_id )  hlogc( hLog, log_id, s, _ );

					return ERROR;
					}

				v = var_get( hReq, "_mftutil" );
				mftcat_logw( hReq, "Relance MFTUTIL ", v, _ );
				
				if ( mftutil( v, &s1 ) == ERROR )
					{
					tr("robot_mft_send_37");
					strdcpy( &s, "ERREUR MFTUTIL : ", BR, s1, _ );
					logw( "ssie_to_mft : ", ida, " : ERREUR : MFTUTIL : ", cmd, BR, s1, _ );
					mftcat_logw( hReq, s, _ );
					mftcat_upd( hReq, null, "201", "ERREUR MFTUTIL" );
					
					if ( log_id ) hlog_free( hLog );
					return ERROR;
					}
*/

				mftcat_upd( hReq, null, null, null );
				if ( log_id )  hlogc( hLog, log_id, s, _ );
				
				if ( log_id ) hlog_free( hLog );
				tr("robot_mft_send_38");
				
				return ERROR;
				}
			
			var_set( hReq, "idt_mft", var_get( h1, "idt" ) );
			
			
			//------------------------------------------------------------------
			// VERIFIE LE STATUS
			//------------------------------------------------------------------
			
			n = atoi( var_get( h1, "diagi" ) );
			v = var_get( h1, "state" );
			if ( ( v )&&( v[0] == 'T' )&&( n == 0 ) )
				{
				tr("robot_mft_send_39");
				
				// TRANSFERT TERMINE CORRECTEMENT

				strdcpy( &s, "TRANSFERT MFT TERMINE OK", _ );
				mftcat_logw( hReq, s, _ );
				mftcat_upd( hReq, "W", "0", "OK" );
				if ( log_id )
					{
					hlogc( hLog, log_id, s, _ );
					hlogu_status( hLog, log_id, "OK", _ );
					}
				
				// Supprime le transfert du catalogue MFT

				strdcpy( &cmd, "delete part=", rpart, ",ida=", ida, _ );
				mftutil( cmd, &s );
				
				if ( log_id ) hlog_free( hLog );
				tr("robot_mft_send_40");
				return 1; // Enleve le transfert de la file d'attente
				}
				

			//------------------------------------------------------------------
			// LE TRANSFERT EST EN COURS
			//------------------------------------------------------------------
			
			if ( n == 0 )
				{
				tr("robot_mft_send_41");
				v = var_get( hReq, "errno" );
				if ( atoi( v ) )
					{ // Si le transfert était en erreur
//					var_set( hReq, "_dt_ref_timeout", itoa(sysdate) );
					mftcat_upd( hReq, null, "0", "En cours : MFT" );
					}
					
				// LE TRANSFERT EST ENCORE EN COURS
				if ( log_id ) hlog_free( hLog );
				return OK;
				}
				
			//------------------------------------------------------------------
			// TRANSFERT EN ERREUR
			//------------------------------------------------------------------
			
			strdcpy( &s, "ERREUR MFT : ", mft_errlib( n ), _ );

			if ( atoi( var_get( hReq, "errno" ) ) != n )
				{ // Ne log l'erreur que si le n° a changé
				// logw( "ROBOT MFT SEND : ", ida, " : ERREUR MFT : ", s, _ );
				tr("robot_mft_send_42");
				
				var_to_str( h1, &s1 );
				strd_replace( &s1, ",", ", " );
				strdcpy( &s2, s, BR, s1, _ );
				mftcat_logw( hReq, s2, _ );
				if ( log_id )  hlogc( hLog, log_id, s2, _ );
				}
			
/*			
			if ( n == 302 )
				{
				// Traite le pb de 302 uniquement si le transfert a été interrompu
				// (mftutil halt ... ==> Le status passe à l'état H)
				tr("robot_mft_send_43");
				if ( var_get( h1, "state" )[0] != 'H' ) n = 402;
				}
			else
*/
			if (( v[0] == 'H' )||( v[0] == 'K' ))
				{
				if ( v[0] == 'H' )
					{
					strdcpy( &cmd, "delete part=", rpart, ",ida=", ida, _ );
					mftcat_logw( hReq, "SUPPRIME LE TRANSFERT MFT", _ );
					mftutil( cmd, &s1 );
					}
				mftcat_upd( hReq, "K", itoa(n), s );
				if ( log_id )
					{
					hlogu_status( hLog, log_id, "ERREUR : ", s, _ );
					hlog_free( hLog );
					}
				return ERROR;
				}
				
			if ( n == 909 )
				{
				tr("robot_mft_send_44");
				// NOTRE PARTENAIRE EST INCONNU CHEZ LE DISTANT
				strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, rpart, ".dat", _ );
				var_free( hPart );
				hPart = var_load( fname );
				v = var_get( hPart, "product_name" );
				v1 = var_get( hPart, "mersi_version" );


                if ( ( v1 )||( ( v )&&( stripos( v, "mersi", 0 ) >= 0 ) ) )
                	{
					tr("robot_mft_send_45");
					strdcpy( &cmd, "delete part=", rpart, ",ida=", ida, _ );
	
					mftcat_logw( hReq, "SUPPRIME LE TRANSFERT MFT (POUR RELANCE)", _ );
	
					if ( mftutil( cmd, &s1 ) == ERROR )
						{
						strdcpy( &s, "ERREUR MFTUTIL : ", BR, cmd, BR, s1, _ );
						logw( "ssie_to_mft : ", ida, " : ERREUR : MFTUTIL : ", cmd, BR, s1, _ );
						mftcat_logw( hReq, s, _ );
						mftcat_upd( hReq, null, "201", "ERREUR MFTUTIL" );
						if ( log_id )  hlogc( hLog, log_id, s, _ );
	
						return ERROR;
						}
	

					// DISTANT EN MERSI V3
					// ==> ON ESSAI DE CONFIGURER LE PARTENAIRE
					
					strdcpy( &cmd, "part ", var_get( hSrvConf, "partname" ),
							" port_mersi=", var_get( hSrvConf, "port" ),
							" port_mft=1761",
							" dns='", var_get( hSrvConf, "dns_ssie" ), "'"
							" ip=", var_get( hSrvConf, "ip_ssie" ),
							" os=" OS_NAME,
							" mersi_version='SSIE v7'",
							" user=" NNI_ADM,
							_ );

					strdcpy( &s, var_get( hPart, "ip" ), ":", var_get( hPart, "port_mersi" ), _ );
					if ( s1 ) s1[0] = 0;

					for( n1 = 3 ; n1 > 0 ; n1--, sleep(1) )
						{
						if ( tcp_request( s, cmd, &s1 ) != null )
							break;
						}
					
					strd_replace( &cmd, "user=" NNI_ADM, "" );
					strdcpy( &s,"DISTANT en MERSI3 => Tentative de configuration : ",
							(char*)((n1) ? "OK":"ERREUR"), BR,
							cmd, BR, s1, BR, _ );
					mftcat_logw( hReq, s, _ );
					if ( log_id )  hlogc( hLog, log_id, s, _ );

					if ( ( stripos( s1, "OK", 0 ) < 0 )&&( n1 ) )
						{ // Le distant à retourné une erreur ==> Inutile de réessayer
						mftcat_upd( hReq, "K", null, null );
						if ( log_id )  hlogu_status( hLog, log_id, "ERREUR : ", s, _ );
						if ( log_id ) hlog_free( hLog );
						return 1; // Pas de retentative
						}

					v = var_get( hReq, "_mftutil" );
					mftcat_logw( hReq, "Relance MFTUTIL ", v, _ );
					
					if ( mftutil( v, &s1 ) == ERROR )
						{
						strdcpy( &s, "ERREUR MFTUTIL : ", BR, s1, _ );
						logw( "ssie_to_mft : ", ida, " : ERREUR : MFTUTIL : ", v, BR, s1, _ );
						mftcat_logw( hReq, s, _ );
						mftcat_upd( hReq, null, "201", "ERREUR MFTUTIL" );
						
						if ( log_id ) hlog_free( hLog );
						return ERROR;
						}
					}
				}

			mftcat_upd( hReq, null, itoa(n), s );
					
/*
			tr("robot_mft_send_46");
			
			if (( n >= 300 )&&( n != 402 )&&( n != 416 )&&( n != 417 )&&( n != 418 )&&( n != 909 )&&( n != 916 ))
				{
				tr("robot_mft_send_47");
				
				strdcpy( &cmd, "delete part=", rpart, ",ida=", ida, _ );

				mftcat_logw( hReq, "SUPPRIME LE TRANSFERT MFT (POUR RELANCE)", _ );

				if ( mftutil( cmd, &s1 ) == ERROR )
					{
					strdcpy( &s, "ERREUR MFTUTIL : ", BR, cmd, BR, s1, _ );
					logw( "ssie_to_mft : ", ida, " : ", s, _ );
					mftcat_logw( hReq, s, _ );
					mftcat_upd( hReq, null, "201", "ERREUR MFTUTIL" );
					if ( log_id )  hlogc( hLog, log_id, s, _ );

					return ERROR;
					}

				v = var_get( hReq, "_mftutil" );
				mftcat_logw( hReq, "Relance MFTUTIL ", v, _ );
				
				if ( mftutil( v, &s1 ) == ERROR )
					{
					strdcpy( &s, "ERREUR MFTUTIL : ", BR, s1, _ );
					logw( "ssie_to_mft : ", ida, " : ERREUR : MFTUTIL : ", cmd, BR, s1, _ );
					mftcat_logw( hReq, s, _ );
					mftcat_upd( hReq, null, "201", "ERREUR MFTUTIL" );
					
					if ( log_id ) hlog_free( hLog );
					return ERROR;
					}
				}
			if ( n == 402 ) n = 302;
				
			if ( n < 300 ) // Si erreur locale, pas de retentative
				{
				mftcat_upd( hReq, "K", itoa(n), s );
				if ( log_id )  hlogu_status( hLog, log_id, "ERREUR : ", s, _ );
				}
			else
				mftcat_upd( hReq, null, itoa(n), s );
*/			
			if ( log_id ) hlog_free( hLog );
			
			tr("robot_mft_send_48");
			
			return ERROR;
		
		
		//----------------------------------------------------------------------
		// LE TRANSFERT EST DANS UN ETAT ANORMAL
		//----------------------------------------------------------------------

		default: 
			
			if ( log_id ) hlog_free( hLog );
			return 1;
		}
	
	
	//--------------------------------------------------------------------------
	
	if ( log_id ) hlog_free( hLog );
	return OK;
	}


#undef cat_fname
#undef log_fname









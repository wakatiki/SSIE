#define cat_fname			fname3
#define log_fname			fname4


	//--------------------------------------------------------------------------
	// ROBOT DE TRAITEMENT DES FICHIERS RECUS PAR MFT
	//--------------------------------------------------------------------------
	
	if ( ! strcmp( argv[2], "wrecv" ) )
		{
		mkdir( dirwrecv );

		for( ; ; sleep(1) )
			{
			strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
			if ( file_exists( fname ) ) exit(0);

		
/*
			// SI AUCUN PROCESS EN ATTENTE OU AUCUN FICHIER RECU => INUTILE DE FAIRE LES TRAITEMENTS
			
			if ( file_count( dirrecv ) < 1 )	continue;
			
			//--------------------------------------------------------------------------
			// CHARGE LA LISTE DES CONDITIONS D'AFFECTATION DANS WRECV
			//--------------------------------------------------------------------------

			if ( ! t_cond )
				t_cond = (T_REC_COND*)alloc0( sizeof(T_REC_COND)*4*1024 );
			else
				memset( t_cond, 0, sizeof(T_REC_COND)*4*1024 );
			
			nb_cond = 0;
			tf = NULL;
			while( ( tf = file_list( tf, dirwrecv, "nodir adir" ) ) != NULL )
				{
				if ( ! is_numeric( tf->name ) ) continue;
				
				strdcpy( &fname, dirwrecv, DIR_CHAR, tf->name, _ );
				if ( !file_load( &s2, fname ) ) continue;

				ts1 = str_explode( s2, "<>" );
				if ( ( ts1[1] == null )||( ts1[2] == null ) )
					{ free( ts1 ); continue; }
				t_cond[nb_cond].timeout = atoi( ts1[0] );
				strdcpy( &(t_cond[nb_cond].cmde), ts1[2], _ );
				t_cond[nb_cond].bKeep = ( ( ts1[3] )&&( ts1[3][0] == 'O' ) );
				
				ts = str_explode( ts1[1], "\n" );
				
				for( i = 0 ; ts[i] ; i++ )
					{
					if ( ! (trim( ts[i] )[0]) ) continue;
					ts2 = str_explode( ts[i], "=>" );
					if ( ! ts2[0] ) { free( ts2 ); continue; }
					
					if ( ts2[0][0] == '[' )
						{
						t_cond[nb_cond].nb_max = atoi( &(ts2[0][1]) );
						if ( t_cond[nb_cond].nb_max > 500 ) t_cond[nb_cond].nb_max = 500;
						p = strpos( ts2[0], "]", 0 );
						if ( p <= 0 )
							p = -1;
						}
					else
						{
						t_cond[nb_cond].nb_max = 1;
						p = -1;
						}
		
					strdcpy( &(t_cond[nb_cond].wrecv), dirwrecv, DIR_CHAR, tf->name, _ );
					strdcpy( &(t_cond[nb_cond].cond), trim(&(ts2[0][p+1])), _ );
					strdcpy( &(t_cond[nb_cond].dest_name), ts2[1], _ );
					t_cond[nb_cond].wrecv_id = atoi(tf->name);
					
					free( ts2 );
					nb_cond++;
					}
				free( ts );
				free( ts1 );
				}
*/
			
			
			//--------------------------------------------------------------------------
			// PARCOUR TOUS LES TRANSFERTS AVEC STATUS W ET CHERCHE UN PROCESSUS QUI CORRESPOND
			//--------------------------------------------------------------------------

/*
			strdcpy( &fname, dirwrk, DIR_CHAR "wrecv.reset.sans_reveil", _ );
			if ( file_exists( fname ) )
				{
				unlink( fname );
				strdcpy( &cmd1, "mft setw part=*", _ );
				var_free( hReq );
				hReq = var_from_cmd( 0, cmd1 );
				cmd_mft_setw( t_work );
				}
*/
			
			strdcpy( &cmd1, "mftcat status=W, of=ssie", _ );
			var_free( hReq );
			hReq = var_from_cmd( 0, cmd1 );
			cmd_mft_cat( t_work );

if ( s4[1] )
logw( "robot wrecv : ", cmd1, BR "==>", s4, _ );

			v = u10; u10 = s4; s4 = v;
			if ( u10[1] )
			for( p6 = 0, l = strlen(u10) /*, k = n = 0*/ ; p6 < l ; p6 += 8 )
				{
				strdncpy( &ida, &(u10[p6]), 8, _ );
logw( "Robot WRECV : traite ida=", ida, _ );
				
				// CHARGE LE TRANSFERT
				
				var_free( h );
				if ( (h = mftcat_load( ida )) == ERROR )
					{
					logw( "Robot WRECV : ERREUR : Transfert inexistant : ", ida, BR, _ );
					continue;
					}
			
				
				// TRAITEMENTS SEND TERMINES
				
				v = var_get( h, "direct" );
				if ( !v ) continue;
				if ( !stricmp( v, "send" ) )
					{
logw( "Robot WRECV : direct=send, ida=", ida, _ );

					if ( atoi( var_get( h, "errno" ) ) )
						{
						//mft_save2( t_work, h, "K", null, null, cat_fname );
						mftcat_upd( h, "K", null, null );
						v = trim(var_get( h, "exec_on_error" ));
						if ( ( v )&&( v[0] ) )
							{
							}
						}
					else
						{
						//mft_save2( t_work, h, "T", null, null, cat_fname );
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
					
				
				// CHERCHE UN REVEIL MFT (s'il existe, il est démarré)
				
				if ( mft_reveil_compute( t_work, "RECV" ) != ERROR )
					continue;
		
		
				//----------------------------------------------------------------------
				// TRAITEMENT DES IMPORTS
				//----------------------------------------------------------------------
				
				v = var_get( h, "EXPORT_SSIE" );
				if ( ( v )&&( !strcmp( v, "true" ) ) )
					{
					v1 = var_get( h, "appname" );
					if ( v1 )
						{
						strdcpy( &fname1, dirrecv, DIR_CHAR, tf->name, _ );
						strdcpy( &cmd, argv[0], " import ", v1, " fname=", var_get( h, "_recep_fname" ), _ );
						System( &s, cmd );
						
						// PASSE LE STATUS A X
						//mft_save2( t_work, h, "X", null, null, cat_fname );
						mftcat_upd( h, "X", null, null );
						continue;
						}
					}

continue;				
				
				//----------------------------------------------------------------------
				// TRAITEMENT DES ACCUSES RECEPTIONS
				//----------------------------------------------------------------------
				
				v = var_get( h, "idf" );
				if ( ( v )&&( !stricmp( v, "ACK" ) ) )
					{
					v1 = var_get( h, "parm" ); // FLUX_ID(sur 5 car base 36) + LOG_ID (sur 3 car base 36): Status
					if (( v1 )&&( strlen( v1 ) >= 8 ))
						{
						flux_id = itoa( ifrom36(substr(v1,0,5)) );
						log_id = ifrom36(substr(v1,5,3));
						hLog = hlog_init( dirflux, flux_id );
						if ( hLog != ERROR )
							{
							strdcpy( &s2, &(v1[9]), " (A.R.)", _ );
							hlogu_status( hLog, log_id, s2, _ );
							v = var_get( h, "fname" );
							if ( s ) s[0] = 0;
							file_load( &s, v );
							if ( s[0] )
								{
								id = hlogc( hLog, log_id, s2, "A.R. : Fichier reçu :", _ );
								hlogc( hLog, id, "", s, _ );
								}
							else
								id = hlogc( hLog, log_id, s2, "A.R. reçu", _ );
							hlog_free( hLog );
							}
						}
						
					// PASSE LE STATUS A X
					//mft_save2( t_work, h, "X", null, null, cat_fname );
					mftcat_upd( h, "X", null, null );
					continue;
					}
				
				
				//----------------------------------------------------------------------
				// PREPARE LES INFO POUR LA RECHERCHE
				//----------------------------------------------------------------------
		
				var_to_str_nq( h, &s ); //Transforme en string sans les guillemets
				strdcat( &s, ",", _ );
				
				
				//----------------------------------------------------------------------
				// RECHERCHE UN FLUX QUI CORRESPOND
				//----------------------------------------------------------------------
				
				cond_ok_no = -1;
				for( i = 0 ; i < nb_cond ; i++ )
					{
//					if ( t_cond[i].cond[0] == 0 ) continue;
// Test précédent enlevé à cause d'ERDF qui veulent pourvoir faire des flux sans condition.
					if ( t_cond[i].bKeep < 0 ) continue;
					if ( str_iexists( s, t_cond[i].cond ) == false ) continue;
					if ( ( stripos( s, "idf=ACK", 0 ) >= 0 )
						&&( stripos( t_cond[i].cond, "idf=ACK", 0 ) < 0 ) )
						continue;
		
					//------------------------------------------------------------------
					// UNE CONDITION A ETE TROUVE
					//------------------------------------------------------------------
					
					if ( cond_ok_no >= 0 )
						{
						// Le fichier convenait déjà à un autre processus => on détermine la priorité
						// en calculant un indice de complexité de la condition
						// c'est le plus complexe qui l'emporte ou le premier dans la liste en cas d'égalité
						
						if ( ts ) free( ts );
						ts = str_explode( t_cond[i].cond, "&" );
		
						if ( ts2 ) free( ts2 );
						ts2 = str_explode( t_cond[cond_ok_no].cond, "&" );
		
						n1 = 0;
						for( j = 0 ; ts[j] ; j++ )
							{
							for( p = 0 ; ts2[p] ; p++ )
								{
								if ( stripos( ts2[p], ts[j], 0 ) >= 0 ) break;
								}
							if ( !ts2[p] ) // Ce critère n'a pas été trouvé dans l'autre condition
								n1 += strlen( ts[j] );
							}
						
						n2 = 0;
						for( j = 0 ; ts2[j] ; j++ )
							{
							for( p = 0 ; ts[p] ; p++ )
								{
								if ( stripos( ts[p], ts2[j], 0 ) >= 0 ) break;
								}
							if ( !ts[p] )
								n2 += strlen( ts2[j] );
							}
						if ( n1 > n2 )
							cond_ok_no = i;
						}
					else
						cond_ok_no = i;
					}
				
				
				//----------------------------------------------------------------------
				// DEMARRE LE PROCESS DONT LA CONDITION CORRESPOND
				//----------------------------------------------------------------------
		
				
				if ( cond_ok_no >= 0 )
					{
					// Démarre le process
					
//					tf->name[strlen(tf->name)-5] = 0;

/*		
// Tentative de changer d'ID du flux qui reste en attente.
// => Abandonné pour le moment car très compliqué.
			
					if ( t_cond[cond_ok_no].bKeep == false )
						{
						strdcpy( &cmd, t_cond[cond_ok_no].cmde, " ",
								dirrecv, DIR_CHAR, tf->name, " ", str_C_quote(t_cond[cond_ok_no].dest_name),
								_ );
						}
					else
						{
						strdcpy( &fname, dirflux, DIR_CHAR "log.id", _ );
						n = getuid( fname );
						strdcpy( &cmd, t_cond[cond_ok_no].cmde, " ", itoa(n), " ",
								dirrecv, DIR_CHAR, tf->name, " ", str_C_quote(t_cond[cond_ok_no].dest_name),
								_ );
						
						p = stripos( t_cond[cond_ok_no].cmde, " dupli_start", 0 );
						p = strrpos( t_cond[cond_ok_no].cmde, " ", p );
						n1 = atoi( t_cond[cond_ok_no].cmde[p+1] );
						strdcpy( &s, substr( t_cond[cond_ok_no].cmde, 0, p ), " ", itoa(n), " dupli_start", _ );
						strdcpy( &(t_cond[cond_ok_no].cmde), s, _ );
logw( "robot wrecv dupli_start : new cmde : ", s, ", old_id=", itoa(n1), _ );
						
						// Rennome le fichier dans rflux
						
						}
*/
		
					strdcpy( &cmd, t_cond[cond_ok_no].cmde, " ",
							cat_fname, " ", str_C_quote(t_cond[cond_ok_no].dest_name),
							_ );
					logw( "robot wrecv : ", cmd,
							" (", itoa(t_cond[cond_ok_no].wrecv_id), ", ", itoa(t_cond[cond_ok_no].bKeep),  ")",
							_ );
					
					//mft_save2( t_work, h, "T", null, null, cat_fname );
					mftcat_upd( h, "T", null, null );
					
					print( currdate("log"), " : StartSurv : ", cmd, BR, _ );
					StartSurv( t_work );
					
					if ( t_cond[cond_ok_no].bKeep == false )
						{
						// Supprime la condition d'attente
						for( ; file_exists( t_cond[cond_ok_no].wrecv ) ; Sleep(1) )
							unlink( t_cond[cond_ok_no].wrecv );
						n3 = t_cond[cond_ok_no].wrecv_id;
						for( k = 0 ; k < nb_cond ; k++ )
							{
							if ( t_cond[k].wrecv_id == n3 )
								{
								t_cond[k].bKeep = -1;
								t_cond[k].timeout = 0;
								}
							}
						}
					}
		
				//----------------------------------------------------------------------
				// AUCUN REVEIL ET AUCUN FLUX POUR CE FICHIER
				//----------------------------------------------------------------------
				else
					{
					// PASSE LE STATUS A T OU K
					if ( atoi( var_get( h, "errno" ) ) )
						//mft_save2( t_work, h, "K", null, null, cat_fname );
						mftcat_upd( h, "K", null, null );
					else
						//mft_save2( t_work, h, "T", null, null, cat_fname );
						mftcat_upd( h, "T", null, null );
					}
				}
		
		
			//--------------------------------------------------------------------------
			// CHERCHE LES PROCESS EN TIMEOUT ET LES DEMARRE
			//--------------------------------------------------------------------------
			
			for( i = 0 ; i < nb_cond ; i++ )
				{
				if ( ( t_cond[i].timeout )&&( t_cond[i].timeout < time(NULL) ) )
					{
					strdcpy( &cmd, t_cond[i].cmde, " timeout timeout", _ );
					
					StartSurv( t_work );
					unlink( t_cond[i].wrecv );
					}
				}


			//--------------------------------------------------------------------------
//logw( "robot wrecv : 3", _ );

			ssie7free( t_work );

//logw( "robot wrecv : 4", _ );
			}
		}
		
	
#undef cat_fname
#undef log_fname


















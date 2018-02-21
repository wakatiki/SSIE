#include "ssie7.h"

extern char *flux_etape;
extern int	etape_no, etape_log_id;



//------------------------------------------------------------------------------
int rec_execute( T_WORK* t_work )
// EXECUTE UNE ETAPE DE FLUX DE TYPE REC (Attente de réception d'un fichier par MFT)
//------------------------------------------------------------------------------
	{
	int _i;
	
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
/*
	#ifdef OS_WINDOWS
	bool bUNZIP = false;
	bool bPRINT = false;
	char *PrinterName, *Sysout = null;
	#endif
*/	
	//--------------------------------------------------------------------------
	// INITIALISATION DE L'ETAPE REC
	//--------------------------------------------------------------------------

	tr( "rec_execute_1_42");


	// UN FICHIER ETAIT EN COURS DE TRAITEMENT ?
	
	strdcpy( &s4, "rec_cond_", flux_etape, _ );
	if ( v = var_get( hFlux , s4 ) )
		strdcpy( &s1, v, _ );
	else
		{
		tr( "rec_execute_2");
		
		// LECTURE DES CONDITIONS
		k = 0;
		v = var_get( hEtape, "agr_start_cond" );
tr2( "agr_start_cond=", v );
		if ( ( v )&&( (trim(v))[0] ) )
			{
			tr( "rec_execute_3");
			strdcpy( &s1, "", _ );
			if ( ts1 ) free( ts1 );
			ts1 = str_explode( v, "\n" );
			for( i = 0 ; ts1[i] ; i++ )
				{
				if ( ! (trim( ts1[i] )[0]) ) continue;
				tr( "rec_execute_4");
				if ( ts2 ) free( ts2 );
				ts2 = str_explode( ts1[i], "=>" );
				if ( ( ! ts2[0] )||( ! ts2[1] ) ) continue;
				tr( "rec_execute_5");
				p = 0;
				if ( ts2[0][0] == '[' )
					{
					if ( (p = strpos(ts2[0], "]", 1)) < 0 ) p = 0;
					else p++;
					}
				strdcpy( &s, &(ts2[0][p]), _ );
				var_list_replace( hFlux, &s );
				strdcat( &s1, s, " => ", ts2[1], BR, _ );
				k++;
				}
			}

		if ( k == 0 )
			{
			tr( "rec_execute_6");
			hlog_raise_status( hLog, etape_log_id, "ERREUR : aucune condition de déclenchement", _ );
			var_to_str( hEtape, &s );
			hlogc( hLog, etape_log_id, "", strd_replace( &s, ",", ", " ), _ );
			hlogu_status( hLog, 0, "ERREUR : aucune condition de déclenchement", _ );
			var_set( hFlux, "flux_do_restart", "false" );
			var_set( hFlux, "etape_next", "stop" );
			return true;
			}
		
		var_set( hFlux, s4, s1 );
		var_list_replace( hFlux, &s1 );
		hlogc( hLog, etape_log_id, "", "Attend : <b>", s1, "</b>", _ );


		// CALCUL L'HEURE LIMITE A NE PAS DEPASSE
		
		v4 = var_get( hEtape, "agr_horaire" );
		if ( ( v4 )&&( v4[0] ) )
			{
			tr( "rec_execute_7");
			v1 = var_get( hFlux, strdcpy( &s, "etape_", flux_etape, "_dt_start", _ ) );
			n = atoi( v1 );
			n1 = n+(2*24*60*60);
	
			strdcpy( &s2, v4, _ );
			var_list_replace( hFlux, &s2 );
			strd_replace( &s2, ",", " " );
			strd_ireplace( &s2, "h", ":" );

			ts1 = str_explode( s2, " " );
			for( i = 0 ; ts1[i] ; i++ )
				{
				if ( ! ts1[i][0] ) continue;
				tr( "rec_execute_8");
				if ( ts2 ) free( ts2 );
				ts2 = str_explode( ts1[i], ":" );
				
				strdcpy( &s, itod( time(NULL), "jj/mm/aaaa" ), " ",
					ts2[0], ":", (char*)((ts2[1]) ? ts2[1] : "00" ), ":00", _ );
				
				n2 = dtoi( s, "jj/mm/aaaa hh:mm:ss" );
				
				if ( n2 <= n ) // date limite antérieure à la date de démarrage
					n2 += (24*60*60); // On ajoute une journée;
				
				if ( n1 > n2 ) n1 = n2;
				}
			
			if ( n1 < (n+(2*24*60*60)) )
				{
				tr( "rec_execute_9");
				v1 = itoa( n1 );
				var_set(  hFlux, "rec_limite", v1 );
				hlogc( hLog, etape_log_id, "", "heure limite=", itod( n1, "jj/mm/aaaa hh:mm:ss" ), _ );
				}
			else
				var_set(  hFlux, "rec_limite", v1 = "0" );
			}
		else
			var_set(  hFlux, "rec_limite", v1 = "0" );
			
				
		// DEMANDE LE REDEMARRAGE AUTOMATIQUE
		
		v = var_get( hFlux, "flux_do_restart" );
		if ( !v )
			var_set( hFlux, "flux_do_restart", "true" );

		
		// PLACE LE FLUX EN ATTENTE
		
		tr( "rec_execute_10");
		
		p = strpos( s1, "\n", 0 );
		
		hlog_raise_status( hLog, etape_log_id, "ATTEND des réceptions de fichiers : ",
					 v2 = substr( s1, 0, p ), _ );
		hlogu_status( hLog, 0, "Etape ", flux_etape, " : rec : ATTEND des réceptions de fichiers : ",
					 v2, _ );
		}


	//--------------------------------------------------------------------------
	// RECHERCHE DANS LE CATALOGUE MFT SI DES TRANSFERTS ME CONVIENNENT
	//--------------------------------------------------------------------------
	// s1 contient les conditions
	
	tr( "rec_execute_11");
	
	if ( ts1 ) free( ts1 );
	ts1 = str_explode( s1, "\n" );
	strdcpy( &s4, "", _ );
	hLock = false; strdcpy( &ida, "", _ );
	n4 = 0;
	for( _i = 0 ; ts1[_i] ; _i++ )
		{
		if ( !( ts1[_i][0] ) ) continue;
		tr( "rec_execute_12");
		ts2 = str_explode( ts1[_i], "=>" );
		p = 0;
		if ( ts2[0][0] == '[' )
			{
			if ( (p = strpos(ts2[0], "]", 1)) < 0 ) p = 0;
			else
				{
				p++;
				if ( atoi( &(ts2[0][1]) ) == 1 )
					var_set( hEtape, "agr_one_by_one", "Oui" );
				}
			}
			
		strdcpy( &s3, &(ts2[0][p]), _ );
		strd_replace( &s3, "|", " or " );
		strdcpy( &cmd, s3, " idf=!ACK direct=recv status=F", _ );
		if (( bd_select( hMftCat, cmd, "dt_upd", &s3 ) == ERROR )||( s3[0] == 0 ))
			break;
		
		tr( "rec_execute_13");
		v = u10; u10 = s3; s3 = v;
		for( p6 = 0, l = strlen(u10) ; p6 < l ; p6 += 8 )
			{
			tr( "rec_execute_14");
			if ( ( hLock )&&( ida[0] ) ) bd_unlock( hMftCat, ida, hLock );
			strdncpy( &ida, &(u10[p6]), 8, _ );
			
			// Verrouille le transfert
			
			hLock = bd_lock( hMftCat, ida );
			if ( hLock == false ) continue;
			
			// restart_onrecv ou one_by_one
			if (( v = var_get( hEtape, "agr_one_by_one" ) )
				&&( !stricmp( v, "Oui" ) )) l = 8; // On ne traite que le premier
			
			// CHARGE LE TRANSFERT
			
			var_free( h );
			if ( (h = mftcat_load( ida )) == ERROR )
				{
				bd_unlock( hMftCat, ida, hLock );
				hLock = 0;
				continue;
				}
			
			// VERIFIE LE STATUS
			v = var_get( h, "status" );
			if ( !v ) continue;
			if ( v[0] != 'F' )
				{
				bd_unlock( hMftCat, ida, hLock );
				hLock = 0;
				// LANCE UNE REPARATION D'INDEXE POUR LE CAS OU
				mftcat_repair( ida );
				continue;
				}
			
			
			tr( "rec_execute_15");
				
			n4++;
			
			strdcpy( &fname, var_get( h, "fname" ), _ );

			
			// Ajoute à la liste des AR à envoyer
			v1 = var_get( h, "ruser" );
			v2 = var_get( hFlux, "sendack_list" );
			strdcpy( &s3, v2, "|", substr( v1, 0, 8 ), ",", var_get( h, "spart" ), _ );
//			strdcpy( &s3, v2, "|", substr( v1, 0, 5 ), substr( v1, 8, 3 ), ",", var_get( h, "spart" ), _ );
			var_set( hFlux, "sendack_list", s3 );
			
		
			// Substitution des variables dans le nom du fichier
			strdcpy( &fname2, ts2[1], _ );
			var_list_prepare( h );
			var_list_replace( h, &fname2 );
			var_list_replace( hFlux, &fname2 );
			
			
			// Log les info
			tr( "rec_execute_16");
			hlogc( hLog, etape_log_id, "", "<b>Réception : ", ts2[0], " => ", fname2, "</b>", _ );
tr2( "rec_execute_16", "a");
			var_free( h1 );
tr2( "rec_execute_16", "b");
			h1 = var_dup( h );
tr2( "rec_execute_16", "c");
			var_delete( h1, "var_list" );
tr2( "rec_execute_16", "d");
			var_delete( h1, "mft_server_recv" );
			var_delete( h1, "rpart_adm_email" );
			var_delete( h1, "_hlog_dir" );
			var_delete( h1, "_hlog_id" );
			var_delete( h1, "_log_id" );
			var_delete( h1, "dt_last_access" );
			var_delete( h1, "dt_start" );
			var_delete( h1, "dt_crea" );
			var_delete( h1, "dt_upd" );
			var_delete( h1, "day_upd" );
			var_delete( h1, "_recep_fname" );
			var_delete( h1, "error_msg" );
			var_delete( h1, "errno" );
			var_delete( h1, "errmsg" );
			var_delete( h1, "part" );
			var_delete( h1, "partname" );
 			var_delete( h1, "_elib_bd_index" );
 			var_delete( h1, "_elib_bd_vlf_mft_log" );
tr2( "rec_execute_16", "e");
			var_to_str_nq( h1, &s );
tr2( "rec_execute_16", "f");
			hlogc( hLog, etape_log_id, "", "INFO DU TRANSFERT :\n", s, _ );

			
			//--------------------------------------------------------------------------
			// VERIFICATION DU REPERTOIRE DE DESTINATION
			//--------------------------------------------------------------------------
			
			tr( "rec_execute_17");
			
			if ( strpos( fname2, "(+)", 0 ) == 0 ) // Si le nom commence par (+) on crée le répertoire
				{
				tr( "rec_execute_18");
				strdcpy( &fname2, &(fname2[3]), _ );
				trim( fname2 );
				if ( !dir_exists( v = file_getdir( fname2 ) ) )
					{
					mkdir( v );
					hlogc( hLog, etape_log_id, "", "mkdir ", v, _ );
					}
				}
			
				
		/*
				if ( !dir_exists( v = file_getdir( fname2 ) ) )
					{
					hlogu_status( hLog, etape_log_id, "ERREUR : Le répertoire '", v, "' n'existe pas ou est inaccessible", _ );
					on_error_execute( t_work );
					return true;
					}
		*/	
			
			
			//--------------------------------------------------------------------------
			// ARCHIVAGE D'UN EVENTUEL FICHIER DEJA EXISTANT
			//--------------------------------------------------------------------------
			
			if ( strpos( fname2, "(#)", 0 ) == (n=(strlen(fname2)-3)) )
				{				// Le nom doit se terminer par (#), sinon le fichier sera écrasé
				tr( "rec_execute_19");
				fname2[n] = 0;
				trim( fname2 );
				if ( file_exists( fname2 ) )
					{
					tr( "rec_execute_20");
					// ARCHIVE LE FICHIER EXISTANT
					strdcpy( &fname1, fname2, ".", itod( file_date(fname2), "aaaa_mm_jj" ), _ );
					n = strlen( fname1 );
					for( i = 1 ; file_exists( fname1 ) ; i++ )
						{
						tr( "rec_execute_21");
						fname1[n] = 0;
						strdcat( &fname1, "_", itoa(i), _ );
						}
		
					for( n = 0 ; n < 100 ; n++ )
						{
						tr( "rec_execute_22");
						file_move( fname2, fname1 );
						Sleep(1);
						if ( ( !file_exists( fname2 ) )&&( file_exists( fname1 ) ) ) break;
						}
					if ( n >= 100 )
						{
						tr( "rec_execute_23");
						hlog_raise_status( hLog, etape_log_id, "ERREUR : Pb renommage du fichier : ", fname2, " vers ", fname1, _ );
						
						// PASSE LE TRANSFERT A L'ETAT T
						
						mftcat_logw( h, "mersi_reception : file_move ", fname2, " => ", fname1, " : ERREUR", _ );
						bd_unlock( hMftCat, ida, hLock );
						hLock = 0;
						mftcat_upd( h, "T", null, null );

						on_error_execute( t_work );
						continue;
						}
					}
				}
			
			tr( "rec_execute_24");
			
			trim(fname2);
			if ( ( fname2[strlen(fname2)-1] == '/' )||( fname2[strlen(fname2)-1] == '\\' ) )
				{
				tr( "rec_execute_25");
				hlog_raise_status( hLog, etape_log_id, "ERREUR : Le nom final du fichier n'est pas valide : ", fname2, _ );
				
				// PASSE LE TRANSFERT A L'ETAT T
				
//				mftcat_logw( h, "mersi_reception : file_move ", fname, " => ", fname2, " : ERREUR", _ );
				bd_unlock( hMftCat, ida, hLock );
				hLock = 0;
				mftcat_upd( h, "T", null, null );

				on_error_execute( t_work );
				continue;
				}
			
			
			var_set( hFlux, "_FNAME", fname2 );
			v2 = var_get( hFlux, "var_list" );
			if ( !v2 )
				{
				var_set( hFlux, "var_list", ",FNAME," );
				}
		    else
		        {
				if ( stripos( v2, ",FNAME", 0 ) < 0 )
		           	var_set( hFlux, "var_list", strdcpy( &s1, ",FNAME", v2, _ ) );
		        else
		           	{
					if ( stripos( v2, ",FNAME,", 0 ) < 0 )
		               	var_set( hFlux, "var_list", strdcpy( &s1, v2, "FNAME", _ ) );
					}
		        }
		
			v3 = var_get( h, "spart" );
			var_set( hFlux, "_PARTNAME", v3 );
			v2 = var_get( hFlux, "var_list" );
			if ( stripos( v2, ",PARTNAME", 0 ) < 0 )
		       	var_set( hFlux, "var_list", strdcpy( &s1, ",PARTNAME", v2, _ ) );
		    else
		       	{
				if ( stripos( v2, ",PARTNAME,", 0 ) < 0 )
		           	var_set( hFlux, "var_list", strdcpy( &s1, v2, "PARTNAME", _ ) );
				}
		
			tr( "rec_execute_26");
			
			v3 = var_get( h, "parm" );
			if ( !v3 ) v3 = "";
			var_set( hFlux, "_PARM", v3 );
			v2 = var_get( hFlux, "var_list" );
			if ( stripos( v2, ",PARM", 0 ) < 0 )
		       	var_set( hFlux, "var_list", strdcpy( &s1, ",PARM", v2, _ ) );
		    else
		       	{
				if ( stripos( v2, ",PARM,", 0 ) < 0 )
		           	var_set( hFlux, "var_list", strdcpy( &s1, v2, "PARM", _ ) );
				}
		
			v3 = var_get( h, "idtu" );
			if ( !v3 ) v3 = "";
			var_set( hFlux, "_IDTU", v3 );
			v2 = var_get( hFlux, "var_list" );
			if ( stripos( v2, ",IDTU", 0 ) < 0 )
		       	var_set( hFlux, "var_list", strdcpy( &s1, ",IDTU", v2, _ ) );
		    else
		       	{
				if ( stripos( v2, ",IDTU,", 0 ) < 0 )
		           	var_set( hFlux, "var_list", strdcpy( &s1, v2, "IDTU", _ ) );
				}
		
			v3 = var_get( h, "suser" );
			if ( !v3 ) v3 = "";
			var_set( hFlux, "_SUSER", v3 );
			v2 = var_get( hFlux, "var_list" );
			if ( stripos( v2, ",SUSER", 0 ) < 0 )
		       	var_set( hFlux, "var_list", strdcpy( &s1, ",SUSER", v2, _ ) );
		    else
		       	{
				if ( stripos( v2, ",SUSER,", 0 ) < 0 )
		           	var_set( hFlux, "var_list", strdcpy( &s1, v2, "SUSER", _ ) );
				}
		
			v3 = var_get( h, "sappl" );
			if ( !v3 ) v3 = "";
			var_set( hFlux, "_SAPPL", v3 );
			v2 = var_get( hFlux, "var_list" );
			if ( stripos( v2, ",SAPPL", 0 ) < 0 )
		       	var_set( hFlux, "var_list", strdcpy( &s1, ",SAPPL", v2, _ ) );
		    else
		       	{
				if ( stripos( v2, ",SAPPL,", 0 ) < 0 )
		           	var_set( hFlux, "var_list", strdcpy( &s1, v2, "SAPPL", _ ) );
				}
		
			v3 = var_get( h, "ruser" );
			if ( !v3 ) v3 = "";
			var_set( hFlux, "_RUSER", v3 );
			v2 = var_get( hFlux, "var_list" );
			if ( stripos( v2, ",RUSER", 0 ) < 0 )
		       	var_set( hFlux, "var_list", strdcpy( &s1, ",RUSER", v2, _ ) );
		    else
		       	{
				if ( stripos( v2, ",RUSER,", 0 ) < 0 )
		           	var_set( hFlux, "var_list", strdcpy( &s1, v2, "RUSER", _ ) );
				}
		
			v3 = var_get( h, "rappl" );
			if ( !v3 ) v3 = "";
			var_set( hFlux, "_RAPPL", v3 );
			v2 = var_get( hFlux, "var_list" );
			if ( stripos( v2, ",RAPPL", 0 ) < 0 )
		       	var_set( hFlux, "var_list", strdcpy( &s1, ",RAPPL", v2, _ ) );
		    else
		       	{
				if ( stripos( v2, ",RAPPL,", 0 ) < 0 )
		           	var_set( hFlux, "var_list", strdcpy( &s1, v2, "RAPPL", _ ) );
				}
		
			var_list_prepare( hFlux );
		
			
			// INFO POUR LES STATISTIQUES
			
			v = var_get( hFlux , "part_list" );
			if ( !v ) var_set( hFlux, "part_list", strdcpy( &s, ",", var_get( h, "part" ), ",", _ ) );
			else
				{
				strdcpy( &s, ",", var_get( h, "part" ), ",", _ );
				if ( strpos( v, s, 0 ) < 0 )
					var_set( hFlux, "part_list", strdcpy( &s, v, var_get( h, "part" ), ",", _ ) );
				}
				
			v = var_get( hFlux, "recv_file_nb" );
			n = ( v ) ? atoi( v ) : 0;
			n++;
			var_set( hFlux, "recv_file_nb", itoa( n ) );
			
			v = var_get( hFlux, "recv_file_size" );
			ll = ( v ) ? atoi( v ) : 0;
			ll += atoi(var_get( h, "file_size" ));
			var_set( hFlux, "recv_file_size", itoa( ll ) );
			
			v = var_get( hFlux , "recv_file_slist" );
			strdcpy( &s, v, (char*)((v)?", ":""), file_getname(fname2), _ );
			var_set( hFlux, "recv_file_slist", s );
			
			
			//--------------------------------------------------------------------------
			// DEPLACE LE FICHIER
			//--------------------------------------------------------------------------
			
			tr( "rec_execute_27");
			
			if ( strcmp( fname2, fname ) )
				{
				move_file:
				
				tr( "rec_execute_28");
				
				var_set( hFlux, "rec_move_file_error_fname", fname );
				var_set( hFlux, "rec_move_file_error_fname2", fname2 );
				
				#ifdef OS_UNIX
				v2 = strdcpy( &s1, trim( var_get( hFlux, "app_login" ) ), _ );
				if ( (!v2)||( !(v2[0]) ) ) v2 = v3 = ssie_user;
				else
				if ( (p=strpos(v2,":",0)) >= 0 )
					{
					v2[p] = 0;
					v3 = &(v2[p+1]);
					}
				else
					v3 = v2;
				
				if ( strcmp( v2, ssie_user ) )
					{
					tr( "rec_execute_29");
					
					strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
							"mv -f ", fname, " ", fname2, _ );
					
					for( n = 10 ; n ; Sleep(10), n-- )
						{
						System( &s3, s );
//						if ( strnicmp( s3, "error create tmp file", -1 ) ) break;
						if ( !(trim( s3 )[0]) ) {logw("flux_exec_rec: prob mv: ",s3,_);break;}
						}
								
					hlogc( hLog, etape_log_id, "", "mv -f ", fname, " ", fname2, BR,
								s3, _ );
					
					if ( trim( s3 )[0] )
						{
						tr( "rec_execute_30");
						hlogc( hLog, etape_log_id, "ERREUR", "Pb déplacement du fichier ", fname, " vers : ", fname2, _ );
						hlogu_status( hLog, etape_log_id, "ERREUR", "Pb déplacement du fichier ", fname, " vers : ", fname2, _ );
						hlogu_status( hLog, 0, "ERREUR : Pb déplacement du fichier ", fname, " vers : ", fname2, _ );
						
						// PASSE LE TRANSFERT A L'ETAT T
						
						mftcat_logw( h, "mersi_reception : file_move ", fname, " => ", fname2, " : ERREUR", _ );
						bd_unlock( hMftCat, ida, hLock );
						hLock = 0;
						if ( mftcat_upd( h, "T", null, null ) == ERROR )
							{
							hlogc( hLog, etape_log_id, "ERREUR", "Pb changement de status à T (3)", _ );
							exit(0);
							}

						on_error_execute( t_work );
						return true;
						}
						
					strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
							"chown -R ", v2, ":", v3, " ", fname2, _ );
					
					System( &s3, s );		
					hlogc( hLog, etape_log_id, "", "chown -R ", v2, ":", v3, " ", fname2, BR,
								s3, _ );
					}
				else
				#endif
					{
					// Supprime un fichier de même nom existant
					tr( "rec_execute_31");
					
					for( n = 0 ; n < 100 ; n++ )
						{
						unlink( fname2 );
						Sleep(1);
						if ( !file_exists( fname2 ) ) break;
						}
						
					if ( n >= 100 )
						{
						hlogc( hLog, etape_log_id, "ERREUR", "Pb déplacement du fichier vers : ",
								fname2, " (il existe déjà et ne peut pas être supprimé !!)", _ );
						hlogu_status( hLog, 0, "ERREUR : Pb déplacement du fichier vers : ",
								fname2, " (il existe déjà et ne peut pas être supprimé !!)", _ );
						
						// PASSE LE TRANSFERT A L'ETAT T
						
						mftcat_logw( h, "mersi_reception : file_move ", fname, " => ", fname2, " : ERREUR", _ );
						bd_unlock( hMftCat, ida, hLock );
						hLock = 0;
						if ( mftcat_upd( h, "T", null, null ) == ERROR )
							{
							hlogc( hLog, etape_log_id, "ERREUR", "Pb changement de status à T (1)", _ );
							exit(0);
							}

						on_error_execute( t_work );
						continue;
						}
					
					for( n = 0 ; n < 100 ; n++ )
						{
						file_move( fname, fname2 );
						Sleep(1);
						if ( ( !file_exists( fname ) )&&( file_exists( fname2 ) ) ) break;
						}
					if ( n >= 100 )
						{
						hlogc( hLog, etape_log_id, "ERREUR", "Pb déplacement du fichier ", fname, " vers : ", fname2, _ );
						hlogu_status( hLog, etape_log_id, "ERREUR : Pb déplacement du fichier ", fname, " vers : ", fname2, _ );
						mftcat_logw( h, "mersi_reception : file_move ", fname, " => ", fname2, " : ERREUR", _ );
						
						// PASSE LE TRANSFERT A L'ETAT T
						
						bd_unlock( hMftCat, ida, hLock );
						hLock = 0;
						if ( mftcat_upd( h, "T", null, null ) == ERROR )
							{
							hlogc( hLog, etape_log_id, "ERREUR", "Pb changement de status du transfert à T (2)", _ );
							exit(0);
							}

						on_error_execute( t_work );
						continue;
						}
					}
		
				tr( "rec_execute_32");
				
				if ( var_get( hFlux, "rec_move_file_error_fname" ) )
					{
					hlogc( hLog, etape_log_id, "OK", "Déplacement du fichier ", fname, " vers : ",
							fname2, _ );
					}
				mftcat_logw( h, "mersi_reception : file_move ", fname, " => ", fname2, " : OK", _ );
				}

			// PASSE LE TRANSFERT A L'ETAT X
			
			bd_unlock( hMftCat, ida, hLock );
			hLock = 0;
//			mftcat_upd( h, "X", null, null );
			if ( mftcat_upd( h, "X", null, null ) == ERROR )
				{
				hlogc( hLog, etape_log_id, "ALERTE", "Pb changement de status du transfert à X", _ );
				exit(0);
				}

				
			var_delete( hFlux, "rec_move_file_error_fname" );
			var_delete( hFlux, "rec_move_file_error_fname2" );
			var_set( hFlux, "rec_move_file_ok", "true" );
				
			//--------------------------------------------------------------------------
			// EXECUTE LE SCRIPT DE RECEPTION
			//--------------------------------------------------------------------------
			
			script_run:
			
			tr( "rec_execute_34");
				
			v = var_get( hEtape, "agr_script" );
			if ( ( v )&&( (trim(v))[0] ) )
				{
				tr( "rec_execute_35");
				id = hlogc( hLog, etape_log_id, "En cours", "Exécute le script de réception", _ );
//				h1 = h;
				ssie7_script_exec( t_work, true );
//				h = h1;
				}
				
			}
		
		tr( "rec_execute_36");
		}

	
	tr( "rec_execute_38");
	if ( ( hLock )&&( ida[0] ) ) bd_unlock( hMftCat, ida, hLock );

/*
	if ( n4 == 0 )
		{
		tr( "rec_execute_39");
		strdcat( &s4, ts1[_i], "\n", _ );
		}
*/
	
	//--------------------------------------------------------------------------
	if (( n4 == 0 )&&( n=atoi(var_get(  hFlux, "rec_limite" )) )&&( sysdate >= n ))
	//--------------------------------------------------------------------------
		{
		tr( "rec_execute_40");
		hlogu_status( hLog, etape_log_id, "ERREUR : La date limite de réception est dépassée", _ );
		hlogc( hLog, etape_log_id, "", "TIMEOUT", _ );
		on_error_execute( t_work );
		return true;
		}
	

	if ( n4 == 0 )
		{
		tr( "rec_execute_41");
		var_set( hFlux, "wait_date", itoa( sysdate + 10 ) );
		return false;
		}
	
	
	//--------------------------------------------------------------------------
	// REMONTE LE STATUS
	//--------------------------------------------------------------------------
	

	tr( "rec_execute_35");
	
	hlogc_stat( hLog, etape_log_id, "ERREUR", &p1, "ALERT", &p2, _ );

	if ( !(p1+p2) )
		{
		n = atoi( var_get( hFlux, "recv_file_nb" ) );
		strdcpy( &s, itoa(n), " fichiers reçus", _ );
		hlogu_status( hLog, etape_log_id, "OK : ", s, _ );
		hlogu_status( hLog, 0, "OK : ", s, _ );
		}
	else
	if ( p1 )
		hlogu_status( hLog, etape_log_id, "ERREUR", _ );
	else
		hlogu_status( hLog, etape_log_id, "ALERTE", _ );
	
	
	tr( "rec_execute_42.");
	return true;
	}





















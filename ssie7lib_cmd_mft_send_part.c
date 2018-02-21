
#define bPublish			b1	// true si hPart trf_direction=recv ou option publish
#define bNocrypt			b2
#define bAscii				b4
#define bGateway			b3
#define cat_fname			fname3
#define log_fname			fname4
#define tran_id				(t_work->u1)
#define src_fname			(t_work->u3)

//------------------------------------------------------------------------------
int cmd_mft_send_part( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	int nb, canal = 0;
	int bRecv = false;
	int bNoSSIE = false;
	int _l, _p6;
	char *lock_fname = null, *_u10 = null;
//	HANDLE _hLock = (HANDLE)hNULL;

tr( "cmd_mft_send_part_1" );

	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();


	//----------------------------------------------------------------------
	// INITIALISATION PROCESSUS
	//----------------------------------------------------------------------


	if ( request_mode == MODE_TCP )
		{
		if ( ! session_partname )
			{
tr( "cmd_mft_send_part_2" );
			print( "ERREUR RECEPTEUR : Vous devez vous identifier", _ );
			return ERROR;
			}
		bRecv = true;

		logw( "mft send_part:mode recv:", session_partname, _ );
		}

	v = var_getn( hReq, 2, &vn );
	if (( !v )&&(bRecv))	v = var_getn( hReq, 1, &vn ); // Cas d'une commande : mft_server_send
	if ( !v )
		{
tr( "cmd_mft_send_part_3" );
		logw( "ERREUR : syntaxe : mft send_part [partname] : ", cmd, _ );
		return ERROR;
		}

	partname = strdcpy( &part, vn, _ );

	if ( bRecv )
		{
tr( "cmd_mft_send_part_4" );
		if ( stricmp( partname, session_partname ) )
			{
			print( "NO MORE FILE TO SEND", _ );
			return OK;
			}
		bGateway = false;
		if ( !part_exists( t_work ) ) // retourne dans fname le nom du fichier partenaire
			{
			print( "NO MORE FILE TO SEND", _ );
			return OK;
			}

//if (bRecv ) logw( "mft send_part 4", _ );
		var_free( hPart );
		hPart = var_load( fname );
		goto debut_send_part;
		}

tr( "cmd_mft_send_part_5" );

	if ( !bRecv )
		{
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_send_part_", part, ".pid", _ );
		file_save_str( fname, itoa(getpid()), -1 );
		}

	//----------------------------------------------------------------------
	// LECTURE INFO SUR LE PARTENAIRE
	//----------------------------------------------------------------------

	bGateway = false;
	var_free( hPart );
	if ( part_exists( t_work ) ) // retourne dans fname le nom du fichier partenaire
		{
tr( "cmd_mft_send_part_6" );

		if ( stricmp( partname, part ) ) // Utilisation d'une ADL ?
			strdcpy( &part, partname, _ );

		hPart = var_load( fname );

		//--------------------------------------------------------------------------
		// VERIFIE SI LE DESTINATAIRE EST EN SSIE v7
		// Non => RECHERCHE UNE PASSERELLE SSIE/MFT
		//--------------------------------------------------------------------------

		v = var_get( hPart, "product_name" );
		v1 = var_get( hPart, "dns_ssie" );
		if (( !v1 )||( !(v1[0]) ))
			v1 = var_get( hPart, "ip_ssie" );

tr( "cmd_mft_send_part_7" );
		if (( ( !v )||( strnicmp( v, "SSIE v7", 7 ) ) )
//				&&( ( !v1 )||( !(v1[0]) ) )
			)
			{
			if ( !mft_exists() )
				{
tr( "cmd_mft_send_part_8" );
				partname = var_get( hPart, "mft_gateway" );
				if ( ! partname ) partname = var_get( hSrvConf, "mft_gateway" );
				if ( ! partname ) partname = "MFTCCWEX";

				// VERIFIE QUE LA PASSERELLE EST DIFINIE EN LOCAL, sinon RECHERCHE SUR L'ANNUAIRE

				if ( ! part_exists( t_work ) )
					{
					error_set( 1, "La passerelle ", partname, " n'existe pas", _ );
					var_free( hPart );
					hPart = 0;
					}

				bGateway = true;
				}
			else	// MFT EN LOCAL => ON NE TRAITE PAS, ON PASSE LE TRANSFERT EN STATUS D
				{
tr( "cmd_mft_send_part_9" );
				bNoSSIE = true;
				}
			}
		}
	else
		{
tr( "cmd_mft_send_part_10" );
		hPart = 0;
		error_set( 1, "Le partenaire ", partname, " n'existe pas", _ );
		}
	//----------------------------------------------------------------------
	// SEND EXECUTE : CONNEXION AU DISTANT OU A LA PASSERELLE
	//----------------------------------------------------------------------

	hCnx = ERROR;
	bPublish = false;

	if ( hPart )
		{
tr( "cmd_mft_send_part_11" );
		bPublish = ( ( v=var_get( hPart, "trf_direction" ) )&&( !stricmp( v, "recv" ) ) );
		if (( !bPublish )&&( ! bNoSSIE ))
			{
			// Vérifie que le dns du distant n'est pas identique au serveur
tr( "cmd_mft_send_part_12" );
			b = true;
			if ( stricmp( part, var_get( hSrvConf, "partname" ) ) )
				{
tr( "cmd_mft_send_part_13" );
				v1 = trim(var_get( hPart, "dns_ssie" ));
tr( "cmd_mft_send_part_13a" );
				v2 = trim(var_get( hSrvConf, "dns_ssie" ));
tr( "cmd_mft_send_part_13b" );
				if ( ( v1 )&&( v2 )&&( v1[0] )&&( v2[0] )&&( ! stricmp( v1, v2 ) ) )
					{
tr( "cmd_mft_send_part_14" );
					b = false;
					error_set( 1, "ERREUR : DNS du distant (", part, ") est identique au notre !!!", _ );
					}
tr( "cmd_mft_send_part_14a" );
				}

			if ( ( b )&&( ssie7connect( t_work, false ) == ERROR ) )
				{
tr( "cmd_mft_send_part_14b" );
				if ( bGateway )
					{
tr( "cmd_mft_send_part_15" );
					for( n1 = 0 ; n1 < 5 ; n1++ )
						if ( ssie7connect( t_work, false ) != ERROR ) break;
					}

				// Si pb de connexion ==> Tous les fichiers en attente partent en erreur
				// Si partenaire inexistant => pas de retentative
				// Si pb de connexion => retentatives toutes les 5 secondes jusqu'au timeout
				}
			}
tr( "cmd_mft_send_part_16" );
		if ( bNoSSIE )
			{
			var_free( hPart );
			hPart = 0;
			}
		}

	//----------------------------------------------------------------------
	// BOUCLE SUR LES TRANSFERTS EN ATTENTE
	//----------------------------------------------------------------------

	debut_send_part:

tr( "cmd_mft_send_part_17" );
//if (bRecv ) logw( "mft send_part 17", _ );

	if ( !bRecv )
		strdcpy( &s, "status=C|H part=", part, _ );
	else
		strdcpy( &s, "status=H part=", part, _ );


	if ( bd_select( hMftCat, s, null, &s4 ) == ERROR )
		{
tr( "cmd_mft_send_part_18" );
//if (bRecv ) logw( "mft send_part 18", _ );
		if ( !bRecv )
			{
			strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_send_part_", part, ".pid", _ );
			file_save_str( fname, itoa(getpid()), -1 );
			}
		return OK;
		}
//if (bRecv ) logw( "mft send_part 19:", s4, _ );

tr( "cmd_mft_send_part_19" );
	log_id = 0;
	v = _u10; _u10 = s4; s4 = v;
	if ( _u10[1] )
	for( _p6 = 0, _l = strlen(_u10) ; _p6 < _l ; _p6 += 8 )
		{
tr( "cmd_mft_send_part_19a" );
		strdncpy( &ida, &(_u10[_p6]), 8, _ );


		if ( !bRecv )
			{
			strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_send_part_", part, ".pid", _ );
			file_save_str( fname, itoa(getpid()), -1 );
			}


tr( "cmd_mft_send_part_20" );
tr2( BR "ida=", ida );
//if (bRecv ) logw( "mft send_part 20:", ida, _ );
		//----------------------------------------------------------------------
		// REPARTITION DE LA CHARGE SUR PLUSIEURS CONNEXIONS
		//----------------------------------------------------------------------




		//----------------------------------------------------------------------
		// CHARGE INFO SUR LE TRANSFERT
		//----------------------------------------------------------------------

		var_free( hReq );
		if ( (hReq = mftcat_load( ida )) == ERROR )
			{
tr( "cmd_mft_send_part_20a" );
			strdcpy( &cmd, argv[0], " mft repair ida=", ida, _ );
			system( cmd );
			logw( "send_part ", part, " : ERREUR : Le transfert n'existe pas : ", ida, " => ", cmd, _ );
			continue;
//			exit(0);
			}



tr( "cmd_mft_send_part_21" );
		//----------------------------------------------------------------------
		// VERIFIE QUE LE TRANSFERT N'EST PAS DEJA TERMINE OU ANNULE
		//----------------------------------------------------------------------

		v = var_get( hReq, "status" );
		if ( !v ) continue;

tr( "cmd_mft_send_part_21a" );
//if (bRecv ) logw( "mft send_part 21a", _ );
		if (( v[0] != 'C' )&&( v[0] != 'H' ))
			{
tr( "cmd_mft_send_part_21b" );
//if (bRecv ) logw( "mft send_part 21b", _ );
			if ( v[0] == 'W' ) continue;
			else
			if (( v[0] == 'T' )||( v[0] == 'K' )||( v[0] == 'X' )) // Transfert terminé => On ne le traite plus
				{
tr( "cmd_mft_send_part_21c" );
				// LANCE UNE REPARATION D'INDEXE POUR LE CAS OU
				mftcat_repair( ida );
				continue;
				}
			}

tr( "cmd_mft_send_part_21d" );
//if (bRecv ) logw( "mft send_part 21d", _ );
		//----------------------------------------------------------------------
		// OUVERTURE DE LA LOG DU TRANSFERT
		//----------------------------------------------------------------------

		if ( log_id ) hlog_free( hLog );
tr( "cmd_mft_send_part_21d1" );

// patch 7.2.a.3 : on annule les mise à jour des log des flux (ça pause des pb en cas de fort charge)
/*
		log_id = atoi(var_get( hReq, "_log_id" ));

tr2( "cmd_mft_send_part_21e:", currdate("log") );
		if ( log_id )
			{
//tr( "cmd_mft_send_part_21f" );
			strdcpy( &fname, dirflux, DIR_CHAR, itoa(log_id), ".run.lock", _ );
tr2( BR "cmd_mft_send_part_21f:", fname );
			if (( !lock_fname )||( strcmp( lock_fname, fname ) ))
				{
				if ( _hLock )
					{ file_unlock( _hLock ) ; _hLock = (HANDLE)hNULL;
					 free( lock_fname ); lock_fname = null; }
//				_hLock = file_lock0( fname );
				_hLock = file_lock( fname );
				if ( ! _hLock )
					{
tr2( "cmd_mft_send_part_21f1:", currdate("log") );
					 log_id=0; continue; }
				lock_fname = fname;
				fname = null;
				}
tr2( "cmd_mft_send_part_21g:", currdate("log") );
			hLog = hlog_init( var_get( hReq, "_hlog_dir" ), var_get( hReq, "_hlog_id" ) );
			}
*/

tr( "cmd_mft_send_part_22" );

		if ( atoi(var_get( hReq, "_transfert_canceled" )) == 1 ) // Transfert annulé
			{
			// SI TRANSFERT VIA LA PASSERELLE => ANNULATION SUR LA PASSERELLE AUSSI

tr( "cmd_mft_send_part_23" );
			if (( bGateway )&&( tran_id = var_get( hReq, "tran_id" ) ))
				{
				p3 = strpos( tran_id, "#", 0 );
				strdncpy( &ip, tran_id, p3, _ );

				strdcpy( &cmd, "mft_server_recv cancel ", &(tran_id[p3+1]), _ );

				if ( tcp_request( ip, cmd, &s1 ) == sERROR )
					{
					if ( log_id )  hlogc( hLog, log_id, "ERREUR", ip, " : ", error_msg, _ );
//					if ( log_id ) hlog_free( hLog );
					mftcat_logw( hReq, "ERREUR GATEWAY CANCEL : ", ip, " : ", error_msg, _ );
					mftcat_upd( hReq, null, null, null );
					continue; // On ne supprime pas de la file d'attente pour retentatives.
					}

				mftcat_logw( hReq, "ANNULATION SUR LA PASSERELLE : ", ip, " : ", s1, BR, _ );
				}

			mftcat_upd( hReq, "K", null, null );
			if ( log_id ) hlogu_status( hLog, log_id, "ANNULE", _ );

			// TRANSFERT SUIVANT

			continue;
			}

//if ( bRecv )
//logw( "mft_send_part:mode recv 7 :", session_partname, _ );

		//----------------------------------------------------------------------
		// PARTENAIRE INEXISTANT ???
		//----------------------------------------------------------------------

tr( "cmd_mft_send_part_24" );

		if ( hPart == 0 )
			{
tr( "cmd_mft_send_part_24a" );

			if ( bNoSSIE ) // Protocol autre que SSIE (MFT par exemple)
				{
tr( "cmd_mft_send_part_24b" );
				if ( bPublish )	// Partenaire en recv uniquement
					var_set( hReq, "publish", "true" );

				mftcat_upd( hReq, "D", null, null );
				continue;
				}

			if ( log_id )
				{
				hlogc( hLog, log_id, "ANNULE", error_msg, " ==> TRANSFERT ANNULE" BR, _ );
				hlogu_status( hLog, log_id, "ANNULE", _ );
//				hlog_free( hLog );
				}

			mftcat_logw( hReq, error_msg, " ==> TRANSFERT ANNULE" BR, _ );
			mftcat_upd( hReq, "K", "408", "ANNULE" );

//			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
//			file_delete( fname ); // Pas de retentative
			continue;
			}



		//----------------------------------------------------------------------
		// VERIFIE LE TIMEOUT (2 Heures par defaut)
		//----------------------------------------------------------------------


tr( "cmd_mft_send_part_25" );

		if ( !bRecv )
			{
			v = var_get( hReq, "_dt_ref_timeout" );
			if ( !v ) var_set( hReq, "_dt_ref_timeout", itoa(sysdate) );
			else
				{
				v2 = var_get( hReq, "status" );
				v1 = var_get( hReq, "errno" );
				if ( ( atoi( v1 ) )||( v2[0] == 'H' ) )
					{ // Transfert en erreur et toujours en cours
					v1 = var_get( hReq, "timeout" );
					if ( !v1 ) v1 = var_get( hSrvConf, "trf_timeout" );
					if ( atoi( v1 ) == 0 ) v1 = "120"; // 2 heures par défaut

					if ( (sysdate - atoi(v)) > (atoi( v1 )*60) )
						{
						// TIMEOUT
						v = var_get( hReq, "status" );
						if ( ( !v )||( v[0] != 'K' ) ) // Uniquement si pas déjà fait
							{
							strdcpy( &s, "ERREUR TIMEOUT:", var_get( hReq, "errmsg" ), _ );
							mftcat_logw( hReq, s, _ );
							mftcat_upd( hReq, "K", null, s );
							if ( log_id )
								{
								hlogc( hLog, log_id, s, _ );
								hlogu_status( hLog, log_id, "ERREUR TIMEOUT", _ );
								}
							}

						// SI TRANSFERT VIA LA PASSERELLE => ANNULATION SUR LA PASSERELLE AUSSI

						// On peut repasser plusieurs fois ici, si la demande d'annulation vers la passerelle ne passe pas.
						tran_id = var_get( hReq, "tran_id" );
						if (( tran_id )&&( bGateway ))
							{
							p3 = strpos( tran_id, "#", 0 );
							strdncpy( &ip, tran_id, p3, _ );

							strdcpy( &cmd, "mft_server_recv cancel ", &(tran_id[p3+1]), _ );

							if ( tcp_request( ip, cmd, &s1 ) == sERROR )
								{
								if ( log_id )  hlogc( hLog, log_id, "ERREUR", ip, " : ", error_msg, _ );
//								if ( log_id ) hlog_free( hLog );
								mftcat_logw( hReq, "ERREUR CANCEL : ", ip, " : ", error_msg, _ );
								mftcat_upd( hReq, null, null, null );
								continue;
								}

							mftcat_logw( hReq, "ANNULATION SUR LA PASSERELLE : ", ip, " : ", s1, _ );
							mftcat_upd( hReq, null, null, null );
							}

						// TRANSFERT SUIVANT

//						if ( log_id ) hlog_free( hLog );
//						strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
//						file_delete( fname ); // Pas de retentative

						continue;
						}
					}
				}
			}



		//----------------------------------------------------------------------
		// MODE PUBLISH ???
		//----------------------------------------------------------------------



tr( "cmd_mft_send_part_26" );
		b = ( ( v = var_get( hReq, "state" ) )&&( !stricmp( v, "hold" ) ) );

		if (( bPublish )||( bRecv )||( b )||( var_get( hReq, "publish" ) ))
			{
			if ( ! var_get( hReq, "published" ) )
				{
tr( "cmd_mft_send_part_26a" );
				var_set( hReq, "published", "true" );
				strdcpy( &s, "FICHIER EN ATTENTE DE TELECHARGEMENT", _ );
				mftcat_logw( hReq, s, _ );
				mftcat_upd( hReq, "H", "0", s );
tr( "cmd_mft_send_part_26b" );
//				if ( log_id )  hlogc( hLog, log_id, s, _ );
				}

			if ( bRecv ) goto debut_send_file;

tr( "cmd_mft_send_part_26c" );

			continue;
			}

		if ( bRecv ) goto debut_send_file;


		//----------------------------------------------------------------------
		// CONNEXION KO ???
		//----------------------------------------------------------------------

tr( "cmd_mft_send_part_27" );

		if ( hCnx == ERROR )
			{
			v = var_get( hReq, "errno" );
			v1 = (char*)(( bGateway ) ? "303" : "302" );

			n = time(null) - dtoi(var_get( hReq, "dt_upd" ), "flog");
			if (( n > 600 )||( !v )||( strcmp( v, v1 ) ))
				{
				if ( strcmp( v, v1 ) )
					{
					mftcat_logw( hReq, error_msg, _ );
					if ( log_id )
						{
						hlogc( hLog, log_id, "", error_msg, _ );
//						hlog_free( hLog );
						}
					}
				mftcat_upd( hReq, null, v1, error_msg );
				}

			continue;
			}
		

		// Trace que l'on utilise une passerelle
		//Correctif du probleme ou le partenaire distant se consedere MFT
		
		strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, var_get( hReq, "partname"), ".dat", _ );
		if ( ! file_exists( fname ) ) return ERROR;
		var_free( hPart );
                hPart = var_load( fname );
		//logw("product ok=",var_get( hPart, "product_name"),_);
		// cas ou product_name n'est pas recupéré
		if (v) v = var_get( hPart, "product_name"); else strdcpy(&v,"mft",_);//par defaut mft
			
		if ( strnicmp( v, "SSIE v7", 7 )) // si !=SSIE ou product_name = vide 
		{
		//TODO: tester si le partname est égale à MFT* ou MERSI* 
		//Si oui : poursuite du traitement
		//Si non : return ERROR : partname mal défin
			if (( !strnicmp(v, "mersi", 5)) || (!strnicmp( v, "MFT", 3 )) || (!strcmp( v, "" )) ) {
			//logw("mft send_part: mft_gw",_);
				if ( ( strcmp( partname, part ) )&&( ! var_get( hReq, "mft_gateway" ) ) )
				{
					var_set( hReq, "mft_gateway", partname );
					if ( log_id )
						hlogc( hLog, log_id, "", "Utilisation de la passerelle MFT : ", partname, _ );
					mftcat_logw( hReq, "Utilisation de la passerelle MFT : ", partname, _ );	
				}	
			}
		else {
		//logw("mft send_part: mft_gw KO",_);
		if ( log_id ) hlogc( hLog, log_id, "", "Erreur de parametrage : ", partname," /product_name=", v, _ );

		mftcat_logw( hReq, "Erreur de parametrage : ", partname, "/product_name=", v, _ );


		//if ( log_id ) hlogc( hLog, log_id, "", "ERREUR PARTENAIRE MAL PARAMETRE :" BR BR, "ERREUR", _ );
		//mftcat_logw( hReq, "ERREUR MFT GATEWAY KO A MENTIONNER", " ==> TRANSFERT KO" BR, _ );
                mftcat_upd( hReq, "K", "106", "ERREUR: parametrage KO" );
		          //             mftcat_upd( hReq, "K", null, null );
                //if ( log_id ) hlogu_status( hLog, log_id, "ANNULE", _ );
                var_set( hReq, "errno","106" );
		strdcpy( &src_fname, var_get( hReq, "fname" ), _ );
		                                file_delete( src_fname );
                                for( n = 100 ; ( n )&&( file_exists( src_fname ) ) ; n--, Sleep(10) )
                                        file_delete( src_fname );


		//logw("mft send_part: mft_gw KO end=",src_fname,_);
		return ERROR;
		}
		}

		//----------------------------------------------------------------------
		// SI UTILISATION D'UNE PASSERELLE MFT ET FICHIER DEJA ENVOYE
		// => ATTENTE QUE LA PASSERELLE AI BIEN TRANSMIS LE FICHIER AU DESTINATAIRE FINAL
		//----------------------------------------------------------------------


tr( "cmd_mft_send_part_28" );

		if ( atoi(var_get( hReq, "_transfert_en_attente_passerelle" )) == 1 )
			{
			// Demande le status à la passerelle

tr( "cmd_mft_send_part_29" );

			tran_id = var_get( hReq, "tran_id" );
			p3 = strpos( tran_id, "#", 0 );
			strdncpy( &s1, tran_id, p3, _ );

			strdcpy( &cmd, "mft_server_recv gw_status ", &(tran_id[p3+1]), _ );

			if ( tcp_request( s1, cmd, &s2 ) == sERROR )
				{
				if ( atoi( var_get( hReq, "errno" ) ) != 303 )
					{
					strdcat( &error_msg, "(", tran_id, ")", _ );
					if ( log_id )
						{
						hlogc( hLog, log_id, "", "PB CONTACTE PASSERELLE :" BR BR, error_msg, _ );
//						hlog_free( hLog );
						}

					mftcat_logw( hReq, error_msg, _ );
					mftcat_upd( hReq, null, "303", error_msg );
					}
				continue;
				}


tr( "cmd_mft_send_part_30" );

			if ( stripos( s2, " : Transaction inconnue : ", 0 ) >= 0 )
				{
				// Refaire le transfert
				var_set( hReq, "_transfert_en_attente_passerelle", "0" );
				mftcat_logw( hReq, "ERREUR SUR LA PASSERELLE : ", s2, BR, "==> Recommence le transfert", _ );
				mftcat_upd( hReq, null, "0", "ERREUR PASSERELLE" );
				if ( log_id )  hlogc( hLog, log_id, "", "ERREUR SUR LA PASSERELLE : ", s2, BR, "==> Recommence le transfert", _ );
//				if ( log_id ) hlog_free( hLog );
				continue;
				}



			n = atoi( s2 );
			b = ( strpos( s2, "::", 0 ) >= 0 ); // true=>v7.2 sinon v7.1

			if (
				( ( b )&&( (p=strpos( s2, "::K::", 0 )) >= 0 ) )
				||( ( !b )&&(
					( (p=stripos( s2, "ERREUR", 0 )) >= 0 )
					||( (p=stripos( s2, "ANNULE", 0 )) >= 0 )
					)
				  )
				)
				{
				if ( atoi( var_get( hReq, "errno" ) ) != n )
					{
					if ( s2[p] == ':' ) p += 5;
					mftcat_logw( hReq, "ERREUR SUR LA PASSERELLE : ", s2, _ );
					mftcat_upd( hReq, null, itoa(n), &(s2[p]) );
					if ( log_id )	hlogc( hLog, log_id, "", "ERREUR SUR LA PASSERELLE : ", s2, _ );
					}
//				if ( log_id )	hlog_free( hLog );
				continue;
				}

tr( "cmd_mft_send_part_31" );

			if (
				( ( b )&&(
							( (p=strpos( s2, "::C::", 0 )) >= 0 )
							||( (p=strpos( s2, "::D::", 0 )) >= 0 )
						)
					)
				||( ( !b )&&( (p=stripos( s2, "En cours", 0 )) >= 0 ) )
				)
				{
tr( "cmd_mft_send_part_31a" );
				// Transfert encore en cours
				if ( atoi( var_get( hReq, "errno" ) ) != n )
					{
tr( "cmd_mft_send_part_31a1" );
					if ( log_id )  hlogc( hLog, log_id, "", "ERREUR SUR LA PASSERELLE : ", s2, _ );
					if ( s2[p] == ':' ) p += 5;
					else p += 8;
					mftcat_logw( hReq, "ERREUR SUR LA PASSERELLE : ", s2, _ );
					mftcat_upd( hReq, null, itoa(n), &(s2[p]) );
					}
tr( "cmd_mft_send_part_31a2" );
//				if ( log_id ) hlog_free( hLog );
tr( "cmd_mft_send_part_31a3" );
				continue;
				}

tr( "cmd_mft_send_part_31b" );

			// FIN CORRECTE SUR LA PASSERELLE : RECUP LOG

			mftcat_logw( hReq, "TRANSFERT SUR LA PASSERELLE TERMINE OK", _ );

			strdcpy( &cmd, "mft_server_recv log ", &(tran_id[p3+1]), _ );

tr( "cmd_mft_send_part_31c" );

			if ( tcp_request( s1, cmd, &s2 ) == sERROR )
				{
tr( "cmd_mft_send_part_31d" );
				mftcat_logw( hReq, "ERREUR RECUP LOG DE LA PASSERELLE :" BR BR, error_msg, _ );
				mftcat_upd( hReq, null, null, null );
				if ( log_id )	hlogc( hLog, log_id, "", "ERREUR RECUP LOG DE LA PASSERELLE :" BR, error_msg, _ );
//				if ( log_id )	hlog_free( hLog );
				continue;
				}

tr( "cmd_mft_send_part_31e" );
			mftcat_logw( hReq, "LOG DE LA PASSERELLE :" BR BR, s2, BR, _ );
			mftcat_upd( hReq, null, null, null );

			if ( log_id )
				{
				hlogc( hLog, log_id, "", "LOG DE LA PASSERELLE :" BR BR, s2, _ );
				hlogu_status( hLog, log_id, "OK", _ );
				}

tr( "cmd_mft_send_part_32" );

			strdcpy( &src_fname, var_get( hReq, "fname" ), _ );

			goto fin_send; // FIN CORRECTE DU TRANSFERT
			}



		//----------------------------------------------------------------------
		// TRAITEMENT DU FICHIER A ENVOYER
		//----------------------------------------------------------------------

		debut_send_file:


tr( "cmd_mft_send_part_33" );

		strdcpy( &src_fname, var_get( hReq, "fname" ), _ );
		bAscii		= ( var_get( hReq, "ascii" ) ) ? true : false;
		ll2 = file_size( src_fname );
		var_set( hReq, "file_size", itoa( ll2 ) );

		// VERIFIE QUE LE FICHIER EXISTE

		if ( !file_exists( src_fname ) )
			{
tr( "cmd_mft_send_part_34" );
			strdcpy( &s, "ERREUR : le fichier à envoyer n'existe pas : ", src_fname,
						/*BR "RETENTATIVES TOUTES LES 30 SECONDES.",*/ _ );
			mftcat_logw( hReq, s, _ );
			mftcat_upd( hReq, "K", "110", s );
			if ( log_id ) hlogu_status( hLog, log_id, s, _ );

//			if ( log_id ) hlog_free( hLog );
			continue;
			}


tr( "cmd_mft_send_part_35" );

		// OPTION WAIT_FILE_FREE

		if ( var_get( hReq, "wait_file_free" ) )
			{
tr( "cmd_mft_send_part_36" );
			n = file_is_free( src_fname );
			if ( !n )
				{
				v = var_get( hReq, "errno" );
				if ( ( !v )||( strcmp( v, "135" ) ) )
					{
					strdcpy( &s, "ERREUR : le fichier à envoyer est bloqué par un autre process ", src_fname,
								_ );
					mftcat_logw( hReq, s, _ );
					mftcat_upd( hReq, null, "135", s );

					if ( log_id ) hlogc( hLog, log_id, "", s, _ );
//					logw( "send_part : ", ida, " : ", s, _ );
					}

//				if ( log_id ) hlog_free( hLog );

				continue; // Passe au fichier suivant
				}
			}

		var_set( hReq, "file_size", itoa(file_size( src_fname )) );



		// OUVRE LE FICHIER A ENVOYER
tr( "cmd_mft_send_part_39" );
		fp = NULL;
		if ( src_fname )	fp = fopen( src_fname, "rb" );
		if ( !fp )
			{
			v = var_get( hReq, "errno" );
			if ( ( !v )||( strcmp( v, "104" ) ) )
				{ // ON NE LOG QU'UNE SEULE FOIS LE MEME CODE ERREUR
				strdcpy( &s, "ERREUR : Problème d'accès au fichier", _ );
//				logw( "send_part : ", ida, " : ", s, _ );
				if ( log_id ) hlogu_status( hLog, log_id, s, _ );
				mftcat_logw( hReq, s, _ );
				mftcat_upd( hReq, "K", "104", s );
				}

//			if ( log_id ) hlog_free( hLog );

//			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
//			file_delete( fname ); // Enlève de la file d'attente

			continue;
			}



		//----------------------------------------------------------------------
		// SEND : PREPARE LA REQUETE POUR LE DESTINATAIRE
		//----------------------------------------------------------------------

		prepare_request :

tr( "cmd_mft_send_part_40" );

		// ECRIT LA VERSION DU PROTOCOLE UTILISE

		if ( strcmp( session_version, PROTOCOLE_VERSION ) )
			{
			mftcat_logw( hReq, "PROTOCOL_VERSION : ", PROTOCOLE_VERSION, ", remote version=", session_version, _ );
			if ( log_id ) hlogc( hLog, log_id, "", "PROTOCOL_VERSION : ", PROTOCOLE_VERSION, ", remote version=", session_version, _ );
			}


		if ( atoi( var_get( hReq, "_transfert_en_cours" ) ) != 1 )
			{
			// NOUVEAU TRANSFERT
			logw("NOUVEAU TRANSFERT: new trans_id",var_get( hReq, "_transfert_en_cours" ),"/RIDA(vide)=", var_get( hReq, "tran_id" ) ,_);
			if ( var_get( hReq, "tran_id" )) ssie7exit(0);// eviter les doubles sni

			if ( ( bGateway )&&( var_get( hReq, "tran_id" ) ) )
				{
				if ( ( v=var_get( hReq, "test_crash" ) )
					&&( !strcmp( v, "1" ) ) )
					var_delete( hReq, "test_crash" ); // Pour que le transfert aille jusqu'au bout
				}

			strdcpy( &cmd, "mft_server_recv fname", _ );
			for( i = 0 ; v = var_getn( hReq, i, &vn ) ; i++ )
				{
				if ( vn[0] == '_' ) continue;
				if ( strmicmp( vn,
					"mft|send|direct|ascii|binary|fname|src_fname|initial_fname|faction|nocopy|copyfile"
					"|waitend|timeout|timeout_exec|alert_timeout|alert_exec|alert_max_nb"
					"|dt_crea|date_crea|dt_upd|day_upd|ssie_mandatory"
					"|errno|errmsg|status|_elib_bd_index|mft_log|_elib_bd_vlf_mft_log"
					"|ida|rida|tran_id|wait_file_free"
					)
					) continue;
				strdcat( &cmd, " ", vn, "=", quote( v ), _ );
				}

			strd_ireplace( &cmd, "[$ida]", ida );
			strd_ireplace( &cmd, "[$mypart]", var_get( hSrvConf, "partname" ) );

			if ( bAscii )
				{
				#ifdef OS_WINDOWS
				strdcat( &cmd, " windows", _ );
				#else
				strdcat( &cmd, " unix", _ );
				#endif
				}
			}
		else
			{
			// RETENTATIVE DE TRANSFERT
			// RECUPERE L'IP DU DESTINATAIRE (IP CAR IL PEUT Y AVOIR UN ALTEON)
			// 		ET LE N° DE TRANSACTION (IDA de Réception)

			v = var_get( hReq, "tran_id" );
			p = strpos( v, "#", 0 );
			strdncpy( &session_ipport, v, p, _ );
			//match entre ip et lip du  part
			mftcat_logw( hReq, "reprise du transfert", _ );
			if ( log_id )
				{
				hlogc( hLog, log_id, "", "Reprise du transfert", _ );
				}

			v = var_get( hReq, "tran_id" );
			p = strpos( v, "#", 0 );
			strdcpy( &cmd, "mft_server_recv tran_id=", substr( v, p+1, -1 ),
					" file_size=", var_get( hReq, "file_size" ), _ );
			}

//		mftcat_logw( hReq, cmd, _ );


		//----------------------------------------------------------------------
		// SEND EXECUTE : SOUMET LA REQUETE
		//----------------------------------------------------------------------

		send_request:

tr( "cmd_mft_send_part_41" );
		if ( tcp_ssend( hCnx, cmd, -1 ) == ERROR )
			{
			v = var_get( hReq, "diagi" );
			if ( ( !v )||( strcmp( v, "302" ) ) )
				{
				strdcpy( &s, "ERREUR : Connexion perdue (1)", _ );
				if ( log_id )
					hlogc( hLog, log_id, "", s, _ );
				mftcat_logw( hReq, s, _ );
				mftcat_upd( hReq, null, "302", s );
				}

//			if ( log_id ) hlog_free( hLog );
			break;
			}


		//----------------------------------------------------------------------
		// SEND EXECUTE : ATTEND ACCEPTATION OU REFUS
		//----------------------------------------------------------------------

		get_resp:
tr( "cmd_mft_send_part_42" );

		if ( tcp_srecv( hCnx, &s1 ) == ERROR )
			{
			v = var_get( hReq, "diagi" );
			if ( ( !v )||( strcmp( v, "302" ) ) )
				{
				strdcpy( &s, "ERREUR : Connexion perdue (2)", _ );
				if ( log_id )
					hlogc( hLog, log_id, "", s, _ );
				mftcat_logw( hReq, s, _ );
				mftcat_upd( hReq, null, "302", s );
				}

//			if ( log_id ) hlog_free( hLog );
			break;
			}
		logw("mft send_part : (reponse du recep avec le rida) ATTEND ACCEPTATION OU REFUS=",s1,_);
		if (strncmp( s1, "OK:", 3 ))
		{
		// traitement du doublon avec flag 
		strdcpy( &fname, dirtmp, DIR_CHAR "send_",ida,".ida", _ );//fichier loop_flag	
		//ssie7exit(0);// pour tester
		// on verifie le doublon 
		if (file_exists(fname) )
                {
                	file_delete(fname);
			logw("mft send_part : cas double reception avec ida=",ida,"/msg=",s1,_);
                        ssie7exit(0);
                }
		// 1er message recu par le recep pour cet ida
		file_save_str( fname, "", 0 );
		}

		tr( "cmd_mft_send_part_43" );
		if ( strncmp( s1, "OK:", 3 ) )
			{
			if ( s1[0] == 0 )		goto get_resp;

			// REFUS DU DISTANT
			mftcat_logw( hReq, "Réponse du distant : <<", s1, ">>", _ );

			if ( !strncmp( cmd, "mft_server_recv fname=", -1 ) )
				{ // Compatibilité avec anciennes versions : 7.1.a à d
				strd_replace( &cmd, "mft_server_recv ", "send_server " );
				goto send_request;
				}

			if ( !strncmp( cmd, "mft_server_recv tran_id=", -1 ) )
				{ // Compatibilité avec anciennes versions
				strd_replace( &cmd, "mft_server_recv ", "send_server " );
				v = var_get( hReq, "src_fname" );
				strdcat( &cmd, " fname=", quote( file_getname( v ) ), _ );
				goto send_request;
				}


			// PAS DE RETENTATIVES DANS CE CAS

			mftcat_upd( hReq, "K", "730", s1 );
			if ( log_id ) hlogu_status( hLog, log_id, s1, _ );
//			if ( log_id ) hlog_free( hLog );
			//On recupere l'ida pour lancer un mft retry
			sleep(5);//on laisse le temps pour un retry
			strdcpy( &cmd, argv[0], " mft retry ida=", ida, _ );
			strdcpy( &fname, dirtmp, DIR_CHAR "retry_",ida, _ );//fichier retry
			
			//logw("mft send_part: cmd pour relancer le flux:",cmd,_);

			if (file_exists(fname)) 
			{
			file_delete(fname);
			continue;
			} 

			file_save_str( fname, "", 0 );

			logw("mft send_part: transfert relance:",cmd," fn=",fname,_);

                        system( cmd );
			goto debut_send_part;
			}


		//----------------------------------------------------------------------
		// TRANSFERT ACCEPTE
		//----------------------------------------------------------------------

tr( "cmd_mft_send_part_44" );

		if ( atoi( var_get( hReq, "_transfert_en_cours" ) ) != 1 )
			{			
			strdcpy( &s, "Transfert accepté : ", s1, _ );
			mftcat_logw( hReq, s, _ );
			if ( log_id ) hlogc( hLog, log_id, "", s, _ );
			if ( ! bGateway )
				// En mode gateway, on recommence les transferts entièrement
				var_set( hReq, "_transfert_en_cours", "1" );

			var_set( hReq, "tran_id", tran_id = substr( s1, 3, -1 ) );
			//logw("sni: Transfert accepte avec tran_id=",tran_id,"/ida=",var_get( hReq,"ida"),_);
			ll = 0; // Taille déjà transféré
			}
		else
			{
			strdcpy( &s, "Reprise acceptée : ", s1, _ );
			mftcat_logw( hReq, s, _ );
			if ( log_id ) hlogc( hLog, log_id, "", s, _ );


			// LECTURE DE LA POSITION DE DEPART POUR LA REPRISE

			ll = atoll( &(s1[3]) ); // Taille déjà transféré
			fseek_top( fp, ll );
			}



		//------------------------------------------------------------------
		// SEND : ENVOI LES DONNEES
		//------------------------------------------------------------------

		// VERIFIE QUE LE TRANSFERT N'A PAS ETE ANNULE ENTRE TEMPS
tr( "cmd_mft_send_part_45" );

		var_free( h );
		if ( (h = mftcat_load( ida )) != ERROR )
			{
			v = var_get( h, "status" );
			if (( v )&&( v[0] != 'C' )&&( v[0] != 'H' )) 
				// Le transfert a été annulé => FIN
				ssie7exit(0);
			}

		mftcat_logw( hReq, "Début du transfert du fichier ", fname_verify(src_fname),
				" (size=", var_get( hReq, "file_size" ), ")", _ );
		if ( log_id ) hlogc( hLog, log_id, "", "Début du transfert du fichier ", fname_verify(src_fname),
				" (size=", var_get( hReq, "file_size" ), ")", _ );

		if ( mftcat_upd( hReq, null, "0", s ) == ERROR )
			exit(0); // Pb de mémoire

		s = realloc( s, 128 Ko );

		bNocrypt = ( var_get( hReq, "nocrypt" ) ) ? true : false;

		ll2 = file_size( src_fname );
		n4 = n5 = time(NULL);

		while( n = fread( fp, s, 96 Ko ) )
			{
			if ( n5 != time(NULL) ) { Sleep(1); n5 = time(NULL); }

			#ifdef OS_WINDOWS
			if ( ( bAscii )&&( s[n] == '\r' ) )
				{
				// VERIFIE QUE \r\n sont bien réunis à la fin du bloc
				// CHARGE LE \n qui manque peut-être
				if ( fread( fp, &(s[n+1]), 1 ) )
					n++;
				}
			#endif
			//logw("sni: un autre tcp_send ici(debut du transfert)=",s,"FINFIN",_);

			if ( bNocrypt ) r = tcp_ssend0( hCnx, s, n ); else r = tcp_ssend( hCnx, s, n );

			if ( r == ERROR )
				{
				strdcpy( &s, "ERREUR : Connexion perdue (3)", _ );
				if ( log_id ) hlogc( hLog, log_id, "", s, _ );
				mftcat_logw( hReq, s, _ );
				mftcat_upd( hReq, null, "302", s );
				break;
				}

			ll += n;
			if ( ( ll2 )&&( ( time(NULL) - n4 ) > 5 ) )
				{
				var_free( h );
				if ( (h = mftcat_load( ida )) != ERROR )
					{
					v = var_get( h, "status" );
					if (( v )&&( v[0] != 'C' )&&( v[0] != 'H' ))
						// Le transfert a été annulé => FIN
						ssie7exit(0);
					}

				// On ne log que toutes les 5 secondes pour ne pas surcharger la machine
				mftcat_upd( hReq, null, "0", strdcpy( &s, itoa( ((ll * 100)/ll2) ), "%", _ ) );
				n4 = time(NULL);
				if ( !bRecv )
					{
					strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_send_part_", part, ".pid", _ );
					file_save_str( fname, itoa(getpid()), -1 );
					}
				}
			} // FIN while( fread )


		fclose( fp );

		if ( r == ERROR )
			{
//			if ( log_id ) hlog_free( hLog );
			break;
			}

		//----------------------------------------------------------------------
		// SEND : TRANSFERT DES DONNEES TERMINE
		//----------------------------------------------------------------------

tr( "cmd_mft_send_part_46" );
		// VERIFIE QUE LE TRANSFERT N'A PAS ETE ANNULE

		var_free( h );
		if ( (h = mftcat_load( ida )) != ERROR )
			{
			v = var_get( h, "status" );
			if (( v )&&( v[0] != 'C' )&&( v[0] != 'H' ))
				// Le transfert a été annulé => FIN
				ssie7exit(0);
			}


		// ENVOI LA TRAME DE FIN

		if ( !strcmp( session_version, "100" ) )
			strdcpy( &s3, "FIN ", file_getname( src_fname ), _ );
		else
			strdcpy( &s3, "FIN ", ida, _ );
		//logw("sni: tcp_ssend FIN=>",ida,"+",s3,"***-",_);
		if ( bNocrypt ) tcp_ssend0( hCnx, s3, -1 ); else tcp_ssend( hCnx, s3, -1 );

		// ATTEND LA REPONSE

		if ( tcp_srecv( hCnx, &s1 ) == ERROR )
			{
			strdcpy( &s, "ERREUR : Connexion perdue (4)", _ );
			mftcat_logw( hReq, s, _ );
			mftcat_upd( hReq, null, "302", s );
			if ( log_id ) hlogc( hLog, log_id, "", s, _ );
//			if ( log_id ) hlog_free( hLog );
			break;
			}


		if ( stripos( s1, "OK", 0 ) < 0 )
			{
			// PROBLEME CHEZ LE RECEPTEUR
			mftcat_logw( hReq, "RECEPTEUR : ", s1, _ );
			if ( log_id ) hlogc( hLog, log_id, "", "RECEPTEUR : ", s1, _ );

			mftcat_upd( hReq, null, "900", s1 );
//			if ( log_id ) hlog_free( hLog );
			continue;
			}

tr( "cmd_mft_send_part_47" );

		// TRANSFERT OK

		var_set( hReq, "errmsg", "100% ok" );
		var_set( hReq, "tran_id", tran_id = substr( s1, 3, -1 ) );
		p = strpos( tran_id, "#", 0 );
		var_set( hReq, "rida", substr( tran_id, p+1, 8 ) );
		var_set( hReq, "_transfert_en_cours", "0" );


		//----------------------------------------------------------------------
		// SEND EXECUTE : FIN D'UN TRANSFERT SANS PASSERELLE
		//----------------------------------------------------------------------

		if ( ! bGateway )
			{
			fin_send: // On revient ici lorsque le transfert via une passerelle se termine correctement.

tr( "cmd_mft_send_part_50" );


			// SEND EXECUTE : MISE A JOUR DU STATUS

			v = currdate( "JJ/MM/AAAA HH:MM:SS" );
			var_set( hReq, "datee", substr( v, 0, 10 ) );
			var_set( hReq, "timee", substr( v, 11, 8 ) );

tr( "cmd_mft_send_part_51" );

			if ( ! bGateway )
				{
				mftcat_logw( hReq, "Transfert des données OK", _ );
				if ( log_id )
					{
//bDebug = true;
					hlogc( hLog, log_id, "", "Transfert des données OK", _ );
//logw( "mft_send_part ", part, " : OK", _ );
					hlogu_status( hLog, log_id, "OK", "", _, _, _ );
//bDebug = false;
					}
				}

tr( "cmd_mft_send_part_52" );

//			v1 = (char*)((log_id)?"T":"W");
//			if ( log_id ) v1 = "T"; else v1 = "W";
			v1 = "T";

tr( "cmd_mft_send_part_52a" );

			// SEND EXECUTE : SUPPRIME LE FICHIER SOURCE SI DEMANDE

			v = var_get( hReq, "faction" );



tr( "cmd_mft_send_part_52b" );

			if (( ( v )&&( !stricmp( v, "delete" ) ) )
				||( ( var_get( hReq, "initial_fname" ) )&&( ! var_get( hReq, "pack_fname" ) ) ))
				{
tr( "cmd_mft_send_part_52c" );
				file_delete( src_fname );
				for( n = 100 ; ( n )&&( file_exists( src_fname ) ) ; n--, Sleep(10) )
					file_delete( src_fname );
tr( "cmd_mft_send_part_52c1" );
				if ( file_exists( src_fname ) )
					{
tr( "cmd_mft_send_part_52d" );
					if ( log_id ) hlogc( hLog, log_id, "ERREUR : Pb suppression fichier : ", src_fname, _ );
tr( "cmd_mft_send_part_52e" );

					mftcat_logw( hReq, "ERREUR : Pb suppression fichier : ", src_fname, _ );
					mftcat_upd( hReq, v1, "0", "ALERTE" );
					}
				else
					{
tr( "cmd_mft_send_part_52f" );
					if ( log_id ) hlogc( hLog, log_id, "", "Suppression du fichier : ", src_fname, _ );
					mftcat_logw( hReq, "Suppression du fichier : ", src_fname, _ );
//					mftcat_upd( hReq, v1, "0", "OK" );
					if ( mftcat_upd( hReq, v1, "0", "OK" ) == ERROR )
						exit(0); // Pb de mémoire
					}
tr( "cmd_mft_send_part_52g" );
				}
			else
//				mftcat_upd( hReq, v1, "0", "OK" );

			if ( mftcat_upd( hReq, v1, "0", "OK" ) == ERROR )
				exit(0); // Pb de mémoire

tr( "cmd_mft_send_part_53" );

			continue;
			}


		//----------------------------------------------------------------------
		// SEND PART : FIN D'UN TRANSFERT AVEC PASSERELLE
		//----------------------------------------------------------------------

		var_set( hReq, "_transfert_en_attente_passerelle", "1" );

		strdcpy( &s, "Transfert sur la passerelle OK", _ );
		if ( log_id ) hlogc( hLog, log_id, "", s, _ );
		mftcat_logw( hReq, s, _ );
		mftcat_upd( hReq, null, "0", "En cours sur la passerelle MFT" );
		}

tr( "cmd_mft_send_part_54" );
//	if ( _hLock ) file_unlock( _hLock );

tr( "cmd_mft_send_part_55" );
	if ( log_id ) hlog_free( hLog );

tr( "cmd_mft_send_part_56" );
	if ( bRecv )
		{
		print( "NO MORE FILE TO SEND", _ );
		return OK;
		}

tr( "cmd_mft_send_part_57" );
	sleep(5);
tr( "cmd_mft_send_part_58" );
	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_send_part_", part, ".pid", _ );
	file_delete( fname );
tr( "cmd_mft_send_part_59" );
	
	//sni: suppression du fichier loop_flag
	strdcpy( &fname, dirtmp, DIR_CHAR "send_",ida,".ida", _ );//fichier loop_flag 
	file_delete( fname );
	
//	exit(0);
	//logw("sni: fin de transfert (send)end,ida=",ida,"/rida=",var_get( hReq, "tran_id" ),_);
	return OK;
	}




#undef bAscii
#undef bNocrypt
#undef bPublish
#undef bGateway
#undef cat_fname
#undef log_fname
#undef tran_id
#undef src_fname

























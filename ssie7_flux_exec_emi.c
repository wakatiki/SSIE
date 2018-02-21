#include "ssie7.h"

extern char *flux_etape;
extern int	etape_no, etape_log_id;

#define send_fname			fname2
#define cat_fname			fname3
#define log_fname			fname4
#define sudo_ini_dir		u14


//------------------------------------------------------------------------------
int emi_execute( T_WORK* t_work )

// EXECUTE UNE ETAPE DE FLUX DE TYPE EMI (Emission de fichier)
//------------------------------------------------------------------------------
	{
	char *adle = null, *appe = null, *adlr = null, *appr = null, *parm = null, *mft_opt = null, **_ts;
	long int _n, _n1, _n2, _i, _b, _id;
	char bPublish = false, bNocopy = false;

	tr("emi_execute_1");
/*
	v = var_get( hFlux, "flux_do_restart" );
	if ( !v )
		var_set( hFlux, "flux_do_restart", "false" );
*/

	// SI LE FLUX ETAIT EN ATTENTE DE FIN => ON Y RETOURNE
		// CAS D'UN ARRET DU SERVEUR AVANT LA FIN DE TOUTS LES TRANSFERTS OU PLANTAGE DU FLUX
	strdcpy( &s, "send_", flux_etape, "_wait_end", _ );
	if ( var_get( hFlux, s ) ) goto send_wait_end;

	tr("emi_execute_2");

	//--------------------------------------------------------------------------
	// PREPARATION DES TRANSFERTS
	//--------------------------------------------------------------------------

	if ( ( ! ( v = var_get( hEtape, "age_dest_liste" ) ) )||( ! (v[0]) ) )
		{
		tr("emi_execute_3");
		hlogu_status( hLog, etape_log_id, "ERREUR : aucun destinataire n'a été défini", _ );
		on_error_execute( t_work );
		return;
		}

	strdcpy( &s, v, _ );
	var_list_replace( hFlux, &s );

	ts = str_explode( s, "," );
	var_set( hFlux, "send_to_surv", "" );

	// RECHERCHE UNE ETAPE WAITACK
	for( i = atoi(flux_etape)+1 ; i < 10000 ; i++ )
		{
		tr("emi_execute_4");
		strdcpy( &fname, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(i), ".dat", _ );
		if ( ! file_exists( fname ) ) break;
		tr("emi_execute_5");
		var_free( h );
		h = var_load( fname );
		v = var_get( h, "etape_type" );
		if ( strcmp( v, "waitack" ) ) continue;
		tr("emi_execute_6");

/*
		n = atoi(var_get( h, "etape_timeout" )) * 60;
		if ( n )
			n += time(NULL);
		var_set( hFlux, "waitack_timeout", itoa(n) );
*/

		var_set( hFlux, "waitack_asked", "true" );

		hlogc( hLog, etape_log_id, "", "Accusés Réceptions demandés", _ );
		break;
		}




	//--------------------------------------------------------------------------
	// VERIFIE LA SYNTAXE DES DESTINATAIRES
	//--------------------------------------------------------------------------
	tr("emi_execute_7");

	for( i = 0 ; partname = ts[i] ; i++ )
		{
		tr("emi_execute_8");
		if ( ( p1 = strpos( partname, "(", 0 ) ) >= 0 )
			{
			tr("emi_execute_9");
			p2 = strpos( partname, ")", p1 );
			if ( p2 < 0 )
				{
				tr("emi_execute_10");
				hlogu_status( hLog, id, "ERREUR : Syntaxe du destinataire non valide [NomPart ou ADL](Code APPR)", _ );
				continue;
				}
			}
		}


	strdcpy( &adle, trim(var_get( hEtape, "age_adle" )), _ );
	if ( ( !adle )||( ! (adle[0]) ) )
		strdcpy( &adle, var_get( hSrvConf, "partname" ), _ );

	strdcpy( &appe, trim(var_get( hEtape, "age_appe" )), _ );
	strdcpy( &mft_opt, var_get( hEtape, "age_mft_options" ), _ );
	var_free( h );
	h = var_from_cmd( 0, mft_opt );
	if ( ( v=var_get( h, "state" ) )&&( !stricmp( v, "hold" ) ) )
		bPublish = true;
	if ( var_get( h, "nocopy" ) )
		{
		bNocopy = true;
		strd_ireplace( &mft_opt, "nocopy", "" );
		}


	//--------------------------------------------------------------------------
	// BOUCLE D'ENVOI PARTENAIRE PAR PARTENAIRE
	//--------------------------------------------------------------------------

	tr("emi_execute_11");

	for( k = b = i = 0, ll = 0 ; ts[i] ; i++ )
		{
		tr("emi_execute_12");

		//----------------------------------------------------------------------
		// LECTURE DU NOM DE PARTENAIRE
		//----------------------------------------------------------------------
		partname = ts[i];
		if ( ( p1 = strpos( partname, "(", 0 ) ) >= 0 )
			{
			tr("emi_execute_13");
			p2 = strpos( partname, ")", p1 );
			if ( p2 < 0 )
				{
				tr("emi_execute_14");
				id = hlogc( hLog, etape_log_id, "ERREUR", "Emission du fichier '",
					fname, "' (", itoa(file_size(fname)), ")",
					" vers le partenaire ", partname, _ );
				hlogu_status( hLog, id, "ERREUR : Syntaxe du destinataire non valide [NomPart ou ADL](Code APPR)", _ );
				b = true;
				continue;
				}
			strdcpy( &appr, upper(trim(substr( partname, p1+1, p2-p1-1 ))), _ );
			partname = trim(upper(substr( partname, 0, p1 )));
			}
		else
			{
			upper(trim( partname ));
			strdcpy( &appr, "", _ );
			}


		//----------------------------------------------------------------------
		// VERIFIE LA VALIDITE DU PARTENAIRE
		//----------------------------------------------------------------------

/*
		strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, partname, ".dat", _ );
		if ( !file_exists( fname ) )
			{
			tr("emi_execute_14");
			strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR "adl" DIR_CHAR, partname, ".dat", _ );
			if ( file_exists( fname ) )
				{
				tr("emi_execute_15");
				strdcpy( &adlr, partname, _ );
				partname = null;
				if ( !file_load( &partname, fname ) )
					{
					tr("emi_execute_16");
					strdcpy( &s, "ERREUR : Le partenaire ", adlr, " n'existe pas", _ );
					hlogc( hLog, etape_log_id, s, "", _ );
					hlog_raise_status( hLog, etape_log_id, s, _ );
					on_error_execute( t_work );
					return true;
					}
				}
			else
				{
				strdcpy( &s, "ERREUR : Le partenaire ", partname, " n'existe pas", _ );
				hlogc( hLog, etape_log_id, s, "", _ );
				hlog_raise_status( hLog, etape_log_id, s, _ );
				on_error_execute( t_work );
				return true;
				}
			}
		else
			strdcpy( &adlr, partname, _ );
*/

		strdcpy( &adlr, partname, _ );
		if ( ! part_exists( t_work ) ) // retourne dans fname le nom du fichier partenaire
			{
			#ifdef OS_LINUX
			strdcpy( &s, "/appli/ssie7annu/data/part/", partname, ".dat", _ );
			if ( file_exists( s ) )
				{
				strdcpy( &s1, "cp ", s, " /appli/ssie7/conf/part", _ );
				system( s1 );
				if ( part_exists( t_work ) ) goto send_suite;
				}
			#endif

			Sleep(10);
			if ( part_exists( t_work ) ) goto send_suite;

			Sleep(10);
			if ( part_exists( t_work ) ) goto send_suite;

			Sleep(10);
			if ( part_exists( t_work ) ) goto send_suite;

			// LE PARTENAIRE N'EXISTE PAS
			strdcpy( &s, "ERREUR : Le partenaire ", partname, " n'existe pas", _ );
			hlogc( hLog, etape_log_id, s, "", _ );
			hlog_raise_status( hLog, etape_log_id, s, _ );
			on_error_execute( t_work );
			return true;
			}

		send_suite:

		strdcpy( &part, partname, _ );


		//----------------------------------------------------------------------
		// BOUCLE D'ENVOI PAR TYPE DE FICHIER
		//----------------------------------------------------------------------

		for( e = 0 ; e < 2 ; e++ )
			{
			tr("emi_execute_17");

			v = trim((char *)( ( e == 0 )
					? var_get( hEtape, "age_bin_files" )
					: var_get( hEtape, "age_txt_files" ) ) );
			if (( v == NULL )||( trim(v)[0] == 0 ))
				{
				tr("emi_execute_18");
				if ( !e ) continue; else break;
				}

			strdcpy( &s1, v, _ );
			var_list_replace( hFlux, &s1 );
			if ( trim(s1)[0] == 0 ) continue;

			tr("emi_execute_19");
			_b = false;

			#ifdef OS_UNIX

			//------------------------------------------------------------------
			// SI SUDO ON COPIE LES FICHIERS SOUS LE COMPTE SSIE
			//------------------------------------------------------------------

			sudo_ini_dir = null;
			v2 = trim( var_get( hFlux, "app_login" ) );
			if (( v2 )&&( (p=strpos( v2, ":", 0 )) >= 0 ))
				v2 = strdncpy( &u6, v2, p, _ );

			if (( !bNocopy )&&( v2 )&&( v2[0] )&&( strcmp( v2, ssie_user ) ))
				{
				tr("emi_execute_20");

				strdcpy( &s2, dirflux, DIR_CHAR, flux_id, "_emi_sudo", _ );
				//Ticket#2017050210001392: [Ginko RC] Envoi de fichier GSE en doublon
				if ( dir_exists(s2))
				{
				logw("emi_execute: ALERTE : Fichier en double: ",s2,_);
				k++;//envite le cas: fichier introuvable lors des doublons
				strdcpy( &fname, dirtmp, DIR_CHAR "double_", flux_id, _ );
				file_save_str( fname, "", 0 );//flag du fichier en double
				break;
				                                                                                                                               }
				mkdir( s2 );

				strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
						"cp -f ", _ );

				strdcpy( &sudo_ini_dir, file_getdir( s1 ), _ );
				if ( strpos( s1, "*", 0 ) >= 0 )
					strdcat( &s, s1, " ", s2, _ );
				else
					strdcat( &s, "\"", s1, "\" ", s2, _ );

				System( &s3, s );
				id = hlogc( hLog, etape_log_id, "", s, BR, s3, _ );
				v = s2; s2 = s1; s1 = v;

				strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
						"chown -R ", ssie_user, ":", ssie_user, " ", s1, _ );

				System( &s3, s );

				_b = true;
				}

			#endif

			tr("emi_execute_21");

			// VERIFIE L'EXISTANCE DU FICHIER OU DU REPERTOIRE
			if ( strpos( s1, "*", 0 ) < 0 )
				{
				if ( strpos( s1, ",", 0 ) >= 0 )
					{
					ts2 = str_explode( s1, "," );
					for( i = 0 ; ts2[i] ; i++ )
						{
						if ( !i )
							strdcpy( &s, ts2[0], _ );
						else
							strdcpy( &s, file_getdir( ts2[0] ), DIR_CHAR, ts2[i], _ );
						if ( ! file_exists( s ) )
							{

                           //Elargir la liste des variables dans le cas ou le fichier n'existe pas
                             VLI = var_get( hFlux, "var_list");
                             strdcat( &VLI, "FNAME",_);
                             var_set( hFlux, "var_list", VLI);
                             var_set( hFlux, "_FNAME",s);
							id = hlogc( hLog, etape_log_id, "ERREUR : Fichier inexistant",
									"Emission du fichier '",s, "' vers le partenaire ", part, _ );
							goto send_fin;
/*
							v = var_get( hFlux, "send_to_surv" );
							strdcpy( &s1, v, "|", itoa(id), ",null", _ );
							var_set( hFlux, "send_to_surv", s1 );
							k++;
							continue;
*/
							}
						}
					free( ts2 );
					}
				else
					{
					tr("emi_execute_22");
					if (( !dir_exists( s1 ) )&&( !file_exists( s1 ) ))
						{
						tr("emi_execute_23");

                        //Elargir la liste des variables dans le cas ou le repertoire n'existe pas
						VLI = var_get( hFlux, "var_list");
                        strdcat( &VLI, "FNAME",_ );
                        var_set( hFlux, "var_list", VLI);
                        var_set( hFlux, "_FNAME",s1);
												;
						id = hlogc( hLog, etape_log_id, "ERREUR : Fichier inexistant",
								"Emission du fichier '",s1, "' vers le partenaire ", part, _ );
/*
						v = var_get( hFlux, "send_to_surv" );
						strdcpy( &s1, v, "|", itoa(id), ",null", _ );
						var_set( hFlux, "send_to_surv", s1 );
						k++;
						continue;
*/
						goto send_fin;
						}
					}
				}

			tf = NULL;
			while( ( tf = file_list( tf, s1, "nodir adir nohide" ) ) != NULL )
				{
				tr("emi_execute_24");
				strdcpy( &send_fname, tf->dir, DIR_CHAR, tf->name, _ );

				k++;
				ll += file_size( send_fname );

				//--------------------------------------------------------------
				// VERIFIE SI LE FICHIER N'A PAS DEJA ETE TRAITE
				//--------------------------------------------------------------

				strdcpy( &s, "send_", flux_etape, "_", itoa(i), "_", itoa(e), "_", send_fname, _ );
				strd_replace( &s, ":", "_" );
				if ( var_get( hFlux, s ) ) continue;
				tr("emi_execute_25");

				id = hlogc( hLog, etape_log_id, "En cours", "Emission du fichier '",
					send_fname, "' (", itoa(file_size(send_fname)), ")",
					" vers le partenaire ", part, _ );

				if ( !id ) return false;

//logw( "emi_execute:flux_id=", flux_id, ", hlogc_id=", itoa(id), _ );
tr2( "hlogc_id=", itoa(id) );

				strdcpy( &parm, var_get( hEtape, "age_appl_info" ), _ );

				v2 = var_get( hFlux, "var_list" );
				if ( !v2 )
					{
					var_set( hFlux, "var_list", ",FNAME," );
					}
		        else
		            {
					tr("emi_execute_36");
					if ( stripos( v2, ",FNAME", 0 ) < 0 )
		               	var_set( hFlux, "var_list", strdcpy( &s1, ",FNAME", v2, _ ) );
		            else
		               	{
						tr("emi_execute_37");
						if ( stripos( v2, ",FNAME,", 0 ) < 0 )
			               	{
							tr("emi_execute_38");
							var_set( hFlux, "var_list", strdcpy( &s1, v2, "FNAME", _ ) );
							}
						}
		            }
				//--------------------------------------------------------------
				// SCRIPT AVANT EMISSION
				//--------------------------------------------------------------

				v = trim(var_get( hEtape, "age_before_script" ));
				if ( ( v )&&( v[0] ) )
					{
					tr("emi_execute_35");
//					strdcpy( &s, "emi_", v1, "_fname", _ );
//					v3 = var_get( hFlux, s );
//					var_set( hFlux, "_FNAME", v3 );
					var_set( hFlux, "_FNAME", send_fname );

					var_set( hFlux, "_PARTNAME", part );
					v2 = var_get( hFlux, "var_list" );
					if ( stripos( v2, ",PARTNAME", 0 ) < 0 )
		               	var_set( hFlux, "var_list", strdcpy( &s1, ",PARTNAME", v2, _ ) );
		            else
		               	{
						tr("emi_execute_39");
						if ( stripos( v2, ",PARTNAME,", 0 ) < 0 )
			               	var_set( hFlux, "var_list", strdcpy( &s1, v2, "PARTNAME", _ ) );
						}

					var_set( hFlux, "_PARM", parm );
					v2 = var_get( hFlux, "var_list" );
					if ( stripos( v2, ",PARM", 0 ) < 0 )
		               	var_set( hFlux, "var_list", strdcpy( &s1, ",PARM", v2, _ ) );
		            else
		               	{
						tr("emi_execute_39b");
						if ( stripos( v2, ",PARM,", 0 ) < 0 )
			               	var_set( hFlux, "var_list", strdcpy( &s1, v2, "PARM", _ ) );
						}

					var_list_prepare( hFlux );

					tr("emi_execute_40");

					_id = id;
					id = hlogc( hLog, id, "En cours", "Exécution du script d'avant émission", _ );
					_i = i; _n = n; _n1 = n1; _n2 = n2; _ts = ts; ts = null;
					error_no = 0;
					ssie7_script_exec( t_work, false );
					i = _i; n = _n; n1 = _n1; n2 = _n2; if ( ts ) free( ts ); ts = _ts;
					id = _id;
//					_b = true;
					if ( error_no ) return true;

					strdcpy( &send_fname, var_get( hFlux, "_FNAME" ), _ );
					strdcpy( &parm, var_get( hFlux, "_PARM" ), _ );
					}


				//--------------------------------------------------------------
				// PREPARE LA REQUETE SEND
				//--------------------------------------------------------------

				strdcpy( &cmd, "mft send part=", part,
					" fname=", quote(send_fname), /*" waitend=60"*/
					" src_fname=", quote( v = send_fname ),
					(char*)(( e ) ? " ascii":""),
					" idf=", (char*)((e==0)?"R2MMSB":"R2MMS"),
					" parm=", quote( parm ),
					" sappl='", adle, appe, "'"
					" suser='[$ida]", adle, "'"
					" rappl='", adlr, appr, "'"
					" ruser='", ito36( atoi(flux_id), 5 ), ito36( id, 3 ), "001'"
					" ", mft_opt,
					(char*)((bPublish)?" publish":""),
					(char*)((var_get( hFlux, "waitack_timeout" ))?
						" waitack" : ""),
					(char*)((_b)? " faction=delete":""),
					" wait_file_free",
					_ );


				var_set( hFlux, "_FNAME", file_getname(v) );
				var_list_replace( hFlux, &cmd );

				hlogc( hLog, id, "", cmd, _ );

				if ( hReq ) var_free( hReq );
				hReq = var_from_cmd( 0, cmd );


				//--------------------------------------------------------------
				// REALISE LE TRANSFERT
				//--------------------------------------------------------------

				log_id = id;
tr2( "log_id=", itoa(log_id) );

				_i = i;
				cmd_mft_send( t_work );
				i = _i;

				// ida contient l'identifiant unique du transfert

//logw( "hlogu : log_id=", itoa(log_id), _ );

				hlogu( hLog, id=log_id, "En cours", ida, " : Emission du fichier '",
					send_fname, "' (", itoa(file_size(send_fname)), ")",
					" vers le partenaire ", part, _ );


				//--------------------------------------------------------------
				// AJOUTE LE TRANSFERT DANS LA LISTE DES TRANSFERTS TRAITES ET ACK A ATTENDRE
				//--------------------------------------------------------------


				strdcpy( &s, "send_", flux_etape, "_", itoa(i), "_", itoa(e), "_", send_fname, _ );
				strd_replace( &s, ":", "_" );
				var_set( hFlux, s, "true" );

				strdcpy( &s, "emi_", v1 = itoa(id), "_fname", _ );
				if ( ! sudo_ini_dir )
					var_set( hFlux, s, send_fname );
				else
					{
					tr("emi_execute_26");
					strdcpy( &s1, sudo_ini_dir, DIR_CHAR, file_getname( send_fname ), _ );
					var_set( hFlux, s, s1 );
					}

				strdcpy( &s, "emi_", v1, "_partname", _ );
				var_set( hFlux, s, part );

				strdcpy( &s, "emi_", v1, "_ida", _ );
				var_set( hFlux, s, ida );

				v = var_get( hFlux, "send_to_surv" );
				strdcpy( &s1, v, "|", v1, "|", ida, _ );
				var_set( hFlux, "send_to_surv", s1 );

				v = var_get( hFlux, "waitack_list" );
//				strdcpy( &s, v, "|", ida, ",", v1, _ );
				strdcpy( &s, v, "|", ito36(id,3), _ );
				var_set( hFlux, "waitack_list", s );

				// INFO POUR LES STATISTIQUES

				v = var_get( hFlux , "part_list" );
				if ( !v ) var_set( hFlux, "part_list",
							 strdcpy( &s, ",", var_get( hReq, "part" ), ",", _ ) );
				else
					{
					strdcpy( &s, ",", var_get( hReq, "part" ), ",", _ );
					if ( strpos( v, s, 0 ) < 0 )
						var_set( hFlux, "part_list", strdcpy( &s, v, var_get( hReq, "part" ), ",", _ ) );
					}

				v = var_get( hFlux, "send_file_nb" );
				n = ( v ) ? atoi( v ) : 0;
				n++;
				var_set( hFlux, "send_file_nb", itoa( n ) );

				v = var_get( hFlux, "send_file_size" );
				ll = ( v ) ? atoi( v ) : 0;
				ll += file_size( send_fname );
				var_set( hFlux, "send_file_size", itoa( ll ) );

				v = var_get( hFlux , "send_file_slist" );
				strdcpy( &s, v, (char*)((v)?", ":""), file_getname(fname2), _ );
				var_set( hFlux, "send_file_slist", s );


				//--------------------------------------------------------------
				// SAUVEGARDE L'ETAT ACTUELLE DU FLUX
				//--------------------------------------------------------------
				// suite a un probleme de sauvegarde de status flux
				//var_save( hFlux, strdcpy( &fname1, dirflux, "/", flux_id, ".dat", _ ) );

				for( j = 100 ;! var_save( hFlux, strdcpy( &fname1, dirflux, "/", flux_id, ".dat", _ ) ); j--, Sleep(100) )
				{
					if ( !j )
					{
					hlogu_status( hLog, log_id, "ERREUR : Pb enregistrement du fichier flux", _ );
					hlog_raise_status( hLog, etape_log_id,"ERREUR : Pb enregistrement du fichier flux", _ );
					on_error_execute( t_work );
					return true;
					}
				}

				}
			}
		}

	tr("emi_execute_27");

	if ( k == 0 )
		{
		tr("emi_execute_28");
		n = n1 = n2 = 0;
//		hlogc( hLog, etape_log_id, "ALERTE : Aucun fichier à envoyer", _ );
		hlog_raise_status( hLog, etape_log_id, "ALERTE : Aucun fichier à envoyer", _ );

		// supression des deux ligne suivantes qui provoque une perte de variables

		on_error_execute( t_work );
		return true;
		}

	strdcpy( &s, "send_", flux_etape, "_wait_end", _ );
	var_set( hFlux, s, "true" );
	var_save( hFlux, strdcpy( &fname1, dirflux, "/", flux_id, ".dat", _ ) );

	hlog_raise_status( hLog, etape_log_id, "Etape ", flux_etape, " : emi : ATTEND la fin des émissions", _ );

	tr("emi_execute_29");

	goto send_sleep;


	//--------------------------------------------------------------------------
	// ATTEND LA FIN DES TRANSFERTS
	//--------------------------------------------------------------------------
	send_wait_end :

	tr("emi_execute_30");

	v = var_get( hFlux, "send_to_surv" );
	tr2("send_to_surv=", v);
	if ( v )
		{
		tr("emi_execute_31");

		// Pour la monté de version
		if ( strpos( v, ",", 0 ) >= 0 )
			{
			strdcpy( &s, v, _ );
			v = strd_replace( &s, ",", "|" );
			var_set( hFlux, "send_to_surv", s );
			}

		if ( ts ) free( ts );
		ts = str_explode( v, "|" );

		tr2("emi_execute_31a ", ts[0]);

		_b = false;
		for( i = n1 = n2 = n = 0 ; ts[i] ; i += 2 )
			{
			if ( ! (ts[i][0]) ) { i--; continue; }
			tr2("emi_execute_32 ", ts[i]);
//			if ( ts2 ) free( ts2 );
//			ts2 = str_explode( ts[i], "," );
			id = atoi( ts[i] );

			hlogr( hLog, id, &s, null );
			n++;

			tr2("emi_execute_32.0 ", s);

			if ( !strnicmp( s, "ERREUR", 6 ) )		{ n1++; continue; }
			if ( !strnicmp( s, "ERROR", 5 ) )		{ n1++; continue; }
			if ( !strnicmp( s, "ALERT", 5 ) )		{ n1++; continue; }
			if ( !strnicmp( s, "ANNULE", 6 ) )		{ n1++; continue; }
			if ( !strnicmp( s, "CANCELLED", 9 ) )	{ n1++; continue; }
			if ( !strnicmp( s, "OK", 2 ) )			{ n2++; continue; }

			// TRANSFERT EN COURS
			tr2("emi_execute_32a ", ts[i+1]);
			var_free( h );
			if ( (h = mftcat_load( ts[i+1] )) == ERROR )
				continue;
			tr2("emi_execute_32b ", ts[i+1]);
			v = var_get( h, "status" );
			if ( ( !v )||(( v[0] != 'T' )&&( v[0] != 'K' )) )
				{
				hlogu_status( hLog, id, "En cours : ", v, " : ", var_get( h, "errno" ),
								" : ", var_get( h, "errmsg" ), _ );
				continue;
				}

			// TRANSFERT TERMINE

			tr("emi_execute_33");

			hlogu_status( hLog, id, var_get( h, "errmsg" ), _ );
			mftcat_logr( ts[i+1], &s );
			p = strpos( s, "\n", 0 );
			hlogc( hLog, id, "", &(s[p]), _ );


			// VERIFIE S'IL FAUT EXECUTER LE SCRIPT DE FIN
			//sni: cas doublon -> gerer le script fin demi
			strdcpy( &fname, dirtmp, DIR_CHAR "double_", flux_id, _ );
        		if ( file_exists( fname ) )
			{
			var_set( h, "status", "T" );
			logw("sni: cas doublon -> gerer le script fin demi",flux_id,_);
			}			


			v = var_get( h, "status" );
			if ( v[0] != 'K'  ) // Uniquement si OK
				{
				strdcpy( &s, "emi_script_", ts[i], _ );
				if ( ! var_get( hFlux, s ) )
					{
					tr("emi_execute_34");
					var_set( hFlux, s, "do" );
					strdcpy( &s, "emi_", ts[i], "_ida", _ );
					if ( ! var_get( hFlux, "waitack_asked" ) )
						{
						strdcpy( &s, argv[0], " mft setx ida=", var_get( hFlux, s ), _ );
						System( &s1, s );
						}

					v = trim(var_get( hEtape, "age_script" ));
					if ( ( v )&&( v[0] ) )
						{
						tr("emi_execute_35");
						strdcpy( &s, "emi_", ts[i], "_fname", _ );
						v3 = var_get( hFlux, s );
						var_set( hFlux, "_FNAME", v3 );
						v2 = var_get( hFlux, "var_list" );
						if ( !v2 )
							{
							var_set( hFlux, "var_list", ",FNAME," );
							}
				        else
				            {
							tr("emi_execute_36");
							if ( stripos( v2, ",FNAME", 0 ) < 0 )
				               	var_set( hFlux, "var_list", strdcpy( &s1, ",FNAME", v2, _ ) );
				            else
				               	{
								tr("emi_execute_37");
								if ( stripos( v2, ",FNAME,", 0 ) < 0 )
					               	{
									tr("emi_execute_38");
									var_set( hFlux, "var_list", strdcpy( &s1, v2, "FNAME", _ ) );
									}
								}
				            }

						strdcpy( &s, "emi_", ts[i], "_partname", _ );
						v3 = var_get( hFlux, s );
						var_set( hFlux, "_PARTNAME", v3 );
						v2 = var_get( hFlux, "var_list" );
						if ( stripos( v2, ",PARTNAME", 0 ) < 0 )
			               	var_set( hFlux, "var_list", strdcpy( &s1, ",PARTNAME", v2, _ ) );
			            else
			               	{
							tr("emi_execute_39");
							if ( stripos( v2, ",PARTNAME,", 0 ) < 0 )
				               	var_set( hFlux, "var_list", strdcpy( &s1, v2, "PARTNAME", _ ) );
							}

						var_list_prepare( hFlux );

						tr("emi_execute_40");

						id = hlogc( hLog, id, "En cours", "Exécution du script de fin d'émission", _ );
						_i = i; _n = n; _n1 = n1; _n2 = n2; _ts = ts; ts = null;
						ssie7_script_exec( t_work, false );
						i = _i; n = _n; n1 = _n1; n2 = _n2; if ( ts ) free( ts ); ts = _ts;
						_b = true;
						}
					}
				}
			}

		tr2("emi_execute_41:b=", itoa(_b));
		tr2("n1+n2=", itoa(n1+n2));
		tr2("n=", itoa(n));


		if ( ( ( n1+n2 ) == n )&&( ! _b ) )
			goto send_fin;

		tr("emi_execute_42");
		}


	//--------------------------------------------------------------------------
	// MET LE FLUX EN SOMMEIL PENDANT 2 SECONDES
	//--------------------------------------------------------------------------
	send_sleep :

	tr("emi_execute_43");
	var_set( hFlux, "wait_date", itoa( sysdate + 5 ) );

	// PLACE LE FLUX EN ATTENTE

	return false;


	//--------------------------------------------------------------------------
	// TOUS LES TRANSFERTS SONT TERMINES
	//--------------------------------------------------------------------------
	send_fin :

	tr("emi_execute_47");

	// VERIFIE SI TOUT EST OK

	hlogc_stat( hLog, etape_log_id, "ERREUR", &n1, "ERROR", &n2, "ALERTE", &n3, "OK", &n4, _ );
	
	// cas doublon => on evite les "erreurs a traiter: 0/0/0"
	strdcpy( &fname, dirtmp, DIR_CHAR "double_", flux_id, _ );
	if ( file_exists( fname ) ) 
	{
		n++;
		n4++;
		file_delete( fname );
		logw("emi_execute: ALERTE : double dans le flux: ",flux_id,"/", itoa(n),"/",itoa(n4),"/",itoa(n3),_);
	}


	//if ( ( n )&&( n4 == n ) )
	if ( ( n )&&( n4 >= n ) ) //On est dans le cas de fichier double
		{
		tr("emi_execute_48");

		// PAS D'ERREUR

		hlogu_status( hLog, etape_log_id, "OK : ", itoa(n4),
					" fichier", (char*)((n4>1)?"s":""),
					" envoyé", (char*)((n4>1)?"s":"") , _ );

		return true;
		}

	logw("emi_execute: ALERTE : erreurs a traiter: ",itoa(n),"/",itoa(n4),"/",itoa(n3),_);

	//--------------------------------------------------------------------------
	// IL Y A DES ERREURS => EXECUTE LE SCRIPT EN CAS D'ERREUR
	//--------------------------------------------------------------------------

	hlog_raise_status( hLog, etape_log_id, "ERREUR : Il y a des erreurs d'émissions", _ );

	on_error_execute( t_work );

	tr("emi_execute_49.");

	//--------------------------------------------------------------------------

	return true;
	}






















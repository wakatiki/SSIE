#include "ssie7.h"



//------------------------------------------------------------------------------
int sendack_execute( T_WORK* t_work )

// EXECUTE UNE ETAPE DE FLUX DE TYPE SENDACK (Envoi d'un accuse-reception)
//------------------------------------------------------------------------------
	{
	int _i;
	char **_ts, **_ts2;
	
//	if ( var_get( hFlux, "sendack_to_surv" ) ) goto sendack_wait_end;

	// ENVOI LES A.R.
	
	v = trim(var_get( hFlux, "sendack_list" ));
	if ( ( !v )||( ! (v[0]) ) )
		{
//		hlogu_status( hLog, etape_log_id, "OK : Pas d'A.R. à envoyer", _ );
		hlogu( hLog, etape_log_id, "OK : Pas d'A.R. à envoyer", "", _ );
		return true;
		}
	
	strdcpy( &fname4, var_get( hEtape, "etape_ack_fname" ), _ );
	var_list_replace( hFlux, &fname4 );
	if ( !file_exists( fname4 ) )
		{
		strdcpy( &fname4, dirwrk, DIR_CHAR "vide.txt", _ );
		file_save_str( fname4, "", 0 );
		}
	#ifdef OS_UNIX
	else
		{
		v2 = trim( var_get( hFlux, "app_login" ) );
		if (( v2 )&&( (p=strpos( v2, ":", 0 )) >= 0 ))
			v2 = strdncpy( &u6, v2, p, _ );
				
		if (( v2 )&&( v2[0] )&&( strcmp( v2, ssie_user ) ))
			{
//			strdcpy( &fname3, dirtemp, DIR_CHAR, flux_id, "_emi_sudo.ack", _ );
			file_tmpname( &fname3 );
			
			strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
					"cp -f ", fname4, " ", fname3, _ );
			
			System( &s3, s );
			id = hlogc( hLog, etape_log_id, "", s, BR, s3, _ );
			
			strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
					"chown -R ", ssie_user, ":", ssie_user, " ", fname3, _ );
			
			System( &s3, s );
			
			v1 = fname3; fname3 = fname4; fname4 = v1;
			}
		else
			v2 = null;
		}
	#endif	
	
	//--------------------------------------------------------------------------
	// PREPARE LA COMMANDE D'ENVOI DES L'A.R.
	//--------------------------------------------------------------------------
	
	n3 = 0;
	_ts = str_explode( v, "|" );
	for( _i = 0 ; _ts[_i] ; _i++ )
		{
		if ( !(_ts[_i][0]) ) continue;
		_ts2 = str_explode( _ts[_i], "," ); // 0=IDA, 1=PARTNAME
		strdcpy( &s, _ts2[0], ":", substr(hlogc_last_status( hLog, logroot ), 0, 40), _ );
		
		strdcpy( &cmd, "mft send part=", _ts2[1], " fname=", fname4, " ascii"
//			" src_fname=", quote( file_getname( fname ) ),
			" idf=ACK ",
			" parm=", quote( s ),
//			(char*)(( v2 ) ? " faction=delete" : ""),
			_ );
		
		var_list_replace( hFlux, &cmd );
		
		if ( hReq ) var_free( hReq );
		hReq = var_from_cmd( 0, cmd );
		
		
		//--------------------------------------------------------------
		// REALISE LE TRANSFERT
		//--------------------------------------------------------------
	
		
		id = hlogc( hLog, etape_log_id, "En cours", "A.R. Envoyé(s) vers ", _ts2[1], _ );
		hlogc( hLog, id, "", cmd, _ );
		
		log_id = id;
		cmd_mft_send( t_work );

		// ida contient l'identifiant unique du transfert

		hlogu( hLog, id, "OK", ida, " : A.R. Envoyé(s) vers ", _ts2[1], _ );
		
		v = var_get( hFlux, "sendack_to_surv" );
		strdcpy( &s1, v, "|", itoa(id), ",", ida, _ );
		var_set( hFlux, "sendack_to_surv", s1 );

		free( _ts2 );
		
		n3++;
		}
	
	free( _ts );
	
/*
	strdcpy( &s, "send_", flux_etape, "_wait_end", _ );
	var_set( hFlux, s, "true" );
	var_set( hFlux, "sendack_list", "" );
	var_save( hFlux, strdcpy( &fname1, dirflux, "/", flux_id, ".dat", _ ) );
*/

	hlogu_status( hLog, etape_log_id, "OK : ", itoa(n3),
				" A.R. envoyé", (char*)((n3>1)?"s":"") , _ );
	
	return true;
	
/*
La suite est annulé car cela ralenti les traitements

	goto sendack_sleep;
	

	//--------------------------------------------------------------------------
	// ATTEND LA FIN DES TRANSFERTS
	//--------------------------------------------------------------------------
	sendack_wait_end :

	v = var_get( hFlux, "sendack_to_surv" );
	if ( v )
		{
		if ( ts ) free( ts );
		ts = str_explode( v, "|" );
		
		b = false;
		for( i = n1 = n2 = n = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			if ( ts2 ) free( ts2 );
			ts2 = str_explode( ts[i], "," );
			id = atoi( ts2[0] );
			
			hlogr( hLog, id, &s, null );
			n++;
			if ( !strnicmp( s, "ERREUR", 6 ) )	n1++;
			if ( !strnicmp( s, "ERROR", 5 ) )	n1++;
			if ( !strnicmp( s, "OK", 2 ) )		n2++;
			}
		
		
		if ( ( ( n1+n2 ) == n )&&( ! b ) )
			goto sendack_fin;

		}


	//--------------------------------------------------------------------------
	// MET LE FLUX EN SOMMEIL PENDANT 2 SECONDES
	//--------------------------------------------------------------------------
	sendack_sleep :
	
	
	// PLACE LE FLUX EN ATTENTE

	hlog_raise_status( hLog, etape_log_id, "ATTEND la fin des émissions des A.R.", _ );
	return false;
	
	
	//--------------------------------------------------------------------------
	// TOUS LES TRANSFERTS SONT TERMINES
	//--------------------------------------------------------------------------
	sendack_fin :

	// VERIFIE SI TOUT EST OK

	if ( var_get( hFlux, "stop_after_sendack" ) )
		var_set( hFlux, "etape_next", "stop" );
	
	hlogc_stat( hLog, etape_log_id, "ERREUR", &n1, "ERROR", &n2, "OK", &n3, _ );

	if ( ( n )&&( n3 == n ) )
		{
		// PAS D'ERREUR
		
		hlogu_status( hLog, etape_log_id, "OK : ", itoa(n3),
					" A.R. envoyé", (char*)((n3>1)?"s":"") , _ );
		
		return true;
		}
	
	
	
	//--------------------------------------------------------------------------
	// IL Y A DES ERREURS => EXECUTE LE SCRIPT EN CAS D'ERREUR
	//--------------------------------------------------------------------------

	hlog_raise_status( hLog, etape_log_id, "ERREUR : Il y a des erreurs d'émissions", _ );
	
	on_error_execute( t_work );
		
	//--------------------------------------------------------------------------

	return true;		
*/
	}



//------------------------------------------------------------------------------
int waitack_execute( T_WORK* t_work )

// EXECUTE UNE ETAPE DE FLUX DE TYPE WAITACK (Attente d'accusé-reception)
//------------------------------------------------------------------------------
	{
	int _i;
	char **_ts, **_ts2;
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
	
	v = trim(var_get( hFlux, "waitack_list" ));
	if ( !v )
		{
		hlogu( hLog, etape_log_id, "OK : Pas d'A.R. à attendre", "", _ );
		return true;
		}
	
	strdcpy( &s4, v, "|", _ );

	if ( ! var_get( hFlux, "waitack_timeout" ) )
		{
//		n = atoi(var_get( hEtape, "etape_timeout" )) * 60;
		strdcpy( &s, var_get( hEtape, "etape_timeout" ), _ );
		var_list_replace( hFlux, &s );
		n = atoi(s) * 60;
		if ( n )	n += sysdate;
		var_set( hFlux, "waitack_timeout", itoa(n) );
		}

	
	// CHERCHE LES idf=ACK DANS LE CATALOGUE MFT
	
	strdcpy( &cmd, "idf=ACK direct=recv status=F|T parm=", ito36( atoi(flux_id), 5 ), "*", _ );
	n4 = 0;

	if (( bd_select( hMftCat, cmd, null, &s3 ) == ERROR )||( s3[0] == 0 ))
		goto traite_timeout;


	v = u10; u10 = s3; s3 = v;
	for( p6 = 0, l = strlen(u10) ; p6 < l ; p6 += 8 )
		{
		if ( ( hLock )&&( ida[0] ) ) bd_unlock( hMftCat, ida, hLock );
		strdncpy( &ida, &(u10[p6]), 8, _ );
		
		// CHARGE LE TRANSFERT
		
		var_free( h );
		if ( (h = mftcat_load( ida )) == ERROR )
			continue;

		// VERIFIE LE STATUS
		
		v = var_get( h, "status" );
		if ( !v ) continue;
		if (( v[0] != 'F' )&&( v[0] != 'T' ))
			{
			// LANCE UNE REPARATION D'INDEXE POUR LE CAS OU
			mftcat_repair( ida );
			continue;
			}
		
		
		v = var_get( h, "parm" );
		strdncpy( &s, "|", 1, &(v[5]), 3, "|", 1, _ );
		if ( strpos( s4, s, 0 ) >= 0 )
			{
			strd_replace( &s4, s, "|" );
			s[4] = 0;
			id = ifrom36( &(s[1]) );
			
			if ( (p=stripos( v, "erreur", 0 )) >= 0 )
				{
				strdcpy( &s, "ERREUR : A.R. Reçu ", &(v[p+6]), _ );
				hlogu_status( hLog, etape_log_id, s, _ );
				var_set( hFlux, "waitack_status", v );
				}
			else
				hlogu_status( hLog, etape_log_id, "OK : A.R. Reçu", _ );
			
			p1 = hlogc( hLog, etape_log_id, "", "A.R. Reçu", _ );
			v = var_get( h, "sfname" );
			if ( stricmp( v, "vide.txt" ) )
				{
				v = var_get( h, "fname" );
				file_load( &s, v );
				hlogc( hLog, p1, "", s, _ );
				}
			}
		
		
		// PASSE LE TRANSFERT A L'ETAT X
		
		strdcpy( &s, argv[0], " mft setx ida=", ida, _ );
		System( &s1, s );
		}
	
	while( strpos( s4, "||", 0 ) >= 0 )
		strd_replace( &s4, "||", "|" );
		
	var_set( hFlux, "waitack_list", s4 );

	
	// VERIFIE LE TIMEOUT
	
	if ( s4[1] ) // Il y a encore des AR en attente
		{
		traite_timeout:
			
		if (( n=atoi(var_get(  hFlux, "waitack_timeout" )) )&&( sysdate >= n ))
			{
			hlogu_status( hLog, etape_log_id, "ERREUR : Temps limite de réception des A.R. est dépassé", _ );
			hlogc( hLog, etape_log_id, "", "TIMEOUT", _ );
			on_error_execute( t_work );
			return true;
			}
			
		return false;
		}	
	
	
	// TOUS LES A.R. ONT ETE RECUS
	
	v = var_get( hFlux, "waitack_status" );
	if ( !v ) v = "OK";
	
	hlogu_status( hLog, etape_log_id, v, _ );
	hlogu_status( hLog, 0, v, _ );
	
	return true;
	}












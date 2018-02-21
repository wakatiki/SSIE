#include "ssie7lib.h"


#define bAscii				b4
#define bNoCrypt			b3
#define tran_id				(t_work->u1)
#define fpLog				fp2
#define cat_fname			fname3
#define log_fname			fname4
#define part_fname			fname2
#define data_fname			fname1

#define repository			v7
#define bJS					v6
#define rexec				v5


//------------------------------------------------------------------------------
int cmd_mft_server_recv( T_WORK * t_work )

// Réception de fichier MFT
//------------------------------------------------------------------------------
	{
	if ( request_mode != MODE_TCP )
		return ERROR;

	// Syntaxe : mft_server_recv fname=..., [windows/unix] , parm1=val1, [...]...

//logw( "cmd_mft_server_recv1", _ );
tr("cmd_mft_server_recv1");

	if ( ! var_get( hReq, "spart" ) )
		var_set( hReq, "spart", session_partname );
		
	v = var_getn( hReq, 1, &vn );


	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "gw_status" ) )
	//--------------------------------------------------------------------------
		{
tr("cmd_mft_server_recv2");
		if ( ! var_getn( hReq, 2, &v ) )
			{
			print( "ERREUR RECEPTEUR : Transaction inconnue : (null)", _ );
			return ERROR;
			}
		
		var_free( h );
//		if ( (h = mftcat_load( v )) == ERROR )
		if (( (h = mftcat_load( v )) == ERROR )
				||( !var_get( h, "status" ) ))
			{
			print( "ERREUR RECEPTEUR : Transaction inconnue : ", v,_ );
			return ERROR;
			}
		
		print( var_get( h, "errno" ), "::", var_get( h, "status" ), "::", var_get( h, "errmsg" ), _ );
tr("cmd_mft_server_recv2f");
		return OK;
		}

	
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "status" ) ) // Pour compatibilité avec ssie v7.1
	//--------------------------------------------------------------------------
		{
tr("cmd_mft_server_recv3");
		if ( ! var_getn( hReq, 2, &v ) )
			{
			print( "ERREUR RECEPTEUR : Transaction inconnue : (null)", _ );
			return ERROR;
			}
		
tr("cmd_mft_server_recv3a");
tr2( "IDA=", v );
		var_free( h );
//		if ( (h = mftcat_load( v )) == ERROR )
		if (( (h = mftcat_load( v )) == ERROR )
				||( !var_get( h, "status" ) ))
			{
			print( "ERREUR RECEPTEUR : Transaction inconnue : ", v,_ );
			return ERROR;
			}
		
tr("cmd_mft_server_recv3b");
		
		strdcpy( &s, var_get( h, "errno" ), ":", _ );
		v = var_get( h, "status" );
if ( !v )
tr2( "status=", "null" );
		
tr("cmd_mft_server_recv3c");
		if ( v )
			{
			if ( v[0] == 'C' ) strdcat( &s, "En cours : ", var_get( h, "errmsg" ), _ );
			else
			if ( v[0] == 'D' ) strdcat( &s, "En cours : ", var_get( h, "errmsg" ), _ );
			else
			if ( v[0] == 'T' ) strdcat( &s, "OK : ", var_get( h, "errmsg" ), _ );
			else
			if ( v[0] == 'K' ) strdcat( &s, "ERREUR : ", var_get( h, "errmsg" ), _ );
			else
			if ( v[0] == 'W' ) strdcat( &s, "En cours : ", var_get( h, "errmsg" ), _ );
			}
		else
			strdcat( &s, "ERREUR : Transaction inconnue", _ );
			
		print( s, _ );
//		print( var_get( h, "diagi" ), ":", var_get( h, "status" ), _ );
tr("cmd_mft_server_recv3f");
		
		return OK;
		}
		
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "log" ) )
	//--------------------------------------------------------------------------
		{
tr("cmd_mft_server_recv4");
		if ( ! var_getn( hReq, 2, &v ) )
			{
			print( "ERREUR RECEPTEUR : Transaction inconnue : (null)", _ );
			return ERROR;
			}
		
		var_free( h );
		if ( (mftcat_logr( v, &s )) == ERROR )
			{
			print( "ERREUR RECEPTEUR : Transaction inconnue : ", v,_ );
			return ERROR;
			}
		print( s, _ );
tr("cmd_mft_server_recv4f");

		return OK;
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "cancel" ) )
	//--------------------------------------------------------------------------
		{
tr("cmd_mft_server_recv5");
		if ( ! var_getn( hReq, 2, &v ) )
			{
			print( "ERREUR RECEPTEUR : Transaction inconnue : (null)", _ );
			return ERROR;
			}
		
		strdcpy( &ida, v, _ );
//		if ( (h = mftcat_load( ida )) == ERROR )
		if (( (h = mftcat_load( ida )) == ERROR )
				||( !var_get( h, "status" ) ))
			{
			print( "ERREUR RECEPTEUR : Transaction inconnue : ", ida,_ );
			return ERROR;
			}
		
		mftcat_logw( h, "TRANSFERT ANNULE PAR L'EMETTEUR.", _ );
		
		if ( atoi( var_get( h, "errno" ) ) ) v = null;
		else v = "200";
		v1 = var_get( h, "errmsg" );
		strdcpy( &s, "ANNULE", (char*)(((v1)&&(v1[0])) ? ":" : ""), v1, _ );

		var_set( h, "_transfert_canceled", "1" );
		mftcat_upd( h, null, v, s );

//		mftcat_upd( h, "K", v, s );
		
		print( "OK", _ );
tr("cmd_mft_server_recv5f");
	
		return OK;
		}
	
	
	
	if ( ! session_partname )
		{
		print( "ERREUR RECEPTEUR : Vous devez vous identifier", _ );
		return ERROR;
		}
	
	t = 0; // N° de test à réaliser		
	
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "tran_id" ) )
	//--------------------------------------------------------------------------
		{
		// CAS D'UNE REPRISE
tr("cmd_mft_server_recv6");
		
		strdcpy( &ida, v, _ );

		var_free( h );
//		if ( (h = mftcat_load( ida )) == ERROR )
		if (( (h = mftcat_load( ida )) == ERROR )
				||( !var_get( h, "status" ) ))
			{
			print( "ERREUR RECEPTEUR : Transaction inconnue : ", ida,_ );
			return ERROR;
			}
		
		
		if ( v = var_get( hReq, "file_size" ) )
			{
			ll2 = atoll(v);
			var_set( h, "file_size", v );
			}
		else
			ll2 = atoll(var_get( h, "file_size" ));
		
		var_free( hReq );
		hReq = h;
		h = 0;
		
		#ifdef OS_WINDOWS
		bAscii = ( var_get( hReq, "unix" ) ) ? true : false;
		#else // OS_UNIX
		bAscii = ( var_get( hReq, "windows" ) ) ? true : false;
		#endif
		
		
		// OUVRE LE FICHIER
		
		strdcpy( &data_fname, var_get( hReq, "_recep_fname" ), _ );
		
		if ( !bAscii )		{ ll = file_size( data_fname ); if ( ll < 0 ) ll = 0; }
		else				ll = 0;
		
		fp = fopen( data_fname, (char*)((bAscii) ? "wb" : "ab") );
		if ( !fp )
			{
			print( "ERREUR RECEPTEUR : Pb accès fichier", _ );
			
			logw( ip_cli, " : ", session_partname, " : mft_server_recv : ERREUR : Pb ouverture fichier : ", data_fname, _ );
			mftcat_logw( hReq, "ERREUR : ouverture du fichier : ", data_fname, _ );
			mftcat_upd( hReq, "K", "101", "ERREUR : Pb ouverture fichier" );
			return ERROR;
			}

		
		// ACCEPTE LA REPRISE
		
		print( "OK:", v = itoa( ll ), __ );
		
		mftcat_logw( hReq, "Reprise du transfert à partir de ", v, _ );
		mftcat_upd( hReq, "C", "0", "En cours : Réception des données" );

tr("cmd_mft_server_recv6f");
		
		goto data_recep;
		}
	
	//--------------------------------------------------------------------------
	// CAS D'UNE NOUVELLE DEMANDE DE TRANSFERT DE FICHIER
	//--------------------------------------------------------------------------

tr("cmd_mft_server_recv7");

	strdcpy( &fname, dirwrk, DIR_CHAR "LockServer", _ );
	for( ; file_exists( fname ) ; sleep(1) );

//logw( "cmd_mft_server_recv2", _ );
	
	
	#ifdef OS_WINDOWS
	bAscii = ( var_get( hReq, "unix" ) ) ? true : false;
	#else // OS_UNIX
	bAscii = ( var_get( hReq, "windows" ) ) ? true : false;
	#endif
	var_delete( hReq, "mft_log" );
	var_delete( hReq, "_elib_bd_vlf_mft_log" );
	var_delete( hReq, "_elib_bd_index" );
	var_delete( hReq, "mft_server_recv" );
	var_delete( hReq, "log_id" );
	t = atoi( var_get( hReq, "test_crash" ) );

//logw( "cmd_mft_server_recv3", _ );
	
	if ( partname = var_get( hReq, "spart" ) )
		{
//logw( "cmd_mft_server_recv4", _ );
		strdcpy( &part_fname, dirconf, DIR_CHAR "part" DIR_CHAR, partname, ".dat", _ );
		if ( !file_exists( part_fname ) )
			{
//logw( "cmd_mft_server_recv5", _ );
			b = false;
			v = var_get( hSrvConf, "part_auto_crea" );
			if ( !strcmp( v, "Oui" ) )
				{
				// RECHERCHE LE PARTENAIRE SUR LE SERVEUR D'ANNUAIRE
				strdcpy( &cmd, argv[0], " part ", partname, " loadnat", _ );
				System( &s, cmd );
				b = file_exists( part_fname );
				}
			if (  !b )
				{
				logw( ip_cli, " : ", partname, " : ", var_get( hReq, "ida" ),
						" : mft_server_recv : connexion refusée (1)", _ );
				print( "ERREUR : connexion refusée (1)", BR, _ );
				return ERROR;
				}
			}
			
//logw( "cmd_mft_server_recv5", _ );
		if ( hPart ) var_free( hPart );
		hPart = var_load( part_fname );
		v = var_get( hPart, "enable" );
		if ( ( v )&&( strcmp( v, "Oui" ) ) )
			{
			logw( ip_cli, " : ", session_partname, " : ", var_get( hReq, "ida" ),
					" : mft_server_recv : connexion refusée (2)", _ );
			print( "ERREUR : connexion refusée (2)", BR, _ );
			return ERROR;
			}
		}
	
//logw( "cmd_mft_server_recv6", _ );
	
	//--------------------------------------------------------------------------
	// LECTURE DES PARAMETRES POUR LA RECEPTION
	//--------------------------------------------------------------------------
	
	if ( v = var_get( hReq, "mft_gateway" ) )
		{
		if ( ! mft_exists() )
			{
			print( "ERREUR MFT GATEWAY : Ce serveur n'est pas une passerelle MFT", _ );
			return ERROR;
			}
		
//logw( "cmd_mft_server_recv7", _ );
		
		// VERIFIE QUE LE DESTINATAIRE EXISTE EST QUE C'EST UN PARTENAIRE MFT
		
		if ( ! ( partname = var_get( hReq, "partname" ) ) )
			if ( ! ( partname = var_get( hReq, "part" ) ) )
				partname = var_get( hReq, "rpart" );
			
		if ( ! part_exists( t_work ) )
			{
			print( "ERREUR MFT GATEWAY : le partnaire n'est pas connu : ", partname, _ );
			return ERROR;
			}
		
		if ( hPart ) var_free( hPart );
		hPart = var_load( fname );

//logw( "cmd_mft_server_recv8", _ );
		
		v = var_get( hPart, "product_name" );
		if ( ( v )&&( !strnicmp( v, "SSIE v7", 7 ) ) )
			{
			print( "ERREUR MFT GATEWAY : le partnaire ne possède pas MFT : ", partname, _ );
			return ERROR;
			}

//logw( "cmd_mft_server_recv9", _ );

		// Vérifie que le dns du distant n'est pas identique au serveur
		if ( stricmp( partname, var_get( hSrvConf, "partname" ) ) )
			{
			v1 = trim(var_get( hPart, "dns" ));
			v2 = trim(var_get( hSrvConf, "dns_ssie" ));
			if ( ( v1 )&&( v2 )&&( v1[0] )&&( v2[0] )&&( ! stricmp( v1, v2 ) ) )
				{
				print( "ERREUR MFT GATEWAY : DNS du distant est identique au serveur !!!", _ );
				logw( "ERREUR MFT GATEWAY : DNS du distant (", partname, ") est identique au serveur !!!", _ );
				return ERROR;
				}
			}
		}
		
	
	//--------------------------------------------------------------------------
	// CALCUL UN IDA LOCAL
	//--------------------------------------------------------------------------
	
	strdcpy( &fname, dirdat, DIR_CHAR "mftcat" DIR_CHAR "ida.id", _ );
	n = get_uid( fname );

	strdcpy( &ida, ito36( n, 8 ), _ );
	ida[0] = 'A' + year(sysdate) - 2013;
	if ( (n=month(sysdate)) < 10 )
		ida[1] = '0' + n;
	else
		ida[1] = 'A' + n - 10;
	ida[2] = ito36( day(sysdate), 1 )[0];
	
	
	
	//--------------------------------------------------------------------------
	// ENREGISTRE LES META-DONNES ET INITIALISE LA LOG
	//--------------------------------------------------------------------------
	
	if ( !var_get( hReq, "sida" ) )
		var_set( hReq, "sida", var_get( hReq, "ida" ) );
	var_set( hReq, "ida", ida );
	var_set( hReq, "rida", ida );
	var_set( hReq, "idtu", ida );
	var_set( hReq, "idt", ida );
	var_set( hReq, "direct", "RECV" );
	var_set( hReq, "part", var_get( hReq, "spart" ) );
	
	v = currdate( "JJ/MM/AAAA HH:MM:SS" );
	if ( ! var_get( hReq, "dateb" ) )
		{
		var_set( hReq, "dateb", substr( v, 0, 10 ) );
		var_set( hReq, "timeb", substr( v, 11, 8 ) );
		}
	
	
	//--------------------------------------------------------------------------
	// PREPARE LE FICHIER DE RECEPTION DES DONNEES
	//--------------------------------------------------------------------------

	h = hReq;
	if ( mft_recv_fname_compute( t_work, "RECV" ) != 1000 )
		strdcpy( &data_fname, dirrecv, DIR_CHAR, ida, ".data", _ );
		
	h = 0;
	var_set( hReq, "_recep_fname", data_fname );
	ll2 = atoll(var_get( hReq, "file_size" ));
//mftcat_logw( hReq, "(0):ll2=", itoa(ll2), " (", var_get( hReq, "file_size" ), ")", _ );	

//	ll2 = atoi(var_get( hReq, "file_size" ));
	ll = 0;
	
//	logw( ip_cli, " : ", session_partname, " : mft_server_recv : ", ida, " : sida = ", var_get( hReq, "sida" ), _ );

	fp = fopen( data_fname, "wb" );
	if ( !fp )
		{
		mkdir( file_getdir( data_fname ) );
		fp = fopen( data_fname, "wb" );
		if ( !fp )
			{
			print( "101 : ERREUR RECEPTEUR : Pb création du fichier de réception", _ );
			logw( ip_cli, " : ", session_partname, " : mft_server_recv : ERREUR : Pb création fichier : ", data_fname, _ );
			mftcat_logw( hReq, session_partname, " : mft_server_recv : ERREUR : Pb création fichier : ", data_fname, _ );
			mftcat_crea( hReq, "K", "101", "ERREUR : Pb création du fichier de réception" );
			return ERROR;
			}
		}

	

	//--------------------------------------------------------------------------
	// ACCEPTE LE TRANSFERT
	//--------------------------------------------------------------------------
	
//	strdcpy( &s, "OK:", tcp_myIP(0), ":", var_get( hSrvConf, "port" ), "#", ida, _ );
	strdcpy( &s, "OK:", var_get( hSrvConf, "ip_ssie" ), ":", var_get( hSrvConf, "port" ),
					"#", ida, _ );
	if ( tcp_ssend( hCnx, s, -1 ) == ERROR )
		{
		fclose( fp );
		logw( ip_cli, " : ", session_partname, " : ", ida,
			 " : mft_server_recv : ERREUR : connexion perdue (1)" BR,
			 var_to_str( hReq, &s ), _ );
		ssie7disconnect( t_work );
		return ERROR;	// Connexion perdue
						// On n'enregistre pas les données car il n'y aura pas
						// de reprise sur le même IDA puisqu'il n'a pas été transmis
		}
	
	mftcat_logw( hReq, ip_cli, ":", var_get( hReq, "spart" ),
					" : Transfert accepté en mode ",
					(char*)((bAscii) ? "ascii":"binaire"),
					/*
					 BR "   ",
					var_to_str( hReq, &s ), BR "   ",
					fname1, " (", var_get( hReq, "file_size" ), ")" BR,
					*/
					_ );
	
	if ( strcmp( session_version, PROTOCOLE_VERSION ) )
		mftcat_logw( hReq, "PROTOCOL_VERSION : ", PROTOCOLE_VERSION, ", remote version=", session_version, _ );
	
	mftcat_logw( hReq, "Début réception des données", _ );
	mftcat_crea( hReq, "C", "0", "En cours : Réception des données" );

	
	//--------------------------------------------------------------------------
	// RECEPTION DES DONNEES
	//--------------------------------------------------------------------------
	
	data_recep:
	
	
	strdcpy( &s1, "FIN ", var_get( hReq, "ida" ), _ );
	bNoCrypt = ( var_get( hReq, "nocrypt" ) ) ? true : false;

	c = n4 = time(NULL);
	ll1 = ll;
	r = OK;

	for( ; ; )
		{
		if ( bNoCrypt ) n = tcp_srecv0( hCnx, &s ); else n = tcp_srecv( hCnx, &s );
		if ( n == ERROR )
			{
			fclose( fp );
			logw( ip_cli, " : ", session_partname, " : ", ida, " : mft_server_recv : ERREUR : connexion perdue (2)", _ );
			ssie7disconnect( t_work );
			mftcat_logw( hReq, "ERREUR:Connexion perdue (2)", _ );
			mftcat_upd( hReq, "K", "302", "ERREUR:Connexion perdue (2)" );
			return ERROR; // Connexion perdue
			}
			
		if ( ! strcmp( s, s1 ) ) break;
if ( ( n < 300 )&&( ! strncmp( s, s1, 4 ) ) ) break; // En attendant que tout le monde passe en dernière version
		
		if ( r != OK ) continue;
		
		n2 = n;

		if ( t == 1 ) // Test crash
			{
			fwrite( fp, s, 10, _ );
			fclose( fp );
			sleep(2);
			ssie7disconnect( t_work );
			mftcat_logw( hReq, "ERREUR:Test crash 1", _ );
			mftcat_upd( hReq, "K", "234", "ERREUR:Test crash 1" );
			return ERROR; // Connexion perdue
			}
		
		if ( bAscii )
			{
			// CONVERSIONS ASCII
			#ifdef OS_WINDOWS
			for( n3 = 0, i = 1 ; i < n ; i++ )
				{
				if (( s[i] == '\n' )&&( s[i-1] != '\r' )) n3++;
				}
			
			if ( n3 )
				{
				if ( !s2 ) s2 = alloc( n+n3 ); else s2 = realloc( s2, n+n3 );
				for( i = j = 0 ; i < n ; i++ )
					{
					if ( ( i )&&( s[i] == '\n' )&&( s[i-1] != '\r' ) )
						{
						s2[j++] = '\r'; s2[j++] = '\n';
						}
					else
						s2[j++] = s[i];
					}
				
				v = s; s = s2; s2 = v;
				n = j;
				}
			#else // UNIX
			for( n3 = 0, i = 0 ; i < n ; i++ )
				{
				if (( s[i] == '\r' )&&( s[i+1] == '\n' )) n3++;
				}
			
			if ( n3 )
				{
				if ( !s2 ) s2 = alloc( n ); else s2 = realloc( s2, n );
				for( i = j = 0 ; i < n ; i++ )
					{
					if (( s[i] == '\r' )&&( s[i+1] == '\n' ))
						{
						s2[j++] = '\n'; i++;
						}
					else
						s2[j++] = s[i];
					}
				
				v = s; s = s2; s2 = v;
				n = j;
				}
			
			#endif
			}
		
		n1 = fwrite( fp, s, n, _ );
		if ( n1 != n )
			{
			fclose( fp );
			logw( ip_cli, " : ", session_partname, " : ", ida, " : mft_server_recv : ERREUR : pb écriture sur le disque", _ );
			tcp_disconnect( hCnx );	session_cnx = hCnx = 0;
			ssie7disconnect( t_work );
			mftcat_logw( hReq, "ERREUR : pb écriture sur le disque", _ );
			mftcat_upd( hReq, "K", "109", "ERREUR : pb écriture sur le disque" );
			r = ERROR;
			continue;
			}
		

		if ( c != time(NULL) ) // flush toutes les secondes
			{
			fflush( fp );
			c = time(NULL);
			}
		
		ll += n2;
		ll1 += n;
		
		if ( ( ll2 )&&( ( time(NULL) - n4 ) > 5 ) )
			{
			mftcat_upd( hReq, null, null, strdcpy( &s, itoa( ((ll * 100)/ll2) ), "%", _ ) );
			n4 = time(NULL);
			}
		}
		
	fclose( fp );
	Sleep( 1 );
	
	//--------------------------------------------------------------------------
	// CONTROLE D'INTEGRITE
	//--------------------------------------------------------------------------
	
	if ( ( (ll=file_size( fname1 )) != ll1 )
		||( ( !bAscii )&&( ll1 != ll2 ) ) )
		{
		logw( ip_cli, " : ", session_partname, " : ", ida, " : mft_server_recv : ERREUR : Le fichier reçu n'a pas la bonne taille", _ );
		mftcat_logw( hReq, "ERREUR : Le fichier reçu ", fname1, " n'a pas la bonne taille : ", itoa( ll ), " au lieu de ", itoa( ll2 ), " (ll1=", itoa(ll1), ")", _ );
		mftcat_upd( hReq, "K", "460", "ERREUR : pb intégrité des données reçues" );
		print( "ERREUR: pb intégrité des données reçues.", __ );
		unlink( fname1 );
		return ERROR;
		}

	
	//--------------------------------------------------------------------------
	// ENVOI CONFIRMATION FIN DE TRANSFERT
	
	print( "OK:", var_get( hSrvConf, "ip_ssie" ), ":", var_get( hSrvConf, "port" ), "#", ida, __ );
	
	var_set( hReq, "fname", data_fname );

	mftcat_logw( hReq, "Réception des données OK", _ );

	
	//--------------------------------------------------------------------------
	// SI MFT GATEWAY => PLACE LE TRANSFERT DANS LA FILE D'ATTENTE MFT
	//--------------------------------------------------------------------------
				
	if (( mft_exists() )&&( var_get( hReq, "mft_gateway" ) ))
		{
		mftcat_logw( hReq, "Place le transfert dans la file d'attente MFT", _ );
		var_set( hReq, "faction", "delete" ); // Demande suppression fichier après transfert
		
		mftcat_upd( hReq, "D", "0", "En cours : MFT" );

		return OK;
		}
				


	//--------------------------------------------------------------------------
	// FIN NORMALE
	//--------------------------------------------------------------------------
				
	v = currdate( "JJ/MM/AAAA HH:MM:SS" );
	var_set( hReq, "datee", substr( v, 0, 10 ) );
	var_set( hReq, "timee", substr( v, 11, 8 ) );
	
	#ifdef OS_UNIX
	v = var_get( hReq, "chown" );
	if ( v )
		{
		strdcpy( &cmd, "sudo ", dirbin, "/ssie7 chown ", v, " ", data_fname, _ );
		system( cmd );
		}
	#endif
	
	mftcat_upd( hReq, "W", "0", "OK" );

	return OK;
	}




#undef bAscii
#undef bNoCrypt
#undef tran_id
#undef fpLog
#undef log_fname
#undef part_fname
#undef repository
#undef bJS
#undef rexec




















#include "ssie7lib.h"


#define src_fname			(fname3)


//------------------------------------------------------------------------------
int cmd_mft_send( T_WORK * t_work )

// INITIALISE UN TRANSFERT DE FICHIER VERS UN AUTRE SSIE v7
//------------------------------------------------------------------------------
	{
	#ifdef OS_UNIX
	if (( request_mode == MODE_TCP )&&( strcmp( ip_cli, "127.0.0.1" ) ))
		{
		print( "ERREUR : commande non autorisée" BR, _ );
		return ERROR;
		}
	#endif
	//logw("sni: cmd_mft_send:idf=", var_get( hReq, "idf" ),_);
	//var_set( hReq, "idf", "R2SNI" );
	//----------------------------------------------------------------------
	// MFT SEND : INITIALISATION D'UN TRANSFERT
	//----------------------------------------------------------------------

	// LECTURE DES PARAMETRES
	
	partname = upper( strdcpy( &part, var_get( hReq, "part" ), _ ) );

	v = var_get( hReq, "fname" );
	if ( ( !v )|| ( !partname ) )
		{
		if ( !log_id ) print( "ERREUR : syntaxe : mft send part=[partname|adl] fname=[src_fname] [binary/ascii] [...]", _ );
		else hlogc( hLog, log_id, "", "ERREUR : syntaxe : send part=[partname|adl] fname=[src_fname] [binary/ascii] [...]", _ );
		return ERROR;
		}
	strdcpy( &src_fname, v, _ );
	
	#ifdef OS_WINDOWS

// Spécificité EDF. Essayer de le déplacer dans MFTUTIL
	
	if ( ( !strnicmp( src_fname, "d:\\r2000\\reseau", -1 ) )&&( !strnicmp( argv[0], "R:", 2 ) ) )
		{
		v = substr( src_fname, 15, -1 );
		strdcpy( &src_fname, "r:\\reseau", v, _ );
		}
		
	#endif
	
/*
	#ifdef OS_UNIX
	//----------------------------------------------------------------------
	if ( request_mode != MODE_TCP )
	//----------------------------------------------------------------------
		{
		if ( stricmp( sysuser(), ssie_user ) )
			{
			// RECHERCHE LE N° PORT DU SERVEUR SSIE
			
			v = var_get( hSrvConf, "port" );
			if ( ( !v )||( !(v[0]) ) )
				{
				System( &s, "ps -ef | grep \"ssie7 server \" | grep -v grep" );
				p = strpos( s, "ssie7 server ", 0 );
				if ( p >= 0 )
					v = itoa( atoi( &(s[p+13]) ) );
				}
			
			// ENVOI LA REQUETE
			
			strdcpy( &s1, "127.0.0.1:", v, _ );
			strdcat( &cmd, " user=", sysuser(), _ );
	
			if ( tcp_request( s1, cmd, &s ) == sERROR )
				{
				print( "ERREUR : ", error_msg, BR, _ );
				return ERROR;
				}
				
			print( s, BR, _ );
	
			return OK;
			}
		}

	#endif
*/

	//--------------------------------------------------------------------------
	// MFT SEND : GENERE UN IDA
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

	print( "IDA=", ida, BR, _ );
//	if ( log_id ) hlogc( hLog, log_id, "", "IDA=", ida, _ );
	

	//--------------------------------------------------------------------------
	// MFT SEND : INITIALISE LES FICHIERS INFO et LOG
	//--------------------------------------------------------------------------

	var_delete( hReq, "mft" );
	var_delete( hReq, "send" );
	
	var_set( hReq, "ida", ida );
	var_set( hReq, "sida", ida );
	var_set( hReq, "direct", "SEND" );
	var_set( hReq, "partname", partname );
	var_set( hReq, "rpart", partname );
	var_set( hReq, "part", partname );
	if ( ! var_get( hReq, "spart" ) )
		var_set( hReq, "spart", var_get( hSrvConf, "partname" ) );
		
	var_set( hReq, "src_fname", src_fname );
	if ( !var_get( hReq, "sfname" ) )
		var_set( hReq, "sfname", file_getname( src_fname ) );

	v = currdate( "JJ/MM/AAAA HH:MM:SS" );
	if ( !var_get( hReq, "dateb" ) ) // Pour ne pas écraser l'éventuelle date MFT
		var_set( hReq, "dateb", substr( v, 0, 10 ) );
	if ( !var_get( hReq, "timeb" ) )
		var_set( hReq, "timeb", substr( v, 11, 8 ) );
	
tr2( "cmd_mft_send:log_id=", itoa(log_id) );

	if ( log_id )
		{
		// SAUVEGARDE LES PARAMETRES DE LA LOG HIERARCHIQUE
	
		var_set( hReq, "_hlog_dir", /*hlog_getdir( hLog )*/ dirflux );
		var_set( hReq, "_hlog_id", hlog_getid( hLog ) );
		var_set( hReq, "_log_id", itoa(log_id) );
		}
	
	if (( !var_get( hReq, "ascii" ) )
		&&( v = var_get( hReq, "idf" ) ))
		{
		strdcpy( &fname, diridf, DIR_CHAR, upper(v), ".ascii", _ );
		if ( file_exists( fname ) )
			var_set( hReq, "ascii", "true" );
		}


	mftcat_logw( hReq, cmd, _ );
	

	//--------------------------------------------------------------------------
	// MFT SEND : VERIFIE QUE LE FICHIER EXISTE
	//--------------------------------------------------------------------------
	
	if ( !file_exists( src_fname ) )
		{
		strdcpy( &s, "ERREUR : le fichier à envoyer n'existe pas : ", src_fname, _ );
		mftcat_logw( hReq, s, _ );
		mftcat_crea( hReq, "W", "110", s );
		if ( log_id ) hlogc( hLog, log_id, "", s, _ );
		else print( s, _ );
		logw( "mft send : ", ida, " : ", s, _ );
		return ERROR;
		}
	
			
	//--------------------------------------------------------------------------
	// MFT SEND : VERIFIE QUE LE PARTENAIRE EXISTE
	//--------------------------------------------------------------------------
	
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
		strdcpy( &s, "ERREUR : le partenaire ", partname, " est inconnu.", _ );
		if ( log_id ) hlogc( hLog, log_id, "ERREUR", s, _ );
		mftcat_logw( hReq, s, _ );
		mftcat_crea( hReq, "K", "408", s );
		
		if ( !log_id ) print( s, BR, _ );
		return ERROR;
		}
		
	send_suite:
		
	if ( stricmp( partname, part ) ) // Utilisation d'une ADL ?
		{
		var_set( hReq, "partname", partname );
		var_set( hReq, "part", partname );
		var_set( hReq, "rpart", partname );
		strdcpy( &part, partname, _ );
		}

	if ( var_get( hReq, "ssie_mandatory" ) )
		{
		hPart = var_load( fname );
		v = var_get( hPart, "product_name" );
		if ( ( !v )||( strnicmp( v, "SSIE v7", 7 ) ) )
			{
			strdcpy( &s, "ERREUR : le partenaire ", part, " n'est pas en version SSIE v7", _ );
			if ( log_id ) hlogc( hLog, log_id, "", s, _ );
			mftcat_logw( hReq, s, _ );
			mftcat_crea( hReq, "K", "409", s );
			
			if ( !log_id ) print( s, BR, _ );
			return ERROR;
			}
		}
		
	
	// OPTION COPYFILE
	
	if ( var_get( hReq, "copyfile" ) )
		{
tr( "cmd_mft_send copyfile" );
		strdcpy( &fname, dirtemp, DIR_CHAR, ida, ".data", _ );
		
		#ifdef OS_UNIX
		if ( !strcmp( user, ssie_user ) )
			file_copy( src_fname, fname );
		else
			{
			// Copie par sudo
			strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
					"cp -f ", C_quote(src_fname), " ", fname, _ );
			System( &error_msg, s );
			strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
					"chown ", ssie_user, ":", ssie_user, " ", fname, _ );
			System( &error_msg, s );
			strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
					"chmod 700 ", fname, _ );
			System( &error_msg, s );
			}
			
		#else // WINDOWS
		file_copy( src_fname, fname );
		#endif
		
		if (( !file_exists( fname ) )||( file_size( src_fname ) != file_size( fname ) ))
			{
			file_delete( fname );
			strdcpy( &s, "ERREUR : file copy : ", src_fname, " => ", fname, _ );
			mftcat_logw( hReq, s, _ );
			mftcat_upd( hReq, "K", "115", s );
			
			if ( log_id ) hlog_free( hLog );
			return ERROR;
			}
			
		strdcpy( &s, "file copy : ", src_fname, " => ", fname, _ );
		mftcat_logw( hReq, s, _ );
		
		strdcpy( &src_fname, fname, _ );
		var_set( hReq, "faction", "delete" );
		var_set( hReq, "fname", src_fname );
		var_delete( hReq, "copyfile" );
		}
	
	//--------------------------------------------------------------------------
	// PLACE LE TRANSFERT DANS LA FILE D'ATTENTE POUR LE PARTENAIRE
	//--------------------------------------------------------------------------
	
	mftcat_crea( hReq, "C", "0", "En cours" );


	//--------------------------------------------------------------------------
	// OPTION WAITEND
	//--------------------------------------------------------------------------
	
	if ( var_get( hReq, "waitend" ) )
		{
		print( "Attend la fin du transfert ..." BR, _ );
		for( sleep(1) ; ; sleep(1) )
			{
			var_free( h );
			if ( (h = mftcat_load( ida )) == ERROR ) continue;
			v = var_get( h, "status" );
			if ( !v )
				{
				print( "ERREUR Lecture transfert" BR, _ );
				continue;
				}
			if (( v[0] != 'C' )&&( v[0] != 'D' ))
				{
				strdcpy( &cmd, argv[0], " mft log ida=", ida, _ );
				System( &s, cmd );
				print( s, BR, _ );
//				print( "status = ", ida, " ", var_get( h, "errno" ), " ", var_get( h, "errmsg" ), BR, _ );
				return OK;
				}
			}
		}

	// FIN DE LA SOUMISSION DU TRANSFERT
	//sni
	//mft_reveil_compute( t_work, "SEND" );//lancer les reveils ACR
	return OK;
	}


#undef src_fname






//------------------------------------------------------------------------------
int upgrade_run( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	char *tmp_dir=null;
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif

	if ( ( b2 == 1 )||( b2 == 2 )||( bKeepUserMersi == true ) )
		{
		if ( ! bRoot )
			{
			print( BR "ERREUR : Vous devez être sous un compte root" BR, _ );
			exit(1);
			}
/*
		System( &s, "whoami" );
		trim(s);
		if ( ( strcmp( s, "root" ) )&&( strcmp( s, "rootmin" ) ) )
			{
			print( BR "ERREUR : Vous devez être sous un compte root" BR, _ );
			exit(1);
			}
		bRoot = true;
*/		}

	if( bKeepUserMersi )
		{
		// VERIFIE QUE LE USER MERSI EXISTE
		if ( !dir_exists( "/home/mersi" ) )
			{
			print( "ERREUR : Le compte mersi doit exister" BR, _ );
			exit(1);
			}
		}
		
	strdcpy( &fname, dirlog, DIR_CHAR "ssie7up_", itod(0,"flog"), ".log", _ );
	print_to_file_and_screen( fname );
	
	
	if ( bKeepUserMersi )
		{
		print( "--------------------------------------------------------------------------------", BR, _ );
		print( "CONFIGURATION POUR FAIRE FONCTIONNER SSIE v7 SOUS LE COMPTE MERSI" , BR, BR, _ );
		
		// CHANGEMENT DE PROPRIETAIRE
		
		System( &s, "chown -R mersi:mersi " LOGICIELS "/ssie7" );
		print( "chown -R mersi:mersi " LOGICIELS "/ssie7" BR, s, BR, _ );
		System( &s, "chown -R mersi:mersi /appli/ssie7" );
		print( "chown -R mersi:mersi /appli/ssie7" BR, s, BR, _ );
		
		// CHANGEMENT SUDOERS ???
		
		// MODIF $dirconf/ssie7.dat
		}
	
	
	if ( b2 == 1 )
		{
		print( "--------------------------------------------------------------------------------", BR, _ );
		print( "MIGRATION MERSI v3 => " SSIE_VERSION , BR, BR, _ );
		
		if ( process_count( "mersi3 server" ) > 0 )
			{
			print( BR BR, "ATTENTION : arrêtez mersi v3 avant de réaliser cette opération." BR BR,	_ );
			print( "--------------------------------------------------------------------------------", BR, BR, _ );
			return;
			}
		}
	else
	if ( b2 == 2 )
		{
		print( "--------------------------------------------------------------------------------", BR, _ );
		print( "MIGRATION MFT 2.4.1 => " SSIE_VERSION , BR, BR, _ );
		
		if ( process_count( "MFTMAIN" ) > 0 )
			{
			print( BR BR, "ATTENTION : arrêtez MFT avant de réaliser cette opération." BR BR,	_ );
			print( "--------------------------------------------------------------------------------", BR, BR, _ );
			return;
			}
		}
	else
		{
		print( "--------------------------------------------------------------------------------", BR, _ );
		print( "MIGRATION ", u15, " => " SSIE_VERSION , BR, BR, _ );
		}
		
	
	#ifdef OS_UNIX
	if (( process_count( "ssie7 server" ) > 0 )
		||( process_count( "ssie7 robot" ) > 0 ))
	#else
	if ( process_count( "ssie7" EXE ) > 0 )
	#endif
		{
		print( BR BR, "ATTENTION : arrêtez ssie v7 avant de réaliser cette opération." BR BR,	_ );
		print( "--------------------------------------------------------------------------------", BR, BR, _ );
		return;
		}
	
	if ( bRoot )
		{
		#ifdef OS_AIX
		strdcpy( &cmd, "USER=", ssie_user, _ );
		putenv( cmd );
		strdcpy( &cmd, "LOGIN=", ssie_user, _ );
		putenv( cmd );
		#else
		strdcpy( &cmd, "LOGNAME=", ssie_user, _ );
		putenv( cmd );
		#endif
		}
	

	mkdir( SAUVE_DIR );
	if ( !dir_exists( SAUVE_DIR ) )
		{
		print( "ERREUR : Problème pour créer le répertoire " SAUVE_DIR BR BR, _ );
		print( "--------------------------------------------------------------------------------", BR, BR, _ );
		exit(2);
		}
	
	
	if ( stricmp( u15, SSIE_VERSION ) )
		{
		// Extraction du fichier ssie7
		
		#ifdef OS_WINDOWS
		if ( stripos( argv[0], ".exe", 0 ) < 0 )
			{
			strdcpy( &fname1, argv[0], ".exe", _ );
			autoextract_extract( fname1, "ssie_v_7.2.a" EXE, dirbin );
			}
		else
		#endif
		autoextract_extract( argv[0], "ssie_v_7.2.a" EXE, dirbin );
		strdcpy( &fname1, dirbin, "/ssie_v_7.2.a" EXE, _ );
		if ( !file_exists( fname1 ) )
			{
			print( "ERREUR : Problème pour extraire le fichier ssie_v_7.2.a" EXE BR, fname1, BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, BR, _ );
			exit(2);
			}
		

	//sni add begin 13/12/17
	//// Extraction du timeout
	#ifdef OS_LINUX
	autoextract_extract( argv[0], "timeout", dirbin );
	System( &s, "chmod 755 " BIN_DIR DIR_CHAR "timeout" );
	//autoextract_extract( argv[0], "cpulimit", dirbin );
        //System( &s, "chmod 755 " BIN_DIR DIR_CHAR "cpulimit" );
	
	#endif
	////sni add end
		// Sauvegarde de la version actuelle dans /appli/ssie7/data/wrk/sauvegardes
		
		strdcpy( &fname2, dirbin, DIR_CHAR "ssie7" EXE, _ );
		if ( file_exists( fname2 ) )
			{
			unlink( SAUVE_DIR DIR_CHAR "ssie7" EXE );
			file_move( fname2, SAUVE_DIR "/ssie7" EXE );
//			strdcpy( &cmd, "mv -f " LOGICIELS "/ssie7/ssie7 /appli/ssie7/data/wrk/sauvegardes", _ );
//			system( cmd );
			if ( !file_exists( SAUVE_DIR DIR_CHAR "ssie7" EXE ) )
				{
				print( "ERREUR : Problème pour sauvegarder la version actuelle dans " SAUVE_DIR BR BR, _ );
				print( "--------------------------------------------------------------------------------", BR, BR, _ );
				exit(2);
				}
			}
		
		// Mise en place de la nouvelle version
		
		strdcpy( &fname2, dirbin, DIR_CHAR "ssie7" EXE, _ );
//		file_copy( fname1, BIN_DIR DIR_CHAR "ssie7" EXE );
		file_copy( fname1, fname2 );
		file_delete( fname1 );
		if ( !file_exists( fname2 ) )
			{
			print( "ERREUR : Problème pour installer la version actuelle" BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, BR, _ );
			file_move( SAUVE_DIR DIR_CHAR "ssie7" EXE, fname2 );
//			strdcpy( &cmd, "mv -f /appli/ssie7/data/wrk/sauvegardes/ssie7 " LOGICIELS "/ssie7/ssie7" , _ );
//			system( cmd );
			exit(2);
			}
	
		#ifdef OS_UNIX
		system( "chmod 755 " LOGICIELS "/ssie7/ssie7" );
		#endif
		}
	
	
	
	// MIGRATION DU CATALOGUE MFT
	
	if ( stripos( u15, "v7.1", 0 ) > 0 )
		{
		print( "MIGRATION CATALOGUE SSIE v7.1 => v7.2 :" BR, _ );

		strdcpy( &fname1, dirdat, DIR_CHAR "wrk", _ );
		strdcpy( &fname2, dirdat, DIR_CHAR "wrk71", _ );
		if ( ! dir_exists( fname2 ) )
			{
			file_move( fname1, fname2 );
			print( "rename ", fname1, " => ", fname2, BR, _ );
			}
		strdcat( &fname1, DIR_CHAR "tmp", _ );
		mkdir( fname1 );
	
	
		hMftCat = mftcat_open();
		
		//------------------------------------------------------------------------------
		// PARTENAIRES EN MODE SEND ONLY
		//------------------------------------------------------------------------------
		
		strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR "*.dat", _ );
		tf = NULL;
		while( tf = file_list( tf, fname, "nodir absdir" ) )
			{
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			var_free( hPart );
			hPart = var_load( fname );
			
			// Retient uniquement les partenaires avec trf_direction=send
			v = var_get( hPart, "trf_direction" );
			if ( ( !v )||( strcmp( v, "send" ) ) ) continue;
			v = var_get( hPart, "enable" );
			if ( ( !v )||( strcmp( v, "Oui" ) ) ) continue;
			v = var_get( hPart, "product_name" );
			
			strd_replace( &fname, ".dat", ".send" );
			file_save_str( fname, "", 0 );
			}
	
	
		//------------------------------------------------------------------------------
		// FLUX EN COURS
		//------------------------------------------------------------------------------
		
		
		strdcpy( &fname, dirdat, DIR_CHAR "wrk71" DIR_CHAR "rflux" DIR_CHAR "*", _ );
		tf = NULL;
		while( tf = file_list( tf, fname, "nodir absdir" ) )
			{
			if ( ! is_numeric( tf->name ) ) continue;
			strdcpy( &fname, dirflux, DIR_CHAR, tf->name, ".run", _ );
			file_save_str( fname, "", 0 );
			}
	
	
		
		//------------------------------------------------------------------------------
		// SAUVEGARDE CATALOGUE MFTCAT v7.1
		//------------------------------------------------------------------------------
		
		strdcpy( &fname1, dirdat, DIR_CHAR "mftcat", _ );
		strdcpy( &fname2, dirdat, DIR_CHAR "mftcat71", _ );
		if ( ! dir_exists( fname2 ) )
			{
			file_move( fname1, fname2 );
			print( "rename ", fname1, " => ", fname2, BR, _ );
			}
		else
			file_delete( fname1 );	
		mkdir( fname1 );
	
		strdcpy( &fname1, dirdat, DIR_CHAR "recv", _ );
		strdcpy( &fname2, dirdat, DIR_CHAR "recv71", _ );
		if ( ! dir_exists( fname2 ) )
			{
			file_move( fname1, fname2 );
			print( "rename ", fname1, " => ", fname2, BR, _ );
			mkdir( fname1 );
			strdcat( &fname2, DIR_CHAR "*.data", _ );
			print( "file_copy ", fname2, " => ", fname1, BR, _ );
			file_copy( fname2, fname1 );
			}
		
		strdcpy( &fname1, dirdat, DIR_CHAR "send", _ );
		strdcpy( &fname2, dirdat, DIR_CHAR "send71", _ );
		if ( ! dir_exists( fname2 ) )
			{
			file_move( fname1, fname2 );
			print( "rename ", fname1, " => ", fname2, BR, _ );
			}
		
	
		n5 = 0;
		n1 = time(null);
	
n4 = 0; // Offset jour pour les tests
		
		strdcpy( &fname, dirdat, DIR_CHAR "send71" DIR_CHAR "*.info", _ );
		tf = NULL;
		while( tf = file_list( tf, fname, "absdir nodir" ) )
			{
			n5++;
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );

//print( fname, BR, __ );
	
			var_free( h );
			h = var_load( fname );
			
			var_set( h, "direct", "SEND" );
			
			v=var_get( h, "dt_last_access" );
			if ( !v )
				v = itoa( file_date( fname ) );
				
			var_set( h, "day_upd",
					itod(atoi(v) + (n4 Jours),
					"aaaa_mm_jj")
				);
			var_set( h, "dt_upd", itod(atoi(v), "flog") );
//	var_set( h, "dt_upd", itod(atoi(v) + (n4 Jours), "flog") );
	
//			v = var_get( h, "dt_start" );
//			var_set( h, "dt_crea", itod(atoi(v), "flog") );
//	var_set( h, "dt_crea", itod(atoi(v) + (n4 Jours), "flog") );
			
			v = var_get( h, "diagi" );
			if ( !v ) v = "0";
			
			var_set( h, "errno", v );
			
			v1 = var_get( h, "status" );
			if ( !v1 ) v1 = "ERREUR";

			v2 = var_get( h, "error_msg" );
			if ( !v2 ) v2 = v1;
			
			var_set( h, "errmsg", v2 );
	
			if ( !strnicmp( v1, "OK", 2 ) )
				var_set( h, "status", "T" );
			else
			if ( !strcmp( v, "415" ) )
				{
				var_set( h, "status", "K" );
				var_set( h, "errmsg", "TIMEOUT" );
				}
			else
				{
				if ( strcmp( v1, "X" ) )
					{
					if ( !strncmp( v2, "ERR", 3 ) )
						var_set( h, "status", "K" );
					else
						{
						v = var_get( h, "dt_last_access" );
						if ( (time(null)-atoi(v)) > 2 Jours )
							{
							var_set( h, "status", "K" );
							var_set( h, "errmsg", "TIMEOUT" );
							}
						else
							var_set( h, "status", "C" );
						}
					}
//				else
//					var_set( h, "status", "X" );
				}
	
	
			var_delete( h, "dt_start" );
			var_delete( h, "diagi" );
			var_delete( h, "error_msg" );
			var_delete( h, "dt_last_access" );
			var_delete( h, "hist_fname" );
			var_delete( h, "mft" );
			var_delete( h, "send" );
			var_delete( h, "nocopy" );
			var_delete( h, "adle" );
			var_delete( h, "appe" );
			var_delete( h, "adlr" );
			var_delete( h, "appr" );
			var_delete( h, "transfert_en_cours" );
			var_delete( h, "faction" );
	
			
			strcpy( &(fname[strlen(fname)-4]), "log" );
			file_load( &s, fname );
			var_set( h, "mft_log", s );
	
/*
print( "bd_insert : " BR, __ );
var_print( h, BR );
print( BR BR, _ );
*/
			
			if ( bd_insert( hMftCat, h, null ) == ERROR )
				{
				print( error_msg, BR BR, _ );
				pause();
				}
				
			}
	
		
		strdcpy( &fname, dirdat, DIR_CHAR "recv71" DIR_CHAR "*.info", _ );
		tf = NULL;
		while( tf = file_list( tf, fname, "absdir nodir" ) )
			{
			n5++;
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
//print( fname, BR, __ );
	
			var_free( h );
			h = var_load( fname );
			
			var_set( h, "direct", "RECV" );
			v = var_get( h, "rida" );
			var_set( h, "ida", v );
			
			v1 = var_get( h, "part" );
			v2 = var_get( h, "spart" );
	
	
			if (( v2 )&&( v2[0] )&&( v1 ))
				{
				if ( stricmp( v1, v2 ) )
					var_set( h, "part", v2 );
				}
			
			v = var_get( h, "dt_last_access" );
			if ( !v )	v = itoa( file_date( fname ) );
			var_set( h, "day_upd", itod(atoi(v), "aaaa_mm_jj") );
			var_set( h, "dt_upd", itod(atoi(v), "flog") );
	
			v = var_get( h, "dt_start" );
			var_set( h, "dt_crea", itod(atoi(v), "flog") );
			
			v = var_get( h, "diagi" );
			if ( !v ) v = "0";
			var_set( h, "errno", v );
			
			
			v1 = var_get( h, "status" );
			if ( !v1 ) v1 = "ERREUR";
			
			v2 = var_get( h, "error_msg" );
			if ( !v2 ) v2 = v1;
			
			var_set( h, "errmsg", v2 );
	
			if ( !strnicmp( v1, "OK", 2 ) )
				var_set( h, "status", "T" );
			else
			if ( !strcmp( v, "415" ) )
				{
				var_set( h, "status", "K" );
				var_set( h, "errmsg", "TIMEOUT" );
				}
			else
				{
				if ( strcmp( v1, "X" ) )
					{
					if ( !strncmp( v2, "ERR", 3 ) )
						var_set( h, "status", "K" );
					else
						var_set( h, "status", "C" );
					}
//				else
//					var_set( h, "status", "X" );
				}
	
	
			var_delete( h, "dt_start" );
			var_delete( h, "diagi" );
			var_delete( h, "error_msg" );
			var_delete( h, "dt_last_access" );
			var_delete( h, "hist_fname" );
			var_delete( h, "mft" );
			var_delete( h, "send" );
			var_delete( h, "nocopy" );
			var_delete( h, "adle" );
			var_delete( h, "appe" );
			var_delete( h, "adlr" );
			var_delete( h, "appr" );
			var_delete( h, "transfert_en_cours" );
			var_delete( h, "faction" );
			
			strcpy( &(fname[strlen(fname)-4]), "log" );
			file_load( &s, fname );
			var_set( h, "mft_log", s );
			
/*
print( "bd_insert : " BR, __ );
var_print( h, BR );
print( BR BR, _ );
*/
			
			if ( bd_insert( hMftCat, h, null ) == ERROR )
				{
				print( error_msg, BR BR, _ );
				pause();
				}
			}
		
		print( BR BR, itoa(n5), " Transferts migrés", BR, _ );
	
		strdcpy( &fname1, dirwrk, "71", DIR_CHAR "ida.id", _ );
		strdcpy( &fname2, dirdat, DIR_CHAR "mftcat", DIR_CHAR "ida.id", _ );
		file_copy( fname1, fname2 );
		if ( ! file_exists( fname2 ) )
			{
			print( "ERREUR : file_copy ", fname1, " => ", fname2, BR, _ );
			}
		else
			print( "file_copy ", fname1, " => ", fname2, " : OK", BR, _ );


		print( "MIGRATION CATALOGUE TERMINEE" BR BR, _ );
		

		#ifdef OS_UNIX
		// Extraction du fichier mft et mftcat
		
		autoextract_extract( argv[0], "mft", dirbin );
		System( &s, "chmod 755 " BIN_DIR DIR_CHAR "mft" );
		autoextract_extract( argv[0], "mftcat", dirbin );
		System( &s, "chmod 755 " BIN_DIR DIR_CHAR "mftcat" );
		
		#endif	// fin os_unix		

		//--------------------------------------------------------------------------
		// ADAPTATION restart_onrecv => one_by_one
		
		strdcpy( &fname, dirconf, DIR_CHAR "flux" DIR_CHAR "*", _ );
		tf = NULL;
		while( tf = file_list( tf, fname, "nodir absdir" ) )
			{
			if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			
			var_free( h1 );
			h1 = var_load( fname );
			if ( ( v = var_get( h1, "agr_restart_onrecv" ) ) )
				{
				var_set( h1, "agr_one_by_one", v );
				var_delete( h1, "agr_restart_onrecv" );
				var_save( h1, fname );
				}
			} 
		}
		

	#ifdef OS_UNIX
	if (( file_exists( "/logiciels/ssie7/mft/MFTMAIN" ) )
		||( file_exists( "/logiciels/ssie7/mft" ) ))
		{
		if ( dir_exists( "/logiciels/ssie7/mftbin" ) )
			{
			file_move( "/logiciels/ssie7/mft", "/logiciels/ssie7/mftbin" );
			if ( ! dir_exists( "/logiciels/ssie7/mftbin" ) )
				{
				print( "ERREUR : Problème pour renommer le répertoire mft en mftbin" EXE BR BR, _ );
				print( "--------------------------------------------------------------------------------", BR, BR, _ );
				exit(2);
				}
			}
		
		// Extraction du fichier mft
		
		autoextract_extract( argv[0], "mft_v_7.2.a" EXE, dirbin );
		strdcpy( &fname1, dirbin, "/mft_v_7.2.a" EXE, _ );
		if ( !file_exists( fname1 ) )
			{
			print( "ERREUR : Problème pour extraire le fichier mft_v_7.2.a" EXE BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, BR, _ );
			exit(2);
			}
		
		file_move( "/logiciels/ssie7/mft_v_7.2.a", "/logiciels/ssie7/mft" );
		if ( !file_exists( "/logiciels/ssie7/mft" ) )
			{
			print( "ERREUR : Problème pour installer le programme mft" EXE BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, BR, _ );
			exit(2);
			}
		
		System( &s, "chmod 700 /logiciels/ssie7/mft" );
		
		print( "INSTALLATION nouvelle version du programme mft : OK" BR, _ );
		}
	
	#endif
	
	#ifdef OS_WINDOWS
	
	// Extraction T0_SSIE
	
	strdcpy( &fname1, dirbin, DIR_CHAR "T0_SSIE.exe", _ );
	strdcpy( &fname2, SAUVE_DIR DIR_CHAR "T0_SSIE.exe", _ );
	file_move( fname1, fname2 );
	if ( file_exists( fname1 ) )
		{
		print( "ERREUR : Problème sauvegade T0_SSIE" EXE BR BR, _ );
		print( "--------------------------------------------------------------------------------", BR, BR, _ );
		exit(2);
		}
	if ( stripos( argv[0], ".exe", 0 ) < 0 )
		{
		strdcpy( &fname, argv[0], ".exe", _ );
		autoextract_extract( fname, "T0_SSIE.exe", dirbin );
		}
	else
	autoextract_extract( argv[0], "T0_SSIE.exe", dirbin );
	if ( ! file_exists( fname1 ) )
		{
		print( "ERREUR : Problème installation T0_SSIE" EXE BR BR, _ );
		print( "--------------------------------------------------------------------------------", BR, BR, _ );
		exit(2);
		}

	strdcpy( &fname1, dirbin, DIR_CHAR "bin" DIR_CHAR "T0_SSIE_TSP.exe", _ );
	if ( file_exists( fname1 ) )
		{
		strdcpy( &fname2, SAUVE_DIR DIR_CHAR "T0_SSIE_TSP.exe", _ );
		file_move( fname1, fname2 );
		if ( file_exists( fname1 ) )
			{
			print( "ERREUR : Problème sauvegade T0_SSIE_TSP" EXE BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, BR, _ );
			exit(2);
			}
		strdcpy( &fname3, dirbin, DIR_CHAR "bin", _ );
		if ( stripos( argv[0], ".exe", 0 ) < 0 )
			{
			strdcpy( &fname, argv[0], ".exe", _ );
			autoextract_extract( fname, "T0_SSIE_TSP.exe", fname3 );
			}
		else
		autoextract_extract( argv[0], "T0_SSIE_TSP.exe", fname3 );
		if ( ! file_exists( fname1 ) )
			{
			print( "ERREUR : Problème installation T0_SSIE_TSP" EXE BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, BR, _ );
			exit(2);
			}
		}
	
	#endif

	#ifdef OS_UNIX
	if ( b2 == 1 )
		migration_mersi3( t_work );
	else
	if ( b2 == 2 )
		migration_mft( t_work );
	else
	#endif
	
//	if (( stripos( u15, "v7.2.a.", 0 ) < 0 )&&( stripos( u15, "v7.2.a.1", 0 ) < 0 ))
// Pour les prochains patch, Mettre la ligne précédente plutôt celle suivante, mais à retester.

//	if (( stripos( u15, "v7.2.a.2", 0 ) < 0 )&&( stripos( u15, "v7.2.a.3", 0 ) < 0 ))
	if (( stripos( u15, "v7.2.a.", 0 ) < 0 )&&( stripos( u15, "v7.2.a.1", 0 ) < 0 ))
		{
		//------------------------------------------------------------------------------
		// MIGRATION CATALOGUE DES FLUX v7.1 ou 7.2.a
		//------------------------------------------------------------------------------
		
		print( "MIGRATION HISTORIQUE DES FLUX ", u15, " => " SSIE_VERSION BR BR, _ );
		
		elib_log1 = (t_elib_log1*)alloc0( sizeof( t_elib_log1 ) );
		elib_log  = (t_elib_log*)alloc0( sizeof( t_elib_log ) );
	
		strdcpy( &fname4, dirflux, ".new", _ );

		mkdir( fname4 );
		print( "mkdir ", fname4, BR, _ );
		
		strdcpy( &fname1, dirflux, DIR_CHAR "*.dat", _ );
		file_copy( fname1, fname4 );
		print( "file_copy ", fname1, " ", fname4, BR, _ );
		strdcpy( &fname1, dirflux, DIR_CHAR "*.run", _ );
		file_copy( fname1, fname4 );
		print( "file_copy ", fname1, " ", fname4, BR, _ );
		
		strdcpy( &fname, dirflux, DIR_CHAR "*.log", _ );
		tf = NULL;
		while( tf = file_list( tf, fname, "absdir nodir" ) )
			{
			print( tf->name, BR, _ );
			strdcpy( &fname1, tf->dir, DIR_CHAR, tf->name, _ );
			n = file_size( fname1 );
			if (( !n )||( n % sizeof( t_elib_log1 ) )) continue;
				
			fp = fopen( fname1, "rb" );
			if ( !fp )
				{
				print( "ERREUR : impossible d'ouvrir le fichier : ", fname1, BR, _ );
				continue;
				}
			
			p = strpos( tf->name, ".log", 0 );
			tf->name[p] = 0;
			strdcpy( &fname, fname4, DIR_CHAR "log.id", _ );
			set_uid( fname, atoi(tf->name)-1 );
			
			hLog = hlog_init( fname4, null );
			if ( hLog == ERROR )
				{
				print( "ERREUR hlog_init( ", fname4, ", ", tf->name, " )" BR, _ );
				break;
				}
				
			if ( w ) { free( w ); w = null; }

			log_id = 0;
			while( (n=fread( fp, (char*)elib_log1, sizeof( t_elib_log1 ) )) > 0 )
				{
				if ( !log_id )
					{
					hlogu( hLog, log_id, elib_log1->status, elib_log1->log, _ );
					}
				else
					{
					if (( elib_log1->log[0] != '@' )||( !file_exists( &(elib_log1->log[1]) ) ))
						v = elib_log1->log;
					else
						v = file_load( &s, &(elib_log1->log[1]) );
					log_id = hlogc( hLog, elib_log1->parent_id, elib_log1->status, v, _ );
					}
				
				hlogu_date( hLog, log_id, dtoi( elib_log1->when, "log" ) );
				log_id++;
				}
			
			fclose( fp );
			
			// Copie le fichier flux_id.dat
			strcpy( &(fname1[strlen(fname1)-3]), "dat" );
			file_copy( fname1, fname4 );
			print( "file_copy ", fname1, " ", fname4, BR, _ );
			}

		strdcpy( &fname1, dirflux, ".new" DIR_CHAR "log.id", _ );
		print( "file_delete ", fname1, BR, _ );

		file_delete( fname1 );
		strdcpy( &fname1, dirflux, DIR_CHAR "log.id", _ );
		file_copy( fname1, fname4 );
		print( "file_copy ", fname1, " ", fname4, BR, _ );
		
		strdcpy( &fname1, dirflux, ".old", _ );
		file_move( dirflux, fname1 );
		print( "file_move ", dirflux, " ", fname1, BR, _ );

		strdcpy( &fname1, dirflux, ".new", _ );
		file_move( fname1, dirflux );
		print( "file_move ", fname1, " ", dirflux, BR, _ );

//strdcpy( &fname, dirflux, DIR_CHAR "log.id", _ );
//print( "LastID=", itoa(getuid( fname )), BR, _ );


		strdcpy( &fname, dirflux, DIR_CHAR "*.log.*.RUN", _ );
		tf = NULL;
		while( tf = file_list( tf, fname, "absdir nodir" ) )
			{
			strdcpy( &fname, dirflux, DIR_CHAR, itoa( atoi( tf->name ) ), ".run", _ );
			if ( !file_exists( fname ) )
				{
				print( "run : ", fname, BR, _ );
				file_save_str( fname, "", 0 );
				}
			}
		}

	
	#ifdef OS_UNIX
	
	// MIGRATION PEPSIS
	
	if ( bMigrPEPSIS )
		migr_pepsis( t_work );
	
	
	// MISE A JOUR CONFIG SERVEUR SI KeepUserMersi et/ou PEPSIS
	
	if (( bKeepUserMersi )||( bMigrPEPSIS ))
		{
		// Mise à jour .bash_profile ou .profile
		print( "   -> Mise à jour de /home/mersi/.bash_profile" BR, _ );
		strdcpy( &fname, "/home/mersi/.bash_profile", _ );
		if ( !file_exists( fname ) )
			strdcpy( &fname, "/home/mersi/.profile", _ );
		if ( file_exists( fname ) )
			{
			file_load( &s, fname );
			strd_replace( &s, "/mersi3", "/ssie7" );
			strd_replace( &s, "mersi3 ", "ssie7 " );
			strd_replace( &s, "mersi3;", "ssie7;" );
			strd_replace( &s, "mersi3'", "ssie7'" );
			strd_replace( &s, "mersi3'", "ssie7'" );
			file_save_str( fname, s, -1 );
			}
		
		// Mise à jour config serveur
		strdcpy( &fname, dirconf, DIR_CHAR "ssie7.dat", _ );
		h = var_load( fname );
		var_set( h, "ssie_user", ssie_user );
		var_set( h, "ancien_mft", "Oui" );
		if ( bMigrPEPSIS )	var_set( h, "est_PEPSIS", "Oui" );
		var_save( h, fname );
		
		// Mise à jour sudoers
		
		b = false;

		strdcpy( &fname, "/etc/sudoers.d/ssie", _ );
		if ( file_exists( fname ) )
			{
			file_load( &s, fname );
			if ( strpos( s, "User_Alias OP_SSIE = ssie", 0 ) >= 0 )
				{
				strd_replace( &s, "User_Alias OP_SSIE = ssie", "User_Alias OP_SSIE = mersi" );
				strd_replace( &s, "Defaults:ssie", "Defaults:mersi" );
				b = true;
				}
			else
			if( strpos( s, "User_Alias OP_SSIE = mersi", 0 ) >= 0 )
				b = true;
			}
		
		if ( !b )
			{	
			strdcpy( &fname, "/etc/sudoers", _ );
			if ( file_exists( fname ) )
				{
				file_load( &s, fname );
				if ( strpos( s, "User_Alias OP_SSIE = ssie", 0 ) >= 0 )
					{
					strd_replace( &s, "User_Alias OP_SSIE = ssie", "User_Alias OP_SSIE = mersi" );
					strd_replace( &s, "Defaults:ssie", "Defaults:mersi" );
					b = true;
					}
				else
				if ( strpos( s, "User_Alias OP_SSIE = mersi", 0 ) < 0 )
					{
					strdcat( &s, BR
						"Host_Alias SERV_LOCAL = localhost" BR
						"Cmnd_Alias CMD_SSIE7 = " LOGICIELS "/ssie7/ssie7" BR
						"User_Alias OP_SSIE = mersi" BR
						"OP_SSIE SERV_LOCAL = NOPASSWD:CMD_SSIE7" BR
						"Defaults:mersi !requiretty" BR
						, _ );
					b = true;
					}
				}
			}
		
		if ( b )
			{
			print( "   -> Mise à jour de ", fname, BR, _ );
			// Sauvegarde du fichier sudoers
			strdcpy( &cmd, "chmod 740 ", fname, _ );
			System( &s1, cmd );
			file_save_str( fname, s, -1 );
			strdcpy( &cmd, "chmod 440 ", fname, _ );
			System( &s1, cmd );
			}
		
		if ( bKeepUserMersi )
			{
			strdcpy( &fname, "/etc/init.d/ssie7", _ );
			if ( file_exists( fname ) )
				{
				print( "   -> Mise à jour de ", fname, BR, _ );
				file_load( &s, fname );
				strd_replace( &s, "su - ssie -c", "su - mersi -c" );
				file_save_str( fname, s, -1 );
				}
			}
		
		}
	
	// MET LES DROITS SSIE
	
	if ( bRoot )
		{
		strdcpy( &cmd, "chown -R ", ssie_user, ":", ssie_user, " /appli/ssie7", _ );	
		System( &s, cmd );
//		strdcpy( &cmd, "chown -R ", ssie_user, ":", ssie_user, " /logiciels/ssie7", _ );	
		strdcpy( &cmd, "chown -R ", ssie_user, ":", ssie_user, " " LOGICIELS "/ssie7", _ );	
		System( &s, cmd );
/*
		if ( bMigrPEPSIS )
			{
			strdcpy( &cmd, "chown -R ", ssie_user, ":", ssie_user, " /home/mersi/script", _ );	
			System( &s, cmd );
			}
*/
		}
		
	System( &s, "chmod -R 750 /appli/ssie7" );
	// ATTENTION /appli/ssie7/data/temp doit être accessible à tous
	System( &s, "chmod 755 /appli/ssie7" );
	System( &s, "chmod 755 /appli/ssie7/data" );
	System( &s, "chmod 777 /appli/ssie7/data/temp" );
	
	#endif
	
	// FIN

	
	print( "MIGRATION TERMINEE." BR, _ );
	print( "--------------------------------------------------------------------------------", BR, BR, _ );
	
	return OK;
	}


















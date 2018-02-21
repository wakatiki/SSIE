
//------------------------------------------------------------------------------
int upgrade_back( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	char key;

	if ( ( b2 == -1 )||( b2 == -2 ) )
		{
		System( &s, "whoami" );
		trim( s );
		if ( ( strcmp( s, "root" ) )&&( strcmp( s, "rootmin" ) ) )
			{
			print( "ERREUR : Vous devez être sous un compte root" BR, _ );
			exit(1);
			}
		bRoot = true;
		}

	if (( b2 == -1 )&&( !file_exists( SAUVE_DIR "/mersi3" ) ))
		b2 = false;
	else
	if (( b2 == -2 )&&( !file_exists( SAUVE_DIR "/MFTMAIN" ) ))
		b2 = false;
	
	strdcpy( &fname, dirlog, DIR_CHAR "ssie7up_", itod(0,"flog"), ".log", _ );
	mkdir( dirlog );
	print_to_file_and_screen( fname );
	
	print( "--------------------------------------------------------------------------------", BR, _ );

	if ( b2 == -1 )
		{
		print( "RETOUR ARRIERE VERS MERSI v3", _ );
/*
		if ( u14 )
			print( " et ", u14, BR, BR, _ );
		else
*/
			print( BR, BR, _ );
		}
	else
	if ( b2 == -2 )
		{
		print( "RETOUR ARRIERE VERS MFT v2.4.1", _ );
/*
		if ( u14 )
			print( " et ", u14, BR, BR, _ );
		else
*/
			print( BR, BR, _ );
		}
	else
		print( "RETOUR ARRIERE VERS ", u14, BR, BR, _ );

	if ( ! bMuet )
		{
		print( "Veuillez confirmer votre demande (o/n) : ", _ );
		strdcpy( &s, " ", _ );
		key = getchar();
		if ( key != 'o' )
			{
			print( BR "ABANDON." BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, _ );
			exit(2);
			}
		}
	
	#ifdef OS_UNIX
 	if (( process_count( "ssie7 server" ) > 0 )
		||( process_count( "ssie7 robot" ) > 0 ))
		{
		print( BR BR, "ATTENTION : arrêtez ssie v7 avant de réaliser cette opération." BR BR, _ );
		print( "--------------------------------------------------------------------------------", BR, _ );
		exit(2);
		}
	
	if ( b2 == -1 )
		{
		if ( process_count( "mersi3 server" ) > 0 )
			{
			print( BR BR, "ATTENTION : arrêtez mersi v3 avant de réaliser cette opération." BR BR,	_ );
			print( "--------------------------------------------------------------------------------", BR, BR, _ );
			return;
			}
		}
	else
	if ( b2 == -2 )
		{
		if ( process_count( "MFTMAIN" ) > 0 )
			{
			print( BR BR, "ATTENTION : arrêtez MFT avant de réaliser cette opération." BR BR,	_ );
			print( "--------------------------------------------------------------------------------", BR, BR, _ );
			return;
			}
		}
	
	#else // OS_WINDOWS

 	if ( process_count( "ssie7" EXE ) > 0 )
		{
		print( BR BR, "ATTENTION : arrêtez ssie v7 avant de réaliser cette opération." BR BR, _ );
		print( "--------------------------------------------------------------------------------", BR, _ );
		exit(2);
		}
	
	#endif
	
	if ( !b2 )
	{
	if ( file_exists( SAUVE_DIR DIR_CHAR "ssie7" EXE ) )
		{
		strdcpy( &fname2, dirbin, DIR_CHAR "ssie7" EXE, _ );
		file_move( SAUVE_DIR DIR_CHAR "ssie7" EXE, fname2 );
		#ifdef OS_UNIX
		System( &s, "chmod 755 " BIN_DIR DIR_CHAR "ssie7" EXE );
		#endif
		
		if ( file_exists( SAUVE_DIR DIR_CHAR "ssie7" EXE ) )
			{
			print( "ERREUR : Pb recupération de l'ancien binaire ssie7" BR
					 SAUVE_DIR DIR_CHAR "ssie7" EXE " => ", fname2, BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, _ );
			exit(3);
			}
		print( "RECUPERATION ancien binaire SSIE v7 : OK" BR BR, _ );
		print( "--------------------------------------------------------------------------------", BR, _ );
		}

	#ifdef OS_WINDOWS
	if ( file_exists( SAUVE_DIR DIR_CHAR "T0_SSIE" EXE ) )
		{
		strdcpy( &fname2, dirbin, DIR_CHAR "T0_SSIE" EXE, _ );
		file_move( SAUVE_DIR DIR_CHAR "T0_SSIE" EXE, fname2 );
		
		if ( file_exists( SAUVE_DIR DIR_CHAR "T0_SSIE" EXE ) )
			{
			print( "ERREUR : Pb recupération de l'ancien binaire T0_SSIE.exe" BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, _ );
			exit(3);
			}
		print( "RECUPERATION ancien binaire T0_SSIE : OK" BR BR, _ );
		print( "--------------------------------------------------------------------------------", BR, _ );
		}

	if ( file_exists( SAUVE_DIR "/T0_SSIE_TSP" EXE ) )
		{
		strdcpy( &fname2, dirbin, DIR_CHAR "bin" DIR_CHAR "T0_SSIE_TSP" EXE, _ );
		file_move( SAUVE_DIR "/T0_SSIE_TSP" EXE, fname2 );
		
		if ( file_exists( SAUVE_DIR DIR_CHAR "T0_SSIE_TSP" EXE ) )
			{
			print( "ERREUR : Pb recupération de l'ancien binaire T0_SSIE_TSP.exe" BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, _ );
			exit(3);
			}
		print( "RECUPERATION ancien binaire T0_SSIE_TSP : OK" BR BR, _ );
		print( "--------------------------------------------------------------------------------", BR, _ );
		}
	#endif

	if ( stripos( u14, "v7.1", 0 ) >= 0 )
		{
		strdcpy( &fname, dirdat, DIR_CHAR "wrk71", _ );
		if ( dir_exists( fname ) )
			{
			strdcpy( &fname1, dirdat, DIR_CHAR "wrk", _ );
			file_delete( fname1 );
			if ( dir_exists( fname1 ) )
				{
				print( "ERREUR : Pb recupération répertoire wrk" BR BR, _ );
				print( "--------------------------------------------------------------------------------", BR, _ );
				}
			else
				{
				file_move( fname, fname1 );
				if ( !dir_exists( fname1 ) )
					{
					print( "ERREUR : Pb recupération répertoire wrk" BR BR, _ );
					print( "--------------------------------------------------------------------------------", BR, _ );
					}
				}
			}
		
		strdcpy( &fname, dirdat, DIR_CHAR "mftcat71", _ );
		if ( dir_exists( fname ) )
			{
			strdcpy( &fname1, dirdat, DIR_CHAR "mftcat", _ );
			file_delete( fname1 );
			if ( dir_exists( fname1 ) )
				{
				print( "ERREUR : Pb recupération répertoire mftcat" BR BR, _ );
				print( "--------------------------------------------------------------------------------", BR, _ );
				}
			else
				{
				file_move( fname, fname1 );
				if ( !dir_exists( fname1 ) )
					{
					print( "ERREUR : Pb recupération répertoire mftcat" BR BR, _ );
					print( "--------------------------------------------------------------------------------", BR, _ );
					}
				}
			}
		
		strdcpy( &fname, dirdat, DIR_CHAR "recv71", _ );
		if ( dir_exists( fname ) )
			{
			strdcpy( &fname1, dirdat, DIR_CHAR "recv", _ );
			file_delete( fname1 );
			if ( dir_exists( fname1 ) )
				{
				print( "ERREUR : Pb recupération répertoire recv" BR BR, _ );
				print( "--------------------------------------------------------------------------------", BR, _ );
				}
			else
				{
				file_move( fname, fname1 );
				if ( !dir_exists( fname1 ) )
					{
					print( "ERREUR : Pb recupération répertoire recv" BR BR, _ );
					print( "--------------------------------------------------------------------------------", BR, _ );
					}
				}
			}
		
		strdcpy( &fname, dirdat, DIR_CHAR "send71", _ );
		if ( dir_exists( fname ) )
			{
			strdcpy( &fname1, dirdat, DIR_CHAR "send", _ );
			file_delete( fname1 );
			if ( dir_exists( fname1 ) )
				{
				print( "ERREUR : Pb recupération répertoire send" BR BR, _ );
				print( "--------------------------------------------------------------------------------", BR, _ );
				}
			else
				{
				file_move( fname, fname1 );
				if ( !dir_exists( fname1 ) )
					{
					print( "ERREUR : Pb recupération répertoire send" BR BR, _ );
					print( "--------------------------------------------------------------------------------", BR, _ );
					}
				}
			}


		#ifdef OS_UNIX
		if ( dir_exists( "/logiciels/ssie7/mftbin" ) )
			{
			file_delete( "/logiciels/ssie7/mft" );
			file_move( "/logiciels/ssie7/mftbin", "/logiciels/ssie7/mft" );
			}
		#endif
		
		print( "RECUPERATION DES DONNES SSIE v7.1 : OK" BR BR, _ );
		print( "--------------------------------------------------------------------------------", BR, _ );
		}
	}

	#ifdef OS_UNIX
	if ( b2 )
		upgrade_back_mersi3_mft( t_work );
	#endif

	strdcpy( &fname1, dirflux, ".old", _ );
	if ( dir_exists( fname1 ) )
		{
		print( "RECUPERATION ANCIEN HISTORIQUE DES FLUX : OK" BR BR, _ );
		strdcpy( &fname2, dirflux, ".7.2", _ );
		if ( dir_exists( fname2 ) )		file_delete( fname2 );
		
		file_move( dirflux, fname2 );
		if ( dir_exists( dirflux ) )
			{
			print( "ERREUR : Pb renommage ", dirflux, " => ", fname2, BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, _ );
			exit(3);
			}
		
		file_move( fname1, dirflux );
		if ( !dir_exists( dirflux ) )
			{
			print( "ERREUR : Pb renommage ", fname1, " => ", dirflux, BR BR, _ );
			print( "--------------------------------------------------------------------------------", BR, _ );
			exit(3);
			}
		}
	
	
	print( BR BR "RETOUR ARRIERE : TERMINE" BR BR, _ );
	print( "--------------------------------------------------------------------------------", BR, _ );

wpause();
	return OK;
	}


#ifdef OS_UNIX

//------------------------------------------------------------------------------
int upgrade_back_mersi3_mft( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( b2 == -1 )
		{
		strdcpy( &fname, "/appli/mersi3/conf/mersi3.dat", _ );
		if ( ! file_exists( fname ) )
			{
			print( " -> ", "Le fichier ", fname, " n'existe pas ou est inacessible.", BR BR, _ );
			r = 1; return ERROR;
			}
		h = var_load( fname );
		v = var_get( h, "srvnat_list" );
		if ( ( v )&&( ! stricmp( v, "none" ) ) ) bDMZ = true;
		var_free( h ); h = 0;
	
		print( BR "REACTIVATION DE MERSI v3", BR, _ );
		
		System( &s, "mv " SAUVE_DIR "/mersi3 " LOGICIELS "/mersi3/mersi3" );
		System( &s, "chown mersi:mersi " LOGICIELS "/mersi3/mersi3" );
		System( &s, "chmod 755 " LOGICIELS "/mersi3/mersi3" );
		
		if ( bMigrPEPSIS )	back_pepsis( t_work );
		
		if ( !bDMZ )
			{
			print( BR "Mise à jour de l'annuaire national." BR, _ );
			strdcpy( &fname, "/tmp/ssie7up_tmp", _ );
			
			file_save_str( fname,
				LOGICIELS "/mersi3/mersi3 <<FIN" BR
				"server save" BR
				"quit" BR
				"FIN" BR, -1
				);
			
			strdcpy( &cmd, "chmod 777 ", fname, _ );
			System( &s, cmd );
			strdcpy( &cmd, "su - mersi -c ", fname, _ );
			System( &s, cmd );
			print( "mersi3 server save" BR, s, BR BR, _ );
			
			file_delete( fname );
			}
		}
	else
	if ( b2 == -2 )
		{
		print( BR "REACTIVATION DE MFT v2.4.1", BR, _ );
		
		System( &s, "mv " SAUVE_DIR "/MFTMAIN " LOGICIELS "/mft/2.4.1/filexe/MFTMAIN" );
		System( &s, "chown mft:mft " LOGICIELS "/mft/2.4.1/filexe/MFTMAIN" );
		System( &s, "chmod 700 " LOGICIELS "/mft/2.4.1/filexe/MFTMAIN" );
		
		
		if ( !bDMZ )
			{
			strdcpy( &fname, "/appli/ssie7/conf/ssie7.dat", _ );
			if ( hPart ) var_free( hPart );
			hPart = var_load( fname );
			strdcpy( &s, "part ", var_get( hPart, "partname" ), " ip='", var_get( hPart, "ip_ssie" ), "'",
				" dns='", var_get( hPart, "dns_ssie" ), "'", " os='", OS_NAME, "'",
				" port_mft=1761", " adl_liste='", var_get( hPart, "adl_liste" ), "'",
				" product_name='MFT v2.4.1'",
				_ );
			nat_request( &s1, s );
			print( BR "MISE A JOUR DE L'ANNUAIRE NATIONAL :" BR
				"------------------------------------------------------------------------------" BR,
				s, BR, "---" BR, s1, BR, "---" BR,
				"------------------------------------------------------------------------------" BR
				, _ );
			}
		}
	
	// Mise à jour .bash_profile ou .profile
	if ( bRoot )
		{
		strdcpy( &fname, "/home/mersi/.bash_profile", _ );
		if ( !file_exists( fname ) )
			strdcpy( &fname, "/home/mersi/.profile", _ );
		if ( file_exists( fname ) )
			{
			file_load( &s, fname );
			strd_replace( &s, "/ssie7", "/mersi3" );
			strd_replace( &s, "ssie7 ", "mersi3 " );
			strd_replace( &s, "ssie7;", "mersi3;" );
			strd_replace( &s, "ssie7'", "mersi3'" );
			strd_replace( &s, "ssie7'", "mersi3'" );
			file_save_str( fname, s, -1 );
			}

		// Mise à jour proprio log
		strdcpy( &fname, dirconf, DIR_CHAR "ssie7.dat", _ );
		h = var_load( fname );
		ssie_user = var_get( h, "ssie_user" );
		if ( !ssie_user ) ssie_user = "ssie";

		strdcpy( &cmd, "chown -R ", ssie_user, ":", ssie_user, " /appli/ssie7/log", _ );	
		System( &s, cmd );
		}
		
	return OK;
	}
	
#endif

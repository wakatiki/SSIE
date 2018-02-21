//------------------------------------------------------------------------------
int cmd_test( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	static FILE *flock = NULL;
	static char *version = null;
	
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
	
	v = var_getn( hReq, 1, &vn );
	if ( !v )
		vn = "bd";
//		return OK;

/*
strdcpy( &cmd, "ssie7" EXE " mft send part=F5313102 fname=c:/e/t.t", _ );

for( i = atoi(vn) ; i > 0 ; i-- )
	{
	System( &s, cmd );
	if ( !(i % 100) ) print( itoa(i), BR, _ );
	}

return OK;
*/


	
	//--------------------------------------------------------------------------------------
	if ( !strcmp( vn, "xlate_cmp" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( ! var_getn( hReq, 2, &v1 ) ) // Nom du fichier 1
			{
			print( "erreur syntaxe : t xlate_cmp {fic1} {fic2}" BR, _ );
			return ERROR;
			}
		if ( ! var_getn( hReq, 2, &v2 ) ) // Nom du fichier 1
			{
			print( "erreur syntaxe : t xtale_cmp {fic1} {fic2}" BR, _ );
			return ERROR;
			}
		
		if ( !file_load( &s1, v1 ) ) { print( "ERREUR ouverture ", v1, BR, _ ); return ERROR; }		
		if ( !file_load( &s2, v2 ) ) { print( "ERREUR ouverture ", v2, BR, _ ); return ERROR; }		
		
		for( i = n = 0 ; i < 256 ; i++ )
			{
			if ( s1[i] == s2[i] ) continue;
			
			print( itoa(i), " : ", itoa(s1[i]), ", ", itoa( s2[i] ), BR, _ );
			n++;
			}
		
		print( BR, itoa(n), " différence", (char*)((n>1)?"s":""), BR, _ );
		return OK;
		}
		
	//--------------------------------------------------------------------------------------
	if ( !strcmp( vn, "lock" ) )
	//--------------------------------------------------------------------------------------
		{
/*
		hLock = bd_lock( hMftCat, "A0000001" );
*/
		strdcpy( &fname, "test.lock", _ );
//		strdcpy( &fname, "/appli/ssie7/data/flux/5.run.lock", _ );
		hLock = file_lock0( fname );
		if ( ! hLock )
			{
			print( "ERREUR lock" BR, _ );
			return OK;
			}
		print( "Lock OK", BR, _ );
		hLock1 = hLock;
		return OK;
		}
		
	//--------------------------------------------------------------------------------------
	if ( !strcmp( vn, "unlock" ) )
	//--------------------------------------------------------------------------------------
		{
//		bd_unlock( hMftCat, "A0000001", hLock1 );
		strdcpy( &fname, "test.lock", _ );
		file_unlock0( hLock1 );
		unlink( fname );
		print( "Unlock OK", BR, _ );
		hLock1 = (HANDLE)hNULL;
		return OK;
		}
		
	//--------------------------------------------------------------------------------------
	if ( !strcmp( vn, "mftpurge" ) )
	//--------------------------------------------------------------------------------------
		{

		mftcat_purge( t_work );

/*

	for( i = 1, n=sysdate ;  ( sysdate - n ) < 10*60 ; i++ )
		{
		mftcat_purge( 30 );
		}
return OK;
*/




	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
	
		for( ; ; )
			{
			if ( bd_select( hMftCat, "status=C|H", "part", &s4 ) == ERROR )
				break;

//print( "s4=", s4, BR, _ );

			if ( bd_load( hMftCat, s4, strlen(s4)/8, ",:part", &s3 ) == ERROR )
				break;

//print( "s3=", s3, BR, _ );
			
			strdcpy( &part, "", _ );
			for( i = 0, l = strlen(s3) ; i < l ; i = j+1 )
				{
				j = strpos( s3, "\n", i );
				if ( j < 0 ) j = l;
				strdncpy( &s2, &(s3[i]), j-i, _ );
				trim( s2 );
				if ( !( s2[0] ) ) break;
				if ( !strcmp( part, s2 ) ) continue;
				v = part; part = s2; s2 = v;
				
				strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_send_part_", part, ".pid", _ );
				if ( file_load( &s, fname ) )
					if ( process_is_alive( atoi( s ) ) ) continue;
				
//print( "ROBOT MFT : mft send_part ", part, BR, _ );

//				strdcpy( &cmd, argv[0], " mft send_part ", part, _ );
//				StartH( cmd );
				}

			
//			break;
			}

return OK;


	h = var_from_cmd( 0,
		"dirdat=c:/e/apps/ssie72/data/mftcat,"
		"diridx=c:/e/apps/ssie72/data/mftcat/index,"
		"key_name=ida,"
		"item_size=2048,"
		"vlfields='mftlog',"
		
		"index_path='dt_crea[0,10]/status/part/direct',"
		"index_key='dt_crea[19],idt_mft[8],parm[100]',"
		);
	
	var_save( h, "c:/e/tmp/bd.dat" );


//	file_delete( "c:/e/apps/ssie72/data" );

	h1 = bd_open( "c:/e/tmp/bd.dat" );

	if ( h1 == hNULL )
		{
		print( error_msg, BR, _ );
		return;
		}



	for( i = 1, n=sysdate ;  ( sysdate - n ) < 10*60 ; i++ )
		{
/*
		strdcpy( &s1, itona(i, 8), _ );
		strdcpy( &s, "ida=", s1, ",part=F5313103,fname='c:/e/atoi',"
					"parm='Test de mft send', status=C, idt_mft=A0023456,"
					"mftlog='aaaaaaaaaaBBBBBBBBBBccccccccccDDDDDDDDDDeeeeeeeeeeFFFFFFFFFF'",
					_ );
		var_free( h );
		h = var_from_str( 0, s );

//		if ( bd_insert( h1, h, &s ) == ERROR )
		if ( bd_update( h1, h, s1, 1 ) == ERROR )
			{
			print( error_msg, BR, _ );
			return;
			}
*/	
		if ( bd_select( h1, "part=F5313103", null, &s4 ) == ERROR )
			{
			print( error_msg, BR, _ );
			return;
			}
//print( itoa(strlen(s4)/8), BR, _ );
//pause();	
		}

print( "i = ", itoa(i), BR, _ );
bd_close( h1 );
h1 = 0;

return;

/*
	hData = var_from_str( 0, "ida=12345678,part=F5313102,fname='c:/e/atoi',"
							"parm='Test de mft send', status=C, idt_mft=A0023456,"
							"mftlog='aaaaaaaaaaBBBBBBBBBB'"
//							"mftlog='aaaaaaaaaaBBBBBBBBBBccccccccccDDDDDDDDDDeeeeeeeeeeFFFFFFFFFF'"
							);

	if ( bd_insert( hBD, hData, &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}


//	bd_clean( hBD );



	hData = var_from_str( 0, "ida=12345670,part=CCIEUX1I,fname='c:/e/atoi',"
							"parm='Test de mft send 2', status=C, idt_mft=A0023456" );

var_print( hData, BR );
print( BR, _ );
	
	if ( bd_insert( hBD, hData, &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}


	hData = var_from_str( 0, "ida=12300000,part=ZEPAAA1I,fname='c:/e/atoi',"
							"parm='Test de mft send 2', status=C, idt_mft=A0023456" );

	if ( bd_insert( hBD, hData, &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}


	hData = var_from_str( 0, "ida=12200000,part=AAAAAA1I,fname='c:/e/atoi',"
							"parm='Test de mft send 2', status=C, idt_mft=A0023456" );

	if ( bd_insert( hBD, hData, &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
*/
	
/*
	if ( bd_load( hBD, "1234567812345678", 2, "js", &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	print( BR, s, BR, _ );
	
	if ( bd_load( hBD, "12345678", 1, "js:mftlog", &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	print( BR, s, BR, _ );
*/
/*
	print( BR "UPDATE" BR, _ );
	
	var_free( hData );
	hData = var_from_str( 0, "ida=12345678, status=T, idt_mft=A0023456, rappl=TESTAPP1,"
							"mftlog = 'elib_bd_vlf_concat:ccccccccccDDDDDDDDDDeeeeeeeeeeFFFFFFFFFFggggggggggHHHHHHHHHHiiiiiiiiiiJJJJJJJJJJkkkkkkkkkk'"
							);
	
	if ( bd_update( hBD, hData, "12345678", 1 ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	

	if ( bd_load( hBD, "12345678", 1, "js", &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	print( BR, s, BR, _ );

	if ( bd_load( hBD, "12345678", 1, "js:mftlog", &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	print( BR, s, BR, _ );
/*	
	bd_clean( hBD );
	
	var_free( hData );
	hData = var_from_str( 0, "ida=12345678, status=T, idt_mft=A0023456, rappl=TESTAPP1,"
							"mftlog = 'LLLLLLLLLL'"
							);
	
	if ( bd_update( hBD, hData, "12345678", 1 ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	

	if ( bd_load( hBD, "12345678", 1, "js", &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	print( BR, s, BR, _ );

	if ( bd_load( hBD, "12345678", 1, "js:mftlog", &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	print( BR, s, BR, _ );

	bd_clean( hBD );

	if ( bd_load( hBD, "12345678", 1, "js", &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	print( BR, s, BR, _ );

	if ( bd_load( hBD, "12345678", 1, "js:mftlog", &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	print( BR, s, BR, _ );


	bd_clean( hBD );


	if ( bd_delete( hBD, "12345678", 1 ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	
	bd_clean( hBD );
*/


/*
	print( "TEST BD_SELECT" BR, _ );
	
//	if ( bd_select( hBD, "part=*53131*, status=T", null, &s ) == ERROR )
//	if ( bd_select( hBD, "*='F5 2017 | 2016 C'", null, &s ) == ERROR )
//	if ( bd_select( hBD, "", null, &s ) == ERROR )
//	if ( bd_select( hBD, "parm=*test*", null, &s ) == ERROR )
//	if ( bd_select( hBD, "rappl=*test*", null, &s ) == ERROR )
//	if ( bd_select( hBD, "parm=*toto* or rappl=*test*", "part,rappl,parm", &s ) == ERROR )
//	if ( bd_select( hBD, "parm=*test*", "part", &s ) == ERROR )
	if ( bd_select( hBD, "part <= ZEQ", null, &s ) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}

	print( BR BR "result = ", s, BR, _ );
	
	print( "OK" BR, _ );
*/
		return OK;
		}
		
/*
	//--------------------------------------------------------------------------------------
	if ( !strcmp( vn, "bp" ) )
	//--------------------------------------------------------------------------------------
		{
		strdcpy( &fname, dirconf, DIR_CHAR "bdoc" DIR_CHAR "*.dat", _ );
		tf = NULL; b = false;
		n = 0;
		while( tf = file_list( tf, fname, "absdir nodir" ) )
			{
			var_free( h );
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			file_load( &s, fname );
			if ( s[0] == '@' )
				strcpy( &(fname[strlen(fname)-3]), "cfg" );
			else
				strdcpy( &fname, s, DIR_CHAR "bdoc.cfg", _ );
			h = var_load( fname );
			v = var_get( h, "dirdat" );
			if (( !v )||( !(v[0]) )) continue;
			
			print( fname, " : ", v,  BR, _ );
			
			tf2 = NULL;
			strdcpy( &fname, v, DIR_CHAR "data" DIR_CHAR "deleted", _ );
			while( tf2 = file_list( tf2, fname, "absdir nodir recursif" ) )
				{
				print( tf2->dir, BR, _ );
				file_delete( tf2->dir );
				n++;
				}
			
			}
		print( BR, itoa(n), " documents supprimés" BR, _ );
		return OK;
		}
*/
		
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "part_delete" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			strdcpy( &fname, "ssie7 part ", v, " delete", _ );
			System( &s, cmd );
			print( cmd, BR "<pre>", s, BR "</pre>", _ );
			return OK;
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c \"mersi3 part ", v, " delete\"", _ );
			System( &s, cmd );
			strdcpy( &cmd1, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c \"mersi3 MFTUTIL MFTPART ID=", v, ",MODE=DELETE\"", _ );
			System( &s1, cmd1 );
			print( cmd, BR "<pre>", s, "</pre>" BR BR, cmd1, BR "<pre>", s1, BR "</pre>", _ );
			return OK;
			}
		else
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mft -c \"MFTUTIL MFTPART ID=", v, ",MODE=DELETE\"", _ );
			System( &s, cmd );
			print( cmd, BR "<pre>", s, BR "</pre>", _ );
			}
			
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "mftcat" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			strdcpy( &fname, "ssie7 mftcat", _ );
			System( &s, cmd );
			print( cmd, BR "<pre>", s, BR "</pre>", _ );
			return OK;
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c \"mersi3 listcat\"", _ );
			System( &s, cmd );
			print( cmd, BR "<pre>", s, BR "</pre>", _ );
			return OK;
			}
		else
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mft -c \"MFTUTIL listcat\"", _ );
			System( &s, cmd );
			print( cmd, BR "<pre>", s, BR "</pre>", _ );
			}
			
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "mftutil" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			p = stripos( cmd, "mftutil", 0 );
			strdcpy( &cmd1, "ssie7 ", &(cmd[p]), _ );
			System( &s, cmd1 );
			print( cmd1, BR "<pre>", s, BR "</pre>", _ );
			return OK;
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			p = stripos( cmd, "mftutil", 0 );

			strdcpy( &cmd1, LOGI_DIR DIR_CHAR "mersi3/mersi3 ", &(cmd[p]), _ );
			file_save_str( "/tmp/mersi3_test", cmd1, -1 );
			system( "chmod 755 /tmp/mersi3_test" );
			
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c \"/tmp/mersi3_test\"", _ );
			System( &s, cmd );
			print( cmd1, BR "<pre>", s, BR "</pre>", _ );
			return OK;
			}
		else
			{
			p = stripos( cmd, "mftutil", 0 );
			strdcpy( &cmd1, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mft -c \"MFTUTIL ",
					&(cmd[p+7]), "\"", _ );
			System( &s, cmd1 );
			print( cmd1, BR "<pre>", s, BR "</pre>", _ );
			}
			
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "version" ) )
	//--------------------------------------------------------------------------------------
		{
		strdcpy( &version, trim(v), _ );
		#ifdef OS_WINDOWS
		if ( strnicmp( version, "ssie v7", 7 ) )
			{
			print( "ERREUR : version non supporté sous Windows" BR, _ );
			return ERROR;
			}
		#endif
		print( v, BR, _ );
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !strcmp( vn, "file_create" ) )
	//--------------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 2, &v1 ); // Nom du fichier
		strdcpy( &fname, v1, _ );
		strd_replace( &fname, "[$dirdat]", dirdat );
		strd_replace( &fname, "[$dirconf]", dirconf );
		strd_replace( &fname, "[$dirtmp]", dirtmp );
		strd_replace( &fname, "[$dirlog]", dirlog );
		
		v = var_getn( hReq, 3, &v2 ); // Taille du fichier
		n = atoi( v2 );

		fp = fopen( fname, "wb" );
		if ( fp == NULL )
			{
			print( "ERREUR : création fichier ", fname, _ );
			return ERROR;
			}
		
		
		for( n1 = 0 ; n1 < n ; n1++ )
			{
			fwrite( fp, "A", 1, _ );
			if ( !( n1 % 80 ) ) fwrite( fp, BR, -1, _ );
			}
		
		fclose( fp );
		print( "OK", _ );
		return OK;
		}
	

	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "mersi3" ) )
	//--------------------------------------------------------------------------------------
		{
		p = strpos( cmd, "mersi3", 0 );
		file_tmpname( &fname );
		file_save_str( fname, &(cmd[p]), -1 );
		strdcpy( &cmd1, "chmod 755 ", fname, _ );
		system( cmd1 );
		strdcpy( &cmd1, "sudo " BIN_DIR "/ssie7 su - mersi -c \"", fname, "\"", _ );
		print( cmd1, BR, _ );
		System( &s, cmd1 );
		
		print( s, BR, _ );
		file_delete( fname );
		
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "sleep" ) )
	//--------------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 2, &v1 ); // delai en secondes
		sleep( atoi( v1 ) );
		print( "sleep ", v1, BR, _ );
		return OK;
		}

	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "is_ascii_file" ) )
	//--------------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 2, &v1 ); // Nom du fichier
		if ( file_load( &s, v1 ) == sERROR )
			{
			print( "ERREUR : fichier non trouvé", _ );
			return ERROR;
			}
		
		#ifdef OS_WINDOWS
		
		p = strpos( s, "\n", 0 );
		if ( p > 0 )
			{
			if ( s[p-1] == '\r' )
				{
				print( "true", _ );
				return OK;
				}
			}
		print( "false", _ );
		
		#else
		
		if ( strpos( s, "\r", 0 ) >= 0 )
			print( "false", _ );
		else
			print( "true", _ );
			
		#endif
		
		return OK;
		}


	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "file_exists" ) )
	//--------------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 2, &v1 ); // Nom du fichier
		strdcpy( &s, v1, _ );
		strdcpy( &s1, dirtmp, DIR_CHAR, _ );
		strd_replace( &s, "/tmp/", s1 );
		if ( file_exists( s ) )
			{
			print( "true", _ );
			return OK;
			}
		print( "false", _ );
		return OK;
		}
		


	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "file_lock" ) )
	//--------------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 2, &v1 ); // Nom du fichier
		strdcpy( &fname, "/tmp/lock.pid", _ );
		if ( file_load( &s, fname ) != sERROR )
			process_kill( atoi( s ) );
		
		if ( flock = fopen( v1, "rb+" ) )
			print( "OK", _ );
		else
			print( "ERREUR", _ );
		
		strdcpy( &fname, "/tmp/lock.pid", _ );
		file_save_str( fname, itoa(getpid()), -1 );
		
		return OK;
		}
		


	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "file_unlock" ) )
	//--------------------------------------------------------------------------------------
		{
		strdcpy( &fname, "/tmp/lock.pid", _ );
		if ( file_load( &s, fname ) != sERROR )
			{
			strdcpy( &cmd1, "kill -9 ", s, _ );
			print( cmd1, BR, __ );
			system( cmd1 );
			}
	
		return OK;
		}
	
	
	
	
	//--------------------------------------------------------------------------------------

	if ( !version )
		{
		print( "ERREUR version non fourni" BR, _ );
		return ERROR;
		}



	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "server_start" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			#ifdef OS_WINDOWS
			System( &s, "ssie7 do_start" );
			#else
			System( &s, "ssie7 start" );
			#endif
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c "
						"\"" LOGI_DIR DIR_CHAR "mersi3" DIR_CHAR "mersi3 start\"", _ );
			print( cmd, BR, _ );
			System( &s, cmd );
			if ( stripos( s, "ERREUR : Le serveur est d", 0 ) >= 0 )
				strdcpy( &s, "ATTENTION : Le serveur est déjà démarré", _ );
			}
		else
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mft -c "
						"\"" LOGI_DIR DIR_CHAR "mft/2.4.1/exploit/mft_start\"", _ );
			print( cmd, BR, _ );
			System( &s, cmd );
			}
		
			
		print( s, BR, _ );
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "server_stop" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			#ifdef OS_WINDOWS
			System( &s, "ssie7 do_stop" );
			#else
			file_save_str( "/tmp/stop_ssie", "ssie7 stop", -1 );
			system( "chmod 700 /tmp/stop_ssie" );
			System( &s, "/tmp/stop_ssie" );
			file_delete( "/tmp/stop_ssie" );
			#endif
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c "
						"\"" LOGI_DIR DIR_CHAR "mersi3" DIR_CHAR "mersi3 stop\"", _ );
			print( cmd, BR, _ );
			System( &s, cmd );
			}
		else
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mft -c "
						"\"" LOGI_DIR DIR_CHAR "mft/2.4.1/exploit/mft_stop\"", _ );
			print( cmd, BR, _ );
			System( &s, cmd );
			}
			
		print( s, BR, _ );
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "server_log" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			System( &s, "ssie7 log" );
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c "
						"\"" LOGI_DIR DIR_CHAR "mersi3" DIR_CHAR "mersi3 log\"", _ );
			print( cmd, BR, _ );
			System( &s, cmd );
			}
		else
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mft -c "
						"\"cat /var/mft/log/*\"", _ );
			print( cmd, BR, _ );
			System( &s, cmd );
			}
			
		print( s, BR, _ );
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "purge_log" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			strdcpy( &fname, dirlog, DIR_CHAR "*", _ );
			strd_replace( &fname, module_name, "ssie7" );
			print( "file_delete : ", fname, BR, _ );
			file_delete( fname );
			return OK;
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 rm -f /appli/mersi3/log/*", _ );
			System( &s, cmd );
			print( cmd, BR, s, BR, _ );
			return OK;
			}
		else
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mft -c "
						"\"rm -f /var/mft/log\"", _ );
			print( cmd, BR, _ );
			System( &s, cmd );
			}
			
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "purge_cat" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			strdcpy( &fname, dirdat, DIR_CHAR "mftcat", _ );
			strd_replace( &fname, module_name, "ssie7" );
			print( "file_delete ", fname, BR, _ );
			file_delete( fname );
			strdcat( &fname, DIR_CHAR "index", _ );
			mkdir( fname );
/*
			strdcpy( &fname, BIN_DIR DIR_CHAR "mft" DIR_CHAR "mft", _ );
			if ( file_exists( fname ) )
				{
				strdcpy( &cmd1, BIN_DIR DIR_CHAR "ssie7 mft delete part=*\"", _ );
				System( &s, cmd1 );
				}
*/
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c "
						"\"" LOGI_DIR DIR_CHAR "mersi3" DIR_CHAR "mersi3 mftutil delete part=*\"", _ );
			print( cmd, BR, _ );
			System( &s, cmd );
			print( cmd, BR, s, BR, _ );
			}
		else
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mft -c "
						"\"" LOGI_DIR DIR_CHAR "mft/2.4.1/tools/MFTUTIL delete part=*\"", _ );
			print( cmd, BR, _ );
			System( &s, cmd );
			print( cmd, BR, s, BR, _ );
			}
			
		return OK;
		}

	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "srvannu_list" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			strdcpy( &cmd, "ssie7 srvannu list='", v, "'", _ );
//[]test version='ssie v7.2'[]test srvannu list=si-ssie.edf.fr

			System( &s, cmd );
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c "
						"\"" LOGI_DIR DIR_CHAR "mersi3" DIR_CHAR "mersi3 srvannu list '", v, "'\"", _ );
			System( &s, cmd );
			}
		else
			{
			strdcpy( &cmd, "Sans objet", _ );
			strdcpy( &s, "", _ );
			}
		
			
		print( cmd, BR, s, BR, _ );
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "part_loadnat" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			strdcpy( &cmd, "ssie7 part ", v, " loadnat", _ );
			System( &s, cmd );
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c "
						"\"" LOGI_DIR DIR_CHAR "mersi3" DIR_CHAR "mersi3 part ", v, " loadnat\"", _ );
			System( &s, cmd );
			}
		else
			{
			strdcpy( &cmd, "part ", v, " get", _ );
			strdcpy( &s1, "si-ssie.edf.fr:1760", _ );
			tcp_request( s1, cmd, &s2 );
			var_free( h );
			print( cmd, BR, s2, BR, _ );
			h = var_from_str( 0, s2 );
			var_to_str( h, &s );
			}
		
			
		print( cmd, BR, s, BR, _ );
		return OK;
		}
	
	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "part" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			strdcpy( &cmd, "ssie7 part ", v, _ );
			System( &s, cmd );
			}
		else
		if ( !strnicmp( version, "mersi", 5 ) )
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mersi -c "
						"\"" LOGI_DIR DIR_CHAR "mersi3" DIR_CHAR "mersi3 part ", v, "\"", _ );
			System( &s, cmd );
			}
		else
			{
			strdcpy( &cmd, "sudo " BIN_DIR DIR_CHAR "ssie7 su - mft -c "
						"\"" LOGI_DIR DIR_CHAR "mft/2.4.1/tools/MFTUTIL mftext id=", v, "\"", _ );
			print( cmd, BR, _ );
			System( &s, cmd );
			}
			
		print( cmd, BR, s, BR, _ );
		return OK;
		}

	//--------------------------------------------------------------------------------------
	if ( !stricmp( vn, "mft_gateway" ) )
	//--------------------------------------------------------------------------------------
		{
		if ( !strnicmp( version, "ssie v7", 7 ) )
			{
			strdcpy( &cmd, "ssie7 server mft_gateway='", v, "'", _ );

			System( &s, cmd );
			print( cmd, BR, s, BR, _ );
			return OK;
			}
		}
	
	print( "ERREUR : commande inconnue" BR, _ );
	}









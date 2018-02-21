
	//--------------------------------------------------------------------------
	// ROBOT DE SURVEILLANCE DES FLUX SSIE
	//--------------------------------------------------------------------------
	
	if ( ! strcmp( argv[2], "flux" ) )
		
	{
	if ( argc < 4 ) ssie7exit(1);
	
	tr("robot_flux_1");
	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", argv[3], ".pid", _ );
	if ( file_load( &s, fname ) != sERROR )
		{
		tr("robot_flux_2");
		if ( process_is_alive( atoi(s) ) ) ssie7exit(0);
		}
	
	tr("robot_flux_3");	
	file_save_str( fname, itoa(getpid()), -1 );
	
	HANDLE _hLock;
	T_FILE_INFO *_tf;
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
	
	tr("robot_flux_4");	
	
	for( n5 = 1 ; ; n5++ )
		{
		#ifdef TR
//		unlink( tr_fname );
		#endif
				
		if ( ! (n5%(60*30)) ) exit(0);	// Redémarrage toutes les 30 minutes

		sleep(1);

		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", argv[3], ".pid", _ );
		file_save_str( fname, itoa(getpid()), -1 ); // Pour surveillance de blocage
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", argv[3], ".timeout", _ );
		unlink( fname );
		
		tr("robot_flux_5");		
		//------------------------------------------------------------------
		// EXECUTE LES FLUX 1 PAR 1
		//------------------------------------------------------------------
		
		for( ; ; )
			{
			bFluxStarted = false;
			strdcpy( &fname, dirflux, DIR_CHAR "*.run", _ );
			_tf = NULL;
			while( _tf = file_list( _tf, fname, "absdir nodir" ) )
				{
				tr("robot_flux_6");
				strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
				if ( file_exists( fname ) ) ssie7exit(0);
				
				tr("robot_flux_7");			
				
				// Vérrouillage pour qu'un flux ne soit traité que par un seul robot flux
				strdcpy( &fname, _tf->dir, DIR_CHAR, _tf->name, ".lock", _ );

				_hLock = file_lock0( fname );
				if ( !_hLock ) continue;

				tr("robot_flux_8");			
	
				strdcpy( &fname, _tf->dir, DIR_CHAR, _tf->name, _ );
				file_save_str( fname, v = itoa(getpid()), -1 ); // Pour ssie7 flux stop
				
				strdcpy( &fname, _tf->dir, DIR_CHAR, _tf->name, _ );
				fname[n = (strlen(fname)-3)] = 'd';
				fname[n+1] = 'a';
				fname[n+2] = 't';
				if ( !file_exists( fname ) )
					{
					strdcpy( &fname, _tf->dir, DIR_CHAR, _tf->name, _ );
					file_delete( fname );
					goto unlock;
					}
				
				strdcpy( &fname, _tf->dir, DIR_CHAR, _tf->name, ".timeout", _ );
				n = 3;
				if ( file_load( &s, fname ) != sERROR )
					n = atoi( s );
				strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", argv[3], ".timeout", _ );
				file_save_str( fname, itoa(n), -1 );
				
				strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", argv[3], ".pid", _ );
				file_save_str( fname, v, -1 ); // Pour surveillance de blocage
				
				strdcpy( &flux_id, _tf->name, _ );
				flux_id[ strlen(flux_id)-4 ] = 0;
				hLog = hlog_init( dirflux, flux_id );
				
				flux_execute( t_work );
				
				hlog_free( hLog );
	
				strdcpy( &fname, _tf->dir, DIR_CHAR, _tf->name, _ );
				if ( file_exists( fname ) )
					file_save_str( fname, "", 0 );

				strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", argv[3], ".timeout", _ );
				unlink( fname );
				
				unlock:

				tr("robot_flux_9");
					
				file_unlock0( _hLock );
				#ifdef OS_WINDOWS
				strdcpy( &fname, _tf->dir, DIR_CHAR, _tf->name, ".lock", _ );
				unlink( fname );
				#endif
//tr2( "unlock fname=", fname );
				}
			if ( !bFluxStarted ) break;
			// Si des flux ont été démarrés, on n'attend pas, on essai de les
			// traiter immédiatement
			tr("robot_flux_10");
			}
		}
	}


















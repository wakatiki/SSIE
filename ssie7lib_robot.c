

//------------------------------------------------------------------------------
int robot_run( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
//	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server_", v = var_get( hSrvConf, "port" ), ".pid", _ );
//	strdcpy( &cmd, argv[0], " server ", v, _ );
	
	for( n = 0 ; ; sleep( 1 ), n++ )
		{
/*
		//------------------------------------------------------------------
		// SURVEILLANCE DU SERVEUR HTTP
		//------------------------------------------------------------------
		
		if ( s ) s[0] = 0;
		file_load( &s, fname );
		if ( ( !s )||( ( p = atoi( s ) )&&( !process_is_alive( p ) ) ) )
			{
			// REDEMARRE LE PROCESS PRINCIPAL
			
			logw( "ROBOT : Le serveur est anormalement arrêté => Redémarrage", _ );
			StartH( cmd );
			
			// SOUS UNIX, LES PROCESS SECONDAIRES SONT DIRECTEMENT SURVEILLES
			// PAR LE PROCESS PRINCIPAL
			}
*/	

		//------------------------------------------------------------------
		// SURVEILLANCE DU SERVEUR HTTP
		//------------------------------------------------------------------
		strdcpy( &fname, dirwrk, DIR_CHAR "ssie7_server_", v = var_get( hSrvConf, "port" ), ".pid", _ );
		if ( s ) s[0] = 0;
		file_load( &s, fname );
		if ( ( s )&&( s[0] )&&( !process_is_alive( atoi( s ) ) ) )
			{
			strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
			if ( !file_exists( fname ) )
				{
				// REDEMARRE LE PROCESS PRINCIPAL
				strdcpy( &cmd, argv[0], " server ", v, _ );
				StartH( cmd );
				logw("ROBOT : serveur anormalement arrêté => Redémarrage : cmd=", cmd, _ );		
				}
			
			// SOUS UNIX, LES PROCESS SECONDAIRES SONT DIRECTEMENT SURVEILLES
			// PAR LE PROCESS PRINCIPAL
			}
		
		


		//------------------------------------------------------------------
		// UNE FOIS PAR JOUR ET A CHAQUE REDEMARRAGE
		//------------------------------------------------------------------
		
		if ( !( n % (24*60*60) ) )
			{
			// PURGE DES LOG
			logw( "ROBOT : Purge de ", dirlog, _ );
			file_purge( dirlog, 30 );
			}
		
	
		//------------------------------------------------------------------
		ssie7free( t_work );
		trace_close( );
		}
	}
			
			

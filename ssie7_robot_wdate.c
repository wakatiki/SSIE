	//--------------------------------------------------------------------------
	// ROBOT DE SURVEILLANCE DES PROCESS EN ATTENTE D'UNE DATE
	//--------------------------------------------------------------------------
	
	if ( ! strcmp( argv[2], "wdate" ) )
		{
		mkdir( dirwdate );
		for( ; ; sleep(1) )
			{
			strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
			if ( file_exists( fname ) ) exit(0);
/*{
logw( "robot_wdate : fichier stop présent", _ );
exit(0);
}*/			
			
			// PARCOUR LE REPERTOIRE WDATE
				// Les fichiers doivent porter un nom qui se termine par la date de déclenchement
			tf = NULL;
			while( tf = file_list( tf, dirwdate, "nodir absdir" ) )
				{
				p = strpos( tf->name, ".", 0 );
				if ( p < 0 ) continue;
				if ( ! is_numeric( &(tf->name[p+1]) ) ) continue;
				if ( atoi( &(tf->name[p+1]) ) > time(NULL) ) continue; // Ce n'est pas encore l'heure
				
				// DEMARRE LE PROCESS ET LE PLACE EN PROCESS SURVEILLE
				
				strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
				if ( file_load( &cmd, fname ) )
					{
					unlink( fname );
					StartSurv( t_work );
					}
				}

			ssie7free( t_work );
//			trace_close( );
			}
		}
		













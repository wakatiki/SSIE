
	//--------------------------------------------------------------------------
	// ROBOT DE TRAITEMENT DE SURVEILLANCE DE REPERTOIRE
	//--------------------------------------------------------------------------

		
	// Le contenu du fichier .run doit être de la forme :
	// wfile:timeout<>pattern file(s)<>Isolate files DIR<>Sudo<>cmde

	if ( ts ) free( ts );
	ts = str_explode( &(s3[6]), "<>" );

	if ( ( ts[1] == null )||( ts[2] == null )||( ts[3] == null )||( ts[4] == null ) ) continue;
	trim( ts[3] );

	if ( ts1 ) free( ts1 );
	ts1 = str_explode( ts[1], "\n" );
	for( i = 0 ; ts1[i] ; i++ )
		{
		if ( ts1[i][0] == 0 ) continue;
		#ifdef OS_UNIX
		if ( ts[3][0] )
			{ // sudo
			strdcpy( &s, "sudo "
			#ifndef OS_AIX
				"/logiciels"
			#endif
				"/ssie7/ssie7 su - ", ts[3], " -c \"\\\""
				"ls -l ", ts1[i],
				" | awk '{ if ((substr(\\\\\\$1,1,1) != \\\\\\\"d\\\\\\\")"
				"&&(substr(\\\\\\$1,1,1) != \\\\\\\"l\\\\\\\")&&( \\\\\\$9 != \\\\\\\"\\\\\\\" )) print \\\\\\$9 }'"
				"\\\"\"", _ );
			System( &u5, s );
			// on recupere le pid			
			System( &s, "echo $(($$-1))" );
			trim(s);
			trim( u5 );
			//Kill les process une fois l'information recuperee
			process_kill( atoi(s) );
			if (( stripos( u5, "No such file", 0 ) >= 0 )
					||( stripos( u5, "not found", 0 ) >= 0 ) )
				break;
			logw("robot_wf: pki_test pid=",s,_);
            }
		else
		#endif
		if ( file_count( ts1[i] ) == 0 ) break;
		if ( !file_is_free( ts1[i] ) ) break;	// Fichier occupé par un autre processus
//logw( "waitfile : file_count : ", ts1[i], " : ", itoa(file_count( ts1[i] )), _ );
		}

	if ( ts1[i] )
		{
		// Pas de fichier => Vérifie le timeout
		n = atoi( ts[0] );

		if ( ( !n )||( n >= time(NULL) ) ) continue;

		// Déclenche le processus avec le paramètre timeout

		strdcpy( &cmd, ts[4], " timeout", _ );
		logw( "robot wfile : TIMEOUT : ", cmd, _ );
		p1 = StartH( cmd );
		strdcpy( &s2, "running:", itoa( p1 ), ":", cmd, _ ),
		file_save_str( fname, s2, -1 );
		continue;
		}

	// Tous les fichiers sont arrivés

//	if (( ts[2][0] )&&( dir_exists( ts[2] ) ))
	if ( dir_exists( ts[2] ))
		{
		// Isolement des fichiers
		for( i = 0 ; ts1[i] ; i++ )
			{
			if ( ts1[i][0] == 0 ) continue;
            #ifdef OS_UNIX
			if ( ts[3][0] )
				{ // sudo
				strdcpy( &cmd, "sudo "
				#ifndef OS_AIX
					"/logiciels"
				#endif
					"/ssie7/ssie7 su - ", ts[3], " -c \"\\\""
					"ls -ld ", ts1[i],
					" | wc -l\\\"\"", _ );
    			System( &s, cmd );
    
    			if ( atoi(s) == 0 ) break;
				}
			else
            #endif
   				if ( file_count( ts1[i] ) == 0 ) break;

			#ifdef OS_WINDOWS
			file_move( ts1[i], ts[2] );
			#else
			strdcpy( &cmd, "sudo "
			#ifndef OS_AIX
				"/logiciels"
			#endif
				"/ssie7/ssie7 mv -f ", ts1[i], " ", ts[2], _ );
			System( &s, cmd );

			if ( ts[3][0] )
				{ // sudo
				strdcpy( &cmd, "sudo "
				#ifndef OS_AIX
					"/logiciels"
				#endif
					"/ssie7/ssie7 chown -R ", ts[3], ":", ts[3], " ", ts[2], _ );
				}
			else
				{ // sudo
				strdcpy( &cmd, "sudo "
				#ifndef OS_AIX
					"/logiciels"
				#endif
					"/ssie7/ssie7 chown -R ssie:ssie ", ts[2], _ );
				}

			System( &s, cmd );
			#endif
			}
		if ( file_count( ts[2] ) == 0 ) continue;
		}

	// Déclenche les traitements

	file_save_str( fname4, "running", -1 );
	flux_execute( t_work );
	
	















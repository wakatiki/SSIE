
#define bAscii				b4
#define bNoCrypt			b2


//------------------------------------------------------------------------------
int cmd_fdata_recv( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &s1, "FIN FICHIER : ", _ ); 
	c = time(NULL);
	n4 = 0;
	for( ; ; )
		{
		if ( bNoCrypt ) n = tcp_srecv0( hCnx, &s ); else n = tcp_srecv( hCnx, &s );
		if ( n == ERROR )
			{
			fclose( fp );
			ssie7disconnect( t_work );
			return ERROR; // Connexion perdue
			}
		
		// FPUT SERVEUR : TRAME DE FIN ? ==> sort de la boucle	
		if ( ( n < 300 )&&( ! strncmp( s, s1, -1 ) ) ) break;
		
		n2 = n;
		
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
				n4 += n3;
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
				n4 -= n3;
				}
			
			#endif
			}
			
		n3 = fwrite( fp, s, n, _ );
		if ( n3 != n )
			{
			// ERREUR Ecriture disque
			fclose( fp );
			print( "ERREUR écriture sur le disque", _ );
			return ERROR;
			}
		

		if ( c != time(NULL) ) // flush toutes les secondes
			{
			fflush( fp );
			c = time(NULL);
			}
		
		}
		
	fclose( fp );
	Sleep( 1 );
	
	//--------------------------------------------------------------------------
	// FPUT SERVEUR : CONTROLE D'INTEGRITE
	//--------------------------------------------------------------------------
	
	ll = atoi( &(s[strlen(s1)]) ) + n4;
	if ( file_size( fname ) != ll )
		{
strdcpy( &s2, "ERREUR dans les données : ", itoa( file_size( fname ) ), ", ", s, ", ", itoa(ll), _ );
		if ( request_mode == MODE_TCP )	print( s2, _ );
//		if ( request_mode == MODE_TCP )	print( "ERREUR dans les données", _ );
//		else tcp_ssend( hCnx, "ERREUR dans les données", -1 );
		else tcp_ssend( hCnx, s2, -1 );
		return ERROR;
		}
			
	if ( request_mode == MODE_TCP )	print( "OK", _ );
	else tcp_ssend( hCnx, "OK", 2 );
	
	return OK;
	}


//------------------------------------------------------------------------------
int cmd_fdata_send( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	ll = file_size( fname );

	r = OK;
	
//	while( n = fread( fp, s4, 96 Ko ) )
//	while( n = fread( fp, s4, 192 Ko ) )
//	while( n = fread( fp, s4, 384 Ko ) )
//	while( n = fread( fp, s4, 768 Ko ) )
	while( n = fread( fp, s4, 96 Ko ) )
		{
		#ifdef OS_WINDOWS
		if ( ( bAscii )&&( s4[n] == '\r' ) )
			{
			// VERIFIE QUE \r\n sont bien réunis à la fin du bloc
			// CHARGE LE \n qui manque peut-être
			if ( fread( fp, &(s4[n+1]), 1 ) )
				n++;
			}
		#endif
		
		if ( bNoCrypt ) r = tcp_ssend0( hCnx, s4, n ); else r = tcp_ssend( hCnx, s4, n );
		
		if ( r == ERROR )
			{
			print( "ERREUR : Connexion perdue (3)." BR, _ );
			ssie7disconnect( t_work );
			break;
			}

		}
	
	fclose( fp );
	if ( r == ERROR ) return ERROR;
	
	// FPUT CLIENT : ENVOI LA TRAME DE FIN
	
	strdcpy( &s3, "FIN FICHIER : ", itoa(ll), _ );
	if ( bNoCrypt ) tcp_ssend0( hCnx, s3, -1 ); else tcp_ssend( hCnx, s3, -1 );

	// ATTEND LA REPONSE

	if ( tcp_srecv( hCnx, &s1 ) == ERROR )
		{
		print( "ERREUR : Connexion perdue (4)." BR, _ );
		ssie7disconnect( t_work );
		return ERROR;
		}

	if ( strcmp( s1, "OK" ) )
		{
		print( cmd1, " : ERREUR : remote: ", s1, BR, _ );
		return ERROR;
		}
	
	return OK;
	}


	
#undef bAscii
#undef bNocrypt

















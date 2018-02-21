
#define bAscii				b4
#define bNoCrypt			b2

//int vdisk_server_exec_cmd( T_WORK * t_work );

//------------------------------------------------------------------------------
int cmd_vdisk_client( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	if ( v = var_get( hReq, "ip_cli" ) )
		strdcpy( &ip_cli, v, _ );
	strdcpy( &user, var_get( hReq, "user" ), _ );
	
	if ( ! var_getn( hReq, 2, &vn ) )
		{
		if ( ! var_getn( hReq, 1, &vn ) )
			{
			// AFFICHE LA LISTE DES VDISK
			print( "Liste des VDISK (à faire)", _ );
			return OK;
			}

		// MODE CONSOLE
		
		return OK;
		}
	

	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "set" ) )
	//--------------------------------------------------------------------------
		{
		// CREATION MODIFICATION D'UN VDISK
		
		var_getn( hReq, 1, &vn );
		if ( !is_alphanum( vn, "_" ) )
			{
			print( "ERREUR : vdisk, le nom n'est pas valide" BR, _ );
			return ERROR;
			}
		
		strdcpy( &fname, dirconf, DIR_CHAR "vdisk", _ );
		mkdir( fname );
		strdcat( &fname, DIR_CHAR, vn, ".dat", _ );
		
		var_free( h );
		if ( file_exists( fname ) ) h = var_load( fname );
		else h = var_new();
		for( i = 3 ; v = var_getn( hReq, i, &vn ) ; i++ )
			{
			if ( ! stricmp( vn, "rootdir" ) )
				{
				var_set( h, vn, v );
				continue;
				}
			else
			if ( ! stricmp( vn, "admin" ) )
				{
				// Liste des admin du disque
				upper(trim(v));
				v1 = var_get( h, "admin" );
				if ( v[0] == '-' )
					{
					if ( !v1 ) continue;
					strdcpy( &s, ",", &(v[1]), ",", _ );
					strdcpy( &s1, v1, _ );
					strd_replace( &s1, s, "," );
					strd_replace( &s1, ",,", "," );
					if ( s1[1] == 0 ) s1[0] = 0;
					var_set( h, vn, s1 );
					}
				else
				if ( v[0] == '+' )
					{
					if ( v1 )
						{
						strdcpy( &s, ",", &(v[1]), ",", _ );
						if ( strpos( v1, s, 0 ) >= 0 ) continue;
						strdcpy( &s1, v1, &(s[1]), _ );
						strd_replace( &s1, ",,", "," );
						}
					else
						strdcpy( &s1, &(v[1]), _ );
					var_set( h, vn, s1 );
					}
				else
					{
					strdcpy( &s1, ",", v, ",", _ );
					strd_replace( &s1, ",,", "," );
					if ( s1[1] == 0 ) s1[0] = 0;
					var_set( h, vn, s1 );
					}
				continue;
				}
			else
			if ( ! stricmp( vn, "shortcut" ) )
				{
				var_set( h, vn, v );
				continue;
				}
			else
				{
				print( "ERREUR : paramètre '", vn, "' inconnu", BR, _ );
				return ERROR;
				}
			}
			
		var_save( h, fname );
		print( "OK", _ );
		return OK;
		}
	else
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "info" ) )
	//--------------------------------------------------------------------------
		{
		// AFFICHE LES INFO D'UN VDISK
		
		var_getn( hReq, 1, &vn );
		
		strdcpy( &fname, dirconf, DIR_CHAR "vdisk" DIR_CHAR, vn, ".dat", _ );
		
		if ( !file_exists( fname ) )
			{
			print( "ERREUR : vdisk inexistant." BR, _ );
			return ERROR;
			}
		var_free( h );
		h = var_load( fname );
		if ( var_get( hReq, "of" ) )
			{
			var_to_js_array( h, &s );
			print( s, _ );
			return OK;
			}
		
		if ( v = var_get( h, "rootdir" ) )
			print( "   rootdir = ", v, BR, _ );
		if ( v = var_get( h, "admin" ) )
			print( "   admin   = ", v, BR, _ );
		if ( v = var_get( h, "shortcut" ) )
			print( "   shortcut = ", v, BR, _ );
		
		return OK;
		}
		
	else
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "delete" ) )
	//--------------------------------------------------------------------------
		{
		// SUPPRIME UN VDISK
		
		var_getn( hReq, 1, &vn );
		
		strdcpy( &fname, dirconf, DIR_CHAR "vdisk" DIR_CHAR, vn, ".dat", _ );
		
		if ( !file_exists( fname ) )
			{
			print( "ERREUR : vdisk inexistant." BR, _ );
			return ERROR;
			}
		file_delete( fname );
		print( "OK", _ );
		return OK;
		}
	
	//------------------------------------------------------------------------------------
	// ACCES A UN VDISK DISTANT
	// LECTURE DU NOM DE PARTENAIRE ET DU VDISK DISTANT
	
	var_getn( hReq, 1, &vn );
	if ( (p = strpos( vn, ":", 0 )) < 0 )
		{
		strdcpy( &fname, dirconf, DIR_CHAR "vdisk" DIR_CHAR, vn, ".dat", _ );
		if ( !file_exists( fname ) )
			{
			print( "ERREUR : vdisk inexistant." BR, _ );
			return ERROR;
			}
		var_free( h );
		h = var_load( fname );
		if ( !( v=var_get( hReq, "shortcut" ) ) )
			{
			print( "ERREUR : vdisk : shortcut non défini." BR, _ );
			return OK;
			}
		if ( (p = strpos( vn, ":", 0 )) < 0 )
			{
			print( "ERREUR : vdisk : shortcut sans nom de partenaire." BR, _ );
			return OK;
			}
		}
	
	partname = strdcpy( &part, substr( vn, 0, p ), _ );


	// MODE CLIENT : TRAITEMENT DE LA REQUETE

	var_getn( hReq, 2, &v2 );

	strdcpy( &cmd1, "vdisk_server ", vn, " ", v2, " ", _ );
	p0 = strlen( cmd1 );

	bNoCrypt = ( var_get( hReq, "nocrypt" ) ) ? true : false;
	
	//----------------------------------------------------------------------
	if ( !stricmp( v2, "put" ) )	// PUT CLIENT
	//----------------------------------------------------------------------
		{
		if ( ! var_getn( hReq, 3, &v1 ) )
			{
			print( "ERREUR : syntaxe fput {src_fname} [dest_fname] [ascii|binary]", _ );
			return ERROR;
			}
		
		bAscii = false;
		if ( ! var_getn( hReq, 4, &v2 ) )
			v2 = null;
		else
		if ( !stricmp( v2, "ascii" ) ) { bAscii = true; v2 = null; }
		if ( ( v2 )&&( var_get( hReq, "ascii" ) ) ) bAscii = true;
		

		// BOUCLE DE LECTURE DES FICHIERS A ENVOYER
		
		tf = NULL; n1 = 0; fp = NULL;
//		s4 = ( s4 ) ? realloc( s4, 128 Ko ) : alloc( 128 Ko );
//		s4 = ( s4 ) ? realloc( s4, 256 Ko ) : alloc( 256 Ko );
//		s4 = ( s4 ) ? realloc( s4, 512 Ko ) : alloc( 512 Ko );
//		s4 = ( s4 ) ? realloc( s4, 1024 Ko ) : alloc( 1024 Ko );
		s4 = ( s4 ) ? realloc( s4, 2048 Ko ) : alloc( 2048 Ko );
n4 = sysdate;

// 48/115 s
// vdisk SPENLX7I:ssie7upgrade fput c:/e/bin/linux/ssie7up_patch Linux/ssie7uprc


		while( tf = file_list( tf, v1, "absdir nodir" ) )
			{
			// CONNEXION SI BESOIN
				
			if ( ssie7connect( t_work, (v=var_get( hReq, "no_auth" ))?true:false ) == ERROR )
				{
				sleep(1);
				if ( ssie7connect( t_work, (v)?true:false ) == ERROR )
					{
					sleep(1);
					if ( ssie7connect( t_work, (v)?true:false ) == ERROR )
						{
						// PROBLEME DE CONNEXION
						print( error_msg, BR, _ );
						return ERROR;
						}
					}
				}
print( "connect OK" BR, _ );
			
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			if ( !v2 )	v3 = tf->name; else v3 = v2;
			
			print( "fput ", fname, " : ", _ );
			
			// FPUT CLIENT : OUVERTURE DU FICHIER
			
			fp = fopen( fname, "rb" );
			if ( fp == NULL )
				{
				print( "ERREUR : Pb ouverture fichier." BR, _ );
				continue;
				}
			
			// FPUT CLIENT : SOUMISSION DE LA REQUETE
			
			cmd1[p0] = 0;
			strdcat( &cmd1, v3, " ", tf->name, _ );
			#ifdef OS_WINDOWS
			if ( bAscii ) strdcat( &cmd1, " " OS_NAME, _ );
			#else
			if ( bAscii ) strdcat( &cmd1, " UNIX", _ );
			#endif
			if ( bNoCrypt )	strdcat( &cmd1, " nocrypt", _ );

			if ( tcp_ssend( hCnx, cmd1, -1 ) == ERROR )
				{
				print( "ERREUR : Connexion perdue (1)." BR, _ );
				fclose( fp );
				break;
				}
			
			if ( tcp_srecv( hCnx, &s1 ) == ERROR )
				{
				print( "ERREUR : Connexion perdue (2)." BR, _ );
				fclose( fp );
				break;
				}

			
			// VERIFIE QUE LE DISTANT ACCEPTE
			
			if ( strcmp( s1, "OK" ) )
				{
				print( s1, BR, _ );
				fclose( fp );
				continue;
				}
			
			// FPUT CLIENT : ENVOI DES DONNEES
			
			if ( cmd_fdata_send( t_work ) == ERROR )
				continue;
			
			print( "OK" BR, _ );
			n1++;
			}

//print( itoa( sysdate - n4 ), BR, _ );
		
		// FPUT CLIENT : FIN DES TRANSFERTS
			
		ssie7disconnect( t_work );
		v = (char*)(( n1 > 1 ) ? "s":"");
		print( itoa( n1 ), " fichier", v, " envoyé", v, _ );
		return OK;
		}

	
	//----------------------------------------------------------------------
	if ( !stricmp( v2, "get" ) )	// FGET CLIENT
	//----------------------------------------------------------------------
		{
		// CONNEXION
		
		if ( ssie7connect( t_work, (v=var_get( hReq, "no_auth" ))?true:false ) == ERROR )
			{
			sleep(1);
			if ( ssie7connect( t_work, (v)?true:false ) == ERROR )
				{
				sleep(1);
				if ( ssie7connect( t_work, (v)?true:false ) == ERROR )
					{
					// PROBLEME DE CONNEXION
					print( error_msg, BR, _ );
					return ERROR;
					}
				}
			}
		
		// LECTURE PARAMETRES
		
		if ( ! var_getn( hReq, 3, &v1 ) )
			{
			print( "ERREUR : syntaxe fget {src_fname} [dest_fname] [ascii|binary]", _ );
			return ERROR;
			}
		
		bAscii = false;
		if ( ! var_getn( hReq, 4, &v2 ) )
			v2 = null;
		else
		if ( !stricmp( v2, "ascii" ) ) { bAscii = true; v2 = null; }
		if ( ( v2 )&&( var_get( hReq, "ascii" ) ) ) bAscii = true;
		
		bNoCrypt = ( var_get( hReq, "nocrypt" ) ) ? true : false;

		// FGET CLIENT : SOUMISSION DE LA REQUETE
		
		cmd1[p0] = 0;
		strdcat( &cmd1, v1, _ );
		if ( bAscii )	strdcat( &cmd1, " " OS_NAME, _ );
		if ( bNoCrypt )	strdcat( &cmd1, " nocrypt", _ );
		
		if ( tcp_ssend( hCnx, cmd1, -1 ) == ERROR )
			{
			print( "ERREUR : Connexion perdue (1)." BR, _ );
			ssie7disconnect( t_work );
			return ERROR;
			}
		
		if ( tcp_srecv( hCnx, &s1 ) == ERROR )
			{
			print( "ERREUR : Connexion perdue (2)." BR, _ );
			ssie7disconnect( t_work );
			return ERROR;
			}
		
		// VERIFIE QUE LE DISTANT ACCEPTE
		
		if ( strcmp( s1, "OK" ) )
			{
			print( s1, BR, _ );
			ssie7disconnect( t_work );
			return ERROR;
			}
		
		// FGET CLIENT : BOUCLE DE RECEPTION DU OU DES FICHIERS
		
		for( n1 = 0 ; ; )
			{
			// ATTEND NOM DU FICHIER OU FIN FGET
			
			if ( tcp_srecv( hCnx, &s ) == ERROR )
				{
				print( "ERREUR : Connexion perdue (3 cli)." BR, _ );
				break;
				}
			
			if ( ! strcmp( s, "FIN FGET" ) ) break;
			if ( ! strncmp( s, "ERREUR", 6 ) )
				{
				print( "remote ", s, BR, _ );
				continue;
				}
			
			// CREATION DU FICHIER
			
			if ( v2 )
				{
				if ( dir_exists( v2 ) )
					strdcpy( &fname, v2, DIR_CHAR, s, _ );
				else
					strdcpy( &fname, v2, _ );
				}
			else
				strdcpy( &fname, s, _ );
			
			fp = fopen( fname, "wb" );
			if ( fp == NULL )
				{
				print( "ERREUR création du fichier ", fname, BR, _ );
				tcp_ssend( hCnx, "ERREUR création fichier", -1 );
				continue;
				}
				
			// FGET CLIENT : RECEPTION DES DONNEES
			
			strdcpy( &s1, "FIN FICHIER ", s, " : ", _ );
			if ( cmd_fdata_recv( t_work ) == ERROR )
				continue;
				
			print( "fget ", fname, " : OK" BR, _ );
			n1++;
			}
		
		ssie7disconnect( t_work );
		v = (char*)(( n1 > 1 ) ? "s":"");
		print( itoa( n1 ), " fichier", v, " reçu", v, _ );
		return OK;
		}
		
	//------------------------------------------------------------------------------------
	// MODE CLIENT : TRAITEMENT DES AUTRES REQUETES (autre que put et get)
	//------------------------------------------------------------------------------------
	
	strdcpy( &cmd1, cmd, _ );
	strd_replace( &cmd1, "vdisk ", "vdisk_server " );
	strd_ireplace( &cmd1, " no_auth ", " " );
	strd_ireplace( &cmd1, " no_auth", "" );

	if ( ssie7connect( t_work, (v=var_get( hReq, "no_auth" ))?true:false ) == ERROR )
		{
		sleep(1);
		if ( ssie7connect( t_work, (v)?true:false ) == ERROR )
			{
			sleep(1);
			if ( ssie7connect( t_work, (v)?true:false ) == ERROR )
				{
				// PROBLEME DE CONNEXION
				print( error_msg, BR, _ );
				return ERROR;
				}
			}
		}
	
	
	if ( tcp_ssend( hCnx, cmd1, -1 ) == ERROR )
		{
		print( "ERREUR : Connexion perdue (1).", _ );
		ssie7disconnect( t_work );
		return ERROR;
		}
	
	if ( tcp_srecv( hCnx, &s1 ) == ERROR )
		{
		print( "ERREUR : Connexion perdue (2).", _ );
		ssie7disconnect( t_work );
		return ERROR;
		}

	print( s1, BR, _ );
	
	// MODE CLIENT : DECONNEXION
	
	ssie7disconnect( t_work );
	return OK;
	}


	
#undef bAscii
#undef bNocrypt


















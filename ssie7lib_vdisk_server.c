
#define bAscii				b4
#define bNoCrypt			b2


int is_vdisk_autorised( T_WORK * t_work, char *right )
	{
	v = var_get( h, "admin" );
	if ( v )
		{
		strdcpy( &s, ",", nvl(session_partname,"NO_AUTH"), ",", _ );
		if ( stripos( v, s, 0 ) >= 0 ) return true;
		}
	strdcpy( &fname, dirconf, DIR_CHAR "vdisk" DIR_CHAR, v6, /* v6 doit contient le nom du vdisk*/
				".right.", right, ".", nvl(session_partname,"NO_AUTH"), ".cfg", _ );
	return file_exists( fname );
	}


//------------------------------------------------------------------------------
int cmd_vdisk_server( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	if ( request_mode != MODE_TCP )
		return ERROR;

	
	//------------------------------------------------------------------------------------
	// CHARGE LA CONFIG DU VDISK
	// LECTURE DU NOM DE PARTENAIRE ET DU VDISK DISTANT
	
	if ( ! var_getn( hReq, 1, &vn ) ) return ERROR;
	p = strpos( vn, ":", 0 ) + 1;
	v6 = &(vn[p]);

	strdcpy( &fname, dirconf, DIR_CHAR "vdisk" DIR_CHAR, v6, ".dat", _ );
	if ( !file_exists( fname ) )
		{
		print( "ERREUR : vdisk inexistant." BR, _ );
		return ERROR;
		}

	var_free( h );
	h = var_load( fname );
	if ( !( v5 = var_get( h, "rootdir" ) ) )
		{
		print( "ERREUR : vdisk : mal défini." BR, _ );
		return ERROR;
		}

	
	if ( ! var_getn( hReq, 2, &vn ) ) return ERROR;
	
	
	//------------------------------------------------------------------------------------
	// FPUT
	//---------------------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------------------
	if ( !stricmp( vn, "put" ) )
	//---------------------------------------------------------------------------------------
		{
		if ( ! is_vdisk_autorised( t_work, "write" ) )
			{
			if ( ! is_vdisk_autorised( t_work, "create" ) )
				{
				print( "ERREUR : droits insuffisants", _ );
				return ERROR;
				}
			}
		

		#ifdef OS_WINDOWS
		bAscii = (int)var_get( hReq, "unix" );
		#else
		bAscii = ( var_get( hReq, "windows" ) ) ? true : false;
		#endif
		
		bNoCrypt = ( var_get( hReq, "nocrypt" ) ) ? true : false;

		if ( ! var_getn( hReq, 3, &v1 ) ) return ERROR;
		v2 == null;
		var_getn( hReq, 4, &v2 );
		
		strdcpy( &fname, v5, DIR_CHAR, v1, _ );
		while( strpos( fname, "..", 0 ) >= 0 )
			strd_replace( &fname, "..", "." );

		if ( dir_exists( fname ) )
			{
			if ( !v2 )
				{
				print( "ERREUR : La destination est un répertoire", _ );
				return ERROR;
				}
			strdcat( &fname, DIR_CHAR, v2, _ );
			}

		// FPUT SERVEUR : OUVERTURE DU FICHIER
		
		fp = fopen( fname, "wb" );
		if ( fp == NULL )
			{
			print( "ERREUR : Pb création du fichier de réception ", fname, _ );
			return ERROR;
			}
		
		// FPUT SERVEUR : ACCEPTE LE TRANSFERT
		
		print( "OK", __ );
		
		
		// BOUCLE DE RECEPTION DES DONNEES
		
		return cmd_fdata_recv( t_work );
		}
	else
	//---------------------------------------------------------------------------------------
	if ( !strcmp( vn, "get" ) )
	//---------------------------------------------------------------------------------------
		{
		if ( ! is_vdisk_autorised( t_work, "read" ) )
			{
			print( "ERREUR : droits insuffisants", _ );
			return ERROR;
			}

		#ifdef OS_WINDOWS
		bAscii = (int)var_get( hReq, "unix" );
		#else
		bAscii = ( var_get( hReq, "windows" ) ) ? true : false;
		#endif
		
		bNoCrypt = ( var_get( hReq, "nocrypt" ) ) ? true : false;


		if ( ! var_getn( hReq, 3, &v1 ) ) return ERROR;
		
		strdcpy( &fname, v5, DIR_CHAR, v1, _ );
		while( strpos( fname, "..", 0 ) >= 0 )
			strd_replace( &fname, "..", "." );

		// ACCEPTE LA REQUETE
		
		print( "OK", __ );
		
		
		// BOUCLE DE LECTURE DES FICHIERS A ENVOYER
		
		n1 = 0; fp = NULL;
		s4 = ( s4 ) ? realloc( s4, 128 Ko ) : alloc( 128 Ko );
		strdcpy( &fname, var_get( h, "rootdir" ), DIR_CHAR, v1, _ );
		
		tf = NULL;
		while( tf = file_list( tf, fname, "nodir absdir" ) )
			{
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			
			// FGET CLIENT : OUVERTURE DU FICHIER
			
			fp = fopen( fname, "rb" );
			if ( fp == NULL )
				{
				print( "ERREUR : Pb ouverture fichier '", fname, "'" BR, _ );
				continue;
				}
			
			tcp_ssend( hCnx, tf->name, -1 );
			
			// ENVOI DES DONNES
			
			if ( cmd_fdata_send( t_work ) == ERROR )
				continue;
			}
		
		tcp_ssend( hCnx, "FIN FGET", 8 ); // Envoi info : plus de fichiers
		return OK;
		}
	
	//---------------------------------------------------------------------------------------
	// RIGHTS
	else
	//---------------------------------------------------------------------------------------
	if (( !stricmp( vn, "right" ) )||( !stricmp( vn, "rights" ) ))
	//---------------------------------------------------------------------------------------
		{
		// syntaxe : rights [set|del] read write create update delete rename mkdir rmdir rights part=...
		if ( var_get( hReq, "set" ) )
			{
			if ( ! is_vdisk_autorised( t_work, "setrights" ) )
				{
				print( "ERREUR : droits insuffisants", _ );
				return ERROR;
				}
				
			v1 = upper(var_get( hReq, "part" ));
			if ( !v1 )
				{
				print( "ERREUR ; paramètre part obligatoire", _ );
				return ERROR;
				}
			if ( ts ) free( ts );
			ts = str_explode( "read,write,create,update,rename,delete,mkdir,rmdir,setrights", "," );
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( var_get( hReq, ts[i] ) )
					{
					strdcpy( &fname, dirconf, DIR_CHAR "vdisk" DIR_CHAR, v6,
								".right.", ts[i], ".", v1, ".cfg", _ );
					file_save_str( fname, "", 0 );
					}
				}
			print( "OK", _ );
			return OK;
			}
		
		//-----------------------------------------------------------------------------------
		
		if (( var_get( hReq, "del" ) )||( var_get( hReq, "delete" ) ))
			{
			if ( ! is_vdisk_autorised( t_work, "setrights" ) )
				{
				print( "ERREUR : droits insuffisants", _ );
				return ERROR;
				}
				
			v1 = upper(var_get( hReq, "part" ));
			if ( !v1 )
				{
				print( "ERREUR ; paramètre part obligatoire", _ );
				return ERROR;
				}
			if ( ts ) free( ts );
			ts = str_explode( "read,write,create,update,rename,delete,mkdir,rmdir,setrights", "," );
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( var_get( hReq, ts[i] ) )
					{
					strdcpy( &fname, dirconf, DIR_CHAR "vdisk" DIR_CHAR, v6,
								".right.", ts[i], ".", v1, ".cfg", _ );
					file_delete( fname );
					}
				}
			print( "OK", _ );
			return OK;
			}

		//-----------------------------------------------------------------------------------
		
		if (( var_get( hReq, "list" ) )||( var_get( hReq, "get" ) ))
			{
			v1 = null;
			if ( is_vdisk_autorised( t_work, "setrights" ) )
				{
				v1 = upper(var_get( hReq, "part" ));
				}
			if ( !v1 )
				{
				v1 = nvl(session_partname,"NO_AUTH");
				}
			if ( ts ) free( ts );
			ts = str_explode( "read,write,create,update,rename,delete,mkdir,rmdir,setrights", "," );
			for( i = b = 0 ; ts[i] ; i++ )
				{
				strdcpy( &fname, dirconf, DIR_CHAR "vdisk" DIR_CHAR, v6,
							".right.", ts[i], ".", v1, ".cfg", _ );
				if ( file_exists( fname ) )
					{ print( ts[i], " ", _ ); b = true; }
				}
			if ( !b )
				{
				if ( is_vdisk_autorised( t_work, "setrights" ) )
					print( "admin", _ );
				else
					print( "none", _ );
				}
			return OK;
			}
		
		}
	//---------------------------------------------------------------------------------------
	// LS
	else
	//---------------------------------------------------------------------------------------
	if ( !stricmp( vn, "ls" ) )
	//---------------------------------------------------------------------------------------
		{
		// syntaxe : ls {chemin} nodir of=js:col list (name, size, date, type(file|dir))
		if ( ! is_vdisk_autorised( t_work, "read" ) )
			{
			print( "ERREUR : droits insuffisants", _ );
			return ERROR;
			}
		
		if ( var_getn( hReq, 3, &vn ) )
			{
			strdcpy( &fname, v5, DIR_CHAR, vn, _ );
			}
		else
			strdcpy( &fname, v5, _ );
		
		while( strpos( fname, "..", 0 ) >= 0 )
			strd_replace( &fname, "..", "." );
			
		if ( var_get( hReq, "nodir" ) )
			v4 = "absdir nodir";
		else
			v4 = "absdir";
		
		v3 = var_get( hReq, "size" );
		v2 = var_get( hReq, "date" );
		v1 = var_get( hReq, "count" );
		
		tf = NULL; if ( v1 ) ll = 0;
		while( tf = file_list( tf, fname, v4 ) )
			{
			if ( v1 )
				{
				if ( v3 ) ll += file_size( strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ ) );
				else ll++;
				}
			print( BR, tf->name, _ );
			if ( v3 || v4 )
				{
				strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
				if ( v3 ) print( " ", itoa( file_size( fname ) ), _ );
				if ( v2 ) print( " ", itod( file_date( fname ), "jj/mm/aaaa hh:mm:ss" ), _ );
				}
			
			if ( tf->is_dir ) print( " (dir)", _ );
			}
		
		if ( v1 ) print( itoa( ll ), _ );
		return OK;
		}
	//---------------------------------------------------------------------------------------
	// rm
	else
	//---------------------------------------------------------------------------------------
	if ( !stricmp( vn, "rm" ) )
	//---------------------------------------------------------------------------------------
		{
		// syntaxe : rm {chemin} -R
		if ( ! is_vdisk_autorised( t_work, "delete" ) )
			{
			print( "ERREUR : droits insuffisants", _ );
			return ERROR;
			}
		
		if ( ! var_getn( hReq, 3, &vn ) )
			{
			print( "ERREUR : nom de fichier obligatoire", _ );
			return ERROR;
			}
		strdcpy( &fname, v5, DIR_CHAR, vn, _ );
		
		while( strpos( fname, "..", 0 ) >= 0 )
			strd_replace( &fname, "..", "." );
		
		if ( v3 = var_get( hReq, "-R" ) )
			v4 = "absdir";
		else
			v4 = "absdir nodir";
		
		tf = NULL; if ( v1 ) ll = 0;
		while( tf = file_list( tf, fname, v4 ) )
			{
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			file_delete( fname );
			print( tf->name, " deleted" BR, _ );
			ll++;
			}
		
		print( BR,itoa( ll ), " file", (char*)((ll>1)?"s":""), " deleted", _ );
		return OK;
		}
	//---------------------------------------------------------------------------------------
	// mkdir
	else
	//---------------------------------------------------------------------------------------
	if ( !stricmp( vn, "mkdir" ) )
	//---------------------------------------------------------------------------------------
		{
		// syntaxe : mkdir {chemin}
		if ( ! is_vdisk_autorised( t_work, "mkdir" ) )
			{
			print( "ERREUR : droits insuffisants", _ );
			return ERROR;
			}
		
		if ( ! var_getn( hReq, 3, &vn ) )
			{
			print( "ERREUR : nom de fichier obligatoire", _ );
			return ERROR;
			}
		strdcpy( &fname, v5, DIR_CHAR, vn, _ );

		while( strpos( fname, "..", 0 ) >= 0 )
			strd_replace( &fname, "..", "." );

		mkdir( fname );
		if ( dir_exists( fname ) )
			print( "OK", _ );
		else
			print( "ERREUR", _ );
			
		return OK;
		}
	//---------------------------------------------------------------------------------------
	// rmdir
	else
	//---------------------------------------------------------------------------------------
	if ( !stricmp( vn, "rmdir" ) )
	//---------------------------------------------------------------------------------------
		{
		// syntaxe : mkdir {chemin}
		if ( ! is_vdisk_autorised( t_work, "rmdir" ) )
			{
			print( "ERREUR : droits insuffisants", _ );
			return ERROR;
			}
		
		if ( ! var_getn( hReq, 3, &vn ) )
			{
			print( "ERREUR : nom de fichier obligatoire", _ );
			return ERROR;
			}
		strdcpy( &fname, v5, DIR_CHAR, vn, _ );
		file_delete( fname );
		if ( dir_exists( fname ) )
			print( "ERREUR", _ );
		else
			print( "OK", _ );
			
		return OK;
		}
			
		
	print( "ERREUR : Commande inconnue.", _ );
	return ERROR;
	}



	
#undef bAscii
#undef bNoCrypt














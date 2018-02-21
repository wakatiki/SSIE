
//------------------------------------------------------------------------------
int migration_mersi3( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	char *mersi3_appli_dir=null, *w1 = null;
	int nid;
	
	print( BR BR "--------------------------------------------------------------------------------", BR, _ );
	print( "MIGRATION MERSI3 => SSIE v7" , BR, _ );
	
	// DONNE LES DROITS D'ACCES A SSIE A TOUS LES FICHIERS MERSI
	print( BR "PREPARATIONS" BR, _ );
	
	strdcpy( &mersi3_appli_dir, "/appli/mersi3", _ );

	strdcpy( &cmd, "mv /appli/mersi3/log /appli/mersi3/log.sav", _ );
	system( cmd );

	strdcpy( &cmd, "mkdir /appli/mersi3/log", _ );
	system( cmd );
	
	strdcpy( &cmd, "chown mersi:mersi /appli/mersi3/log", _ );
	system( cmd );

//	strdcpy( &cmd, "chmod -R 777 /appli/mersi3/*", _ );
//	system( cmd );


	if ( ! creation_arbo_ssie( t_work ) )	
		{
		print( " -> ", "Problème lors de la création de l'arborescence de SSIE v7.", BR, BR, _ );
		return -1;
		}
	
	
	//-----------------------------------------------------------------------------------
	// RECUPERATION DES LOGIN	
	
	print( BR "RECUPERATION DES LOGIN" BR, _ );

	strdcpy( &w, mersi3_appli_dir, "/conf/login", _ );
	if ( dir_exists( w ) )
		{
		strdcpy( &cmd, "ls ", w, _ );
		System( &s, cmd );
		if ( trim(s)[0] )
			{
			strdcpy( &cmd, "cp -Rf ", w, "/* /appli/ssie7/conf/login" , _ );
			System( &s, cmd );
			print( cmd, BR, s, BR, _ );
			if ( trim(s)[0] )
				{ r = 1; goto fin; }
			}
		}     
	
	//-----------------------------------------------------------------------------------
	// RECUPERATION DES REVEILS
	
	print( BR "RECUPERATION DES REVEILS" BR, _ );
	strdcpy( &w, mersi3_appli_dir, "/data/reveils", _ );
	if ( dir_exists( w ) )
		{
		strdcpy( &cmd, "ls ", w, _ );
		System( &s, cmd );
		if ( trim(s)[0] )
			{
			strdcpy( &cmd, "cp -Rf ", w, "/* /appli/ssie7/conf/reveils" , _ );
			System( &s, cmd );
			print( cmd, BR, s, BR, _ );
			if ( trim(s)[0] )
				{ r = 1; goto fin; }
			}
		}	
	
	
	//-----------------------------------------------------------------------------------
	// RECUPERATION DES PARTENAIRES
	
	print( BR "RECUPERATION DES PARTENAIRES" BR, _ );

	strdcpy( &fname, mersi3_appli_dir, "/conf/mersi3.dat", _ );
	if ( ! file_exists( fname ) )
		{
		print( " -> ", "Le fichier ", fname, " n'existe pas ou est inacessible.", BR BR, _ );
		r = 1; goto fin;
		}
	h = var_load( fname );
	v = var_get( h, "srvnat_list" );
	if ( ( v )&&( ! stricmp( v, "none" ) ) ) bDMZ = true;
	
	
	tf = NULL;
	
	strdcpy( &w, mersi3_appli_dir, "/conf/partenaires/*", _ );
	while( tf = file_list( tf, w, "nodir" ) )
		{
		if ( (p=strpos( tf->name, ".dat", 0 )) < 0 ) continue;
		v1 = strdncpy( &w1, tf->name, p, _ );
		strdcpy( &fname, mersi3_appli_dir, "/conf/partenaires/", v1, ".dat", _ );
		h = var_load( fname );
		var_to_str( h, &s );
		var_free( h );
		strd_replace( &s, "part_", "" );

		h = var_from_cmd( 0, s );
		var_set( h, "partname", v1 );
		v = var_get( h, "direction" );
		if ( v )
			{
			var_set( h, "trf_direction", v );
			var_delete( h, "direction" );
			}

		v = var_get( h, "ssl_enable" );
		if ( v )
			{
			switch ( atoi(v) )
				{
				case 2 :
					v = var_get( h, "port_ssl2" );
					if ( (v)&&( v[0] ) )
						var_set( h, "port_mft", v );
					break;
				case 1 :
					v = var_get( h, "port_ssl" );
					if ( (v)&&( v[0] ) )
						var_set( h, "port_mft", v );
					break;
				}
			}
		var_delete( h, "port_ssl2" );
		var_delete( h, "port_ssl" );
		
		
		strdcpy( &fname, "/appli/ssie7/conf/part/", v1, ".dat", _ );
		var_save( h, fname );

		print( "    ", " - '", v1, "' depuis le fichier de configuration local : ", _ );
//print( BR, s, BR, _ );		
		
		// RECUPERATION PARAMETRAGE SSL
		
		if (( v = trim(var_get( h, "ssl_certifr" )) )&&( v[0] ))
			{
			if ( !strnicmp( v, "der:", 4 ) )
				v = der_to_pem( v );
				
			strdcpy( &cmd, "part ", v1, " ssl=", quote( v ), _ );
			file_tmpname( &fname );
			file_save_str( fname, cmd, -1 );
			
			strdcpy( &cmd1, LOGICIELS "/ssie7/ssie7 @", fname, _ );
			System( &s, cmd1 );

//print( BR, cmd, BR, s, BR, _ );		

			if ( stripos( s, "erreur", 0 ) >= 0 )
				print( " -> ", "Paramétrage SSL :", BR,
					"part ", v1, " ssl=..." BR, trim(s), BR, BR, _ );

			var_free( h );
			strdcpy( &fname, "/appli/ssie7/conf/part/", v1, ".dat", _ );
			h = var_load( fname );
			var_delete( h, "ssl_certifr" );
			var_delete( h, "error_msg" );
			
			var_save( h, fname );
			}

		
		var_free( h );
		
//		print( "    ", " - '", v1, "' depuis le fichier de configuration local : OK.", BR, _ );
		print( "OK.", BR, _ );
		
		strdcpy( &fname, "/appli/mersi3/conf/partenaires/", v1, ".no_maj_nat", _ );
		if ( file_exists( fname ) )
			{
			strdcpy( &cmd, ssie7_prefix, "part ", v1, " no_maj_nat", _ );
			System( &s, cmd );
			}
		else
		if ( !bDMZ )
			{
			strdcpy( &cmd, ssie7_prefix, "part ", v1, " loadnat", _ );
			System( &s, cmd );
			}
		}
	
	if ( !bDMZ )
		{
		strdcpy( &cmd, ssie7_prefix, "part MFTCCWEX loadnat", _ );
		System( &s, cmd );
		}
	
	//-----------------------------------------------------------------------------------
	// RECUPERATION DES EXPORTS
	
	print( BR "RECUPERATION DES EXPORTS" BR, _ );
	
	strdcpy( &w, mersi3_appli_dir, "/data/export", _ );
	if ( dir_exists( w ) )
		{
		strdcpy( &cmd, "ls ", w, _ );
		System( &s, cmd );
		if ( trim(s)[0] )
			{
			if ( ! dir_exists( "/appli/ssie7/data/export" ) )
			mkdir( "/appli/ssie7/data/export" );
			
			// COPIE DES FICHIERS
			strdcpy( &cmd, "cp -Rf ", w, "/* /appli/ssie7/data/export", _ );
			System( &s, cmd );
			if ( trim(s)[0] )
				{
				print( " -> ", "Récupération des exports : ERREUR : ", s, BR, _ );
				r = 1; goto fin;
				}
			
			// CONVERSION DES FICHIER ID
			tf = NULL;
			strdcpy( &w, "/appli/ssie7/data/export/*.id", _ );
			while( tf = file_list( tf, w, "nodir" ) )
				{
				strdcpy( &fname, "/appli/ssie7/data/export/", tf->name, _ );
				file_load( &s, fname );
				nid = atoi( s );
				fp = fopen( fname, "wb" );
				if ( fp == null )
					{
					print( "ERREUR : création fichier ", fname, BR, _ );
					continue;
					}
				fwrite( fp, &nid, sizeof(nid), _ );
				fclose( fp );
				}
			
			
			// FIN
			print( " -> ", "Récupération des exports : OK" BR, _ );
			}
		}
	
	//-----------------------------------------------------------------------------------
	// RECUPERATION DES IMPORTS
	
	print( BR "RECUPERATION DES IMPORTS" BR, _ );

	strdcpy( &w, mersi3_appli_dir, "/data/import", _ );
	if ( dir_exists( w ) )
		{
		strdcpy( &cmd, "ls ", w, _ );
		System( &s, cmd );
		if ( trim(s)[0] )
			{
			if ( ! dir_exists( "/appli/ssie7/data/import" ) )
			mkdir( "/appli/ssie7/data/import" );
			
			// COPIE DES FICHIERS
			strdcpy( &cmd, "cp -Rf ", w, "/* /appli/ssie7/data/import", _ );
			System( &s, cmd );
			if ( trim(s)[0] )
				{
				print( " -> ", "Récupération des imports : ERREUR : ", s, BR, _ );
				r = 1; goto fin;
				}
			
			// CONVERSION DES FICHIER ID
			tf = NULL;
			strdcpy( &w, "/appli/ssie7/data/import/*.id", _ );
			while( tf = file_list( tf, w, "nodir" ) )
				{
				strdcpy( &fname, "/appli/ssie7/data/import/", tf->name, _ );
				file_load( &s, fname );
				nid = atoi( s );
				fp = fopen( fname, "wb" );
				if ( fp == null )
					{
					print( "ERREUR : création fichier ", fname, BR, _ );
					continue;
					}
				fwrite( fp, &nid, sizeof(nid), _ );
				fclose( fp );
				}
			
			
			// FIN
			}
		}
	
	//-----------------------------------------------------------------------------------
	// RECUPERATION DES FLUX
	
	if ( migration_flux_mersi3( t_work ) ) goto fin;
	
	
	//-----------------------------------------------------------------------------------
	// RECUPERATION DES STATISTIQUES
	
	print( BR "RECUPERATION DES STATISTIQUES" BR, _ );
	
	strdcpy( &w, mersi3_appli_dir, "/data/report", _ );
	if ( dir_exists( w ) )
		{
		strdcpy( &cmd, "ls ", w, _ );
		System( &s, cmd );
		if ( trim(s)[0] )
			{
			if ( ! dir_exists( "/appli/ssie7/data/report" ) )
				mkdir( "/appli/ssie7/data/report" );
			strdcpy( &cmd, "cp -Rf ", w, " /appli/ssie7/data", _ );
			System( &s, cmd );
			if ( trim(s)[0] )
				{ r = 1;
				print( "ERREUR : ", cmd, BR, s, BR BR, _ );
				goto fin;
				}
			}
		}
	
	//-----------------------------------------------------------------------------------
	// RECUPERATION DE LA CONF SERVEUR
	
	print( BR "RECUPERATION CONFIGURATION SERVEUR" BR, _ );
	
	strdcpy( &fname, mersi3_appli_dir, "/conf/mersi3.dat", _ );
	if ( ! file_exists( fname ) )
		{
		print( " -> ", "ERREUR : Le fichier ", fname, " n'existe pas ou est inacessible.", BR BR, _ );
		r = 1; goto fin;
		}
	h = var_load( fname );
	v = var_get( h, "srvnat_list" );
	if ( ( v )&&( ! stricmp( v, "none" ) ) )
		{
		strdcpy( &cmd, ssie7_prefix, "srvannu list=none", _ );
		System( &s, cmd );
		}


	strdcpy( &cmd, "server partname=", var_get( h, "srv_part_name" ),
				" ip=", quote( var_get( h, "srv_ip" ) ),
				" dns=", quote( var_get( h, "srv_dns" ) ),
				" port=", quote( var_get( h, "srv_port" ) ),
				" descr=", quote( var_get( h, "srv_descr" ) ),
				" adl_liste=", quote( var_get( h, "srv_adl_liste" ) ),
				_ );
	
	file_save_str( "/tmp/ssie7_migr_conf", cmd, -1 );
	strdcpy( &s1, ssie7_prefix, "@/tmp/ssie7_migr_conf", _ );
	System( &s, s1 );
	if ( stripos( s, "erreur", 0 ) >= 0 )
		print( " -> ", "Paramétrage du serveur SSIE v7 :", BR,
		cmd, BR, trim(s), BR, BR, _ );
	else
		print( " -> ", "Paramétrage du serveur SSIE v7 : OK", BR, _ );
		
	
	// RECUPERATION PARAMETRAGE SSL
	
	if ( v = var_get( h, "srv_ssl_certifp" ) )
		{
		strdcpy( &s, v, BR, _ );
		if ( v = var_get( h, "srv_ssl_certifu" ) )
			strdcat( &s, BR, v, BR, _ );
		if ( v = var_get( h, "srv_ssl_certifi1" ) )
			strdcat( &s, BR, v, BR, _ );
		if ( v = var_get( h, "srv_ssl_certifi2" ) )
			strdcat( &s, BR, v, BR, _ );
		if ( v = var_get( h, "srv_ssl_certifi3" ) )
			strdcat( &s, BR, v, BR, _ );
		if ( v = var_get( h, "srv_ssl_certifi4" ) )
			strdcat( &s, BR, v, BR, _ );
		if ( v = var_get( h, "srv_ssl_certifr" ) )
			strdcat( &s, BR, v, BR, _ );
		
		strdcpy( &cmd, "server ssl=", quote( s ), _ );
		file_tmpname( &fname );
		file_save_str( fname, cmd, -1 );
		
		strdcpy( &cmd, "ssie7 @", fname, _ );
		System( &s, cmd );
		
		if ( stripos( s, "erreur", 0 ) >= 0 )
			print( " -> ", "Paramétrage du serveur SSIE v7 SSL :", BR,
			"ssie7 server ssl=..." BR, trim(s), BR, BR, _ );
		else
			print( " -> ", "Paramétrage du serveur SSIE v7 SSL : OK", BR, _ );
			
		}
	
	
	// RECUPERATION CLEF MFT
	
	if (( file_exists( LOGICIELS "/ssie7/mft" ) )&&( dir_exists( LOGICIELS "/ssie7/mftbin" ) ))
		{
		print( BR "RECUPERATION CONF MFT" BR, _ );
		strdcpy( &cmd, LOGICIELS "/ssie7/mft conf key=", var_get( h, "srv_mft_key" ),
					", port_mft='", var_get( h, "srv_port_mft" ), "'",
					", port_ssl='", var_get( h, "srv_port_ssl" ), "'",
					", port_ssl2='", var_get( h, "srv_port_ssl2" ), "'",
					", maxtran='", var_get( h, "srv_mft_maxtran" ), "'",
					_ );
		System( &s, cmd );
		}
	
	
	// DESACTIVATION DE MERSI3 pour obliger la commande back si on veut un retour arrière

	print( BR "DESACTIVATION DE MERSI3" BR, _ );
	
	System( &s, "mv -f " LOGICIELS "/mersi3/mersi3 " SAUVE_DIR );
	System( &s, "chown ssie:ssie " SAUVE_DIR "/mersi3" );
	if ( bKeepUserMersi )
		{
		fp = fopen( LOGICIELS "/mersi3/mersi3", "wb" );
		if ( !fp )
			print( "ERREUR : Problème pour créer le fichier " LOGICIELS "/mersi3/mersi3" BR, _ );
		else
			{
			fprint( fp, LOGICIELS "/ssie7/ssie7 $*" BR, _ );
			fclose( fp );
			System( &s, "chown mersi:mersi " LOGICIELS "/mersi3/mersi3" );
			System( &s, "chmod 755 " LOGICIELS "/mersi3/mersi3" );
			}
		}
	
	//-----------------------------------------------------------------------------------
	
	fin:
	
	// REMET LES DROITS MERSI COMME A L'ORIGINE

	strdcpy( &cmd, "rm -Rf /appli/mersi3/log", _ );
	system( cmd );
	
	strdcpy( &cmd, "mv /appli/mersi3/log.sav /appli/mersi3/log", _ );
	system( cmd );

	strdcpy( &cmd, "chown -R mersi:mersi /appli/mersi3/log", _ );	
	System( &s, cmd );

	
	// FIN
	
	print( BR "MIGRATION MERSI v3 ==> SSIE v7 : TERMINEE." BR
			BR "ATTENTION, pour revenir à mersi v3," BR
			"vous devez obligatoirement passer la commande : ssie7up backM" BR BR
			"--------------------------------------------------------------------------------", BR, BR, _ );
	
	return OK;
	}

//-----------------------------------------------------------------------------------
	

int migration_flux_mersi3( T_WORK *t_work )
	{
	char *_ssie_flux_dir="/appli/ssie7/conf/flux";
	char *_mersi3_appli_dir=null;
	char *_wrk_dir=null, *_wrk_file_pattern=null;
	char *_wrk_date=null;
	char *_tmp_dir=null;
	int _p_deb=0, _p_fin=0, _sav_id=0;
	char *ssie7_prefix=null;
	char *ssie7up_prefix=null;
	char *sudo_ssie7_prefix=null;
	char *mersi3_prefix=null;
	char **ts3=null, **ts4=null;
	char *fname5=null;
	int uid = 0, mersi3_uid=0;
	HANDLE fd;
	FILE *ptr_file;
	char *buf;
	
	#ifdef OS_AIX
	strdcpy( &ssie7_prefix, "/ssie7/ssie7 ", _ );
	strdcpy( &ssie7up_prefix, "/ssie7/ssie7up ", _ );
	strdcpy( &mersi3_prefix, "/mersi3/mersi3 ", _ );
	#elif OS_LINUX
	strdcpy( &ssie7_prefix, "/logiciels/ssie7/ssie7 ", _ );
	strdcpy( &ssie7up_prefix, "/logiciels/ssie7/ssie7up ", _ );
	strdcpy( &mersi3_prefix, "/logiciels/mersi3/mersi3 ", _ );
	#endif
	strdcpy( &sudo_ssie7_prefix, "sudo ", ssie7_prefix, _ );
	
	strdcpy( &_mersi3_appli_dir, "/appli/mersi3", _ );
	
	strdcpy( &_tmp_dir, "/appli/ssie7/tmp", _ );
	if ( ! dir_exists( _tmp_dir ) )
		{
		mkdir(_tmp_dir);
		System( &s, "chmod 777 /appli/ssie7/tmp" );
		}

	
	//--------------------------------------------------------------------------
	// COPIE DES FICHIERS DE DEFINITION DES FLUX DE MERSI VERS SSIE
	
	print( BR "RECUPERATION DE LA DEFINITION DES FLUX" BR, _ );
	
	strdcpy( &_wrk_dir, _mersi3_appli_dir, "/conf/applications", _ );
	if ( dir_exists( _wrk_dir ) )
		{
		strdcpy( &cmd, "ls ", _wrk_dir, _ );
		System( &s, cmd );
		if ( trim(s)[0] )
			{
			strdcpy( &cmd, "cp -Rf ", _wrk_dir, "/* /appli/ssie7/conf/flux", _ );
			System( &s, cmd );
			print( cmd, BR, s, BR BR, _ );
			
			if ( trim(s)[0] )
				{
				r = 1; 
				return r;
				}
			}
		}
	
	//--------------------------------------------------------------------------
	// ADAPTATION DES FLUX POUR SSIE
	
	print( "    ", "Adaptation pour SSIE v7", BR, _ );
	tf = NULL;
	while( tf = file_list( tf, "/appli/ssie7/conf/flux/*", "nodir" ) )
		{
		if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;
		strdcpy( &fname, "/appli/ssie7/conf/flux/", tf->name, _ );
		
		// REMPLACE LES ':' par '@' DANS LES NOM DE FICHIERS
		if ( ( p = strpos( tf->name, ":", 0 ) ) >= 0 )
			{
			fname1 = str_replace( fname, ":", "@" );
			rename( fname, fname1 );
			fname = fname1;
			}
		
		// ETAPES REC : REMPLACE ':' PAR '=>'
		
		h1 = var_load( fname );
		v = var_get( h1, "agr_start_cond" );
		if ( v )
			{
			_p_deb = 0;
			if ( ( _p_deb = strpos( v, "[", 0 ) ) >= 0 ) 
				{
				if ( ( _p_fin = strpos( v, "]", _p_deb + 1 ) ) >= 0 )
					{
					n = atoi( &(v[_p_deb+1]) );
					if ( n == 1 )
						var_set( h1, "agr_one_by_one", "Oui" );
					else
						var_set( h1, "agr_one_by_one", "Non" );
					
					strdcpy( &v, substr( v, _p_fin + 1, -1 ), _ );
					}
				}
			v = str_replace( trim( v ), ":", "=>" );
			var_set( h1, "agr_start_cond", v );
			}
		
//		if ( !bKeepUserMersi )
			{
			// ETAPES SCRIPTS : REMPLACE "mersi3 flux" PAR "ssie7 flux"
			
			v = var_get( h1, "etape_script" );
			if ( v )
				{
				v = str_replace( v, "/ssie/", "/mersi/" );
				v = str_replace( v, "mersi3 flux", "ssie7 flux" );
				v = str_replace( v, "/mersi3", "/ssie7" );
				v = str_replace( v, "mersi3 ", "ssie7 " );
				var_set( h1, "etape_script", v );
				}
			
			v = var_get( h1, "agr_script" );
			if ( v )
				{
				v = str_replace( v, "/ssie/", "/mersi/" );
				v = str_replace( v, "mersi3 flux", "ssie7 flux" );
				v = str_replace( v, "/mersi3", "/ssie7" );
				v = str_replace( v, "mersi3 ", "ssie7 " );
				var_set( h1, "agr_script", v );
				}
			}
		
		if ( ! var_get( h1, "agr_one_by_one" ) )
			var_set( h1, "agr_one_by_one", "Oui" );

		if (( v = var_get( h1, "app_login" ) )&&( !strcmp( v, "mersi" ) ))
			var_set( h1, "app_login", "" );
			
		var_save( h1, fname );
		} 
	
	//--------------------------------------------------------------------------
	// RECUPERATION DES INSTANCES ET LOG DES FLUX

	
	h=0;
	strdcpy( &fname3, "/appli/mersi3/data/flux/flux.id", _ );
	file_load( &s2, fname3 );
	mersi3_uid=atoi(s2) + 1;
	//print( "mersi_flux_migration :", " mersi3_uid=", itoa(mersi3_uid), ".", BR, _ );    
	strdcpy( &_wrk_dir, _mersi3_appli_dir, "/data/flux", _ );

//	if ( false )
	if ( dir_exists( _wrk_dir ) )
		{
		print( BR BR "RECUPERATION HISTORIQUE DES EXECUTIONS DES FLUX" BR, _ );
		print( "    ", "l'opération peut prendre plusieurs minutes...", BR BR, _ );
		
		mkdir("/appli/ssie7/data/flux");
		strdcpy( &s2, "/appli/ssie7/data/flux/log.id", _ );
		
		strdcpy( &fname3, _tmp_dir, "/ssie7up.migration.", itoa(getpid()), _ );
		
		// LECTURE HISTORIQUE DES 30 DERNIERS JOURS
		
		#ifdef OS_AIX
		putenv( "USER=mersi" );
		putenv( "LOGIN=mersi" );
		#else
		putenv( "LOGNAME=mersi" );
		#endif
		
		print( "    ", "-> Chargement de l'historique", BR, _ );

//		for( i=30 ; i >= 0 ; i-- )
		for( i=60 ; i >= 0 ; i-- )
			{
			_wrk_date = itod( time(null)-(i*24*60*60), "aaaa_mm_jj" );
			//            strdcpy( &cmd, sudo_ssie7_prefix, "su - mersi -c \"", mersi3_prefix, "flux log date=", _wrk_date, " >> ", fname3, "\"", _ );
			strdcpy( &cmd, mersi3_prefix, "flux log date=", _wrk_date, " >> ", fname3, _ );
			system( cmd );
			}

		#ifdef OS_AIX
		putenv( "USER=ssie" );
		putenv( "LOGIN=ssie" );
		#else
		putenv( "LOGNAME=ssie" );
		#endif

		
		// TRAITEMENT UNITAIRE
		
		buf = alloc0(8 Ko);
		ptr_file =fopen(fname3,"r");
		while (fgets(buf, 4096*2, ptr_file)!=NULL)
			{  		
			trim(buf);
			if (( !buf )||( ! (buf[0]) ))
				continue;
			if ( ts1 ) { free( ts1 ); ts1=null; }
			ts1 = str_explode( buf, " : " );

			strdcpy( &s4, "/appli/ssie7/data/flux/", ts1[0], ".dat", _ );
			if ( file_exists( s4 ) ) continue;
	
			print( "    ", "-> flux : ", buf, BR, __ );

			// SI LE FLUX ETAIT EN COURS => ON LE REDEMARRE DANS SSIE7
			
			strdcpy( &fname5, "/appli/mersi3/data/flux/waiting/", ts1[0], _ );
			if ( file_exists( fname5 ) )
				{
				strdcpy( &s4, _wrk_dir, DIR_CHAR, ts1[0], ".dat", _ );
				
				if ( h ) { var_free( h ); h=0; }
				h = var_load( s4 );
				
				print("    ", "    -> REDEMARRAGE du flux" BR, _ );
	
				#ifdef OS_AIX
				putenv( "USER=ssie" );
				putenv( "LOGIN=ssie" );
				#else
				putenv( "LOGNAME=ssie" );
				#endif
				
				set_uid( s2, mersi3_uid );
				mersi3_uid++;
				
	
				#ifdef OS_AIX
				//                 strdcpy( &cmd, "/ssie7/ssie7 flux ", ts1[0], " start", _ );
				strdcpy( &cmd, "/ssie7/ssie7", _ );
				#else
				//                 strdcpy( &cmd, "/logiciels/ssie7/ssie7 flux ", ts1[0], " start", _ );
				strdcpy( &cmd, "/logiciels/ssie7/ssie7", _ );
				#endif
				strdcpy( &cmd, cmd, " flux ", var_get( h, "app_name" ), " ", str_replace( var_get( h, "flux_name" ), ":", "/" ), " start", _ );
				
				System( &s, cmd );
				
				print( cmd, BR, s, BR, _ );
				
				continue;
				}

//continue;
			
			
			// COPIE DU FICHIER {flux_id}.dat
			
			strdcpy( &cmd, "cp ", _wrk_dir, DIR_CHAR, ts1[0], ".dat /appli/ssie7/data/flux", _ );
			system( cmd );
			
			// ADAPTATION POUR SSIE7
			strdcpy( &s4, "/appli/ssie7/data/flux/", ts1[0], ".dat", _ );
			if ( !file_exists( s4 ) )		continue;
			
			if ( h ) { var_free( h ); h=0; }
			h = var_load( s4 );
			
			// REMPLACE ':' PAR "@"
			
			var_set( h, "flux_name", str_replace( var_get( h, "flux_name" ), ":", "@" ) );


			// VERIFIE LE STATUS (NE DOIT PAS ETRE EN COURS)
			v = var_get( h, "status" );
			if ( ( strnicmp( v, "ERREUR", -1 ) )
				&&( strnicmp( v, "ERROR", -1 ) )
				&&( strnicmp( v, "OK", -1 ) )
				&&( strnicmp( v, "ALERT", -1 ) )
				&&( strnicmp( v, "ANNUL", -1 ) )
				)
				var_set( h, "status", "ANNULE" );


			var_save( h, s4 );
			
			
			// ADAPTATIONS AU FORMAT DE LOG SSIE7
			
			uid=atoi(ts1[0])-1;
			set_uid( s2, uid );
						
			hLog = hlog_init( "/appli/ssie7/data/flux", null );
			hlogu( hLog, 0, var_get( h, "status" ), ts1[0], ":", var_get( h, "app_name" ), "/", var_get( h, "flux_name" ), _ );
			hlogu_date( hLog, 0,
				dtoi( var_get( h, "flux_dt_start" ), "aaaa/mm/jj hh:mm:ss" )
//+ (12 Jours)
				);


			#ifdef OS_AIX
			putenv( "USER=mersi" );
			putenv( "LOGIN=mersi" );
			#else
			putenv( "LOGNAME=mersi" );
			#endif
			
			strdcpy( &cmd, mersi3_prefix, "flux ", ts1[0], " log of=js", _ );

			System( &s3, cmd );
			
			strdcpy( &s3, substr( s3, 0, strpos( s3, "][]", 0) + 1 ), _ );
			if ( s3[0] == 0 ) continue;
			
			if ( ts2 ) free( ts2 );
			ts2 = str_explode( s3, "],[" );
			for ( j=0 ; ts2[j] ; j++ )
				{  	
				if ( ts3 ) free( ts3 );	
				ts3=get_flux_line(ts2[j]);
				l=0;
				if ( ts3[8] && strlen(ts3[8]) != 0 ) 
					l=atoi(ts3[8]);
				strdcpy( &s1, "", _ );
				for ( e=2; e <= 6; e++ )
					{
					if ( strlen(ts3[e]) > 0 )
						{
						if ( strlen(s1) == 0 )
							strdcpy( &s1, ts3[e], _ );
						else
							strdcpy( &s1, s1, " ", ts3[e], _ );
						}
					}
		
				id = hlogc( hLog, l, "", s1, _ );
				hlogu_date( hLog, id, dtoi( ts3[1], "aaaa/mm/jj hh:mm:ss" )
//+ (12 Jours)				
				 );
				}
		
			hlog_free( hLog );
			}

		fclose(ptr_file);
	
	
		#ifdef OS_AIX
		putenv( "USER=ssie" );
		putenv( "LOGIN=ssie" );
		#else
		putenv( "LOGNAME=ssie" );
		#endif
		
		// MISE A JOUR DE {dirdat}/flux/log.id
		
		set_uid( s2, mersi3_uid );
		print( BR "    FIN RECUP HISTORIQUE DES FLUX" BR, _ );
		}
	

	strdcpy( &cmd, "rm -Rf /appli/ssie7/tmp", _ );  
	system( cmd );
	
	return OK;
	}
















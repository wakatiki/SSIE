
//------------------------------------------------------------------------------
int migration_mft( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	char *part_name=null, *part_os=null, *part_port=null, *part_ip=null, *part_dns=null;
	char *ip_address=null;
	char *part_host=null, **host_list=null;
	char *part_data_save=null;
	int p_part=0, p_deb=0, p_fin=0;
	
	char *wrk_dir=null;
	char *mft_tmp_dir=null;
	
	
	print( BR BR "--------------------------------------------------------------------------------", BR, _ );
	print( "MIGRATION MFT ==> SSIE v7", BR, BR, _ );

			
	print( BR "PREPARATION" BR, _ );

	if ( ! creation_arbo_ssie( t_work ) )	
		{
		print( "ERREUR : Problème lors de la création de l'arborescence de SSIE v7." BR BR, _ );
//		logw( "MIGRATION MFT ==> SSIE v7 : ", "Problème lors de la création de l'arborescence de SSIE v7." BR, _ );
		return -1;
		}
	
	strdcpy( &cmd, "chmod -R 755 /appli/mft", _ );
	system( cmd );

	strdcpy( &cmd, "chmod -R 755 /home/mft", _ );
	system( cmd );

	
	strdcpy( &cmd, "su - mft -c \"MFTUTIL mftext type=parm > /tmp/mft.conf ; chmod 777 /tmp/mft.conf\"", _ );	
	System( &s, cmd );
	strdcpy( &cmd, "su - mft -c \"MFTUTIL mftext type=net >> /tmp/mft.conf ; chmod 777 /tmp/mft.conf\"", _ );	
	System( &s, cmd );

/*
	strdcpy( &fname, "/home/mft/2.4.1/config/PART_LOC.CFG", _ );
	
	if ( file_exists( fname ) )
		{
		strdcpy( &cmd, "cp ", fname, " /tmp ; chmod 777 /tmp/PART_LOC.CFG'\"", _ );
		System( &s, cmd );
		}
*/
	
	strdcpy( &fname, "/tmp/mft.conf", _ );
	file_load( &s, fname );
	if ( ( p = strpos( s, "IDPARM0", 0 ) ) < 0 ) 
		{
		print( "ERREUR : IDPARM0 non trouvé" BR BR, _ );
		return -1;
		}
	
	if ( ( p = strpos( s, "NPART", p ) ) < 0 ) 
		{
		print( "ERREUR : NPART non trouvé" BR BR, _ );
		return -1;
		}
	
	p_deb = strpos( s, "'", p );
	p_fin = strpos( s, "'", p_deb + 1 );
	
	part_name = substr( s, p_deb + 1, p_fin - ( p_deb + 1 ) );
	
	if ( ( p = strpos( s, "MFTNET", p_fin ) ) < 0 ) 
		{
		print( "ERREUR : MFTNET non trouvé" BR BR, _ );
		return -1;
		}
	if ( ( p = strpos( s, "TCPINOUT", p ) ) < 0 ) 
		{
		print( "ERREUR : TCPINOUT non trouvé" BR BR, _ );
		return -1;
		}
	if ( ( p = strpos( s, "HOST", p ) ) < 0 ) 
		{
		print( "ERREUR : HOST non trouvé" BR BR, _ );
		return -1;
		}
	p_deb = strpos( s, "'", p );
	p_fin = strpos( s, "'", p_deb + 1 );
	
	ip_address = substr( s, p_deb + 1, p_fin - ( p_deb + 1 ) );
	
	mkdir( dirlog );
	if ( !dir_exists( dirlog ) )
		{
		print( " -> ", "ERREUR : échec de la création du répertoire '", dirlog, "'" BR, _ );
//		logw( "MIGRATION MFT ==> SSIE v7 : ", "ERREUR : échec de la création du répertoire '", dirlog, "'" BR, _ );
		r = -1; return ERROR;
		}
	
	print( BR "RECUPERATION DES REVEILS" BR, _ );
	
//	print( " -> ", "Récupération des réveils MFT :", _ );
//	logw( "MIGRATION MFT ==> SSIE v7 : ", "Récupération des réveils MFT." BR, _ );
	
	System( &s, "cp -Rf /appli/mft/DATA/REVEIL/* /appli/ssie7/conf/reveils" );
//	print( " OK.", BR, _ );
	
	System( &s, str( ssie7_prefix, " server", _ ) );
	print( " -> ", "Initialisation du serveur SSIE v7 : OK.", BR, "Résultat :", BR, s, BR, _ );
//	logw( "MIGRATION MFT ==> SSIE v7 : ", "Initialisation du serveur SSIE v7" BR, _ );
	
//	if ( ! stricmp( PackageCode, "mft241" ) )
		{
		strdcpy( &fname, "/home/mft/2.4.1/config/PART_LOC.CFG", _ );
		if ( stat( fname, &buffer ) == 0 )
			{
			file_load( &s, fname );
			s = unblank( s );
			print( " -> ", "Récupération des partenaires locaux :" BR, _ );
//			logw( "MIGRATION MFT ==> SSIE v7 : ", "Récupération des partenaires locaux :" BR, _ );
			p_part=0;
			while( ( p_part = strpos( s, "MFTPART", p_part ) ) >= 0 )
				{
				if ( ( p = strpos( s, "ID=", p_part ) ) >= 0 ) 
					{
					p_deb = p + 3;
					p_fin = strpos( s, ",", p_deb );
					part_name = unquote( substr( s, p_deb, p_fin - p_deb ) );
					if ( hPart ) var_free( hPart );
					strdcpy( &fname1, "/appli/ssie7/conf/part/", part_name, ".dat", _ );
					hPart = var_load( fname1 );
					v1 = var_get( hPart, "partname" );
					if ( ! v1 )
						var_set( hPart, "partname", part_name );
					//                    	print( "MIGRATION MFT ==> SSIE v7 : ", "Partenaire '", part_name, "'." BR, _ );
					print( "      - ", "Partenaire '", part_name, "'", _ );
//					logw( "MIGRATION MFT ==> SSIE v7 : ", "Partenaire '", part_name, "'." BR, _ );

					if ( ( p = strpos( s, "SYST=", p ) ) >= 0 ) 
						{
						p_deb = p + 5;
						p_fin = strpos( s, "\n", p_deb );
						if ( ! strcmp( substr( s, p_fin - 1, 1 ), "," ) ) p_fin--;
						var_set( hPart, "os", unquote ( trim ( substr( s, p_deb, p_fin - p_deb ) ) ) );
						if ( ( p = strpos( s, "SAP=", p ) ) >= 0 ) 
							{
							p_deb = p + 4;
							p_fin = strpos( s, "\n", p_deb );
							if ( ! strcmp( substr( s, p_fin - 1, 1 ), "," ) ) p_fin--;
//								part_port = unquote ( trim ( substr( s, p_deb, p_fin - p_deb ) ) );
							v = trim ( substr( s, p_deb, p_fin - p_deb ) );
							if ( v[0] == '(' )
								{
								p = strpos( v, ")", 1 );
								v = substr( v, 1, p-1 );
								}
							part_port = unquote ( trim ( v ) );
							var_set( hPart, "port_mft", part_port );
							}
						}
					var_save( hPart, fname1 );
					print( " OK.", BR, _ );
					}
				
				p_part++;
				}
			
			p_part=0;
			while ( ( p_part = strpos( s, "MFTTCP", p_part ) ) >= 0 ) 
				{
				if ( ( p = strpos( s, "ID=", p_part ) ) >= 0 ) 
					{
					p_deb = p + 3;
					p_fin = strpos( s, ",", p_deb );
					part_name = unquote( substr( s, p_deb, p_fin - p_deb ) );
					if ( hPart ) var_free( hPart );
					strdcpy( &fname1, "/appli/ssie7/conf/part/", part_name, ".dat", _ );
					hPart = var_load( fname1 );
					v1 = var_get( hPart, "partname" );
					if ( ! v1 )
						var_set( hPart, "partname", part_name );
					if ( ( p = strpos( s, "HOST=", p ) ) >= 0 ) 
						{
						p_deb = p + 5;
						if ( s[p_deb] != '(' )
							{
							p_fin = strpos( s, "\n", p_deb );
							if ( ! strcmp( substr( s, p_fin - 1, 1 ), "," ) ) p_fin--;
							part_host = char_purge( substr( s, p_deb, p_fin - p_deb ), '\'' );
							if ( ! is_numeric( char_purge( part_host, '.' ) ) )
								var_set( hPart, "dns", part_host );
							else
								var_set( hPart, "ip", part_host );
							}
						else
							{
							p_deb = p_deb + 1;
							p_fin = strpos( s, ")", p_deb );
							part_host = char_purge( char_purge( char_purge( substr( s, p_deb, p_fin - p_deb ), '\'' ), ')' ), '(' );
							host_list = str_explode( char_purge( substr( s, p_deb, p_fin - p_deb ), '\'' ), "," );
							for( i = 0 ; host_list[i] ; i++ )
								{
								if ( ! is_numeric( char_purge( host_list[i], '.' ) ) )
									{
									if ( ! part_dns || ( strlen( part_dns ) == 0 ) )
										var_set( hPart, "dns", host_list[i] );
									}      
								else
									{
									if ( ! part_ip || ( strlen( part_ip ) == 0 ) )
										var_set( hPart, "ip", host_list[i] );
									}      
								}
							}
						var_save( hPart, fname1 );
						}
					p_part++;
					}
				}
			}
		}

	// DESACTIVATION DE MERSI3 pour obliger la commande back si on veut un retour arrière

	print( BR "DESACTIVATION DE MFT" BR, _ );
	
	System( &s, "sudo " LOGICIELS "/ssie7/ssie7 mv -f " LOGICIELS "/mft/2.4.1/filexe/MFTMAIN " SAUVE_DIR );
	System( &s, "sudo " LOGICIELS "/ssie7/ssie7 chown ssie:ssie " SAUVE_DIR "/MFTMAIN" );

	
/*
	if ( s ) s[0] = 0;
	strdcpy( &fname, dirbin, DIR_CHAR "ssie7up.hist", _ );
	file_load( &s, fname );
	strdcpy( &s1, PackageCode, BR, s, _ );
	file_save_str( fname, s1, -1 );
*/
	
	strdcpy( &cmd, "rm /tmp/mft.conf", _ );
	System( &s, cmd );
	
	print( BR, "MIGRATION MFT ==> SSIE v7 : ", "TERMINEE.", BR, _ );
	print( "--------------------------------------------------------------------------------", BR, BR, _ );
//	logw( "MIGRATION MFT ==> SSIE v7 : ", "TERMINEE." BR, _ );
	
	return OK;
	}














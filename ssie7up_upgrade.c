

//------------------------------------------------------------------------------
int ssie7_upgrade( T_WORK *t_work )
// SERVICE DE MISE A JOUR DE VERSION
//------------------------------------------------------------------------------
{
    int p_deb=0, p_fin=0;
    char *tmp_dir=null;
    char *s_display=null;
    char *s_filter=null;

    print( BR, SSIE7_VERSION_UPGRADE, ".", BR BR, _ );
    
// Selon l'option "upgrade" ou "uprc", on définit le masque de recherche des mises à jour sur le serveur FTP.
    if ( strpos( argv[0], "ssie7uprc", 0 ) >= 0 )
        strdcpy( &s_filter, "SSIE_", _ );
    else
        strdcpy( &s_filter, "SSIE_v", _ );

    #ifdef OS_AIX
    strdcpy( &ssie7_prefix, "/ssie7/ssie7 ", _ );
//    strdcpy( &ssie7up_prefix, "/ssie7/ssie7up ", _ );
    #elif OS_LINUX
    strdcpy( &ssie7_prefix, "/logiciels/ssie7/ssie7 ", _ );
//    strdcpy( &ssie7up_prefix, "/logiciels/ssie7/ssie7up ", _ );
    #endif
    strdcpy( &sudo_ssie7_prefix, "sudo ", ssie7_prefix, _ );

    strdcpy( &curr_version_fname, dirbin, DIR_CHAR "current_version", _ );
    strdcpy( &curr_binaries_dname, dirbin, DIR_CHAR "current_binaries", _ );
    strdcpy( &prev_version_fname, dirbin, DIR_CHAR "previous_version", _ );
    strdcpy( &prev_binaries_dname, dirbin, DIR_CHAR "previous_binaries", _ );
    strdcpy( &prev_install_version_fname, dirbin, DIR_CHAR "previous_install_version", _ );
    strdcpy( &prev_install_config_dname, dirbin, DIR_CHAR "previous_install_config", _ );

// Compare la version actuelle et la version téléchargée.
    if ( ! file_exists( curr_version_fname ) )
    {
// print( "curr_version_fname n'existe pas.", BR BR, _ ); 
        strdcpy( &tmp_dir, "/tmp/ssie7.", itoa(getpid()), _ );
        crea_rep( tmp_dir );
        strdcpy( &cmd, "cp -R ", dirbin, "/* ", tmp_dir, _ );
    	System( &s, cmd );
        crea_rep( curr_binaries_dname );
        strdcpy( &cmd, "mv ", tmp_dir, "/* ", curr_binaries_dname, _ );
    	System( &s, cmd );
    	strdcpy( &cmd, dirbin, DIR_CHAR "ssie7" EXE " version", _ );
    	System( &curr_ver, cmd );
        curr_ver = str_replace( str_replace( curr_ver, " ", "_" ), "_du_", "_" );
        file_save_str( curr_version_fname, curr_ver, -1 );
    }
    else
        file_load( &curr_ver, curr_version_fname );

    if ( file_exists( prev_version_fname ) )
        file_load( &prev_ver, prev_version_fname );
    else
        strdcpy( &prev_ver, "", _ );

    if ( file_exists( prev_install_version_fname ) )
        file_load( &prev_inst, prev_install_version_fname );
    else
        strdcpy( &prev_inst, "", _ );
        
    trim( curr_ver );  
    trim( prev_ver ); 
    trim( prev_inst );

//print( "ssie7_upgrade : curr_ver='", curr_ver, "', prev_ver='", prev_ver, "', prev_inst=", prev_inst, ".", BR BR, _ );

/***********************************************************************************/
/* Section de réécriture.                                                          */
//	#ifdef OS_UNIX

	bMuet = (( var_getn( hReq, 2, &vn ) )&&( !stricmp( vn, "-noquestion" ) ));
	
	if ( ! var_getn( hReq, 1, &PackageCode ) )
	{
         strdcpy( &s_display, "", _ );
//        if ( ( ! file_exists( curr_version_fname ) ) && ( ! file_exists( prev_version_fname ) ) )
        if ( strlen( prev_inst ) == 0 )
        {
            b = false;
// CHERCHE SI MERSI v3 EST INSTALLE
//            if ( stat( "/appli/mersi3", &buffer ) == 0 )
            if ( check_file( "/appli/mersi3", t_work ) )
            {
                 b = 1;
                 strdcpy( &s_display, s_display, "   - mersi3 : migration mersi v3 vers ", curr_ver, BR, _ );
            }
// CHERCHE SI MFT EST INSTALLE
            if ( check_file( "/home/mft/2.3.0", t_work ) )
            {
                 b = 2;
                 strdcpy( &s_display, s_display, "   - mft230 : migration MFT v2.3.0 vers ", curr_ver, BR, _ );
            }
            if ( check_file( "/home/mft/2.4.1", t_work ) )
            {
                 b = 2;
                 strdcpy( &s_display, s_display, "   - mft241 : migration MFT v2.4.1 vers ", curr_ver, BR, _ );
            }
            if ( b )
                print( BR, "Liste des migrations possibles :", BR BR, s_display, BR, _ );
        }
        if ( strlen( s_display ) > 0 )
             strdcpy( &s_display, "", _ );
        if ( strlen ( prev_ver ) > 0 )
             strdcpy( &s_display, "   - ", prev_ver, " (version précédemment installée)", BR, _ );
        s = ssie_patch_get_list( t_work, s_filter );
        p_deb=0;
        while( ( p_deb = strpos( s, s_filter, p_deb ) ) >= 0 )
        {
#ifdef OS_LINUX
            if ( ( p_fin = strpos( s, BR, p_deb ) ) >= 0 ) 
#else
            if ( ( p_fin = strpos( s, "/", p_deb ) ) >= 0 ) 
#endif
            {
                fname = unquote( substr( s, p_deb, p_fin - p_deb ) );

                if ( ( ( strlen( curr_ver ) == 0 ) || ( ( strlen( curr_ver ) > 0 ) && stricmp( fname, curr_ver ) ) )
                       && ( ( strlen( prev_ver ) == 0 ) || ( ( strlen( prev_ver ) > 0 ) && stricmp( fname, prev_ver ) ) ) )
                     strdcpy( &s_display, s_display, "   - ", fname, BR, _ );
            }
            
            p_deb=p_fin;
        }
        print( BR, "Liste des mises à jour possibles (version actuelle : ", curr_ver, ") :", BR, _ );
        if ( strlen( s_display ) > 0 )
        {
            print( s_display, BR, _ );
            print( "Pour démarrer une migration ou une mise à jour, exécutez la commande :" BR
            "      ssie7 upgrade {code action} [-noquestion]" BR
            "      (code action = code en début de chacune des lignes.)" BR BR, _ );
            
            r = 0;
            exit( r );
        }
        else
        	print( BR, "   Aucune mise à jour disponible." BR BR, _ );
            
    }
#ifdef OS_UNIX
	else
    if ( ! stricmp( PackageCode, "mersi3" ) )
    {
             migration_mersi3( t_work );
             exit(0);
    }
    else
    if ( ( ! stricmp( PackageCode, "mft241" ) ) || ( ! stricmp( PackageCode, "mft230" ) ) )
    {
             migration_mft( t_work );
             exit(0);
    }
#endif
	else
	if ( ( strpos( PackageCode, s_filter, 0 ) >= 0 ) || ( ! stricmp( PackageCode, "last" ) ) )
	{
        if ( ! stricmp( PackageCode, "last" ) )
        {
                s = ssie_patch_get_list( t_work, s_filter );
                p_deb=0;
                while( ( p_deb = strpos( s, s_filter, p_deb ) ) >= 0 )
                {
                    if ( ( p_fin = strpos( s, BR, p_deb ) ) >= 0 ) 
                        fname = unquote( substr( s, p_deb, p_fin - p_deb ) );
                    
                    p_deb=p_fin;
                }
                if ( curr_ver && ( ! stricmp( fname, curr_ver ) ) )
                {
                    print( "   La dernière version de SSIE v7 (", fname, ") est déjà installée. ", BR, _ );
                    r = 1;
                    exit( r );
                }
                else
                    PackageCode = fname;
        }
//        if ( stricmp( prev_ver, PackageCode ) )
        		ssie_patch_install( t_work, prev_ver );
/*
        else
        {
                strdcpy( &tmp_dir, "/tmp/curr_binaries.", itoa(getpid()), _ );
                crea_rep( tmp_dir );
                strdcpy( &cmd, "mv ", curr_binaries_dname, "/* ", tmp_dir, _ );
    	        System( &s, cmd );
                strdcpy( &cmd, "mv ", prev_binaries_dname, "/* ", curr_binaries_dname, _ );
    	        System( &s, cmd );
                strdcpy( &cmd, "mv ", tmp_dir, "/* ", prev_binaries_dname, _ );
    	        System( &s, cmd );
                strdcpy( &cmd, "cp ", curr_binaries_dname, "/* ", dirbin, _ );
    	        System( &s, cmd );
                strdcpy( &cmd, "cp ", curr_version_fname, " /tmp/curr_version.", itoa(getpid()), _ );
    	        System( &s, cmd );
//print( "s='", s, "', cmd='", cmd, "'.", BR, _ );
                strdcpy( &cmd, "cp ", prev_version_fname, " ", curr_version_fname, _ );
    	        System( &s, cmd );
//print( "s='", s, "', cmd='", cmd, "'.", BR, _ );
                strdcpy( &cmd, "cp /tmp/curr_version.", itoa(getpid()), " ", prev_version_fname, _ );
    	        System( &s, cmd );
//print( "s='", s, "', cmd='", cmd, "'.", BR, _ );
        }
        r = 0;
		exit( r );
*/
	}
/*
	else
	if ( ! stricmp( PackageCode, "mftext" ) )
	{
	    strdcpy( &cmd, "su - mft -c \"MFTUTIL mftext > /tmp/mft.conf\"", _ );	
//print( "cmd='", cmd, "'", BR, _ );
    	System( &s, cmd );
//print( "s='", s, "'.", BR, _ );
        strdcpy( &fname, "/home/mft/2.4.1/config/PART_LOC.CFG", _ );
        if ( stat( fname, &buffer ) == 0 )
        {
             strdcpy( &cmd, "su - mft -c \"cp ", fname, " /tmp\"", _ );
             System( &s, cmd );
        }
	    strdcpy( &cmd, "chown ssie:ssie /tmp/mft.conf /tmp/PART_LOC.CFG", _ );	
    	System( &s, cmd );
    }
*/
    else
	if ( ! stricmp( PackageCode, "back" ) )
	{
		upgrade_back( t_work );
		exit( r );
	}
	else
	{
		print( "ERREUR : Code package inconnu '", PackageCode, "'." BR BR, _ );
		r = 1;
		exit( r );
	}
	
//	#else
/*	
	print( BR BR, "   Aucune mise à jour disponible." BR BR, _ );
	r = 1;
	exit( r );
*/	
//	#endif
}


	





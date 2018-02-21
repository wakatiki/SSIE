#include "ssie7.h"

#define SSIE7_VERSION_UPGRADE		"SSIE UPGRADE v7.1.d_2015.05.12"


#undef print
#define bMuet		b3
#define PackageCode	v4




struct stat buffer;  

char *curr_ver=null, *prev_ver=null;
char *curr_version_fname=null, *prev_version_fname=null;
char *curr_binaries_dname=null, *prev_binaries_dname=null;

//char *str_list[10];
typedef struct
	{
	int parent_id;
	int first_child_id;
	int last_child_id;
	int next_id;
	char when[21];
	char status[201];
	char log[501];
	}
	t_elib_log;


typedef struct
	{
	t_elib_log elib_log;
	char *hlog_id;
	char *hlog_fname;
	}
	t_elib_log_wrk;

#define elib_log 		(((t_elib_log_wrk*)hlog)->elib_log)
#define hlog_fname		(((t_elib_log_wrk*)hlog)->hlog_fname)
#define hlog_id	 		(((t_elib_log_wrk*)hlog)->hlog_id)



char * unblank( char * _str )
//------------------------------------------------------------------------------
{
char *wrk_str=null;

	int _i, _j;

	if ( !_str ) return null;
	
	if ( _str[0] )
	{
        strdcpy( &wrk_str, _str, _ );
        _j = 0;
        for( _i = 0 ; _i < strlen(_str) ; _i++ )
        {
        //             if ( ( wrk_str[_i] != ' ' ) && ( wrk_str[_i] != '	' ) && ( wrk_str[_i] != 13 ) && ( wrk_str[_i] != 10 ) )
             if ( ( _str[_i] != ' ' ) && ( _str[_i] != '	' ) )
                  wrk_str[_j++] = _str[_i];
        }
        wrk_str[_j] = 0;			 
	}

	return wrk_str;
}

char * char_purge(char * _str, char _char )
{
char *wrk_str=null;
	int _i, _j;

	if ( !_str ) return null;
	
	if ( _str[0] )
	{
        strdcpy( &wrk_str, _str, _ );
		_j = 0;
		for( _i = 0 ; _i < strlen(_str) ; _i++ )
		{
//             if ( ( wrk_str[_i] != ' ' ) && ( wrk_str[_i] != '	' ) && ( wrk_str[_i] != 13 ) && ( wrk_str[_i] != 10 ) )
             if ( _str[_i] != _char )
                  wrk_str[_j++] = _str[_i];
        }
        wrk_str[_j] = 0;			 
	}

	return wrk_str;
}

char * get_ssie_updates( T_WORK *t_work, char *s_filter )
{
//print( "get_ssie_updates : entrée.", BR, _ );
        #ifdef OS_WINDOWS
        strdcpy( &fname1, "C:", DIR_CHAR, "Temp", _ );
        if ( ! dir_exists( fname1 ) )
           crea_rep( fname1 );
        //	System( &s, "chmod 755 ssie7" );
        #else
        strdcpy( &fname1, DIR_CHAR, "tmp", _ );
        #endif
//print( "fname1='", fname1, "'.", BR, _ );
        strdcpy( &fname2, fname1, DIR_CHAR, "ssie7up.", itoa( getpid() ), _ );
//print( "fname2='", fname2, "'.", BR, _ );
        crea_rep( fname2 );
        strdcpy( &cmd, "chmod 777 ", fname2, _ );
//print( "cmd='", cmd, "'.", BR, _ );
        System( &s, cmd );
        chdir( fname2 );
        
        strdcpy( &fname3, fname2, DIR_CHAR, "ftp.log", _ );
//print( "fname3='", fname3, "'.", BR, _ );
/*
        strdcpy( &cmd, "ftp -n 10.151.29.82 21 << fin_FTP > ", fname3, BR,
        "user ssie7src ssie7src" BR,
*/
		strdcpy( &cmd, "ftp -n 10.151.29.82 721 << fin_FTP > ", fname3, BR,
					"user ssiestore ssiestore", BR,
        /*
        strdcpy( &cmd, "ftp -n 10.151.29.25 << fin_FTP > ", fname3, BR,
                "user ssie7src EDF_mft_2015" BR,
        */
                "verbose" BR,
                "prompt -n" BR,
                "cd bin/" OS_NAME BR,
                "ls ", s_filter, "*", BR,
                "quit" BR,
                "fin_FTP" BR, _ );
//print( "cmd='", cmd, "'.", BR, _ );
        System( &s, cmd	);	
        file_load( &s, fname3 );
        
//        print( "OS_NAME='", OS_NAME, "', ftp : '", s, "'.", BR, _ );
        
//print( "get_ssie_updates : sortie.", BR, _ );
        return s;
}

//------------------------------------------------------------------------------
int ssie7_upgrade( T_WORK *t_work )
// SERVICE DE MISE A JOUR DE VERSION
//------------------------------------------------------------------------------
{
int p_deb=0, p_fin=0;
char *s_display=null;
char *s_filter=null;
/*
        var_getn( hReq, 1, &v1 );
        var_getn( hReq, 2, &v2 );
        var_getn( hReq, 3, &v3 );
        var_getn( hReq, 4, &v4 );
        print( "v1=", v1, ", v2=", v2, ", v3=", v3, BR, _ );
*/
/*
char *ssie7_prefix=null;

#ifdef OS_AIX
strdcpy( &s, "/ssie7/ssie7 ", _ );
#elif OS_LINUX
strdcpy( &s, "/logiciels/ssie7/ssie7 ", _ );
#endif
strdcpy( &ssie7_prefix, "sudo ", s, _ );
*/

	//--------------------------------------------------------------------------
	// VERSION : 12/2014.a
	//--------------------------------------------------------------------------
		// TRANSMET LA COMMANDE

//	strdcpy( &fname, dirbin, DIR_CHAR "ssie7" EXE, _ );
//	if (( !file_exists( fname ) )||( (time(NULL) - file_date(fname)) > (3*60*60) ))
		// TELECHARGE LE PROGRAMME DU SERVEUR D'ANNUAIRE

/*
	strdcpy( &cmd, "chmod 755 ", fname1, DIR_CHAR "*", _ );
   	System( &s, cmd );
*/	
// Selon l'option "upgrade" ou "uprc", on définit le masque de recherche des mises à jour sur le serveur FTP.
    strdcpy( &curr_version_fname, dirwrk, DIR_CHAR "current_version", _ );
    strdcpy( &curr_binaries_dname, dirwrk, DIR_CHAR "current_binaries", _ );
    strdcpy( &prev_version_fname, dirwrk, DIR_CHAR "previous_version", _ );
    strdcpy( &prev_binaries_dname, dirwrk, DIR_CHAR "previous_binaries", _ );

    if ( strpos( argv[0], "ssie7uprc", 0 ) >= 0 )
        strdcpy( &s_filter, "ssie_", _ );
    else
        strdcpy( &s_filter, "ssie_v_", _ );
//print( "argv[0]=", argv[0], ", ", "s_filter=", s_filter, ".", BR, _ );

// Compare la version actuelle et la version téléchargée.
    if ( ! file_exists( curr_version_fname ) )
    {
    	strdcpy( &cmd, dirbin, DIR_CHAR "ssie7" EXE " version", _ );
    	System( &curr_ver, cmd );
        curr_ver = str_replace( lower( str_replace( str_replace( curr_ver, " ", "_" ), "_du_", "_" ) ), "ssie_v", "ssie_v_" );
    }
    else
        file_load( &curr_ver, curr_version_fname );

    if ( file_exists( prev_version_fname ) )
        file_load( &prev_ver, prev_version_fname );
    else
        strdcpy( &prev_ver, "", _ );
        
    trim( curr_ver );  
    trim( prev_ver ); 
//print( "Version actuelle : ", curr_ver, BR, _ );
//print( "Version précédente : ", prev_ver, BR, _ );
/*     
*/
/*
*/
/***********************************************************************************/
/* Section de réécriture.                                                          */
	#ifdef OS_UNIX

	bMuet = (( var_getn( hReq, 2, &vn ) )&&( !stricmp( vn, "-noquestion" ) ));
	
	if ( ! var_getn( hReq, 1, &PackageCode ) )
	{
//print( "PackageCode : ", PackageCode, BR, _ );
        if ( ( ! file_exists( curr_version_fname ) ) && ( ! file_exists( prev_version_fname ) ) )
        {
            b = false;
//print( "Pas de ssie7 de configuré, on cherche les installations de produits antérieurs.", BR, _ );
    
// CHERCHE SI MERSI v3 EST INSTALLE
            if ( stat( "/appli/mersi3", &buffer ) == 0 )
            {
//print( "Mersi v3 trouvé.", BR, _ );
                 b = 1;
//                 print( "   - mersi3: migration mersi v3 vers ", product_name, BR, _ );
                 strdcpy( &s_display, s_display, "   - mersi3: migration mersi v3 vers ", curr_ver, BR, _ );
/*
                 if ( ! file_exists( "/appli/mersi3/conf/mersi3.dat" ) )
                 {
//                     print( "   ATTENTION : avant de tenter la migration,",
                     strdcpy( &s_display, s_display, "   ATTENTION : avant de tenter la migration,",
                               " veuillez donner les droits d'accès aux fichiers de l'application :" BR
                               "      chmod -R 777 /appli/mersi3" BR,
                               _ );
                 }
*/
            }
            
// CHERCHE SI MFT EST INSTALLE
            if ( ( ( stat( "/home/mft/2.3.0", &buffer ) == 0 ) || ( stat( "/home/mft/2.4.1", &buffer ) == 0 ) )
                   && ( stat( "/appli/mft/DATA/TEMP", &buffer ) == 0 ) )
            {
                if ( stat( "/home/mft/2.3.0", &buffer ) == 0 )
                {
//print( "MFT v2.3.0 trouvé.", BR, _ );
                     b = 2;
//                     print( "   - mft230: migration MFT v2.30 vers ", product_name, BR, _ );
                     strdcpy( &s_display, s_display, "   - mft230: migration MFT v2.3.0 vers ", curr_ver, BR, _ );
                }
                if ( stat( "/home/mft/2.4.1", &buffer ) == 0 )
                {
//print( "MFT v2.4.1 trouvé.", BR, _ );
                     b = 2;
//                     print( "   - mft241: migration MFT v2.41 vers ", product_name, BR, _ );
                     strdcpy( &s_display, s_display, "   - mft241: migration MFT v2.4.1 vers ", curr_ver, BR, _ );
                }
/*
                if ( b == 2 )
                {
                    if ( ! file_exists( "/appli/mft/DATA/TEMP/mft.conf" ) )
                    {
//                        print( BR BR "   ATTENTION : avant de tenter la migration,",
                        strdcpy( &s_display, s_display, BR, "   ATTENTION : avant de tenter la migration,",
                                " veuillez exécuter la commande suivante sous le login mft :" BR,
                                "      MFTUTIL mftext > /appli/mft/DATA/TEMP/mft.conf" BR,
                                _ );
                    }
*/
/*
                    if ( stat( "/appli/mft/DATA/TEMP/mft.conf", &buffer ) != 0 )
                    {
                        if ( errno == EACCES )
                        {
                    		strdcpy( &s_display, s_display, BR BR, "   ATTENTION : veuillez donner les droits d'accès au fichier '/appli/mft/DATA/TEMP/mft.conf' :", BR,
                                         "      chmod 777 /appli/mft/DATA/TEMP/mft.conf" BR BR, _ );
                        }
                        else
                        {
                    		strdcpy( &s_display, s_display, BR BR, "   ATTENTION : Pour une migration MFT, avant de tenter la migration," BR
                    				"   Veuillez exécuter la commande suivante sous le login mft :" BR
                    				"      MFTUTIL mftext > /appli/mft/DATA/TEMP/mft.conf" BR BR,
                    				_ );
                        }
                		return;
                	}
                }
*/
            }
            else
            {
                if ( stat( "/home/mft", &buffer ) != 0 )
                {
                    if ( errno == EACCES )
                    {
//                        print( BR,
                        strdcpy( &s_display, s_display, "   Une installation de MFT a été trouvée ; avant de tenter la migration :" BR
                                "   - veuillez donner les droits d'accès au répertoire '/home/mft' :" BR
                                "      chmod -R 777 /home/mft" BR,
                                _ );
                        if ( stat( "/appli/mft", &buffer ) != 0 )
                            if ( errno == EACCES )
//                                print( "   - veuillez donner les droits d'accès au répertoire '/appli/mft' :" BR
                                strdcpy( &s_display, s_display, "   - veuillez donner les droits d'accès au répertoire '/appli/mft' :" BR
                                		"      chmod -R 777 /appli/mft" BR,
                                		_ );
                    }
                }
            }
            if ( b )
            {
                print( "Liste des migrations possibles :" BR BR, s_display, _ );
                print( BR BR "   Pour démarrer une migration, exécutez la commande :" BR
                    "      ssie7 upgrade {code pkg} [-noquestion]" BR
                    "      (code pkg = code en début de chacune des lignes 'mersi3', 'mft230', ...)" BR BR, _ );
                
                r = 0;
                exit( r );
                return 0;
            }
        }
///////////////////////////////////////////////////////////////////////////////////////////:
/*
        #ifdef OS_WINDOWS
        strdcpy( &fname1, "C:", DIR_CHAR, "Temp", _ );
        if ( ! dir_exists( fname1 ) )
           crea_rep( fname1 );
        //	System( &s, "chmod 755 ssie7" );
        #else
        strdcpy( &fname1, DIR_CHAR, "tmp", _ );
        #endif
                print( "fname1='", fname1, "'.", _ );
        strdcpy( &fname2, fname1, DIR_CHAR, "ssie7up.", itoa( getpid() ), _ );
                print( "fname2='", fname2, "'.", _ );
        crea_rep( fname2 );
        strdcpy( &cmd, "chmod 777 ", fname2, _ );
                print( "cmd='", cmd, "'.", _ );
        System( &s, cmd );
        chdir( fname2 );
        
        strdcpy( &fname3, fname2, DIR_CHAR, "ftp.log", _ );
                print( "fname3='", fname3, "'.", _ );
        strdcpy( &cmd, "ftp -n 10.151.29.25 << fin_FTP > ", fname3, BR,
                "user ssie7src EDF_mft_2015" BR,
                "verbose" BR,
                "prompt -n" BR,
                "cd bin/" OS_NAME BR,
                "ls ", s_filter, "*", BR,
                "quit" BR,
                "fin_FTP" BR, _ );
                print( "cmd='", cmd, "'.", BR, _ );
        System( &s, cmd	);	
        file_load( &s, fname3 );
*/
///////////////////////////////////////////////////////////////////////////////////////
        s = get_ssie_updates( t_work, s_filter );
//print("Après 'get_ssie_updates' (s='", s, "').", BR, _ );       
        p_deb=0;
        while( ( p_deb = strpos( s, s_filter, p_deb ) ) >= 0 )
        {
            if ( ( p_fin = strpos( s, BR, p_deb ) ) >= 0 ) 
            {
                fname = unquote( substr( s, p_deb, p_fin - p_deb ) );
                if ( ( !curr_ver || ( curr_ver && stricmp( fname, curr_ver ) ) )
                       && ( !prev_ver || ( prev_ver && stricmp( fname, prev_ver ) ) ) )
//                   print( "   - ", fname, BR, _ );
                     strdcpy( &s_display, s_display, "   - ", fname, BR, _ );
            }
            
            p_deb=p_fin;
        }
//print("s_display='", s_display, "'.", BR, _ );       
        if ( s_display )
        {
            print( "Liste des mises à jour possibles :" BR BR, s_display, _ );
            print( BR BR "   Pour démarrer une mises à jour, exécutez la commande :" BR
            "      ssie7 upgrade {code màj} [-noquestion]" BR
            "      (code màj = code en début de chacune des lignes.)" BR BR, _ );
            
            r = 0;
            exit( r );
        }
/*
        else
        {
    		
    		if ( b )
    			{
    			print( BR BR "   Pour démarrer une migration, exécutez la commande :" BR BR
    					"   ssie7 upgrade {code pkg} [-noquestion]" BR BR
    					"   (code pkg = code en début de chacune des lignes 'mersi3', 'mft230', ...)" BR BR, _ );
    				
    			r = 0;
    			exit( r );
    			}
    
    		print( "   Aucune mise à jour disponible." BR BR BR, _ );
    		r = 1;
    		exit( r );
        }
*/
    }
	else
/*
	if ( ( ! stricmp( PackageCode, "mersi3" ) )
           || ( ! stricmp( PackageCode, "mft241" ) ) || ( ! stricmp( PackageCode, "mft230" ) ) )

	{
//print( "Package demandé : ", PackageCode, ".", BR, _ );
        var_getn( hReq, 1, &v1 );
        var_getn( hReq, 2, &v2 );
        var_getn( hReq, 3, &v3 );
        var_getn( hReq, 4, &v4 );
        print( "v1=", v1, ", v2=", v2, ", v3=", v3, BR, _ );
        
        if ( ( !v2 || stricmp( v2, "rooted" ) ) && ( !v3 || stricmp( v3, "rooted" ) ) )
        {
            strdcpy( &cmd, ssie7_prefix, " upgrade ", v1, " ", v2, " ", v3, " rooted", _ );
            print( "cmd=", cmd, BR, _ );
    		Start( cmd );
    //		print( s, BR, _ );
    		return OK;
        }
*/
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
/*
		{
    		file_save_str( curr_version_fname, curr_version_fname, -1 );

            if ( file_exists(curr_binaries_dname) )
                file_delete( curr_binaries_dname );
            crea_rep( curr_binaries_dname );
            strdcpy( &cmd, "cp -f ", dirbin, "/* ", curr_binaries_dname, _ );
            System( &s, cmd );

    		file_save_str( prev_version_fname, "mersi3", -1 );
        }
*/
/*
		exit( r );
	}
*/
/*
	else
	if (( ! stricmp( PackageCode, "mft241" ) )||( ! stricmp( PackageCode, "mft230" ) ))
	{
//print( "Package demandé : ", PackageCode, ".", BR, _ );
        var_getn( hReq, 1, &v1 );
        var_getn( hReq, 2, &v2 );
        var_getn( hReq, 3, &v3 );
        var_getn( hReq, 4, &v4 );
        print( "v1=", v1, ", v2=", v2, ", v3=", v3, BR, _ );
        
        if ( ( !v2 || stricmp( v2, "rooted" ) ) && ( !v3 || stricmp( v3, "rooted" ) ) )
        {
            strdcpy( &cmd, ssie7_prefix, " upgrade ", v1, " ", v2, " ", v3, " rooted", _ );
            print( "cmd=", cmd, BR, _ );
    		Start( cmd );
    //		print( s, BR, _ );
    		return OK;
        }

		if ( migration_mft( t_work ) )
		{
/*
    		file_save_str( curr_version_fname, curr_version_fname, -1 );

            if ( file_exists(curr_binaries_dname) )
                file_delete( curr_binaries_dname );
            crea_rep( curr_binaries_dname );
            strdcpy( &cmd, "cp -f ", dirbin, "/* ", curr_binaries_dname, _ );
            System( &s, cmd );

    		file_save_str( prev_version_fname, PackageCode, -1 );
        }
		exit( r );
		}
*/
	else
	if ( ( strpos( PackageCode, s_filter, 0 ) >= 0 ) || ( ! stricmp( PackageCode, "last" ) ) )
	{
        if ( ! stricmp( PackageCode, "last" ) )
        {
                s = get_ssie_updates( t_work, s_filter );
                p_deb=0;
                while( ( p_deb = strpos( s, s_filter, p_deb ) ) >= 0 )
                {
                    if ( ( p_fin = strpos( s, BR, p_deb ) ) >= 0 ) 
//                    {
                        fname = unquote( substr( s, p_deb, p_fin - p_deb ) );
//                    }
                    
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
//print( "Package demandé : ", PackageCode, ".", BR, _ );
		if ( migration_ssie( t_work ) )
		{
/*
    		file_save_str( curr_version_fname, PackageCode, -1 );
            if ( file_exists(curr_binaries_dname) )
                file_delete( curr_binaries_dname );
            crea_rep( curr_binaries_dname );
            strdcpy( &cmd, "cp -f ", dirbin, "/* ", curr_binaries_dname, _ );
            System( &s, cmd );
    		file_save_str( prev_version_fname, curr_version_fname, -1 );
*/
        }
        r = 0;
		exit( r );
	}
/*
	else
	if ( ! stricmp( PackageCode, "last" ) )
		{
print( "Package demandé : ", "dernière version disponible de SSIE v7", ".", BR, _ );
		strdcpy( &fname1, dirwrk, DIR_CHAR "current_version", _ );
		strdcpy( &fname2, dirwrk, DIR_CHAR "previous_version", _ );
		file_copy(fname2, fname1);
		file_delete(fname2);
		strdcpy( &fname2, dirwrk, DIR_CHAR "previous_data", _ );
		file_delete(fname2);
		exit( r );
		}
*/
	else
	if ( ! stricmp( PackageCode, "mftext" ) )
	{
	    strdcpy( &cmd, "su - mft -c \"MFTUTIL mftext > /tmp/mft.conf\"", _ );	
//    strdcpy( &cmd, sudo_ssie7_prefix, "su - mft -c MFTUTIL mftext > /appli/mft/DATA/TEMP/mft.conf", _ );	
//        print( "cmd='", cmd, "'", BR, _ );
    	System( &s, cmd );
//        print( "s='", s, "'.", BR, _ );
        strdcpy( &fname, "/home/mft/2.4.1/config/PART_LOC.CFG", _ );
        if ( stat( fname, &buffer ) == 0 )
        {
             strdcpy( &cmd, "su - mft -c \"cp ", fname, " /tmp\"", _ );
//        print( "cmd='", cmd, "'", BR, _ );
             System( &s, cmd );
//        print( "s='", s, "'.", BR, _ );
        }
	    strdcpy( &cmd, "chown ssie:ssie /tmp/mft.conf /tmp/PART_LOC.CFG", _ );	
//    strdcpy( &cmd, sudo_ssie7_prefix, "su - mft -c MFTUTIL mftext > /appli/mft/DATA/TEMP/mft.conf", _ );	
//        print( "cmd='", cmd, "'", BR, _ );
    	System( &s, cmd );
//        print( "s='", s, "'.", BR, _ );
    }
    else
	if ( ! stricmp( PackageCode, "back" ) )
	{
//print( "Package demandé : ", PackageCode, ".", BR, _ );
		upgrade_back( t_work );
/*
		strdcpy( &fname1, dirwrk, DIR_CHAR "current_version", _ );
		strdcpy( &fname2, dirwrk, DIR_CHAR "previous_version", _ );
		file_copy(fname2, fname1);
		file_delete(fname2);
		strdcpy( &fname2, dirwrk, DIR_CHAR "previous_data", _ );
		file_delete(fname2);
*/
		exit( r );
	}
	else
	{
//print( "Package demandé : ", PackageCode, ".", BR, _ );
		print( "ERREUR : Code package inconnu '", PackageCode, "'." BR BR, _ );
		r = 1;
		exit( r );
	}
	
	#else
	
	print( "   Aucune mise à jour disponible." BR BR BR, _ );
	r = 1;
	exit( r );
	
	#endif
	}


int crea_rep( char *nom_rep )
{
    if ( stat( nom_rep, &buffer ) != 0 )
        if ( errno == ENOENT )
        {
        	mkdir( nom_rep );
        	if ( !dir_exists( nom_rep ) )
        	{
        		print( "      ERREUR : Création répertoire '", nom_rep, "'" BR, _ );
        		return false;
        	}
        }	
    return true;
}

int creation_arbo_ssie( T_WORK *t_work )
{
    if ( ( ! crea_rep( dirlog ) )
           || ( ! crea_rep( "/appli/ssie7/conf" ) )
           || ( ! crea_rep( "/appli/ssie7/conf/part" ) )
           || ( ! crea_rep( "/appli/ssie7/conf/login" ) )
           || ( ! crea_rep( "/appli/ssie7/conf/reveils" ) )
           || ( ! crea_rep( "/appli/ssie7/conf/flux" ) )
           || ( ! crea_rep( "/appli/ssie7/data" ) )
           || ( ! crea_rep( "/appli/ssie7/data/wrk" ) )
           || ( ! crea_rep( "/appli/ssie7/data/export" ) )
           || ( ! crea_rep( "/appli/ssie7/data/import" ) )
           || ( ! crea_rep( "/appli/ssie7/data/report" ) )
           )
       return false;

    return true;
}


//------------------------------------------------------------------------------
int migration_mersi3( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
char *wrk_dir=null;
char *mersi3_tmp_dir=null;
char *ssie7_prefix=null;
char *sudo_ssie7_prefix=null;

#ifdef OS_AIX
strdcpy( &ssie7_prefix, "/ssie7/ssie7 ", _ );
#elif OS_LINUX
strdcpy( &ssie7_prefix, "/logiciels/ssie7/ssie7 ", _ );
#endif
strdcpy( &sudo_ssie7_prefix, "sudo ", ssie7_prefix, _ );

/*
	// VERIFIE QUE LES FICHIERS MERSI3 SONT ACCESSIBLE
	strdcpy( &cmd, ssie7_prefix, "chmod -R 777 /appli/mersi3", _ );
    System( &s, cmd );
*/  
/*  
	if ( !file_exists( "/appli/mersi3/conf/mersi3.dat" ) )
		{
		print( "Veuillez donner les droits d'accès aux fichiers mersi v3 "
				"en passant la commande suivante sous le login root ou mersi : " BR BR
				"chmod -R 777 /appli/mersi3" BR BR
				"Puis redémarrer ce programme." BR BR,
				_ );
		r = 1; return ERROR;
		}
*/

/*
	if ( !bMuet )
		{
		print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Vérifiez que mersi v3 est arrêté et appuyez sur 'C' pour continuer." BR, _ );
		c = kbget();
		if (( c != 67 )&&( c != 99 ))
			{
			print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "MIGRATION ABANDONNEE." BR BR, _ );
			logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "MIGRATION ABANDONNEE." BR BR, _ );
			r = 1; return OK;
            }
		}
*/
/*
	System( &s, "ps -ef | grep mersi3 | grep -v grep" );
	if ( strlen(s) != 0 )
	{
		print( BR BR, "MIGRATION MERSI v3 ==> SSIE v7 : ", "ATTENTION : arrêter Mersi v3 avant de lancer la migration (", s, ")." BR BR,	_ );
		return;
    }
*/    
	logw( "MIGRATION MERSI v3 : DEMARRAGE" BR
		"----------------------------------------------------------------------------" BR, _ );
    if ( ! creation_arbo_ssie( t_work ) )	
    {
		print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Problème lors de la création de l'arborescence de SSIE v7." BR BR, _ );
		logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Problème lors de la création de l'arborescence de SSIE v7." BR BR, _ );
		return -1;
    }
    
//    strdcpy( &mersi3_tmp_dir, "/appli/mersi3/", _ );
    strdcpy( &mersi3_tmp_dir, "/tmp/mersi3/", itoa( getpid() ), _ );
    crea_rep( mersi3_tmp_dir );
    strdcpy( &cmd, sudo_ssie7_prefix, "cp -R /appli/mersi3/conf ", mersi3_tmp_dir, _ );
//    print( "cmd='", cmd, "'", BR, _ );
    System( &s, cmd );
//    print( "s='", s, "'", BR, _ );
    strdcpy( &cmd, sudo_ssie7_prefix, "cp -R /appli/mersi3/data/reveils ", mersi3_tmp_dir, _ );
//    print( "cmd='", cmd, "'", BR, _ );
    System( &s, cmd );
//    print( "s='", s, "'", BR, _ );
    strdcpy( &cmd, sudo_ssie7_prefix, "cp -R /appli/mersi3/data/flux ", mersi3_tmp_dir, _ );
//    print( "cmd='", cmd, "'", BR, _ );
    System( &s, cmd );
//    print( "s='", s, "'", BR, _ );
    strdcpy( &cmd, sudo_ssie7_prefix, "cp -R /appli/mersi3/data/export ", mersi3_tmp_dir, _ );
//    print( "cmd='", cmd, "'", BR, _ );
    System( &s, cmd );
//    print( "s='", s, "'", BR, _ );
    strdcpy( &cmd, sudo_ssie7_prefix, "cp -R /appli/mersi3/data/import ", mersi3_tmp_dir, _ );
//    print( "cmd='", cmd, "'", BR, _ );
    System( &s, cmd );
//    print( "s='", s, "'", BR, _ );
    strdcpy( &cmd, sudo_ssie7_prefix, "cp -R /appli/mersi3/data/import ", mersi3_tmp_dir, _ );
//    print( "cmd='", cmd, "'", BR, _ );
    System( &s, cmd );
//    print( "s='", s, "'", BR, _ );
    strdcpy( &cmd, sudo_ssie7_prefix, "cp -R /appli/mersi3/data/report ", mersi3_tmp_dir, _ );
//    print( "cmd='", cmd, "'", BR, _ );
    System( &s, cmd );
//    print( "s='", s, "'", BR, _ );
    strdcpy( &cmd, sudo_ssie7_prefix, "chown -R ssie:ssie ", mersi3_tmp_dir, _ );
//    print( "cmd='", cmd, "'", BR, _ );
    System( &s, cmd );
//    print( "s='", s, "'", BR, _ );
    strdcpy( &cmd, sudo_ssie7_prefix, "chmod -R 750 ", mersi3_tmp_dir, _ );
//    print( "cmd='", cmd, "'", BR, _ );
    System( &s, cmd );
//    print( "s='", s, "'", BR, _ );

	//--------------------------------------------------------------------------
//	if ( dir_exists( "/appli/mersi3/conf/login" ) )
    strdcpy( &wrk_dir, mersi3_tmp_dir, "/conf/login", _ );
	if ( dir_exists( wrk_dir ) )
    {
        strdcpy( &cmd, "ls ", wrk_dir, _ );
//        strdcpy( &cmd, "ls /appli/mersi3/conf/login", _ );
        System( &s, cmd );
//        printf("Résultat 'ls /appli/mersi3/conf/login' : '%s'.\n", s);
        if ( trim(s)[0] )
        {
            strdcpy( &cmd, "cp -Rf ", wrk_dir, "/* /appli/ssie7/conf/login" , _ );
//            strdcpy( &cmd, "cp -Rf /appli/mersi3/conf/login/* /appli/ssie7/conf/login" , _ );
            System( &s, cmd );
		    print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des login :" , s, BR BR, _ );
        	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des login : ", s, BR BR, _ );
        	if ( trim(s)[0] )
        		{ r = 1; goto fin; }
        }
    }     
	//--------------------------------------------------------------------------
    strdcpy( &wrk_dir, mersi3_tmp_dir, "/reveils", _ );
	if ( dir_exists( wrk_dir ) )
//	if ( dir_exists( "/appli/mersi3/data/reveils" ) )
    {
        strdcpy( &cmd, "ls ", wrk_dir, _ );
        System( &s, cmd );
//    	printf("Résultat 'ls /appli/mersi3/data/reveils' : '%s'.\n", s);
    	if ( trim(s)[0] )
    	{
            strdcpy( &cmd, "cp -Rf ", wrk_dir, "/* /appli/ssie7/conf/reveils" , _ );
            System( &s, cmd );
		    print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des réveils MFT :" , s, BR, _ );
        	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des réveils MFT : ", s, BR, _ );
        	if ( trim(s)[0] )
        		{ r = 1; goto fin; }
        }
    }	
	
	//--------------------------------------------------------------------------
	tf = NULL;

    strdcpy( &wrk_dir, mersi3_tmp_dir, "/conf/partenaires/*", _ );
	while( tf = file_list( tf, wrk_dir, "nodir" ) )
	{
		if ( (p=strpos( tf->name, ".dat", 0 )) < 0 ) continue;
		
		strdcpy( &cmd, ssie7_prefix, "part ", v = substr( tf->name, 0, p ), " loadnat", _ );
//	    print( "cmd='", cmd, "'", BR, _ );
		System( &s, cmd );
//        print( "s='", s, "'.", BR, _ );
        	
		if ( stripos( s, "ERREUR", 0 ) >= 0 )
		{
//			strdcpy( &fname, "/appli/mersi3/conf/partenaires/", substr( tf->name, 0, p ), ".dat", _ );
			strdcpy( &fname, mersi3_tmp_dir, "/conf/partenaires/", substr( tf->name, 0, p ), ".dat", _ );
			h = var_load( fname );
			var_to_str( h, &s );
			var_free( h );
			strd_replace( &s, "part_", "" );
			h = var_from_cmd( 0, s );
			strdcpy( &fname, "/appli/ssie7/conf/part/", substr( tf->name, 0, p ), ".dat", _ );
			var_save( h, fname );
			var_free( h );
			print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération du partenaire '", v, "' depuis le fichier de configuration local.", BR, _ );
			logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération du partenaire '", v, "' depuis le fichier de configuration local.", BR, _ );
		}
		else
        {
			print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération du partenaire : '", v, "' depuis l'annuaire national.", BR, _ );
			logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération du partenaire : '", v, "' depuis l'annuaire national.", BR, _ );
        }
		
		strdcpy( &fname, "/appli/mersi3/conf/partenaires/", substr( tf->name, 0, p ), ".no_maj_nat", _ );
		if ( file_exists( fname ) )
		{
//			strdcpy( &fname, "/appli/mersi3/conf/partenaires/", substr( tf->name, 0, p ), ".dat", _ );
			strdcpy( &fname, mersi3_tmp_dir, "/conf/partenaires/", substr( tf->name, 0, p ), ".dat", _ );
			h = var_load( fname );
			strdcpy( &cmd, "part ", substr( tf->name, 0, p ), " no_maj_nat adl_liste = ",
					quote( var_get( h, "part_adl_liste" ) ), _ );
//            print( "cmd='", cmd, "'", BR, _ );
			var_free( h );
			file_save_str( "/tmp/ssie7_migr_conf", cmd, -1 );
			strdcpy( &s1, ssie7_prefix, "@/tmp/ssie7_migr_conf", _ );
//    	    print( "s1='", s1, "'", BR, _ );
    		System( &s, s1 );
//    	    print( "s='", s, "'", BR, _ );
			logw( cmd, BR, s, BR BR, _ );
		}
	}
	
	strdcpy( &cmd, ssie7_prefix, "part MFTCCWEX loadnat", _ );
//    print( "cmd='", cmd, "'", BR, _ );
	System( &s, cmd );
//    print( "s='", s, "'.", BR, _ );
	
	//--------------------------------------------------------------------------
	
    strdcpy( &wrk_dir, mersi3_tmp_dir, "/export", _ );
	if ( dir_exists( wrk_dir ) )
    {
        strdcpy( &cmd, "ls ", wrk_dir, _ );
//	    print( "cmd='", cmd, "'", BR, _ );
		System( &s, cmd );
//	    print( "s='", s, "'.", BR, _ );
//    	printf("Résultat 'ls /appli/mersi3/data/export' : '%s'.\n", s);
    	if ( trim(s)[0] )
    	{
        	if ( ! dir_exists( "/appli/ssie7/data/export" ) )
        	   mkdir( "/appli/ssie7/data/export" );
            strdcpy( &cmd, "cp -Rf ", wrk_dir, "/* /appli/ssie7/data/export", _ );
//    	    print( "cmd='", cmd, "'", BR, _ );
    		System( &s, cmd );
//    	    print( "s='", s, "'.", BR, _ );
        	print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des exports : " BR, s, BR, _ );
        	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des exports : " BR, s, BR, _ );
        	if ( trim(s)[0] )
        		{ r = 1; goto fin; }
        }
    }
	//--------------------------------------------------------------------------

    strdcpy( &wrk_dir, mersi3_tmp_dir, "/import", _ );
	if ( dir_exists( wrk_dir ) )
//	if ( dir_exists( "/appli/mersi3/data/import" ) )
    {
        strdcpy( &cmd, "ls ", wrk_dir, _ );
//	    print( "cmd='", cmd, "'", BR, _ );
		System( &s, cmd );
//	    print( "s='", s, "'.", BR, _ );
//    	printf("Résultat 'ls /appli/mersi3/data/import' : '%s'.\n", s);
    	if ( trim(s)[0] )
    	{
        	if ( ! dir_exists( "/appli/ssie7/data/import" ) )
        	   mkdir( "/appli/ssie7/data/import" );
            strdcpy( &cmd, "cp -Rf ", wrk_dir, "/* /appli/ssie7/data/import", _ );
//    	    print( "cmd='", cmd, "'", BR, _ );
    		System( &s, cmd );
//    	    print( "s='", s, "'.", BR, _ );
        	print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des imports : " BR, s, BR, _ );
        	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des imports : " BR, s, BR, _ );
        	if ( trim(s)[0] )
        		{ r = 1; goto fin; }
        }
    }

/*	
	//--------------------------------------------------------------------------
	logw( "MIGRATION MERSI v3 : RECUPERATION DES LOG DES FLUX" BR BR, _ );
	//--------------------------------------------------------------------------
*/
	//--------------------------------------------------------------------------
	logw( "MIGRATION MERSI v3 : RECUPERATION DES DESCRIPTIONS DES FLUX" BR BR, _ );
	//--------------------------------------------------------------------------
    mersi_flux_migration( t_work );
    	
/*
	//--------------------------------------------------------------------------
	logw( "MIGRATION MERSI v3 : RECUPERATION DES FLUX EN COURS" BR BR, _ );
	//--------------------------------------------------------------------------
    strdcpy( &wrk_dir, mersi3_tmp_dir, "/data/flux", _ );
	if ( dir_exists( wrk_dir ) )
//	if ( dir_exists( "/appli/mersi3/data/import" ) )
    {
        strdcpy( &cmd, "ls ", wrk_dir, _ );
//	    print( "cmd='", cmd, "'", BR, _ );
		System( &s, cmd );
//	    print( "s='", s, "'.", BR, _ );
//    	printf("Résultat 'ls /appli/mersi3/data/import' : '%s'.\n", s);
    	if ( trim(s)[0] )
    	{
            strdcpy( &cmd, "cp -Rf ", wrk_dir, "/* /appli/ssie7/conf/flux", _ );
//    	    print( "cmd='", cmd, "'", BR, _ );
    		System( &s, cmd );
//    	    print( "s='", s, "'.", BR, _ );
        	print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des flux : " BR, s, BR, _ );
        	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des flux : " BR, s, BR, _ );
        	if ( trim(s)[0] )
        		{ r = 1; goto fin; }
        }
    }
*/
	
	//--------------------------------------------------------------------------
	logw( "MIGRATION MERSI v3 : RECUPERATION DES STATISTIQUES" BR BR, _ );
	//--------------------------------------------------------------------------
    strdcpy( &wrk_dir, mersi3_tmp_dir, "/report", _ );
	if ( dir_exists( wrk_dir ) )
//	if ( dir_exists( "/appli/mersi3/data/import" ) )
    {
        strdcpy( &cmd, "ls ", wrk_dir, _ );
//	    print( "cmd='", cmd, "'", BR, _ );
		System( &s, cmd );
//	    print( "s='", s, "'.", BR, _ );
//    	printf("Résultat 'ls /appli/mersi3/data/import' : '%s'.\n", s);
    	if ( trim(s)[0] )
    	{
        	if ( ! dir_exists( "/appli/ssie7/data/report" ) )
        	   mkdir( "/appli/ssie7/data/report" );
            strdcpy( &cmd, "cp -Rf ", wrk_dir, "/* /appli/ssie7/data/report", _ );
//    	    print( "cmd='", cmd, "'", BR, _ );
    		System( &s, cmd );
//    	    print( "s='", s, "'.", BR, _ );
        	print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des statistiques : " BR, s, BR, _ );
        	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des statistiques : " BR, s, BR, _ );
        	if ( trim(s)[0] )
        		{ r = 1; goto fin; }
        }
    }

/*	
	mkdir( "/appli/ssie7/data/report" );
	System( &s, "cp -Rf /appli/mersi3/data/report/* /appli/ssie7/data/report" );
	logw( s, BR, _ );
*/
	
	//--------------------------------------------------------------------------
    strdcpy( &v, mersi3_tmp_dir, "/conf/mersi3.dat", _ );
	if ( ! file_exists( v ) )
	{
    	print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Le fichier ", v, " n'existe pas ou est inacessible.", BR BR, _ );
    	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Le fichier ", v, " n'existe pas ou est inacessible.", BR, _ );
		r = 1; goto fin;
	}

//    strdcpy( &cmd, ssie7_prefix, "ssie7 server", _ );
    strdcpy( &cmd, ssie7_prefix, "server", _ );
//    print( "cmd='", cmd, "'", BR, _ );
	System( &s, cmd );
//    print( "s='", s, "'.", BR, _ );
	print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Initialisation du serveur SSIE v7 :", BR,
			cmd, BR, trim(s), BR, _ );
	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Initialisation du serveur SSIE v7 :", BR,
			cmd, BR, trim(s), BR, _ );
		
//	h = var_load( "/appli/mersi3/conf/mersi3.dat" );
//    strdcpy( &v, wrk_dir, "/conf/mersi3.dat", _ );
	h = var_load( v );
	
    strdcpy( &s1, ssie7_prefix, "srvannu", _ );
//    print( "s1='", s1, "'", BR, _ );
	System( &s, s1 );
    s=(char_purge(unblank(s), '\n' ));
//    print( "s='", s, "' (len=", itoa(strlen(s)), ").", BR, _ );
    if ( strlen(s) == 12 )
    {
        strdcpy( &s1, ssie7_prefix, "srvannu list='si-ssie.edf.fr'", _ );
//        print( "s1='", s1, "'", BR, _ );
    	System( &s, s1 );
//        print( "s='", s, "'.", BR, _ );
    }
    if ( strpos( s, "none", 0 ) >= 0 )
    {
    	strdcpy( &cmd, "server partname=", var_get( h, "srv_part_name" ),
    			" ip=", quote( var_get( h, "srv_ip" ) ),
    			" dns=", quote( var_get( h, "srv_dns" ) ),
    			" port=", quote( var_get( h, "srv_port" ) ),
    			" adl_liste=", quote( var_get( h, "srv_adl_liste" ) ),
    			_ );
    }
    else
    	strdcpy( &cmd, "server", _ );
    	
	var_free( h );
	file_save_str( "/tmp/ssie7_migr_conf", cmd, -1 );
//        print( "cmd='", cmd, "'", BR, _ );
    strdcpy( &s1, ssie7_prefix, "@/tmp/ssie7_migr_conf", _ );
//        print( "s1='", s1, "'", BR, _ );
	System( &s, s1 );
//        print( "s='", s, "'.", BR, _ );

	print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Paramétrage du serveur SSIE v7 :", BR,
			cmd, BR, trim(s), BR, _ );
	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Paramétrage du serveur SSIE v7 :", BR,
			cmd, BR, trim(s), BR, _ );

	
	//--------------------------------------------------------------------------
	fin:

		
	strdcpy( &cmd, ssie7_prefix, "log", _ );
//    print( "cmd='", cmd, "'", BR, _ );
	System( &s, cmd );
//    print( "s='", s, "'.", BR, _ );
/*
	if ( r )
	{
		print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "ATTENTION : Il y a eu des erreurs !!!" BR BR, _ );
		logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "ATTENTION : Il y a eu des erreurs !!!" BR, _ );
    }
	else
	{
		// ENREGISTRE LES INFO POUR UN RETOUR ARRIERE EVENTUEL
		strdcpy( &fname, dirwrk, DIR_CHAR "previous_version", _ );
		file_save_str( fname, "mersi3", -1 );
		print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "AUCUNE ERREUR DETECTEE." BR BR, _ );
		logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "AUCUNE ERREUR DETECTEE." BR, _ );
	}
*/		
    if ( file_exists(prev_version_fname) )
       file_delete(prev_version_fname);
    if ( file_exists(prev_binaries_dname) )
       file_delete(prev_binaries_dname);
    if ( file_exists(curr_version_fname) )
       file_delete(curr_version_fname);
    if ( file_exists(curr_binaries_dname) )
       file_delete(curr_binaries_dname);

    file_save_str( prev_version_fname, PackageCode, -1 );

    file_save_str( curr_version_fname, curr_ver, -1 );
//print( "dirbin='", dirbin, "'.", _ );

    crea_rep( curr_binaries_dname );
/*
    strdcpy( &fname, dirbin, DIR_CHAR, "*", _ );
    file_copy( fname, curr_binaries_dname );
*/
    strdcpy( &cmd, "cp -f ", dirbin, DIR_CHAR, "*", " ", curr_binaries_dname, _ );
//    print( "cmd='", cmd, "'", BR, _ );
	System( &s, cmd );
//    print( "s='", s, "'.", BR, _ );
    
    System( &s, "chown -R ssie:ssie /appli/ssie7" );
    System( &s, "chmod -R 750 /appli/ssie7" );

	print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "TERMINEE." BR BR, _ );
	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "TERMINEE." BR
	"----------------------------------------------------------------------------", _ );

	return OK;
}




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
char *ssie7_prefix=null;
char *ssie7up_prefix=null;
char *sudo_ssie7_prefix=null;

#ifdef OS_AIX
strdcpy( &ssie7_prefix, "/ssie7/ssie7 ", _ );
strdcpy( &ssie7up_prefix, "/ssie7/ssie7up ", _ );
#elif OS_LINUX
strdcpy( &ssie7_prefix, "/logiciels/ssie7/ssie7 ", _ );
strdcpy( &ssie7up_prefix, "/logiciels/ssie7/ssie7up ", _ );
#endif
strdcpy( &sudo_ssie7_prefix, "sudo ", ssie7_prefix, _ );


    strdcpy( &cmd, sudo_ssie7_prefix, ssie7up_prefix, "go mftext", _ );
//    print( "cmd=", cmd, BR, _ );
	Start( cmd );
//    print( "After Start.", BR, _ );
    sleep(3);
//		print( s, BR, _ );

	// VERIFIE QUE LES FICHIERS MERSI3 SONT ACCESSIBLE
/*
//    strdcpy( &cmd, sudo_ssie7_prefix, "su - mft -c \"MFTUTIL mftext > /appli/mft/DATA/TEMP/mft.conf\"", _ );	
    strdcpy( &cmd, "su - mft -c \"MFTUTIL mftext > /appli/mft/DATA/TEMP/mft.conf\"", _ );	
//    strdcpy( &cmd, sudo_ssie7_prefix, "su - mft -c MFTUTIL mftext > /appli/mft/DATA/TEMP/mft.conf", _ );	
    print( "cmd='", cmd, "'", BR, _ );
	System( &s, cmd );
    print( "s='", s, "'.", BR, _ );
*/
    
//	if ( ! file_exists( "/appli/mft/DATA/TEMP/mft.conf" ) )
/*
    if ( stat( "/appli/mft/DATA/TEMP/mft.conf", &buffer ) != 0 )
    {
        if ( errno == EACCES )
        {
    		print( BR BR "   ATTENTION : veuillez donner les droits d'accès au fichier '/appli/mft/DATA/TEMP/mft.conf' :", BR,
                         "      chmod 777 /appli/mft/DATA/TEMP/mft.conf" BR BR, _ );
        }
        else
        {
    		print( BR BR "   ATTENTION : Pour une migration MFT, avant de tenter la migration," BR
    				"   Veuillez exécuter la commande suivante sous le login mft :" BR
    				"      MFTUTIL mftext > /appli/mft/DATA/TEMP/mft.conf" BR BR,
    				_ );
        }
		return;
//    				"      MFTUTIL mftext > /appli/mft/DATA/TEMP/mft.conf" BR BR,
	}
*/
/*	
	System( &s, "ps -ef | grep MFT | grep -v grep" );
	if ( strlen(s) != 0 )
	{
		print( BR BR "MIGRATION MFT ==> SSIE v7 : ATTENTION : arrêter MFT avant de lancer la migration." BR BR, _ );
		logw( BR BR "MIGRATION MFT ==> SSIE v7 : ATTENTION : arrêter MFT avant de lancer la migration." BR, _ );
		return;
    }
*/    
    if ( ! creation_arbo_ssie( t_work ) )	
    {
		print( "MIGRATION MFT ==> SSIE v7 : ", "problème lors de la création de l'arborescence de SSIE v7." BR BR, _ );
        logw( "MIGRATION MFT ==> SSIE v7 : ", "problème lors de la création de l'arborescence de SSIE v7." BR, _ );
		return -1;
    }
//    print( "Après creation_arbo_ssie.", BR, _ );
    

//	strdcpy( &fname, "/appli/mft/DATA/TEMP/mft.conf", _ );
	strdcpy( &fname, "/tmp/mft.conf", _ );
//    print( "MIGRATION MFT ==> SSIE v7 : ", "fname='", fname, "'." BR BR, _ );
	file_load( &s, fname );
//    print( "Après file_load (s='", s, "').", BR, _ );
    if ( ( p = strpos( s, "IDPARM0", 0 ) ) < 0 ) 
    {
         return -1;
    }
//    print( "MIGRATION MFT ==> SSIE v7 : ", "p='", itoa(p), "'." BR BR, _ );
    if ( ( p = strpos( s, "NPART", p ) ) < 0 ) 
    {
         return -1;
    }
//    print( "MIGRATION MFT ==> SSIE v7 : ", "p='", itoa(p), "'." BR BR, _ );
    p_deb = strpos( s, "'", p );
    p_fin = strpos( s, "'", p_deb + 1 );
//    print( "MIGRATION MFT ==> SSIE v7 : ", "p_deb='", itoa(p_deb), ", p_fin='", itoa(p_fin), "'." BR BR, _ );
     
    part_name = substr( s, p_deb + 1, p_fin - ( p_deb + 1 ) );
 	
//    print( "MIGRATION MFT ==> SSIE v7 : ", "partname='", part_name, "'." BR, _ );
	strdcpy( &s1, "part ", part_name, " get", _ );
/*
{partname:'DSDSAX1I',os:'UNIX',ip:'163.81.8.135',dns:'',adl_liste:'D98XSAP1,D99XSAP1,',port_mft:1761}
*/
//    print( "Avant nat_request (s1='", s1, "').", BR, _ );
	if ( nat_request( &part_data_save, s1 ) )
	{
//    	printf("nat_request='%s'.\n", s1);
    	
    
        if ( ( p = strpos( s, "MFTNET", p_fin ) ) < 0 ) 
        {
             return -1;
        }
    //    print( "MIGRATION MFT ==> SSIE v7 : ", "p='", itoa(p), "'." BR BR, _ );
        if ( ( p = strpos( s, "TCPINOUT", p ) ) < 0 ) 
        {
             return -1;
        }
        if ( ( p = strpos( s, "HOST", p ) ) < 0 ) 
        {
             return -1;
        }
    //    print( "MIGRATION MFT ==> SSIE v7 : ", "p='", itoa(p), "'." BR BR, _ );
        p_deb = strpos( s, "'", p );
        p_fin = strpos( s, "'", p_deb + 1 );
    //    print( "MIGRATION MFT ==> SSIE v7 : ", "p_deb='", itoa(p_deb), ", p_fin='", itoa(p_fin), "'." BR BR, _ );
         
        ip_address = substr( s, p_deb + 1, p_fin - ( p_deb + 1 ) );
    
    //    print( "MIGRATION MFT ==> SSIE v7 : ", "ip='", ip_address, "'." BR, _ );
    
    //return OK;
    /*
    	if ( !bMuet )
    	{
    		print( "MIGRATION MFT ==> SSIE v7 : ", "Vérifiez que MFT est arrêté et appuyez sur 'C' pour continuer." BR, _ );
    		c = kbget();
    		if (( c != 67 )&&( c != 99 ))
    		{
    			print( "MIGRATION MFT ==> SSIE v7 : ", "MIGRATION ABANDONNEE." BR BR, _ );
    	        logw( "MIGRATION MFT ==> SSIE v7 : ", "MIGRATION ABANDONNEE." BR, _ );
    			r = 1; return OK;
            }
    	}
    */	
    
    	mkdir( dirlog );
    	if ( !dir_exists( dirlog ) )
    		{
    		print( "MIGRATION MFT ==> SSIE v7 : ", "ERREUR : Création répertoire '", dirlog, "'" BR, _ );
            logw( "MIGRATION MFT ==> SSIE v7 : ", "ERREUR : Création répertoire '", dirlog, "'" BR, _ );
    		r = -1; return ERROR;
    		}
    	
    	
    	print( "MIGRATION MFT ==> SSIE v7 : ", "Récupération des réveils MFT." BR, _ );
    	logw( "MIGRATION MFT ==> SSIE v7 : ", "Récupération des réveils MFT." BR, _ );
    	
    	//--------------------------------------------------------------------------
    //	logw( "RECUPERATION DES REVEILS MFT" BR BR, _ );
    	//--------------------------------------------------------------------------
    	
    	mkdir( "/appli/ssie7/conf/reveils" );
    	System( &s, "cp -Rf /appli/mft/DATA/REVEIL/* /appli/ssie7/conf/reveils" );
    //	print( "MIGRATION MFT ==> SSIE v7 : ", "Récupération des réveils MFT" BR, s, BR, _ );
    	
    	
    	System( &s, "/logiciels/ssie7/ssie7 server" );
    	print( "MIGRATION MFT ==> SSIE v7 : ", "Initialisation du serveur SSIE v7" BR, _ );
    	logw( "MIGRATION MFT ==> SSIE v7 : ", "Initialisation du serveur SSIE v7" BR, _ );
    		
    	strdcpy( &cmd, "server partname=", part_name,
    			" ip=", ip_address,
    			_ );
    	
    	var_free( h );
    	file_save_str( "/tmp/ssie7_migr_conf", cmd, -1 );
    	System( &s, "ssie7 @/tmp/ssie7_migr_conf" );
    	print( "MIGRATION MFT ==> SSIE v7 : ", "Paramétrage du serveur SSIE v7" BR, _ );
    	logw( "MIGRATION MFT ==> SSIE v7 : ", "Paramétrage du serveur SSIE v7" BR, _ );
    	
        if ( ! stricmp( PackageCode, "mft241" ) )
        {
//            strdcpy( &fname, "/home/mft/2.4.1/config/PART_LOC.CFG", _ );
            strdcpy( &fname, "/tmp/PART_LOC.CFG", _ );
            if ( stat( fname, &buffer ) == 0 )
            {
                file_load( &s, fname );
                s = unblank( s );
            	print( "MIGRATION MFT ==> SSIE v7 : ", "Récupération des partenaires locaux :" BR, _ );
            	logw( "MIGRATION MFT ==> SSIE v7 : ", "Récupération des partenaires locaux :" BR, _ );
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
                    	print( "MIGRATION MFT ==> SSIE v7 : ", "Partenaire '", part_name, "'." BR, _ );
                    	logw( "MIGRATION MFT ==> SSIE v7 : ", "Partenaire '", part_name, "'." BR, _ );
    
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
        						part_port = unquote ( trim ( substr( s, p_deb, p_fin - p_deb ) ) );
            				    var_set( hPart, "port_mft", unquote ( trim ( substr( s, p_deb, p_fin - p_deb ) ) ) );
        					}
                		}
    	                var_save( hPart, fname1 );
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
    /*
    	strdcpy( &fname, dirwrk, DIR_CHAR "previous_version", _ );
    //	print( "MIGRATION MFT ==> SSIE v7 : fname='", fname, "'." BR BR, _ );
    	file_save_str( fname, "mft", -1 );
    
    	strdcpy( &fname, dirwrk, DIR_CHAR "previous_data", _ );
    //	print( "MIGRATION MFT ==> SSIE v7 : fname='", fname, "', data='", part_data_save, "'." BR BR, _ );
    	file_save_str( fname, part_data_save, -1 );
    */
        if ( file_exists(prev_version_fname) )
           file_delete(prev_version_fname);
        if ( file_exists(prev_binaries_dname) )
           file_delete(prev_binaries_dname);
        if ( file_exists(curr_version_fname) )
           file_delete(curr_version_fname);
        if ( file_exists(curr_binaries_dname) )
           file_delete(curr_binaries_dname);
    
        file_save_str( prev_version_fname, PackageCode, -1 );
    
        file_save_str( curr_version_fname, curr_ver, -1 );
    
        crea_rep( curr_binaries_dname );
    
        crea_rep( prev_binaries_dname );
    	strdcpy( &fname, prev_binaries_dname, DIR_CHAR "MFT.dat", _ );
    //	print( "MIGRATION MFT ==> SSIE v7 : fname='", fname, "', data='", part_data_save, "'." BR BR, _ );
    	file_save_str( fname, part_data_save, -1 );
    /*
        strdcpy( &fname, dirbin, DIR_CHAR, "*", _ );
        file_copy( fname, curr_binaries_dname );
    */
        strdcpy( &cmd, "cp -f ", dirbin, DIR_CHAR, "*", " ", curr_binaries_dname, _ );
        System( &s, cmd );
    
        System( &s, "chown -R ssie:ssie /appli/ssie7" );
        System( &s, "chmod -R 750 /appli/ssie7" );
    }
    else
    {
   		print( "MIGRATION MFT ==> SSIE v7 : ", "ERREUR : impossible de  se connecter à l'annuaire." BR BR, _ );
		logw( "MIGRATION MFT ==> SSIE v7 : ", "ERREUR : impossible de  se connecter à l'annuaire." BR, _ );

    }

	print( "MIGRATION MFT ==> SSIE v7 : ", "TERMINEE." BR BR, _ );
	logw( "MIGRATION MFT ==> SSIE v7 : ", "TERMINEE." BR, _ );
	
    return OK;
}




//------------------------------------------------------------------------------
int upgrade_back( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
char *_cmd=null;
char *_s=null;

/*
	// VERIFIE QUE LES FICHIERS MERSI3 SONT ACCESSIBLE
	
	strdcpy( &fname, dirwrk, DIR_CHAR "previous_version", _ );
	
	if ( !file_exists( fname ) )
		{
		print( "Version précédente inconnue. Pas de retour arrière possible." BR BR,
				_ );
		r = 1; return ERROR;
		}
*/
/*	
	System( &s, "ps -ef | grep ssie7 | grep -v grep" );
	if ( strlen(s) != 0 )
	{
		print( BR BR "   ATTENTION : arrêter SSIE v7 avant de lancer la migration." BR BR,
				_ );
		return;
    }
	if ( s ) s[0] = 0;
	file_load( &s, fname );
*/    
	if ( !strcmp( prev_ver, "mersi3" ) )
	{
/*
		if ( !bMuet )
			{
			print( "ANNULATION DE LA MIGRATION MERSI v3 ==> SSIE v7" BR BR, _ );
			print( "Appuyez sur 'C' pour confirmer." BR, _ );
			c = kbget();
			if (( c != 67 )&&( c != 99 ))
				{
				print( "ANNULATION DE LA MIGRATION MERSI v3 ABANDONNEE." BR BR, _ );
				r = 1; return OK; }
			}
*/		
		strdcpy( &fname, "/appli/mersi3/conf/mersi3.dat", _ );
		hSrvConf = var_load( fname );
		
		partname = var_get( hSrvConf, "srv_part_name" );
		
		hReq = var_new();
		var_set( hReq, "ip", var_get( hSrvConf, "srv_ip" ) );
		var_set( hReq, "descr", var_get( hSrvConf, "srv_descr" ) );
		var_set( hReq, "dns", var_get( hSrvConf, "srv_dns" ) );
		var_set( hReq, "adl_liste", var_get( hSrvConf, "srv_adl_liste" ) );
		var_set( hReq, "is_onr", var_get( hSrvConf, "srv_is_onr" ) );
		var_set( hReq, "port_mersi", var_get( hSrvConf, "srv_port" ) );
		var_set( hReq, "port_mft", var_get( hSrvConf, "srv_port_mft" ) );
		var_set( hReq, "port_ssl", var_get( hSrvConf, "srv_port_ssl" ) );
		var_set( hReq, "port_ssl2", var_get( hSrvConf, "srv_port_ssl2" ) );
		var_set( hReq, "network", var_get( hSrvConf, "srv_network" ) );
		var_set( hReq, "os", var_get( hSrvConf, "srv_os" ) );
		var_set( hReq, "mersi_version", var_get( hSrvConf, "srv_mersi_version" ) );
		var_set( hReq, "mft_version", var_get( hSrvConf, "srv_mft_version" ) );
		var_set( hReq, "mft_about", var_get( hSrvConf, "srv_mft_about" ) );
		var_set( hReq, "mft_maxtran", var_get( hSrvConf, "srv_mft_maxtran" ) );
		var_set( hReq, "mft_ssl_authorized", var_get( hSrvConf, "srv_mft_ssl" ) );
		var_set( hReq, "ssl_certifu", var_get( hSrvConf, "srv_ssl_certifu" ) );
		var_set( hReq, "ssl_certifr", var_get( hSrvConf, "srv_ssl_certifr" ) );
		var_set( hReq, "ssl_certifi1", var_get( hSrvConf, "srv_ssl_certifi1" ) );
		var_set( hReq, "ssl_certifi2", var_get( hSrvConf, "srv_ssl_certifi2" ) );
		var_set( hReq, "ssl_certifi3", var_get( hSrvConf, "srv_ssl_certifi3" ) );
		var_set( hReq, "ssl_certifi4", var_get( hSrvConf, "srv_ssl_certifi4" ) );
		var_set( hReq, "rescue_part", var_get( hSrvConf, "srv_rescue_part" ) );

		// MISE A JOUR DES SERVEURS NATIONAUX
		
		var_to_str( hReq, &s );
		strdcpy( &s, "part ", partname, " ", s, _ );
		nat_request( &s1, s );
//		print( "nat_request : s='", s, "', s1='", s1, "'.", BR, _ );
/*
	#ifdef OS_LINUX
	strdcpy( &_cmd, "sudo -u mersi -n mersi3 server save", _ );
	System( &_s, _cmd );
	#else
	strdcpy( &_cmd, "sudo -S mersi3 server save << fin\n\n\n\nquit\nfin\n", _ );
	System( &_s, _cmd );
	#endif
*/
		logw( "ANNULATION MIGRATION MERSI v3 : MISE A JOUR DE L'ANNUAIRE NATIONAL :" BR
			"------------------------------------------------------------------------------" BR,
//			 s, BR BR, s1, BR BR
			 s, BR, "---" BR, s1, BR, "---" BR,
			"------------------------------------------------------------------------------" BR
			 , _ );
		
		print( "ANNULATION MIGRATION MERSI v3 : MISE A JOUR DE L'ANNUAIRE NATIONAL :" BR
			"------------------------------------------------------------------------------" BR,
//			 s, BR BR, "---" BR, s1, "---" BR, BR BR
//			 "---" BR, s1, "---" BR, BR BR
			 s, BR, "---" BR, s1, BR, "---" BR,
			"------------------------------------------------------------------------------" BR
			 , _ );
		
        if ( file_exists(curr_version_fname) )
           file_delete(curr_version_fname);
        if ( file_exists(curr_binaries_dname) )
           file_delete(curr_binaries_dname);
        if ( file_exists(prev_version_fname) )
           file_delete(prev_version_fname);
        if ( file_exists(prev_binaries_dname) )
           file_delete(prev_binaries_dname);

				print( "ANNULATION DE LA MIGRATION MERSI v3 TERMINEE." BR BR, _ );
		}
	else
	if ( strpos( prev_ver, "mft", 0 ) >= 0 )
	{
/*
		if ( !bMuet )
		{
			print( "ANNULATION DE LA MIGRATION MFT ==> SSIE v7" BR BR, _ );
			print( "Appuyez sur 'C' pour confirmer." BR, _ );
			c = kbget();
			if (( c != 67 )&&( c != 99 ))
			{
				print( "ANNULATION DE LA MIGRATION MFT ABANDONNEE." BR BR, _ );
				r = 1; return OK;
            }
		}
*/
	strdcpy( &fname, prev_binaries_dname, DIR_CHAR "MFT.dat", _ );
//	strdcpy( &fname, dirwrk, DIR_CHAR "previous_data", _ );
	file_load( &s, fname );
//	print( "s='", s, "'." BR, _ );
	if ( hPart ) var_free( hPart );
	hPart = var_from_str( 0, s );
	strdcpy( &s, "part ", var_get( hPart, "partname" ), " ip='", var_get( hPart, "ip" ), "'",
                 " dns='", var_get( hPart, "dns" ), "'", " os='", var_get( hPart, "os" ), "'",
                 " port_mft='", var_get( hPart, "port_mft" ), "'", " adl_liste='", var_get( hPart, "adl_liste" ), "'",
                  _ );
	nat_request( &s1, s );
//	print( "MIGRATION MFT ==> SSIE v7 : fname='", fname, "', data='", part_data_save, "'." BR BR, _ );
		logw( "ANNULATION MIGRATION MFT : MISE A JOUR DE L'ANNUAIRE NATIONAL :" BR
			"------------------------------------------------------------------------------" BR,
//			 s, BR BR, s1, BR BR
			 s, BR, "---" BR, s1, BR, "---" BR,
			"------------------------------------------------------------------------------" BR
			 , _ );
		
		print( "ANNULATION MIGRATION MFT : MISE A JOUR DE L'ANNUAIRE NATIONAL :" BR
			"------------------------------------------------------------------------------" BR,
//			 s, BR BR, "---" BR, s1, "---" BR, BR BR
//			 "---" BR, s1, "---" BR, BR BR
			 s, BR, "---" BR, s1, BR, "---" BR,
			"------------------------------------------------------------------------------" BR
			 , _ );

        if ( file_exists(curr_version_fname) )
           file_delete(curr_version_fname);
        if ( file_exists(curr_binaries_dname) )
           file_delete(curr_binaries_dname);
        if ( file_exists(prev_version_fname) )
           file_delete(prev_version_fname);
        if ( file_exists(prev_binaries_dname) )
           file_delete(prev_binaries_dname);

				print( "ANNULATION DE LA MIGRATION MFT TERMINEE." BR BR, _ );
	}
	else
	{
		print( "ANNULATION DE LA MISE A JOUR DE SSIE : " BR BR, _ );
/*
		if ( !bMuet )
		{
			print( "Appuyez sur 'C' pour confirmer." BR, _ );
			c = kbget();
			if (( c != 67 )&&( c != 99 ))
			{
				print( "ANNULATION DE LA MISE A JOUR DE SSIE ABANDONNEE." BR BR, _ );
				r = 1;
                return OK;
            }
		}
*/		
        if ( ! file_exists(prev_version_fname) )
        {
             print( "ANNULATION DE LA MISE A JOUR DE SSIE : Aucune installation antérieure n'a été trouvée.", BR BR, _ );
             print( "ANNULATION DE LA MISE A JOUR DE SSIE : RETOUR ARRIERE ANNULE.", BR BR, _ );
             exit(2);
        }
        if ( file_exists(curr_version_fname) )
           file_delete(curr_version_fname);
        if ( file_exists(curr_binaries_dname) )
           file_delete(curr_binaries_dname);
        
        if ( file_exists(prev_version_fname) )
           file_move(prev_version_fname, curr_version_fname);
        if ( file_exists(prev_binaries_dname) )
           file_move(prev_binaries_dname, curr_binaries_dname);
        
        strdcpy( &cmd, "cp -f ", curr_binaries_dname, DIR_CHAR, "*", " ", dirbin, _ );
        System( &s, cmd );

        print( "ANNULATION DE LA MISE A JOUR DE SSIE : RETOUR ARRIERE EFFECTUE.", BR BR, _ );
    }

    print( "ANNULATION DE LA MISE A JOUR DE SSIE : effacement du contenu du répertoire '/appli/ssie7'.", BR BR, _ );
    strdcpy( &cmd, "rm -rf /appli/ssie7/*", _ );
    System( &s, cmd );

    
	return OK;
}
	
	
int migration_ssie( T_WORK *t_work )
{
/*
//	print( "MIGRATION MFT ==> SSIE v7 : fname='", fname, "'." BR BR, _ );
	strdcpy( &cmd, dirbin, DIR_CHAR "ssie7" EXE " version", _ );
	System( &s1, cmd );
//	file_save_str( fname, str_replace( s1, " ", "_" ), -1 );
	file_save_str( fname, s1, -1 );
*/
        #ifdef OS_WINDOWS
        strdcpy( &fname1, "C:", DIR_CHAR, "Temp", _ );
        if ( ! dir_exists( fname1 ) )
           crea_rep( fname1 );
        //	System( &s, "chmod 755 ssie7" );
        #else
        strdcpy( &fname1, DIR_CHAR, "tmp", _ );
        #endif
                print( "fname1='", fname1, "'.", BR, _ );
        strdcpy( &fname2, fname1, DIR_CHAR, file_getname(argv[0]), ".", itoa( getpid() ), _ );
                print( "fname2='", fname2, "'.", BR, _ );
        crea_rep( fname2 );
        #ifndef OS_WINDOWS
        strdcpy( &cmd, "chmod 777 ", fname2, _ );
                print( "cmd='", cmd, "'.", BR, _ );
        System( &s, cmd );
        #endif
        chdir( fname2 );
        
        strdcpy( &fname3, fname1, DIR_CHAR, "ftp.", itoa( getpid() ), ".log", _ );
                print( "fname3='", fname3, "'.", BR, _ );
//        print( "fname3='", fname3, "'.", _ );
/*
        strdcpy( &cmd, "ftp -n 10.151.29.82 21 << fin_FTP > ", fname3, BR,
        "user ssie7src ssie7src" BR,
*/
        /*
        strdcpy( &cmd, "ftp -n 10.151.29.25 << fin_FTP > ", fname3, BR,
                "user ssie7src EDF_mft_2015" BR,
        */
/*
        strdcpy( &cmd, "ftp -n 10.151.29.82 21 << fin_FTP > ", fname3, BR,
        "user ssie7src ssie7src" BR,
*/
/*
        strdcpy( &cmd, "ftp -n 10.151.29.100 << fin_FTP > ", fname3, BR,
                "user comp_v7.1 compile" BR,
*/
		strdcpy( &cmd, "ftp -n 10.151.29.82 721 << fin_FTP > ", fname3, BR,
					"user ssiestore ssiestore", BR,
    				"verbose" BR,
    				"prompt -n" BR,
    				"lcd ", fname2, BR,
    				"binary ", BR,
    				"ls", BR,
//    				"cd Bin/", OS_NAME, "/", PackageCode, BR,
    				"cd bin/", OS_NAME, "/", PackageCode, BR,
    				"ls", BR,
    				"mget *" BR,
    				"quit" BR,
    				"fin_FTP" BR, _ );
        print( "cmd='", cmd, "'.", BR BR, _ );
    	System( &s, cmd	);	
//    }
/*
//	strdcpy( &fname1, dirwrk, DIR_CHAR "current_version", _ );
	strdcpy( &fname2, dirwrk, DIR_CHAR "previous_version", _ );
    file_copy( fname1, fname2);
*/
  
    if ( file_exists(prev_version_fname) )
       file_delete(prev_version_fname);
    if ( file_exists(prev_binaries_dname) )
       file_delete(prev_binaries_dname);
       
    if ( file_exists(curr_version_fname) )
       file_move(curr_version_fname, prev_version_fname);
    if ( file_exists(curr_binaries_dname) )
       file_move(curr_binaries_dname, prev_binaries_dname);

    file_save_str( curr_version_fname, PackageCode, -1 );

    crea_rep( curr_binaries_dname );
    strdcpy( &cmd, "cp -f ", fname2, DIR_CHAR, "*", " ", curr_binaries_dname, _ );
    System( &s, cmd );
    strdcpy( &cmd, "cp -f ", fname2, DIR_CHAR, "*", " ", dirbin, _ );
    System( &s, cmd );

    return OK;
}


char ** get_flux_line( char *_str )
{
     char **str_list=null, *str=null;
//     *str=null;
     int list_ind=0, str_ind=0;
     int ind=0, beg_ind=0, end_ind=0;
     int pos=0;
     int continue_parse=0, desactivate_char=0;
     
//print("get_flux_line : ", "_str='", _str, "'.", BR, _ );
//print("get_flux_line : ", "strlen(_str)='", itoa(strlen(_str)), "'.", BR, _ );
//	str_list = (char**)malloc( strlen(_str)*2*sizeof(char*) );
//	str_list = (char**)calloc( 1, strlen(_str)+1+(sizeof(char*)*2001) );
//print("get_flux_line : ", "strlen(_str)*2*sizeof(char*)='", itoa(strlen(_str)*2*sizeof(char*)), "'.", BR, _ );
	str_list = (char**)calloc( 10, sizeof(char*) );
//print("get_flux_line : ", "strlen(str_list[0])='", itoa(strlen(str_list[0])), "'.", BR, _ );
//	if ( str_list == NULL ) return NULL;
   str = calloc(strlen(_str), sizeof(char));
//	str = (char*)((unsigned long int)str_list + (unsigned long int)(sizeof(char*)*2000));

//exit(0);

     if ( _str[0] == '[' )
     {
        ind++;
        beg_ind=ind;
//print("get_flux_line : ", "ind=", itoa(ind), ", beg_ind=", itoa(beg_ind), ".", BR, _ );
     }
     if ( ( ( pos = strpos( _str, "][]", 0 ) ) >= 0 ) && _str[pos+3] == '\0' )
     {
        _str[pos] = '\0';
//print("get_flux_line : ", "pos=", itoa(pos), ".", BR, _ );
//print("get_flux_line : ", "_str='", _str, "'.", BR, _ );
     }
     end_ind=-1;
     while ( true )
     {
            if ( ! continue_parse )
            {
                if ( _str[ind] == ',' || _str[ind] == '\0')
                {
                     if ( end_ind == -1 )
                        end_ind=ind;
//print("get_flux_line : ", "beg_ind=", itoa(beg_ind), ", end_ind=", itoa(end_ind), ".", BR, _ );
//print("get_flux_line : ", "end_ind-beg_ind+1=", itoa(end_ind-beg_ind+1), ".", BR, _ );
/*
                     str = calloc(end_ind-beg_ind+1, sizeof(char));
                     if ( str == NULL )
                     {
print("get_flux_line : ", "ERREUR : calloc.", BR, _ );
break;
                     }
*/
//print("get_flux_line : ", "strlen(str)=", itoa(strlen(str)), ".", BR, _ );
//print("get_flux_line : ", "end_ind-beg_ind=", itoa(end_ind-beg_ind), ".", BR, _ );
                     
		             memcpy( str, &(_str[beg_ind]), end_ind-beg_ind );
                     str[end_ind-beg_ind]='\0';
//print("get_flux_line : ", "str='", str, "'.", BR, _ );
                     strdcpy(&str_list[list_ind], str, _ );
//print("get_flux_line : ", "str_list[", itoa(list_ind), "]='", str_list[list_ind], "'.", BR, _ );
                     if ( _str[ind] == '\0' )
                        break;
                     list_ind++;
                     beg_ind=ind+1;
                     end_ind=-1;
                     
                }
                if ( _str[ind] == '\'' )
                {
                   continue_parse=1;
                   beg_ind=ind+1;
                }
            }
            else
            {
                if ( _str[ind] == '\\' )
                {
                   desactivate_char=1;
                }
                else
                {
                    if ( ( ! desactivate_char) && ( _str[ind] == '\'' ) )
                    {
                       continue_parse=0;
                       end_ind=ind;
                    }
                    desactivate_char=0;
                }
            }
            ind++;
     }
     
     return str_list;
}




int mersi_flux_migration( T_WORK *t_work )
{
//    char *mersi_flux_dir="/appli/mersi3/conf/applications";
    char *_ssie_flux_dir="/appli/ssie7/conf/flux";
    char *_mersi3_tmp_dir=null;
    char *_wrk_dir=null, *_wrk_file_pattern=null;
    char *_wrk_date=null;
    int _p_deb=0, _p_fin=0, _sav_id=0;
char *ssie7_prefix=null;
char *ssie7up_prefix=null;
char *sudo_ssie7_prefix=null;
char *mersi3_prefix=null;

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
    
    strdcpy( &_mersi3_tmp_dir, "/tmp/mersi3/", itoa( getpid() ), _ );

/* Récupération des sources des flux.
*/    
	//--------------------------------------------------------------------------
    strdcpy( &_wrk_dir, _mersi3_tmp_dir, "/conf/applications", _ );
	if ( dir_exists( _wrk_dir ) )
//	if ( dir_exists( "/appli/mersi3/conf/applications" ) )
    {
        strdcpy( &cmd, "ls ", _wrk_dir, _ );
//	    print( "cmd='", cmd, "'", BR, _ );
		System( &s, cmd );
//	    print( "s='", s, "'.", BR, _ );
//    	printf("Résultat 'ls /appli/mersi3/conf/applications' : '%s'.\n", s);
    	if ( trim(s)[0] )
    	{
            strdcpy( &cmd, "cp -Rf ", _wrk_dir, "/* /appli/ssie7/conf/flux", _ );
//    	    print( "cmd='", cmd, "'", BR, _ );
    		System( &s, cmd );
//    	    print( "s='", s, "'.", BR, _ );
        	print( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des flux : " BR, s, BR, _ );
        	logw( "MIGRATION MERSI v3 ==> SSIE v7 : ", "Récupération des flux : " BR, s, BR, _ );
        	if ( trim(s)[0] )
        		{
                 r = 1; 
                //return OK;
                }
        }
    }
	tf = NULL;
	
	//--------------------------------------------------------------------------
	// TRANSFORME LES : EN @

	while( tf = file_list( tf, "/appli/ssie7/conf/flux/*", "nodir" ) )
	{
		if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;
		
/*
		if ( ( p = strpos( tf->name, ":", 0 ) ) < 0 ) continue;
		
		strdcpy( &fname, "/appli/ssie7/conf/flux/", tf->name, _ );
        strdcpy( &fname1, "/appli/ssie7/conf/flux/", str_replace( tf->name, ":", "@" ), _ );
*/
		strdcpy( &fname, "/appli/ssie7/conf/flux/", tf->name, _ );
		if ( ( p = strpos( tf->name, ":", 0 ) ) >= 0 )
		{
             fname1 = str_replace( fname, ":", "@" );
             rename( fname, fname1 );
             fname = fname1;
        }
		h1 = var_load( fname );
		v = var_get( h1, "agr_start_cond" );
        if ( v )
        {
           _p_deb = 0;
           if ( ( _p_deb = strpos( v, "[", 0 ) ) >= 0 ) 
           {
                if ( ( _p_fin = strpos( v, "]", _p_deb + 1 ) ) >= 0 )
                   strdcpy( &v, substr( v, _p_fin + 1, -1 ), _ );
           }
           v = str_replace( trim( v ), ":", "=>" );
//print("mersi_flux_migration : ", "fichier '", fname, ", mise à jour de la variable 'agr_start_cond' : '", v, "'.", BR, _ );
           var_set( h1, "agr_start_cond", v );
           var_save( h1, fname );
        }
    } 


/* Récupération des instances et des logs des flux.
*/    
	//--------------------------------------------------------------------------
    strdcpy( &_wrk_dir, _mersi3_tmp_dir, "/flux", _ );
//print( "mersi_flux_migration :", " répertoire '", _wrk_dir, "'.", BR, _ );
	if ( dir_exists( _wrk_dir ) )
//	if ( dir_exists( "/appli/mersi3/conf/applications" ) )
    {
         mkdir("/appli/ssie7/data/flux");
/*
         strdcpy( &s2, "/appli/ssie7/data/flux/log.id", _ );
         if ( file_exists( s2 ) )
            unlink( s2 ); 
*/
         strdcpy( &fname3, "/tmp/migration.", itoa(getpid()), _ );
//print( "mersi_flux_migration :", " fichier '", fname3, "'.", BR, _ );
         for( i=30 ; i >= 0 ; i-- )
         {
            _wrk_date = itod( time(null)-(i*24*60*60), "aaaa_mm_jj" );
//print( "mersi_flux_migration :", " _wrk_date='", _wrk_date, "'.", BR, _ );
//            strdcpy( &cmd, sudo_ssie7_prefix, "\"", mersi3_prefix, "flux log date=", _wrk_date, "\" > /tmp/migration.", itoa(getpid()), ".", itoa(i), _ );
            strdcpy( &cmd, sudo_ssie7_prefix, " su - mersi -c \"'", mersi3_prefix, "flux log date=", _wrk_date, "'\" >> ", fname3, _ );
//print( "mersi_flux_migration :", " cmd='", cmd, "'.", BR, _ );
            system( cmd );
         }
         file_load( &s3, fname3 );
         ts = str_explode( s3, "\n" );
         strdcpy( &s2, "/appli/ssie7/data/flux/log.id", _ );
         for ( i=0 ; ts[i] ; i++ )
         {  		
              trim(ts[i]);
              if (( !ts[i] )||( ! (ts[i][0]) ))
                 continue;
//print( "mersi_flux_migration :", " ts[", itoa( i ), "]='", ts[i], "'.", BR, _ );
              ts1 = str_explode( ts[i], " : " );

for ( j=0 ; ts1[j] ; j++ )
//print( "mersi_flux_migration :", " premier ts1, ts1[", itoa( j ), "]='", ts1[j], "'.", BR, _ );

              strdcpy( &cmd, "cp ", _wrk_dir, DIR_CHAR, ts1[0], ".dat /appli/ssie7/data/flux", _ );
//print( "mersi_flux_migration :", " cmd='", cmd, "'.", BR, _ );
              system( cmd );
              strdcpy( &s4, "/appli/ssie7/data/flux/", ts1[0], ".dat", _ );
//	if ( h ) var_free( h );	
              h = var_load( s4 );
//print( "mersi_flux_migration :", " app_name='", var_get( h, "app_name" ), "', .", BR, _ );
//print( "mersi_flux_migration :", " flux_name='", var_get( h, "flux_name" ), "', .", BR, _ );
//print( "mersi_flux_migration :", " flux_dt_start='", var_get( h, "flux_dt_start" ), "', .", BR, _ );
//print( "mersi_flux_migration :", " status='", var_get( h, "status" ), "'.", BR, _ );
//              hLog = hlog_init( "/appli/ssie7/data/flux", ts1[0] );
             if ( file_exists( s2 ) )
                unlink( s2 ); 
//print( "mersi_flux_migration :", " ts1[0]='", ts1[0], "'.", BR, _ );
             for ( n=1 ; n < atoi(ts1[0]) ; n++ )
    //             print( "mersi_flux_migration :", " get_uid( s2 )='", itoa(get_uid( s2 )), "'.", _ );
                 get_uid( s2 );

              hLog = hlog_init( "/appli/ssie7/data/flux", null );
//print( "mersi_flux_migration :", " hLog='", itoa(hLog), "'.", BR, _ );
	hlogu( hLog, 0, var_get( h, "status" ), ts1[0], ":", var_get( h, "app_name" ), "/", var_get( h, "flux_name" ), _ );
		hlogu_date( hLog, 0, dtoi( var_get( h, "flux_dt_start" ), "aaaa/mm/jj hh:mm:ss" ) );
              strdcpy( &cmd, sudo_ssie7_prefix, " su - mersi -c \"'", mersi3_prefix, "flux ", ts1[0], " log of=js'\" > ", fname3, _ );
//print( "mersi_flux_migration :", " cmd='", cmd, "'.", BR, _ );
              system( cmd );
              file_load( &s3, fname3 );
              ts1 = str_explode( s3, "],[" );
              for ( j=0 ; ts1[j] ; j++ )
              {  		
//print( "mersi_flux_migration :", " ts1[", itoa( j ), "]='", ts1[j], "'.", BR, _ );
                  ts2=get_flux_line(ts1[j]);
/**/
                  l=0;
//print( "mersi_flux_migration :", " l='", itoa(l), "'.", BR, _ );
                  if ( ts2[8] && strlen(ts2[8]) != 0 ) 
                     l=atoi(ts2[8]);
//print( "mersi_flux_migration :", " l='", itoa(l), "'.", BR, _ );
strdcpy( &v, "", _ );
for ( e=2; e <= 6; e++ )
{
    if ( strlen(ts2[e]) > 0 )
    {
       if ( strlen(v) == 0 )
          strdcpy( &v, ts2[e], _ );
       else
          strdcpy( &v, v, " ", ts2[e], _ );
    }
}

//                  hlogc( hLog, l, "", ts2[2], ts2[3], ts2[4], ts2[5], ts2[6], _ );
                  id = hlogc( hLog, l, "", v, _ );
		hlogu_date( hLog, id, dtoi( ts2[1], "aaaa/mm/jj hh:mm:ss" ) );
//	              hlogc( hLog, 0, "", "Démarrage demandé par ", user, ", flux_id = ", flux_id, _ );
/*
                  for ( k=0 ; ts2[k] ; k++ )
                    {  		
print( "mersi_flux_migration :", " ts2[", itoa( k ), "]='", ts2[k], "'.", BR, _ );
                  }
*/
/**/                  
              }
         }
/*
         strdcpy( &_wrk_file_pattern, _wrk_dir, "/*", _ );
    	 while( tf = file_list( tf, _wrk_file_pattern, "nodir" ) )
    	 {
            strdcpy( &fname1, _wrk_dir, "/", tf->name, _ );

    		if ( strpos( tf->name, ".jslog", 0 ) >= 0 || strpos( tf->name, ".dat", 0 ) >= 0 )
            {
                 strdcpy( &fname2, "/appli/ssie7/data/flux/", tf->name, _ );
                 s = substr( tf->name, 0, strpos( tf->name, ".", 0 ) );
//print( "mersi_flux_migration :", " id='", s, "'.", BR, _ );
                 if ( atoi( s ) > _sav_id )
                 {
                    _sav_id = atoi( s );
//print( "mersi_flux_migration :", " _sav_id='", s, "'.", BR, _ );
                 }
                 file_copy( fname1, fname2 );
            }
         }
         for ( i=1 ; i <= _sav_id ; i++ )
             get_uid( s2 );
    /*
    		if ( ( p = strpos( tf->name, ":", 0 ) ) < 0 ) continue;
    		
    		strdcpy( &fname, "/appli/ssie7/conf/flux/", tf->name, _ );
            strdcpy( &fname1, "/appli/ssie7/conf/flux/", str_replace( tf->name, ":", "@" ), _ );
    */
//    		strdcpy( &fname, "/appli/ssie7/conf/flux/", tf->name, _ );
    }
    
    return OK;
}









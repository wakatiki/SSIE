struct stat buffer;  

char *ssie7_prefix=null;
//char *ssie7up_prefix=null;
char *sudo_ssie7_prefix=null;

char *curr_version_fname=null, *prev_version_fname=null, *prev_install_version_fname=null;
char *curr_binaries_dname=null, *prev_binaries_dname=null, *prev_install_config_dname=null;
char *curr_ver=null, *prev_ver=null, *prev_inst=null;




int migr_pepsis( T_WORK* t_work )
	{
	print( "--------------------------------------------------------------------------------", BR, _ );
	print( "CONVERSION PEPSIS : change mersi3 en ssie7 dans les fichiers de /home/mersi" , BR, BR, _ );
	
	System( &w, "find /home/mersi/script \"mersi3\"" );
	p = 0;
	while( sgets( &s1, w, &p ) )
		{
		if ( dir_exists( s1 ) ) continue;
		if ( strpos( s1, ".sav", 0 ) > 0 )
			{
			unlink( s1 ); continue;
			}
		if ( strpos( s1, "/log/", 0 ) > 0 ) continue;
		// A faire confirmer par la TMA
		if ( strpos( s1, "/cache/", 0 ) > 0 ) continue;
		if ( file_exists( s1 ) )
			{
			file_load( &s2, s1 );
			if (( strpos( s2, "mersi3", 0 ) < 0 )
				&&( strpos( s2, "/ssie/", 0 ) < 0 ))
				continue;
			
			strdcpy( &cmd, "cp -f ", s1, " ", s1, ".sav", _ );
			System( &s, cmd );
			print( cmd, BR, s, BR, _ );
			print( "update ", s1, BR, _ );
			strd_replace( &s2, "/ssie/", "/mersi/" );
			strd_replace( &s2, "/mersi3", "/ssie7" );
			strd_replace( &s2, "mersi3 ", "ssie7 " );
			strd_replace( &s2, "mersi3;", "ssie7;" );
			strd_replace( &s2, "mersi3'", "ssie7'" );
			strd_replace( &s2, "mersi3'", "ssie7'" );

			file_save_str( s1, s2, -1 );
			}
		}
	}


int back_pepsis( T_WORK* t_work )
	{
	print( "--------------------------------------------------------------------------------", BR, _ );
	print( "CONVERSION PEPSIS : change ssie7 en mersi3 dans les fichiers de /home/mersi" , BR, BR, _ );
	
	System( &w, "find /home/mersi/script \"ssie7\"" );
	p = 0;
	while( sgets( &s1, w, &p ) )
		{
		if ( dir_exists( s1 ) ) continue;
		if ( strpos( s1, ".sav", 0 ) > 0 )
			{
			unlink( s1 ); continue;
			}
		if ( strpos( s1, "/log/", 0 ) > 0 ) continue;
		// A faire confirmer par la TMA
		if ( strpos( s1, "/cache/", 0 ) > 0 ) continue;
		if ( file_exists( s1 ) )
			{
			file_load( &s2, s1 );
			if ( strpos( s2, "ssie", 0 ) < 0 ) continue;
			
			strdcpy( &cmd, "cp -f ", s1, " ", s1, ".sav", _ );
			System( &s, cmd );
			print( cmd, BR, s, BR, _ );
			print( "update ", s1, BR, _ );
			strd_replace( &s2, "/ssie7", "/mersi3" );
			strd_replace( &s2, "ssie7 ", "mersi3 " );
			strd_replace( &s2, "ssie7;", "mersi3;" );
			strd_replace( &s2, "ssie7'", "mersi3'" );
			strd_replace( &s2, "/ssie/", "/mersi/" );
			file_save_str( s1, s2, -1 );
			}
		}
	}


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


int crea_rep( char *nom_rep )
	{
	if ( stat( nom_rep, &buffer ) != 0 )
	if ( errno == ENOENT )
		{
		mkdir( nom_rep );
		if ( !dir_exists( nom_rep ) )
			{
			printf( "      ERREUR : Création répertoire '%s'" BR, nom_rep );
			return false;
			}
		}	
	return true;
	}


int creation_arbo_ssie( T_WORK *t_work )
	{
	strdcpy( &cmd, sudo_ssie7_prefix, "chown -Rf ssie:ssie /appli/ssie7", _ );
	System( &s, cmd );
//print( cmd, BR, s, BR, _ );
	
	if ( ( ! crea_rep( dirlog ) )
			|| ( ! crea_rep( "/appli/ssie7/conf" ) )
			|| ( ! crea_rep( "/appli/ssie7/conf/part" ) )
			|| ( ! crea_rep( "/appli/ssie7/conf/login" ) )
			|| ( ! crea_rep( "/appli/ssie7/conf/reveils" ) )
			|| ( ! crea_rep( "/appli/ssie7/conf/flux" ) )
			|| ( ! crea_rep( "/appli/ssie7/data" ) )
			|| ( ! crea_rep( "/appli/ssie7/data/wrk" ) )
//			|| ( ! crea_rep( "/appli/ssie7/data/wrk/tmp" ) )
//			|| ( ! crea_rep( "/appli/ssie7/data/wrk/psurv" ) )
//			|| ( ! crea_rep( "/appli/ssie7/data/wrk/rflux" ) )
//			|| ( ! crea_rep( "/appli/ssie7/data/wrk/wdate" ) )
//			|| ( ! crea_rep( "/appli/ssie7/data/wrk/wfile" ) )
//			|| ( ! crea_rep( "/appli/ssie7/data/wrk/wrecv" ) )
			|| ( ! crea_rep( "/appli/ssie7/data/export" ) )
			|| ( ! crea_rep( "/appli/ssie7/data/import" ) )
			|| ( ! crea_rep( "/appli/ssie7/data/report" ) )
			)
		return false;
	
	return true;
	}


char ** get_flux_line( char *_str )
{
     char **str_list=null, *str=null;
     int list_ind=0, str_ind=0;
     int ind=0, beg_ind=0, end_ind=0;
     int pos=0;
     int continue_parse=0, desactivate_char=0;
     
     str_list = (char**)calloc( 10, sizeof(char*) );
     str = calloc(strlen(_str), sizeof(char));

     if ( _str[0] == '[' )
     {
        ind++;
        beg_ind=ind;
     }
     if ( ( ( pos = strpos( _str, "]", 0 ) ) >= 0 ) && _str[pos+1] == '\0' )
        _str[pos] = '\0';
     end_ind=-1;
     while ( true )
     {
            if ( ! continue_parse )
            {
                if ( _str[ind] == ',' || _str[ind] == '\0')
                {
                     if ( end_ind == -1 )
                        end_ind=ind;
		             memcpy( str, &(_str[beg_ind]), end_ind-beg_ind );
                     str[end_ind-beg_ind]='\0';
                     strdcpy(&str_list[list_ind], str, _ );
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
     
     free(str);
     
     return str_list;
}


bool check_file( char *_str, T_WORK *t_work )
{
    strdcpy( &cmd, sudo_ssie7_prefix, " ls ", _str, _ );
    System( &s, cmd );

//print( cmd, BR, s, BR, _ );
    
    if ( ( strpos( s, "not found", 0 ) >= 0 ) || ( strpos( s, "No such file or directory", 0 ) >= 0 ) )
        return false;

//print( "check_file true" BR, _ );

    return true;
}


//------------------------------------------------------------------------------------------
int histo_version( T_WORK *t_work )
//------------------------------------------------------------------------------------------
	{
    strdcpy( &curr_version_fname, dirbin, DIR_CHAR "current_version", _ );
    strdcpy( &curr_binaries_dname, dirbin, DIR_CHAR "current_binaries", _ );
    strdcpy( &prev_version_fname, dirbin, DIR_CHAR "previous_version", _ );
    strdcpy( &prev_binaries_dname, dirbin, DIR_CHAR "previous_binaries", _ );
    strdcpy( &prev_install_version_fname, dirbin, DIR_CHAR "previous_install_version", _ );
    strdcpy( &prev_install_config_dname, dirbin, DIR_CHAR "previous_install_config", _ );

// Compare la version actuelle et la version téléchargée.
    if ( ! file_exists( curr_version_fname ) )
    {
//print( "curr_version_fname n'existe pas.", BR BR, _ ); 
        strdcpy( &w, "/tmp/ssie7.", itoa(getpid()), _ );
        crea_rep( w );
        strdcpy( &cmd, "cp -R ", dirbin, "/* ", w, _ );
    	System( &s, cmd );
        crea_rep( curr_binaries_dname );
        strdcpy( &cmd, "mv ", w, "/* ", curr_binaries_dname, _ );
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

	}






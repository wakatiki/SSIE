#include "ssie7.h"

extern char *flux_etape;
extern int	etape_no, etape_log_id;

#define bExecButton		b4

//------------------------------------------------------------------------------
int ssie7_script_exec( T_WORK* t_work, bool bExecOnError )

// EXECUTE UNE ETAPE DE FLUX DE TYPE SCRIPT (Exécution d'un script BAT ou KSH)
//------------------------------------------------------------------------------
	{
	// v doit contenir le script à exécuter
	// id doit contenir la log conteneur
	// vn doit contenir le nom du bouton à exécuter si bExecOnError = 2

tr( "ssie7_script_exec1" );
logw("sni: ssie7_script_exec",_);
	
	if ( bExecOnError == 2 ) { bExecButton = true; bExecOnError = false; }
	else bExecButton = false;
	
	//--------------------------------------------------------------------------
	// PREPARATION DU SCRIPT
	//--------------------------------------------------------------------------

tr( "ssie7_script_exec2" );

	hlogr( hLog, etape_log_id, &s, null ); // LECTURE DU STATUS

tr( "ssie7_script_exec3" );


	strdcpy( &s3, "etape_", flux_etape, "_retry_nb", _ );
	v3 = var_get( hFlux, s3 );

	#ifdef OS_UNIX

	v2 = trim( var_get( hFlux, "app_login" ) );
	if (( v2 )&&( (p=strpos( v2, ":", 0 )) >= 0 ))
		v2 = strdncpy( &u6, v2, p, _ );
		
	if (( v2 )&&( v2[0] )&&( strcmp( v2, ssie_user ) ) )
		strdcpy( &s3, "sudo ", v2, BR, _ );
	else
		strdcpy( &s3, "", _ );
	
	if ( strpos( v, "#!o", 0 ) != 0 )
		strdcat( &s3, "#!/bin/ksh", _ );
	else
		strdcat( &s3, "#!o", _ );
	 
	strdcat( &s3,
			BR "ETAPE_NO=", itoa( etape_no ),
			BR "RETRY_NB=", v3,
			BR "STATUS=\"", s, "\"",
			BR "status=\"", s, "\"",
			BR, _ );
	
	v2 = var_get( hFlux, "var_list" );
	if ( v2 )
		{
		if ( ts ) free( ts );
		ts = str_explode( v2, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			strdcpy( &s2, var_get( hFlux, strdcpy( &s, "_", ts[i], _ ) ), _ );
			strd_replace( &s2, "`", "'" );
			strd_C_quote( &s2 );
			
			strdcat( &s3, upper(ts[i]), "=", s2, BR, _ );
			strdcat( &s3, lower(ts[i]), "=", s2, BR, _ );
			}
		}
	

	v2 = var_get( hFlux, "pki_list" );
	if ( v2 )
		{
		if ( ts ) free( ts );
		ts = str_explode( v2, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			strdcpy( &s2, var_get( hFlux, strdcpy( &s, "pki_", ts[i], _ ) ), _ );
			strdcpy( &s, upper(ts[i]), "=", s2, _ );
			putenv( s );
			}
		}
	
	if ( strpos( v, "#!o", 0 ) != 0 )
		strdcat( &s3,
				BR, _ );

	#else

	strdcpy( &s3, "@echo off" 
			BR "set ETAPE_NO=", flux_etape,
			BR "set RETRY_NB=", v3,
			BR "set STATUS=\"", s, "\"",
			BR, _ );

	v2 = var_get( hFlux, "var_list" );
	if ( v2 )
		{
		if ( ts ) free( ts );
		ts = str_explode( v2, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			
			strdcpy( &s2, var_get( hFlux, strdcpy( &s, "_", ts[i], _ ) ), _ );
			strd_replace( &s2, "`", "'" );
			
			strdcat( &s3, "set ", upper(ts[i]), "=", s2, BR, _ );
			}
		}

	v2 = var_get( hFlux, "pki_list" );
	if ( v2 )
		{
		if ( ts ) free( ts );
		ts = str_explode( v2, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			strdcpy( &s2, var_get( hFlux, strdcpy( &s, "pki_", ts[i], _ ) ), _ );
			strdcpy( &s, upper(ts[i]), "=", s2, _ );
			putenv( s );
			}
		}
	

	#endif

	
	if ( !bExecButton )
		{
//		h = 0;
		p0 = 0;
		n = 0; // Nombre de ssie_button
		
		while( sgets( &s, v, &p0 ) )
			{
			if ( (p=stripos( s, "var ", 0 )) >= 0 )
				{
				if ( trim(substr( s, 0, p ))[0] == 0 )
					{
					p1 = strpos( s, "=", 0 );
					if ( p1 < 0 ) continue;
					
					vn = trim(substr( s, 4, p1-4 ));
					if ( vn[0] == '$' ) vn = &(vn[1]);
					v1 = trim(substr( s, p1+1, -1 ));
					
					#ifdef OS_UNIX
					strdcat( &s3, vn, "=", str_C_quote( v1 ), BR, _ );
					strdcat( &s3, "echo \"var ", vn, "=$", vn, "\"" BR, _ );
					#else //WINDOWS
					strdcat( &s3, "set ", vn, "=", v1, BR, _ );
					strdcat( &s3, "echo var ", vn, "=%", vn, "%" BR, _ );
					#endif
					continue;
					}
				}
			
			if ( (p=stripos( s, "pki ", 0 )) == 0 )
				continue;
				
			if ( (p=stripos( s, "ssie_button ", 0 )) >= 0 )
				{
				if ( trim(substr( s, 0, p ))[0] == 0 )
					{
					// lecture du nom du bouton
					p += 12;
					for( ; ( ( s[p] )&&( s[p] == ' ' ) ) ; p++ );
					p1 = strpos( s, " ", p );
					if ( p1 > 0 )
						#ifdef OS_UNIX
						strdcat( &s3, "echo \"SSIE_BUTTON_SET ", flux_etape, ":", substr( s, p, p1-p ), "\"" BR, _ );
						#else //WINDOWS
						strdcat( &s3, "echo SSIE_BUTTON_SET ", flux_etape, ":", substr( s, p, p1-p ), BR, _ );
						#endif
					
					n++;	
					continue;
					}
				}
			
			if ( (p=stripos( s, "end_ssie_button", 0 )) >= 0 )
				{
				if ( trim(substr( s, 0, p ))[0] == 0 )
					{
					n--;
					continue;
					}
				}
	
			if ( n ) continue;
			
			if ( (p=stripos( s, "ssie_button_set", 0 )) >= 0 )
				{
				if ( trim(substr( s, 0, p ))[0] == 0 )
					{
					#ifdef OS_UNIX
					strdcat( &s3, "echo \"", trim(upper(s)), "\"" BR, _ );
					#else //WINDOWS
					strdcat( &s3, "echo ", trim(upper(s)), BR, _ );
					#endif
					continue;
					}
				}
	
			if ( (p=stripos( s, "ssie_button_delete", 0 )) >= 0 )
				{
				if ( trim(substr( s, 0, p ))[0] == 0 )
					{
					#ifdef OS_UNIX
					strdcat( &s3, "echo \"", trim(upper(s)), "\"" BR, _ );
					#else //WINDOWS
					strdcat( &s3, "echo ", trim(upper(s)), BR, _ );
					#endif
					continue;
					}
				}
	
			strdcat( &s3, s, BR, _ );
			}
		}
	else
		{
		// EXECUTE UN BOUTON
		
//		h = 0;
		p0 = 0;
		strdcpy( &s4, "ssie_button ", vn, _ );
		
		while( sgets( &s, v, &p0 ) )
			{
			// RECHERCHE LE BOUTON SOUHAITE
			if ( (p=stripos( s, s4, 0 )) < 0 )		continue;
			if ( trim(substr( s, 0, p ))[0] != 0 )	continue;
			
			// BOUTON TROUVE => LECTURE DU SCRIPT
			while( sgets( &s, v, &p0 ) )
				{
				if ( (p=stripos( s, "var ", 0 )) >= 0 )
					{
					if ( trim(substr( s, 0, p ))[0] == 0 )
						{
						p1 = strpos( s, "=", 0 );
						if ( p1 < 0 ) continue;
						
						vn = trim(substr( s, 4, p1-4 ));
						v1 = trim(substr( s, p1+1, -1 ));
						
						#ifdef OS_UNIX
						strdcat( &s3, vn, "=", str_C_quote( v1 ), BR, _ );
						strdcat( &s3, "echo \"var ", vn, "=$", vn, "\"" BR, _ );
						#else //WINDOWS
						strdcat( &s3, "set ", vn, "=", v1, BR, _ );
						strdcat( &s3, "echo var ", vn, "=%", vn, "%" BR, _ );
						#endif
						continue;
						}
					}
				
				if ( (p=stripos( s, "pki ", 0 )) == 0 )
					continue;
					
				if ( (p=stripos( s, "end_ssie_button", 0 )) >= 0 )
					break;
		
				if ( (p=stripos( s, "ssie_button_set", 0 )) >= 0 )
					{
					if ( trim(substr( s, 0, p ))[0] == 0 )
						{
						#ifdef OS_UNIX
						strdcat( &s3, "echo \"", trim(upper(s)), "\"" BR, _ );
						#else //WINDOWS
						strdcat( &s3, "echo ", trim(upper(s)), BR, _ );
						#endif
						continue;
						}
					}
		
				if ( (p=stripos( s, "ssie_button_delete", 0 )) >= 0 )
					{
					if ( trim(substr( s, 0, p ))[0] == 0 )
						{
						#ifdef OS_UNIX
						strdcat( &s3, "echo \"", trim(upper(s)), "\"" BR, _ );
						#else //WINDOWS
						strdcat( &s3, "echo ", trim(upper(s)), BR, _ );
						#endif
						continue;
						}
					}
		
				strdcat( &s3, s, BR, _ );
				}
			}
		}

	

	//--------------------------------------------------------------------------
	// EXECUTION DU SCRIPT
	//--------------------------------------------------------------------------

	p1 = 5;
	if (( stripos( s3, "#SSIE_NO_PROCESS_TIMEOUT", 0 ) >= 0 )&&( argc >= 4 ))
		p1 = 0;

	if (( (p=stripos( s3, "#SSIE_PROCESS_TIMEOUT=", 0 )) >= 0 )&&( argc >= 4 ))
		p1 = atoi(&(s3[p+22]));

	// Allonge le timeout du robot flux
	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", argv[3], ".timeout", _ );
	file_save_str( fname, itoa(p1+((p1)?1:0)), -1 );
	

	logw("sni: timeout=",itoa(p1),"/",flux_id,_);
tr( "ssie7_script_exec3" );

	if ( p1 != 5 )
		hlogc( hLog, id, "", "timeout=", itoa(p1), _ );

tr( "ssie7_script_exec3a" );
	
	#ifdef OS_UNIX
	v2 = trim( var_get( hFlux, "app_login" ) );
	if ( strpos( v2, ssie_user, 0 ) != 0 )
		{ // Si sudo => fichiers temporaire dans /tmp
		v = s4; s4 = dirtmp; dirtmp = v;
		strdcpy( &dirtmp, "/tmp", _ );
		}
	#endif
	
	b = script_exec( s3, 0, p1 Minutes, &s2 );

	#ifdef OS_UNIX
	if ( strpos( v2, ssie_user, 0 ) != 0 )
		{
		v = s4; s4 = dirtmp; dirtmp = v;
		}
	#endif

	// Remet le timeout normal du robot flux
	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", argv[3], ".pid", _ );
	file_save_str( fname, itoa(getpid()), -1 ); // Pour rallonge du timeout
	strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", argv[3], ".timeout", _ );
	unlink( fname );

	if ( !b )
		{ // Timeout
tr( "ssie7_script_exec4" );

		hlogc( hLog, id, "", BR, s3, _ );

tr( "ssie7_script_exec5" );

		hlogc( hLog, id, "", BR, s2, _ );

tr( "ssie7_script_exec6" );
		
		strdcpy( &s, "ERREUR : Script TIMEOUT", _ );
		goto script_fin;
		}
		
	//--------------------------------------------------------------------------
	// RECUPERATION DES INFORMATIONS
	//--------------------------------------------------------------------------


	b = true;
	p1 = -1;
	while( ( p1 = stripos( s2, "NO_LOG", p1+1 ) ) >= 0 )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		b = false;
		break;
		}
	
	
	if ( b )
		{
tr( "ssie7_script_exec7" );
		hlogc( hLog, id, "", BR, s3, _ );
tr( "ssie7_script_exec8" );
		hlogc( hLog, id, "", BR, s2, _ );
tr( "ssie7_script_exec9" );
		}
	
	
	
	for( p1 = 0 ;( p1 = stripos( s2, "STOP", p1 ) ) >= 0 ; p1++ )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		p2 = strpos( s2, "\n", p1+4 );
		if ( p2 >= 0 ) v = substr( s2, p1, p2-p1 );
		else v = substr( s2, p1, -1 );
		trim( v );
		if ( stricmp( v, "stop" ) ) continue;
		var_set( hFlux, "etape_next", "stop" );
		break;
		}

	
	for( p1 = 0 ;( p1 = stripos( s2, "NORESTART", p1 ) ) >= 0 ; p1++ )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		p2 = strpos( s2, "\n", p1+9 );
		if ( p2 >= 0 ) v = substr( s2, p1, p2-p1 );
		else v = substr( s2, p1, -1 );
		trim( v );
		if ( stricmp( v, "norestart" ) ) continue;
		var_set( hFlux, "flux_do_restart", "false" );
		break;
		}
	

	for( p1 = 0 ;( p1 = stripos( s2, "CONTINU", p1 ) ) >= 0 ; p1++ )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		p2 = strpos( s2, "\n", p1+7 );
		if ( p2 >= 0 ) v = substr( s2, p1, p2-p1 );
		else v = substr( s2, p1, -1 );
		trim( v );
		var_set( hFlux, "etape_next", "continue" );
		break;
		}


	for( p1 = 0 ;( p1 = stripos( s2, "VAR ", p1 ) ) >= 0 ; p1++ )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		p2 = strpos( s2, "\n", p1+10 );
		if ( p2 >= 0 ) v = substr( s2, p1+4, p2-p1-4 );
		else v = substr( s2, p1+4, -1 );
		trim( v );

		if ( (p3 = strpos( v, "=", 0 )) < 0 ) continue;

		var_set( hFlux, strdcpy( &s1, "_", v1=trim(substr(v, 0, p3)), _ ), unquote(trim(&(v[p3+1]))) );

		v = var_get( hFlux, "var_list" );
		if ( !v )
			{
			var_set( hFlux, "var_list", strdcpy( &s1, ",", v1, ",", _ ) );
			}
        else
            {
			if ( stripos( v, strdcpy( &s1, ",", v1, _ ), 0 ) < 0 )
               	var_set( hFlux, "var_list", strdcpy( &s1, ",", v1, v, _ ) );
            else
               	{
				if ( stripos( v, strdcpy( &s1, ",", v1, ",", _ ), 0 ) < 0 )
	               	var_set( hFlux, "var_list", strdcpy( &s1, v, v1, ",", _ ) );
				}
            }
		}


	// TRI LA LISTE PAR ORDRE DU PLUS LONG AU PLUS COURT
	
	var_list_prepare( hFlux );

	
	// LECTURE ETAPE SUIVANTE
	for( p1 = 0 ;( p1 = stripos( s2, "ETAPE=", p1 ) ) >= 0 ; p1++ )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		p2 = strpos( s2, "\n", p1+6 );
		if ( p2 >= 0 ) v = substr( s2, p1+6, p2-p1-6 );
		else v = substr( s2, p1, -1 );
		trim( v );
		var_set( hFlux, "etape_next", v );
		break;
		}

	p1 = -1;
	while( ( p1 = stripos( s2, "ATTEND ", p1+1 ) ) >= 0 )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		
		n = atoi( &(s2[p1+7]) );
		var_save( hFlux, strdcpy( &fname, dirflux, "/", flux_id, ".dat", _ ) );
		sleep(n);
		}
	
	p1 = -1;
	while( ( p1 = stripos( s2, "RECOMMENCE ", p1+1 ) ) >= 0 )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		
		n = atoi( &(s2[p1+11]) );

		strdcpy( &s3, "etape_", flux_etape, "_retry_nb", _ );
		v = var_get( hFlux, s3 );
		if ( ( v )&&( atoi(v) >= n ) )
			{
			var_set( hFlux, "etape_next", "" );
			strdcat( &s2, BR "STATUS=ERREUR : Trop de retentatives" BR, _ );
			break;
			}
		
		n1 = atoi(v)+1;
		var_set( hFlux, s3, itoa( n1 ) );
		strdcpy( &s, "retry", itoa(n), _ );
		var_set( hFlux, "etape_next", s );
		}
	
	
	// LECTURE SSIE_BUTTON_SET
	for( p1 = 0 ; ( p1 = stripos( s2, "SSIE_BUTTON_SET ", p1 ) ) >= 0 ; p1++ )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		p2 = strpos( s2, "\n", p1+16 );
		if ( p2 >= 0 ) v = substr( s2, p1+16, p2-p1-16 );
		else v = substr( s2, p1, -1 );
		trim( v );

		strdcpy( &s1, ",", v, ",", _ );
		v1 = var_get( hFlux, "ssie_buttons" );
		if ( ( v1 )&&( stripos( v1, s1, 0 ) >= 0 ) ) continue;
		
		if (( atoi(v) == atoi(flux_etape) )
			&&( !var_get( hFlux, v ) ))
			{
			strdcpy( &u5, var_get( hFlux,
					strdcpy( &s, "etape_", fluxname, "_", flux_etape, "_conf_fname", _ ) ),
							"::", itoa(etape_log_id), _ );
			var_set( hFlux, v, u5 );

tr( "ssie7_script_exec10" );
hlogc( hLog, id, "", v, " = ", u5, _ );
			}
		
tr( "ssie7_script_exec11" );

		if ( ( v1 )&&( v1[0] ) )
			strdcpy( &s4, v1, &(s1[1]), _ );
		else
			strdcpy( &s4, v1, s1, _ );
		
		var_set( hFlux, "ssie_buttons", s4 );
hlogc( hLog, id, "", "ssie_buttons = ", s4, _ );
tr( "ssie7_script_exec12" );

		continue;
		}


	// LECTURE SSIE_BUTTON_SET
	for( p1 = 0 ; ( p1 = stripos( s2, "SSIE_BUTTON_DELETE ", p1 ) ) >= 0 ; p1++ )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		p2 = strpos( s2, "\n", p1+19 );
		if ( p2 >= 0 ) v = substr( s2, p1+19, p2-p1-19 );
		else v = substr( s2, p1, -1 );
		trim( v );

		strdcpy( &s1, ",", v, ",", _ );
		v1 = var_get( hFlux, "ssie_buttons" );
		if ( ( !v1 )||( !(v1[0]) )||( stripos( v1, s1, 0 ) < 0 ) ) continue;
		
		strdcpy( &s4, v1, _ );
		strd_ireplace( &s4, s1, "," );
		var_set( hFlux, "ssie_buttons", s4 );
tr( "ssie7_script_exec13" );
hlogc( hLog, id, "", "delete : ssie_buttons = ", s4, _ );
tr( "ssie7_script_exec14" );

		continue;
		}

tr( "ssie7_script_exec15" );

	hlogr( hLog, etape_log_id, &s, null ); // LECTURE DU STATUS
tr( "ssie7_script_exec16" );
	
	for( p1 = 0 ;( p1 = stripos( s2, "STATUS=", p1 ) ) >= 0 ; p1++ )
		{
		if ( ( p1 )&&( s2[p1-1] != '\n' )&&( s2[p1-1] != '\r' ) ) continue;
		p2 = strpos( s2, "\n", p1+7 );
		if ( p2 >= 0 ) v = substr( s2, p1+7, p2-p1-7 );
		else v = substr( s2, p1, -1 );
		trim( v );
		strdcpy( &s, v, _ );
		break;
		}

	script_fin:
		
	if ( ( stripos( s, "ERREUR", 0 ) < 0 )&&( stripos( s, "ERROR", 0 ) < 0 )
		&&( stripos( s, "ALERT", 0 ) < 0 )&&( stripos( s, "OK", 0 ) < 0 ) )
		strdcpy( &s, "OK", _ );

tr2( "ssie7_script_exec17:", s );
	hlog_raise_status( hLog, id, s, _ );

tr( "ssie7_script_exec18" );

	if ( !bExecButton )
		hlogu_status( hLog, 0, "En cours : Etape ", flux_etape, " Script exec : ", s, _ );
	else
		{
		hlogu_status( hLog, 0, s, _ );
		print( s, _ );
		}

tr( "ssie7_script_exec19" );


	if ( ( stripos( s, "ERREUR", 0 ) >= 0 )||( stripos( s, "ERROR", 0 ) >= 0 )
		||( stripos( s, "ALERT", 0 ) >= 0 ) )
		{
		error_no = 1;
		if ( bExecOnError )
			{
			v1 = var_get( hFlux, "etape_next" );
			if ( ( !v1 )||( !(v1[0]) ) )
				{
//				hlogc( hLog, id, "", BR, s3, _ );
//				hlogc( hLog, id, "", BR, s2, _ );
				on_error_execute( t_work );
				return true;
				}
			}
		}

tr( "ssie7_script_exec20" );
	
	return true;
	}



//------------------------------------------------------------------------------
int script_execute( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	v = var_get( hEtape, "etape_script" );
	id = etape_log_id;
	
	var_set( hFlux, "etape_next", "" );
	return ssie7_script_exec( t_work, true );
	}
	


//------------------------------------------------------------------------------
int button_execute( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	hLog = hlog_init( dirflux, flux_id );
	var_getn( hReq, 3, &vn );
	var_getn( hReq, 4, &flux_etape );
	etape_no = atoi( flux_etape );
	strdcpy( &s, flux_etape, ":", vn, _ );
	v1 = var_get( hFlux, s );
	if ( !v1 )
		{
		print( "ERREUR:Bouton inconnu", _ );
		return ERROR;
		}

	p1 = strpos( v1, "::", 0 );
	if ( p1 < 0 )
		{
		print( "ERREUR:Bouton inconnu (2)", _ );
		return ERROR;
		}
	
	// CHARGE LE SCRIPT
//	v1[p1] = 0;
	strdncpy( &fname, v1, p1, _ );
	var_free( hEtape );
	hEtape = var_load( fname );
	v = var_get( hEtape, "etape_script" );
	if ( !v ) v = var_get( hEtape, "age_script" );
	if ( !v ) v = var_get( hEtape, "agr_script" );
	if ( !v )
		{
		print( "ERREUR:Bouton inconnu (3)", _ );
		return ERROR;
		}
	
	id = etape_log_id = hlogc( hLog, 0, "", user, ":EXEC BUTTON : ", vn, " (étape ", flux_etape, ")", _ );
	
	ssie7_script_exec( t_work, 2 );
	}
	





















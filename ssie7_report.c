#include "ssie7.h"


//------------------------------------------------------------------------------
int report_compute( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	char *dirrep = null, *grpname = null;
	handle bi = bigint_new();

	if ( process_count( "report_compute" ) > 1 ) return OK;

tr( "report_compute1" );

	strdcpy( &dirrep, dirdat, DIR_CHAR, "report", _ );
	mkdir( dirrep );
	
	strdcpy( &fname, dirflux, DIR_CHAR "log.id", _ );
	n2 = read_uid( fname );
	if ( n2 == ERROR ) return ERROR;
	
	v = var_get( hReq, "reset" );
	b = (v) ? true : false;
	if ( b )
		#ifdef OS_UNIX
		system( strdcpy( &s, "rm -f ", dirrep, DIR_CHAR "*", _ ) );
		#else
		system( strdcpy( &s, "del /Q ", dirrep, DIR_CHAR "*", _ ) );
		#endif
		
	n1 = 0;
	h = h1 = 0;
	while(1)
		{
		// VERIFIE QU'IL N'Y A PAS TROP DE PROCESS EN ROUTE
//		if ( ( ! ( n1%10) )&&( !b ) )
//			if ( process_count( "mersi3" ) > 20 ) return OK;
		
tr( "report_compute2" );
		n1++;
		if ( n1 > n2 ) return OK;

tr( "report_compute2a" );

		strdcpy( &flux_id, itoa(n1), _ );
		
		// VERIFIE QUE LE FLUX N'EST PAS EN COURS D'EXECUTION

//		strdcpy( &fname, dirrflux, DIR_CHAR, flux_id, _ );
		strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".run", _ );
		if ( file_exists( fname ) ) continue;

tr( "report_compute2b" );

		// CHARGE LE FLUX A CALCULER
		
		strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".dat", _ );
		if ( ! file_exists( fname ) ) continue;

tr( "report_compute2c" );
		if ( h ) var_free( h );
		h = var_load( fname );
		
		// VERIFIE QUE LE FLUX N'A PAS DEJA ETE PRIS EN COMPTE
		
		if ( ( !b )&&( var_get( h, "report_ok" ) ) ) continue;

		
		// CALCUL
tr2( "report_compute3:", flux_id );
		
		print( "Reporting pour le flux ", flux_id, BR, _ );
		
		strdcpy( &appname, var_get( h, "app_name" ), _ );
		v = var_get( h, "flux_name" );
		if ( (p=strpos( v, "@", 0 )) < 0 )
			{
			strdcpy( &grpname, "<sans>", _ );
			strdcpy( &fluxname, v, _ );
			}
		else
			{
			strdcpy( &grpname, substr( v, 0, p ), _ );
			strdcpy( &fluxname, &(v[p+1]), _ );
			}

tr( "report_compute4" );
			
		hLog = hlog_init( dirflux, flux_id );
		hlogr( hLog, 0, &s4, null );
//		free( (char*)hLog );
		hlog_free( hLog );
		v = s4;

tr( "report_compute5" );
			
		print( "app_name = ", appname, BR,
			"group_name = ", grpname, BR,
			"flux_name = ", fluxname, BR,
			"date = ", v1 = itod(file_date(fname), "aaaa/mm/jj hh:mm:ss"), BR,
			"status = ", v, BR,
			_ );
			
		if ( appname[0] == 0 ) continue;
		if ( fluxname[0] == 0 ) continue;
		if ( !v1 ) continue;
		if ( !v ) continue;
		if ( ( stripos( v, "Arrêt demandé par", 0 ) >= 0 )
				&&( !var_get( h, "ask_noreport" ) )
				&&( !var_get( h, "ask_report_ok" ) )
				&&( !var_get( h, "ask_report_error" ) )
				)
			{
			var_set( h, "report_ok", "ok" );
			var_set( h, "noreport", "true" );
			var_set( h, "report_version", "2" );
			strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".dat", _ );
			var_save( h, fname );
			continue;
			}
		
		// ECRIT LA LISTE DES FICHIERS PAR JOUR
		
		
		// CALCUL LES CUMUL
		
		e = 0;
		while(1)
			{
			e++;
			if ( e == 1 )
				strdcpy( &fname, dirrep, DIR_CHAR "J_FLX_", substr( v1, 0, 4 ), substr( v1, 5, 2 ),
					substr( v1, 8, 2 ), ".[", appname, "][", grpname, "][", fluxname, "].dat", _ );
			else
			if ( e == 2 )
				strdcpy( &fname, dirrep, DIR_CHAR "M_FLX_", substr( v1, 0, 4 ), substr( v1, 5, 2 ),
					".[", appname, "][", grpname, "][", fluxname, "].dat", _ );
			else
			if ( e == 3 )
				strdcpy( &fname, dirrep, DIR_CHAR "A_FLX_", substr( v1, 0, 4 ),
					".[", appname, "][", grpname, "][", fluxname, "].dat", _ );
			else
			if ( e == 4 )
				strdcpy( &fname, dirrep, DIR_CHAR "J_GRP_", substr( v1, 0, 4 ), substr( v1, 5, 2 ),
					substr( v1, 8, 2 ), ".[", appname, "][", grpname, "].dat", _ );
			else
			if ( e == 5 )
				strdcpy( &fname, dirrep, DIR_CHAR "M_GRP_", substr( v1, 0, 4 ), substr( v1, 5, 2 ),
					".[", appname, "][", grpname, "].dat", _ );
			else
			if ( e == 6 )
				strdcpy( &fname, dirrep, DIR_CHAR "A_GRP_", substr( v1, 0, 4 ),
					".[", appname, "][", grpname, "].dat", _ );
			else
			if ( e == 7 )
				strdcpy( &fname, dirrep, DIR_CHAR "J_APP_", substr( v1, 0, 4 ), substr( v1, 5, 2 ),
					substr( v1, 8, 2 ), ".[", appname, "].dat", _ );
			else
			if ( e == 8 )
				strdcpy( &fname, dirrep, DIR_CHAR "M_APP_", substr( v1, 0, 4 ), substr( v1, 5, 2 ),
					".[", appname, "].dat", _ );
			else
			if ( e == 9 )
				strdcpy( &fname, dirrep, DIR_CHAR "A_APP_", substr( v1, 0, 4 ),
					".[", appname, "].dat", _ );
			else
				break;
					
			print( "calcul du fichier ", fname, BR, _ );
			
			if ( h1 ) var_free( h1 );
			h1 = var_load( fname );
			
			if ( v = var_get( h, "ask_noreport" ) )
				{
				b1 = false;
				if ( v = var_get( h, "report_error" ) )
					{
					v = var_get( h1, "error_nb" );
					if ( !v ) n = 0;
					else n = atoi( v ) - 1;
					var_set( h1, "error_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					b1 = true;
					}
				else
				if ( v = var_get( h, "report_alert" ) )
					{
					v = var_get( h1, "alert_nb" );
					if ( !v ) n = 0;
					else n = atoi( v ) - 1;
					var_set( h1, "alert_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					b1 = true;
					}
				else
				if ( v = var_get( h, "report_set_ok" ) )
					{
					v = var_get( h1, "ok_nb" );
					if ( !v ) n = 0;
					else n = atoi( v ) - 1;
					var_set( h1, "ok_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					b1 = true;
					}
				else
				if ( v = var_get( h, "noreport" ) )
					{
					b1 = true;
					}

				if ( b1 )
					{
					var_save( h1, fname );
					continue;
					}
				else
					var_delete( h, "ask_noreport" );
				}
			else
			if ( v = var_get( h, "ask_report_error" ) )
				{
				b1 = false;
				if ( v = var_get( h, "report_error" ) )
					{
					v = var_get( h1, "error_nb" );
					if ( !v ) n = 0;
					else n = atoi( v ) - 1;
					var_set( h1, "error_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					b1 = true;
					}
				else
				if ( v = var_get( h, "report_alert" ) )
					{
					v = var_get( h1, "alert_nb" );
					if ( !v ) n = 0;
					else n = atoi( v ) - 1;
					var_set( h1, "alert_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					b1 = true;
					}
				else
				if ( v = var_get( h, "report_set_ok" ) )
					{
					v = var_get( h1, "ok_nb" );
					if ( !v ) n = 0;
					else n = atoi( v ) - 1;
					var_set( h1, "ok_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					b1 = true;
					}
				else
				if ( v = var_get( h, "noreport" ) )
					{
					b1 = true;
					}

				if ( b1 )
					{
					v = var_get( h1, "error_nb" );
					if ( !v ) n = 1;
					else n = atoi( v ) + 1;
					var_set( h1, "error_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					
					var_save( h1, fname );
					
					continue;
					}
				else
					var_delete( h, "ask_report_error" );
				}
			else
			if ( v = var_get( h, "ask_report_ok" ) )
				{
				b1 = false;
				if ( v = var_get( h, "report_error" ) )
					{
					v = var_get( h1, "error_nb" );
					if ( !v ) n = 0;
					else n = atoi( v ) - 1;
					var_set( h1, "error_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					b1 = true;
					}
				else
				if ( v = var_get( h, "report_alert" ) )
					{
					v = var_get( h1, "alert_nb" );
					if ( !v ) n = 0;
					else n = atoi( v ) - 1;
					var_set( h1, "alert_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					b1 = true;
					}
				else
				if ( v = var_get( h, "report_set_ok" ) )
					{
					v = var_get( h1, "ok_nb" );
					if ( !v ) n = 0;
					else n = atoi( v ) - 1;
					var_set( h1, "ok_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					b1 = true;
					}
				else
				if ( v = var_get( h, "noreport" ) )
					{
					b1 = true;
					}

				if ( b1 )
					{
					v = var_get( h1, "ok_nb" );
					if ( !v ) n = 1;
					else n = atoi( v ) + 1;
					var_set( h1, "ok_nb", itoa(n) );
					if ( ( e == 1 )&&( v = var_get( h, "comment" ) ) )
						var_set( h1, "comment", v );
					
					var_save( h1, fname );
					
					continue;
					}
				else
					var_delete( h, "ask_report_ok" );
				}
			
			
			v = s4;
			
			if ( ! strnicmp( v, "OK", -1 ) )
				{
				v = var_get( h1, "ok_nb" );
				if ( !v ) n = 1;
				else n = atoi( v ) + 1;
				var_set( h1, "ok_nb", itoa(n) );
				if ( e == 1 )	var_set( h, "report_set_ok", "true" );
				}
			else
			if ( ! strnicmp( v, "ALERT", -1 ) )
				{
				v = var_get( h1, "ok_nb" );
				if ( !v ) n = 1;
				else n = atoi( v ) + 1;
				var_set( h1, "ok_nb", itoa(n) );
				if ( e == 1 )	var_set( h, "report_set_ok", "true" );
				}
			else	
				{
				v = var_get( h1, "error_nb" );
				if ( !v ) n = 1;
				else n = atoi( v ) + 1;
				var_set( h1, "error_nb", itoa(n) );
				if ( e == 1 )	var_set( h, "report_error", "true" );
				}
			
			if ( v2 = var_get( h, "send_file_nb" ) )
				{
				v = var_get( h1, "emi_mft_nb" );
				n = ( v ) ? atoi( v ) : 0;
				n += atoi( v2 );
				var_set( h1, "emi_mft_nb", itoa(n) );

				if ( e == 1 )
					{
					if ( v = var_get( h1, "details_fichiers" ) )
						strdcpy( &s, v, _ );
					else
						strdcpy( &s, "", _ );
						
					v = s4;
					if ( ! strnicmp( v, "OK", -1 ) ) v = "OK";
					else v = "ERREUR";
					strdcat( &s, v1, " : MFT send : ",
							v, " : ", var_get( h, "send_file_size" ),
							" : (", v2, ") ", var_get( h, "send_file_slist" ), "<br>", _ );
					var_set( h1, "details_fichiers", s );
					}

				}
			
			if ( ( v2 = var_get( h, "send_file_size" ) )&&( atoi(v2) > 0 ) )
				{
				v = var_get( h1, "emi_mft_size" );
				if ( v )
					{
					bigint_from10( bi, v );
					bigint_add( bi, atoi(v2) );
					v2 = bigint_to10( bi );
					}
				var_set( h1, "emi_mft_size",v2 );
				}
			
			if ( v2 = var_get( h, "recv_file_nb" ) )
				{
				v = var_get( h1, "rec_mft_nb" );
				n = ( v ) ? atoi( v ) : 0;
				n += atoi( v2 );
				var_set( h1, "rec_mft_nb", itoa(n) );

				if ( e == 1 )
					{
					if ( v = var_get( h1, "details_fichiers" ) )
						strdcpy( &s, v, _ );
					else
						strdcpy( &s, "", _ );

					v = s4;
					if ( ! strnicmp( v, "OK", -1 ) ) v = "OK";
					else v = "ERREUR";
						
					strdcat( &s, v1, " : MFT recv : ",
							v, " : ", var_get( h, "recv_file_size" ),
							" : (", v2, ") ", var_get( h, "recv_file_slist" ), "<br>", _ );
					var_set( h1, "details_fichiers", s );
					}
				}
			
			if ( ( v2 = var_get( h, "recv_file_size" ) )&&( atoi(v2) > 0 ) )
				{
				v = var_get( h1, "rec_mft_size" );
				if ( v )
					{
					bigint_from10( bi, v );
					bigint_add( bi, atoi(v2) );
					v2 = bigint_to10( bi );
					}
				var_set( h1, "rec_mft_size", v2 );
				}
			
			if ( v2 = var_get( h, "ftp_get_nb" ) )
				{
				v = var_get( h1, "rec_ftp_nb" );
				if ( !v ) n = atoi(v2);
				else n = atoi( v ) + atoi(v2);
				var_set( h1, "rec_ftp_nb", itoa(n) );
				}
			
			if ( ( v2 = var_get( h, "ftp_get_size" ) )&&( atoi(v2) > 0 ) )
				{
				v = var_get( h1, "rec_ftp_size" );
				if ( v )
					{
					bigint_from10( bi, v );
					bigint_add( bi, atoi(v2) );
					v2 = bigint_to10( bi );
					}
				var_set( h1, "rec_ftp_size", v2 );
				}
			
			if ( v2 = var_get( h, "ftp_put_nb" ) )
				{
				v = var_get( h1, "emi_ftp_nb" );
				if ( !v ) n = atoi(v2);
				else n = atoi( v ) + atoi(v2);
				var_set( h1, "emi_ftp_nb", itoa(n) );
				}
			
			if ( ( v2 = var_get( h, "ftp_put_size" ) )&&( atoi(v2) > 0 ) )
				{
				v = var_get( h1, "emi_ftp_size" );
				if ( v )
					{
					bigint_from10( bi, v );
					bigint_add( bi, atoi(v2) );
					v2 = bigint_to10( bi );
					}
				var_set( h1, "emi_ftp_size", v2 );
				}
			
			var_save( h1, fname );
			}

		if ( var_get( h, "ask_report_ok" ) )
			{
			var_delete( h, "ask_report_ok" );
			var_delete( h, "noreport" );
			var_delete( h, "report_error" );
			var_delete( h, "report_alert" );
			var_set( h, "report_set_ok", "true" );
			}
			
		if ( var_get( h, "ask_report_error" ) )
			{
			var_delete( h, "ask_report_error" );
			var_delete( h, "noreport" );
			var_delete( h, "report_set_ok" );
			var_delete( h, "report_alert" );
			var_set( h, "report_error", "true" );
			}

		if ( var_get( h, "ask_noreport" ) )
			{
			var_delete( h, "ask_noreport" );
			var_delete( h, "report_error" );
			var_delete( h, "report_alert" );
			var_delete( h, "report_set_ok" );
			var_set( h, "noreport", "true" );
			}

		var_set( h, "report_ok", "ok" );
		var_set( h, "report_version", "2" );
		strdcpy( &fname, dirflux, DIR_CHAR, flux_id, ".dat", _ );
		var_save( h, fname );
		}

tr( "report_compute:fin" );
	}


//------------------------------------------------------------------------------
int cmd_report( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	char *dirrep = null, *grpname = null, *dt = null;

	strdcpy( &dirrep, dirdat, DIR_CHAR, "report", _ );
	
	if ( var_get( hReq, "of" ) ) b = true; else b = false;
	
	if ( var_get( hReq, "J" ) )
		strdcpy( &s, dirrep, DIR_CHAR "J_", _ );
	else
	if ( var_get( hReq, "A" ) )
		strdcpy( &s, dirrep, DIR_CHAR "A_", _ );
	else
		strdcpy( &s, dirrep, DIR_CHAR "M_", _ );
	
	if ( var_get( hReq, "APP" ) )
		strdcat( &s, "APP_*.dat", _ );
	else
	if ( ( var_get( hReq, "FLUX" ) )||( var_get( hReq, "FLX" ) ) )
		strdcat( &s, "FLX_*.dat", _ );
	else
		strdcat( &s, "GRP_*.dat", _ );
	
	vn = var_get( hReq, "dt" );
	
	ts = NULL;
	h = var_load( strdcpy( &fname, dirconf, DIR_CHAR "login" DIR_CHAR, user, ".dat", _ ) );
	if ( ( v = var_get( h, "reporting" ) )&&( v[0] ) )
		ts = str_explode( v, "," );

	tf = NULL;
	h = n = 0;
	while( ( tf = file_list( tf, s, "nodir adir" ) ) != NULL )
		{
		p1 = 6;
		p2 = strpos( tf->name, ".", p1 );
		if ( p2 < 0 ) continue;
		strdcpy( &dt, substr( tf->name, p1, p2-p1 ), _ );
		
		if (( vn )&&( strncmp( dt, vn, -1 ) )) continue;

		p1 = strpos( tf->name, "[", 0 );
		if ( p1 < 0 ) continue;
		p2 = strpos( tf->name, "]", p1 );
		if ( p2 < 0 ) continue;
		strdcpy( &appname, substr( tf->name, p1+1, p2-p1-1 ), _ );
		strdcpy( &s1, appname, _ );
		
		strdcpy( &grpname, "*", _ );
		p1 = strpos( tf->name, "[", p2 );
		if ( p1 >= 0 )
			{
			p2 = strpos( tf->name, "]", p1 );
			if ( p2 >= 0 )
				{
				strdcpy( &grpname, substr( tf->name, p1+1, p2-p1-1 ), _ );
				strdcat( &s1, ".", grpname, _ );
				}
			}

		strdcpy( &fluxname, "*", _ );
		p1 = strpos( tf->name, "[", p2 );
		if ( p1 >= 0 )
			{
			p2 = strpos( tf->name, "]", p1 );
			if ( p2 >= 0 )
				{
				strdcpy( &fluxname, substr( tf->name, p1+1, p2-p1-1 ), _ );
				strdcat( &s1, ".", fluxname, _ );
				}
			}
		
		if ( ts )
			{
			for( i = 0 ; ts[i] ; i++ )
				if ( !strnicmp( s1, ts[i], -1 ) ) break;
			if ( ! ts[i] ) continue;
			}
			
		if ( h ) var_free( h );
		strdcpy( &fname, dirrep, DIR_CHAR, tf->name, _ );
		h = var_load( fname );
		
		if ( ! b )
			{
			print( dt, " ", appname, " ", grpname, " ", fluxname, BR, "   ", _ );
			var_print( h, BR "   " );
			print( BR, _ );
			}
		else
			{
			if ( n ) print( ",", _ );
			strdcpy( &s1, var_get( h, "details_fichiers" ), _ );
			strdcpy( &s2, (char*)( (v=var_get( h, "comment" )) ? v : "" ), _ );
			strd_quote( &s2 );
			print( "['", dt,
				"',", quote(appname),
				",", quote(grpname),
				",", quote(fluxname),
				",", (char*)( (v=var_get( h, "ok_nb" )) ? v : "0" ),
				",", (char*)( (v=var_get( h, "error_nb" )) ? v : "0" ),
				",", (char*)( (v=var_get( h, "emi_mft_nb" )) ? v : "0" ),
				",", (char*)( (v=var_get( h, "emi_mft_size" )) ? v : "0" ),
				",", (char*)( (v=var_get( h, "rec_mft_nb" )) ? v : "0" ),
				",", (char*)( (v=var_get( h, "rec_mft_size" )) ? v : "0" ),
				",", (char*)( (v=var_get( h, "emi_ftp_nb" )) ? v : "0" ),
				",", (char*)( (v=var_get( h, "emi_ftp_size" )) ? v : "0" ),
				",", (char*)( (v=var_get( h, "rec_ftp_nb" )) ? v : "0" ),
				",", (char*)( (v=var_get( h, "rec_ftp_size" )) ? v : "0" ),
				",", quote( s1 ),
				",", s2,
				"]",
				_ );
			}
		n++;
		}
	
	}


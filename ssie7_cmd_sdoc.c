#include "ssie7.h"



//------------------------------------------------------------------------------
int sdoc_conf_load( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
/*
	if ( sdoc_dirdat_nb < 0 )
		{
		memset(&sdoc_dirdat, 0, sizeof sdoc_dirdat);
		memset(&sdoc_diridx, 0, sizeof sdoc_diridx);
		memset(&sdoc_dirkwd, 0, sizeof sdoc_dirkwd);
		}
		
	sdoc_dirdat_nb = 0;
	sdoc_diridx_nb = 0;
	sdoc_dirkwd_nb = 0;
	
	v = var_get( hSrvConf, "sdoc_dirdat" );
	if ( !v )
		{
		v = strdcpy( &s, dirdat, DIR_CHAR "sdoc" DIR_CHAR "data", _ );
		mkdir( v );
		}
	ts = str_explode( v, ";" );
	for( i = 0 ; ts[i] ; i++ )
		{
		if ( !(ts[i][0]) ) continue;
		ts2 = str_explode( ts[i], "::" );
		strdcpy( &(sdoc_dirdat[sdoc_dirdat_nb].name), ts2[0], _ );
		sdoc_dirdat[sdoc_dirdat_nb].maxnb = sdoc_dirdat[sdoc_dirdat_nb].maxsize = 0;
		if ( ts2[1] )
			{
			sdoc_dirdat[sdoc_dirdat_nb].maxnb = atoi( ts2[1] );
			if ( ts2[2] )
				{
				sdoc_dirdat[sdoc_dirdat_nb].maxsize = atoi( ts2[2] );
				}
			}
		sdoc_dirdat_nb++;
		}


for( i = 0 ; i < sdoc_dirdat_nb ; i++ )
{
print( itoa(i), ":", sdoc_dirdat[i].name,
		"::", itoa((long)sdoc_dirdat[i].maxnb),
		"::", itoa(sdoc_dirdat[i].maxsize), BR, _ );
}


	v = var_get( hSrvConf, "sdoc_diridx" );
	if ( !v )
		{
		v = strdcpy( &s, dirdat, DIR_CHAR "sdoc" DIR_CHAR "idx", _ );
		mkdir( v );
		}
	ts = str_explode( v, ";" );
	for( i = 0 ; ts[i] ; i++ )
		{
		if ( !(ts[i][0]) ) continue;
		ts2 = str_explode( ts[i], "::" );
		strdcpy( &(sdoc_diridx[sdoc_diridx_nb].name), ts2[0], _ );
		sdoc_diridx[sdoc_diridx_nb].maxnb = sdoc_diridx[sdoc_diridx_nb].maxsize = 0;
		if ( ts2[1] )
			{
			sdoc_diridx[sdoc_diridx_nb].maxnb = atoi( ts2[1] );
			if ( ts2[2] )
				{
				sdoc_diridx[sdoc_diridx_nb].maxsize = atoi( ts2[2] );
				}
			}
		sdoc_diridx_nb++;
		}


for( i = 0 ; i < sdoc_diridx_nb ; i++ )
{
print( itoa(i), ":", sdoc_diridx[i].name,
		"::", itoa((long)sdoc_diridx[i].maxnb),
		"::", itoa(sdoc_diridx[i].maxsize), BR, _ );
}


	v = var_get( hSrvConf, "sdoc_dirkwd" );
	if ( !v )
		{
		v = strdcpy( &s, dirdat, DIR_CHAR "sdoc" DIR_CHAR "kwd", _ );
		mkdir( v );
		}
	ts = str_explode( v, ";" );
	for( i = 0 ; ts[i] ; i++ )
		{
		if ( !(ts[i][0]) ) continue;
		ts2 = str_explode( ts[i], "::" );
		strdcpy( &(sdoc_dirkwd[sdoc_dirkwd_nb].name), ts2[0], _ );
		sdoc_dirkwd[sdoc_dirkwd_nb].maxnb = sdoc_dirkwd[sdoc_dirkwd_nb].maxsize = 0;
		if ( ts2[1] )
			{
			sdoc_dirkwd[sdoc_dirkwd_nb].maxnb = atoi( ts2[1] );
			if ( ts2[2] )
				{
				sdoc_dirkwd[sdoc_dirkwd_nb].maxsize = atoi( ts2[2] );
				}
			}
		sdoc_dirkwd_nb++;
		}


for( i = 0 ; i < sdoc_dirkwd_nb ; i++ )
{
print( itoa(i), ":", sdoc_dirkwd[i].name,
		"::", itoa((long)sdoc_dirkwd[i].maxnb),
		"::", itoa(sdoc_dirkwd[i].maxsize), BR, _ );
}
*/

	}




//------------------------------------------------------------------------------
int cmd_sdoc( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	if ( ! var_getn( hReq, 1, &vn ) )
		return ERROR;
	
	//----------------------------------------------------------------------------------
	if ( !stricmp( vn, "conf" ) )
	//----------------------------------------------------------------------------------
		{
		b = false;
		for( i = 2 ; v = var_getn( hReq, i, &vn ) ; i++ )
			{
			//----------------------------------------------------------------------
			if ( !stricmp( vn, "dirdat" ) )
			//----------------------------------------------------------------------
				{
				if ( v[0] == '+' )
					{
					v1 = var_get( hSrvConf, "sdoc_dirdat" );
					if ( v1 ) { strdcpy( &s, v1, ";", &(v[1]), _ ); v = s; }
					else v = &(v[1]);
					}

				var_set( hSrvConf, "sdoc_dirdat", v );
				b = true;
				continue;
				}
				
				
			//----------------------------------------------------------------------
			if ( !stricmp( vn, "writers" ) )
			//----------------------------------------------------------------------
				{
				if ( v[0] == '+' )
					{
					v1 = var_get( hSrvConf, "sdoc_writers" );
					if ( v1 ) { strdcpy( &s, v1, ";", &(v[1]), _ ); v = s; }
					else v = &(v[1]);
					}

				var_set( hSrvConf, "sdoc_writers", v );
				b = true;
				continue;
				}
			
			print( "ERREUR : paramètre '", vn, "' inconnu" BR, _ );
			if ( b )
				{
				var_free( hSrvConf );
				server_conf_load( );
				return ERROR;
				}
			}
		
		if ( b )	{ server_conf_save( ); sdoc_conf_load( t_work ); }
		
		if ( v = var_get( hSrvConf, "sdoc_dirdat" ) )
			print( "   dirdat  = ", v, BR, _ );
		if ( v = var_get( hSrvConf, "sdoc_writers" ) )
			print( "   writers = ", v, BR, _ );
		
		return OK;
		}
		
	
	//----------------------------------------------------------------------------------
	if ( !stricmp( vn, "save" ) )
	//----------------------------------------------------------------------------------
		{
		// LECTURE DU TYPE
		v1 = trim(lower(var_get( hReq, "type" )));
		if ( v1 == null )
			{
			print( "ERREUR : sdoc 'type' obligatoire" BR, _ );
			return ERROR;
			}
		
		// LECTURE DE L'ID
		v = var_getn( hReq, 2, &vn );
		if (( !v )||( !(vn[0]) )||( ( !is_numeric( vn ) )&&( stricmp( vn, "new" ) ) ))
			{
			print( "ERREUR : sdoc id obligatoire" BR, _ );
			return ERROR;
			}
		
		if ( !stricmp( vn, "new" ) )
			{
			strdcpy( &fname, dirconf, DIR_CHAR "sdoc.id", _ );
			strdcpy( &sid, itoa( get_uid( fname ) ), _ );
			}
		else
			strdcpy( &sid, vn, _ );
			
		n1 = strlen( sid );
		v4 = var_get( hSrvConf, "sdoc_dirdat" );
		strdcpy( &s1, "", _ ); // INIT LISTE DES MOTS CLEF
		strdncpy( &s4, ",", 1, sid, n1, "          ", 10-n1, ",", 1, _ );
		
			
		// VERIFIE LE CODE UNIQUE
/*		
		if ( v = var_get( hReq, "ucode" ) )
			{
			strdcpy( &fname, v4, DIR_CHAR "kw" DIR_CHAR "ucode" DIR_CHAR, upper(v), _ );
			if ( file_exists( fname ) )
				{
				if ( !stricmp( vn, "new" ) )
					{
					file_load( &s, fname );
					if (( strcmp( s, s4 ) )&&( strcmp( s, "," ) ))
						{
						print( "ERREUR : duplicate ucode '", v, "'", _ );
						return ERROR;
						}
					}
				else
					{
					print( "ERREUR : duplicate ucode '", v, "'", _ );
					return ERROR;
					}
				}
			
			fp = fopen( fname, "wb" );
			if ( fp == NULL )
				{
				mkdir( file_getdir( fname ) );
				fp = fopen( fname, "wb" );
				if ( fp == NULL )
					{
					print( "ERREUR : creation du fichier '", fname, "'", _ );
					return ERROR;
					}
				}
				
			fwrite( fp, ",", 1, sid, n1, "               ", 10-n1, ",", 1, _ );
			fclose( fp );
			strdcpy( &s1, ",ucode/", v, ",", _ );
			}
*/
		
		// SAUVEGARDE DES DONNEES
		
		var_delete( hReq, "sdoc" );
		var_delete( hReq, "save" );
		var_delete( hReq, "new" );
		var_delete( hReq, sid );
		var_set( hReq, "id", sid );
		
		n = atoi( sid ) / 10000;
		strdcpy( &fname, v4, DIR_CHAR, itoa(n), DIR_CHAR, sid, ".dat", _ );
		var_save( hReq, fname );
		
		strcpy( &(fname[n=(strlen(fname)-3)]), "lst" );
		fp = fopen( fname, "wb" );
		fprint( fp, "{id:", sid, ",type:", str_quote( v1 ),
				",title:", str_quote( var_get(hReq,"title") ),
				",resume:", str_quote( var_get(hReq,"resume") ),
				",authors:", str_quote( var_get(hReq,"authors") ),
				",status:", str_quote( var_get(hReq,"status") ),
				",dt_upd:", str_quote( var_get(hReq,"dt_upd") ),
				"}", _ );
		fclose( fp );
		


		// Calcul la liste des mots clef (noms d'indexs)
		
		strcpy( &(fname[n]), "kwl" );
		strdcpy( &s1, ",type=", upper(v), ",", _ );
		for( k = 1 ; k ; k++ )
			{
			switch( k )
				{
				case 1 : v = var_get( hReq, "title" ); break;
				case 2 : v = var_get( hReq, "resume" ); break;
				case 3 : v = var_get( hReq, "keywords" ); break;
				case 4 : v = var_get( hReq, "authors" ); break;
				default: v = null; k = -1;
				}
			
			if ( !v ) continue;
			
			ts = str_explode( upper( v ), " " );
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( ts[i][0] == 0 ) continue;
				strdcpy( &s, ",", ts[i], ",", _ );
				if ( strpos( s1, s, 0 ) < 0 )
					{
					if ( s1[0] )
						strdcat( &s1, &(s[1]), _ );
					else
						strdcat( &s1, s, _ );
					}
				}
			free( ts );
			}
	
		
		// SUPPRIME LES ANCIENS INDEXS NON RECONDUITS
		
		if ( file_load( &s2, fname ) )
			{
			ts = str_explode( s2, "," );
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( ts[i][0] == 0 ) continue;
				strdcpy( &s, ",", ts[i], ",", _ );
				if ( strpos( s1, s, 0 ) < 0 )
					{
					strdcpy( &fname1, v4, DIR_CHAR "kw" DIR_CHAR, v1, ".", ts[i], _ );
					if ( file_load( &s3, fname1 ) )
						{
						strd_replace( &s3, s4, "," );
						strd_replace( &s3, ",,", "," );
						file_save_str( fname1, s3, -1 );
						}
					}
				}
			}
	
		
		// CREATION DES INDEXS
		
		ts = str_explode( s1, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ts[i][0] == 0 ) continue;
			if ( !strncmp( ts[i], "ucode/", 6 ) ) continue;
			strdcpy( &s, ",", ts[i], ",", _ );
			if ( strpos( s2, s, 0 ) >= 0 ) continue; // si déjà indexé => suivant
			strdcpy( &fname1, v4, DIR_CHAR "kw" DIR_CHAR, v1, ".", ts[i], _ );
			b = file_exists( fname1 );
			fp = fopen( fname1, "ab" );
			if ( fp == NULL )
				{
				mkdir( file_getdir( fname1 ) );
				fp = fopen( fname1, "ab" );
				}
			if( b )
				fwrite( fp, sid, n1, "               ", 10-n1, ",", 1, _ );
			else
				fwrite( fp, ",", 1, sid, n1, "               ", 10-n1, ",", 1, _ );
			fclose( fp );
			}	
		
		
		// ENREGISTRE LA LISTE DES INDEXS

		file_save_str( fname, s1, -1 );
		

		print( "OK:", sid, _ );
		return OK;
		}
		
	//----------------------------------------------------------------------------------
	if ( !stricmp( vn, "load" ) )
	//----------------------------------------------------------------------------------
		{
		// LECTURE DE L'ID
		v = var_getn( hReq, 2, &vn );
		
		v4 = var_get( hSrvConf, "sdoc_dirdat" );
		if ( !stricmp( vn, "ucode" ) )
			{
			strdcpy( &fname, v4, DIR_CHAR "kw" DIR_CHAR "ucode" DIR_CHAR, upper(v), _ );
			if ( !file_exists( fname ) )
				{
				print( "ERREUR" BR, _ );
				return ERROR;
				}
			file_load( &s, fname );
			vn = itoa( atoi( &(s[1]) ) );
			}
		else
		if (( !v )||( !(vn[0]) )||( ( !is_numeric( vn ) )&&( stricmp( vn, "new" ) ) ))
			{
			print( "ERREUR : sdoc id obligatoire" BR, _ );
			return ERROR;
			}
		
		n = atoi( vn ) / 10000;
		strdcpy( &fname, v4, DIR_CHAR, itoa(n), DIR_CHAR, vn, ".dat", _ );
		if ( !file_exists( fname ) )
			{
			print( "ERREUR" BR, _ );
			return ERROR;
			}
		var_free( h );
		h = var_load( fname );
		print( var_to_js_array( h, &s ), BR, _ );
		
		return OK;
		}
		
	//----------------------------------------------------------------------------------
	if ( !stricmp( vn, "list" ) )
	//----------------------------------------------------------------------------------
		{
		// list {keyw1} !{kw2} ...
		
		v1 = trim(lower(var_get( hReq, "type" )));
		v4 = var_get( hSrvConf, "sdoc_dirdat" );
		if ( s1 ) free( s1 );
		s1 = null;
		v2 = upper(trim(var_get( hReq, "kwords" )));
		ts = str_explode( v2, " " );
		for( i = 0 ; v = ts[i] ; i++ )
			{
			if ( v[0] == 0 ) continue;
			strdcpy( &fname, v4, DIR_CHAR "kw" DIR_CHAR, v1, ".", upper(vn), _ );
			if ( file_load( &s, fname ) == sERROR ) continue;
			if ( !s1 ) { s1 = s; s = null; continue; }
			strdcpy( &s2, "", _ );
			p = 0; l = strlen( s );
			for( ; p < l ; p+=11 )
				{
				n = s[p+12];
				s[p+12] = 0;
				if ( strpos( s1, &(s[p]), 0 ) >= 0 )
					{
					if ( s2[0] )
						strdcat( &s2, &(s[p+1]), _ );
					else
						strdcat( &s2, &(s[p]), _ );
					}
				s[p+12] = n;
				}
			v = s2 ; s2 = s1 ; s1 = v ;
			}


		if ( s1 )
			{
			print( "[", _ );
			for( b = false, p = 1, l = strlen( s1 ) ; p < l ; p += 11 )
				{
				n = atoi( &(s1[p]) );
				strdcpy( &fname, v4, DIR_CHAR, itoa( n / 10000 ), DIR_CHAR, itoa(n), ".lst", _ );
				if ( file_load( &s, fname ) )
					{
					print( (char*)((b)?",":""), s, _ );
					b = true;
					}
				}
			print( "]", _ );
			}
			
		
		return OK;
		}
		
		
	print( "ERREUR : sdoc command inconnue : ", vn, BR, _ );
	}
	
	













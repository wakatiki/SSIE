#include "ssie7lib.h"


//------------------------------------------------------------------------------
int cmd_mft_cat( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	// Syntaxe : mftcat part=... {param_name}=..., date=0, ida={list_ida},
	// 	direct, status, rappl, parm, idt=, idf=, dt_upd,...,
	//	of=[ida|js|js:|jsa:|csv:|<sep>:|scr: chp1[p,l],chp2,...]
	//  OrderBy="chp1,-chp2,chp3[p,l],dt_upd,..."

	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif


//print( "mftcat" BR, _ );

	hMftCat = mftcat_open();

	var_delete( hReq, "user" );
	
	v = var_get( hReq, "ida" );
	if (( v )&&( strpos( v, "*", 0 ) < 0 )&&( strlen(v) >= 8 ))
		{
		strdcpy( &s4, v, _ );
		goto print_cat;
		}


	v = var_get( hReq, "idt" );
	if (( v )&&( strpos( v, "*", 0 ) < 0 )&&( strlen(v) >= 8 ))
		{
		var_free( h );
		if ( (h = mftcat_load( v )) != ERROR )
			{
			strdcpy( &s4, v, _ );
			goto print_cat;
			}
		}
	if ( v )
		{
		var_set( hReq, "ida", v );
		var_delete( hReq, "idt" );
		}
		
	// Construction de la requête bd
	
	strdcpy( &s3, "", _ );
	
	v = var_get( hReq, "date" );
	if ( !v ) v = "*";
	if (( v[0] != '*' )||( (v[1]) ))
		{
		n = atoi(v);
		if ( n <= 0 )
			{
			n = sysdate + ( n Jours );
			strdcpy( &s1, itod( n, "AAAA_MM_JJ" ), _ );
			}
		else
			{
			v1 = itod( 0, "AAAAMMJJ" );
			if ( atoi( &(v1[6]) ) < n )
				{
				v1[6] = 0;
				n1 = atoi( &(v1[4]) ) - 1;
				v1[4] = 0;
				if ( n1 <= 0 )
					{
					n1 = 12;
					strdcpy( &s, itoa( atoi(v1) - 1 ), itona(n1,2), itona(n,2), _ );
					}
				else
					{
					strdcpy( &s, v1, itona(n1,2), itona(n,2), _ );
					}
				}
			else
				{
				v1[6] = 0;
				strdcpy( &s, v1, itona(n,2), _ );
				}
			strdncpy( &s1, s, 4, "_", 1, &(s[4]), 2, "_", 1, &(s[6]), 2, _ );
			}
		strdcat( &s3, " day_upd=", s1, _ );
		}


	for( i = 1 ; v = var_getn( hReq, i, &vn ) ; i++ )
		{
		if ( strmicmp( vn, "cat|of|orderby|date" ) ) continue;
		if ( !stricmp( vn, "idt" ) )
			{
			strdcat( &s3, " idt_mft=", quote(v), _ );
			continue;
			}
		if (( !stricmp( vn, "status" ) )||( !stricmp( vn, "state" ) ))
			{
			strdcat( &s3, " status=", _ );
			for( j = 0 ; v[j] ; j++ )
				strdncat( &s3, (char*)((j)?"|":""), -1, &(v[j]), 1, _ );
			continue;
			}
		if (( !stricmp( vn, "error" ) )||( !stricmp( vn, "erreur" ) ))
			{
			strdcat( &s3, " status=K", _ );
			continue;
			}
		if ( !stricmp( vn, "send" ) )
			{
			strdcat( &s3, " direct=SEND", _ );
			continue;
			}
		if ( !stricmp( vn, "recv" ) )
			{
			strdcat( &s3, " direct=RECV", _ );
			continue;
			}
		if ( !stricmp( vn, "type" ) )
			{
			if ( !stricmp( v, "file" ) )
				strdcat( &s3, " type!=message", _ );
			continue;
			}
		if ( !stricmp( vn, "file_exists" ) ) continue;
		if ( !stricmp( vn, "force_status_t" ) ) continue;
		
		if (( !stricmp( vn, "or" ) )||( !stricmp( vn, "|" ) ))
			{
			strdcat( &s3, " or", _ );
			continue;
			}
			
		if ( v[0] == '!' )
			{
			strdcat( &s3, " ", vn, "!=", quote(&(v[1])), _ );
			continue;
			}
		
		strdcat( &s3, " ", vn, "=", quote(v), _ );
		}

	
	if ( ( v = var_get( hReq, "of" ) )&&( !stricmp( v, "count" ) ) )
		{
		v = null; // Inutile de trier si c'est pour compter
		if ( (n = bd_select( hMftCat, s3, v, null )) == ERROR )
			{
			print( error_msg, BR, _ );
			return;
			}
		print( itoa(n), BR, _ );
		return;
		}
	else
		{
		v = var_get( hReq, "orderby" );
		if ( !v )
			{
			v = var_get( hReq, "repair" );
			if ( v ) v = "ida";
			}
		if ( !v ) v = ( request_mode != MODE_TCP ) ? "" : "-dt_upd";

		if ( bd_select( hMftCat, s3, v, &s4 ) == ERROR )
			{
			print( error_msg, BR, _ );
			return;
			}
		}


	print_cat:
	
	if ( var_get( hReq, "repair" ) )
		{
		
/*
		strdcpy( &s, "", _ );
		
		// Recherche les doublons
		for( i = n = 0, l = strlen( s4 ) ; i < l ; i += 8 )
			{
			strdncpy( &s1, &(s4[i]), 8, _ );
			if ( !strcmp( s, s1 ) )
				{
				// Doublon trouvé => Réparation
				print( "Réparation de ", s, BR, _ );
				n++;
				}
			v = s1; s1 = s; s = v;
			}
		
		print( itoa( n ), " élément", (char*)((n>1) ? "s" : ""), " réindexé", (char*)((n>1) ? "s" : ""), BR, _ );
		return;
*/
		}
	
	v = var_get( hReq, "of" );
	if ( !v )
		{
		if ( request_mode != MODE_TCP )
			v = "scr:part,8,direct,4,ida,8,status,1,errno,3,dt_upd,19,fname,50";
		else
			v = "wscr:part,8,direct,4,ida,8,status,1,errno,3,dt_upd,19,fname,50";
		}
	else
	if ( !stricmp( v, "ssie" ) )
		return;
		
	if ( !stricmp( v, "ida" ) )
		{
		print( s4, _ );
		return;
		}
		
/*
	if ( !stricmp( v, "count" ) )
		{
		print( itoa(strlen(s4)/8), _ );
		return;
		}
*/		
	if ( (n=bd_load( hMftCat, s4, strlen(s4)/8, v, &s )) == ERROR )
		{
		print( error_msg, BR, _ );
		return;
		}
	
		
	print( s, _ );
	
	if (( !strnicmp( v, "scr:", 4 ) )||( !strnicmp( v, "wscr:", 5 ) ))
		{
//		n=(strlen(s4)/8);
		print( BR BR, itoa(n), " Transfert", (char*)((n>1)?"s":""), BR, _ );
		}

	return OK;
	}

























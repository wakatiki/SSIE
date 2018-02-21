#include "ssie7lib.h"


// GESTION DE L'ANNUAIRE DES PARTENAIRES

//------------------------------------------------------------------------------
int cmd_part( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	#define bJS			b1
	#define bMersi3		b2
	#define bSsie7		b3
	#define bSsie7List	b4

	partname = null;
	bMersi3 = bSsie7 = bSsie7List = false;	
	
	v = var_getn( hReq, 1, &vn );
	if ( !v ) goto part_liste;
	if ( ! stricmp( vn, "-mersi3" ) ) { bMersi3 = true; goto part_liste; }
	if ( ! stricmp( vn, "-ssie7" ) )  { bSsie7 = true; goto part_liste; }
	if ( ! stricmp( vn, "-ssie7list" ) )  { bSsie7List = true; goto part_liste; }
	
	partname = str( vn, _ );
	upper( partname );

	v = var_getn( hReq, 2, &vn );
	if ( !v ) goto part_liste;

	//--------------------------------------------------------------------------
	// RECHERCHE D'UN PARTENAIRE
	//--------------------------------------------------------------------------
	
	if ( !strcmp( vn, "get" ) )
		{
		// Recherche par le nom
		strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );
		if ( !file_exists( fname ) )
			{
			// Recherche par une ADL
			strdcpy( &fname1, dirpart, DIR_CHAR "adl" DIR_CHAR, partname, ".dat", _ );
			if ( !file_exists( fname1 ) )
				{
				// Recherche par l'IP ou une liste d'IP
				ts = str_explode( partname, "," );
				for( i = 0 ; ts[i] ; i++ )
					{
					strdcpy( &fname1, dirpart, DIR_CHAR "ip" DIR_CHAR, ts[i], ".dat", _ );
					if ( file_exists( fname1 ) ) break;
					}
				if ( ! ts[i] )
					{
					free( ts );
					print( "ERREUR ANNU : partenaire non trouvé", _ );
					return ERROR;
					}
				free( ts );
				}
			
			partname = trim(file_load( &s, fname1 ));
			strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );
			if ( !file_exists( fname ) )
				{
				print( "ERREUR ANNU : partenaire non trouvé", _ );
				return ERROR;
				}
			}
		
		// Retourne les info sur le partenaire
		
		if ( hPart ) var_free( hPart );
		hPart = var_load( fname );	
		var_set( hPart, "partname", partname );
		v = var_get( hPart, "part_mersi_version" );
		if ( !v ) v = var_get( hPart, "mersi_version" );
		if ( v )
			{
			if ( v[0] )
				{
				strdcpy( &s, "mersi v", v, _ );
				var_set( hPart, "product_name", s );
				}
			var_delete( hPart, "part_mersi_version" );
			var_delete( hPart, "mersi_version" );
			}
		var_to_js_array( hPart, &s );
		strd_replace( &s, "part_", "" );
		print( "{", s, "}", _ );
		
		return OK;
		}
			

	
	//--------------------------------------------------------------------------
	// SAUVEGARDE D'UN PARTENAIRE PAR SON SERVEUR LUI-MEME
	//--------------------------------------------------------------------------
	

	if ( var_get( hReq, "save" ) )
		{
		partname = var_get( hReq, "partname" );
		if ( !partname ) { print( "ERREUR ANNU : partname non fourni", _ ); return ERROR; }

		strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );
		
		if ( ! file_exists( fname ) )
			{
			if ( ! is_alphanum( partname, "_" ) )
				{
				logw( "part save : ", partname, " : ERREUR nom invalide", _ );
				print( "ERREUR ANNU : Le nom de partenaire doit être alphanumérique", _ );
				return ERROR;
				}
			
			if ( ! stricmp( partname, "MFTCCWEX" ) )
				{
				logw( "part save : ", partname, " : ERREUR nom invalide", _ );
				print( "ERREUR ANNU : ce nom de partenaire est déjà pris", _ );
				return ERROR;
				}
/*
			if ( ( strlen( partname ) == 8 )&&( partname[7] == 'I' ) )
				{
				logw( "part save : ", partname, " : ERREUR nom invalide", _ );
				print( "ERREUR ANNU : Nom de partenaire invalide (seul ONS peut fournir des noms qui finissent par I)", _ );
				return ERROR;
				}
*/
			}
			

		// VERIFIE QUE LE NOM N'EST PAS DEJA PRIS PAR UNE AUTRE MACHINE
		
		if ( file_exists( fname ) )
			{
			v = var_get( hReq, "ip_ssie" );
			if ( !v ) v = var_get( hReq, "ip" );

			var_free( hPart );
			hPart = var_load( fname );
			v1 = var_get( hPart, "ip_ssie" );
			if ( !v1 ) v1 = var_get( hPart, "ip" );
			if ( !v1 ) v1 = var_get( hPart, "part_ip" );
			if ( strcmp( v1, v ) )
				{
				logw( "part save : ", partname, ", ", v, " : ERREUR nom déjà pris (", v1, ")", _ );
				print( "ERREUR ANNU : ce nom de partenaire est déjà pris", _ );
				return ERROR;
				}
			v = var_get( hReq, "old_partname" );
			if ( ( v )&&( v[0] )&&( strcmp( v, "MFTCCWEX" ) ) )
				{
				// SUPPRIME L'ANCIEN NOM
				strdcpy( &fname1, dirpart, DIR_CHAR, v, ".dat", _ );
				file_delete( fname1 );
				}
			}
			
		// SAUVEGARDE LE PARTENAIRE
		
		var_delete( hReq, "part" );
		var_delete( hReq, "save" );
		partname = var_get( hReq, "partname" );

		strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );
		var_save( hReq, fname );

		logw( "part save : OK : ", fname, " ip_ssie=", var_get( hReq, "ip_ssie" ),
				" ip=", var_get( hReq, "ip" ), " port=", var_get( hReq, "port" ), _ );
		

		// SAUVEGARDE L'INDEX PAR IP

		strdcpy( &fname, dirpart, DIR_CHAR "ip", _ );
		mkdir( fname );
		v1 = var_get( hReq, "ip_ssie" );
		if ( !v1 ) var_get( hReq, "ip" );
		strdcpy( &fname, dirpart, DIR_CHAR "ip" DIR_CHAR, v1, ".dat", _ );
		file_save_str( fname, partname, -1 );
		

		
		// REPLIQUE SUR LES AUTRES SERVEURS

		if ( ! var_get( hReq, "no_replique" ) )
		    {
			#ifdef OS_UNIX
			if ( file_load( &s, "/appli/ssie7annu/conf/srvannu_list.cfg" ) )
			#else
			if ( file_load( &s, "c:/e/apps/ssie7annu/conf/srvannu_list.cfg" ) )
			#endif
				{
				strdcat( &cmd, " no_replique", _ );
				strdcpy( &cmd1, "part ", partname, " loadnat user=" NNI_ADM, _ );
	
				ts = str_explode( s, "," );
				for( i = 0 ; ts[i] ; i++ )
					{
					if ( ! (ts[i][0]) ) continue;
					if ( ! strncmp( ts[i], tcp_myIP(0), -1 ) ) continue;
					if ( strpos( ts[i], ":1760", 0 ) >= 0 )
{
						tcp_request( ts[i], cmd, &s );
//logw( "part save : replique : ", ts[i], " : ", cmd, BR, s, _ );
}
					else
{
						tcp_request( ts[i], cmd1, &s );
logw( "part save : replique : ", ts[i], " : ", cmd1, BR, s, _ );
}
					}
				}
			}
		
		// REPLIQUE SUR LE SERVEUR SSIE7 LOCAL
		
	   	strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );
	    strdcpy( &cmd, "part ", partname, " load ", fname, _ );
        				
	    strdcpy( &fname, "/tmp/maj_part_ssie_", itoa(getpid()), _ );
        file_save_str( fname, cmd, -1 );
        strdcpy( &cmd, dirbin, DIR_CHAR, "ssie7 @", fname, _ );
	    system( cmd );

		print( "OK", __ );
		return OK;
		}
	
	
	
	//--------------------------------------------------------------------------
	// SUPRESSION D'UN PARTENAIRE
	//--------------------------------------------------------------------------
	
	if ( !strcmp( vn, "delete" ) )
		{
/*
		v = var_getn( hReq, 1, &partname );
		if ( !v ) return ERROR;
*/
		strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );
		file_delete( fname );
		// REPLIQUE SUR LES AUTRES SERVEURS

		if ( ! var_get( hReq, "no_replique" ) )
		    {
			#ifdef OS_UNIX
			if ( file_load( &s, "/appli/ssie7annu/conf/srvannu_list.cfg" ) )
			#else
			if ( file_load( &s, "c:/e/apps/ssie7annu/conf/srvannu_list.cfg" ) )
			#endif
			strdcat( &cmd, " no_replique", _ );

			logw( "part delete : cmd=", cmd, ", s=", s, _ );
			ts = str_explode( s, "," );
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( ! (ts[i][0]) ) continue;
				if ( ! strncmp( ts[i], tcp_myIP(0), -1 ) ) continue;
				tcp_request( ts[i], cmd, &s );
				}
			}
		
		// REPLIQUE SUR LE SERVEUR SSIE7 LOCAL
		
	    strdcpy( &cmd, "part ", partname, " delete", _ );
        				
	    strdcpy( &fname, "/tmp/maj_part_ssie_", itoa(getpid()), _ );
        file_save_str( fname, cmd, -1 );
        strdcpy( &cmd, dirbin, DIR_CHAR, "ssie7 @", fname, _ );
	    system( cmd );

		print( "OK", _ );
		return OK;
		}
			
		
		
	//--------------------------------------------------------------------------
	// SAUVEGARDE ONS D'UN PARTENAIRE
	//--------------------------------------------------------------------------
	

	if ( !strcmp( vn, "save_ons" ) )
		{
		strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );

		v = var_get( hReq, "old_partname" );
		if ( ( v )&&( v[0] )&&( strcmp( v, "MFTCCWEX" ) ) )
			{
			// SUPPRIME L'ANCIEN NOM
			strdcpy( &fname1, dirpart, DIR_CHAR, v, ".dat", _ );
			file_delete( fname1 );
			}
		
		// SAUVEGARDE LE PARTENAIRE
		
		strdcpy( &fname, dirpart,DIR_CHAR, partname, ".dat", _ );
		var_free( h );
		h = var_load( fname );
		var_set( h, "partname", partname );
		if ( v = var_get( hReq, "os" ) )
			{
            var_set( h, "os", v );
            if ( var_get( h, "part_os") )
               var_delete( h, "part_os" );
			}
		if ( v = var_get( hReq, "ip" ) )
        {
            var_set( h, "ip", v );
            var_delete( h, "part_ip" );
        }
		if ( v = var_get( hReq, "ip_ssie" ) )
           var_set( h, "ip_ssie", v );

		if ( v = var_get( hReq, "dns" ) )
			{
             var_set( h, "dns", v );
             if ( var_get( h, "part_dns") )
                var_delete( h, "part_dns" );
	        }
		if ( v = var_get( hReq, "dns_ssie" ) )
             var_set( h, "dns_ssie", v );

		if ( v = var_get( hReq, "adl_liste" ) )
			{
			var_set( h, "adl_liste", v );
			var_delete( h, "part_adl_liste" );
			}
		if ( v = var_get( hReq, "port" ) )
			var_set( h, "port", v );
		if ( v = var_get( hReq, "port_mft" ) )
			{
			var_set( h, "port_mft", v );
			var_delete( h, "part_port_mft" );
			}
		if ( v = var_get( hReq, "product_name" ) )
			var_set( h, "product_name", v );

        var_set( h, "is_nat", "Oui" );
		
		var_save( h, fname );
		
		
		// SAUVEGARDE L'INDEX PAR IP
		
		strdcpy( &fname, dirpart, DIR_CHAR "ip", _ );
		mkdir( fname );
		if ( v = var_get( hReq, "ip" ) )
			strdcpy( &fname, dirpart, DIR_CHAR "ip" DIR_CHAR, v, ".dat", _ );
		if ( v = var_get( hReq, "ip_ssie" ) )
			strdcpy( &fname, dirpart, DIR_CHAR "ip" DIR_CHAR, v, ".dat", _ );
		file_save_str( fname, partname, -1 );
		

		// SUPPRIME LES ANCIENNES ADL

		strdcpy( &fname, dirpart, DIR_CHAR "adl", _ );
		mkdir( fname );
		
		v = var_get( hReq, "old_adl_liste" );
		if ( v )
			{
			ts = str_explode( v, "," );
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( ts[i][0] == 0 ) continue;
				strdcpy( &fname, dirpart, DIR_CHAR "adl" DIR_CHAR, ts[i], ".dat", _ );
				file_delete( fname );
				}
			}

		// MISE A JOUR DES INDEX ADL

		v = var_get( hReq, "adl_liste" );
		if ( !v ) return ERROR;
		ts = str_explode( v, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ts[i][0] == 0 ) continue;
			strdcpy( &fname, dirpart, DIR_CHAR "adl" DIR_CHAR, ts[i], ".dat", _ );
			file_save_str( fname, partname, -1 );
			}
		
		print( "OK", _ );
		

		// REPLIQUE SUR LES AUTRES SERVEURS D'ANNUAIRE

		if ( ! var_get( hReq, "no_replique" ) )
			{
			#ifdef OS_UNIX
			if ( file_load( &s, "/appli/ssie7annu/conf/srvannu_list.cfg" ) )
			#else
			if ( file_load( &s, "c:/e/apps/ssie7annu/conf/srvannu_list.cfg" ) )
			#endif
				{
				strdcat( &cmd, " no_replique", _ );
				strdcat( &cmd1, "part ", partname, " loadnat user=" NNI_ADM, _ );
	
				ts = str_explode( s, "," );
				for( i = 0 ; ts[i] ; i++ )
					{
					if ( ! (ts[i][0]) ) continue;
					if ( ! strncmp( ts[i], tcp_myIP(0), -1 ) ) continue;
					if ( strpos( ts[i], ":1760", 0 ) >= 0 )
						tcp_request( ts[i], cmd, &s );
					else
						tcp_request( ts[i], cmd1, &s );
					}
				}
			}
		
		// REPLIQUE SUR LE SERVEUR SSIE7 LOCAL
		
		strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );
		strdcpy( &cmd, "part ", partname, " load ", fname, _ );
				
		strdcpy( &fname, "/tmp/maj_part_ssie_", itoa(getpid()), _ );
		file_save_str( fname, cmd, -1 );
		strdcpy( &cmd, dirbin, DIR_CHAR, "ssie7 @", fname, _ );
		system( cmd );

		
		return OK;
		}
		
			
	//--------------------------------------------------------------------------
	// RECHERCHE D'UN PARTENAIRE POUR LES MERSI3
	//--------------------------------------------------------------------------
	
	if ( !strcmp( vn, "of" ) )
		{
		bJS = ( !strcmp( v, "js" ) ) ? true : false;
		
		// Recherche par le nom
		strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );
		if ( !file_exists( fname ) )
			{
			// Recherche par une ADL
			strdcpy( &fname1, dirpart, DIR_CHAR "adl" DIR_CHAR, partname, ".dat", _ );
			if ( !file_exists( fname1 ) )
				{
				// Recherche par l'IP ou une liste d'IP
				ts = str_explode( partname, "," );
				for( i = 0 ; ts[i] ; i++ )
					{
					strdcpy( &fname1, dirpart, DIR_CHAR "ip" DIR_CHAR, ts[i], ".dat", _ );
					if ( file_exists( fname1 ) ) break;
					}
				if ( ! ts[i] )
					{
					free( ts );
					print( "0 partenaire", _ );
					return ERROR;
					}
				free( ts );
				}
			
			partname = file_load( &s, fname1 );
			strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );
			if ( !file_exists( fname ) )
				{
				unlink( fname1 ); // Fait du nettoyage
				print( "0 partenaire", _ );
				return ERROR;
				}
			}
		
		// Retourne les info sur le partenaire
		
		var_free( hPart );
		hPart = var_load( fname );

		var_set( hPart, "part_name", partname );
		var_delete( hPart, "partname" );
		
		v = var_get( hPart, "part_ip" );
		if ( !v )
			{
			v = var_get( hPart, "ip_ssie" );
			if ( !v )	v = var_get( hPart, "ip" );
			var_set( hPart, "part_ip", v );
			var_delete( hPart, "ip" );
			}

		v = var_get( hPart, "part_os" );
		if ( !v ) var_set( hPart, "part_os", var_get( hPart, "os" ) );
		var_delete( hPart, "os" );
				
		v = var_get( hPart, "part_dns" );
		if ( !v ) var_set( hPart, "part_dns", var_get( hPart, "dns" ) );
		var_delete( hPart, "dns" );
				
		v = var_get( hPart, "part_mersi_version" );
		if ( !v ) var_set( hPart, "part_mersi_version", var_get( hPart, "product_name" ) );
		var_delete( hPart, "product_name" );
				
		v = var_get( hPart, "part_adl_liste" );
		if ( !v ) var_set( hPart, "part_adl_liste", var_get( hPart, "adl_liste" ) );
		var_delete( hPart, "adl_liste" );
				
		if ( bJS )
			var_to_js_array( hPart, &s );
		else
			var_to_str( hPart, &s );
		
		print( s, _ );
		
		return OK;
		}
			

	


	//--------------------------------------------------------------------------
	// SAUVEGARDE D'UN PARTENAIRE MERSI3
	//--------------------------------------------------------------------------
	

	if ( v = var_get( hReq, "ip" ) )
		{
		strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );

		if ( ! file_exists( fname ) )
			{
			if ( ! is_alphanum( partname, "_" ) )
				{
				print( "ERREUR ANNU : Le nom de partenaire doit être alphanumérique", _ );
				return ERROR;
				}
/*			
			if ( ( strlen( partname ) == 8 )&&( partname[7] == 'I' ) )
				{
				print( "ERREUR ANNU : Nom de partenaire invalide", _ );
				return ERROR;
				}
*/
			}

		// VERIFIE QUE LE NOM N'EST PAS DEJA PRIS PAR UNE AUTRE MACHINE
		
		if ( file_exists( fname ) )
			{
			v = var_get( hReq, "ip_ssie" );
			if ( !v ) v = var_get( hReq, "ip" );

			var_free( hPart );
			hPart = var_load( fname );
			v1 = var_get( hPart, "ip_ssie" );
			if ( !v1 ) v1 = var_get( hPart, "ip" );
			if ( ( v1 )&&( strcmp( v1, v ) ) )
				{
				logw( "part save : ", partname, ", ", v, " : ERREUR nom déjà pris (", v1, ")", _ );
				print( "ERREUR ANNU : ce nom de partenaire est déjà pris", _ );
				return ERROR;
				}
			v = var_get( hReq, "old_partname" );
			if ( ( v )&&( v[0] )&&( strcmp( v, "MFTCCWEX" ) ) )
				{
				// SUPPRIME L'ANCIEN NOM
				strdcpy( &fname1, dirpart, DIR_CHAR, v, ".dat", _ );
				file_delete( fname1 );
				}
			
			var_set( hReq, "adl_liste", (char*)( ( v = var_get( hPart, "adl_liste" ) ) ? v : "" ) );

			// CORRECTION BOGUE MERSI3 (PERTE DU DNS)
			v1 = var_get( hReq, "dns" );
			if ( ( v1 )&&( v1[0] )&&( strpos( v1, ".", 0 ) < 0 ) )
				{
				strdcpy( &s, v, ":", var_get( hReq, "port_mersi" ), _ );
				strdcpy( &cmd, "part ", partname, " of=js user=DSPCSPITCCWEBEXPESMMERSIMFT", _ );
	
				if ( s1 ) s1[0] = 0;
				tcp_request( s, cmd, &s1 );
				if ( stripos( s1, "part_dns:", 0 ) >= 0 )
					{
					strd_replace( &s1, "part_", "" );
					strdcpy( &s2, "part ", partname, " ", s1, _ );
					h = var_from_str( 0, s2 );
					if ( h != ERROR )
						{
						var_delete( h, "is_admin" );
						var_free( hReq );
						hReq = h;
						h = 0;
						}
					}
				else
					{
					v = var_get( hPart, "dns" );
					if (( v )&&( v[0] ))
						var_set( hReq, "dns", v );
					}
				}
			}
		
		
		// SAUVEGARDE LE PARTENAIRE

		var_delete( hReq, partname );
		var_delete( hReq, "part" );
		var_delete( hReq, "save" );

		var_save( hReq, fname );
		
		// SAUVEGARDE L'INDEX PAR IP
		
		strdcpy( &fname, dirpart, DIR_CHAR "ip", _ );
		mkdir( fname );
		strdcpy( &fname, dirpart, DIR_CHAR "ip" DIR_CHAR, var_get( hReq, "ip" ), ".dat", _ );
		file_save_str( fname, partname, -1 );
		
		
		// REPLIQUE SUR LES AUTRES SERVEURS D'ANNUAIRE

		if ( ! var_get( hReq, "no_replique" ) )
		   {
     		file_load( &s, "/appli/ssie7annu/conf/srvannu_list.cfg" );
	     	strdcat( &cmd, " no_replique", _ );

            ts = str_explode( s, "," );
		    for( i = 0 ; ts[i] ; i++ )
				{
				if ( ! (ts[i][0]) ) continue;
				if ( ! strncmp( ts[i], tcp_myIP(0), -1 ) ) continue;
				tcp_request( ts[i], cmd, &s );
 				}
           }

		// REPLIQUE SUR LE SERVEUR SSIE7 LOCAL
		
		strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );
		strdcpy( &cmd, "part ", partname, " load ", fname, _ );
				
		strdcpy( &fname, "/tmp/maj_part_ssie_", itoa(getpid()), _ );
		file_save_str( fname, cmd, -1 );
		strdcpy( &cmd, dirbin, DIR_CHAR, "ssie7 @", fname, _ );
		system( cmd );

		print( "OK", _ );
		return OK;
		}
	
	
	
	//--------------------------------------------------------------------------
	// LISTE DES PARTENAIRES
	//--------------------------------------------------------------------------
	
	part_liste:
	
	if ( var_get( hReq, "of" ) )
		{
		// RETOUR FACON IHM MERSI3
		// TEMPORAIRE LE TEMPS DE MISE EN PROD DE L'IHM SSIE
/*		
		b = ( var_get( hReq, "count" ) ) ? true : false;
		b1 = false;
		v = var_get( hReq, "of" );
		if ( ( v )&&( !strcmp( v, "js" ) ) ) b1 = true;
	
	
		b2 = true;
	
		n = 0;
		tf = NULL;
		hConf = 0;
	
		while( tf = file_list( tf, dirpart, "nodir" ) )
			{
			if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;
			if ( ( partname )&&( !b2 )&&( strncmp( tf->name, partname, -1 ) ) ) continue;
	
			if ( hConf ) var_free( hConf );
			hConf = var_load( strdcpy( &fname, dirpart, DIR_CHAR, tf->name, _ ) );
	
			if ( b2 )
				{
				v = var_get( hConf, "part_mersi_version" );
				if ( ( !v )||( strncmp( v, "3.", 2 ) ) ) continue;
				}
			tf->name[strlen(tf->name)-4] = 0;
			if ( !b )
				{
				if ( ! b1 )
				print( tf->name, " ", var_get( hConf, "part_ip" ), " ",
						var_get( hConf, "part_os" ), " ", var_get( hConf, "part_dns" ),
						" ", var_get( hConf, "part_mft_version" ),
						" ", var_get( hConf, "part_mersi_version" ),
						BR, _ );
				else
					{
					if ( n ) print( ",", _ );
					print( itoa(n), ":['", tf->name, "','", var_get( hConf, "part_ip" ), "','",
						var_get( hConf, "part_os" ), "','", var_get( hConf, "part_dns" ),
						"','", var_get( hConf, "part_mft_version" ),
						"','", var_get( hConf, "part_mersi_version" ),
						"']", _ );
					}
				}
			n++;
			}
	
	
		if ( hConf ) { var_free( hConf ); hConf = 0; }
		if ( ! b1 )
		print( itoa(n), " partenaire", (char*)((n>1)?"s":""), BR, _ );
		else
			{
			if ( n ) print( ",", _ );
			print( "count:", itoa(n), ",col_count:6", _ );
			print( ",is_admin:true", _ );
			}
*/
			print( "count:0,col_count:6,is_admin:true", _ );
		
		return OK;
		}
	
	if (( request_mode == MODE_TCP )&&( ! bSsie7List )&&( !bJS ))	print( "[", _ );
	
	strdcpy( &fname, dirpart, DIR_CHAR "*.dat", _ );

	tf = NULL; n = 0;
	while( tf = file_list( tf, fname, "nodir" ) )
		{
		strdcpy( &fname, dirpart, DIR_CHAR, tf->name, _ );

		var_free( hPart );
		hPart = var_load( fname );
		
		v1 = var_get( hPart, "partname" );
		if ( !v1 )
			{
			// FICHIER EN VERSION MERSI3
			if ( var_get( hPart, "part_ip" ) )
				strdcpy( &s,
					strpad( v = substr(tf->name, 0, strlen(tf->name)-4), 12, " " ), "| ",
					strpad( var_get( hPart, "part_ip" ), 16, " " ), "| ",
					strpad( var_get( hPart, "part_os" ), 8, " " ), "| ",
					var_get( hPart, "part_mersi_version" ), BR,
					_ );
			else
				strdcpy( &s,
					strpad( v = substr(tf->name, 0, strlen(tf->name)-4), 12, " " ), "| ",
					strpad( var_get( hPart, "ip" ), 16, " " ), "| ",
					strpad( var_get( hPart, "os" ), 8, " " ), "| ",
	                (char*)( ( v1=var_get( hPart, "product_name" ) )
						? v1
						: var_get( hPart, "mersi_version" ) ), BR,
					_ );
					
			}
		else
			{
			if ( ( ( v1 = var_get( hPart, "part_ip" ) ) || ( v1 = var_get( hPart, "ip" ) ) )
			          && ( ! var_get( hPart, "ip_ssie" ) ) )
				strdcpy( &s,
					strpad( v = var_get( hPart, "partname" ), 12, " " ), "| ",
					strpad( v1, 16, " " ), "| ",
					strpad( var_get( hPart, "os" ), 8, " " ), "| ",
                    (char*)( ( v2=var_get( hPart, "part_mersi_version" ) )
						? v2
						: var_get( hPart, "mersi_version" ) ), BR,
					_ );
			else
				strdcpy( &s,
					strpad( v = var_get( hPart, "partname" ), 12, " " ), "| ",
					strpad( var_get( hPart, "ip_ssie" ), 16, " " ), "| ",
					strpad( var_get( hPart, "os" ), 8, " " ), "| ",
					var_get( hPart, "product_name" ), BR,
					_ );
			}

		if ( ( bSsie7 )&&( stripos( s, "| SSIE v7", 0 ) < 0 ) ) continue;
		if ( bSsie7List )
			{
			if ( stripos( s, "| SSIE v7", 0 ) < 0 ) continue;
			v1 = var_get( hPart, "mft_gateway" );
			v2 = null;
			if ( ( v1 )&&( v1[0] ) )
				{
				strdcpy( &fname, dirpart, DIR_CHAR, v1, ".dat", _ );
				h = var_load( fname );
				if ( h != ERROR )
					if ( ! ( v2 = var_get( h, "dns_ssie" ) ) )
						v2 = var_get( h, "ip_ssie" );
				}
			if ( !v2 ) v2 = "si-ssie.edf.fr";
				
			print( v, "|", v2, BR, _ );
			continue;
			}
		if ( ( bMersi3 )&&( stripos( s, "| 3.1", 0 ) < 0 ) ) continue;
		if ( ( partname )&&( stripos( s, partname, 0 ) < 0 ) ) continue;

		if ( ( request_mode == MODE_TCP )&& ( ( bMersi3 )||( bSsie7 ) ) )
			{
			v2 = var_get( hPart, "ip_ssie" );
			if ( !v2 ) v2 = var_get( hPart, "ip" );
			if ( !v2 ) v2 = var_get( hPart, "part_ip" );
            v5 = var_get( hPart, "port" );
			if ( !v5 ) v5 = var_get( hPart, "part_port" );
			if ( !v5 ) v5 = var_get( hPart, "port_mersi" );
			if ( !v5 ) v5 = var_get( hPart, "part_port_mersi" );
			v2 = str( v2, ":", v5, _ );
			v3 = var_get( hPart, "os" );
			if ( !v3 ) v3 = var_get( hPart, "part_os" );
			v4 = var_get( hPart, "product_name" );
			if ( !v4 ) v4 = var_get( hPart, "part_mersi_version" );
			if ( !v4 ) v4 = var_get( hPart, "mersi_version" );
			
			print( (char*)((n)?",[":"["),
				quote( v ),
				",", quote( v2 ),
				",", quote( v3 ),
				",", quote( v4 ),
				"]",
				_ );

			n++;
			continue;
			}
			
		if ( request_mode == MODE_TCP )
			{
			v2 = var_get( hPart, "ip_ssie" );
			if ( !v2 ) v2 = var_get( hPart, "ip" );
			if ( !v2 ) v2 = var_get( hPart, "part_ip" );
            v5 = var_get( hPart, "port" );
			if ( !v5 ) v5 = var_get( hPart, "part_port" );
			if ( !v5 ) v5 = var_get( hPart, "port_mersi" );
			if ( !v5 ) v5 = var_get( hPart, "part_port_mersi" );
			v2 = str( v2, ":", v5, _ );
			v3 = var_get( hPart, "os" );
			if ( !v3 ) v3 = var_get( hPart, "part_os" );
			v4 = var_get( hPart, "product_name" );
			if ( !v4 ) v4 = var_get( hPart, "part_mersi_version" );
			if ( !v4 ) v4 = var_get( hPart, "mersi_version" );
			
			print( (char*)((n)?",{":"{"),
				"partname:", quote( v ),
				",ip:", quote( v2 ),
				",os:", quote( v3 ),
				",product_name:", quote( v4 ),
				"}",
				_ );
			}
		else
			print( s, _ );
		
		n++;
		}	

	if ( request_mode == MODE_TCP )
		{
		if (( ! bSsie7List )&&( !bJS )) // pas avec l'ihm mersi3
			print( "]", _ );
		}
	else
	if ( ! bSsie7List )
		print( BR, itoa(n), " partenaire", (char*)((n>1)?"s":""), BR, _ );
	


	//--------------------------------------------------------------------------

	return ERROR;
	}




// MISE A JOUR DES PARTENAIRES DE LA PASSERELLE SSIE

//------------------------------------------------------------------------------
int cmd_maj_part_ssie( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	tf = NULL;
	while( tf = file_list( tf, dirpart, "nodir absdir" ) )
		{
		strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
		strdcpy( &fname1, "/appli/ssie7/conf/part/", tf->name, _ );
		if ( ( file_exists( fname1 ) )&&( file_date( fname ) <= file_date( fname1 ) ) )
			continue;
		
		strdcpy( &cmd, "part ",
				substr(tf->name, 0, strlen(tf->name)-4), " load ", fname, _ );
				
		file_save_str( "/tmp/maj_part_ssie", cmd, -1 );
		print( cmd, BR, _ );
		
		strdcpy( &cmd, dirbin, DIR_CHAR, "ssie7 @/tmp/maj_part_ssie", _ );
		system( cmd );
		print( BR BR, _ );
		file_delete( "/tmp/maj_part_ssie" );
		}
	}


// MET A JOUR L'ANNUAIRE LOCAL SSIE7ANNU AVEC CELUI D'ONS

//------------------------------------------------------------------------------
int cmd_maj_part_ons( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	FILE *fpl;
	static char buf[512];

    int ind=0;
    
	v = var_getn( hReq, 1, &vn );
	if ( ( ! v )||( !file_exists( vn ) ) )
		{
		print( "ERREUR : Le fichier des partenaires ONS n'existe pas." BR, _ );
		return ERROR;
		}
	strdcpy( &fname2, vn, _ );
	
	
	v = var_getn( hReq, 2, &vn );
	if ( ( ! v )||( !file_exists( vn ) ) )
		{
		print( "ERREUR : Le fichier des ADL n'existe pas." BR, _ );
		return ERROR;
		}
	strdcpy( &fname1, vn, _ );
	
	file_load( &s1, fname1 );
	
	
			
	fpl = fopen( fname2, "rt" );
	if ( fpl == NULL )
		{
		print( "ERREUR : ouverture du fichier : ", fname2, BR, _ );
		return ERROR;
		}
	
	n1 = 0;
	strdcpy( &fname, "/appli/ssie7annu/data/maj_part_ons", _ );
	if ( file_exists( fname ) )
		n1 = dtoi( itod( file_date( fname ), "AAAA/MM/JJ" ), "AAAA/MM/JJ" );

	fgets( buf, 512, fpl );
	while( fgets( buf, 512, fpl ) != NULL )
		{
		ts = str_explode( buf, ";" );
		partname = str( ts[2], _ );
		strdcpy( &cmd, "part ", partname, " save_ons ", _ );

		if ( strlen( partname ) != 8 ) continue;
		
		strdcpy( &fname, dirpart, DIR_CHAR, partname, ".dat", _ );

if ( !stricmp( partname, "PXFUPP1I" ) )
	ts[13] = "192.168.139.110:1765/192.196.120.29";
else
if ( !stricmp( partname, "CXFUPP1I" ) )
	ts[13] = "192.168.137.126:1765/192.196.91.18";
else
if ( !stricmp( partname, "NOEFY7NI" ) )
	ts[13] = "10.122.49.77:1765/163.116.0.64";
	
		
		n2 = dtoi( ts[1], "JJ/MM/AAAA" );

		if ( ( !file_exists( fname ) )||( n2 >= n1 ) )
			{
			if ( ! ts[21] || strlen(ts[21]) == 0 )
				{ strdcat( &cmd, " ip=", ts[13], " os=", ts[3], " dns='", ts[20], "' port_mft=1761", _ ); n2++; }
			else
				{
				if ( !file_exists( fname ) )
					strdcat( &cmd, " ip=", ts[13], " ip_ssie=", ts[21], " os=", ts[3], " dns='", ts[20], "' dns_ssie='", ts[22], "' port=1765 product_name=ssie", _ );
				else
					strdcat( &cmd, " ip=", ts[13], " ip_ssie=", ts[21], " os=", ts[3], " dns='", ts[20], "' dns_ssie='", ts[22], "'", _ );
				}
			p = -1;
			strdcpy( &s3, "", _ );
			while( ( p = strpos( s1, partname, ++p ) ) >= 0 )
				{
				if ( ( p - 19 ) < 0 ||
                     ( ( p - 20 ) >= 0 && s1[p-20] != '\n' ) ) continue;
				strdncpy( &s2, &(s1[p-19]), 8, _ );
				if ( strpos( s3, s2, 0 ) < 0 )
					{
					strdcat( &s3, s2, ",", _ );
					}
				}
			strdcat( &cmd, " adl_liste='", s3, "'", _ );
			
			print( cmd, BR, _ );
			file_save_str( "/tmp/maj_part_ons", cmd, -1 );
			system( "ssie7annu @/tmp/maj_part_ons" );
			print( BR BR, _ );
			}

		free( ts );
		}
	
	fclose( fpl );
	file_save_str( "/appli/ssie7annu/data/maj_part_ons", "", 0 );
	
	return OK;
	}	


















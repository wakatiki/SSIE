#include "ssie7.h"


//------------------------------------------------------------------------------
int cmd_app( T_WORK *t_work )

// GESTION DES APPLICATIONS AU SENS SSIE v7
//------------------------------------------------------------------------------
	{
	
	// RECHERCHE SI C'EST UNE MISE A JOUR
	
	b = false;
	b1 = false; // Changement de login
	
	for( i = var_count( hReq )-1 ; ( i > 0 )&&( v = var_getn( hReq, i, &vn ) ) ; i-- )
		{
		if ( strcmp( v, "true" ) )
			{
			if ( strmicmp( vn, "descr|name|login|of" ) == false )
				{
				print( "ERREUR : le paramètre ", vn, " est inconnu (cmd_app)." BR, _ );
				return ERROR;
				}
			if ( strcmp( vn, "of" ) )
				b = true;
			}
		}
	
	// LECTURE DU DEUXIEME PARAMETRE
	
	appname = null;
	v = var_getn( hReq, 1, &appname );
	if (( v )&&( strcmp( appname, "of" ) ))
		{
		if ( strcmp( v, "true" ) )
			{
			print( "ERREUR : syntaxe : app {app_name} ...", BR, _ );
			return ERROR;
			}
		
		// LE NOM DE L'APPLICATION A ETE FOURNI
		
		upper( appname );
		if ( ! is_authorized( appname, "visiteur" ) ) return ERROR; // PAS DE DROIT
		
		
		// CHARGE LA CONFIGURATION ACTUELLE DE L'APPLI
		
		if ( hConf ) var_free( hConf );
		hConf = var_load( strdcpy( &fname, dircflux, "/", appname, ".dat", _ ) );
		if ( b )
			{
			// VERIFIE LE NOM D'APPLICATION
			
            for( i = strlen( appname ) - 1 ; i >=0 ; i-- )
                {
                if ( ( appname[i] >= 'A' )&&( appname[i] <= '_' ) ) continue;
                if ( ( appname[i] >= '0' )&&( appname[i] <= '9' ) ) continue;
                switch( appname[i] )
                    {
                    case ' ' :
                        appname[i] = '_';
                    case '_' :
                    case '-' :
                        break;
                    default :
                        print( "ERREUR : Le nom d'application contient des caractères interdits" BR, _ );
                        return ERROR;
                    }
                }
            
			if ( strmicmp( appname, "start|stop|delete|create|version|up|down|"
								"descr|actif|version|etape|type|dest_liste|adle|appe"
								"|appr|mft_options|appl_info|bin_files|txt_files"
								"|planif_enable|waitfile|waitack|sendack|restart|restart_onrecv"
								"|planif|ftp|ftp_dest|ftp_login|ftp_pw|ftp_prot|ftp_sshkey"
								"|ftp_cmd|emet_liste|start_cond|script|locked|comment"
								"|ack_fname|wait_fname|timeout|horaire|flux_name"
								"|log|c|of|filter|date|name|duplique|pki|restore|noreport"
								"|desactive|insert|no_verify|encours|termine|execute|delete" ) )
				{
				print( "ERREUR : Ce nom d'application n'est pas permis" BR, _ );
				return ERROR;
				}
			

			if ( ! is_authorized( appname, "admin" ) ) return ERROR; // PAS DE DROIT DE MODIF
			
			// CHARGE LES INFORMATIONS
			
			if ( v = var_get( hReq, "descr" ) )
				var_set( hConf, "app_descr", v );
			if ( v = var_get( hReq, "env_liste" ) )
				var_set( hConf, "app_env_liste", v );
			if ( v = var_get( hReq, "env" ) )
				var_set( hConf, "app_env", v );
			if ( v = trim(var_get( hReq, "login" )) )
				{
				var_set( hConf, "app_login", v );
				b1 = true;
				}
				
			// VERIFIE SI DEMANDE DE CHANGEMENT DE NOM DE L'APPLICATION
			
			if ( ( v = upper(var_get( hReq, "name" )) )&&( strcmp( v, appname ) ) )
				{
				// VERIFIE LE NOUVEAU NOM
	            for( i = strlen( appname ) - 1 ; i >=0 ; i-- )
	                {
	                if ( ( v[i] >= 'A' )&&( v[i] <= '_' ) ) continue;
	                if ( ( v[i] >= '0' )&&( v[i] <= '9' ) ) continue;
	                switch( appname[i] )
	                    {
	                    case ' ' :
	                        v[i] = '_';
	                    case '_' :
	                    case '-' :
	                        break;
	                    default :
	                        print( "ERREUR : Le nom d'application contient des caractères interdits" BR, _ );
	                        return ERROR;
	                    }
	                }
	                
	            // CHANGE LE NOM DE L'APPLICATION
	            
				unlink( fname ); // SUPPRIME L'ANCIEN FICHIER
				

				// RENOMME LES FICHIERS DES FLUX ET ETAPES
				
				tf = NULL;
				while( tf = file_list( tf, dircflux, "nodir" ) )
					{
					if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;
					if ( ( p = strpos( tf->name, "_flux_", 0 ) ) < 0 )
						continue;
					if ( strcmp( substr( tf->name, 0, p ), appname ) ) continue;
					
					file_move( strdcpy( &s1, dircflux, "/", tf->name, _ ), strdcpy( &s2, v, &(tf->name[p]), _ ) );
					
					}
				}
			var_save( hConf, fname );
			}
		
		
		// DEMANDES DE SUPPRESSION
		
		if ( var_get( hReq, "delete" ) )
			{
 			file_delete( strdcpy( &s, dircflux, DIR_CHAR, appname, ".*", _ ) );
			file_delete( strdcpy( &s, dircflux, DIR_CHAR, appname, "_*", _ ) );
			print( "OK" BR, _ );
			return OK;
			}
		
		// CHANGEMENT DE LOGIN => ON CHANGE POUR LES FLUX EN COURS
		
		if ( b1 )
			{
			v1 = var_get( hConf, "app_login" );
			strdcpy( &fname, dirflux, DIR_CHAR "*.run", _ );
			tf = NULL;
			while( tf = file_list( tf, fname, "absdir nodir" ) )
				{
				strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
				strcpy( &(fname[strlen(fname)-3]), "dat" );
//print( "modif flux : ", fname, BR, _ );
				if ( h ) var_free( h );
				h = var_load( fname );
				v = var_get( h, "app_name" );
				if ( ( v )&&( !stricmp( v, appname ) ) )
					{
					var_set( h, "app_login", v1 );
					var_save( h, fname );
					}
				}
			}
		
		
		// AFFICHE LES INFORMATIONS SUR L'APPLICATION

		v = var_get( hReq, "of" );
		if ( !v )
			{
			print( "   descr     = ", var_get( hConf, "app_descr" ), BR, _ );	
			print( "   login     = ", var_get( hConf, "app_login" ), BR, _ );	
			}
		else
		if ( ! strcmp( v, "js" ) )
			{
			if ( is_authorized( appname, "gestionnaire" ) )
				var_set( hConf, "profile", "gestionnaire" );
			else
			if ( is_authorized( appname, "editeur" ) )
				var_set( hConf, "profile", "editeur" );
			else
			if ( is_authorized( appname, "visiteur" ) )
				var_set( hConf, "profile", "visiteur" );
			else
				var_set( hConf, "profile", "none" );

//			var_set( hConf, "is_admin", (char*)(( is_admin )?"true":"false") );	
//			var_set( hConf, "version", PRODUCT_NAME );	
			print( var_to_js_array( hConf, &s ), _ );
			}
		else
			{
			print( var_to_str( hConf, &s ), _ );
			}
		
		return OK;
		}

	// AUCUN PARAMETRES FOURNI => AFFICHE LA LISTE DES APPLICATIONS
	
	n = 0;
	tf = NULL;
	b1 = false;
	v = var_get( hReq, "of" );
	if ( ( v )&&( !strcmp( v, "js" ) ) ) b1 = true;
	
	if ( b1 ) print( "[", _ );
	
	while( tf = file_list( tf, dircflux, "nodir" ) )
		{
		if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;
		if ( ( strpos( tf->name, "_flux", 0 ) ) >= 0 ) continue;
		
		if ( hConf ) var_free( hConf );
		hConf = var_load( strdcpy( &fname, dircflux, "/", tf->name, _ ) );
		tf->name[strlen(tf->name)-4] = 0;
		if ( ! is_authorized( tf->name, "visiteur" ) ) continue;
		if ( ! b1 )
			print( strpad( tf->name, 25, " " ), " : ", var_get( hConf, "app_descr" ), BR, _ );
		else
			{
			print( (char*)((n) ? ",":""), "[", str_quote(tf->name), _ );
				
			if ( is_authorized( tf->name, "gestionnaire" ) )
				print( ",'gestionnaire',", _ );
			else
			if ( is_authorized( tf->name, "editeur" ) )
				print( ",'editeur',", _ );
			else
				print( ",'visiteur',", _ );

			print( str_quote(var_get( hConf, "app_login" )), "]", _ );
			}
		n++;
		}
		
	if ( ! b1 )
		print( itoa(n), " application", (char*)((n>1)?"s":""), BR, _ );	
	else
		{
		if ( b1 ) print( "]", _ );
/*		
		if ( n ) print( ",", _ );
		print( "count:", itoa(n), ",col_count:1", _ );
		print( ",is_admin:", (char*)(( is_admin ) ? "true":"false"), _ );
		print( ",version:", str_quote(PRODUCT_NAME), _ );
*/		
		}
	}






























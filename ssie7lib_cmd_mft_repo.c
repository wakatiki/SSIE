#include "ssie7lib.h"


#define reponame	v5


//------------------------------------------------------------------------------
int cmd_mft_repo( T_WORK * t_work )

// COMMANDE REPO : Gestion de repository
//------------------------------------------------------------------------------
	{
	if ( ! is_authorized( "is_admin", null ) )
		{
		print( "Commande non autorisée." BR, _ );
		return ERROR;
		}
		
	
	// RECHERCHE SI C'EST UNE MISE A JOUR
	
	b = false;
	for( i = var_count( hReq )-1 ; ( i > 0 )&&( v = var_getn( hReq, i, &vn ) ) ; i-- )
		{
		if ( strcmp( v, "true" ) )
			{
			if ( strmicmp( vn, "dirdat|of" ) == false )
				{
				print( "ERREUR : le paramètre ", vn, " est inconnu." BR, _ );
				return ERROR;
				}
			if ( strcmp( vn, "of" ) )
				b = true;
			}
		}
	
	// LECTURE DU DEUXIEME PARAMETRE
	
	v = var_getn( hReq, 2, &reponame );
	if (( v )&&( strcmp( reponame, "of" ) ))
		{
		if ( strcmp( v, "true" ) )
			{
			print( "ERREUR : syntaxe : mft repo {repo_name} ...", BR, _ );
			return ERROR;
			}
		
		// LE NOM DU REPOSITORY A ETE FOURNI
		
		upper( reponame );
		
		// CHARGE LA CONFIGURATION ACTUELLE
		
		if ( hConf ) var_free( hConf );
		hConf = var_load( strdcpy( &fname, dirconf, DIR_CHAR "repo" DIR_CHAR, reponame, ".dat", _ ) );
		if ( b )
			{
			// VERIFIE LE NOM D'APPLICATION
			
            for( i = strlen( reponame ) - 1 ; i >=0 ; i-- )
                {
                if ( ( reponame[i] >= 'A' )&&( reponame[i] <= '_' ) ) continue;
                if ( ( reponame[i] >= '0' )&&( reponame[i] <= '9' ) ) continue;
                switch( reponame[i] )
                    {
                    case ' ' :
                        reponame[i] = '_';
                    case '_' :
                    case '-' :
                        break;
                    default :
                        print( "ERREUR : Le nom du dépôt contient des caractères interdits" BR, _ );
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
				print( "ERREUR : Ce nom de dépôt n'est pas permis" BR, _ );
				return ERROR;
				}
			

			// CHARGE LES INFORMATIONS
			
			if ( v = var_get( hReq, "dirdat" ) )
				var_set( hConf, "dirdat", v );
				
			var_save( hConf, fname );
			}
		
		
		// DEMANDES DE SUPPRESSION
		
		if ( var_get( hReq, "delete" ) )
			{
			file_delete( strdcpy( &s, dirconf, DIR_CHAR "repo" DIR_CHAR, reponame, ".dat", _ ) );
			print( "OK" BR, _ );
			return OK;
			}
			
		// AFFICHE LES INFORMATIONS SUR L'APPLICATION

		v = var_get( hReq, "of" );
		if ( !v )
			{
			print( "   dirdat = ", var_get( hConf, "dirdat" ), BR, _ );	
			}
		else
		if ( ! strcmp( v, "js" ) )
			{
			print( var_to_js_array( hConf, &s ), _ );
			}
		else
			{
			print( var_to_str( hConf, &s ), _ );
			}
		
		return OK;
		}

	// AUCUN PARAMETRES FOURNI => AFFICHE LA LISTE DES DEPOTS
	
	n = 0;
	tf = NULL;
	b1 = false;
	v = var_get( hReq, "of" );
	if ( ( v )&&( !strcmp( v, "js" ) ) ) b1 = true;
	
	if ( b1 ) print( "[", _ );
	strdcpy( &fname, dirconf, DIR_CHAR "repo", _ );
	while( tf = file_list( tf, fname, "nodir" ) )
		{
		if ( (p=strpos( tf->name, ".dat", 0 )) < 0 ) continue;
		tf->name[p] = 0;
		print( tf->name, (char*)((b1)?"]":BR), _ );
		n++;
		}
		
	if ( ! b1 )
		print( itoa(n), " repository", (char*)((n>1)?"s":""), BR, _ );	
	else
		{
		if ( b1 ) print( "]", _ );
		}
	
	return OK;
	}


























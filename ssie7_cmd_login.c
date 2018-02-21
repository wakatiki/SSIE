#include "ssie7.h"

#define bModif		b
#define login_name	(t_work->u1)

//------------------------------------------------------------------------------
int cmd_login( T_WORK *t_work )

// COMMANDE LOGIN DE SSIE7 (GESTION DES HABILITATIONS POUR LES CONNEXION VIA L'INTRANET)
//------------------------------------------------------------------------------
	{
	if ( ! is_authorized( "is_admin", null ) )
		{
		print( "ERREUR : Vous n'êtes pas habilité pour réaliser cette action", _ );
		return ERROR;
		}
	
	b = false;
	for( i = var_count( hReq )-1 ; ( i > 0 )&&( v = var_getn( hReq, i, &vn ) ) ; i-- )
		{
		if ( strcmp( v, "true" ) )
			{
			if ( strmicmp( vn, "app_liste|is_admin|enable|of|adll|adld|reporting" ) == false )
				{
				print( "ERREUR : le paramètre ", vn, " est inconnu (cmd_login)." BR, _ );
				return ERROR;
				}
			if ( strcmp( vn, "of" ) )
				b = true;
			}
		}
	
	
	// LECTURE DU DEUXIEME PARAMETRE
	
	login_name = null;
	v = var_getn( hReq, 1, &login_name );
	if (( v )&&( strcmp( login_name, "of" ) ))
		{
		if ( strcmp( v, "true" ) )
			{
			print( "ERREUR : syntaxe : login {log_name} ...", BR, _ );
			return ERROR;
			}
		
		upper( login_name );
		if ( hConf ) var_free( hConf );
		hConf = var_load( strdcpy( &fname, dirconf, "/login/", login_name, ".dat", _ ) );
		if ( v = var_get( hReq, "delete" ) )
			{
			unlink( fname );
			v = var_get( hSrvConf, "srv_part_name" );
			nat_request( &s, strdcpy( &s, "login ", login_name, " delete ", v, _ ) );
			print( "OK", _ );
			return OK;
			}
		b = false;
		if ( v = var_get( hReq, "app_liste" ) )
			{ var_set( hConf, "app_liste", v ); b = true; }
		if ( v = var_get( hReq, "desable" ) )
			{ var_set( hConf, "enable", "false" ); b = true; }
		if ( v = var_get( hReq, "enable" ) )
			{
			if ( strmicmp( v, "n|no|non|false" ) )
				var_set( hConf, "enable", "Non" );
			else
				var_set( hConf, "enable", "Oui" );
			b = true;
			}
		if ( v = var_get( hReq, "is_admin" ) )
			{
			if ( strmicmp( v, "o|oui|y|yes|true|vrai" ) )
				var_set( hConf, "is_admin", "Oui" );
			else
				var_set( hConf, "is_admin", "Non" );
			b = true;
			}
		if ( v = var_get( hReq, "is_super_admin" ) )
			{
			if ( stricmp( v, "o|oui|y|yes|true|vrai" ) )
				var_set( hConf, "is_admin", "Super" );
			else
				var_set( hConf, "is_admin", "Non" );
			b = true;
			}
		if ( v = var_get( hReq, "mftcat" ) )
			{ var_set( hConf, "mftcat", upper(v) ); b = true; }
		if ( v = var_get( hReq, "adll" ) )
			{ var_set( hConf, "adll", upper(v) ); b = true; }
		if ( v = var_get( hReq, "adld" ) )
			{ var_set( hConf, "adld", upper(v) ); b = true; }
		if ( v = var_get( hReq, "reporting" ) )
			{ var_set( hConf, "reporting", v ); b = true; }
		
		if ( b )
			{
			var_save( hConf, fname );
			v = var_get( hConf, "enable" );
			if ( ( !v )||( !strcmp( v, "Oui" ) ) )
				{
				v = var_get( hSrvConf, "partname" );
				nat_request( &s, strdcpy( &s, "login ", login_name, " set ", v, _ ) );
				}
			else
				{
				v = var_get( hSrvConf, "partname" );
				nat_request( &s, strdcpy( &s, "login ", login_name, " delete ", v, _ ) );
				}
			}
			
		v = var_get( hReq, "of" );
		if ( !v )
			{
			print( "   enable    = ", var_get( hConf, "enable" ), BR, _ );	
			print( "   is_admin  = ", var_get( hConf, "is_admin" ), BR, _ );	
			print( "   mftcat    = ", var_get( hConf, "mftcat" ), BR, _ );	
			print( "   app_liste = ", var_get( hConf, "app_liste" ), BR, _ );	
			print( "   reporting = ", var_get( hConf, "reporting" ), BR, _ );	
			print( "   adll      = ", var_get( hConf, "adll" ), BR, _ );	
			print( "   adld      = ", var_get( hConf, "adld" ), BR, _ );	
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

	n = 0;
	tf = NULL;
	b1 = false;
	v = var_get( hReq, "of" );
	if ( ( v )&&( !strcmp( v, "js" ) ) ) b1 = true;
	
	if ( !b1 )
		print( BR "  NNI     | Actif | Est Admin", BR, _ );
	
	strdcpy( &fname, dirconf, "/login", _ );
	while( tf = file_list( tf, fname, "nodir" ) )
		{
		if ( strpos( tf->name, ".dat", 0 ) < 0 ) continue;

		if ( hConf ) var_free( hConf );
		hConf = var_load( strdcpy( &fname, dirconf, "/login/", tf->name, _ ) );
		p = strpos( tf->name, ".", 0 );
		tf->name[p] = 0;
		if ( ! b1 )
			print( "  ", strpad( tf->name, 8, " " ), "| ",
				strpad( var_get( hConf, "enable" ), 6, " " ), "| ", var_get( hConf, "is_admin" ), BR, _ );
		else
			{
			if ( n ) print( ",", _ );
			print( itoa(n), ":[", str_quote(tf->name),
				",", str_quote(var_get( hConf, "enable" )),
				",", str_quote(var_get( hConf, "is_admin" )),
				",", str_quote(var_get( hConf, "app_liste" )),
				",", str_quote(var_get( hConf, "reporting" )),
				"]", _ );
			}
		n++;
		}
	if ( ! b1 )
		print( BR "  ", itoa(n), " login", BR, _ );	
	else
		{
		if ( n ) print( ",", _ );
		print( "count:", itoa(n), ",col_count:1", _ );
		}

	return OK;
	}

































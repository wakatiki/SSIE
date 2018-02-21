#include "ssie7lib.h"

// cmd_u2w, cmd_w2u, cmd_var_print, cmd_var_set, cmd_system


/*
//------------------------------------------------------------------------------
int cmd_u2w( T_WORK *t_work )

// TRANSFORME UN FICHIER TEXTE FORMAT UNIX AU FORMAT WINDOWS
//------------------------------------------------------------------------------
	{
	if ( var_count( hReq ) < 3 )
		{
		print( "ERREUR : syntaxe : u2w [in fname] [out fname]" BR, _ );
		return;
		}
	var_getn( hReq, 1, &v1 );
	var_getn( hReq, 2, &v2 );
	
	if ( u2w( v1, v2 ) == ERROR )
		print( "ERREUR : ", error_msg, BR, _ );
	else
		print( "OK", BR, _ );

	return OK;
	}




//------------------------------------------------------------------------------
int cmd_w2u( T_WORK *t_work )

// TRANSFORME UN FICHIER TEXTE FORMAT WINDOWS AU FORMAT UNIX
//------------------------------------------------------------------------------
	{
	if ( var_count( hReq ) < 3 )
		{
		print( "ERREUR : syntaxe : w2u [in fname] [out fname]" BR, _ );
		return ERROR;
		}
	var_getn( hReq, 1, &v1 );
	var_getn( hReq, 2, &v2 );
	
	if ( w2u( v1, v2 ) == ERROR )
		print( "ERREUR : ", error_msg, BR, _ );
	else
		print( "OK", BR, _ );
	return OK;
	}
*/



//------------------------------------------------------------------------------
int cmd_var_print( T_WORK *t_work )

// AFFICHE LE CONTENU D'UN FICHIER DE CONFIGURATION
//------------------------------------------------------------------------------
	{
/*
	if ( ! is_authorized( "is_admin", null ) )
		{
		print( "ERREUR : Vous n'êtes pas habilité pour réaliser cette action", _ );
		return ERROR;
		}
*/	
	v = var_getn( hReq, 1, &vn );
	if ( !v ) return OK;
	
	if ( ! stricmp( vn, "srvconf" ) ) strdcpy( &fname, "$dirconf/", module_name, ".dat", _ );
	else
		strdcpy( &fname, vn, _ );
		
	strd_replace( &fname, "$dirconf", dirconf );
	strd_replace( &fname, "$dirdat", dirdat );
//	strd_replace( &fname, "$dirflux", dirflux );
	
	if ( !file_exists( fname ) )
		{
		print( "ERREUR : Le fichier n'existe pas : ", fname, BR, _ );
		return ERROR;
		}
	
	if ( h ) var_free( h );	
	h = var_load( fname );
	
	if ( request_mode != MODE_TCP )
		var_print( h, BR );	
	else
		{
		var_sprint( h, BR, &s );
		ssie7print( t_work, s, _ );
		}
	
	return OK;
	}





//------------------------------------------------------------------------------
int cmd_var_print0( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &vn );
	if ( !v ) return OK;
	
	if ( ! stricmp( vn, "srvconf" ) ) strdcpy( &fname, "$dirconf/", module_name, ".dat", _ );
	else
		strdcpy( &fname, vn, _ );
		
	strd_replace( &fname, "$dirconf", dirconf );
	strd_replace( &fname, "$dirdat", dirdat );
	
	if ( !file_exists( fname ) )
		{
		print( "ERREUR : Le fichier n'existe pas : ", fname, BR, _ );
		return ERROR;
		}
	
	if ( h ) var_free( h );	
	h = var_load( fname );
	
	var_print0( h, BR );	
	
	return OK;
	}





//------------------------------------------------------------------------------
int cmd_var_set( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &vn );
	if ( !v ) return OK;
	
	
	if ( ! stricmp( vn, "srvconf" ) ) strdcpy( &fname, "$dirconf/", module_name, ".dat", _ );
	else
		strdcpy( &fname, vn, _ );
		
	strd_replace( &fname, "$dirconf", dirconf );
	strd_replace( &fname, "$dirdat", dirdat );

	
	if ( h ) var_free( h );	
	h = var_load( vn );
	
	for( i = 2 ; v2 = var_getn( hReq, i, &v1 ) ; i++ )
		{
		var_set( h, v1, v2 );
		}
	var_save( h, vn );
	
	return OK;
	}




//------------------------------------------------------------------------------
int cmd_system( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( !strncmp( cmd, "ssie7 ", 6 ) )
		{
		file_tmpname( &fname );
		file_save_str( fname, &(cmd[6]), -1 );
		strdcpy( &cmd1, "ssie7 @", fname, _ );
		}
	else
		{
		strdcpy( &cmd1, "", _ );
		for( i = 0 ; v = var_getn( hReq, i, &vn ) ; i++ )
			if ( stricmp( v, "true" ) ) strdcat( &cmd1, vn, "=", v, _ );
			else						strdcat( &cmd1, vn, " ", _ );
		}


	System( &w, cmd1 );
	ssie7print( t_work, w, _ );
	}



#ifdef OS_LINUX

//------------------------------------------------------------------------------
int cmd_fuser( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	// Essai de verrouiller le fichier
	
	if ( !var_getn( hReq, 1, &vn ) ) return ERROR;
	
	if ( !file_exists( vn ) ) return ERROR;
	fp = fopen( vn, "rb" );
	if ( !fp ) return ERROR;
	fclose( fp );
	
	h = file_lock( vn );
	close( h );
	}
	
#endif

/*

//------------------------------------------------------------------------------
int cmd_purge_log( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, dirlog, DIR_CHAR "*", _ );
	file_delete( fname );
	
	return OK;
	}
	

//------------------------------------------------------------------------------
int cmd_purge_cat( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, dirdat, DIR_CHAR "mftcat", _ );
	file_delete( fname );
	strdcat( &fname, DIR_CHAR "index", _ );
	mkdir( fname );
	
	return OK;
	}

*/














#include "ssie7lib.h"


// GESTION DES LOGIN

//------------------------------------------------------------------------------
int cmd_mftkey( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, dirdat, DIR_CHAR "mft_tmpkeys.dat", _ );
	hConf = var_load( fname );

	b = false;
	
	for( i = 1 ; v = var_getn( hReq, i, &vn ) ; i++ )
		{
		if ( strcmp( v, "true" ) )
			{
			var_set( hConf, vn, v );
			b = true;
			}
		else
			print( var_get( hConf, vn ), _ );	
		}

	if ( b )
		var_save( hConf, fname );
		
	if ( ( var_count( hReq ) == 1 )|| ( b ) )
		{
		print( "   ", _ );
		var_print( hConf, "," BR "   " );
		print( BR, _ );
		}

	return OK;
	}




//------------------------------------------------------------------------------
int cmd_srvnat_load( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, dirdat, DIR_CHAR "srvnat_certif.dat", _ );
	file_load( &s, fname );
	print( s, _ );
	return OK;
	}























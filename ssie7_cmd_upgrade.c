#include "ssie7.h"


//------------------------------------------------------------------------------
int cmd_upgrade( T_WORK *t_work )

// SERVICE DE MISE A JOUR DE VERSION
//------------------------------------------------------------------------------
	{
	if ( request_mode == MODE_TCP ) return ERROR;
	
	// VERIFIE LA PRESENCE DU FICHIER SSIE7UP.EXE ET RECENT DE MOINS DE 2 HEURES
	
	v = var_get( hSrvConf, "srvnat_list" );
	strdcpy( &fname, dirbin, DIR_CHAR "ssie7up" EXE, _ );
	if ( (( !file_exists( fname ) )||( (sysdate - file_date(fname)) > 2 Heures ))
		&&( ( !v )||( strcmp( v, "none" ) ) )
		)
		{
		strdcpy( &u4, cmd, _ );
		h3 = hReq;
		
		strdcpy( &cmd, "vdisk MFTCCWEX:ssie7upgrade fget " OS_NAME "/ssie7up" EXE " ", dirbin, " no_auth", _ );
		hReq = var_from_cmd( 0, cmd );
		
		cmd_vdisk_client( t_work );
		
		#ifdef OS_UNIX
		System( &s, strdcpy( &s1, "chmod 700 ", dirbin, DIR_CHAR "ssie7up", _ ) );
		#endif
		
		strdcpy( &cmd, u4, _ );
		var_free( hReq );
		hReq = h3;
		strdcpy( &fname, dirbin, DIR_CHAR "ssie7up" EXE, _ );
		print( BR, _ );
		}

		
	if ( file_exists( fname ) )
		{
		// TRANSMET LA COMMANDE
		var_getn( hReq, 1, &v1 );
		var_getn( hReq, 2, &v2 );
		var_getn( hReq, 3, &v3 );
		
		strdcpy( &cmd, fname, " ", v1, " ", v2, " ", v3, _ );
		
		Start( cmd );
		return OK;
		}
	
	print( "Aucune mise à jour disponible." BR BR, _ );
	return OK;
	}
	


//------------------------------------------------------------------------------
int cmd_uprc( T_WORK *t_work )

// SERVICE DE MISE A JOUR DE VERSION
//------------------------------------------------------------------------------
	{
	if ( request_mode == MODE_TCP ) return ERROR;
	
	// VERIFIE LA PRESENCE DU FICHIER SSIE7UP.EXE ET RECENT DE MOINS DE 2 HEURES
	
	v = var_get( hSrvConf, "srvnat_list" );
	strdcpy( &fname, dirbin, DIR_CHAR "ssie7uprc" EXE, _ );
/*
	if ( (( !file_exists( fname ) )||( (sysdate - file_date(fname)) > 2 Heures ))
		&&( ( !v )||( strcmp( v, "none" ) ) )
		)
*/
	if ( ( !v )||( strcmp( v, "none" ) ) )
		{
		strdcpy( &u4, cmd, _ );
		h3 = hReq;
		
		strdcpy( &cmd, "vdisk MFTCCWEX:ssie7upgrade fget " OS_NAME "/ssie7uprc" EXE " ", dirbin, " no_auth", _ );
//		strdcpy( &cmd, "vdisk MERSISUQ:ssie7upgrade fget " OS_NAME "/ssie7uprc" EXE " ", dirbin, " no_auth", _ );
		hReq = var_from_cmd( 0, cmd );
		
		cmd_vdisk_client( t_work );
		
		#ifdef OS_UNIX
		System( &s, strdcpy( &s1, "chmod 700 ", dirbin, DIR_CHAR "ssie7uprc", _ ) );
		#endif
		
		strdcpy( &cmd, u4, _ );
		var_free( hReq );
		hReq = h3;
		strdcpy( &fname, dirbin, DIR_CHAR "ssie7uprc" EXE, _ );
		print( BR, _ );
		}

		
	if ( file_exists( fname ) )
		{
		// TRANSMET LA COMMANDE
		var_getn( hReq, 1, &v1 );
		var_getn( hReq, 2, &v2 );
		var_getn( hReq, 3, &v3 );
		
		strdcpy( &cmd, fname, " ", v1, " ", v2, " ", v3, _ );
		
		Start( cmd );
		return OK;
		}
	
	print( "Aucune mise à jour disponible." BR BR, _ );
	return OK;
	}
	














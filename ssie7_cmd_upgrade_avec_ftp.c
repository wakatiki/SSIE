#include "ssie7.h"


//------------------------------------------------------------------------------
int cmd_upgrade( T_WORK *t_work )

// SERVICE DE MISE A JOUR DE VERSION
//------------------------------------------------------------------------------
	{
	if ( request_mode == MODE_TCP ) return ERROR;
	
	// VERIFIE LA PRESENCE DU FICHIER SSIE7UP.EXE ET RECENT DE MOINS DE 2 HEURES
	
	v = var_get( hSrvConf, "srvnat_list" );
//print("cmd_upgrade : ", "récupération de l'info de connexion à l'annuaire : '", v, "'.", BR, _ );
	if ( !v || ( v  && ( strcmp( v, "none" ) ) ) )
	{
//print("cmd_upgrade : ", "téléchargement du module 'ssie7up'.", BR, _ );
//	if (( !file_exists( fname ) )||( (time(NULL) - file_date(fname)) > (3*60*60) ))
		// TELECHARGE LE PROGRAMME DU SERVEUR D'ANNUAIRE
		strdcpy( &fname1, dirbin, _ );
		chdir( fname1 );
		strdcpy( &cmd, "ftp -n 10.151.29.82 721 << fin_FTP" BR
					"user ssiestore ssiestore" BR
					"verbose" BR
					"prompt -n" BR
					"cd bin/" OS_NAME BR
					"binary" BR
					"get ssie7up" EXE BR
					"quit" BR
					"fin_FTP" BR
                    , _	);	
       script_exec( cmd, 0, &s ); 

		#ifdef OS_UNIX
		System( &s, "chmod 700 ssie7up" );
		#endif
	}
	
	strdcpy( &fname, dirbin, DIR_CHAR "ssie7up" EXE, _ );
	if ( file_exists( fname ) )
	{
		// TRANSMET LA COMMANDE
		var_getn( hReq, 1, &v1 );
		var_getn( hReq, 2, &v2 );
		var_getn( hReq, 3, &v3 );
		
		strdcpy( &cmd, fname, " go ", v1, " ", v2, " ", v3, _ );

/*
		System( &s, cmd );
		print( s, BR, _ );
*/
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
//print("cmd_upgrade : ", "récupération de l'info de connexion à l'annuaire : '", v, "'.", BR, _ );
	if ( !v || ( v  && ( strcmp( v, "none" ) ) ) )
	{
//print("cmd_upgrade : ", "téléchargement du module 'ssie7uprc'.", BR, _ );
//	if (( !file_exists( fname ) )||( (time(NULL) - file_date(fname)) > (3*60*60) ))
		// TELECHARGE LE PROGRAMME DU SERVEUR D'ANNUAIRE
		strdcpy( &fname1, dirbin, _ );
		chdir( fname1 );

		strdcpy( &cmd, "ftp -n 10.151.29.82 721 << fin_FTP" BR
					"user ssiestore ssiestore" BR
					"verbose" BR
					"prompt -n" BR
					"cd bin/" OS_NAME BR
					"binary" BR
					"get ssie7uprc" EXE BR
					"quit" BR
					"fin_FTP" BR
                    , _	);	
       script_exec( cmd, 0, &s ); 

		#ifdef OS_UNIX
		System( &s, "chmod 700 ssie7uprc" );
		#endif
	}
	
	strdcpy( &fname, dirbin, DIR_CHAR "ssie7uprc" EXE, _ );
	if ( file_exists( fname ) )
	{
		// TRANSMET LA COMMANDE
		var_getn( hReq, 1, &v1 );
		var_getn( hReq, 2, &v2 );
		var_getn( hReq, 3, &v3 );
		
		strdcpy( &cmd, fname, " go ", v1, " ", v2, " ", v3, _ );

//		system( cmd );
		Start( cmd );
		return OK;
	}
	
	print( "Aucune mise à jour disponible." BR BR, _ );
	return OK;
	}
	














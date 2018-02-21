#include "ssie7lib.h"

// cmd_send

/*
#define bAscii				b1
#define bNocrypt			b2
#define bRecv				p6
#define cat_fname			fname3
#define log_fname			fname4
#define tran_id				(t_work->u1)
#define src_fname			(t_work->u3)
#define fpLog				fp2
*/

//------------------------------------------------------------------------------
int cmd_mft_server_send( T_WORK * t_work )

// TRAITEMENTS DES TRANSFERTS EN MODE PUBLISH
// Syntaxe : mft_server_send [partname]
//------------------------------------------------------------------------------
	{
	//----------------------------------------------------------------------
	if ( request_mode != MODE_TCP )
	//----------------------------------------------------------------------
		return ERROR;
	
	if ( ! session_partname )
		{
		print( "ERREUR RECEPTEUR : Vous devez vous identifier", _ );
		return ERROR;
		}
	
	v = var_getn( hReq, 1, &partname );
	if ( !v )
		{
		print( "ERREUR : syntaxe : mft_server_send [partname]", _ );
		return ERROR;
		}
	strdcpy( &part, partname, _ );

	
	//----------------------------------------------------------------------
	// RECHERCHE DES FICHIERS A ENVOYER
	//----------------------------------------------------------------------

/*	
	free( ssie7out );
	ssie7out = null;
	
	strdcpy( &fname, dirsend, DIR_CHAR "*.info", _ );
	tf = null;
	while( tf = file_list( tf, fname, "nodir absdir" ) )
		{
		var_free( hReq );
		strdcpy( &cat_fname, tf->dir, DIR_CHAR, tf->name, _ );
		hReq = var_load( cat_fname );
		if ( !var_get( hReq, "publish" ) ) continue;
		v = var_get( hReq, "status" );
		if ( ( !v )||( ( stricmp( v, "Publié" ) )&&( stricmp( v, "En cours" ) ) ) ) continue;
		v = var_get( hReq, "rpart" );
		if ( ( !v )||( stricmp( v, part ) ) ) continue;
		
		strdcpy( &ida, var_get( hReq, "ida" ), _ );
		logw( "cmd_mft_server_send : ", part, " : send_execute ", ida, _ );	
		bRecv = true;
//		cmd_mft_send_execute( t_work );
		}

//logw( "mft_server_send2", _ );
*/
	
	if (( hCnx )&&( hCnx != ERROR ))
		tcp_ssend( hCnx, "NO MORE FILE TO SEND", -1 );
		
/*	
	if (( hCnx )&&( hCnx != ERROR ))
		strdcpy( &ssie7out, "NO MORE FILE TO SEND", _ );
	else
		strdcpy( &ssie7out, "", _ );
*/		

	return OK;
	}


/*
#define repository			v7


//------------------------------------------------------------------------------
int cmd_recv_rfname_server( T_WORK * t_work )

//------------------------------------------------------------------------------
	{
	// TRAITEMENTS DU SERVEUR RECV
	// Syntaxe : recv_server [partname]
	
logw( "cmd_recv_rfname_server", _ );

	//----------------------------------------------------------------------
	if ( request_mode != MODE_TCP )
	//----------------------------------------------------------------------
		{
		return ERROR;
		}
	
	
	partname = session_partname;

	//----------------------------------------------------------------------
	// RECHERCHE LE FICHIER A ENVOYER
	//----------------------------------------------------------------------
	
logw( "cmd_recv_rfname_server1", _ );

	vn = var_get( hReq, "rfname" );
	
	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, upper(partname), ".dat", _ );
	var_free( hPart );
	hPart = var_load( fname );
	repository = var_get( hPart, "repository" );
logw( "cmd_recv_rfname_server2:", repository, _ );
/*
	if ( ( !repository )||( part_get_fileaccess( t_work ) == false ) )
		{
		print( "ERREUR RFNAME : vous n'avez pas les droits", _ );
		return ERROR;
		}
* /

	bAscii		= ( var_get( hReq, "ascii" ) ) ? true : false;
	bNocrypt	= ( var_get( hReq, "nocrypt" ) ) ? true : false;
//	bNocrypt	= true;

if ( bAscii )	
logw( "cmd_recv_rfname_server3:mode ascii", _ );
	
	//----------------------------------------------------------------------
	// VERIFICATION DU FICHIER A ENVOYER
	//----------------------------------------------------------------------
	
logw( "cmd_recv_rfname_server4:", fname, _ );

	if ( !file_exists( fname ) )
		{
		strdcpy( &s, "ERREUR RECV : le fichier demandé n'existe pas : ", fname, _ );
		print( s, _ );
		return ERROR;
		}
	

	//------------------------------------------------------------------
	// OUVRE LE FICHIER A ENVOYER
	//------------------------------------------------------------------
	
	fp = fopen( fname, "rb" );
	if ( !fp )
		{
		strdcpy( &s, "ERREUR RECV : Problème de lecture du fichier : ", fname, _ );
		print( s, _ );
		return ERROR;
		}
		
logw( "cmd_recv_rfname_server5", _ );
	
	tcp_ssend( hCnx, "OK", -1 );
	
	//------------------------------------------------------------------
	// ENVOI LES DONNEES
	//------------------------------------------------------------------
	
	
	s = realloc( s, 128 Ko );
	
	while( n = fread( fp, s, 96 Ko ) )
		{
		#ifdef OS_WINDOWS

if ( bAscii )
logw( "cmd_recv_rfname_server:mode ascii", _ );

		if ( ( bAscii )&&( s[n] == '\r' ) )
			{
			// VERIFIE QUE \r\n sont bien réunis à la fin du bloc
			// CHARGE LE \n qui manque peut-être
			if ( fread( fp, &(s[n+1]), 1 ) )
				n++;
			}
		#endif

if ( bNocrypt )
logw( "cmd_recv_rfname_server:mode nocrypt", _ );

		if ( bNocrypt ) r = tcp_ssend0( hCnx, s, n ); else r = tcp_ssend( hCnx, s, n );
		
		if ( r == ERROR )
			{
			tcp_disconnect( hCnx ); hCnx = ERROR;
			return ERROR;
			}

		}
	
	fclose( fp );
	
	
	//------------------------------------------------------------------
	// FIN ENVOI
		
	if ( bNocrypt ) tcp_ssend0( hCnx, "FIN", 3 ); else tcp_ssend( hCnx, "FIN", 3 );
	tcp_srecv( hCnx, &s1 );
	if ( stripos( s1, "ERREUR", 0 ) > 0 )
		{
		strdcpy( &s, "ERREUR DISTANT : ", s1, _ );
		logw( "recv_rfname_server : ", fname, " : ", s, _ );
		return ERROR;
		}
		
	strdcpy( &s, "Transfert terminé", _ );
	logw( "recv_rfname_server : ", fname, " : ", s, _ );

	return OK;
	}
*/


/*
#undef bAscii
#undef bNocrypt
#undef bRecv
#undef cat_fname
#undef log_fname
#undef tran_id
#undef src_fname
#undef fpLog
*/





















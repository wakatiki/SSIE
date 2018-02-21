#include "ssie7lib.h"

#define bRecv				p6

//------------------------------------------------------------------------------
int cmd_mft_recv( T_WORK * t_work )

// COMMANDE RECV (Partie du recv exécuté coté client ou initiateur du transfert)
//------------------------------------------------------------------------------
	{
	int init_mode = request_mode, nb = 0;
	
	#ifdef OS_UNIX
	if (( request_mode == MODE_TCP )&&( strcmp( ip_cli, "127.0.0.1" ) ))
		{
		print( "ERREUR : commande non autorisée" BR, _ );
		return ERROR;
		}
	#endif
	
	strdcpy( &fname, dirwrk, DIR_CHAR "LockServer", _ );
	for( ; file_exists( fname ) ; sleep(1) );

	if ( request_mode == MODE_TCP )
		{
		if ( ! session_partname )
			{
			print( "ERREUR RECEPTEUR : Vous devez vous identifier", _ );
			return ERROR;
			}
		
		if ( !mft_exists() )
			{
			print( "ERREUR RECEPTEUR : Ce serveur n'est une passerelle MFT", _ );
			return ERROR;
			}
		
		
		upper( partname = var_get( hReq, "spart" ) );
		if ( !partname )
			{
			print( "ERREUR : syntaxe : mft recv spart=[partname|adl]", _ );
			return ERROR;
			}
		
		strdcpy( &cmd, argv[0], " mft super_recv rpart=", session_partname,
									",spart=", partname, _ );
		System( &s, cmd );
		print( cmd, BR, s, BR, _ );
		
		return OK;
		}
	
	if ( ! is_authorized( "is_admin", null ) )
		{
		print( "ERREUR : Commande non autorisée." BR, _ );
		return ERROR;
		}
		
	//--------------------------------------------------------------------------
	// CHARGE LES INFO DU PARTENAIRE
	//--------------------------------------------------------------------------
		
	upper( partname = var_get( hReq, "part" ) );
	if ( !partname )
		{
		print( "ERREUR : syntaxe : mft recv part=[partname|adl]", _ );
		return ERROR;
		}
	
	if ( ! part_exists( t_work ) ) // retourne dans fname le nom du fichier partenaire
		{
		print( "ERREUR : le partenaire ", partname, " est inconnu.", _ );
		return ERROR;
		}
	
	var_free( hPart );
	hPart = var_load( fname );
	
	
	//--------------------------------------------------------------------------
	// SI LE PARTENAIRE EST EN RECEPTION UNIQUEMENT (PAS POSSIBLE DE FAIRE DES RECV)
	//--------------------------------------------------------------------------

	v = var_get( hPart, "trf_direction" );
	if ( ( v )&&( !stricmp( v, "recv" ) ) )
		{
		print( "ERREUR : le partenaire ", partname, " n'est pas joignable en directe", _ );
		return ERROR;
		}
	

	//--------------------------------------------------------------------------
	// VERIFIE SI LE DESTINATAIRE EST EN SSIE v7
	// Non => Demande un mft recv sur la passerelle
	//--------------------------------------------------------------------------
	
	v = var_get( hPart, "product_name" );

	if ( ( !v )||( strnicmp( v, "SSIE v7", 7 ) ) )
		{
/*
		if ( mft_exists() )
			{
			strdcpy( &cmd, argv[0], " mft recv part=", partname, ",idf=*,file=all", _ );
			System( &s, cmd );
			print( cmd, BR, s, BR, _ );
			}
		else
*/
			{
			print( "ERREUR : Le partenaire doit être en version SSIE v7", _ );
			}
		
		return OK;
		}
	

	//--------------------------------------------------------------------------
	// CONNEXION
	//--------------------------------------------------------------------------
	
	if ( ssie7connect( t_work, false ) == ERROR )
		{
		// PROBLEME DE CONNEXION
		
		print( "ERREUR : Problème de connexion à ", partname, _ );
		return ERROR;
		}
	

tr( "cmd_mft_recv1" );

	//--------------------------------------------------------------------------
	// ENVOI LA COMMANDE MFT SERVER_SEND
	//--------------------------------------------------------------------------
	
/*
	v = var_get( hPart, "product_name" );
	if ( !strnicmp( v, "SSIE v7.2", 9 ) )
		strdcpy( &cmd, "mft send_part ", var_get( hSrvConf, "partname" ), _ );
	else
*/
		strdcpy( &cmd, "mft_server_send ", var_get( hSrvConf, "partname" ), _ );
	
	if ( tcp_ssend( hCnx, cmd, -1 ) == ERROR )
		{
		print( "ERREUR : Connexion perdue (1)", _ );
		logw( "mft recv part=", partname, " : ERREUR : Connexion perdue (1)", _ );
		return ERROR;
		}
		
	
	
	//----------------------------------------------------------------------
	// ATTEND ET TRAITE LES REQUETES
	//----------------------------------------------------------------------
	
	request_mode = MODE_TCP;
	p = strpos( session_ipport, ":", 0 );
	strdcpy( &ip_cli, substr( session_ipport, 0, p ), _ );
	
	for( ; ; )
		{
tr( "cmd_mft_recv2" );

		if ( ( hCnx == ERROR )||( !hCnx ) )
			{
tr( "cmd_mft_recv3" );
			ssie7disconnect( t_work );
			if ( ssie7out ) free( ssie7out );
			ssie7out = null;
			print( "ERREUR : Connexion perdue (2)", _ );
			logw( "mft recv part=", partname, " : ERREUR : Connexion perdue (2)", _ );
			request_mode = init_mode;
			return ERROR;
			}
		if ( ( n = tcp_srecv( hCnx, &cmd ) ) == ERROR )
			{
tr( "cmd_mft_recv4" );
			ssie7disconnect( t_work );
			if ( ssie7out ) free( ssie7out );
			ssie7out = null;
			hCnx = ERROR;
			print( "ERREUR : Connexion perdue (3)", _ );
			logw( "mft recv part=", partname, " : ERREUR : Connexion perdue (3)", _ );
			request_mode = init_mode;
			return OK;
			}
				
tr( "cmd_mft_recv5" );
		cmd[n] = 0;
		if ( (trim(cmd))[0] == 0 ) continue;
		if ( stripos( cmd, "ERREUR", 0 ) == 0 )
			{
tr( "cmd_mft_recv6" );
			ssie7disconnect( t_work );
			if ( ssie7out ) free( ssie7out );
			ssie7out = null;
			hCnx = ERROR;
			strd_replace( &cmd, "ERREUR", "ERREUR DISTANT" );
			print( cmd, _ );
			logw( "mft recv ", partname, " : ", cmd, _ );
			request_mode = init_mode;
			return OK;
			}
			
tr2( "cmd_mft_recv7:", cmd );

tr( "cmd_mft_recv8" );

		if ( !strcmp( cmd, "NO MORE FILE TO SEND" ) )
			{
			ssie7disconnect( t_work );
			if ( ssie7out ) free( ssie7out );
			ssie7out = null;
			hCnx = ERROR;
			strdcpy( &s, itoa( nb ), " Fichier",
			(char*)((nb>1)?"s":""), " reçu", (char*)((nb>1)?"s":""), _ );
			print( s, BR, _ );
			logw( "mft recv part=", partname, " : ", s, _ );
			request_mode = init_mode;
			return OK;
			}
		
		nb++;
tr( "cmd_mft_recv9" );
		
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd );
		if ( !hReq )
			{
			tcp_ssend( hCnx, "ERREUR de syntaxe", -1 );
			continue;
			}

tr( "cmd_mft_recv10" );

		var_getn( hReq, 0, &what );
		
		if ( v = var_get( hReq, "user" ) )
			{
			strdcpy( &user, v, _ );
			var_delete( hReq, "user" );
			}
		else
		if ( user )
			user[0] = 0;
		else
			strdcpy( &user, "", _ );
		
		
		// TRAITEMENT DE LA REQUETE
		
		if ( ssie7out ) ssie7out[0] = 0;
		else	
			strdcpy( &ssie7out, "", _ );
		
tr( "cmd_mft_recv11" );

		ssie7_cmd( t_work );
		

tr( "cmd_mft_recv12" );
		
		// ENVOI LA REPONSE
		
//		if ( ssie7out[0] )
//			tcp_ssend( hCnx, ssie7out, -1 );


		ssie7free( t_work );
		}
	
	return OK;
	}



#undef bRecv























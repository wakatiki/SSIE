#include "ssie7lib.h"




#define src_fname			v7
#define dest_fname			v6
#define dest_is_dir			b
#define bAscii				b1

//------------------------------------------------------------------------------
int cmd_getfile( T_WORK * t_work )

// COMMANDE GETFILE
//------------------------------------------------------------------------------
	{
	if ( request_mode != MODE_TCP )
		{
		// TRAITEMENTS DU CLIENT GETFILE
		// Syntaxe : getfile [partname] [fname] [dest=] [binary/ascii]
		
		// LECTURE DES PARAMETRES
		
		partname = null;
		v = var_getn( hReq, 1, &partname );
		if ( !v )
			{
			print( "ERREUR : syntaxe : getfile [partname] [fname] [dest=...] [binary/ascii]", _ );
			return ERROR;
			}
		
		if ( part_exists( t_work ) )
			{
			v = var_getn( hReq, 2, &src_fname );
			}
		else
			{
			src_fname = partname;
			partname = null;
			}
		
		bAscii		= ( var_get( hReq, "ascii" ) ) ? true : false;
		
		
				
		// PREPARE LA DESTINATION
		
		dest_fname	= var_get( hReq, "dest" );
		if ( !dest_fname )
			strdcpy( &fname3, "." DIR_CHAR, _ );
		else
			{
			strdcpy( &fname3, dest_fname, _ );
			if ( ( fname3[n=(strlen(fname3)-1)] == '/' )||( fname3[n] == '\\' ) )
				fname3[n] = 0;
			if ( dir_exists( fname3 ) )
				strdcat( &fname3, DIR_CHAR, _ );
			else
			if ( !dir_exists( file_getdir(fname3) ) )
				{
				print( "ERREUR : La destination n'est pas valide", BR, _ );
				return ERROR;
				}
			}

		dest_is_dir = (( fname3[n=(strlen( fname3 )-1)] == '/' )||( fname3[n] == '\\' ));
		
		
		
		// CONNEXION AU DISTANT
		
		if ( ssie7connect( t_work ) == ERROR )
			{
			print( error_msg, BR, _ );		
			return ERROR;
			}
		
		
		
		// ENVOI LA REQUETE GETFILE AU DISTANT
		
		strdcpy( &cmd, "getfile ", quote(src_fname), _ );
		
		if ( bAscii )
			{
			#ifdef OS_WINDOWS
			strdcat( &cmd, " windows", _ );
			#else
			strdcat( &cmd, " unix", _ );
			#endif
			}
		
		if ( tcp_ssend( hCnx, cmd, -1 ) == ERROR )
			{
			print( "ERREUR : Connexion perdue (1)", BR, _ );
			ssie7disconnect( t_work );
			return ERROR;
			}
		
		
		// ATTEND LA LISTE DES FICHIERS A RECEVOIR OU REFUS DE LA DEMANDE
			
		if ( tcp_srecv( hCnx, &s ) == ERROR )
			{
			print( "ERREUR : Connexion perdue (2)", BR, _ );
			ssie7disconnect( t_work );
			return ERROR;
			}
		
		if ( ! strnicmp( s, "ERREUR", 6 ) )
			{
			print( s, _ );
			ssie7disconnect( t_work );
			return ERROR;
			}

		ts = str_explode( s, "|" );
		
		
		// BOUCLE DE RECEPTION
		
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ts[i][0] == 0 ) continue;
			
			// CALCUL LE NOM DU FICHIER DE RECEPTION ET L'OUVRE EN CREATION
			
			if ( dest_is_dir )
				strdcpy( &fname1, fname3, file_getname( ts[i] ), _ );
			else
				strdcpy( &fname1, fname3, _ );
			
			fp = fopen( fname1, "wb" );
			if ( fp == NULL )
				{
				print( "ERREUR : Pb création du fichier ", fname1, BR, _ );
				ssie7disconnect( t_work );
				return ERROR;
				}
			
			print( "reçois ", fname_verify(fname1), " : ", _ );
			
			
			// DEMANDE LE FICHIER
				
			if ( tcp_ssend( hCnx, ts[i], -1 ) == ERROR )
				{
				print( "ERREUR : Connexion perdue (3)", BR, _ );
				ssie7disconnect( t_work );
				fclose( fp );
				return ERROR;
				}
			
			
			// ATTEND L'ACCORD

			if ( tcp_srecv( hCnx, &s ) == ERROR )
				{
				print( "ERREUR : Connexion perdue (4)", BR, _ );
				ssie7disconnect( t_work );
				fclose( fp );
				return ERROR;
				}
			
			if ( stripos( s, "ERREUR", 0 ) >= 0 )
				{
				print( s, _ );
				ssie7disconnect( t_work );
				fclose( fp );
				return ERROR;
				}
			
			
			// RECEPTION DES DONNEES
			
			strdcpy( &s1, "FIN ", ts[i], _ );
			
			for( ; ; )
				{
				if ( ( n = tcp_srecv( hCnx, &s ) ) == ERROR )
					{
					fclose( fp );
					print( "ERREUR : connexion perdue (5)", _ );
					ssie7disconnect( t_work );
					return ERROR; // Connexion perdue
					}
				if ( ! strcmp( s, s1 ) ) break;
				
				fwrite( fp, s, n, _ );
				if ( c != time(NULL) )
					{
					fflush( fp );
					c = time(NULL);
					}
				}
					
			fclose( fp );
			
			if ( tcp_ssend( hCnx, "OK", 2 ) == ERROR )
				{
				print( "ERREUR : Connexion perdue (6)", BR, _ );
				ssie7disconnect( t_work );
				return ERROR;
				}
					
			print( "OK" BR, _ );
			}

		if ( tcp_ssend( hCnx, "FIN GETFILE", 11 ) == ERROR )
			{
			print( "ERREUR : Connexion perdue (6)", BR, _ );
			ssie7disconnect( t_work );
			return ERROR;
			}
				
		ssie7disconnect( t_work );
		print( "OK", BR, _ );
		return OK;
		}
	
	
	
	//--------------------------------------------------------------------------
	else	// TRAITEMENT DU SERVEUR GETFILE
	//--------------------------------------------------------------------------
		{
		// Syntaxe : getfile [fname], windows/unix uniquement pour convertion, ...
		
//		if ( send_private_port( t_work ) ) return OK;

trace( "getfile : ", cmd, _ );

		if ( ! session_partname )
			{
			print( "ERREUR RECEPTEUR : Vous devez vous identifier", _ );
			return ERROR;
			}
		
		
		// CONSTRUIT LA LISTE DES FICHIERS ET L'ENVOI
		
trace( "getfile1", _ );
		strdcpy( &fname3, "", _ );
		v = var_getn( hReq, 1, &v1 );
		if ( !v )
			{
			print( "ERREUR : fname non fourni", _ );
			return ERROR;
			}

		if ( v1[0] == '/' )		
			strdcpy( &fname1, session_files_dir, &(v1[1]), _ );
		else
			strdcpy( &fname1, session_files_dir, v1, _ );


trace( "getfile2", _ );
		
		n = strlen( session_files_dir );
		tf = null;
		while( tf = file_list( tf, fname1, "absdir nodir nohide" ) )
			{
			strdcpy( &fname2, tf->dir, DIR_CHAR, tf->name, _ );
			strdcat( &fname3, &(fname2[n]), "|", _ );
			}
		
		if ( fname3[0] == 0 ) return OK; // Pas de fichier
		print( fname3, __ );
		
		
		// BOUCLE D'ENVOI DES FICHIERS
		#ifdef OS_WINDOWS
		bAscii = ( var_get( hReq, "unix" ) ) ? true : false;
		#else // OS_UNIX
		bAscii = ( var_get( hReq, "windows" ) ) ? true : false;
		#endif

trace( "getfile3", _ );
		
		for( ; ; )
			{
			// ATTEND LE NOM DU FICHIER A ENVOYER
			
			if ( tcp_srecv( hCnx, &s ) == ERROR )
				{
trace( "getfile3a", _ );
//				print( "ERREUR : Connexion perdue (4)", BR, _ );
				return ERROR;
				}
			
trace( "getfile:", s, _ );
			if ( !strcmp( s, "FIN GETFILE" ) )
{trace( "FIN GETFILE", _ );
				return OK;
}				
			strdcpy( &fname1, session_files_dir, s, _ );
			
			if ( ! file_exists( fname1 ) )
				{
trace( "getfile3b", _ );
				print( "ERREUR ENVOYEUR : Le fichier demandé n'existe pas : ", s, _ );
				return ERROR;
				}
				
			// OUVRE LE FICHIER A ENVOYER
			
			fp = fopen( fname1, "rb" );
			if ( fp == NULL )
				{
trace( "getfile3c", _ );
				print( "ERREUR ENVOYEUR: Pb ouverture du fichier : ", s, _ );
				return ERROR;
				}
			
			
			// ENVOI L'ACCORD
				
			print( "OK", __ );
trace( "getfile4", _ );

				
			// ENVOI LES DONNEES
			
			strdcpy( &s1, "FIN ", s, _ );
			
			s = realloc( s, 5000 );
			
			while( n = fread( fp, s, 4096 ) )
				{
				if ( bAscii )
					{
					// CONSERVIONS ASCII
					#ifdef OS_WINDOWS
					strd_replace( &s, BR, "\n" );
					#else // OS_UNIX
					strd_replace( &s, BR, "\r\n" );
					#endif

					n = strlen( s );
					}
					
				if ( tcp_ssend( hCnx, s, n ) == ERROR )
					{
trace( "getfile3d", _ );
					return ERROR;
					}
				
				if ( b ) s = realloc( s, 5000 );
				}

trace( "getfile5", _ );

			if ( tcp_ssend( hCnx, s1, -1 ) == ERROR )	// ENVOI MARQUE DE FIN
				{
trace( "getfile5a", _ );
				return ERROR;
				}

			fclose( fp );
			tcp_srecv( hCnx, &s );		// ATTEND RETOUR
			}
		}
	}




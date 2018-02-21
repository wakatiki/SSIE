#include "ssie7lib.h"


// cmd_putfile

#define src_fname			v7
#define dest_fname			v6
#define bAscii				b

//------------------------------------------------------------------------------
int cmd_putfile( T_WORK * t_work )

// COMMANDE PUTFILE
//------------------------------------------------------------------------------
	{

	if ( request_mode != MODE_TCP )
		{
		// TRAITEMENTS DU CLIENT PUTFILE
		// Syntaxe : putfile [partname] [fname] [dest=...] [binary/ascii]
		
		
		// LECTURE DES PARAMETRES
		
		partname = null;
		v = var_getn( hReq, 1, &partname );
		if ( !v )
			{
			print( "ERREUR : syntaxe : putfile [partname] [src_fname] [dest=...] [binary/ascii]", _ );
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
		
		if ( strpos( src_fname, "*", 0 ) < 0 )
			{
			if ( !file_exists( src_fname ) )
				{
				print( "ERREUR : le fichier à envoyer n'existe pas : ", src_fname, _ );
				return ERROR;
				}
			}
		else
		if ( file_count( src_fname ) <= 0 )
			{
			print( "ERREUR : aucun fichier à envoyer : ", src_fname, _ );
			return ERROR;
			}

		dest_fname	= var_get( hReq, "dest" );
		bAscii		= ( var_get( hReq, "ascii" ) ) ? true : false;
		
				
		// CONNEXION AU DISTANT
		
		if ( ssie7connect( t_work ) == ERROR )
			{
			print( error_msg, BR, _ );		
			return ERROR;
			}

		
		// BOUCLE D'ENVOI
		
		tf = NULL;
		while( tf = file_list( tf, src_fname, "nodir absdir nohide" ) )
			{
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			print( "envoi ", fname_verify(fname), " : ", _ );
				
			// OUVRE LE FICHIER A ENVOYER
			
			fp = fopen( fname, "rb" );
			if ( !fp )
				{
				print( "ERREUR : Pb lecture du fichier : ", fname, BR, _ );
				ssie7disconnect( t_work );
				return ERROR;
				}
				
				
			// PREPARE LA REQUETE
			
			strdcpy( &cmd,
				"putfile fname=", quote(v = file_getname( fname )),	" dest=", quote( dest_fname ),
				_ );

			strdcpy( &s1, "FIN ", v, _ );
			
				
			if ( bAscii )
				{
				#ifdef OS_WINDOWS
				strdcat( &cmd, " windows", _ );
				#else
				strdcat( &cmd, " unix", _ );
				#endif
				}
			
			// SOUMET LA REQUETE

			if ( tcp_ssend( hCnx, cmd, -1 ) == ERROR )
				{
				print( "ERREUR : Connexion perdue (1)", BR, _ );
				ssie7disconnect( t_work );
				return ERROR;
				}
			
			
			// ATTEND ACCEPTATION OU REFUS
				
			if ( tcp_srecv( hCnx, &s ) == ERROR )
				{
				print( "ERREUR : Connexion perdue (2)", BR, _ );
				ssie7disconnect( t_work );
				return ERROR;
				}
			
			if ( strcmp( s, "OK" ) )
				{
				print( s, _ );
				ssie7disconnect( t_work );
				return ERROR;
				}
	
			
			// ENVOI LES DONNEES
			
			s = realloc( s, 5000 );
			
			while( n = fread( fp, s, 4096 ) )
				{
				#ifdef OS_WINDOWS
				if ( ( bAscii )&&( s[n] == '\r' ) )
					{
					// VERIFIE QUE \r\n sont bien réunis à la fin du bloc
					// CHARGE LE \n qui manque peut-être
					if ( fread( fp, &(s[n+1]), 1 ) )
						n++;
					}
				#endif
				tcp_ssend( hCnx, s, n );
				}
			
			fclose( fp );
			
			tcp_ssend( hCnx, s1, -1 );
	
			tcp_srecv( hCnx, &s );
			if ( stripos( s, "ERREUR", 0 ) > 0 )
				{
				print( s, _ );
				return ERROR;
				}
				
			print( "OK" BR, _ );
			}

//		tcp_ssend( hCnx, "FIN PUTFILE", -1 );

		ssie7disconnect( t_work );
		
		print( "OK", BR, _ );
		return OK;
		}



	//--------------------------------------------------------------------------
	else	// TRAITEMENT DU SERVEUR PUTFILE
	//--------------------------------------------------------------------------
		{
		
		// Syntaxe : putfile fname=..., dest=..., windows/unix uniquement pour convertion, ...
		
//		if ( send_private_port( t_work ) ) return OK;

		if ( ! session_partname )
			{
			print( "ERREUR RECEPTEUR : Vous devez vous identifier", _ );
			return ERROR;
			}
		
		
		// LECTURE DES PARAMETRES
		
		#ifdef OS_WINDOWS
		bAscii = ( var_get( hReq, "unix" ) ) ? true : false;
		#else // OS_UNIX
		bAscii = ( var_get( hReq, "windows" ) ) ? true : false;
		#endif
		
		if ( ! (v1 = var_get( hReq, "fname" )) )
			{
			print( "ERREUR RECEPTEUR : fname non fourni", _ );
			return ERROR;
			}
		
		if ( ! (v2 = var_get( hReq, "dest" )) )
			{
			print( "ERREUR RECEPTEUR : dest non fourni", _ );
			return ERROR;
			}
			
		if (( v2[0] == '/' )||( v2[0] == '\\' ))
			strdcpy( &fname1, session_files_dir, &(v2[1]), _ );
		else
			strdcpy( &fname1, session_files_dir, v2, _ );
		
		if ( ( fname1[n=(strlen(fname1)-1)] == '/' )||( fname1[n] == '\\' ) )
			fname1[n] = 0;
		
		if ( dir_exists( fname1 ) )
			{
			strdcat( &fname1, DIR_CHAR, v1, _ );
			}
		else
		if ( !dir_exists( file_getdir( fname1 ) ) )
			{
			print( "ERREUR RECEPTEUR : La destination n'est pas valide", _ );
			return ERROR;
			}
		
		
		// PREPARE LE FICHIER DE RECEPTION DES DONNES
		
		strdcpy( &fname2, fname1, ".putfile.tmp", _ );
		fp = fopen( fname2, "wb" );
		if ( !fp )
			{
			print( "ERREUR RECEPTEUR : Pb création du fichier de réception", _ );
			logw( ip_cli, " : ", session_partname, " : putfile : ERREUR : Pb création fichier : ", fname2, _ );
			return ERROR;
			}
		
		
		// ACCEPTE LE TRANSFERT
		
		print( "OK", __ );
		
		
		// RECEPTION DES DONNEES
		
		strdcpy( &s1, "FIN ", v1, _ );
		
		c = time(NULL);
		for( ; ; )
			{
			if ( ( n = tcp_srecv( hCnx, &s ) ) == ERROR )
				{
				fclose( fp );
				logw( ip_cli, " : ", session_partname, " : putfile : ERREUR : connexion perdue", _ );
				return ERROR; // Connexion perdue
				}
			if ( ! strcmp( s, s1 ) ) break;
			
			if ( bAscii )
				{
				// CONSERVIONS ASCII
				#ifdef OS_WINDOWS
				strd_replace( &s, "\n", BR );
				#else // OS_UNIX
				strd_replace( &s, "\r\n", BR );
				#endif

				n = strlen( s );
				}
				
			fwrite( fp, s, n, _ );
			if ( c != time(NULL) )
				{
				fflush( fp );
				c = time(NULL);
				}
			}
			
		fclose( fp );
		
		
		// RENOMME LE FICHIER DES DONNEES
		
		unlink( fname1 );	
		file_move( fname2, fname1 );
		if ( ! file_exists( fname1 ) )
			{
			print( "ERREUR RECEPTEUR : Pb renommage du fichier de réception", _ );
			logw( ip_cli, " : ", session_partname, " : putfile : ERREUR : Pb renommage fichier : ", fname2, _ );
			return ERROR;
			}
		
		print( "OK", _ );
			
		return OK;
		}
	}






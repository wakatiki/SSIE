//#include "ssie7lib.h"
//#include "mft.h"

extern FILE *fpd;
	
	
//------------------------------------------------------------------------------
int robot_run( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	if ( argc > 2 )
		{
		if ( !stricmp( argv[2], "mft_to_ssie" ) )
			return robot_mft_to_ssie( t_work );
		if ( !stricmp( argv[2], "ssie_to_mft" ) )
			return robot_ssie_to_mft( t_work );
		return;			
		}
	
	if ( process_count( "mft robot" ) > 1 )
		exit(0);
			
	strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot.pid", _ );
	file_save_str( fname, itoa(getpid()), -1 );
	
	// DEMARRE ROBOT MFT_TO_SSIE
	
	strdcpy( &cmd, argv[0], " robot mft_to_ssie", _ );
	p5 = StartH( cmd );
	
	
	// DEMARRE ROBOT SSIE_TO_MFT

	strdcpy( &cmd, argv[0], " robot ssie_to_mft", _ );
	p4 = StartH( cmd );
	
	
	// SURVEILLANCE DES ROBOTS ET DE MFT
	
	for( sleep(20), n5 = 0 ; ; sleep(20), n5++ )
		{
		// SURVEILLE EXISTANCE PROCESS ROBOT MFT_TO_SSIE
		if ( ! process_is_alive( p5 ) )
			{
			p5 = StartH( strdcpy( &cmd, argv[0], " robot mft_to_ssie", _ ) );
//			logw("MFT ROBOT : robot mft_to_ssie anormalement arrêté => Redémarrage", _ );
			}
		
		
		// SURVEILLE PROCESS ROBOT MFT_TO_SSIE ACTIF
		strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot_mft_to_ssie.pid", _ );
		if ( (sysdate - file_date( fname )) >= (3 Minutes) )
			{
			if ( file_load( &s, fname ) == sERROR ) continue;
			process_kill( atoi( s ) );
			p5 = StartH( strdcpy( &cmd, argv[0], " robot mft_to_ssie", _ ) );
			logw( "MFT ROBOT : robot mft_to_ssie : semble bloqué => kill et restart", _ );
			}
		

		// SURVEILLE PROCESS ROBOT SSIE_TO_MFT
		if ( ! process_is_alive( p4 ) )
			{
			p4 = StartH( strdcpy( &cmd, argv[0], " robot ssie_to_mft", _ ) );
//			logw("MFT ROBOT : robot ssie_to_mft anormalement arrêté => Redémarrage", _ );
			}
		
		
		// SURVEILLE PROCESS ROBOT SSIE_TO_MFT ACTIF
		strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot_ssie_to_mft.pid", _ );
		if ( (sysdate - file_date( fname )) >= (3 Minutes) )
			{
			if ( file_load( &s, fname ) == sERROR ) continue;
			process_kill( atoi( s ) );
			p5 = StartH( strdcpy( &cmd, argv[0], " robot ssie_to_mft", _ ) );
			logw( "MFT ROBOT : robot ssie_to_mft : semble bloqué => kill et restart", _ );
			}
		
		
		// SURVEILLE PROCESS MFT
		if ( ( ! process_count( "MFTTCPS" ) )||( ! process_count( "MFTMAIN" EXE ) ) )
			{
			logw( "MFT ROBOT : MFT est anormalement arrêté => Redémarrage", _ );
			strdcpy( &cmd, argv[0], " restart", _ );
			System( &s, cmd );
//			logw( "MFT ROBOT : mft restart : ", s, _ );
			
			#ifdef OS_UNIX
			if ( v1 = var_get( hSrvConf, "adm_email" ) )
				{
				strdcpy( &cmd, "echo \"\" | mail -s \"", itod( 0, "jj/mm/aaaa hh:mm:ss" ),
						" : ALERTE MFT ARRETE : $HOSTNAME ==> Redemarrage\" ", v1, _
						 );
			
				system( cmd );
				}
			#endif
			}
		
		
		
		if ( ! ( (n5+1) % 3 ) ) // Toutes les minutes
			{
			// VERIFIE LES PARTENAIRES DISTANTS NON CONFIGURE EN LOCAL
			
			for( ; ; ) // Astuce de programmation pour ne pas avoir trop d'indentation du code
				{
				v = var_get( hSrvConf, "part_auto_crea" );
				if ( ( !v )||( stricmp( v, "Oui" ) ) ) break;
				
				// CHARGE LA LOG MFT
				if ( file_load( &s,
						strdcpy( &fname1, dirdat, DIR_CHAR "mft" DIR_CHAR "log"
								DIR_CHAR "mft.log", _ ) ) == sERROR )
					break;
					
				p0 = 0;
				while( ( p = strpos( s, "MFTT15E ", p0 ) ) >= 0 )
//				while( ( p = strpos( s, "MFTH22E ", p0 ) ) >= 0 )
					{
					p0 = strpos( s, "\n", p+8 );
					if ( p0 < 0 ) break;
					p1 = strpos( s, "NPART=", p+8 );
					if ( ( p1 >= 0 )&&( p1 < p0 ) )
						{
						partname = trim( substr( s, p1+6, 8 ) );
						strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, partname, ".dat", _ );
						if ( file_exists( fname ) ) continue;
						strdcpy( &cmd, dirbin, DIR_CHAR "ssie7" EXE " part ", partname, " loadnat", _ );
						System( &s, cmd );
						logw( "ROBOT MFT : ", cmd, BR, s, _ );
						}
					}
					
				break;
				}
			}
		
		
		if ( ! ( (n5+1) % 9 ) ) // TOUTES LES 3 MINUTES
			{
			// DECLENCHE LES TRANSFERTS LES FICHIERS EN ATTENTE
			// CHEZ LES PARTENAIRES EN EMISSION UNIQUEMENT
			
			strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR "*.send", _ );
			tf = NULL;
			while( tf = file_list( tf, fname, "nodir absdir" ) )
				{
				strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
				strd_replace( &fname, ".send", ".dat" );
				var_free( hPart );
				hPart = var_load( fname );
				
				// Retient les partenaires avec trf_direction=send
				v = var_get( hPart, "trf_direction" );
				if ( ( !v )||( strcmp( v, "send" ) ) ) continue;
				v = var_get( hPart, "enable" );
				if ( ( !v )||( strcmp( v, "Oui" ) ) ) continue;
				v = var_get( hPart, "product_name" );
				
				// Partenaire non MFT ??
				if ( ( v )&&( !strnicmp( v, "SSIE v7", 7 ) ) ) continue;
				
				strdcpy( &cmd, argv[0], " recv part=", var_get( hPart, "partname" ),
						", idf=*,file=all", _ );
				system( cmd );
				strdcpy( &cmd, argv[0], " delete direct=recv,state=H,part=", var_get( hPart, "partname" ), _ );
				system( cmd );
				}
			}
		
		
		if ( !( (n5+1) % 90 ) ) // TOUTES LES 30 MINUTES
			{
			// SURVEILLE LE PROCESS MFTTPRO TOUTE LES 30 MINUTES
			
			System( &s, "top -b -n 1 | grep MFTTPRO | awk '{print $10}'" );
			if ( atoi( s ) > 50 )
				{
				logw( "MFT ROBOT : MFTTPRO prend ", s, " % de mémoire  => Redémarrage", _ );
				strdcpy( &cmd, dirbin, DIR_CHAR "mft" EXE " stop", _ );
				System( &s, cmd );
				logw( "MFT ROBOT : mft stop : ", s, _ );
				
				strdcpy( &cmd, dirbin, DIR_CHAR "mft" EXE " start", _ );
				System( &s, cmd );
				logw( "MFT ROBOT : mft start : ", s, _ );
				
				if ( v1 = var_get( hSrvConf, "adm_email" ) )
					{
					strdcpy( &cmd, "echo \"\" | mail -s \"", itod( 0, "jj/mm/aaaa hh:mm:ss" ),
							" : ALERTE MFTTPRO prend trop de memoire : $HOSTNAME ==> Redemarrage\" ", v1, _
							 );
				
					system( cmd );
					}
				}
			
			
			// PURGE DES FICHIER RECU PAR MFT NON TRAITE DEPUIS PLUS DE 30 JOURS
			
			strdcpy( &fname, dirdat, DIR_CHAR "mft" DIR_CHAR "recv", _ );
			file_purge( fname, 30 );
			}
		}
	
	return;
	}

























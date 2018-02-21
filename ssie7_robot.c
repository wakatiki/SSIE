#include "ssie7.h"


//------------------------------------------------------------------------------
int robot_run( T_WORK* t_work )
//------------------------------------------------------------------------------
	{

	//--------------------------------------------------------------------------
	// ROBOT PRINCIPAL ET SURVEILLANCE DES PROCESS
	//--------------------------------------------------------------------------
	
	if ( argc == 2 )
		{
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot.pid", _ );
		if ( file_load( &s, fname ) != sERROR )
			{
			if ( process_is_alive( atoi(s) ) ) ssie7exit(0);
			}
			
		file_save_str( fname, itoa(getpid()), -1 );
		

		// DEMARRE LES AUTRES ROBOTS
		
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_rtask.pid", _ );
		file_delete( fname );
		p1 = StartH( str( argv[0], " robot rtask", _ ) );
		
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_mft.pid", _ );
		file_delete( fname );
		p2 = StartH( str( argv[0], " robot mft", _ ) );
		
		int pf[10], nb_pf = 3;
		v = var_get( hSrvConf, "rflux_nb" );
		if ( v ) nb_pf = atoi(v);
		
		for( i = 1 ; i <= nb_pf; i++ )
			pf[i] = StartH( str( argv[0], " robot flux ", itoa(i), _ ) );
		
		strdcpy( &fname, dirbin, DIR_CHAR "ssie7_robot_local" EXE, _ );
		p4 = 0;
		if ( file_exists( fname ) )
			p4 = StartH( fname );
		
		sleep(1); // Laisse le temps aux robots de démarrer	
		
		// BOUCLE DE TRAITEMENT
		
		for( n5 = 1 ; ; n5++, sleep( 1 ) )
			{
			strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
			if ( file_exists( fname ) )	ssie7exit(0);
			
			//------------------------------------------------------------------
			// SURVEILLANCE DES ROBOT SSIE7
			//------------------------------------------------------------------
			
			if ( ! process_is_alive( p1 ) )
				{
				p1 = StartH( str( argv[0], " robot rtask", _ ) );
				#ifdef TR
				logw("ROBOT : robot rtask anormalement arrêté => Redémarrage", _ );
				#endif
				}
			else
				{
				strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_rtask.pid", _ );
				if ( (sysdate - file_date( fname )) >= (10 Minutes) )
					{
					// Pas d'activité depuis 10 minutes
					if ( s ) s[0] = 0;
					file_load( &s, fname );
					if ( atoi( s ) )
						{
						process_kill( atoi( s ) );
						logw( "ROBOT : etat du robot rtask => synchronisation", _ );
						}
					}
				}
			

			if ( ( p2 )&&( ! process_is_alive( p2 ) ) )
				{
				p2 = StartH( str( argv[0], " robot mft", _ ) );
				#ifdef TR
				logw("ROBOT : robot mft anormalement arrêté => Redémarrage", _ );
				#endif
				}
			else
				{
				strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_mft.pid", _ );
				if (( file_date( fname ) )&&( (sysdate - file_date( fname )) >= (10 Minutes) ))
					{
					// Pas d'activité depuis 10 minutes
					if ( s ) s[0] = 0;
					file_load( &s, fname );
					process_kill( atoi( s ) );
					logw( "ROBOT : etat du robot mft  => synchronisation", _ );
					}
				}
		
			for( i = 1 ; i <= nb_pf; i++ )
				{
				if ( ( pf[i] )&&( ! process_is_alive( pf[i] ) ) )
					{
					pf[i] = StartH( str( argv[0], " robot flux ", itoa(i), _ ) );
					#ifdef TR
					logw("ROBOT : robot flux ", itoa(i), " anormalement arrêté => Redémarrage", _ );
					#endif
					}
				else
					{
					strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", itoa(i), ".timeout", _ );
					
					n = 3;
					if ( file_load( &s, fname ) != sERROR )
						n = atoi( s );
					strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_robot_flux_", itoa(i), ".pid", _ );
					if ( (file_date( fname ))&&( n )&&( (sysdate - file_date( fname )) >= (n Minutes) ) )
						{
						// Pas d'activité depuis n minutes
						if ( s ) s[0] = 0;
						file_load( &s, fname );
						process_kill( atoi( s ) );
						logw( "ROBOT : etat du robot flux ",  itoa(i), " => synchronisation", _ );
						}
					}
				}


			if ( ( p4 )&&( ! process_is_alive( p4 ) ) )
				{
				p4 = StartH( str( dirbin, DIR_CHAR "ssie7_robot_local" EXE, _ ) );
				logw("ROBOT : robot local anormalement arrêté => Redémarrage", _ );
				}
			


			#ifdef OS_WINDOWS
			#ifndef MODE_CONTROLEUR
			//------------------------------------------------------------------
			// SURVEILLANCE DE T0_SSIE
			//------------------------------------------------------------------

			if ( process_count( "T0_SSIE" ) == 0 )
				{
				strdcpy( &cmd, dirbin, "\\T0_SSIE.exe", _ );
				if ( file_exists( cmd ) )
					{
					// suite au correctif de T0_SSIE:Ticket#2017041910000907
					//logw( "ROBOT : T0_SSIE est anormalement arrêté => Redémarrage : ", cmd, _ );
					StartH( cmd );
					}
				}
			
			#endif	
			#endif
			
			
			//------------------------------------------------------------------
			// SURVEILLANCE DE MFT
			//------------------------------------------------------------------

			if (( mft_exists( ) )&&( !(n5%60) ))
				{
				strdcpy( &fname, dirwrk, DIR_CHAR "mft_is_started", _ );
				if ( file_exists( fname ) )
					{
					// SURVEILLANCE DU ROBOT MFT
					
					strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot.pid", _ );
					if ( s ) s[0] = 0;
					file_load( &s, fname );
					if ( ( !s )||( !(s[0]) )||( !process_is_alive( atoi( s ) ) ) )
						{
						logw( "ROBOT : le robot mft est anormalement arrêté => Redémarrage", _ );
						process_kill_all( "mft robot" );
						strdcpy( &cmd, mft_exe(), " robot", _ );
						StartH( cmd );
						}
					}
				}
			
			
			//------------------------------------------------------------------
			ssie7free( t_work );
			}
		}
	

	#include "ssie7_robot_rtask.c"
	
	#include "ssie7_robot_mft.c"
	
	#include "ssie7_robot_flux.c"

	
	//--------------------------------------------------------------------------
		
	return OK;
	}


	














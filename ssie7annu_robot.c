#include "ssie7annu.h"


//------------------------------------------------------------------------------
int robot_run( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	for( n = 0 ; ; sleep( 1 ), n++ )
		{
		//------------------------------------------------------------------
		// SURVEILLANCE DU SERVEUR HTTP
		//------------------------------------------------------------------
		
		strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server_", v = var_get( hSrvConf, "port" ), ".pid", _ );
		if ( s ) s[0] = 0;
		file_load( &s, fname );
		if ( ( !s )||( !process_is_alive( atoi( s ) ) ) )
			{
			strdcpy( &fname, dirwrk, DIR_CHAR, module_name, "_server.stop", _ );
			if ( !file_exists( fname ) )
				{
				// REDEMARRE LE PROCESS PRINCIPAL
				strdcpy( &cmd, argv[0], " server ", v, _ );
				StartH( cmd );
				}
			
			// SOUS UNIX, LES PROCESS SECONDAIRES SONT DIRECTEMENT SURVEILLES
			// PAR LE PROCESS PRINCIPAL
			}
		
		// Essai de se connecter
		
		#ifndef OS_WINDOWS
		if ( !(n % 60) ) // Toutes les minutes
			{
			strdcpy( &s2, argv[0], " request 127.0.0.1:", v, " ping no_log", _ );
			p = StartH( s2 );
			for( n1 = 0 ; ; sleep( 1 ), n1++ )
				{
				if ( ! process_is_alive( p ) ) break;
				if ( n1 > 30 )
					{
					process_kill( p );
					logw( "SERVER SURVEY : Pb de connexion ==> kill des process et redémarrage", _ );
					strdcpy( &cmd, "echo \"\" | mail -s \"", itod( 0, "jj/mm/aaaa hh:mm:ss" ), " : ALERTE PROBLEME " MODULE_NAME
							" $HOSTNAME ==> Redemarrage\" "
							"eric.vautrin@edf.fr didier-d-externe.lefevre@edf.fr olivier-externe.moinard@edf.fr", _ );
					system( cmd );
					strdcpy( &cmd, module_name, " server ", v, _ );
					while( process_count( cmd ) > 0 )
						process_kill_all( cmd );
					break;
					}
				}
			}
		#endif
		
		
		//------------------------------------------------------------------
		// TOUTES LES 5 SECONDES : SI LE FICHIER ANNUAIRE ONS EST RECENT => CHARGE L'ANNUAIRE ONS
		//------------------------------------------------------------------
		
		if ( (! (n % 5))
			&&(
//			( file_exists( "/appli/mersi3nat/data/adl_nat.dat" ) )
			( file_exists( "/appli/ssie7annu/data/adl_nat.dat" ) )
			&&
//			( file_date( "/appli/mersi3nat/data/adl_nat.dat" ) > 
			( file_date( "/appli/ssie7annu/data/adl_nat.dat" ) > 
					file_date( "/appli/ssie7annu/data/maj_part_ons" ) )
			) )
			{
//			strdcpy( &cmd, argv[0], " maj_part_ons /appli/mersi3nat/data/mftpart2.dat /appli/mersi3nat/data/adl_nat.dat", _ );
			strdcpy( &cmd, argv[0], " maj_part_ons /appli/ssie7annu/data/mftpart2.dat /appli/ssie7annu/data/adl_nat.dat", _ );
			System( &s, cmd );
			logw( "robot : maj_part_ons : ", s, _ );
			}
		
		
		//------------------------------------------------------------------
		// UNE FOIS PAR JOUR ET A CHAQUE REDEMARRAGE
		//------------------------------------------------------------------
		
		if ( !( n % (24*60*60) ) )
			{
			// PURGE DES LOG
			file_purge( dirlog, 30 );
			}
		
		//------------------------------------------------------------------
		ssie7free( t_work );
		trace_close( );
		}
	}
			
			

















#include "ssie7.h"

// BIBLIOTHEQUE DE FONCTIONS

/*
//------------------------------------------------------------------------------
int StartSurv( T_WORK *t_work )

// DEMARRE UN PROCESS ET LE MET DANS LA LISTE DES PROCESS A SURVEILLER PAR LE ROBOT PSURV
//------------------------------------------------------------------------------
	{
	static int max_task = 0;
	int wait_id = 0;
	

	if ( ! max_task )
		{
		max_task = atoi(var_get( hSrvConf, "maxtask" ));
		if ( !max_task ) max_task = 50;
		}
	
	if ( ( file_count( dirpsurv ) - 1 ) < max_task )
	{
		while( (pid = StartH( cmd )) == -1 ) sleep(1);
    	strdcpy( &fname, dirpsurv, DIR_CHAR, itoa(pid), _ );
    }
    else
	{
    	wait_id=getuid(str(dirpsurv, DIR_CHAR "wait", DIR_CHAR "wait.id", _ ));
    	strdcpy( &fname, dirpsurv, DIR_CHAR "wait", DIR_CHAR, itona(wait_id, 8), ".w", _ );
    }
    for(;;Sleep(1))
    	{
		file_save_str( fname, cmd, -1 );
		if ( file_exists( fname ) ) break;
		}
	}
*/

/*
//------------------------------------------------------------------------------
int EndSurv()

// FONCTION A APPELER OBLIGATOIREMENT AVANT DE TERMINER LE PROCESS EN COURS
// SINON LE PROCESS RISQUE D'ETRE REDEMARRER EN PERMANENCE
//------------------------------------------------------------------------------
	{
    char *wrk_str = null;
    int loop_cpt = 0;
    
    strdcpy( &wrk_str, dirpsurv, DIR_CHAR, itoa( getpid() ), _ );
    
    while ( !file_exists( wrk_str ) && loop_cpt < 5 )
    {
         Sleep(100);
         loop_cpt++;
    }    

    unlink( wrk_str );
	}



//------------------------------------------------------------------------------
int TouchSurv()

// Met à jour la date du fichier psurv pour la surveillance des process bloqués
//------------------------------------------------------------------------------
	{
	#ifdef OS_UNIX
	static char *str = null;
	
	if ( ! str )	strdcpy( &str, "touch ", dirpsurv, DIR_CHAR, itoa( getpid() ), _ );
    system( str );

	#else
	
    FILE *_fp;
    static char *_fname = null, _c;
    
    if ( !_fname )
    	strdcpy( &_fname, dirpsurv, DIR_CHAR, itoa( getpid() ), _ );
    	
    _fp = fopen( _fname, "rb+" );
	if ( _fp )
		{
		fread( _fp, (char*)(&_c), 1 );
		fseek_top( _fp, 0 );
		fwrite( _fp, (char*)(&_c), 1, _ );
		fclose( _fp );
		}
	
	#endif
	}
*/

/*
//------------------------------------------------------------------------------
int StartPause( T_WORK *t_work, char *prefix, int delay )

// PLACE LE PROCESS ACTUELLE DANS LA LISTE WDATE POUR ETRE REDEMARRE AUTOMATIQUEMENT
// DANS 'delay' SECONDES PAR LE ROBOT WDATE.
//------------------------------------------------------------------------------
	{
	// PLACE LE PROCESS EN COURS DANS LA LISTE D'ATTENTE WDATE

	strdcpy( &fname, dirtmp, DIR_CHAR "pause_", itoa(getpid()), _ );
	fp = fopen( fname, "wb" );
	if ( !fp )
		{
		logw( "SYSTEM : ERREUR StartPause : Pb Création fichier ", fname, _ );
		return ERROR;
		}


	fprint( fp, argv[0], _ );

	for( i = 1 ; i < argc ; i++ )
		fprint( fp, " ", argv[i], _ );
	fclose( fp );

	
	strdcpy( &fname1, dirwdate, DIR_CHAR, prefix, ".", itoa(time(NULL) + delay), _ );
	
	if ( file_move( fname, fname1 ) == ERROR )
		logw( "StartPause : ", error_msg, _ );
	

	return OK;
	}
*/


//------------------------------------------------------------------------------
char* mft_exe( )

// RETOURNE LE CHEMIN COMPLET DE L'EXECUTABLE 'mft.exe'
//------------------------------------------------------------------------------
	{
	static char *mft = null;
	
	if ( mft == null )
		strdcpy( &mft, dirbin, DIR_CHAR "mft" EXE, _ );
	
	return mft;
	}
	
	
//------------------------------------------------------------------------------
bool mft_exists( )

// RETOURNE true SI mft_exe() EXISTE, false SINON.
//------------------------------------------------------------------------------
	{
	static bool bMFT = -1;
	
	if ( bMFT == -1 )
		{
		if ( strcmp( module_name, "ssie7" ) ) bMFT = false;
		else
			bMFT = (( file_exists( mft_exe() ) )
					&&( dir_exists( str( dirbin, DIR_CHAR "mftbin", _ ) ) )
					);
		}
	
	return bMFT;
	}
	
	










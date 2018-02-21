#define MAIN
//#include "ssie7.h"
#include "elib.h"
#include "elib4.c"

//------------------------------------------------------------------------------
int main(int ac, char *av[])
//------------------------------------------------------------------------------
	{
	long int _i, _p, _p1;
	handle _h;
	char *_cmd = null, *_v, *_s = null, *_s1 = null, *_ip = null, *_r = null;
	FILE *fpd, *fout = NULL;


	_v = getenv( "SSIE7DIRBIN" );

	if ( ( !_v )||( !(_v[0]) ) )
		{
		if ( file_exists( "d:\\r2000\\mft\\ssie7.exe" ) )
			_v = "d:\\r2000\\mft";
		else
		if ( file_exists( "d:\\r2000\\ssie7\\bin\\ssie7.exe" ) )
			_v = "d:\\r2000\\ssie7\\bin";
		else
		if ( file_exists( "d:\\ssie7\\bin\\ssie7.exe" ) )
			_v = "d:\\ssie7\\bin";
		else
			{
			// MODE CLIENT TCP
//			if ( ac < 2 ) exit(1);

			// RECHERCHE DU FICHIER DE CONF OU DU FICHIER MFTUTIL.CFG POUR OBTENIR L'IP DU SERVEUR
			if ( file_exists( ".\\mftutil.cfg" ) )
				{
				file_load( &_ip, ".\\mftutil.cfg" );
				}
			else
			if ( file_exists( strdcpy( &_cmd, file_getdir( av[0] ), "\\mftutil.cfg", _ ) ) )
				{
				file_load( &_ip, _cmd );
				}
			else
			if ( file_exists( ".\\ssie\\conf\\ssie7.dat" ) )
				{
				_h = var_load( ".\\ssie\\conf\\ssie7.dat" );
				strdcpy( &_ip, var_get( _h, "ip_ssie" ), ":", var_get( _h, "port" ), _ );
				}
			else
			if ( file_exists( "r:\\mft\\ssie\\conf\\ssie7.dat" ) )
				{
				_h = var_load( "r:\\mft\\ssie\\conf\\ssie7.dat" );
				strdcpy( &_ip, var_get( _h, "ip_ssie" ), ":", var_get( _h, "port" ), _ );
				}
			else
			if ( file_exists( strdcpy( &_cmd, file_getdir( av[0] ), "\\ssie\\conf\\ssie7.dat", _ ) ) )
				{
				_h = var_load( _cmd );
				strdcpy( &_ip, var_get( _h, "ip_ssie" ), ":", var_get( _h, "port" ), _ );
				}
			else
				{
				printf( "ERREUR : ssie7 non trouvé\n" );
				exit(2);
				}

			
			if ( ac == 1 )
				{
				// MODE CONSOLE

				_cmd = alloc( 4096 );
			
				for( ; ; )
					{
					printf( BR BR "MFTUTIL > " );
					_cmd = realloc( _cmd, 4096 );
			
					fgets( _cmd, 4096, stdin );
			
					if ( trim( _cmd )[0] == 0 ) continue;
					
					//------------------------------------------------------------------
					if ( strmicmp( _cmd, "q|quit|exit" ) )
					//------------------------------------------------------------------
						{
						free( _cmd );
						_cmd = null;
						return OK;
						}
			
					//------------------------------------------------------------------
					
					if ( tcp_request( _ip, _cmd, &_r ) == null )
						{
						printf( "ERREUR connexion au serveur : %s" BR, _ip );
						exit(2);
						}
					printf( "%s" BR, _r );
					}
				}

			
			// EXECUTE LA COMMANDE PAR REQUETE TCP
			
			if (( av[1][0] != '@' )&&( av[1][0] != '#' ))
				{
				strdcpy( &_cmd, "mftutil", _ );
				for( _i = 1 ; _i < ac ; _i++ )
					strdcat( &_cmd, " ", (char*)((strpos(av[_i]," ",0)>=0) ? quote(av[_i]) : av[_i]), _ );

				if ( tcp_request( _ip, _cmd, &_r ) == null )
					{
					printf( "ERREUR connexion au serveur : %s" BR, _ip );
					exit(2);
					}
				printf( "%s" BR, _r );
				exit(0);
				}

			
			
			// EXECUTE UN FICHIER DE COMMANDE
			
			if ( _s1 ) _s1[0] = 0;
			file_load( &_s1, &(av[1][1]) );
			_p = 0;
			strdcpy( &_cmd, "mftutil ", _ );
			while( sgets( &_s, _s1, &_p ) )
				{
				if ( stripos( _s, "type=output", 0 ) >= 0 )
					{
					_p1 = stripos( _s, "fname=", 0 );
					fout = fopen( trim(substr( _s, _p1+6, -1 )), "wb" );
					}
				else
				if ( stripos( _s, "config type", 0 ) >= 0 ) continue;
				else
					{
					trim( _s );
					strdcat( &_cmd, _s, _ );
					if ( _s[strlen(_s)-1] == ',' ) continue;

					if ( tcp_request( _ip, _cmd, &_r ) == null )
						{
						printf( "ERREUR connexion au serveur : %s" BR, _ip );
						exit(2);
						}

					if ( fout ) fprint( fout, _r, BR, _ );
					else	printf( "%s" BR, _r );
	
					strdcpy( &_cmd, "mftutil ", _ );
					}
				}
			
			exit(0);
			}
		}
	

	strdcpy( &_cmd, _v, DIR_CHAR "ssie7" EXE " mftutil", _ );
	for( _i = 1 ; _i < ac ; _i++ )
		strdcat( &_cmd, " ", (char*)((strpos(av[_i]," ",0)>=0) ? quote(av[_i]) : av[_i]), _ );

	system( _cmd );
	}



	
	















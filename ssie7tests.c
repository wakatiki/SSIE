#define MAIN
#include "ssie7.h"


//------------------------------------------------------------------------------
int main(int ac, char *av[])
//------------------------------------------------------------------------------
	{
	T_WORK_ALLOC;
	
	argc = ac;
	argv = av;
	
	if ( argc < 2 )
		{
		print( "syntaxe : ", argv[0], " charge mftsend {nb fichier} {nb process}" BR, _ );
		print( "syntaxe : ", argv[0], " charge send {nb fichier} {nb process}" BR, _ );
		wexit( 0 );
		}
		
	if ( ! stricmp( argv[1], "charge" ) )
		{
		if ( ! stricmp( argv[2], "mftsend" ) )
			return tests_charge_mftsend( t_work );
		
		if ( ! stricmp( argv[2], "mftsend_exec" ) )
			return tests_charge_mftsend_exec( t_work );
		}
	
	}



	
	















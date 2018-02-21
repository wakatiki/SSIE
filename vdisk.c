#define MAIN
#include "elib4.c"
#define MFT
#include "ssie7lib.c"

#ifdef OS_WINDOWS
	#define APPLI_DIR		"c:\\e\\apps"
	#define BIN_DIR			"c:\\e\\bin\\windows"
	#define OPENSSL			"c:\\e\\bin\\windows\\ssie7sec.exe"
#endif

#ifdef OS_AIX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/"
	#define OPENSSL			null
#endif

#ifdef OS_LINUX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/logiciels/ssie7"
	#define OPENSSL			null
#endif

#ifdef OS_SOLARIS
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/logiciels/ssie7"
	#define OPENSSL			null
#endif


#include "ssie7lib_cmd_connect.c"
#include "ssie7lib_vdisk.c"
#include "ssie7lib_vdisk_cmd.c"



int main(int ac, char *av[])
	{
	T_WORK_ALLOC;
	
	if ( ac < 2 ) exit(1);
	
	argc = ac;
	argv = av;

	strdcpy( &module_name, getenv("SSIE_MODULE_NAME"), _ );
	strdcpy( &dirbin	, getenv("SSIE_DIRBIN"), _ );
	strdcpy( &dirwrk	, getenv("SSIE_DIRWRK"), _ );
	strdcpy( &dirlog	, getenv("SSIE_DIRLOG"), _ );
	strdcpy( &dirconf	, getenv("SSIE_DIRCONF"), _ );
	strdcpy( &server_conf_fname	, getenv("SSIE_CONF_FNAME"), _ );
	
	hSrvConf = var_load( server_conf_fname );
	

	// RECHERCHE D'OPENSSL (OBLIGATOIRE)
	
	#ifdef OS_WINDOWS
	// POUR WINDOWS C'EST $DIRBIN/SSIE7SEC.EXE
	strdcpy( &fname, dirbin, DIR_CHAR "ssie7sec.exe", _ );
	if ( ssl_pgm( fname ) == null )
	#else
	// POUR UNIX, LE PROGRAMME EST RECHERCHE DANS LE PATH
	if ( ssl_pgm( null ) == null )
	#endif
		{
		print( "ERREUR : openssl n'est pas installé ou est inaccessible.(",fname,")" BR, _ );
		wexit(1);
		}
	
	// LECTURE DE LA COMMANDE
	
	if ( argv[1][0] == '@' )
		{
		if ( cmd ) cmd[0] = 0;
		file_load( &cmd, &(argv[1][1]) );
		file_delete( &(argv[1][1]) );
		}
	else
		{
		strdcpy( &cmd, "vdisk", _ );
		for( i = 1 ; i < argc ; i++ )
			{
			if ( (p=strpos( argv[i], " ", 0 )) < 0 )
				strdcat( &cmd, " ", argv[i], _ );
			else
				{
				if (( (p1=strpos( argv[i], "=", 0 )) > 0 )&&( p1 < p ))
					strdcat( &cmd, " ", substr( argv[i], 0, p1+1 ), str_quote( substr( argv[i], p1+1, -1 ) ), _ );
				else
						strdcat( &cmd, " ", str_quote( argv[i] ), _ );
				}
			}
		}


	if ( hReq ) var_free( hReq );
	hReq = var_from_cmd( 0, cmd );
	if ( !hReq )
		{
		print( "ERREUR de syntaxe" BR, cmd, _ );
		return;
		}
	
	r = vdisk_exec_cmd( t_work );
	exit(r);
	}
	
//------------------------------------------------------------------------------
int ssie7_cmd( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	}

/*	
//------------------------------------------------------------------------------
int vdisk_exec_cmd( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 0, &vn );
	if ( !v ) return ERROR;
	
	if ( !strcmp( vn, "vdisk" ) )
		{
		// LECTURE
		v = var_getn( hReq, 1, &what );

		}

	if ( !strcmp( vn, "vdisk_server" ) )
		{
		}

	}
*/
	

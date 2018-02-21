#define MAIN
#define PRODUCT_NAME		"SSIE v7.2.a.5.rc1 - MFT Controler du 2017/01/04"

//#define TR	// Mode trace pour les tests

#include "elib4.c"

#include "mft.h"

#include "mft_lib.c"
#include "ssie7lib.c"
#include "ssie7lib_run.c"
#include "ssie7lib_mft.c"

//#include "ssie7lib.h"

#include "mft_cmd.c"
#include "mft_cmd_confpart.c"
#include "mft_cmd_start.c"
//#include "mft_cmd_tsend.c"
#include "mft_robot_ssie_to_mft.c"
#include "mft_robot_mft_to_ssie.c"
#include "mft_robot.c"

// main , ssie7_init , server_run , robot_run

//------------------------------------------------------------------------------
int main(int ac, char *av[])
//------------------------------------------------------------------------------
	{
	argc = ac;
	argv = av;
	tr_init();

	error_no = ssie7run();
	exit( error_no );
	}




//------------------------------------------------------------------------------
void ssie7_init( T_WORK* t_work )
	// Cette fonction est appelée immédiatement par ssie7run()
	// Elle permet d'initialiser le programme
//------------------------------------------------------------------------------
	{
	strdcpy( &module_name, "ssie7", _ );
	product_name	= PRODUCT_NAME;
	
	strdcpy( &dirbin,	"/logiciels/ssie7", _ );
	strdcpy( &mft_dirbin,	dirbin, DIR_CHAR "mftbin", _ );
	strdcpy( &dirlog,		APPLI_DIR	DIR_CHAR "ssie7" DIR_CHAR "log", _ );
	strdcpy( &dirconf,		APPLI_DIR	DIR_CHAR "ssie7" DIR_CHAR "conf", _ );
	strdcpy( &dirdat,		APPLI_DIR	DIR_CHAR "ssie7" DIR_CHAR "data", _ );
	strdcpy( &mft_dirconf,	dirconf,	DIR_CHAR "mft", _ );
	strdcpy( &dirrecv, 		dirdat , 	DIR_CHAR "recv", _ );
	strdcpy( &mft_dirdat,	dirdat, 	DIR_CHAR "mft", _ );
	strdcpy( &dirwrk,		dirdat, 	DIR_CHAR "wrk", _ );
	strdcpy( &mft_dirlog,	mft_dirdat,	DIR_CHAR "log", _ );
	strdcpy( &mft_dirrecv,	mft_dirdat,	DIR_CHAR "recv", _ );

	strdcpy( &dirtmp,		dirwrk,     DIR_CHAR "tmp", _ );
//	strdcpy( &dirwdate, 	dirwrk ,	DIR_CHAR "wdate", _ );
//	strdcpy( &dirpsurv,		dirwrk, 	DIR_CHAR "psurv", _ );
	
	strdcpy( &mft_conf_fname,	 dirconf, DIR_CHAR "mft.dat", _ );
	strdcpy( &server_conf_fname, dirconf, DIR_CHAR "ssie7.dat", _ );


	#ifdef OS_WINDOWS
	
	putenv( strdcpy( &s, "MFTDIRBIN=",		mft_dirbin, _ ) );
	putenv( strdcpy( &s, "MFTDIRLIB=",		mft_dirbin, _ ) );
	putenv( strdcpy( &s, "MFTDIRINSTALL=",	mft_dirbin, _ ) );
	putenv( strdcpy( &s, "MFTDIRRUNTIME=",	mft_dirbin, _ ) );
	putenv( strdcpy( &s, "MFTXIP_COMPONENT_PROPERTIES=",	mft_dirbin, DIR_CHAR "component.properties", _ ) );
	putenv( strdcpy( &s, "MFTUCONF=",		mft_dirbin, DIR_CHAR "mftuconf.dat", _ ) );
	putenv( strdcpy( &s, "MFTDIRDAT=",		mft_dirdat, _ ) );
	putenv( strdcpy( &s, "MFTDIRLOG=",		mft_dirlog, _ ) );

	putenv( strdcpy( &s, "MFTPARM=",		mft_dirdat, DIR_CHAR "mft_parm", _ ) );
	putenv( strdcpy( &s, "MFTPART=",		mft_dirdat, DIR_CHAR "mft_part", _ ) );
	putenv( strdcpy( &s, "MFTCAT=", 		mft_dirdat, DIR_CHAR "mft_cat", _ ) );
	putenv( strdcpy( &s, "MFTCOM=", 		mft_dirdat, DIR_CHAR "mft_com", _ ) );
	putenv( strdcpy( &s, "MFTPKU=", 		mft_dirdat, DIR_CHAR "mft_pki", _ ) );
	putenv( strdcpy( &s, "MFTPKIDIR=",		mft_dirdat, _ ) );
	putenv( strdcpy( &s, "MFTDIRINQ=",		mft_dirdat, _ ) );

	putenv( strdcpy( &s, "MFTDIRWORK=", 	mft_dirdat, _ ) );

	#else

	#ifndef OS_SUN
	
	setenv( "MFTDIRBIN",		mft_dirbin, 1 );
	setenv( "MFTDIRLIB",		mft_dirbin, 1 );
	setenv( "MFTDIRINSTALL",	mft_dirbin, 1 );
	setenv( "MFTDIRRUNTIME",	mft_dirbin, 1 );
	setenv( "MFTXIP_COMPONENT_PROPERTIES", strdcpy( &s, mft_dirbin, DIR_CHAR "component.properties", _ ), 1 );
	setenv( "MFTUCONF",			strdcpy( &s, mft_dirbin, DIR_CHAR "mftuconf.dat", _ ), 1 );
	setenv( "MFTDIRDAT",		mft_dirdat, 1 );
	setenv( "MFTDIRLOG",		mft_dirlog, 1 );

	setenv( "MFTPARM",			strdcpy( &s, mft_dirdat, DIR_CHAR "mft_parm", _ ), 1 );
	setenv( "MFTPART",			strdcpy( &s, mft_dirdat, DIR_CHAR "mft_part", _ ), 1 );
	setenv( "MFTCAT",			strdcpy( &s, mft_dirdat, DIR_CHAR "mft_cat", _ ), 1 );
	setenv( "MFTCOM",			strdcpy( &s, mft_dirdat, DIR_CHAR "mft_com", _ ), 1 );
	setenv( "MFTPKU",			strdcpy( &s, mft_dirdat, DIR_CHAR "mft_pki", _ ), 1 );
	setenv( "MFTPKIDIR",		mft_dirdat, 1 );
	setenv( "MFTDIRINQ",		mft_dirdat, 1 );

	#endif

	#ifdef OS_AIX
	setenv( "LIBPATH", mft_dirbin, 1 );
	#else
	#ifndef OS_SUN
	setenv( "LD_LIBRARY_PATH", mft_dirbin, 1 );
	#endif
	#endif
	
	#endif

	hSrvConf = var_load( server_conf_fname );
	
	ssie_user = var_get( hSrvConf, "ssie_user" );
	if (( !ssie_user )||( ssie_user[0] == 0 ))
		{
		ssie_user = null;
		System( &ssie_user, "whoami" );
		trim( ssie_user );
		}
	}


	
//------------------------------------------------------------------------------
int ssie7_cmd( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	if ( !stricmp( what, "full" ) )
		{
		var_getn( hReq, 1, &vn );
		var_free( h );
		p = strpos( cmd, "full", 0 );
		h = mft_listcat_full( &(cmd[p+4]) );
		if ( h != ERROR )
			var_print( h, BR );
		else
			print( "ERREUR:", error_msg, BR, _ );
			
		return OK;
		}
	
//	if ( !stricmp( what, "tsend" ) )	return cmd_tsend( t_work );

	if ( !stricmp( what, "conf" ) )		return cmd_conf( t_work );

	if ( !stricmp( what, "confpart" ) )	return cmd_confpart( t_work );

	if ( !stricmp( what, "start" ) )	return cmd_start( t_work, false );

	if ( !stricmp( what, "status" ) )	return cmd_status( t_work );

	if ( !stricmp( what, "stop" ) )		return cmd_stop( t_work, false );

	if ( !stricmp( what, "restart" ) )
		{
		cmd_stop( t_work, true );
		cmd_start( t_work, true );
		return;
		}
	
	if ( !stricmp( what, "find" ) )		return cmd_find( t_work );

	if ( !stricmp( what, "log" ) )		return cmd_log( t_work );

	if ( !stricmp( what, "out" ) )		return cmd_out( t_work );

	if ( !stricmp( what, "support" ) )
		{
		system( "/logiciels/ssie7/mft/mft_support collect" );
		return OK;
		}
	
	if ( !stricmp( what, "env" ) )
		{
		system( "env" );
		return OK;
		}
		
	if ( !stricmp( what, "mftdir" ) )
		{
		print(	"mft_dirconf = ", mft_dirconf, BR,
				"mft_dirdat  = ", mft_dirdat, BR,
				_ );
				
		return OK;
		}
	if ( !stricmp( what, "version" ) )
		{
		print( PRODUCT_NAME BR, _ );
		return OK;
		}

	return cmd_mftutil( t_work );
	}



//------------------------------------------------------------------------------
int cmd_server( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	return OK;
	}

	
	















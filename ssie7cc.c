#define MAIN
#include "elib.h"

#define SRV_IP "THEBES"		// THEBES APACHE AIX6
#define SRV_OS "LINUX"		// LINUX AIX


#define COMPILE_SSIE7		1
#define COMPILE_SSIE7ANNU	0
#define COMPILE_SSIE7C		0
#define COMPILE_MFT			0
#define COMPILE_MFTUTIL		0



char *srv_ip = null;

//------------------------------------------------------------------------------
char* putfile( char *fsrc, char *fdest, int bAscii )
//------------------------------------------------------------------------------
	{
	static char *cmd = null, *res = null;
	
	strdcpy( &cmd, "putfile ", SRV_IP, " ", fsrc, " dest=", fdest,
			(char*)((bAscii)?" ascii":""), _ );
	file_save_str( "c:/temp/putfile.cmd", cmd, -1 );
	print( cmd, BR, _ );

	strdcpy( &cmd, "ssie7c" EXE " @c:/temp/putfile.cmd", _ );
	
	System( &res, cmd );
	print( res, BR, _ );
	return res;
	}
	
//------------------------------------------------------------------------------
char* getfile( char *fsrc, char *fdest, int bAscii )
//------------------------------------------------------------------------------
	{
	static char *cmd = null, *res = null;
	
	strdcpy( &cmd, "getfile ", SRV_IP, " ", fsrc, " dest=", fdest,
			(char*)((bAscii)?" ascii":""), _ );
	file_save_str( "c:/temp/getfile.cmd", cmd, -1 );
	print( cmd, BR, _ );

	strdcpy( &cmd, "ssie7c" EXE " @c:/temp/getfile.cmd", _ );
	
	System( &res, cmd );
	print( res, BR, _ );
	return res;
	}

//------------------------------------------------------------------------------
char * request( char *p_cmd )
//------------------------------------------------------------------------------
	{
	static char *cmd = null, *res = null, *v;

	print( p_cmd, BR, _ );
	v = tcp_request( srv_ip, p_cmd, &res );

	if ( ! v )
		print( "ERREUR : ", error_msg, _ );
	else
		print( v, _ );
	return res;
	}


bool bCompiled;
char *compiled_list = null;

#define compile( f ) \
	strdcat( &s, f, ".o ", _ );\
	strdcpy( &s1, f, ".o ", _ );\
	if ( stripos( compiled_list, s1, 0 ) < 0 )\
		{\
		strdcat( &compiled_list, f, ".o ", _ );\
		strdcpy( &cmd, "cd /tmp/o;", gcc, " -DOS_", SRV_OS, " -c ", f, ".c", _ );\
		v = request( cmd );\
		if ( ( stripos( v, "error", 0 ) >= 0 )||( stripos( v, "erreur", 0 ) >= 0 ) )\
			wexit(0);\
		}

//gcc -DOS_AIX -c elib4.c


//------------------------------------------------------------------------------
int main(int ac, char *av[])
//------------------------------------------------------------------------------
	{
	char *v, *gcc=null, *fname1 = null, *fname2 = null, *cmd = null, *s = null, *s1 = null;
	int h;
	T_FILE_INFO *tf;

	strdcpy( &fname1, "c:/appli/ssie7c/conf/part/", SRV_IP, ".dat", _ );
	if ( !file_exists( fname1 ) )
		{
		print( "ERREUR : Le partenaire ", SRV_IP, " n'existe pas" BR, _ );
		wexit(0);
		}
	
	h = var_load( fname1 );
	strdcpy( &srv_ip, var_get( h, "ip_ssie" ), ":", var_get( h, "port" ), _ );
	print( SRV_IP " => ", srv_ip, BR, _ );

	request( "mkdir /tmp/o" );
	request( "rm -f /tmp/o/*" );

	strdcpy( &gcc, trim(request( "which gcc" )), _ );
	
	
	print( "SSIE v7 : COMPILE LES PROGRAMMES", BR, _ );
	
	//--------------------------------------------------------------------------
	// COMPILE LA ELIB v4
	//--------------------------------------------------------------------------
	

	v = putfile( "\"c:/edf/ssie/v 7.1.A/src/elib4src/c/*\"", "/tmp/o", true );
	if ( strpos( v, "OK", 0 ) < 0 )
		wexit(0);
		
	v = putfile( "\"c:/edf/ssie/v 7.1.A/src/c/*\"", "/tmp/o", true );
	if ( strpos( v, "OK", 0 ) < 0 )
		wexit(0);


	strdcpy( &cmd, "cd /tmp/o;", gcc, " -DOS_", SRV_OS, " -c elib4.c", _ );
	v = request( cmd );
	if ( ( stripos( v, "error", 0 ) >= 0 )||( stripos( v, "erreur", 0 ) >= 0 ) )
		{
		print( s, BR BR, v, BR, _ );
		wexit(0);
		}



	v = getfile( "/tmp/o/elib4.o", "c:/bin/" SRV_OS "/elib4.o", false );

	if ( strpos( v, "OK", 0 ) < 0 ) wexit(0);
	if ( file_size( "c:/bin/" SRV_OS "/elib4.o" ) < 1000 )
		{
		print( "ERREUR : téléchargement de elib4.o" BR, _ );
		wexit(0);
		}



	//--------------------------------------------------------------------------
	// COMPILE SSIE7
	//--------------------------------------------------------------------------

if ( COMPILE_SSIE7 )
	{
	strdcpy( &s, "", _ );

	compile( "ssie7" );
	compile( "ssie7_cmd_app" );
	compile( "ssie7_cmd_mft" );
	compile( "ssie7_cmd_mftutil" );
	compile( "ssie7_cmd_exploit" );
	compile( "ssie7_cmd_export" );
	compile( "ssie7_cmd_flux" );
	compile( "ssie7_cmd_gateway" );
	compile( "ssie7_cmd_import" );
	compile( "ssie7_cmd_login" );
	compile( "ssie7_cmd_mersi" );
	compile( "ssie7_cmd_part" );
	compile( "ssie7_cmd_server" );
	compile( "ssie7_flux_exec" );
	compile( "ssie7_flux_exec_ack" );
	compile( "ssie7_flux_exec_emi" );
	compile( "ssie7_flux_exec_on_error" );
	compile( "ssie7_flux_exec_planif" );
	compile( "ssie7_flux_exec_rec" );
	compile( "ssie7_flux_exec_script" );
	compile( "ssie7_flux_exec_start" );
	compile( "ssie7_flux_exec_startinside" );
	compile( "ssie7_flux_exec_waitfile" );
	compile( "ssie7_lib" );
	compile( "ssie7_robot" );
	compile( "ssie7lib" );
	compile( "ssie7lib_cmd" );
	compile( "ssie7lib_cmd_connect" );
	compile( "ssie7lib_cmd_exploit" );
	compile( "ssie7lib_cmd_getfile" );
	compile( "ssie7lib_cmd_putfile" );
	compile( "ssie7lib_cmd_recv" );
	compile( "ssie7lib_cmd_recv_server" );
	compile( "ssie7lib_cmd_request" );
	compile( "ssie7lib_cmd_rexec" );
	compile( "ssie7lib_cmd_send" );
	compile( "ssie7lib_cmd_send_client" );
	compile( "ssie7lib_cmd_send_execute" );
	compile( "ssie7lib_cmd_send_status_cancel" );
	compile( "ssie7lib_cmd_send_server" );
	compile( "ssie7lib_gateway" );
	compile( "ssie7lib_run" );
	

	strdcpy( &cmd, "cd /tmp/o;", gcc, " ", s, " elib4.o -o ssie7", _ );

	if ( !strcmp( SRV_OS, "SOLARIS" ) )
		strdcat( &cmd, " -l socket -l nsl", _ );

	v = request( cmd );
	if ( ( stripos( v, "error", 0 ) >= 0 )||( stripos( v, "erreur", 0 ) >= 0 ) )
		wexit(0);


	v = getfile( "/tmp/o/ssie7", "c:/bin/" SRV_OS "/ssie7", false );
	if ( strpos( v, "OK", 0 ) < 0 ) wexit(0);
	if ( file_size( "c:/bin/" SRV_OS "/ssie7" ) < 1000 )
		{
		print( "ERREUR : téléchargement de ssie7" BR, _ );
		wexit(0);
		}
	}
	
//wexit(0);

	
	//--------------------------------------------------------------------------
	// COMPILE SSIE7C
	//--------------------------------------------------------------------------

if ( COMPILE_SSIE7C )
	{
	strdcpy( &s, "", _ );

	compile( "ssie7c" );
	compile( "ssie7c_cmd_initialize" );
	compile( "ssie7c_cmd_gw_inst" );
	compile( "ssie7lib" );
	compile( "ssie7lib_cmd" );
	compile( "ssie7lib_cmd_connect" );
	compile( "ssie7lib_cmd_exploit" );
	compile( "ssie7lib_cmd_getfile" );
	compile( "ssie7lib_cmd_part" );
	compile( "ssie7lib_cmd_putfile" );
	compile( "ssie7lib_cmd_request" );
	compile( "ssie7lib_cmd_rexec" );
	compile( "ssie7lib_cmd_server" );
	compile( "ssie7lib_robot" );
	compile( "ssie7lib_run" );

	strdcpy( &cmd, "cd /tmp/o;", gcc, " ", s, " elib4.o -o ssie7c", _ );

	if ( !strcmp( SRV_OS, "SOLARIS" ) )
		strdcat( &cmd, " -l socket -l nsl", _ );

	v = request( cmd );
	if ( ( stripos( v, "error", 0 ) >= 0 )||( stripos( v, "erreur", 0 ) >= 0 ) )
		wexit(0);


	v = getfile( "/tmp/o/ssie7c", "c:/bin/" SRV_OS "/ssie7c", false );
	if ( strpos( v, "OK", 0 ) < 0 ) wexit(0);
	if ( file_size( "c:/bin/" SRV_OS "/ssie7c" ) < 1000 )
		{
		print( "ERREUR : téléchargement de ssie7c" BR, _ );
		wexit(0);
		}
	}
	

	//--------------------------------------------------------------------------
	// COMPILE MFTUTIL
	//--------------------------------------------------------------------------

if( COMPILE_MFTUTIL )
	{
	compile( "mftutil" );
	
	strdcpy( &cmd, "cd /tmp/o;", gcc, " mftutil.o elib4.o -o MFTUTIL", _ );
//	if ( !strcmp( SRV_OS, "SOLARIS" ) )		strdcat( &cmd, " -l socket -l nsl", _ );

	v = request( cmd );
	if ( ( stripos( v, "error", 0 ) >= 0 )||( stripos( v, "erreur", 0 ) >= 0 ) )
		wexit(0);


	v = getfile( "/tmp/o/MFTUTIL", "c:/bin/" SRV_OS "/MFTUTIL", false );
	if ( strpos( v, "OK", 0 ) < 0 ) wexit(0);
	if ( file_size( "c:/bin/" SRV_OS "/MFTUTIL" ) < 1000 )
		{
		print( "ERREUR : téléchargement de MFTUTIL" BR, _ );
		wexit(0);
		}
	}


	//--------------------------------------------------------------------------
	// COMPILE MFT
	//--------------------------------------------------------------------------

if ( COMPILE_MFT )
	{	
	strdcpy( &s, "", _ );
	
	compile( "mft" );
	compile( "mft_cmd" );
	compile( "mft_cmd_confpart" );
	compile( "mft_cmd_start" );
	compile( "mft_cmd_tsend" );
	compile( "mft_lib" );
	compile( "mft_robot" );
	compile( "ssie7lib" );
	compile( "ssie7lib_run" );
	
	strdcpy( &cmd, "cd /tmp/o;", gcc, " ", s, " elib4.o -o mft", _ );

	if ( !strcmp( SRV_OS, "SOLARIS" ) )
		strdcat( &cmd, " -l socket -l nsl", _ );

	v = request( cmd );
	if ( ( stripos( v, "error", 0 ) >= 0 )||( stripos( v, "erreur", 0 ) >= 0 ) )
		wexit(0);


	v = getfile( "/tmp/o/mft", "c:/bin/" SRV_OS "/mft", false );
	if ( strpos( v, "OK", 0 ) < 0 ) wexit(0);
	if ( file_size( "c:/bin/" SRV_OS "/mft" ) < 1000 )
		{
		print( "ERREUR : téléchargement de mft" BR, _ );
		wexit(0);
		}
	}


	
	//--------------------------------------------------------------------------
	// COMPILE SSIE7ANNU
	//--------------------------------------------------------------------------

if ( COMPILE_SSIE7ANNU )
	{
	strdcpy( &s, "", _ );
	
	compile( "ssie7annu" );
	compile( "ssie7annu_cmd_get_ssl_certif" );
	compile( "ssie7annu_cmd_mersi3nat" );
	compile( "ssie7annu_cmd_login" );
	compile( "ssie7annu_cmd_log_agrega" );
	compile( "ssie7annu_cmd_part" );
	compile( "ssie7annu_robot" );
	compile( "ssie7lib" );
	compile( "ssie7lib_cmd" );
	compile( "ssie7lib_cmd_connect" );
	compile( "ssie7lib_cmd_exploit" );
	compile( "ssie7lib_cmd_getfile" );
	compile( "ssie7lib_cmd_putfile" );
	compile( "ssie7lib_cmd_request" );
	compile( "ssie7lib_cmd_server" );
	compile( "ssie7lib_run" );

	strdcpy( &cmd, "cd /tmp/o;", gcc, " ", s, " elib4.o -o ssie7annu", _ );

	if ( !strcmp( SRV_OS, "SOLARIS" ) )
		strdcat( &cmd, " -l socket -l nsl", _ );

	v = request( cmd );
	if ( ( stripos( v, "error", 0 ) >= 0 )||( stripos( v, "erreur", 0 ) >= 0 ) )
		wexit(0);


	v = getfile( "/tmp/o/ssie7annu", "c:/bin/" SRV_OS "/ssie7annu", false );
	if ( strpos( v, "OK", 0 ) < 0 ) wexit(0);
	if ( file_size( "c:/bin/" SRV_OS "/ssie7annu" ) < 1000 )
		{
		print( "ERREUR : téléchargement de ssie7annu" BR, _ );
		wexit(0);
		}
	}


	wexit(0);
	}
	

















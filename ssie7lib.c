#include "ssie7lib.h"


FILE * fpd = NULL;
extern FILE *elib_print_fp;

void	ssie7trace( T_WORK *t_work, char *msg, ... );


//------------------------------------------------------------------------------
void	ssie7print( T_WORK *t_work, char *msg, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg;


	if ( msg == _ ) return;
	if ( msg == __ )
		{
		if ( !ssie7out )
			{
			fflush( stdout );
			if ( elib_print_fp ) fflush( elib_print_fp );
			}
		else
			{
			tcp_ssend( hCnx, ssie7out, -1 );
			ssie7out[0] = 0;
			}
		return;
		}

	if ( msg )
		{
		if ( !ssie7out )
			{
			if ( ( elib_print_fp )&&( elib_print_fp != stdout ) )
				fprintf( elib_print_fp, "%s", msg );
			else
				#ifdef OS_WINDOWS
				printf( "%s", to_ascii_dos(msg) );
				#else
				printf( "%s", msg );
				#endif
			
			}
		else
			strdcat( &ssie7out, msg, _ );
		}
		
	va_start(args, msg); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		
		if ( !arg ) continue;
		if ( arg == _ ) return;
		if ( arg == __ )
			{
			if ( !ssie7out )
				{
				fflush( stdout );
				if ( elib_print_fp ) fflush( elib_print_fp );
				}
			else
				{
				tcp_ssend( hCnx, ssie7out, -1 );
				ssie7out[0] = 0;
				}
			return;
			}
			
		if ( !ssie7out )
			{
			if ( ( elib_print_fp )&&( elib_print_fp != stdout ) )
				fprintf( elib_print_fp, "%s", arg );
			else
				#ifdef OS_WINDOWS
				printf( "%s", to_ascii_dos(arg) );
				#else
				printf( "%s", arg );
				#endif
			}
		else
			strdcat( &ssie7out, arg, _ );
		}
	}
	


//------------------------------------------------------------------------------
void	ssie7flush( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( !ssie7out )	fflush( stdout );
	else
		{
		tcp_ssend( hCnx, ssie7out, -1 );
		ssie7out[0] = 0;
		}
	
	}




//------------------------------------------------------------------------------
void	ssie7free( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	memfree();
	
	if ( s ) { free( s ); s = null; }
	if ( s1 ) { free( s1 ); s1 = null; }
	if ( s2 ) { free( s2 ); s2 = null; }
	if ( s3 ) { free( s3 ); s3 = null; }
	if ( s4 ) { free( s4 ); s4 = null; }

	if ( fname ) { free( fname ); fname = null; }
	if ( fname1 ) { free( fname1 ); fname1 = null; }
	if ( fname2 ) { free( fname2 ); fname2 = null; }
	if ( fname3 ) { free( fname3 ); fname3 = null; }
	if ( fname4 ) { free( fname4 ); fname4 = null; }

	if ( hPart ) { var_free( hPart ); hPart = 0; }
	if ( hConf ) { var_free( hConf ); hConf = 0; }
	if ( hConf1 ) { var_free( hConf1 ); hConf1 = 0; }
	if ( hConf2 ) { var_free( hConf2 ); hConf2 = 0; }

	if ( ( h )&&( h != ERROR ) ) { var_free( h ); h = 0; }
	if ( ( h1 )&&( h1 != ERROR ) ) { var_free( h1 ); h1 = 0; }
	if ( ( h2 )&&( h2 != ERROR ) ) { var_free( h2 ); h2 = 0; }
	if ( ( h3 )&&( h3 != ERROR ) ) { var_free( h3 ); h3 = 0; }
	}


//------------------------------------------------------------------------------
void	ssie7trace( T_WORK *t_work, char *msg, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg;
	int _i;
    
	#ifdef OS_WINDOWS
	if ( !fpTrace )
		{
		if ( !dir_exists( dirlog ) ) mkdir( dirlog );
		strdcpy( &trace_fname, dirlog, DIR_CHAR, file_getname(argv[0]), ".id", _ );
		_i = get_uid( trace_fname );
		strdcpy( &trace_fname, dirlog, DIR_CHAR, file_getname(argv[0]), ".", itoa(_i), ".trc", _ );
	#else
	if (( !trace_fname )||( !(trace_fname[0]) ))
		{
		strdcpy( &trace_fname, dirlog, DIR_CHAR, file_getname(argv[0]), ".", itoa(getpid()), ".trc", _ );
	#endif
	
                  
    strdcpy( &cmd_arg, argv[0], _ );
	for( _i = 1 ; _i < argc ; _i++ )
		strdcat( &cmd_arg, " ", argv[_i], _ );
		
		fpTrace = fopen( trace_fname, "ab" );
		if ( ! fpTrace ) return;
		for( _i = 1 ; _i < argc ; _i++ )
			fprint( fpTrace, currdate( "log" ), " (pid=", itoa(getpid()), ") :  ", argv[_i], _ );
		fprint( fpTrace, BR, __ );
		}


	fprint( fpTrace, currdate( "log" ), " (pid=", itoa(getpid()), ") ; '", cmd_arg, "' : ", msg, __ );
	va_start(args, msg); 
	while( 1 )
		{
		arg = va_arg(args, char *);
		if ( arg == _ )		{ fprint( fpTrace, BR, __ ); return; }
		if ( arg == __ )	{ fprint( fpTrace, BR, __ ); return; }
		if ( arg ) fprint( fpTrace, arg, __ );
		}
	}



//------------------------------------------------------------------------------
void ssie7trace_close( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( fpTrace ) fclose( fpTrace );
	fpTrace = NULL;
	if ( ( trace_fname )&&( ! trace_no_delete ) )
		{
		unlink( trace_fname );
		trace_fname[0] = 0;
		}
	}
	


//------------------------------------------------------------------------------
bool part_refresh( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if (( ! file_exists( fname ) )
		||( ( time(NULL) - file_date( fname ) ) > (2 Heures) )
		)
		{
		v = var_get( hSrvConf, "part_auto_crea" );
		if (( ( v )&&( !strcmp( v, "Oui" ) ) )
			&&( nat_search() )
			)
			{
			strdcpy( &u6, argv[0], " part ", partname, " loadnat", _ );
			System( &u7, u6 );
			}
		}
	}
	
//------------------------------------------------------------------------------
bool part_exists( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, partname, ".dat", _ );
	if ( !file_exists( fname ) )
		{
		strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR "adl" DIR_CHAR, partname, ".dat", _ );
		if ( file_exists( fname ) )
			{
			file_load( &u15, fname );
			partname = u15;
			}
		strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, partname, ".dat", _ );
		}

	
	part_refresh( t_work );
		
	return file_exists( fname );
	}
	
	


//------------------------------------------------------------------------------


char *	ssie7_logfname = null;
FILE *	ssie7_fplog = NULL;
int		ssie7_log_ntim = 0;


//------------------------------------------------------------------------------
int logw( char *msg, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg;
	int _n = 0;
	int _i;

	#ifdef OS_UNIX
	static int _b = -1;

	if ( _b == -1 )
		_b = strcmp( sysuser(), ssie_user );
		
	if( _b ) return;
	#endif
	
	BeginCriticalSection();

	if ( ( ssie7_logfname == null )||( ssie7_log_ntim < time(NULL) ) )
		{
		strdcpy( &ssie7_logfname, dirlog, "/", module_name, "_", arg=currdate("aaaa_mm_jj"), ".log", _ );
		ssie7_log_ntim = dtoi( arg, "aaaa_mm_jj" ) + (24*60*60);
		}

		
	for( _n = 1000 ; ( ( ssie7_fplog = fopen( ssie7_logfname, "ab" ) ) == NULL )&&( _n ) ; _n-- )
		{
		if ( !dir_exists( dirlog ) ) { mkdir( dirlog ); continue; }
		Sleep( 5 );
		}
		
	EndCriticalSection();
		
	if ( ssie7_fplog == NULL ) return;

	fprint( ssie7_fplog, currdate( "log" ), " : ", msg, __ );
	
	va_start(args, msg); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ )
			{
			fprint( ssie7_fplog, BR, _ );
			fclose( ssie7_fplog );
			return;
			}
		fprint( ssie7_fplog, arg, _ );
		}
	}



//------------------------------------------------------------------------------
int server_conf_load( )
//------------------------------------------------------------------------------
	{
	static int dt_file;
	
	if ( ! server_conf_fname )
		strdcpy( &server_conf_fname, dirconf, DIR_CHAR, module_name, ".dat", _ );
	else
	if ( hSrvConf )
		{
		if ( dt_file == file_date( server_conf_fname ) )
			return;
		}

	dt_file = file_date( server_conf_fname );
	hSrvConf = var_load( server_conf_fname );
	}


//------------------------------------------------------------------------------
int server_conf_reload( )
//------------------------------------------------------------------------------
	{
	if ( hSrvConf ) var_free( hSrvConf );

	if ( ! server_conf_fname )
		strdcpy( &server_conf_fname, dirconf, DIR_CHAR, module_name, ".dat", _ );
		
	hSrvConf = var_load( server_conf_fname );
	}


//------------------------------------------------------------------------------
int server_conf_save( )
//------------------------------------------------------------------------------
	{
	if ( !hSrvConf ) return;
	
	if ( ! server_conf_fname )
		strdcpy( &server_conf_fname, dirconf, "/", module_name, ".dat", _ );

	var_save( hSrvConf, server_conf_fname );
	}



#define PORT_NAT	"1760"

//------------------------------------------------------------------------------
char * nat_search( )
//------------------------------------------------------------------------------
{
	char *nat_list=null;
    char **nat_tab=null;
    char *nat_sel=null;
    int wrk_i=0;

    if ( hSrvConf )
    	nat_list = var_get( hSrvConf, "srvnat_list" );
	if ( ( nat_list )&&( nat_list[0] )&&( !stricmp( nat_list, "none" ) ) )
		return null;
	if ( !nat_list )
         strdcpy( &nat_list, "si-ssie.edf.fr", _ );
		
    nat_tab = str_explode( nat_list, "," );
    for ( wrk_i=0 ; nat_tab[wrk_i] ; wrk_i++ )
    {  		
        nat_sel=nat_tab[wrk_i];
    	if (( !nat_sel )||( ! (nat_sel[0]) ))
    	{
    		nat_sel = "si-ssie.edf.fr";
    		var_set( hSrvConf, "srvnat_list", nat_sel );
    		server_conf_save( );
    	}

    	if ( ! is_ip_valide( nat_sel ) )
    	{
    		nat_sel = tcp_dns_address( nat_sel );
    	    if (( !nat_sel )||( ! (nat_sel[0]) ))
    	       continue;
        }
        break;
	}
	return nat_sel;
}


//------------------------------------------------------------------------------
char * nat_request( char **p_out, char *p_cmd )
//------------------------------------------------------------------------------
	{
	char *nat_list, *srvnat_ip;


	nat_list = nat_search();

	if ( nat_list == null ) return null;

	srvnat_ip = str( nat_list,
		(char*)(( strpos( nat_list, ":", 0 ) < 0 ) ? ":" PORT_NAT : ""), _ );


	if ( tcp_request( srvnat_ip, p_cmd, p_out ) == null )
		{
		sleep( 1 );
		if ( tcp_request( srvnat_ip, p_cmd, p_out ) == null )
			{
			sleep( 1 );
			if ( tcp_request( srvnat_ip, p_cmd, p_out ) == null )
				{
				return null;
				}
			}
		}

	return *p_out;
	}



//------------------------------------------------------------------------------
int cmd_srvannu( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	if ( v = var_get( hReq, "list" ) )
		{
		var_set( hSrvConf, "srvnat_list", v );
		server_conf_save( );
		}
	nat_search();
		
	ssie7print( t_work, "srvannu list = ", var_get( hSrvConf, "srvnat_list" ), BR, _ );
	return OK;
	}


//------------------------------------------------------------------------------
int cmd_natrequest( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	vn = null;
	var_getn( hReq, 1, &vn );
	strdcpy( &cmd, vn, _ );
	nat_request( &s, cmd );
	ssie7print( t_work, s, BR, _ );
	}
	


//------------------------------------------------------------------------------
bool is_authorized0( T_WORK *t_work, char *app, char *right )
//------------------------------------------------------------------------------
	{
	int _p, _p1, _p2, _l1, _l2;
	handle _h;
	char *_v;


	if ( is_admin ) return true;
#ifndef OS_WINDOWS
	if ( !strcmp( user, ssie_user ) )		return is_admin = true;
	if ( !strcmp( user, "root" ) )		return is_admin = true;
	if ( !strcmp( user, "rootmin" ) )	return is_admin = true;
#else
	if ( request_mode != MODE_TCP )		return is_admin = true;
	if ( !strcmp( ip_cli, "127.0.0.1" ) ) return is_admin = true;
#endif
//	if ( ! strcmp( user, NNI_ADM ) )	return is_admin=true;

	if ( session_partname )
		{
		if ( ! strcmp( session_partname, "MFTCCWEX" ) ) return is_admin=true;
		if (( v = var_get( hSrvConf, "mft_gateway" ) )&&( !stricmp( v, session_partname ) ))
			return is_admin=true; // Droits admin si la requête vient d'une passerelle MFT
		}

	if ( ( ! profile )||( ! (profile[0]) ) )
		{
		if ( session_partname )
			{
			if ( !file_exists( strdcpy( &fname, dirconf, DIR_CHAR "login" DIR_CHAR, session_partname, ".dat", _ ) ) )
			if ( !file_exists( strdcpy( &fname, dirconf, DIR_CHAR "login" DIR_CHAR, user, ".dat", _ ) ) )
				return false;
			}
		else
		if ( !file_exists( strdcpy( &fname, dirconf, DIR_CHAR "login" DIR_CHAR, user, ".dat", _ ) ) )
			return false;
		
		_h = var_load( fname );
		
		_v = var_get( _h, "is_admin" );
		if ( ( _v )&&
			(( !strcmp( _v, "Oui" ) )||( !strcmp( _v, "Super" ) ))
			)
			return is_admin = true;
		else is_admin = false;
//		if (( !app )&&( !strcmp( right, "visiteur" ) )&&( v = var_get( _h, "mftcat" ) )
//			&&( !stricmp( v, "TRUE" ) )) return true;
		strdcpy( &profile, var_get( _h, "app_liste" ), _ );
		var_free( _h );
		}
		
	if ( app == null ) return true;
	
	_l1 = strlen( app );
	_l2 = ( right ) ? strlen( right ) : 0;
	
	for( _p = 0 ; ( ( _p = stripos( profile, app, _p ) ) >= 0 ) ; _p++ )
		{
		if ( ( _p )&&( profile[_p-1] != ',' )&&( profile[_p-1] != ' ' ) ) continue;
		if ( profile[_p+_l1] != ':' ) continue;
		_p2 = strpos( profile, ",", _p+_l1 );
		if ( _p2 < 0 ) _v = substr( profile, _p+_l1+1, -1 );
		else _v = substr( profile, _p+_l1+1, _p2-_p-_l1-1 );
		trim(_v);
		if ( !right ) return true;
		
		if ( !stricmp( right, "gestionnaire" ) )
			{
			if ( !stricmp( _v, "gestionnaire" ) ) return true;
			return false;
			}
		if ( !stricmp( right, "editeur" ) )
			{
			if ( !stricmp( _v, "gestionnaire" ) ) return true;
			if ( !stricmp( _v, "éditeur" ) ) return true;
			return false;
			}
		if ( !stricmp( right, "visiteur" ) )
			{
			if ( !stricmp( _v, "gestionnaire" ) ) return true;
			if ( !stricmp( _v, "éditeur" ) ) return true;
			if ( !stricmp( _v, "visiteur" ) ) return true;
			return false;
			}
		return false;
		}
	return false;
	}
	


#ifdef OS_WINDOWS
#define is_sudo_conf_ok()	true

#else	// UNIX

bool is_sudo_conf_ok( void )
	{
	static char *_cmd = null, *_s = null;
	
	#ifdef OS_LINUX
	strdcpy( &_cmd, "sudo -n /logiciels/ssie7/ssie7 app", _ );
	System( &_s, _cmd );
	if ( stripos( _s, "a password is required", 0 ) >= 0 ) return false;
	#else
	strdcpy( &_cmd, "sudo -S /ssie7/ssie7 << fin\nquit\n\n\nfin\n", _ );
	System( &_s, _cmd );
	#endif
	
	if ( stripos( _s, "sorry", 0 ) >= 0 ) return false;
	return true;
	}

#endif



//------------------------------------------------------------------------------

int file_is_free( char *f )
	{
	#ifdef OS_WINDOWS
	return true;
	#endif


	#ifdef OS_LINUX
	static char *_s1 = null, *_cmd = null;
	static bool bFuser = -1;

	if ( !is_sudo_conf_ok() ) return 3;

	if ( bFuser < 0 )
		{
		strdcpy( &_cmd, "which fuser", _ );
		System( &_s1, _cmd );
		trim( _s1 );
		bFuser = file_exists( _s1 );
		}

	if ( bFuser )
		{
		strdcpy( &_cmd, "sudo /logiciels/ssie7/ssie7 /sbin/fuser -f \"'", f, "'\"", _ );
		System( &error_msg, _cmd );
		if ( stripos( error_msg, "No such file or directory", 0 ) >= 0 )
			{
			strdcat( &error_msg, " (", _cmd, ")", _ );
			return 2;
			}
			
		trim( error_msg );
		return ( error_msg[0] == 0 );
		}
	else
		{
		strdcpy( &_cmd, "sudo /logiciels/ssie7/ssie7 lsof | "
						"awk '{print $9 \":\"}' | grep \"", f, ":\"", _ );
		System( &_s1, _cmd );
		return ( strpos( _s1, f, 0 ) < 0 );
		}
	
	
	#endif
	
	
	#ifdef OS_AIX
	static char *_s1 = null, *_s2 = null, *_cmd = null;
	long int _p = 0;
	
	if ( !is_sudo_conf_ok() ) return 3;
	strdcpy( &_cmd, "sudo /ssie7/ssie7 /usr/sbin/fuser -f \"'", f, "'\"", _ );
	System( &_s1, _cmd );
	while( sgets( &_s2, _s1, &_p ) )
		{
		trim( _s2 );
		if ( ! strlen(_s2) ) continue;
		if ( stripos( _s2, "No such file or directory", 0 ) >= 0 )
			return 2;

		return ( _s2[strlen(_s2)-1] == ':' );
		}

	#endif

	}















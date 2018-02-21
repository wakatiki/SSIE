//#include "ssie7c.h"


//------------------------------------------------------------------------------
int cmd_initialize( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	// CONFIGURE LE SERVEUR
	
	mkdir( dirconf );
	mkdir( dirdat );
	mkdir( dirtmp );
	mkdir( dirlog );
	mkdir( dirwrk );
	file_copy( fname1, fname );
	v = getenv( "HOSTNAME" );
	p = strpos( v, ".", 0 );
	if ( p > 0 ) v = upper(substr( v, 0, p ));
	strdcpy( &partname, v, _ );
	
	strdcpy( &cmd, argv[0], " server partname=", partname, " ip=''", _ );
	print( cmd, BR, _ );
	System( &s, cmd );
	print( s, BR, BR, _ );
	

//	v = tcp_dns_address( "si-ssie.edf.fr" );
	v = "10.122.0.81";
	strdcpy( &s4, v, ":1770", _ );
	vn = "1770";
	
	// RECUPERE LES PARTENAIRES ET LES CONFIGURES
	
	if ( ( tcp_request( s4, "part", &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion (1)", BR, _ );
		return ERROR;
		}
	
	ts = str_explode( s1, "\n" );
	strdcpy( &fname1, dirconf, DIR_CHAR "ssie7c.dat", _ );
	
	for( i = 1 ; ts[i] ; i++ )
		{
		print( ts[i], BR, _ );
		ts2 = str_explode( ts[i], "|" );
		if ( ( !(ts2[0][0]) )||( !(ts2[1]) )||( !(ts2[2]) )||( !(ts2[3]) ) )
			{ free( ts2 ); continue; }

		strdcpy( &fname, dirconf, DIR_CHAR "part", _ );
		mkdir( fname );
		strdcat( &fname, DIR_CHAR, ts2[0], ".dat", _ );
		file_copy( fname1, fname );
		
		#ifdef OS_WINDOWS
		strdcpy( &cmd, argv[0], " part ", ts2[0], " ip=", ts2[2],
				" port=", ts2[3], " enable=Oui files_dir='c:'", _ );
		#else
		strdcpy( &cmd, argv[0], " part ", ts2[0], " ip=", ts2[2],
				" port=", ts2[3], " enable=Oui files_dir=/", _ );
		#endif
		print( cmd, BR, _ );
		system( cmd );


		strdcpy( &cmd, "ssie7c_part name=", partname, " ip=", tcp_myIP(0),
				" port=", vn, _ );
		strdcpy( &s, ts2[2], ":", ts2[3], _ );
		print( s, " => ", cmd, BR, _ );
		tcp_request( s, cmd, &s );
		print( s, BR, _ );
		}

//	file_delete( "certif*.tmp" );
	
	return OK;		
	}



//------------------------------------------------------------------------------
int cmd_ssie7c_part( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	// Récupére les certificats

	strdcpy( &fname1, dirconf, DIR_CHAR "ssie7c.dat", _ );
	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, upper(var_get( hReq, "name" )), ".dat", _ );
	file_copy( fname1, fname );

	
	strdcpy( &cmd, argv[0], " part ", var_get( hReq, "name" ),
			" ip=", var_get( hReq, "ip" ),
			" port=", var_get( hReq, "port" ),
			" enable=Oui",
			" files_dir=",
				#ifdef OS_WINDOWS
				"'c:'"
				#else
				"/"
				#endif
			, _ );
	

	System( &s, cmd );
	print( cmd, BR, s, BR, _ );

	return OK;		
	}
















	
	

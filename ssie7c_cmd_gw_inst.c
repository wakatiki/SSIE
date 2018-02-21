//#include "ssie7c.h"


//------------------------------------------------------------------------------
char* putfile( char *pname, char *fsrc, char *fdest, int bAscii )
//------------------------------------------------------------------------------
	{
	static char *_cmd = null, *res = null;
	
	strdcpy( &_cmd, "putfile ", pname, " ", fsrc, " dest=", fdest,
			(char*)((bAscii)?" ascii":""), _ );
	file_save_str( "/tmp/putfile.cmd", _cmd, -1 );
	printf( "%s" BR, _cmd );

	strdcpy( &_cmd, "ssie7c" EXE " @/tmp/putfile.cmd", _ );
	
	System( &res, _cmd );
	printf( "%s" BR, res );
	
	return res;
	}
	
//------------------------------------------------------------------------------
char* getfile( char *pname, char *fsrc, char *fdest, int bAscii )
//------------------------------------------------------------------------------
	{
	static char *_cmd = null, *res = null;
	
	strdcpy( &_cmd, "getfile ", pname, " ", fsrc, " dest=", fdest,
			(char*)((bAscii)?" ascii":""), _ );
	file_save_str( "/tmp/getfile.cmd", _cmd, -1 );
	printf( "%s" BR, _cmd );

	strdcpy( &_cmd, "ssie7c" EXE " @/tmp/getfile.cmd", _ );
	
	System( &res, _cmd );
	printf( "%s" BR, res );

	#ifndef OS_WINDOWS
	if ( !file_exists( fdest ) ) exit(0);
	#endif

	return res;
	}


//------------------------------------------------------------------------------
int cmd_gw_inst( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	// INSTALLATION D'UNE PASSERELLE MFT - SSIE, OU MONTEE DE VERSION
	
	v = var_getn( hReq, 1, &partname );
	if ( ! v )
		{
		print( "ERREUR syntaxe : gw_inst {partname de téléchargement}" BR, _ );
		return ERROR;
		}
	
	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, upper(partname), ".dat", _ );
	if ( ! file_exists( fname ) )
		{
		print( "ERREUR : le partenaire n'existe pas" BR, _ );
		return ERROR;
		}
	
	hPart = var_load( fname );
	strdcpy( &srv_ip, var_get( hPart, "ip_ssie" ), ":", var_get( hPart, "port" ), _ );
	
	
	// CREATION DES REPERTOIRES
	
	mkdir( "/appli/ssie7annu/conf" );
	mkdir( "/appli/ssie7annu/data" );
	mkdir( "/appli/ssie7annu/log" );
	mkdir( "/appli/ssie7annu/tmp" );
	
	mkdir( "/appli/ssie7/conf" );
	mkdir( "/appli/ssie7/data" );
	mkdir( "/appli/ssie7/data/mft" );
	mkdir( "/appli/ssie7/data/gen_certif" );
	mkdir( "/appli/ssie7/log" );
	mkdir( "/appli/ssie7/tmp" );
	
	system( "ls -lR /appli/ssie7" );
	
	// TELECHARGE LES BINAIRES

	print( "TELECHARGE LES BINAIRES SSIE7" BR, _ );
	
	strdcpy( &fname, "/logiciels/ssie7/ssie7", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 ssie7" );

	strdcpy( &fname, "/logiciels/ssie7/ssie7annu", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 ssie7annu" );
	
	getfile( partname, "/appli/ssie7annu/conf/ssie7annu.dat", "/appli/ssie7annu/conf/ssie7annu.dat", false );
	getfile( partname, "/appli/ssie7annu/conf/srvannu_list.cfg", "/appli/ssie7annu/conf/srvannu_list.cfg", false );
	getfile( partname, "/appli/ssie7/conf/ssie7.dat", "/appli/ssie7/conf/ssie7.dat", false );
	getfile( partname, "/appli/ssie7/conf/mft.dat", "/appli/ssie7/conf/mft.dat", false );

	strdcpy( &fname, "/logiciels/ssie7/ssie7start", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 ssie7start" );

	strdcpy( &fname, "/logiciels/ssie7/ssie7stop", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 ssie7stop" );

	strdcpy( &fname, "/logiciels/ssie7/MFTUTIL", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 MFTUTIL" );

	strdcpy( &fname, "/logiciels/ssie7/mersi3cli", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 mersi3cli" );


	print( "TELECHARGE MFT" BR, _ );
	
	strdcpy( &cmd, "cd /logiciels/ssie7;tar cfz /tmp/mft2.7.1.tar.gz mft", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/mft2.7.1.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/mft2.7.1.tar.gz", "/logiciels/ssie7/mft2.7.1.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/mft2.7.1.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/logiciels/ssie7" );
	system( "tar xfz mft2.7.1.tar.gz" );
	system( "rm mft2.7.1.tar.gz" );



	strdcpy( &cmd, "cd /logiciels/ssie7;tar cfz /tmp/ssie7certif.tar.gz certif", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/ssie7certif.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/ssie7certif.tar.gz", "/logiciels/ssie7/ssie7certif.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/ssie7certif.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/logiciels/ssie7" );
	system( "tar xfz ssie7certif.tar.gz" );
	system( "rm ssie7certif.tar.gz" );

	
	print( BR "TELECHARGE L'ANNUAIRE NAT" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7annu/data;tar cfz /tmp/ssie7annu_nat.tar.gz part", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/ssie7annu_nat.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/ssie7annu_nat.tar.gz", "/logiciels/ssie7/ssie7annu_nat.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/ssie7annu_nat.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7annu/data" );
	system( "tar xfz /logiciels/ssie7/ssie7annu_nat.tar.gz" );
	system( "rm /logiciels/ssie7/ssie7annu_nat.tar.gz" );



	print( BR "TELECHARGE LA CONFIGURATION SSL" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7annu/data;tar cfz /tmp/ssie7annu_ssl.tar.gz gen_certif", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/ssie7annu_ssl.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/ssie7annu_ssl.tar.gz", "/logiciels/ssie7/ssie7annu_ssl.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/ssie7annu_ssl.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7annu/data" );
	system( "tar xfz /logiciels/ssie7/ssie7annu_ssl.tar.gz" );
	system( "rm /logiciels/ssie7/ssie7annu_ssl.tar.gz" );



	print( BR BR "TELECHARGE L'ANNUAIRE SSIE" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7/conf;tar cfz /tmp/ssie7annu.tar.gz part", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/ssie7annu.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/ssie7annu.tar.gz", "/logiciels/ssie7/ssie7annu.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/ssie7annu.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7/conf" );
	system( "tar xfz /logiciels/ssie7/ssie7annu.tar.gz" );
	system( "rm /logiciels/ssie7/ssie7annu.tar.gz" );



	print( BR BR "TELECHARGE L'ANNUAIRE MFT" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7/conf ; tar cfz /tmp/mft_annu.tar.gz mft", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/mft_annu.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/mft_annu.tar.gz", "/logiciels/ssie7/mft_annu.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/mft_annu.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7/conf" );
	system( "tar xfz /logiciels/ssie7/mft_annu.tar.gz" );
	system( "rm /logiciels/ssie7/mft_annu.tar.gz" );


	print( BR BR "TELECHARGE LE FICHIER DE CONFIGURATION DES PARTENAIRES MFT" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7/data/mft;tar cfz /tmp/mft_part.tar.gz mft_part mft_part.idx", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/mft_part.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/mft_part.tar.gz", "/logiciels/ssie7/mft_part.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/mft_part.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7/data/mft" );
	system( "tar xfz /logiciels/ssie7/mft_part.tar.gz" );
	system( "rm /logiciels/ssie7/mft_part.tar.gz" );

/*
	print( BR "TELECHARGE L'ANNUAIRE NAT" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7annu/data;tar cfz /tmp/ssie7annu_nat.tar.gz part", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/ssie7annu_nat.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/ssie7annu_nat.tar.gz", "/logiciels/ssie7/ssie7annu_nat.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/ssie7annu_nat.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7annu/data" );
	system( "tar xfz /logiciels/ssie7/ssie7annu_nat.tar.gz" );
	system( "rm /logiciels/ssie7/ssie7annu_nat.tar.gz" );
*/

	print( BR BR "TELECHARGE LES LOGIN" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7annu/data;tar cfz /tmp/ssie7login.tar.gz login", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/ssie7login.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/ssie7login.tar.gz", "/logiciels/ssie7/ssie7login.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/ssie7login.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7annu/data" );
	system( "tar xfz /logiciels/ssie7/ssie7login.tar.gz" );
	system( "rm /logiciels/ssie7/ssie7login.tar.gz" );



	print( BR BR "CONFIGURE SSIE7ANNU" BR, _ );

	chdir( "/logiciels/ssie7" );
	system( "ssie7annu server ip=''" );
	

	print( BR BR "CONFIGURE SSIE7" BR, _ );

	system( "ssie7 server ip=''" );
	}


//------------------------------------------------------------------------------
int cmd_perf_inst( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	// INSTALLATION D'UNE PASSERELLE MFT - SSIE, OU MONTEE DE VERSION
#ifdef OS_LINUX
       char *bin_dir = "/logiciels/ssie7";
#elif OS_AIX	
       char *bin_dir = "/ssie7";
#endif

#ifndef OS_WINDOWS
	v = var_getn( hReq, 1, &partname );
	if ( ! v )
		{
		print( "ERREUR syntaxe : perf_inst {partname de téléchargement}" BR, _ );
		return ERROR;
		}
	
	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, upper(partname), ".dat", _ );
	if ( ! file_exists( fname ) )
		{
		print( "ERREUR : le partenaire n'existe pas" BR, _ );
		return ERROR;
		}
	
	hPart = var_load( fname );
	strdcpy( &srv_ip, var_get( hPart, "ip_ssie" ), ":", var_get( hPart, "port" ), _ );
	
	
	// CREATION DES REPERTOIRES
	
	mkdir( "/appli/ssie7perf/conf" );
	mkdir( "/appli/ssie7perf/data" );
	mkdir( "/appli/ssie7perf/log" );
	mkdir( "/appli/ssie7perf/mft" );
	mkdir( "/appli/ssie7perf/mft/conf" );
	mkdir( "/appli/ssie7perf/mft/log" );
	mkdir( "/appli/ssie7perf/mft/data" );
	mkdir( "/appli/ssie7perf/mft/data/recv" );
	mkdir( "/appli/ssie7perf/mft/data/reveils" );
	
	mkdir( "/appli/ssie7/conf" );
	mkdir( "/appli/ssie7/data" );
	mkdir( "/appli/ssie7/data/mft" );
	mkdir( "/appli/ssie7/data/gen_certif" );
	mkdir( "/appli/ssie7/log" );
	mkdir( "/appli/ssie7/tmp" );
	
	system( "ls -lR /appli/ssie7" );
	
	// TELECHARGE LES BINAIRES

	print( "TELECHARGE LES BINAIRES SSIE7" BR, _ );
	
	strdcpy( &fname, bin_dir, "/ssie7", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 ssie7" );

//	getfile( partname, "/appli/ssie7/conf/ssie7.dat", "/appli/ssie7/conf/ssie7.dat", false );
//	getfile( partname, "/appli/ssie7/conf/mft.dat", "/appli/ssie7/conf/mft.dat", false );

/*
	strdcpy( &fname, bin_dir, "/ssie7start", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 ssie7start" );

	strdcpy( &fname, bin_dir, "/ssie7stop", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 ssie7stop" );
*/
	strdcpy( &fname, bin_dir, "/MFTUTIL", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 MFTUTIL" );
/*
	strdcpy( &fname, bin_dir, "/ssie7/mersi3cli", _ );
	getfile( partname, fname, fname, false );
	system( "chmod 755 mersi3cli" );
*/

	print( "TELECHARGE MFT" BR, _ );
	
	strdcpy( &cmd, "cd ", bin_dir, " ; tar cfz /tmp/mft2.7.1.tar.gz mft", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/mft2.7.1.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/mft2.7.1.tar.gz ", str( bin_dir, "/mft2.7.1.tar.gz", _ ), false );

	strdcpy( &cmd, "rm /tmp/mft2.7.1.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( bin_dir );
	system( "tar xfz mft2.7.1.tar.gz" );
	system( "rm mft2.7.1.tar.gz" );



	strdcpy( &cmd, "cd ", bin_dir, " ; tar cfz /tmp/ssie7certif.tar.gz certif", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/ssie7certif.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/ssie7certif.tar.gz ", str( bin_dir, "/ssie7certif.tar.gz", _ ), false );

	strdcpy( &cmd, "rm /tmp/ssie7certif.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( bin_dir );
	system( "tar xfz ssie7certif.tar.gz" );
	system( "rm ssie7certif.tar.gz" );


	system( str( "mv ", bin_dir, "/mft/mft ", bin_dir, "/mft/mft.INACTIF", _ ) );
	
	print( BR BR "CONFIGURE SSIE7" BR, _ );

// Utilisation de NOEYY4L9 comme serveur d'annuaire.
	system( "ssie7 srvannu list='10.122.47.38:1760'" );
// Récupération de la configuration du serveur depuis le serveur d'annuaire.
	system( "ssie7 server ip=''" );
#endif
	}



//------------------------------------------------------------------------------
int cmd_gw_sync( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	// SYNCHRONISATION D'UNE PASSERELLE MFT - SSIE.
	
	v = var_getn( hReq, 1, &partname );
	if ( ! v )
		{
		print( "ERREUR syntaxe : gw_sync {partname de téléchargement}" BR, _ );
		return ERROR;
		}
	
	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, upper(partname), ".dat", _ );
	if ( ! file_exists( fname ) )
		{
		print( "ERREUR : le partenaire n'existe pas" BR, _ );
		return ERROR;
		}
	
	hPart = var_load( fname );
	strdcpy( &srv_ip, var_get( hPart, "ip_ssie" ), ":", var_get( hPart, "port" ), _ );
	
    
	print( BR BR "ARRET DE SSIE7ANNU" BR, _ );
	system( "/logiciels/ssie7/ssie7annu stop ; sleep 5" );
	
	print( BR "SAUVEGARDE L'ANNUAIRE NAT" BR, _ );
	file_move("/appli/ssie7annu/data/part", "/appli/ssie7annu/data/part.SAV");
	
	print( BR "TELECHARGE L'ANNUAIRE NAT" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7annu/data ; tar cfz /tmp/ssie7annu_nat.tar.gz part", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/ssie7annu_nat.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/ssie7annu_nat.tar.gz", "/logiciels/ssie7/ssie7annu_nat.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/ssie7annu_nat.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7annu/data" );
	system( "tar xfz /logiciels/ssie7/ssie7annu_nat.tar.gz" );
	system( "rm /logiciels/ssie7/ssie7annu_nat.tar.gz" );


	print( BR "SUPPRIME LA SAUVEGARDE DE L'ANNUAIRE NAT" BR, _ );
	file_delete("/appli/ssie7annu/data/part.SAV");

	print( BR BR "DEMARRAGE DE SSIE7ANNU" BR, _ );
	system( "/logiciels/ssie7/ssie7annu start ; sleep 5" );

	
	


	print( BR BR "ARRET DE SSIE7" BR, _ );
	system( "/logiciels/ssie7/ssie7 stop ; sleep 5" );

	print( BR BR "SAUVEGARDE L'ANNUAIRE SSIE" BR, _ );
	file_move("/appli/ssie7/conf/part", "/appli/ssie7/conf/part.SAV");
	
	print( BR "TELECHARGE L'ANNUAIRE SSIE" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7/conf ; tar cfz /tmp/ssie7annu.tar.gz part", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/ssie7annu.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/ssie7annu.tar.gz", "/logiciels/ssie7/ssie7annu.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/ssie7annu.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7/conf" );
	system( "tar xfz /logiciels/ssie7/ssie7annu.tar.gz" );
	system( "rm /logiciels/ssie7/ssie7annu.tar.gz" );

	print( BR "SUPPRIME LA SAUVEGARDE DE L'ANNUAIRE SSIE" BR, _ );
	file_delete("/appli/ssie7/conf/part.SAV");

	


	print( BR "SAUVEGARDE L'ANNUAIRE MFT" BR, _ );
	file_move("/appli/ssie7/conf/mft", "/appli/ssie7/conf/mft.SAV");
	
	print( BR BR "TELECHARGE L'ANNUAIRE MFT" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7/conf ; tar cfz /tmp/mft_annu.tar.gz mft", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/mft_annu.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/mft_annu.tar.gz", "/logiciels/ssie7/mft_annu.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/mft_annu.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7/conf" );
	system( "tar xfz /logiciels/ssie7/mft_annu.tar.gz" );
	system( "rm /logiciels/ssie7/mft_annu.tar.gz" );

	print( BR "SUPPRIME LA SAUVEGARDE DE L'ANNUAIRE MFT" BR, _ );
	file_delete("/appli/ssie7/conf/part.SAV");

	


	print( BR BR "SAUVEGARDE LE FICHIER DE CONFIGURATION DES PARTENAIRES MFT" BR, _ );
	file_move("/appli/ssie7/data/mft/mft_part", "/appli/ssie7/conf/mft/mft_part.SAV");
	file_move("/appli/ssie7/data/mft/mft_part.idx", "/appli/ssie7/conf/mft/mft_part.idx.SAV");
	
	print( BR "TELECHARGE LE FICHIER DE CONFIGURATION DES PARTENAIRES MFT" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7/data/mft ; tar cfz /tmp/mft_part.tar.gz mft_part mft_part.idx", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/mft_part.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/mft_part.tar.gz", "/logiciels/ssie7/mft_part.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/mft_part.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7/data/mft" );
	system( "tar xfz /logiciels/ssie7/mft_part.tar.gz" );
//	system( "rm /logiciels/ssie7/mft_part.tar.gz" );

	print( BR "SUPPRIME LA SAUVEGARDE DU FICHIER DE CONFIGURATION DES PARTENAIRES MFT" BR, _ );
	file_delete("/appli/ssie7/conf/mft/mft_part.SAV");
	file_delete("/appli/ssie7/conf/mft/mft_part.idx.SAV");

	print( BR BR "DEMARRAGE DE SSIE7" BR, _ );
	system( "/logiciels/ssie7/ssie7 start ; sleep 5" );

	

/*
	print( BR "TELECHARGE L'ANNUAIRE NAT" BR, _ );
	
	strdcpy( &cmd, "cd /appli/ssie7annu/data;tar cfz /tmp/ssie7annu_nat.tar.gz part", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	strdcpy( &cmd, "ls -l /tmp/ssie7annu_nat.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	getfile( partname, "/tmp/ssie7annu_nat.tar.gz", "/logiciels/ssie7/ssie7annu_nat.tar.gz", false );

	strdcpy( &cmd, "rm /tmp/ssie7annu_nat.tar.gz", _ );
	print( BR, cmd, BR, _ );
	if ( ( tcp_request( srv_ip, cmd, &s1 ) == sERROR )||( !s1 ) )
		{
		print( "ERREUR connexion : ", srv_ip, BR, _ );
		return ERROR;
		}
	print( s1, BR BR, _ );
	
	chdir( "/appli/ssie7annu/data" );
	system( "tar xfz /logiciels/ssie7/ssie7annu_nat.tar.gz" );
	system( "rm /logiciels/ssie7/ssie7annu_nat.tar.gz" );
*/

	}


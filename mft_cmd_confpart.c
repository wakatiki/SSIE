//#include "ssie7lib.h"
//#include "mft.h"

#define bSSL	b2


//------------------------------------------------------------------------------
int cmd_confpart( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &partname );
	if ( !v ) return ERROR;
	upper( partname );
	
	if ( ! strcmp( partname, "ALL" ) )
		{
		strdcpy( &fname, mft_dirdat, DIR_CHAR "mft_part*", _ );
		file_delete( fname );
		strdcpy( &fname, mft_dirdat, DIR_CHAR "mft_part", _ );
		mftutil( strdcpy( &s, "mftfile type=part,fname=", fname, _ ), &s1 );
		print( s1, BR, _ );
		if ( !file_exists( fname ) )
			return ERROR;

		tf = NULL;
		strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, "*.dat", _ );
		for( i = 0 ; tf = file_list( tf, fname, "nodir" ) ; )
			{
			if ( !stricmp( tf->name, "all.dat" ) ) continue;
			
			i++;
			strdcpy( &cmd, argv[0], " confpart ", tf->name, _ );
			p = strpos( cmd, ".dat", 0 );
			cmd[p] = 0;
			strdcat( &cmd, " no_rebuild", _ );
			print( BR BR, itoa( i ), " : ", cmd, " : ", _ );
			system( cmd );
			}
		return OK;
		}
	
	if ( var_get( hReq, "delete" ) )
		{
		strdcpy( &cmd, "mftpart id=", partname, ", mode=delete", _ );
		mftutil( cmd, &s );
		print( s, BR, _ );
		strdcpy( &cmd, "mfttcp id=", partname, ", mode=delete", _ );
		mftutil( cmd, &s );
		print( s, BR, _ );
		return OK;
		}
		
	strdcpy( &fname, dirconf, DIR_CHAR "part" DIR_CHAR, partname, ".dat", _ );
	if ( ! file_exists( fname ) ) return ERROR;
	hPart = var_load( fname );
	
	strdcpy( &fname3, mft_dirconf, DIR_CHAR "part", _ );
	
	mkdir( fname3 );
	strdcat( &fname3, DIR_CHAR, partname, ".cfg", _ );

	if ( ( file_exists( fname3 ) )&&( var_get( hReq, "no_rebuild" ) ) )
		goto mft_exec;

	fp = fopen( fname3, "wb" );
	
//print( "coucou" BR, _ );
	
	
	//--------------------------------------------------------------------------
	// MFTPART
	//--------------------------------------------------------------------------
	
	fprint( fp,	"MFTPART ID       = ", partname, "," BR,
				"        COMMUT   = SERVER,", BR,
				"        SYST     = ",
		_ );


	// L'OS
	
	v4 = "UNIX";
	v = var_get( hPart, "ip_ssie" );
//logw( "v4=", v4, ", v=", v, _ );
	if ( ( !v )||( ! (v[0]) ) )
		{
		v = var_get( hPart, "os" );
		if ( !v )
			{
			fclose( fp );
			print( "ERREUR : L'OS n'est pas fourni", _ );
			return ERROR;
			}
			
		if ( strmicmp( v, "windows|os2|winnt" ) )
			v4 = "WINNT";
		else
		if ( !stricmp( v, "mvs" ) )
			v4 = "MVS";
		else
		if ( !stricmp( v, "gcos7" ) )
			v4 = "GCOS7";
		}
		
	fprint( fp,	v4, "," BR, _ );
	
	
	// DETERMINE LE MODE SSL (0=pas de SSL, 1=SSL auth serveur, 2=Auth mutuelle)
	
	bSSL = atoi( var_get( hPart, "ssl_enable" ) );
	
//print( "coucou1:bSSL=", itoa(bSSL), BR, _ );
	
	// LE PROTOCOLE ET SAP
	
	if ( ! bSSL )
		{
		v = var_get( hPart, "port_mft" );
		if ( ( !v )||( !(v[0]) ) )	v = var_get( hPart, "port" );
		if ( ( !v )||( !(v[0]) ) )	v = "1761";
		fprint( fp,
				"        PROT     = PSITETCP,", BR,
				"        SAP      = ", v, BR,
				_ );
		}
	else
	if ( bSSL == 1 )
		{
		v = var_get( hPart, "port_ssl" );
		if ( ( !v )||( !(v[0]) ) ) v = var_get( hPart, "port_mft" );
		if ( ( !v )||( !(v[0]) ) ) v = var_get( hPart, "port" );
		if ( ( !v )||( !(v[0]) ) ) v = "1762";
		fprint( fp,
				"        PROT     = PSITSSLS,", BR,
				"        SAP      = ", v, ",", BR,
				"        SSL      = SSLCLI", BR,
				_ );
		}
	else
		{
		v = var_get( hPart, "port_ssl2" );
		if ( ( !v )||( !(v[0]) ) ) v = var_get( hPart, "port_mft" );
		if ( ( !v )||( !(v[0]) ) ) v = var_get( hPart, "port" );
		if ( ( !v )||( !(v[0]) ) ) v = "1763";
		fprint( fp,
				"        PROT     = PSITSSLM,", BR,
				"        SAP      = ", v, ",", BR,
				"        SSL      = SSLCLI", BR,
				_ );
		}


	
	//--------------------------------------------------------------------------
	// MFTPART
	//--------------------------------------------------------------------------
	
	strdcpy( &cmd, "", _ );
	v = var_get( hPart, "dns" );
	if (( v )&&( !stricmp( v, "si-ssie.edf.fr" ) ))
		v = var_get( hPart, "dns_ssie" );
	if ( ( v )&&( v[0] ) )
		{
		if ( !tcp_dns_address( v ) )
			{
			print( "ATTENTION DNS INVALIDE : ", v, " ", _ );
			v = null;
			}
		else
			strdcat( &cmd, "'", v, "'", _ );
		}
	
	v = var_get( hPart, "ip_ssie" );
	if ( ( v )&&( (v[0]) ) )
		strdcat( &cmd, (char*)((cmd[0]) ? "," : ""), "'", v, "'", _ );
	
	v = var_get( hPart, "ip" );
	if ( ( v )&&( (v[0]) ) )
		{
		if ( strpos( v, ",", 0 ) < 0 )
			strdcat( &cmd, (char*)((cmd[0]) ? "," : ""), "'", v, "'", _ );
		else
			{
			ts = str_explode( v, "," );
			for( i = 0 ; ts[i] ; i++ )
				{
				str_unquote( ts[i] );
				if ( ts[i][0] == 0 ) continue;
				strdcat( &cmd, (char*)((cmd[0]) ? "," : ""), "'", ts[i], "'", _ );
				}
			free( ts );
			}
		}

	fprint( fp,	BR
				"MFTTCP  ID       = ", partname, "," BR,
				"        HOST     = (", cmd, ")," BR,
				"        CNXIN    = " CNX "," BR,
				"        CNXOUT   = " CNX "," BR,
				"        CNXINOUT = " CNX,
				_ );


	v = var_get( hPart, "mft_retry" );
	if ( ( v )&&( v[0] ) )
		{
		ts = str_explode( v, "," );
		
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! ts[i][0] ) continue;
			switch( i )
				{
				case 0 :
					fprint( fp,	"," BR "        RETRYW   = ", ts[i], _ );
					break;
				case 1 :
					fprint( fp,	"," BR "        RETRYM   = ", ts[i], _ );
					break;
				case 2 :
					fprint( fp,	"," BR "        RETRYN   = ", ts[i], _ );
					break;
				}
			}
		}

	fprint( fp,	BR BR, _ );



	//--------------------------------------------------------------------------
	// CONFIGURATION SSL
	//--------------------------------------------------------------------------
//print( "coucou2" BR, _ );
	
//	if ( bSSL == 2 )
	if ( bSSL )
		{
		// RECHERCHE LE CERTIFICAT ROOT
//print( "coucou3" BR, _ );
		
		v1 = null; b = false;

		for( i = 1 ; ; i++ )
			{
			strdcpy( &s, "ssl_certif_ca", itoa(i), _ );
			v = var_get( hPart, s );
			if ( !v ) break;
			v1 = v;
			}

//		v1 = var_get( hPart, "ssl_certifr" );			
		if ( v1 )
			{
//print( "Conf certif SSL" BR, _ );
			strdcpy( &s1, v1, _ );
			trim( s1 );
			strd_replace( &s1, "\r", "" );
			strdcpy( &fname, mft_dirconf, DIR_CHAR "certif", _ );
			mkdir( fname );
			tf = NULL;
			while( tf = file_list( tf, fname, "nodir absdir" ) )
				{
				strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
				file_load( &s, fname );
				trim( s );
				strd_replace( &s, "\r", "" );
				if ( !strcmp( s, s1 ) )
					{ b = true; break; }
				}
			
			if ( !b ) // Nouveau certificat root
				{
				for( i = 1 ; ; i++ )
					{
					strdcpy( &fname, mft_dirconf, DIR_CHAR "certif" DIR_CHAR "root", itoa(i), ".pem", _ );
					if ( ! file_exists( fname ) ) break;
					}
				file_save_str( fname, v1, -1 );
				strdcpy( &s2, "ROOT", itoa(i), _ );
				
				if ( pkiutil( strdcpy( &cmd,
					"PKICER ID=", s2, ",ITYPE=ROOT,IFORM=PEM,"
						"INAME=", fname, ",pkipassw='SSIE7',"
						"PKIFNAME=", mft_dirdat, DIR_CHAR "mft_pki,"
						"ROOTCID=", s2,
						_ ), &s ) == ERROR )
					{
					print( "ERREUR Configuration PKI Certificat ROOT" BR, cmd, BR, s, BR, _ );
					return ERROR;
					}
				
				mft_conf_ssl2( t_work );
				}
			}
		}
	
	fclose( fp );
	
	// PRISE EN COMPTE DANS MFT
	mft_exec:
		
	strdcpy( &cmd, MFTCHAR, fname3, _ );
	mftutil( cmd, &s );

	print( s, BR, _ );
	
	print( "OK", _ );
	}



//------------------------------------------------------------------------------
int mft_conf_ssl2( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &cmd,"MFTSSL \"ID=SSLCLI,direct=client,verify=required,"
		"ciphlist=(47,10,9,5,4),usercid='USER',rootcid=(ROOT0", _ );
	
	for( i = 1 ; i < 1000 ; i++ )
		{
		strdcpy( &fname, mft_dirconf, DIR_CHAR "certif" DIR_CHAR "root", itoa(i), ".pem", _ );
		if ( !file_exists( fname ) ) continue;
		strdcat( &cmd, ",ROOT", itoa(i), _ );
		}
	strdcat( &cmd, ")\"", _ );
	
	
	if ( mftutil( cmd, &s ) == ERROR )
		{
		print( "ERREUR : Configuration MFTSSL : ", BR, cmd, BR, s, BR, _ );
		return ERROR;
		}
		
		
	hSrvConf = var_load( server_conf_fname );
		
	strdcpy( &s3, "", _ );
	v1 = trim(var_get( hSrvConf, "ssl_certif_ca2" ));
	if ( ( v1 )&&( v1[0] ) )
		strdcat( &s3, "INTER1", _ );
	v1 = trim(var_get( hSrvConf, "ssl_certif_ca3" ));
	if ( ( v1 )&&( v1[0] ) )
		strdcat( &s3, (char*)((s3[0])?",":""), "INTER2", _ );
	v1 = trim(var_get( hSrvConf, "ssl_certif_ca4" ));
	if ( ( v1 )&&( v1[0] ) )
		strdcat( &s3, (char*)((s3[0])?",":""), "INTER3", _ );
	strdcat( &s3, (char*)((s3[0])?",":""), "ROOT0", _ );

	strdcpy( &cmd,"MFTSSL \"ID=SSLSRVM,direct=server,verify=required,"
		"ciphlist=(47,10,9,5,4),usercid='USER',rootcid=(", s3, _ );
		
	for( i = 1 ; i < 1000 ; i++ )
		{
		strdcpy( &fname, mft_dirconf, DIR_CHAR "certif" DIR_CHAR "root", itoa(i), ".pem", _ );
		if ( !file_exists( fname ) ) continue;
		strdcat( &cmd, ",ROOT", itoa(i), _ );
		}
		
	strdcat( &cmd, ")\"", _ );

	if ( mftutil( cmd, &s ) == ERROR )
		{
		print( "ERREUR : Configuration MFTSSL : ", BR, cmd, BR, s, BR, _ );
		return ERROR;
		}
		
	}


#undef bSSL



























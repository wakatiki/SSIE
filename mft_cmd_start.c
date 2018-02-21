//#include "ssie7lib.h"
//#include "mft.h"

#define	mftkey		(t_work->u1)

//------------------------------------------------------------------------------
int cmd_start( T_WORK * t_work, char bRestart )
//------------------------------------------------------------------------------
	{
	int maxtrans, bSSL = false, bConfAllPart = false;
	
	
	//--------------------------------------------------------------------------
	// VERIFIE QUE MFT N'EST PAS DEJA DEMARRE
	//--------------------------------------------------------------------------
	
	if ( !bRestart )
		{	
		strdcpy( &fname, dirwrk, DIR_CHAR "mft_is_started", _ );
		if ( file_exists( fname ) )
			{
			print( "MFT est déjà démarré", BR, _ );
			return OK;
			}
		}

		
	strdcpy( &fname, dirwrk, DIR_CHAR "mft_*.pid", _ );
	file_delete( fname );
	

	//--------------------------------------------------------------------------
	// VERIFIE LA CONFIGURATION DE MFT ET DE SSIE7
	//--------------------------------------------------------------------------
	
	hSrvConf = var_load( server_conf_fname );
	hMFT = var_load( mft_conf_fname );
	
	partname = var_get( hSrvConf, "partname" );
	if ( ( !partname )||( !(partname[0]) ) )
		{
		print( v = "ERREUR : Le nom de partenaire n'a pas été configuré dans SSIE7", BR, _ );
		logw( "mft start : ", v, _ );
		return ERROR;
		}

	mftkey = var_get( hMFT, "key" );

	if ( ( mftkey )&&( mftkey[0] == '@' ) )
		{
		if ( ! file_load( &s, &(mftkey[1]) ) )
			{
			print( "ERREUR : Le fichier de clef MFT '", mftkey, " n'existe pas ou n'est pas accessible." BR, _ );
			logw( "mft start : ERREUR : Le fichier de clef MFT '", mftkey, " n'existe pas ou n'est pas accessible.", _ );
			return ERROR;
			}
		mftkey = trim(s);
		}
	
	if ( ( !mftkey )||( !(mftkey[0]) ) )
		{
		print( "ERREUR : La clef de licence MFT n'a pas été fournie (conf key='...')" BR, _ );
		logw( "mft start : ERREUR : La clef de licence MFT n'a pas été fournie (conf key='...')", _ );
		return ERROR;
		}
	
	if ( atoi( var_get( hMFT, "cat_size" ) ) < 100 )
		{
		print( "ERREUR : La taille du catalogue n'est pas valide" BR, _ );
		logw( "mft start : ERREUR : La taille du catalogue n'est pas valide", _ );
		return ERROR;
		}
	
	if ( atoi( var_get( hMFT, "com_size" ) ) < 100 )
		{
		print( "ERREUR : La taille du fichier de communication n'est pas valide" BR, _ );
		logw( "mft start : ERREUR : La taille du fichier de communication n'est pas valide", _ );
		return ERROR;
		}



	//--------------------------------------------------------------------------
	// CREATION DES FICHIERS DE TRAVAIL
	//--------------------------------------------------------------------------
	
	print( "Configuration de MFT" BR, _ );	
	logw( "mft start : Configuration de MFT", _ );

		
	mkdir( mft_dirdat );
	mkdir( mft_dirlog );
	mkdir( mft_dirrecv );
	
	strdcpy( &fname1, mft_dirbin, DIR_CHAR "mftuconf0.dat", _ );
	strdcpy( &fname2, mft_dirbin, DIR_CHAR "mftuconf.dat", _ );
	
	file_copy( fname1, fname2 );
	
	file_delete( strdcpy( &cmd, mft_dirdat, DIR_CHAR "mft_parm*", _ ) ); // Suppression pour recréation

	
	// CREATION DU FICHIER MFT_PARM
	
	strdcpy( &fname, mft_dirdat, DIR_CHAR "mft_parm", _ );
	mftutil( strdcpy( &cmd, "mftfile type=parm,fname=", fname, _ ), &s1 );
	if ( ! file_exists( fname ) )
		{
		#ifdef OS_WINDOWS
		sleep(1);
		mftutil( cmd, &s1 );
		if ( ! file_exists( fname ) )
		#endif
			{
			print( cmd, BR, s1, BR, _ );
			logw( "mft start : ", cmd, BR, s1, _ );
			return ERROR;
			}
		}



	// CREATION DU FICHIER MFT_PART
	
	strdcpy( &fname, mft_dirdat, DIR_CHAR "mft_part", _ );
	if ( var_get( hReq, "part_reset" ) )
		{
		strdcpy( &fname1, fname, "*", _ );
		file_delete( fname1 );
		}
	
	
	if ( ! file_exists( fname ) )
		{
		mftutil( strdcpy( &s, "mftfile type=part,fname=", fname, _ ), &s1 );
		bConfAllPart = true;
		}
	if ( ! file_exists( fname ) )
		{
		print( "ERREUR : Création du fichier partenaires : " BR, s, BR, s1, BR, _ );
		logw( "mft start : ERREUR : Création du fichier partenaires : " BR, s, BR, s1, _ );
		return ERROR;
		}



	// CREATION DU FICHIER MFT_CAT
	
	strdcpy( &fname, mft_dirdat, DIR_CHAR "mft_cat", _ );
	if ( !file_exists( fname ) )
		{
		mftutil( strdcpy( &s, "mftfile type=cat,recnb=", var_get( hMFT, "cat_size" ),
											",fname=", fname, _ ), &s1 );
		if ( ! file_exists( fname ) )
			{
			print( "ERREUR : Création du fichier catalogue : " BR, s, BR, s1, BR, _ );
			logw( "mft start : ERREUR : Création du fichier catalogue : " BR, s, BR, s1, _ );
			return ERROR;
			}
		}


	// CREATION DU FICHIER MFT_COM
	
	strdcpy( &fname, mft_dirdat, DIR_CHAR "mft_com", _ );
	if ( !file_exists( fname ) )
		{
		mftutil( strdcpy( &s, "mftfile type=com,recnb=", var_get( hMFT, "com_size" ),
									",fname=", fname, _ ), &s1 );
		if ( ! file_exists( fname ) )
			{
			print( "ERREUR : Création du fichier de communication : " BR, s, BR, s1, BR, _ );
			logw( "mft start : ERREUR : Création du fichier communication : " BR, s, BR, s1, _ );
			return ERROR;
			}
		}



	
	//--------------------------------------------------------------------------
	// VERIFICATION DE LA LICENCE MFT ET LES OPTIONS
	//--------------------------------------------------------------------------


	strdcpy( &cmd,
		"MFTPARM \"ID=IDPARM0,PART=LOCAL,NPART=", partname, ","
			"NET=TCPINOUT,PROT=PSITETCP,",
			"COM=IDCOM0,CAT=IDCAT0,LOG=IDLOG0,"
			"KEY='",	mftkey, "',"
			"BUFSIZE=65535,SSLMTASK = '1',SSLTTASK = '1',SSLWTASK = '1',"
			"PARTFNAM=", mft_dirdat, DIR_CHAR "mft_part,"
			"WAITTASK=1441,WAITRESP=120,DEFAULT=DEFAUT,MAXTRANS=3\"", _ );
			
	mftutil( cmd, &s );

	if ( mftutil( "about", &s ) == ERROR )
		{
		print( "ERREUR : Configuration MFTPARM :" BR, "about", BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTPARM :" BR, "about", BR, s, _ );
		return ERROR;
		}

	
	if ( strpos( s, mftkey, 0 ) < 0 )
		{
		print( "ERREUR : La clef de licence MFT n'est pas valide" BR, _ );
		logw( "mft start : ERREUR : La clef de licence MFT n'est pas valide", _ );
		return ERROR;
		}
	
	if ( ( p = stripos( s, "* Nb Trans", 0 ) ) < 0 )
		{
		print( "ERREUR : lecture du maxtrans" BR BR, s, BR, _ );
		logw( "mft start : ERREUR : lecture du maxtrans" BR BR, s, _ );
		return ERROR;
		}
	
	p = strpos( s, "=", p );
	if ( strpos( s, "Max", p ) > 0 )	maxtrans = 1000;
	else maxtrans = atoi( &(s[p+2]) );
	
	if ( maxtrans < 3 )
		{
		print( "ERREUR : lecture du maxtrans" BR BR, s, BR, _ );
		logw( "mft start : ERREUR : lecture du maxtrans" BR BR, s, _ );
		return ERROR;
		}
	
	if ( ( p = stripos( s, "* Options", 0 ) ) < 0 )
		{
		print( "ERREUR : lecture des options" BR BR, s, BR, _ );
		logw( "mft start : ERREUR : lecture du options" BR BR, s, _ );
		return ERROR;
		}

	if ( strpos( s, " SSL ", p ) > 0 )	bSSL = true;

	


	//--------------------------------------------------------------------------
	// CONFIGURATION DE MFTPARM
	//--------------------------------------------------------------------------


	strdcpy( &cmd,
		"MFTPARM \"ID=IDPARM0,PART=LOCAL,NPART=", partname, ","
			"NET=TCPINOUT,PROT=(PSITETCP", (char*)((bSSL)?",PSITSSLM,PSITSSLS":""), "),"
			"COM=IDCOM0,CAT=IDCAT0,LOG=IDLOG0,KEY=", mftkey, ","
			"BUFSIZE=65535,SSLMTASK = '1',SSLTTASK = '1',SSLWTASK = '1',"
			"PARTFNAM=", mft_dirdat, DIR_CHAR "mft_part,"
			"WAITTASK=1441,WAITRESP=120,DEFAULT=DEFAUT,MAXTRANS=", itoa(maxtrans),
		_ );
	
	if (bSSL)
		strdcat( &cmd, ",pkipassw='SSIE7',PKIFNAME=", mft_dirdat, DIR_CHAR "mft_pki\"", _ );
	else
		strdcat( &cmd, v, "\"", _ );
	
	if ( mftutil( cmd, &s ) == ERROR )
		{
		print( "ERREUR : Configuration MFTPARM :" BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTPARM :" BR, cmd, BR, s, _ );
		return ERROR;
		}
	
	
	
	
	//--------------------------------------------------------------------------
	// MFTNET
	//--------------------------------------------------------------------------

	
	r = mftutil( strdcpy( &cmd, "MFTNET ID=TCPINOUT,MAXCNX=", itoa(maxtrans),
				",TYPE=TCP,HOST=INADDR_ANY,CALL=INOUT", _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTNET : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTNET : ", BR, cmd, BR, s, _ );
		return ERROR;
		}



	//--------------------------------------------------------------------------
	// MFTPROT
	//--------------------------------------------------------------------------
	

	mftutil( strdcpy( &cmd,
			"MFTPROT ID=PSITETCP,TYPE=PESIT,SAP=", var_get( hMFT, "port_mft" ), ","
				"NET=TCPINOUT,CONCAT=YES,"
				"MULTART=YES,PROF=ANY,RCOMP=15,SCOMP=15,SRUSIZE=32750," 
				"SPACING=1024,RPACING=1024,schkw=3,rchkw=3,"
				"RRUSIZE=32750,SEGMENT=YES", _ ), &s );

	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTPROT : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTPROT : ", BR, cmd, BR, s, _ );
		return ERROR;
		}



	//--------------------------------------------------------------------------
	// MFTCOM
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd, "MFTCOM ID=IDCOM0,NAME=",
					mft_dirdat, DIR_CHAR "mft_com,TYPE=FILE,WSCAN=1", _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTCOM : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTCOM : ", BR, cmd, BR, s, _ );
		return ERROR;
		}

	//--------------------------------------------------------------------------
	// MFTCAT
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd, "MFTCAT ID=IDCAT0,FNAME=",
			mft_dirdat, DIR_CHAR "mft_cat,"
			"SH=", var_get( hMFT, "sh" ), ","
			"ST=", var_get( hMFT, "st" ), ","
			"SX=", var_get( hMFT, "sx" ), ","
			"RH=", var_get( hMFT, "rh" ), ","
			"RT=", var_get( hMFT, "rt" ), ","
			"RX=", var_get( hMFT, "rx" ),
			_ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTCAT : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTCAT : ", BR, cmd, BR, s, _ );
		return ERROR;
		}

	//--------------------------------------------------------------------------
	// MFTLOG
	//--------------------------------------------------------------------------

	r = mftutil( strdcpy( &cmd,
			"MFTLOG ID=IDLOG0,"
				"FNAME=",	mft_dirlog, DIR_CHAR "mft.log,"
				"AFNAME=",	mft_dirlog, DIR_CHAR "mft.log,"
				"EXEC=",	mft_dirdat, DIR_CHAR "switchlog.mft",
				_ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTLOG : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTLOG : ", BR, cmd, BR, s, _ );
		return ERROR;
		}

	// CREATION DU SCRIPT DE SWITCH DES LOG
	
	fp = fopen( strdcpy( &fname, mft_dirdat, DIR_CHAR "switchlog.mft", _ ), "wb" );
	if ( fp )
		{
		#ifdef OS_UNIX
		fprint( fp, 
			"#!/bin/ksh" BR
			"filename=", mft_dirlog, DIR_CHAR "mft.log30" BR
			"if [[ -f $filename ]] then" BR "rm -f $filename" BR "fi" BR, _ );
		for( n = 29 ; n > 0 ; n-- )
			fprint( fp,
				"filename=", mft_dirlog, DIR_CHAR "mft.log", itoa(n), BR
				"if [[ -f $filename ]] then" BR
					"mv ", mft_dirlog, DIR_CHAR "mft.log", itoa(n), " ", mft_dirlog, DIR_CHAR "mft.log", itoa(n+1), BR
				"fi" BR, _ );
		fprint( fp,
			"filename=", mft_dirlog, DIR_CHAR "mft.log" BR
			"if [[ -f $filename ]] then" BR
				"mv ", mft_dirlog, DIR_CHAR "mft.log ", mft_dirlog, DIR_CHAR "mft.log1" BR
			"fi" BR
			"touch $filename" BR
			"rm $0" BR,
			_ );
		#else
		fprint( fp, "del /Q ", mft_dirlog, DIR_CHAR "mft.log30" BR, _ );

		for( n = 29 ; n > 0 ; n-- )
			fprint( fp,
				"rename ", mft_dirlog, DIR_CHAR "mft.log", itoa(n), " mft.log", itoa(n+1), BR
				, _ );

		fprint( fp,
			"rename ", mft_dirlog, DIR_CHAR "mft.log mft.log1", BR
			"MFTUTIL MFTFILE type=LOG,fname=", mft_dirlog, DIR_CHAR "mft.log" BR
			"del ", mft_dirdat, DIR_CHAR "MFT0????.BAT" BR
			"exit 0" BR,
			_ );
		#endif
		fclose( fp );
		}



	//--------------------------------------------------------------------------
	// MFTXLATE
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd, "MFTXLATE ID=DEFAUT,DIRECT=SEND,FCODE=ASCII,NCODE=EBCDIC," 
			"FNAME=", mft_dirbin, DIR_CHAR "atoe", _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTXLATE : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTXLATE : ", BR, cmd, BR, s, _ );
		return ERROR;
		}
	
	r = mftutil( strdcpy( &cmd, "MFTXLATE ID=DEFAUT,DIRECT=RECV,FCODE=ASCII,NCODE=EBCDIC," 
			"FNAME=", mft_dirbin, DIR_CHAR "etoa", _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTXLATE : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTXLATE : ", BR, cmd, BR, s, _ );
		return ERROR;
		}



	
	
	
	//--------------------------------------------------------------------------
	// IDF SEND DEFAUT
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd,
				"MFTSEND ID=DEFAUT,FCODE=BINARY,NCODE=BINARY,FACTION=NONE,"
				"EXEC=\"", mft_dirconf, DIR_CHAR "reveils" DIR_CHAR "&9.SAPPL.&1.8SAPPL.ACR\""
				,_ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTSEND : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTSEND : ", BR, cmd, BR, s, _ );
		return ERROR;
		}


	//--------------------------------------------------------------------------
	// IDF SEND R2MMSB
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd,
				"MFTSEND ID=R2MMSB,FCODE=BINARY,NCODE=BINARY,FACTION=NONE,"
				"EXEC=\"", mft_dirconf, DIR_CHAR "reveils" DIR_CHAR "&9.SAPPL.&1.8SAPPL.ACR\""
				, _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTSEND : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTSEND : ", BR, cmd, BR, s, _ );
		return ERROR;
		}


	//--------------------------------------------------------------------------
	// IDF SEND R2MMS
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd, "MFTSEND ID=R2MMS,FCODE=ASCII,NCODE=EBCDIC,"
				"FTYPE=X,FLRECL=10000,FACTION=NONE,"
				",EXEC=\"", mft_dirconf, DIR_CHAR "reveils" DIR_CHAR "&9.SAPPL.&1.8SAPPL.ACR\""
				, _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTSEND : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTSEND : ", BR, cmd, BR, s, _ );
		return ERROR;
		}


	//--------------------------------------------------------------------------
	// IDF SEND ACK
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd, "MFTSEND ID=ACK,FCODE=ASCII,NCODE=EBCDIC,"
				"FTYPE=X,FLRECL=10000,FACTION=NONE", _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTSEND : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTSEND : ", BR, cmd, BR, s, _ );
		return ERROR;
		}




	//--------------------------------------------------------------------------
	// IDF RECV ACK
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd, "MFTRECV ID=ACK,FCODE=ASCII,FTYPE=T,FACTION=DELETE,FDISP=BOTH,"
			"FNAME=\"", mft_dirrecv, DIR_CHAR "&IDTU\""
			, _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTRECV : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTRECV : ", BR, cmd, BR, s, _ );
		return ERROR;
		}



	//--------------------------------------------------------------------------
	// IDF RECV R2MMSB
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd, "MFTRECV ID=R2MMSB,FCODE=BINARY,FACTION=DELETE,FDISP=BOTH,"
			"FNAME=\"",mft_dirrecv, DIR_CHAR "&IDTU\","
			"EXEC=\"", mft_dirconf, DIR_CHAR "reveils" DIR_CHAR "&9.RAPPL.&1.8RAPPL.AVR\""
			, _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTRECV : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTRECV : ", BR, cmd, BR, s, _ );
		return ERROR;
		}

	
	
	//--------------------------------------------------------------------------
	// IDF RECV R2MMS
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd, "MFTRECV ID=R2MMS,FCODE=ASCII,FTYPE=T,FACTION=DELETE,FDISP=BOTH,"
			"FNAME=\"",mft_dirrecv, DIR_CHAR "&IDTU\","
			"EXEC=\"", mft_dirconf, DIR_CHAR "reveils" DIR_CHAR "&9.RAPPL.&1.8RAPPL.AVR\""
			, _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTRECV : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTRECV : ", BR, cmd, BR, s, _ );
		return ERROR;
		}



	//--------------------------------------------------------------------------
	// IDF RECV R2MFS
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd, "MFTRECV ID=R2MFS,FCODE=ASCII,FTYPE=T,FACTION=DELETE,FDISP=BOTH,"
			"FNAME=\"",mft_dirdat, DIR_CHAR "recv" DIR_CHAR "&IDTU\","
			"EXEC=\"", mft_dirconf, DIR_CHAR "reveils" DIR_CHAR "&9.RAPPL.&1.8RAPPL.AVR\""
			, _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTRECV : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTRECV : ", BR, cmd, BR, s, _ );
		return ERROR;
		}


	//--------------------------------------------------------------------------
	// IDF RECV DEFAUT
	//--------------------------------------------------------------------------
	
	r = mftutil( strdcpy( &cmd, "MFTRECV ID=DEFAUT,FCODE=BINARY,FACTION=DELETE,"
			"FNAME=\"",mft_dirrecv, DIR_CHAR "&IDTU\","
			"EXEC=\"", mft_dirconf, DIR_CHAR "reveils" DIR_CHAR "&9.RAPPL.&1.8RAPPL.AVR\""
			, _ ), &s );
	if ( r == ERROR )
		{
		print( "ERREUR : Configuration MFTRECV : ", BR, cmd, BR, s, BR, _ );
		logw( "mft start : ERREUR : Configuration MFTRECV : ", BR, cmd, BR, s, _ );
		return ERROR;
		}



	//--------------------------------------------------------------------------
	// CONFIGURATION SSL
	//--------------------------------------------------------------------------

	if ( bSSL )
		{
		//----------------------------------------------------------------------
		// PROTOCOLES SSL
		//----------------------------------------------------------------------
		
		if ( mftutil( strdcpy( &cmd,
			"MFTPROT ID=PSITSSLS,TYPE=PESIT,SAP=", var_get( hMFT, "port_ssl" ), ","
				"NET=TCPINOUT,CONCAT=YES,"
				"MULTART=YES,PROF=ANY,RCOMP=15,SCOMP=15,SRUSIZE=32750,"
				"SPACING=1024,RPACING=1024,schkw=3,rchkw=3,"
				"RRUSIZE=32750,SEGMENT=YES,SSL=SSLSRVS", _ ), &s ) == ERROR )
			{
			print( "ERREUR : Configuration MFTPROT SSL : ", BR, cmd, BR, s, BR, _ );
			logw( "mft start : ERREUR : Configuration MFTPROT SSL : ", BR, cmd, BR, s, _ );
			return ERROR;
			}
		
		if ( mftutil( strdcpy( &cmd,
			"MFTPROT ID=PSITSSLM,TYPE=PESIT,SAP=", var_get( hMFT, "port_ssl2" ), ","
				"NET=TCPINOUT,CONCAT=YES,"
				"MULTART=YES,PROF=ANY,RCOMP=15,SCOMP=15,SRUSIZE=32750," 
				"SPACING=1024,RPACING=1024,schkw=3,rchkw=3,"
				"RRUSIZE=32750,SEGMENT=YES,SSL=SSLSRVM", _ ), &s ) == ERROR )
			{
			print( "ERREUR : Configuration MFTPROT SSL2 : ", BR, cmd, BR, s, BR, _ );
			logw( "mft start : ERREUR : Configuration MFTPROT SSL2 : ", BR, cmd, BR, s, _ );
			return ERROR;
			}
		
		//----------------------------------------------------------------------
		// PREPARATION CARTE SSLSRVS
		//----------------------------------------------------------------------
		
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
		
		
		
		//----------------------------------------------------------------------
		// SSLSRVS
		//----------------------------------------------------------------------
		
		strdcpy( &cmd,"MFTSSL \"ID=SSLSRVS,direct=server,verify=none,"
			"ciphlist=(47,10,9,5,4)", _ );
		if ( s3[0] )
			strdcat( &cmd, ",usercid='USER',rootcid=(", s3, ")", _ );
		strdcat( &cmd, "\"", _ );
		if ( mftutil( cmd, &s ) == ERROR )
			{
			print( "ERREUR : Configuration MFTSSL : ", BR, cmd, BR, s, BR, _ );
			logw( "mft start : ERREUR : Configuration MFTSSL : ", BR, cmd, BR, s, _ );
			return ERROR;
			}



		//----------------------------------------------------------------------
		// PREPARATION CARTE SSLSRVM
		//----------------------------------------------------------------------
		
/*
		tf = NULL;
		n = 0;
		while( tf = file_list( tf, dirpart, "nodir" ) )
			{
			if ( (p = strpos( tf->name, ".dat", 0 )) < 0 ) continue;
			v = trim(str( substr( tf->name, 0, p ), _ ));
			if ( strpos( s3, v, 0 ) >= 0 ) continue;
			strdcpy( &fname, dirpart, "/", upper(v), ".dat", _ );
			if ( !file_exists( fname ) ) continue;
			h = var_load( fname );
			v1 = trim(var_get( h, "part_ssl_certifr" ));
			if ( ( v1 )&&( v1[0] ) )
				strdcat( &s3, (char*)((s[0])?",":""), v, "R", _ );
			var_free( h );
			}
			
*/			
		//----------------------------------------------------------------------
		// SSLSRVM
		//----------------------------------------------------------------------
		
		mft_conf_ssl2( t_work );


		//----------------------------------------------------------------------
		// CREATION FICHIER PKI
		//----------------------------------------------------------------------
		
		strdcpy( &fname, mft_dirdat, DIR_CHAR "mft_pki*", _ );
		file_delete( fname );
		strdcpy( &fname, mft_dirdat, DIR_CHAR "mft_pki", _ );

		pkiutil( strdcpy( &cmd, "PKIFILE mode=create,fname=", fname, _ ), &s );
		if ( ! file_exists( fname ) )
			{
			print( "ERREUR : Création fichier PKI : ", BR, cmd, BR, s, BR, _ );
			logw( "mft start : ERREUR : Création fichier PKI : ", BR, cmd, BR, s, _ );
			return ERROR;
			}
			

		//----------------------------------------------------------------------
		// INSERTION DES CERTIFICATS SERVEUR
		//----------------------------------------------------------------------
		
		
		// CERTIFICAT ROOT

//print( "Récup certif root" BR, _ );		
		v1 = null;
		for( i = 1 ; ; i++ )
			{
			v = trim(var_get( hSrvConf, strdcpy( &s, "ssl_certif_ca", itoa(i), _ ) ));
			if ( !v ) break;
			v1 = v;
			}
		i--;

		strdcpy( &fname, mft_dirconf, DIR_CHAR "certif", _ );
		mkdir( fname );
		strdcat( &fname, DIR_CHAR "root0.pem", _ );
		file_save_str( fname, v1, -1 );

//print( "Configure certif root" BR, _ );		
		tf = NULL;
		while( tf = file_list( tf, "/appli/ssie7/conf/mft/certif/root*", "absdir nodir" ) )
			{
//print( tf->name, BR, _ );
			if ( strpos( tf->name, ".pem", 0 ) < 0 ) continue;
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );

			strdcpy( &s1, tf->name, _ );
			s1[strlen(s1)-4] = 0;
			upper( s1 );
				
			if ( pkiutil( strdcpy( &cmd,
				"PKICER ID=", s1, ",ITYPE=ROOT,IFORM=PEM,"
					"INAME=", fname, ",pkipassw='SSIE7',"
					"PKIFNAME=", mft_dirdat, DIR_CHAR "mft_pki,"
					"ROOTCID=", s1,
					_ ), &s ) == ERROR )
				{
				print( "ERREUR Configuration PKI Certificat ROOT" BR, cmd, BR, s, BR, _ );
				logw( "mft start : ERREUR : Configuration PKI Certificat ROOT : ", BR, cmd, BR, s, _ );
				return ERROR;
				}
//print( cmd, BR, s, BR, _ );
			}


			
		// CERTIFICATS CA INTERMEDIAIRES
		
		strdcpy( &fname, dirtmp, DIR_CHAR "mft_cert.tmp", _ );
		strdcpy( &s2, "ROOT0", _ );
		
		for( i-- ; i > 0 ; i-- )
			{
			v1 = trim(var_get( hSrvConf, strdcpy( &s, "ssl_certif_ca", itoa(i), _ ) ));
			
			strdcpy( &s3, s2, _ );
			strdcpy( &s2, "INTER", itoa(i), _ );
	
			file_save_str( fname, v1, -1 );
					
			if ( pkiutil( strdcpy( &cmd,
				"PKICER ID=", s2, ",ITYPE=INTER,IFORM=PEM,"
					"INAME=", fname, ",pkipassw='SSIE7',"
					"PKIFNAME=", mft_dirdat, DIR_CHAR "mft_pki,"
					"ROOTCID=", s3,
					_ ), &s ) == ERROR )
				{
				unlink( fname );
				print( "ERREUR Configuration PKI Certificat Inter ", itoa(i), BR, cmd, BR, s, BR, _ );
				logw( "mft start : ERREUR : Configuration PKI Certificat Inter : ", itoa(i), BR, cmd, BR, s, _ );
				return ERROR;
				}
			}
		

		// CERTIFICAT SERVEUR
				
		strdcpy( &fname1, dirtmp, DIR_CHAR "mft_cert0.tmp", _ );
		
		v2 = trim(var_get( hSrvConf, "ssl_private_key" ));
		file_save_str( fname1, v2, -1 );

		v1 = trim(var_get( hSrvConf, "ssl_certif_server" ));
		file_save_str( fname, v1, -1 );

		if ( pkiutil( strdcpy( &cmd,
			"PKICER ID=USER,ITYPE=USER,IFORM=PEM,"
			"INAME=", fname, ",IKFORM=PEM,IKPASSW='SSIE7',"
			"IKNAME=", fname1, ","
			"PKIFNAME=", mft_dirdat, DIR_CHAR "mft_pki,"
			"pkipassw='SSIE7',ROOTCID=", s2,
			_ ), &s ) == ERROR )
			{
			unlink( fname1 );
			unlink( fname );
			print( "ERREUR Configuration PKI Certificat User" BR, cmd, BR, s, BR BR, _ );
			logw( "mft start : ERREUR : Configuration PKI Certificat User" BR, cmd, BR, s, _ );
//			return ERROR;
			}
//logw( cmd, s, _ );

			
		unlink( fname1 );
		unlink( fname );
		}



	//--------------------------------------------------------------------------
	// PARAMETRAGE LOCAL SPECIFIQUE
	//--------------------------------------------------------------------------
	
	strdcpy( &fname, dirconf, "/MFTPARMLOC.CFG", _ );
	if ( file_exists( fname ) )
		{
		print( BR "Configuration MFT complémentaire :" BR, _ );
		#ifdef OS_WINDOWS
		System( &s, strdcpy( &s1, mft_dirbin, DIR_CHAR "MFTUTIL.exe #", fname, _ ) );
		#else
		System( &s, strdcpy( &s1, mft_dirbin, DIR_CHAR "MFTUTIL @", fname, _ ) );
		#endif
		print( s, BR, _ );
		}
		


	//--------------------------------------------------------------------------
	// PARAMETRAGE PKI SPECIFIQUE
	//--------------------------------------------------------------------------
	
	strdcpy( &fname, dirconf, "/MFTPKILOC.CFG", _ );
	if ( file_exists( fname ) )
		{
		print( BR "Configuration MFT PKI complémentaire :" BR, _ );
		#ifdef OS_WINDOWS
		System( &s, strdcpy( &s1, mft_dirbin, DIR_CHAR "PKIUTIL.exe #", fname, _ ) );
		#else
		System( &s, strdcpy( &s1, mft_dirbin, DIR_CHAR "PKIUTIL @", fname, _ ) );
		#endif
		print( s, BR, _ );
		}
		


	//--------------------------------------------------------------------------
	// NETTOYAGE AVANT DEMARRAGE
	//--------------------------------------------------------------------------

	file_delete( strdcpy( &cmd, mft_dirdat, DIR_CHAR "S_TCP", _ ) );

	#ifdef OS_UNIX
		
		#ifndef OS_LINUX
		system( "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*mft.*/ -\\1 \\2/p\"` 2> /dev/null" );
	//	system( "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*ssie.*/ -\\1 \\2/p\"` 2> /dev/null" );
		strdcpy( &cmd, "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*",
					ssie_user, ".*/ -\\1 \\2/p\"` 2> /dev/null", _ );
		system( cmd );
		#else
		system( "ipcrm `ipcs | awk '{if ( $3 == \"mft\" ) {print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'` 2> /dev/null" );
	//	system( "ipcrm `ipcs | awk '{if ( $3 == \"ssie\" ) {print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'` 2> /dev/null" );
		strdcpy( &cmd, "ipcrm `ipcs | awk '{if ( $3 == \"", ssie_user,"\" ) "
						"{print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'`"
						" 2> /dev/null", _ );
		system( cmd );
		#endif

	#endif

	
	
	//--------------------------------------------------------------------------
	// HISTORISATION ET CREATION DU FICHIER MFT.LOG
	//--------------------------------------------------------------------------

	strdcpy( &fname, mft_dirlog, DIR_CHAR "mft.log", _ );
	if ( !file_exists( fname ) )
		{
		r = mftutil( strdcpy( &cmd, "mftfile type=log,fname=", fname, _ ), &s );
		if ( ( r == ERROR )||( ! file_exists( fname ) ) )
			{
			print( cmd, BR, s, BR, _ );
			logw( "mft start : ERREUR : " BR, cmd, BR, s, _ );
			return ERROR;
			}
		}

	

	//--------------------------------------------------------------------------
	// CONFIGURE TOUS LES PARTENAIRES
	//--------------------------------------------------------------------------

	print( "Configuration des partenaires" BR, _ );
	logw( "mft start : Configuration des partenaires.", _ );
	
	if ( bConfAllPart )
		{
		strdcpy( &cmd, argv[0], " confpart all", _ );
		system( cmd );
		}
	else
		{
		strdcpy( &fname, mft_dirdat, DIR_CHAR "mft_part", _ );
		n = file_date( fname );
		b = ( var_get( hReq, "part_reset" ) ) ? true : false;
			
		strdcpy( &fname, mft_dirconf, DIR_CHAR "part", _ );
		
		tf = NULL;
		while( tf = file_list( tf, fname, "nodir absdir" ) )
			{
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
	        if (( !b )&&( file_date( fname ) < n )) continue;
			strdcpy( &cmd, MFTCHAR, fname, _ );
			mftutil( cmd, &s );
			if ( trim(s)[0] )
				{
				print( "ATTENTION : Configuration du partenaire ", tf->name, ", ", s, BR BR, _ );
				logw( "mft start : ATTENTION : Configuration du partenaire ", tf->name, ", ", s, _ );
				}
			}
		}

	
	//--------------------------------------------------------------------------
	// DEMARRAGE DE MFT
	//--------------------------------------------------------------------------
	
	print( "Démarrage de MFT ..." BR, __ );
	logw( "mft start : Démarrage de MFT ...", _ );

//	strdcpy( &s, mft_dirbin, _ );
//	chdir( s );
	chdir( mft_dirbin );

	StartH( "MFTMAIN" EXE );
	b = 0;


	//--------------------------------------------------------------------------
	// ATTEND QUE MFT SOIT DEMARRE
	//--------------------------------------------------------------------------
	
	n = 0;
	for( sleep(1) ; ; sleep(1) )
		{
		n1 = process_count( "MFTMAIN" EXE );
//print( "coucou1:", itoa(n1), BR, _ );
		
		#ifdef OS_WINDOWS
		if ( ( process_count( "MFTN_005" ) )&&( n1 ) )
		#else
		if ( ( process_count( "MFTTCPS" ) )&&( n1 ) )
		#endif
			{
//print( "coucou2" BR, _ );
			if ( s ) s[0] = 0;
			file_load( &s, strdcpy( &s1, mft_dirdat, DIR_CHAR "log" DIR_CHAR "mft.log", _ ) );
			if ( stripos( s, "MFT init complete", 0 ) > 0 )
				{
				print( "MFT est démarré", BR, _ );
				logw( "mft start : MFT est démarré.", _ );
				break;
				}
			}
		
		n++;
//print( "coucou3" BR, _ );
		
		if ( n > 5 )
			{
//print( "coucou4" BR, _ );
			if ( s ) s[0] = 0;
			file_load( &s, strdcpy( &s1, mft_dirdat, DIR_CHAR "log" DIR_CHAR "mft.log", _ ) );
			if ( stripos( s, "MFT stop complete", 0 ) > 0 )
				n = 1000;
			}
		
//print( "coucou5" BR, _ );
		if ( ( !n1 )||( n > 60 ) )
			{
			if ( !n1 )
				{
				System( &s, "ps -ef | grep MFTMAIN" );
				logw( "mft start : ERREUR : process MFTMAIN absent : " BR, s, _ );
				print( "ERREUR : process MFTMAIN absent : " BR, s, BR BR, _ );
				}
//print( "coucou6" BR, _ );
				
			if ( n <= 60 )
				{
				print( "ERREUR : kill MFT et redémarre" BR, _ );
				
				process_kill_all( "MFT" );
				#ifdef OS_UNIX
			
				file_delete( strdcpy( &fname, dirwrk, "/S_TCP", _ ) );
				
/*
				#ifndef OS_LINUX
				system( "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*mft.* / -\\1 \\2/p\"` 2> /dev/null" );
				system( "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*ssie.* / -\\1 \\2/p\"` 2> /dev/null" );
				#else
				system( "ipcrm `ipcs | awk '{if ( $3 == \"mft\" ) {print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'` 2> /dev/null" );
				system( "ipcrm `ipcs | awk '{if ( $3 == \"ssie\" ) {print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'` 2> /dev/null" );
				#endif
*/				

				#ifndef OS_LINUX
				system( "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*mft.*/ -\\1 \\2/p\"` 2> /dev/null" );
			//	system( "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*ssie.*/ -\\1 \\2/p\"` 2> /dev/null" );
				strdcpy( &cmd, "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*",
							ssie_user, ".*/ -\\1 \\2/p\"` 2> /dev/null", _ );
				system( cmd );
				#else
				system( "ipcrm `ipcs | awk '{if ( $3 == \"mft\" ) {print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'` 2> /dev/null" );
			//	system( "ipcrm `ipcs | awk '{if ( $3 == \"ssie\" ) {print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'` 2> /dev/null" );
				strdcpy( &cmd, "ipcrm `ipcs | awk '{if ( $3 == \"", ssie_user,"\" ) "
								"{print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'`"
								" 2> /dev/null", _ );
				system( cmd );
				#endif
				
				strdcpy( &fname1, mft_dirbin, DIR_CHAR "mftuconf0.dat", _ );
				strdcpy( &fname2, mft_dirbin, DIR_CHAR "mftuconf.dat", _ );
				
				file_copy( fname1, fname2 );
				
				sleep(3);
				
				#endif
				StartH( "MFTMAIN" EXE );
				b++;
				continue;
				}
			print( "ERREUR : MFT ne démarre pas !!!" BR, _ );
			logw( "mft start : ERREUR : MFT ne démarre pas !!!", _ );
			#ifdef OS_UNIX
			print( file_load( &s, strdcpy( &s1, mft_dirdat, DIR_CHAR "mft.out", _ ) ), BR, _ );
			#endif
			
			if ( ( file_load( &s, strdcpy( &s1, mft_dirdat, DIR_CHAR "log" DIR_CHAR "mft.log", _ ) ) )
				&&( strlen( s ) < 3000 ) )
				{
				if ( trim(s)[0] )
					print( s, BR, _ );
				else
				if ( ( file_load( &s, strdcpy( &s1, mft_dirdat, DIR_CHAR "log" DIR_CHAR  "mft.log1", _ ) ) )
					&&( strlen( s ) < 3000 ) )
					{
					if ( trim(s)[0] )
						print( s, BR, _ );
					}
				}
			break;
			}
		}
	
	
	
	//--------------------------------------------------------------------------
	// DEMARRE LE ROBOT MFT
	//--------------------------------------------------------------------------
	
	strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot.pid", _ );
	if ( s ) s[0] = 0;
	file_load( &s, fname );
	if ( ( !s )||( !process_is_alive( atoi(s) ) ) )
		{
		print( "Démarrage des robots MFT" BR, __ );
		chdir( dirbin );
		strdcpy( &cmd, "mft robot", _ );
		StartH( cmd );
		}
	
	

	//--------------------------------------------------------------------------

	strdcpy( &fname, dirwrk, DIR_CHAR "mft_is_started", _ );
	file_save_str( fname, "", 0 );
	
	mft_start_fin :
		

	//--------------------------------------------------------------------------

	return OK;
	}	



//------------------------------------------------------------------------------
int cmd_status( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, dirwrk, DIR_CHAR "mft_is_started", _ );
	if ( file_exists( fname ) )
		print( "MFT est démarré", BR, _ );
	else	
		print( "MFT est arrêté", BR, _ );
	}



//------------------------------------------------------------------------------
int cmd_stop( T_WORK * t_work, char bRestart )
//------------------------------------------------------------------------------
	{
	if ( !bRestart )
		{
		strdcpy( &fname, dirwrk, DIR_CHAR "mft_is_started", _ );
		while( file_exists( fname ) )
			{
			unlink( fname );
			Sleep(10);
			}
		}

	// ARRET DES PROCESS MFT
	if ( process_count( "MFT" ) )
		{
		print( "Arrêt de MFT ..." BR, _ );
		logw( "mft stop : Arrêt de MFT ...", _ );
		
		#ifdef OS_UNIX
		process_kill_all( "mft " );
		#endif
		
		for( ; ; )
			{
			process_kill_all( "MFT" );
			if ( process_count( "MFT" ) < 2 ) break;
			Sleep( 20 );
			if ( ! process_count( "MFT" ) < 2 ) break;
			Sleep( 20 );
			if ( ! process_count( "MFT" ) < 2 ) break;
			Sleep( 20 );
			if ( ! process_count( "MFT" ) < 2 ) break;
			}
		}

	#ifdef OS_UNIX

	file_delete( strdcpy( &fname, mft_dirdat, "/S_TCP", _ ) );
	
	#ifndef OS_LINUX
	system( "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*mft.*/ -\\1 \\2/p\"` 2> /dev/null" );
//	system( "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*ssie.*/ -\\1 \\2/p\"` 2> /dev/null" );
	System( &s, "whoami" ); trim(s);
	system( "ipcrm `ipcs | sed -n \"s/^\\([qms]\\) *\\([0-9][0-9]*\\).*",
				s, ".*/ -\\1 \\2/p\"` 2> /dev/null" );
	#else
	system( "ipcrm `ipcs | awk '{if ( $3 == \"mft\" ) {print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'` 2> /dev/null" );
//	system( "ipcrm `ipcs | awk '{if ( $3 == \"ssie\" ) {print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'` 2> /dev/null" );
	System( &s, "whoami" ); trim(s);
	strdcpy( &cmd, "ipcrm `ipcs | awk '{if ( $3 == \"", s,
						"\" ) {print \"-q \" $2; print \"-m \" $2; print \"-s \" $2;}}'` 2> /dev/null", _ );
	system( cmd );
	#endif
	#endif


	print( "MFT est arrêté", BR, _ );
	logw( "mft stop : MFT est arrêté", _ );
	return OK;
	}

























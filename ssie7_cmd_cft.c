#include "ssie7.h"


//------------------------------------------------------------------------------
int cmd_mft( T_WORK *t_work )

// EXECUTE UNE COMMANDE MFT
//------------------------------------------------------------------------------
	{
	p = strpos( cmd, " ", 0 );
	if ( p < 0 ) return ERROR;
	strdcpy( &s, mft_exe(), &(cmd[p]), _ );
	if ( request_mode == MODE_TCP )
		{
		System( &s1, s );
		print( s1, _ );
		}
	else
		system( s );
	return OK;
	}
	



//------------------------------------------------------------------------------
int mft_reveil_start( T_WORK *t_work )

// DEMARRE UN REVEIL MFT
//------------------------------------------------------------------------------
	{
	// fname contient le nom du fichier réveil
	// h contient les info sur le transfert
	
//	logw( "mft_reveil_start : ", fname, _ );

	// CHARGEMENT DU REVEIL
	
	if ( file_load( &s3, fname ) == sERROR )
		{
		mftcat_logw( h, "ERREUR Chargement du reveil ", fname, " : ", error_msg, BR, _ );
		mftcat_upd( h, null, null, null );
		return ERROR;
		}
	
	mftcat_logw( h, "Charge le reveil :", fname, BR, _ );

	
	// REMPLACE LES VARIABLES MFT PAR LEUR VALEUR
	
	for( p = p0 = 0 ; ; )
		{
		p = strpos( s3, "&", p0 );
		if ( p < 0 ) break;
		n1 = atoi( &(s3[p+1]) ) - 1;
		for( p1 = p+1, b=false, b1=false ; s3[p1] ; p1++ )
			{
			c = s3[p1];
			if ( c == '.' )
				{
				if ( b1 ) break;
				b1 = true;
				n2 = atoi( &(s3[p1+1]) );
				if ( !n2 ) n2 = -1;
				else n2 = n2 - n1;
				continue;
				}
			if ( ( c >= '0' )&&( c <= '9' ) ) continue;
			if ( !strnicmp( &(s3[p1]), v3="IDF",   -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="PARM",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="IDTU",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="IDT",   -1 ) ) { b = true; v3="IDA"; break; }
			if ( !strnicmp( &(s3[p1]), v3="RIDA",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="IDA",   -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="SPART", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="RPART", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="PART",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="SUSER", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="RUSER", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="SAPPL", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="RAPPL", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="NFNAME",-1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="FNAME", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="DATEB", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="DATEE", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="TIMEB", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="TIMEE", -1 ) ) { b = true; break; }
			//Ticket#2017082310000268: ajout de la variable TT
			if ( !strnicmp( &(s3[p1]), v3="TT", -1 ) ) { 
								b = true; 
                						strdcpy(&s1,var_get( h, "DATEB" )," ", substr( var_get( h, "TIMEB" ),0,8),_);
            							strdcpy(&s2,var_get( h, "DATEE" )," ", substr( var_get( h, "TIMEE" ),0,8),_);
								//eviter de faire le calcul si <= 1 sec
								if ( !strnicmp( s1, s2, -1 ) ) var_set( h, "TT","0");
								else var_set( h, "TT", itoa((int)tt(s1, s2, "")) );
								break; 
								}
			break;
			}

		if ( !b ) { p0 = p1; continue; }
			
		if ( b1 )
			strdncpy( &s4, s3, p, substr( var_get( h, v3 ), n1, n2 ), -1, &(s3[p1+strlen(v3)]), -1, _ );
		else
			strdncpy( &s4, s3, p, var_get( h, v3 ), -1, &(s3[p1+strlen(v3)]), -1, _ );
		v = s4; s4 = s3; s3 = v;
		p0 = p;
		}
	
	
	// CREATION DU FICHIER SCRIPT A EXECUTER
	
	strdcpy( &fname, dirlog, DIR_CHAR "reveil_", var_get( h, "ida" ),
															#ifdef OS_WINDOWS
															 ".bat",
															#else
															 ".ksh",
															#endif
																 _ );

	file_save_str( fname, s3, -1 );

	
	// DEMARRAGE DU SCRIPT EN PROCESS PARALLELE
	
	strdcpy( &fname1, dirlog, DIR_CHAR "reveil_", var_get( h, "ida" ), ".log", _ );
	
	mftcat_logw( h, "Start reveil : ", fname, " >> ", fname1, BR BR, s3, BR, _ );
	if (strcmp(var_get( h, "reveil" ),"ACR")) mftcat_upd( h, "T", null, null );// sni: que lorsque AVR
	//else mftcat_upd( h, "T", null, null );//sni crash
	#ifdef OS_UNIX
	
	strdcpy( &cmd, "chmod 700 ", fname, _ );
	system( cmd );
	
	strdcpy( &cmd, fname, " >>", fname1, BR,
			argv[0], " mft rlogw ida=", var_get( h, "ida" ), BR,
			"rm -f ", fname, BR
			"rm -f ", fname1, BR
			"rm -f $0" BR
			"rm -f $0.err" BR,
			_ );

	strdcpy( &fname2, fname, ".err", _ );
	file_save_str( fname2, "", 0 );
	strdcat( &fname, ".start", _ );
	file_save_str( fname, cmd, -1 );
	strdcpy( &cmd1, "chmod 700 ", fname, _ );
	
	system( cmd1 );
	
	while( StartH( fname ) == -1 ) Sleep(100);
	
	#else
	
	strdcpy( &cmd, fname, " >>", fname1, _ );

	while( StartH( cmd ) == -1 ) Sleep(100);
	
	#endif
	//sleep(5);//sni
	//sni mftcat_logw( h,"End reveil : ",cmd,_);//sni
	//mftcat_upd( h, "X", null, null );//sni
	//mftcat_upd( h, "X", "0", "OK" );//sni
	//logw("sni: fin du reveil=",cmd,_);
	//mftcat_upd( h, "X", "0", "OK" );//sni
	return OK; //sni
	}





//------------------------------------------------------------------------------
int mft_recv_fname_compute( T_WORK *t_work, char *direc )

// CALCUL LE NOM DU FICHIER DE RECEPTION
//------------------------------------------------------------------------------
	{
	// h contient les info sur le transfert
	
	// CHARGE L'EXEC MFT EN FONCTION DE L'IDF

	if ( var_get( h, "chown") )
		var_delete( h, "chown" );
		
	v = var_get( h, "nidf");
	//logw("sni: mft_recv_fname_compute -------->",_);
	//if (!strcmp(direc,"SEND")) h = hReq;//corrige le prob des ACR
	if ( v ) var_set( h, "idf", v );

	strdcpy( &fname, diridf, DIR_CHAR, direc, "_", upper(var_get( h, "idf" )), ".cfg", _ );
	if ( ! file_exists( fname ) ) return OK;

	if ( s3 ) s3[0] = 0;
	file_load( &s3, fname );
	
	var_free( h1 );
	h1 = var_from_cmd( 0, s3 );
	if ( h1 == ERROR ) return OK;
	
	if ( ( stricmp( direc, "RECV" ) )||( ! ( v = var_get( h1, "fname" ) ) ) )
		{
		var_free( h1 ); h1 = 0;
		return OK;
		}
		
	if ( v1 = var_get( h1, "chown" ) )
		{
		var_set( h, "chown", v1 );
		}
		
	strdcpy( &s3, v, _ );
	var_free( h1 ); h1 = 0;

	// SUBSTITUTION DES VARIABLES MFT

	var_set( h, "part", var_get( h, "spart" ));
/*
	if ( v = var_get( h, "idt_mft" ) )
		var_set( h, "idt", v );
	else
*/
		var_set( h, "idt", var_get( h, "rida" ));
	var_set( h, "idtu", var_get( h, "rida" ));
	
	for( p = p0 = 0 ; ; )
		{
		p = strpos( s3, "&", p0 );
		if ( p < 0 ) break;

		n1 = atoi( &(s3[p+1]) ) - 1;
		for( p1 = p+1, b=false, b1=false ; s3[p1] ; p1++ )
			{
			c = s3[p1];
			if ( c == '.' )
				{
				if ( b1 ) break;
				b1 = true;
				n2 = atoi( &(s3[p1+1]) );
				if ( !n2 ) n2 = -1;
				else n2 = n2 - n1;
				continue;
				}
			if ( ( c >= '0' )&&( c <= '9' ) ) continue;
			if ( !strnicmp( &(s3[p1]), v3="IDF",   -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="PARM",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="IDTU",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="IDT",   -1 ) ) { b = true; v3="IDA"; break; }
			if ( !strnicmp( &(s3[p1]), v3="RIDA",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="IDA",   -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="SPART", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="RPART", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="PART",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="SUSER", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="RUSER", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="SAPPL", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="RAPPL", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="NFNAME",-1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="FNAME", -1 ) ) { b = true; break; }
			break;
			}
		
		if ( !b ) { p0 = p1; continue; }
			
		if ( b1 )
			{
			strdncpy( &s4, s3, p, substr( var_get( h, v3 ), n1, n2 ), -1, &(s3[p1+strlen(v3)]), -1, _ );
			v = s4;
			s4 = s3;
			s3 = v;
			}
		else
			{
			strdncpy( &s4, s3, p, var_get( h, v3 ), -1, &(s3[p1+strlen(v3)]), -1, _ );
			v = s4;
			s4 = s3;
			s3 = v;
			}
		p0 = p;
		}
	
	// s3 contient maintenant le nouveau nom du fichier
	
	// DEPLACEMENT DU FICHIER
	
	v = var_get( hSrvConf, "part_auto_crea" );
	if ( !strcmp( v, "Oui" ) )
		mkdir( file_getdir( s3 ) );

	unlink( s3 );
	strdcpy( &fname1, s3, _ );
	
	return 1000;
	}
	
	
//------------------------------------------------------------------------------
int mft_reveil_compute( T_WORK *t_work, char *direct )

// CALCUL LE NOM DU REVEIL
//------------------------------------------------------------------------------
	{
	// h contient les info sur le transfert
	
	// CHARGE L'EXEC MFT EN FONCTION DE L'IDF
	v = var_get( h, "nidf");
	// ajout d'un champ reveil
	//logw("sni: cmd (reveil_compute)===>",cmd,"/",_);
	if (!strcmp(direct,"SEND"))  { h=hReq; var_set( h, "reveil","ACR");} else var_set( h, "reveil","AVR");//corrige le prob des ACR mais pas 
	if ( v ) var_set( h, "idf", v );

	strdcpy( &fname, diridf, DIR_CHAR, direct, "_", upper(var_get( h, "idf" )), ".cfg", _ );
	//logw("sni: fichier reveil=",fname,_);
	if ( ! file_exists( fname ) ) return ERROR;

	if ( s3 ) s3[0] = 0;
	file_load( &s3, fname );

	
	var_free( h1 );
	h1 = var_from_cmd( 0, s3 );
	if ( h1 == ERROR ) return ERROR;
	
	// RECHERCHE UN SCRIPT DE REVEIL MFT
	if ( ! ( v = var_get( h1, "exec" ) ) )
		{ var_free( h1 ); h1 = 0; return ERROR; }
	

//	if ( !log_id ) print( currdate("log"), " : IDF EXEC : ", v, BR, _ );
//	else hlogc( hLog, log_id, "IDF EXEC : ", v, _ );

	strdcpy( &s3, v, _ );
	var_free( h1 ); h1 = 0;
	// SUBSTITUTION DES VARIABLES MFT
	
	if ( !stricmp( direct, "RECV" ) )
		var_set( h, "part", var_get( h, "spart" ));
	else
		var_set( h, "part", var_get( h, "rpart" ));
		
/*
	if ( v = var_get( h, "idt_mft" ) )
		var_set( h, "idt", v );
	else
*/
	

	//logw("sni: ida=",var_get( h, "ida" ),_);	

		var_set( h, "idt", var_get( h, "ida" ));
		
	var_set( h, "idtu", var_get( h, "ida" ));
	
	for( p = p0 = 0 ; ; )
		{
		p = strpos( s3, "&", p0 );
		if ( p < 0 ) break;

		n1 = atoi( &(s3[p+1]) ) - 1;
		for( p1 = p+1, b=false, b1=false ; s3[p1] ; p1++ )
			{
			c = s3[p1];
			if ( c == '.' )
				{
				if ( b1 ) break;
				b1 = true;
				n2 = atoi( &(s3[p1+1]) );
				if ( !n2 ) n2 = -1;
				else n2 = n2 - n1;
				continue;
				}
			if ( ( c >= '0' )&&( c <= '9' ) ) continue;
			if ( !strnicmp( &(s3[p1]), v3="IDF",   -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="PARM",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="IDTU",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="IDT",   -1 ) ) { b = true; v3="IDA"; break; }
			if ( !strnicmp( &(s3[p1]), v3="RIDA",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="IDA",   -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="SPART", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="RPART", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="PART",  -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="SUSER", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="RUSER", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="SAPPL", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="RAPPL", -1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="NFNAME",-1 ) ) { b = true; break; }
			if ( !strnicmp( &(s3[p1]), v3="FNAME", -1 ) ) { b = true; break; }
			break;
			}
		
		if ( !b ) { p0 = p1; continue; }
			
		if ( b1 )
			{
			strdncpy( &s4, s3, p, substr( upper(var_get( h, v3 )), n1, n2 ), -1, &(s3[p1+strlen(v3)]), -1, _ );
			v = s4;
			s4 = s3;
			s3 = v;
			}
		else
			{
			strdncpy( &s4, s3, p, upper(var_get( h, v3 )), -1, &(s3[p1+strlen(v3)]), -1, _ );
			v = s4;
			s4 = s3;
			s3 = v;
			}
		p0 = p;
		}
	
	
	// s3 contient maintenant le nom du fichier réveil
	if ( ! file_exists( s3 ) ) return ERROR;
	
	// Démarre le réveil MFT
	
	strdcpy( &fname, s3, _ );
	mft_reveil_start( t_work );
	//logw("sni: apres le mft_reveil_start=",s3,_);
	return OK;
	}



























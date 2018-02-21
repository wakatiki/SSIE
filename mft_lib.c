//#include "elib.h"
//#include "mft.h"

extern char * dirtmp;
extern FILE *fpd;

//------------------------------------------------------------------------------
int mftutil0( char *p_cmd )
//------------------------------------------------------------------------------
	{
	static char *cmd = null, *res = null, *fname1 = null, *fname2 = null /*, *fname=null*/;

	if ( !(p_cmd[0]) ) return ERROR;

	strdcpy( &fname2, mft_dirbin, DIR_CHAR "mftuconf.dat", _ );
	if ( file_size( fname2 ) == 0 )
		{
		strdcpy( &fname1, mft_dirbin, DIR_CHAR "mftuconf0.dat", _ );
		file_copy( fname1, fname2 );
		}

	
	strdcpy( &cmd, mft_dirbin, DIR_CHAR "MFTUTIL" EXE " ", p_cmd, _ );

	system( cmd );

	return OK;
	}


//------------------------------------------------------------------------------
int mftutil( char *p_cmd, char **out )
//------------------------------------------------------------------------------
	{
	static char *cmd = null, *res = null, *fname1 = null, *fname2 = null /*, *fname=null*/;

	if ( !(p_cmd[0]) ) return ERROR;

	strdcpy( &fname2, mft_dirbin, DIR_CHAR "mftuconf.dat", _ );
	if ( file_size( fname2 ) == 0 )
		{
		strdcpy( &fname1, mft_dirbin, DIR_CHAR "mftuconf0.dat", _ );
		file_copy( fname1, fname2 );
		}

	
	strdcpy( &cmd, mft_dirbin, DIR_CHAR "MFTUTIL" EXE " ", p_cmd, _ );

	if ( res ) res[0] = 0;
	
	System( &res, cmd );
	grepv( out, res, "MFTU20I|MFTU00I" );

	if ( file_size( fname2 ) == 0 )
		print( cmd, BR, _ );
		
	
//	if ( ( *out )&&( (*out)[0] )&&( strpos( *out, "MFTU00I", 0 ) < 0 )
//		&&( strpos( *out, "MFTU24W", 0 ) < 0 )&&( stricmp( p_cmd, "about" ) ) )
	if ( ( *out )&&( (*out)[0] )&&( strpos( *out, "error", 0 ) >= 0 ) )
		return ERROR;

	return OK;
	}



//------------------------------------------------------------------------------
int pkiutil( char *p_cmd, char **out )
//------------------------------------------------------------------------------
	{
	static char *cmd = null, *res = null;

	if ( !(p_cmd[0]) ) return ERROR;
	
	trim(strdcpy( &cmd, mft_dirbin, DIR_CHAR "PKIUTIL" EXE " ", p_cmd, _ ));
	
	if ( *out ) (*out)[0] = 0;
	
	System( &res, cmd );
	grepv( out, res, "PKIU20I|PKIU00I" );
	if ( ( *out )&&( (*out)[0] )&&( stripos( *out, "error", 0 ) >= 0 ) )
		return ERROR;

	return OK;
	}


//------------------------------------------------------------------------------
char * mft_errlib( int err )
//------------------------------------------------------------------------------
	{
	static char *msg = null;
	switch( err )
		{
		case 100 : return strdcpy( &msg, itoa( err ), ":Erreur de lecture/écriture fichier", _ );
		case 101 : return strdcpy( &msg, itoa( err ), ":Erreur création du fichier à recevoir", _ );
		case 102 : return strdcpy( &msg, itoa( err ), ":1.FILE - Error allocating the transfer file (physical file or VFM database logical file)"
							BR "2.FILE - The receive file cannot be allocated (FDISP=OLD case)", _ );
		case 108 : return strdcpy( &msg, itoa( err ), ":Erreur lecture du fichier à envoyer."
						BR "Le fichier n'est peut-être pas un fichier texte.", _ );
		case 109 : return strdcpy( &msg, itoa( err ), ":Erreur d'écriture dans le fichier à recevoir", _ );
		case 110 : return strdcpy( &msg, itoa( err ), ":Le fichier à émettre n'existe pas", _ );
		case 111 : return strdcpy( &msg, itoa( err ), ":Espace disque insuffisant", _ );
		case 114 : return strdcpy( &msg, itoa( err ), ":Espace disque insuffisant", _ );
		case 115 : return strdcpy( &msg, itoa( err ), ":Pas de droit d'accès en lecture sur le fichier", _ );
		case 121 : return strdcpy( &msg, itoa( err ), ":Transfert interrompu volontairement", _ );
		case 122 : return strdcpy( &msg, itoa( err ), ":Mémoire insuffisante", _ );

		case 260 :
		case 261 :
		case 262 :
		case 263 :
		case 278 :
		case 280 :
			return strdcpy( &msg, itoa( err ), ":Problème de paramétrage SSL", _ );
		
		case 302 : return strdcpy( &msg, itoa( err ), ":Problème de connexion sur le distant", _ );
		case 405 : return strdcpy( &msg, itoa( err ), ":Impossible de joindre le partenaire", _ );
		case 406 : return strdcpy( &msg, itoa( err ), ":Le transfert ne se fait pas malgrès toutes les tentatives", _ );
		case 408 : return strdcpy( &msg, itoa( err ), ":Le partenaire distant est inconnu de votre MFT", _ );
		case 416 : return strdcpy( &msg, itoa( err ), ":Le nombre de transfert en simultané a atteint le maximum autorisé par votre licence", _ );
		
		case 600 : return strdcpy( &msg, itoa( err ), ":Problème de lecture/écriture fichier chez le distant", _ );

		case 730 : return strdcpy( &msg, itoa( err ), ":Transfert refusé par le distant : Soit il ne vous connait pas, soit le fichier existe déjà et est non vide", _ );
		case 909 : return strdcpy( &msg, itoa( err ), ":Transfert refusé par le distant : il ne nous connait pas.", _ );
		case 916 : return strdcpy( &msg, itoa( err ), ":Le nombre de transfert en simultané a atteint le maximum chez le partenaire distant", _ );
		
		
		default : return strdcpy( &msg, itoa( err ), ":Numéro d'erreur non référencé, veuillez consulter la documentation MFT DIAGI", _ );
		}
	}


//------------------------------------------------------------------------------

#define getparm(p)	\
	print( "getparm ", p, BR, _ );\
	p1 = strpos( s, " " p " ", p1 ); \
	if ( p1 < 0 ) { logw("ERREUR1 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; } \
	p2 = strpos( s, "=", p1+1 ); \
	if ( p2 < 0 ) { logw("ERREUR2 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	p3 = strpos( s, "\n", p2+1 ); \
	if ( p3 < 0 ) { logw("ERREUR3 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	var_set( out, p, v=trim(strdncpy(&s1,&(s[p2+1]), p3-p2-1, _ )) )

#ifdef OS_UNIX
#define getparm2(p)	\
	print( "getparm2 ", p, BR, _ );\
	p1 = strpos( s, " " p " ", p1 ); \
	if ( p1 < 0 ) { logw("ERREUR4 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	p2 = strpos( s, "=", p1+2 ); \
	if ( p2 < 0 ) { logw("ERREUR5 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	p2 = strpos( s, "     *", p2+1 ); \
	if ( p2 < 0 ) { logw("ERREUR6 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	p2 += 6; \
	p3 = strpos( s, "\n", p2 ); \
	if ( p3 < 0 ) { logw("ERREUR7 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	strdncpy(&s1, &(s[p2]), p3-p2, _); \
	p2 = strpos( s, "     *", p3+1 ); \
	p3 = strpos( s, "\n", p3+1 ); \
	if ( ( p2 >= 0 )&&( p3 >= 0 )&&( p2 < p3 ) ) \
		strdncat( &s1, &(s[p2+6]), p3-p2, _); \
	var_set( out, p, trim( s1 ) );
#else
#define getparm2(p)	\
	print( "getparm2 ", p, BR, _ );\
	p1 = strpos( s, " " p " ", p1 ); \
	if ( p1 < 0 ) { logw("ERREUR4 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	p2 = strpos( s, "=", p1+2 ); \
	if ( p2 < 0 ) { logw("ERREUR5 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	p2 = strpos( s, "     *", p2+1 ); \
	if ( p2 < 0 ) { logw("ERREUR6 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	p2 += 6; \
	p3 = strpos( s, "\r", p2 ); \
	if ( p3 < 0 ) { logw("ERREUR7 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	strdncpy(&s1, &(s[p2]), p3-p2, _); \
	p2 = strpos( s, "     *", p3+1 ); \
	p3 = strpos( s, "\r", p3+1 ); \
	if ( ( p2 >= 0 )&&( p3 >= 0 )&&( p2 < p3 ) ) \
		strdncat( &s1, &(s[p2+6]), p3-p2, _); \
	var_set( out, p, trim( s1 ) );
#endif
	
#define getparm3(p)	\
	print( "getparm3 ", p, BR, _ );\
	p1 = strpos( s, " " p " ", p1 ); \
	if ( p1 < 0 ) { logw("ERREUR8 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	p2 = strpos( s, "=", p1+1 ); \
	if ( p2 < 0 ) { logw("ERREUR9 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	p3 = strpos( s, " ", p2+2 ); \
	if ( p3 < 0 ) { logw("ERREUR10 getparm ", p, BR, s, _ ); var_free( out ); return ERROR; }\
	var_set( out, p, trim(strdncpy(&s1,&(s[p2+1]), p3-p2-1, _ )) )


//------------------------------------------------------------------------------
int	mft_idtu_load( char *p_idtu )
//------------------------------------------------------------------------------
	{
	static char *s = null, *s1 = null, *v;
	int p1 = 0, p2, p3, r;
	bool is_file;
	handle out;

	out = var_new();

	r = mftutil( strdcpy( &s1, "listcat content=full,idtu=", p_idtu, _ ), &s );

	if ( r == ERROR )
		{
//if ( fpd ) fprint( fpd, BR, currdate("log"), " : ERREUR MFTUTIL : ", s1, BR, s, BR BR, __ );

		var_free( out );
		return ERROR;
		}

	var_set( out, "IDTU", p_idtu );

	if ( ( strpos( s, " 0 record(s) selected", 0 ) >= 0 )
			||( strpos( s, "_ Warning ( No record found)", 0 ) >= 0 ) )
		{
		sleep(5);

		r = mftutil( s1, &s );
		if ( r == ERROR )
			{
//if ( fpd ) fprint( fpd, BR, currdate("log"), " : ERREUR MFTUTIL : ", s1, BR, s, BR BR, __ );
			error_set( 1, s1, BR, s, _ );
			var_free( out );
			return ERROR;
			}
	
		if ( ( strpos( s, " 0 record(s) selected", 0 ) >= 0 )
				||( strpos( s, "_ Warning ( No record found)", 0 ) >= 0 ) )
			{
//if ( fpd ) fprint( fpd, BR, currdate("log"), " : ERREUR MFTUTIL : ", s1, BR, s, BR BR, __ );
			var_free( out );
			return ERROR;
			}
		}		
	
	getparm( "IDT" );

	getparm( "DIRECT" );
//	if ( strcmp( s1, "RECV" ) )
//		{ var_free( out ); return ERROR; }
	
	getparm( "TYPE" );
	is_file = ( !strcmp( var_get( out, "TYPE" ), "FILE" ) );
//	if ( !is_file )
//		{ var_free( out ); return ERROR; }

	getparm( "STATE" );
/*	
	if ( strcmp( s1, "TERM" ) )
		{ var_free( out ); return ERROR; }
*/
	
	getparm( "DIAGI" );
/*	
	if ( strcmp( s1, "0" ) )
		{ var_free( out ); return ERROR; }
*/
	
	getparm( "DIAGP" );
	
	if ( is_file )
		{ getparm( "IDF" );	}
	else
		{ getparm( "IDM" ); }
	getparm( "NIDF" );
	
	if ( is_file )
		{
		getparm2( "FNAME" );
		getparm2( "NFNAME" );
		getparm2( "PARM" );
		}
	
	getparm( "PART" );
	getparm( "SPART" );
	getparm( "RPART" );
	getparm( "IPART" );
	getparm( "NPART" );
	getparm( "DATED" );
	getparm( "TIMED" );
	getparm3( "DATEB" );
	getparm( "DATEE" );
	getparm3( "TIMEB" );
	getparm( "TIMEE" );
	getparm3( "RETRY" );
	getparm3( "RETRYN" );
	getparm3( "RETRYM" );
	getparm3( "RETRYW" );
	getparm( "NCARS" );
	getparm( "FCARS" );
	getparm( "EXITA" );
	getparm( "EXITE" );
	getparm( "EXITEOT" );
	getparm( "SUSER" );
	getparm( "RUSER" );
	getparm2( "SAPPL" );
	getparm2( "RAPPL" );
	
	#ifndef OS_WINDOWS
	getparm( "SSLMODE" );
	getparm( "SSLAUTH" );
	getparm( "SSLCIPH" );
	getparm( "SSLPROF" );
	getparm( "SSLRMCA" );
	getparm( "SSLUSER" );
	getparm2( "SSLCFNAM" );
	getparm2( "SSLPARM" );
	#endif

	getparm2( "MSG" );
	getparm( "NRPART" );
	getparm( "PROT" );

	return out;	
	}



//------------------------------------------------------------------------------
int	mft_listcat_full( char *select )
//------------------------------------------------------------------------------
	{
	static char *s = null, *s1 = null, *v;
	int p1 = 0, p2, p3, r;
	bool is_file;
	handle out;
	
	out = var_new();
	r = mftutil( strdcpy( &s1, "listcat content=full,", select, _ ), &s );
//print( s1, BR, s, BR, _ );

	if ( r == ERROR )
		{
		error_set( 1, s1, BR, s, _ );
		return ERROR;
		}

	if ( ( strpos( s, " 0 record(s) selected", 0 ) >= 0 )
			||( strpos( s, "_ Warning ( No record found)", 0 ) >= 0 ) )
		{
		error_set( 1, s1, BR, s, _ );
		return ERROR;
		}		
	
	getparm( "IDT" );
	getparm( "IDTU" );
	getparm( "DIRECT" );
//	if ( strcmp( s1, "RECV" ) )
//		{ var_free( out ); return ERROR; }
	
	getparm( "TYPE" );
	is_file = ( !strcmp( var_get( out, "TYPE" ), "FILE" ) );
	
	getparm( "STATE" );
//	if ( strcmp( s1, "TERM" ) )
//		{ var_free( out ); return ERROR; }
	
	getparm( "DIAGI" );
//	if ( strcmp( s1, "0" ) )
//		{ var_free( out ); return ERROR; }
	
	getparm( "DIAGP" );
	
	if ( is_file )
		{ getparm( "IDF" );	}
	else
		{ getparm( "IDM" ); }
	getparm( "NIDF" );
	
	if ( is_file )
		{
		getparm2( "FNAME" );
		getparm2( "NFNAME" );
		getparm2( "PARM" );
		}
	
	getparm( "PART" );
	getparm( "SPART" );
	getparm( "RPART" );
	getparm( "IPART" );
	getparm( "NPART" );
	getparm( "DATED" );
	getparm( "TIMED" );
	getparm3( "DATEB" );
	getparm( "DATEE" );
	getparm3( "TIMEB" );
	getparm( "TIMEE" );
	getparm3( "RETRY" );
	getparm3( "RETRYN" );
	getparm3( "RETRYM" );
	getparm3( "RETRYW" );
	getparm( "NCARS" );
	getparm( "FCARS" );
	getparm( "EXITA" );
	getparm( "EXITE" );
	getparm( "EXITEOT" );
	getparm( "SUSER" );
	getparm( "RUSER" );
	getparm2( "SAPPL" );
	getparm2( "RAPPL" );
	
	#ifndef OS_WINDOWS
	getparm( "SSLMODE" );
	getparm( "SSLAUTH" );
	getparm( "SSLCIPH" );
	getparm( "SSLPROF" );
	getparm( "SSLRMCA" );
	getparm( "SSLUSER" );
	getparm2( "SSLCFNAM" );
	getparm2( "SSLPARM" );
	#endif
	
	getparm2( "MSG" );
	getparm( "PROT" );

	return out;	
	}


//------------------------------------------------------------------------------
int	mft_find( char *select, char *to_search )
//------------------------------------------------------------------------------
	{
	static char *r = null, *cmd = null, *s = null, *s1 = null, *s2 = null, *v, *v2;
	int p, p0, p1 = 0, p2, p3;
	bool is_file;
	static handle h = 0;
	
	h = var_new();
	p = mftutil( strdcpy( &s1, "listcat ", select, _ ), &r );
	if ( p == ERROR )
		{
		error_set( 1, s1, BR, r, _ );
		return ERROR;
		}

	if ( ( strpos( r, " 0 record(s) selected", 0 ) >= 0 )
			||( strpos( r, "_ Warning ( No record found)", 0 ) >= 0 ) )
		{
		error_set( 1, s1, BR, r, _ );
		return ERROR;
		}		
	
	
	p = strpos( r, "-------- ---- ", 0 );
	p0 = strpos( r, "\n", p ) + 1;
	
	while( ( p1 = strpos( r, "\n", p0 ) ) > 0 )
		{
/*
 K70IX01I RFT  R2MMS    A2112192          2          2   0 CP 80%
*/			
		v2 = substr( r, p0, p1-p0 );
		if ( strlen( v2 ) < 40 ) break;
			
		strdcpy( &s1, "listcat content=full,part=", substr( v2, 2, 8 ),
			",direct=", (char*)(( strcmp( substr( v2, 11, 1 ), "R" ) ) ? "send":"recv"),
			",idt=", substr( v2, 25, 8 ), _ );
		
		p = mftutil( s1, &s );
		if ( p == ERROR )
			{
			error_set( 1, s1, BR, s, _ );
			return ERROR;
			}
		
		if ( str_iexists( s, to_search ) )
			print( v2, BR, _ );
		
		p0 = p1+1;
		}
	

	return OK;	
	}






















#define MAIN
//#define TR
#include "elib.h"
#include "elib4.c"

extern bool elib_print_to_mem;
extern bool elib_print_to_screen;
extern char * elib_print_buffer;


int main( int ac, char **av )
	{
	char *fname = null, *fname1 = null, *s = null, *s1 = null, *s2 = null, *v, *v1;
	T_FILE_INFO *tf;
	long long i, j, n, n1, n2;
	int p, r;
	handle hLog, hVar, hI, h, h1;
	HANDLE fLock;
	FILE *fp;

	char *port, **ts;
	int p1, p2, p3, b;

	DIR *rep;
	struct dirent *fic;

	argc = ac;
	argv = av;

	strdcpy( &fname, "c:/e/src/ssie7/portail/aide0/*", _ );
	tf = NULL;
	while( tf = file_list( tf, fname, "nodir" ) )
		{
		strdcpy( &fname, "c:/e/src/ssie7/portail/aide0/", tf->name, _ );
		strdcpy( &fname1, "c:/e/src/ssie7/portail/aide/", tf->name, _ );
		print( tf->name, BR, _ );
		file_load( &s, fname );
		str_to_utf8( &s );
		file_save_str( fname1, s, -1 );
		}

wexit(0);

/*
	if ( strpcmp( "*_CABE_*", "Q_CABE_" ) )
		print( "true", BR, _ );
	else
		print( "false", BR, _ );
*/

	// TESTS STRPCMP
	
	print( "strpcmp(",v = "*bbb*", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );

	print( "strpcmp(",v = "*ddd*", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );

	print( "strpcmp(",v = "*eee*", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*bbb", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*dd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "a*dd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "a*edd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "a*bc*dd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc*ddd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc*d*dd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc*d*dd*", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc*d***dd**", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc****?cdd?", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc*d***d**d", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	


v1 = "2016_07_13_10_00_56F5313102F5313102F53131A1TESTAPPEF5313102TESTSSIE[NULL]R2MMSBD7D0003ZSSIE7.EXE";
v = "*3102*";

//	print( "strpcmp(",v = "*a*bc****?cdd?", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );




wexit(0);








	strdcpy( &s, str_quote( "zzzz\\nzzzz" ), ",", str_C_quote("aaaa\\nbbbb"), _ );
print( s, BR, _ );
	ts = str_explode( s, "," );
	for( i = 0 ; ts[i] ; i++ )
		print( ts[i], BR, _ );
	
wexit(0);


	// HLOGC avec accès concurents
	
	hLog = hlog_init( "c:/temp/hlog", null );
	if ( hLog == ERROR )
		print( "ERREUR hlog_init", BR, __ );
	else
		hlogu( hLog, 0, "", "coucou", _ );

pause();

		hlogu( hLog, 0, "erreur", "coucou", _ );

pause();

		hlogu( hLog, 0, "ok", "coucou", _ );

pause();

		hlogu( hLog, 0, "alerte", "coucou", _ );

pause();

		hlogu( hLog, 0, "annule", "coucou", _ );

pause();

		hlogu( hLog, 0, "en cours", "coucou", _ );

//	for(;;Sleep(1))
//		hlogc( hLog, 0, "", "coucou", _ );
	
	wexit(0);
	













	strdcpy( &fname, "c:/e/bin/windows/ssie7sec.exe", _ );
	ssl_pgm( fname );
	
/*	strdcpy( &fname, "c:/e/tmp/pepsiaxa.dat", _ );
	h = var_load( fname );
	v = var_get( h, "part_ssl_certifr" );
print( "v = ", v, BR BR, _ );
*/
	strdcpy( &fname, "c:/e/tmp/tmp1.der", _ );

	print( der_to_pem( fname ), BR, _ );


wexit(0);










	tr_init("c:\\e\\elib_traces");

	
	// TESTS REPLACE
/*	
	strdcpy( &s, "ce texte contient MOT10 et Mot2.", _ );
	strdcpy( &s1, "MOT(10)=>9,Mot(2)=>1", _ );
//	strdcpy( &s1, "*M*10=>mot9", _ );
	p = r = 0;
	n = strRegExpr( s1, &s, &p, 0, true, false, 0, &r );
	print( itoa( n ), ", r=", itoa(r), ", s=", s, BR, _ );
	
	wexit(0);
*/

	// TESTS SEARCH
	
	strdcpy( &s, "ce texte contient MOT10 et Mot2.", _ );
	strdcpy( &s1, "~mot2=>toto", _ );
	p = 0;
	n = strRegExpr( s1, &s, &p, 0, true, false, 0, null );
	print( itoa( n ), ", pos=", itoa(p), BR, _ );
	
	wexit(0);


	// TESTS CTRL
	
/*
	strdcpy( &s, "MOT10 et, Mot2.", _ );
	strdcpy( &s1, "[0-z, ' ', ., \\,]*", _ );
	p = 0;
	n = strRegExpr( s1, s, &p, 0, false, false, 0, null );
	print( (char*)(( n ) ? "true" : "false"), BR, _ );
	
	wexit(0);
*/	
/*
	strdcpy( &s, "MOT10 et Mot2.", _ );
	strdcpy( &s1, "[*~mot10< & !*Mot2]?[]*", _ );
	p = 0;
	n = strRegExpr( s1, s, &p, 0, false, false, 0, null );
	print( (char*)(( n ) ? "true" : "false"), BR, _ );
	
	wexit(0);
*/	
/*
	strdcpy( &s, "ce texte contient MOT10 et Mot2.", _ );
	strdcpy( &s1, "*[~mot1<,~mot2]?[]*", _ );
	p = 0;
	n = strRegExpr( s1, s, &p, 0, false, false, 0, null );
	print( (char*)(( n ) ? "true" : "false"), BR, _ );
	
	wexit(0);
*/	
/*
	strdcpy( &s, "ce texte contient mot10 et mot2.", _ );
	strdcpy( &s1, "*[>*t1*<,mot3]?[]*", _ );
	p = 0;
	n = strRegExpr( s1, s, &p, 0, false, false, 0, null );
	print( (char*)(( n ) ? "true" : "false"), BR, _ );
	
	wexit(0);
*/	
/*
	strdcpy( &s, "c:/temp/fic.t*mp.txt.bat.bat.txt.t*mp.bat", _ );
	strdcpy( &s1, "*[ .txt , \".t*mp\" | .bat ]?2", _ );
	p = 0;
	n = strRegExpr( s1, s, &p, 0, false, false, 0, null );
	print( (char*)(( n ) ? "true" : "false"), BR, _ );
	
	wexit(0);
*/	
/*
	strdcpy( &s, "c:/temp/fic.tmp", _ );
	strdcpy( &s1, "c:/?2mp/*.tm**p", _ );
	p = 0;
	n = strRegExpr( s1, s, &p, 0, false, false, 0, null );
	print( (char*)(( n ) ? "true" : "false"), BR, _ );
	
	wexit(0);
*/








	
	// TESTS STRJCMP
/*
	print( "strjcmp(",v = "abcde", ", ", v1 = "abcdef", ")=", _ );
	print( ( strjcmp( v, v1 ) ) ? "true" : "false", BR, _ );

	print( "strjcmp(",v = "abcde?", ", ", v1 = "abcdef", ")=", _ );
	print( ( strjcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strjcmp(",v = "abcd?", ", ", v1 = "abcdef", ")=", _ );
	print( ( strjcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strjcmp(",v = "?bcde?", ", ", v1 = "abcdef", ")=", _ );
	print( ( strjcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strjncmp(",v = "?bcdef?", ", ", v1 = "abcdef", ", ", itoa(n=6), ")=", _ );
	print( ( strjncmp( v, v1, n ) ) ? "true" : "false", BR, _ );
	
	print( "strjncmp(",v = "?bcdef?", ", ", v1 = "abcdef", ", ", itoa(n=7), ")=", _ );
	print( ( strjncmp( v, v1, n ) ) ? "true" : "false", BR, _ );
*/
	
	// TESTS STRPCMP
	
	print( "strpcmp(",v = "*bbb*", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );

	print( "strpcmp(",v = "*eee*", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*bbb", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*dd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "a*dd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "a*edd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "a*bc*dd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc*ddd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc*d*dd", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc*d*dd*", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc*d***dd**", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc****?cdd?", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	
	print( "strpcmp(",v = "*a*bc*d***d**d", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );
	


v1 = "2016_07_13_10_00_56F5313102F5313102F53131A1TESTAPPEF5313102TESTSSIE[NULL]R2MMSBD7D0003ZSSIE7.EXE";
v = "*3102*";

//	print( "strpcmp(",v = "*a*bc****?cdd?", ", ", v1 = "abbbbcccddd", ")=", _ );
	print( ( strpcmp( v, v1 ) ) ? "true" : "false", BR, _ );

	
	wexit(0);
	
	
	

	h = var_from_cmd( 0, "chp1 >< 3" );
	var_print( h, BR );

//bd_test();
wexit(0);

	//------------------------------------------------------------------------------
	// TESTS IDAT
	
//	idat_create( "c:\\temp\\idat\\mftcat.dat", "c:\\temp\\idat\\mftcat", "part,idf,rappl,dt_crea_day,status,errno", "" );
	
	hI = idat_open( "c:\\temp\\idat\\mftcat.dat" );

/*
	h = var_from_cmd( 0, "idt=A0000003,part=F5313102,idf=R2MMS,status=T,errno=302" );
	
	for( i = 0 ; i < 100 ; i++ )
		{
		var_set( h, "dt_crea", itoa(n=sysdate) );
		var_set( h, "dt_crea_day", itod( n, "aaaa_mm_jj" ) );
		var_set( h, "rappl", itona( i, 8 ) );
		var_set( h, "errno", "0" );
		idat_insert( hI, h, &s, true );
		}
*/

	idat_select( hI, "errno=0", "part", &s );	
	
	print( s, BR, _ );
	
	
//	idat_update( hI, h, s, true );


/*	
	strdcpy( &s, "A0000004", _ );
	h = var_from_cmd( 0, "idt=A0000004,part=F5313102,idf=R2MMSB,status=T,errno=0" );
	idat_insert( hI, h, &s, false );
	
	strdcpy( &s, "A0000002", _ );
	h = var_from_cmd( 0, "idt=A0000002,part=F5313102,idf=R2MMS,status=C,errno=302" );
	idat_insert( hI, h, &s, false );

	strdcpy( &s, "A0000003", _ );
	h = var_from_cmd( 0, "idt=A0000003,part=F5313102,idf=R2MMS,status=T,errno=0" );
	idat_insert( hI, h, &s, false );
*/

		
	wexit(0);

	//------------------------------------------------------------------------------
	
	tf = NULL;
	strdcpy( &fname, "c:/e/apps/*", _ );
	n = file_count( fname );
	print( "file_count = ", itoa(n), BR, _ );
	
	
	for( ; ; )
		{
		tf = NULL;
		n1 = 0;
		while( tf = file_list( tf, fname, "absdir" ) )
			n1++;
		
		if ( n1 != n )
			{
			print( "ERREUR:", itoa(n1), _ );
			break;
			}
		}
		
	wexit(0);
	
	
	
	port = "1765";
	
	
	if ( ! is_numeric( port ) ) return false;

	#define prefix_port ":"
	
//	System( &s1, strdcpy( &s2, "netstat -an | grep tcp | grep \"", prefix_port, port, " \"", _ ) );

	strdcpy( &s1,
		"tcp        0      0 10.122.0.81:1765            163.84.212.111:53748        CLOSE_WAIT" BR
		"tcp        0      0 10.122.0.81:1765            163.84.212.113:38922        CLOSE_WAIT" BR
		"tcp        0      0 10.122.0.81:1765            163.84.212.112:6364         CLOSE_WAIT" BR
		"tcp        0      0 10.122.0.81:1765            163.84.212.112:3551         CLOSE_WAIT" BR
		, _ );
	strdcpy( &s2, prefix_port, port, " ", _);
	for( p1 = -1, b = false ; ; )
		{
		p1 = strpos( s1, s2, p1+1 );
		if ( p1 < 0 )
			{
			print( (char*)( ( !b ) ? "true" : "false" ), BR, _ );
			goto fin;
//			return ( !b ) ? true : false;
			}
		b = true;
		p2 = strpos( s1, "LISTEN", p1);
		if ( p2 > 0 )
			{ // Vérifie que listen est sur la même ligne
			p3 = strpos( s1, "\n", p1);
			if ( ( p3 < 0 )||( p3 > p2 ) )
				{
				print( "-1", BR, _ );
				goto fin;
//				return -1;	// Le port est pris => pas la peine d'attendre qu'il se libère
				}
			}
		}


	print( (char*)( ( !b ) ? "true" : "false" ), BR, _ );

	fin:
	wexit(0);


/*
	if ( tcp_request( "10.122.47.38:1760", "part MFTCCWEX get", &s ) == sERROR )
		{
		print( "ERREUR : ", s, BR, _ );
		}
	else
		print( s, BR, _ );
*/		
	
/*	
	h = var_new();
	var_set( h, "param", "valeur" );
	v = var_get( h, "param" );
	print( v, BR, _ );
	
	var_save( h, "essai.dat" );
	print( "var_save OK" BR, _ );
	var_free( h );
	
	h = var_load( "essai.dat" );
	print( "var_load OK : ", itoa( h ), BR, _ );
	v = var_get( h, "param" );
	print( v, BR, _ );
*/	
	
	wexit(0);
	
	
/*	
	h = var_from_str( 0, s );
	var_save( h, "test.dat" );
	var_free( h );
	
	for( n = 0; ; n++ )
		{
		if ( ! (n%1000) ) print( ", ", itoa(n/1000), _ );
		h = var_load( "test.dat" );
		var_free( h );
		}
*/
	
	
/*
//	n = file_size( "/bpfr/fichier.3.GIGA" );
	n = file_size( "/home/fichier.2.5.GIGA" );
	print( itoa( n ), BR, _ );
/*	
	fp = fopen( "/bpfr/fichier.3.GIGA", "rb" );
	if ( fp == NULL ) print( "ERREUR" BR, _ );
	else print( "OK" BR, _ );
*/	
	
	wexit(0);
	
		
	// TEST DE VAR_REPLACE
	
	hVar = var_new();
	var_set( hVar, "testnum", "1.0" );


	v = "Test var_replace : $$TestNum";
	var_replace( hVar, v, &s );
	print( s, BR, _ );


	v = "Test var_replace : $$TestNum(bis)";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );


	v = "Test var_replace : $$TestNum[].3";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );


	v = "Test var_replace : $$TestNum;.4";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );


	v = "Test var_replace : $$$TestNum;.5";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );


	v = "Test var_replace : $$$ .6";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );


	v = "Test var_replace : $$$;.7";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );


	v = "Test var_replace : $$TestNum[0,1].8";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );



	v = "Test var_replace : $$TestNum[2].9";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );



	v = "Test var_replace : $$TestNum[,1].10";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );



	v = "Test var_replace : $$TestNum[-1].11";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );



	v = "Test var_replace : $$TestNum;.12$$Toto;(fin)";
	var_replace( hVar, v, &s );
	print( BR, s, BR, _ );

	wexit(0);


/*	
	print( crypte64( &s, "coucou", -1 ), BR, _ );
	
	n = uncrypte64( &s1, s );
	print( itoa(n), " : ", s1, BR, _ );
*/	
	wexit(0);


s = alloc( 12 );	
s[0] = -30;
s[1] = 7;
s[2] = 42;
s[3] = 93;
s[4] = 0;
s[5] = 7;
s[6] = 0;
s[7] = 121;
s[8] = 0;
s[9] = -97;
s[10] = 63;
s[11] = -119;

	toBase64( &s1, s, 12 );
	
//	crypte64( &s, "coucou", -1 );
	print( s1, BR, _ );

	n = fromBase64( &s2, s1 );
	print( itoa(n), " : ", _ );
	for( i = 0 ; i < n ; i++ )
		if ( s[i] != s2[i] )
			{
			print( "ERREUR from64" BR, _ );
			break;
			}

	if ( i == n ) print( "OK" BR, _ );
	
	wexit(0);
	
	
	ssl_pgm( "ssie7sec.exe" );
	
	h = var_load( "c:/appli/ssie7/conf/ssie7.dat" );
	h1 = var_load( "c:/appli/ssie7/conf/part/F5313101.dat" );
	
	ssl_encrypt_file( h, h1, "c:/bin/windows/mftutil.exe", "c:/bin/windows/mftutil.zip", "ssl" );
	
	wexit(0);


	SystemH( &s, "dir" );
	print( s, BR, _ );
	wexit(0);
	
	
//	h = var_from_cmd( 0, "mftutil send PART=K99IX03I, IDF=R2MMS, SAPPL=GUCD0304BT, RAPPL=GQQ49ACC49I, SUSER=niyn0000GUCD0304, RUSER=BT, IDA=niyn0000, NRECFM=F, NLRECL=80, NBLKSIZE=6160, PARM=MI20130924130958&00000I1006421.V0,FNAME=//kuc23d03/COMMUN/LOGICIEL/RELVTSP/SSIE/KUC2ED13/niyn0000," );
	h = var_from_cmd( 0, "//kuc23d03/COMMUN/LOGICIEL/RELVTSP/SSIE/KUC2ED13/niyn0000," );

var_print( h, BR );
wexit( 0 );
	
	
	// Fonction strpcmp avec '?'
	
	print( (char*)( ( strpcmp( "AB???*", "ABCDEFGH" ) ) ? "true" : "false" ), BR, _ );
	
	wexit(0);
	
	
	// Fonction hlog_index
	
//	print( hlog_index( &s, "c:/edf/appli/ssie7/log/flux", null, "!erreur* & !ok* & !alert & !annule", null, 10, 0 ), BR, _ );
	print( hlog_index( &s, "c:/edf/appli/ssie7/log/flux", null, "821", null, 10, 0 ), BR, _ );
	
	wexit(0);
	
	// fonction str_exists avec tests complexes
	
	v = "Ceci est un test de str_exists";
	v1 = "!ceci est | Test & eXists";
	print( (char*)((str_iexists( v, v1 )) ? "true" : "false" ), BR, _ );
	
	wexit(0);
	
	// Fonction hday
	
	print( itoa( hday(time(NULL)-(60*60)) ), BR, _ );
	wexit(0);


	// HLOGC avec accès concurents
	
	hLog = hlog_init( "c:/temp", "1" );
	if ( hLog == ERROR )
		print( "ERREUR hlog_init", BR, __ );
	else
	for(;;Sleep(1))
		hlogc( hLog, 0, "", "coucou", _ );
	
	wexit(0);
	

	// TESTS FONCTIONS SSL

	ssl_pgm( "c:\\edf\\openssl.exe" );

	h = var_new();

	if ( ssl_conf_load( h, "c:/edf/certif/inter.key", "c:/temp/ca_lib" ) == ERROR )
		print( error_msg, BR BR BR, _ );

	var_print( h, BR );
	
	wexit(0);


	// TESTS FONCTIONS SSL

	h = var_new();
	v = 
		"-----BEGIN CERTIFICATE-----" BR
		"MIIDGzCCAgMCAQEwDQYJKoZIhvcNAQEEBQAwVDELMAkGA1UEBhMCRlIxFjAUBgNV" BR
		"BAgTDUlsZS1kZS1GcmFuY2UxDjAMBgNVBAcTBVBhcmlzMQwwCgYDVQQKEwNFREYx" BR
		"DzANBgNVBAsTBkNTUC1JVDAeFw0xMzAzMTExMDI5NDFaFw0yMzAzMTExMDI5NDFa" BR
		"MFMxCzAJBgNVBAYTAkZSMRYwFAYDVQQIEw1JbGUtZGUtRnJhbmNlMQ4wDAYDVQQH" BR
		"EwVQYXJpczEMMAoGA1UEChMDRURGMQ4wDAYDVQQLEwVDQ1dFWDCCASIwDQYJKoZI" BR
		"hvcNAQEBBQADggEPADCCAQoCggEBAKZwtCbv7gX1WOpHHsRFXJZ1pvSWzwF5CK3/" BR
		"EYrvlpdN4tXD6nYRVFyYRXo2VacfbP0p5vWOmtn5MHkn5LKMl5QeosOnNQRwLb3z" BR
		"5OSXm1MOfixiR3pNAfQFhESv+67Q9Kz8K/cuom4T7MYQ+qErSJoYnifO/EX/HHmy" BR
		"SQYZ7e7Ni9FoCYGXzo8v0i4Hl8SqSEL9PHMUA9FQ3il4wKcz7az2hvP3sjjP3hOw" BR
		"2uJQsm0OnLtDjo1z2PWx2RBlVgvb0V97ZVZfI9WwAxJuLR6tQ+xfxym4YT2z9KSq" BR
		"+iGczAZulmX6d61e1jrin4B0wVQLfZ7pMjYBmhics2DZCs87Gz0CAwEAATANBgkq" BR
		"hkiG9w0BAQQFAAOCAQEAW+aO5zMhDyGnxWQtOnfP41mveFdGkoaCFOtKvNhw2zqm" BR
		"tMFKJ2cK7/LLHqWhi5hDLNS985PWf5oUpzlYz68VZIOIFpqY5zLGlHIVP2HeL1uA" BR
		"RKSiR3+M9GaIc2aKxGCl+RoD5zAPlXUc4eP5pch6GS7pdrArQDd+B5ZOGlgoyfEl" BR
		"XVvRO3XtUY9eg7NzKFthswOhQKFL6k3jb0l1cB0jbKhKhNp6F7V9Qx7FvOo3xwU+" BR
		"EW3XLo6RI+ucRkXWPfSihieOUQUSBkGtagc/LBwG86a4Bxs9ftyif1h+ARSkvO9k" BR
		"cJN/0KJh06oiOikrL87PCgB9D+ZGsJa2K8hlhrHJpw==" BR
		"-----END CERTIFICATE-----" BR
		BR
		BR
		"-----BEGIN RSA PRIVATE KEY-----" BR
		"MIIEowIBAAKCAQEApnC0Ju/uBfVY6kcexEVclnWm9JbPAXkIrf8Riu+Wl03i1cPq" BR
		"dhFUXJhFejZVpx9s/Snm9Y6a2fkweSfksoyXlB6iw6c1BHAtvfPk5JebUw5+LGJH" BR
		"ek0B9AWERK/7rtD0rPwr9y6ibhPsxhD6oStImhieJ878Rf8cebJJBhnt7s2L0WgJ" BR
		"gZfOjy/SLgeXxKpIQv08cxQD0VDeKXjApzPtrPaG8/eyOM/eE7Da4lCybQ6cu0OO" BR
		"jXPY9bHZEGVWC9vRX3tlVl8j1bADEm4tHq1D7F/HKbhhPbP0pKr6IZzMBm6WZfp3" BR
		"rV7WOuKfgHTBVAt9nukyNgGaGJyzYNkKzzsbPQIDAQABAoIBAQCIVTUb19hOhSOp" BR
		"80ueVXp7vC9fldBGUo/rhk0mUw41LZUqqGn7AELr/cabTrpmRHWQAayvY3owJc+w" BR
		"zh6kqkjXCSklvmbNFd5CA+9v10dhhzOOw4ntAfshmQs5ADXWgk6JC54vwMFIDYYa" BR
		"z3mcXsn/fz69OOynoUn/3Su3lbGxstYSP/RidQR+2etrw+VKcEP9dqjCPRlQpl6d" BR
		"cArPdvMgfHBYUNbiJdX8WmLtPsY1Q+UtX3aDXCfBQf+drxGgSfeFliE+ZbJGMPj0" BR
		"M79vGvjphGh98Xt0XjkPsckbuuhOkKnBlWWRcWJE247kwKaQv10Oo+ItcN7b5Abr" BR
		"AIQmuWOBAoGBANRJHSGM2tEzmwpZf26J6WdO8OWDOyFK2AGV8GIGbvRFKsU/Yk9J" BR
		"o6Pr6RmAM9ZGPOpxuZZwIQuDbOAnhAA9Qr9h2ssYm69UvFS1tJ/ShlQ3iPEA3eJ4" BR
		"vFjnsG1u0cIIoFqAoNlg+mTD1TT/s9ErkfzXEnayPq1Lr96IpxHp0FnRAoGBAMi2" BR
		"zEzhpY+ISDrpt990RvAXQhLaVypawMdsNyLlZNnbjADIp5aOjY7n8WtWGzP5XyqP" BR
		"fCjDHPnsV8ui0DpZ6CD1GjQcEABE2WZAglSoqlOv7jkLUzdQZk4VQ8G64/hjL6jV" BR
		"b48Z1pnHj33c5ihC7EBdAz2j2cG3HipzFGtLWRmtAoGAIpnQL7m/qL2albNEvAWq" BR
		"j3YkeaRMLP7/+jIwPioCR7pGYGQWaAn+rCc1lTx6NoC4SRZEQ6r/F9tSEa8fKtLR" BR
		"YFRYzyWxqoBLOhtr12j/EBcFMRMNF27KdEs/THb1pYS2bkns7cGKzoXE6KCM9Jj/" BR
		"cJC1p7ch1Ckz/zeS066ikSECgYBXW5hX7e4NJpepbIfm7cjiPn03j2hYut8k7Oii" BR
		"e40FdRTeUnb+hFUAWdstiHCWknblI0Y6x5EZBdDAhMB0ZSUvEv6wp3YfZ9fnNTSE" BR
		"fFSF0BCzK0aw0IF2grLW4uzN8v2PUjYX5nsMu1qswTQ7WcVRzinW7jWcpoKunx/I" BR
		"AydokQKBgE48kR4B6Sb46KPlSUk0oxt7h7ZbVXnoG+tV+v+Dey1JLmKqe6BFjlob" BR
		"WCibyyDc0HdpiIPkzEY1or4yfVFkWuiLdL9Ej4xCjEv4uPlOJH284lZ4+XF+sfV5" BR
		"rcnBT0tjrNeC05vq2J6neBNkLXDMFOk7+M5frKXqKAaDQahrBEgr" BR
		"-----END RSA PRIVATE KEY-----"
/*
		BR
		BR
		"-----BEGIN CERTIFICATE-----" BR
		"MIIDbTCCAlWgAwIBAgIJALkMJ+Dal6QWMA0GCSqGSIb3DQEBBQUAMFQxCzAJBgNV" BR
		"BAYTAkZSMRYwFAYDVQQIEw1JbGUtZGUtRnJhbmNlMQ4wDAYDVQQHEwVQYXJpczEM" BR
		"MAoGA1UEChMDRURGMQ8wDQYDVQQLEwZDU1AtSVQwHhcNMTMwMzExMDk1OTMyWhcN" BR
		"MjMwMzExMDk1OTMyWjBUMQswCQYDVQQGEwJGUjEWMBQGA1UECBMNSWxlLWRlLUZy" BR
		"YW5jZTEOMAwGA1UEBxMFUGFyaXMxDDAKBgNVBAoTA0VERjEPMA0GA1UECxMGQ1NQ" BR
		"LUlUMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyaigdU/dkjKKVDly" BR
		"U2BfLDhLZm74Ka1Ugm3FqNgOixxdXeqVcPf4LBKNtpLBYjQ1XKy0nce3jREeIue+" BR
		"6+G6KV5VZZ4g5b9ZJFMVHbHHuAwuFXWw4Bcy3KN8AvPs9wA2+QmM7+cVkjsIho68" BR
		"jTzMXd+8/937wXl+h2dC8DUk3ZwzQvfLzYCzcykmAKkLje1JxXIEw1Ri26vxbNsY" BR
		"jc7X8bFAotGSJpLtkjpBhbZ/3m/qBVQmYGCkxXZtPEOQJkV0GO9dASNEF8lUGEsn" BR
		"Ft/cDGVgCf6C9YgWp5aMhIHYG/WOLpBXOJwwaZaZ7cXhXTtVfJIQpgwV3TX9E7pU" BR
		"ExFzjwIDAQABo0IwQDAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBQRwnC+0iCC" BR
		"6YXaXvGOWY7YQEbtRjAOBgNVHQ8BAf8EBAMCAQYwDQYJKoZIhvcNAQEFBQADggEB" BR
		"AK2i79vusZBQdgkAQVBvG2u4ACGvV8Qlwb7x5rJvHBKibif7woS40f9v5jPSPHZb" BR
		"w6SpR8a2yJMeRaLjx1GVpk2LZoft7if7MgE8ChXaSArfRBz7nrwhOyH/bp4//m3o" BR
		"0XRrOTkKKT5D/WzXatLMcAjboipaALF0WYo10V0c654O8RKs+XHtCsGGbyf//A2e" BR
		"Ljj/CKOeh8NcmPUgZzmDC2CkXrCL+wv2SB5hJos0rWG7mMPTjI2tJ9IEf08Hp8DE" BR
		"JywTmELTb+Ot2307V6MkYGN+h9kA9Ft425EAtngaXmBLzGJpM2R9QST3FysjJQ5r" BR
		"AP0dUxOJll8GedPcawCicoE=" BR
		"-----END CERTIFICATE-----" BR
*/
		;

	file_save_str( "c:/tmp/testssl.tmp", v, -1 );

	if ( ssl_certif_load( h, "certif0.tmp", "c:/temp/ca_lib", "ssl" ) == ERROR )
		print( error_msg, BR BR BR, _ );

	var_print( h, BR );

	wexit(0);
	
	if ( ssl_conf_load( h, "c:/tmp/testssl.tmp", "c:/temp/ca_lib" ) == ERROR )
//	if ( ssl_certif_load( h, "c:/tmp/testssl.tmp", "c:/temp/ca_lib" ) == ERROR )
//	if ( ssl_conf_load( h, v, "c:/temp/ca_lib" ) == ERROR )
//	if ( ssl_certif_load( h, v, "c:/temp/ca_lib" ) == ERROR )
		print( error_msg, BR BR BR, _ );

	var_print( h, BR );
	

/*
	strdcpy( &s, v = itod( time(NULL), "jj/mm/aaaa" ), " 00:00:00", _ );
	print( s, " : ", itoa(dtoi( s, "jj/mm/aaaa hh:mm:ss" )), BR, _ );
//	print( itoa(dtoi( s, "jj/mm/aaaa" )), BR, _ );
	print( itoa(dtoi( "22/04/2013 00:00:00", "jj/mm/aaaa hh:mm:ss" )), BR, _ );
*/	
/*
	hLog = hlog_init( "c:/edf/appli/ssie7/data/flux", "115" );
	hlogr( hLog, 12, &s, &s1 );
	
	print( s, BR "----" BR, s1, BR, _ );
*/	
/*
	tf = NULL;
	while( tf = file_list( tf, "c:/edf/appli/ssie7/data/flux/73.lo*", "nodir absdir" ) )
		print( tf->name, BR, _ );
*/	
/*
	h = var_from_str( 0, "send=true, fname=mft.c, adle=F5313101, appe='', adlr=F5313101, appr='', parm='', idf=R2MMSB" );
	strdcpy( &s, "c:\\tmp\\$parm", _ );
	
	var_list_prepare( h );
	var_replace( h, &s );
	
	print( s, BR );
*/

	

//	tcp_request( "127.0.0.1:1765", "status", &s );
//	print( s, BR, _ );
	
/*	
	for( n = 0; ; n++ )
		{
		print( ", ", itoa(n), _ );
		tcp_request( "127.0.0.1:1765", "status", &s );
//		print( s, BR, _ );
		}
*/	
/* 70+165+60+68+75+105+90+80+87+112
	for( i = 0 ; i < 100 ; i++ )
		strdcat( &s, (char*)((i)?",":""), "parm", itoa(i), " = val", itoa(i), _ );

	h = var_from_str( 0, s );
	
	for( n = 0; ; n++ )
		{
		if ( ! (n%1000) ) print( ", ", itoa(n/1000), _ );
		var_to_js_array( h, &s );
		}
*/
/*
	h = var_from_str( 0, s );
	
	for( n = 0; ; n++ )
		{
		if ( ! (n%1000) ) print( ", ", itoa(n/1000), _ );
		var_save( h, "test.dat" );
		}
*/

/*
	h = var_from_str( 0, s );
	var_save( h, "test.dat" );
	var_free( h );
	
	for( n = 0; ; n++ )
		{
		if ( ! (n%1000) ) print( ", ", itoa(n/1000), _ );
		h = var_load( "test.dat" );
		var_free( h );
		}
*/

/*
	h = var_from_str( 0, s );
	
	for( n = 0; ; n++ )
		{
		if ( ! (n%1000) ) print( ", ", itoa(n/1000), _ );
		var_to_str( h, &s );
		}
*/	
/*	
	for( n = 0; ; n++ )
		{
		if ( ! (n%1000) ) print( ", ", itoa(n/1000), _ );
		h = var_from_str( 0, s );
		for( i = 0 ; i < 100 ; i++ )
			{
			strdcpy( &s, "parm", itoa(i), _ );
			var_delete( h, s );
			}
		var_free( h );
		}
*/	
/*
void 	var_delete( handle, char *pname );
handle	var_from_str( handle, char *in );
char *	var_to_str( handle, char **p_out );
handle	var_load( char *fname );
bool	var_save( handle, char *fname );
char *	var_to_js_array( handle, char **p_out );

char *	var_to_str_nq( handle, char **p_out );
void	var_print( handle, char *sep );

*/	
	wexit(0);
	}























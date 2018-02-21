#ifndef __ELIB_H__
#define __ELIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/file.h>


int		logw( char *msg, ... );


#ifdef MAIN
	#define EXTERN
	#define init_null  =null
	#define init_false =false
	#define init_error =-1
#else
	#define EXTERN extern
	#define init_null
	#define init_false
	#define init_error
#endif


EXTERN int argc;		// Vous devez initialiser ces 2 variables dans le main()
EXTERN char **argv;


#ifndef OS_AIX
typedef unsigned int	uint;
typedef unsigned short	ushort;
typedef unsigned char	uchar;
#else
#include <string.h>
#endif

#ifdef OS_WINDOWS
typedef long	 		handle;
#else
typedef long long 		handle;
#endif

typedef long long 				llong;
typedef unsigned long long 		ullong;

#define	bool			int
#define null			NULL
#define _				((char*)-1)
#define __				((char*)-2)
#define hNULL			((handle)NULL)

#define false			0
#define true			1

#define	Ko				*1024
#define Mo				Ko*1024
#define Go				Mo*1024

#define	Minute			*60
#define	Minutes			*60
#define	Heure			*60*60
#define	Heures			*60*60
#define	Jour			*60*60*24
#define	Jours			*60*60*24

#define alloc(n)		((char*)malloc( ( (n<256) ? 256 : n ) ))
#define alloc0(n)		((char*)calloc( 1, ( (n<256) ? 256 : n ) ))
#define realloc(m,n)	((char*)((n<256) ? m : realloc( m, n )))
#define Realloc(m,n)	((char*)((m)?realloc(m,n):alloc0(n)))

#ifdef OS_WINDOWS
	#include <windows.h>

	#define OS_NAME		"Windows"
	#define DIR_CHAR	"\\"
	#define DIR_CAR		'\\'
	#define PATH_CHAR	";"
	#define BR			"\r\n"
	#define EXE			".exe"

/*
	#ifndef MODE_CONSOLE
		#define system		elib_system		// N'ouvre pas de fenêtre dos (windows) et gestion des timeout
		bool	system( char *cmd );
	#endif
*/
#endif

#ifdef OS_AIX
	#define OS_UNIX
	#define OS_NAME "AIX"
#endif
#ifdef OS_SOLARIS
	#define OS_UNIX
	#define OS_NAME "Solaris"
#endif
#ifdef OS_LINUX
	#include <string.h>
	#define OS_UNIX
	#define OS_NAME "Linux"
	#define file_is_free_cmd "ssie7 fuser"
#endif

#ifdef OS_UNIX
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <signal.h>

	#define DIR_CHAR	"/"
	#define DIR_CAR		'/'
	#define PATH_CHAR	":"
	#define BR			"\n"
	#define EXE			""
	#define HANDLE		long long
#endif

#ifdef OS_WINDOWS
	#define sleep(t)	Sleep( t*1000 )	// 1 seconde = 1
	#define usleep(t)	Sleep( t/1000 )	// 1 seconde = 1000000
#else
	#define Sleep(t)	usleep( t*1000 ) // 1 seconde = 1000
#endif

//------------------------------------------------------------------------------
// ERREURS

#define OK			0
#undef  ERROR
#define ERROR	   ((int)-1)
#define sERROR	   NULL

#ifdef MAIN
	int 		 error_no = 0;
	char		*error_msg = null;
#else
	extern int 	 error_no;
	extern char	*error_msg;
#endif

void	error_set( int error_no, ... );

#ifdef TR

char *tr_dir = null;
char *tr_fname = null;
char *tr_w = null;
char bDebug = false;

#define tr_init(d) {\
					strdcpy(&tr_dir,d,_);mkdir(tr_dir);\
					strdcpy(&tr_w,tr_dir, "/trc",_);mkdir(tr_w);\
					strdcpy(&tr_w,"",_);\
					if (argc>1) strdcat(&tr_w,argv[1],_);\
					if (argc>2) strdcat(&tr_w," ",argv[2],_);\
					if (argc>3) strdcat(&tr_w," ",argv[3],_);\
					strd_replace( &tr_w, " ", "_" );\
					strd_replace( &tr_w, ".", "_" );\
					strd_replace( &tr_w, ":", "_" );\
					strd_replace( &tr_w, "/", "_" );\
					strd_replace( &tr_w, "\\", "_" );\
					strdcpy( &tr_fname, tr_dir, DIR_CHAR "trc" DIR_CHAR, itod(time(NULL),"flog"),\
							"_", itoa(getpid()), "_", tr_w, ".trc", _);\
					}
#define tr(s)	{\
				strdcpy( &tr_w, s, ", ", _ );\
				file_append(tr_fname, tr_w, -1);\
				strdcpy( &tr_w, tr_dir, DIR_CHAR "c0c1" DIR_CHAR, s, _ );\
				file_save_str( tr_w, "", 0 );\
				}

#define tr2(s1,s2)	{\
				strdcpy( &tr_w, s1, s2, ", ", _ );\
				file_append(tr_fname, tr_w, -1);\
				}

#define tr_reinit() {file_delete(tr_fname);tr_init();}

#ifdef TR_DEL
#define exit(rc) {file_delete(tr_fname); exit(rc);}
#else
#define exit(rc) {memfree(); exit(rc);}
#endif

#else

#define tr_init(d)
#define tr(s)
#define tr2(s1,s2)
#define exit(rc) {memfree(); exit(rc);}

#endif


//------------------------------------------------------------------------------
// CONSOLE

void	print( char *, ... );
void	print_ind( int ind, int maxlen, char *msg, ... );

#define flush()		fflush(stdout)
void	print_to_file( char *fname );
void	print_to_socket( handle hSock );
FILE *	print_get_fp( void );

#ifdef OS_WINDOWS
	#define cls()		system( "cls" )
#else
	#define cls()		system( "clear" )
#endif

void	cursor_pos( int col, int line );

#define pause()		{print(BR "Appuyez sur entrée pour continuer ...",_); getchar();}
#ifdef OS_WINDOWS
#define wpause()		{print(BR "Appuyez sur entrée pour continuer ...",_); getchar();}
#define wexit(n)		{print(BR "Appuyez sur entrée pour continuer ...",_); getchar(); exit(n);}
#else
#define wpause()
#define wexit(n)		exit(n)
#endif

int		kbget( void );
	/*
	Flèche haute	: 572
	Flèche bas		: 580
	Flèche gauche	: 575
	Flèche droite	: 577
	Home			: 571
	Inser			: 582
	Suppr			: 583
	Fin				: 579
	Pg up			: 573
	Pg down			: 581
	F1				: 1001
	F2				: 1002
	F3				: 1003
	F4				: 1004
	F5				: 1005
	F6				: 1006
	F7				: 1007
	F8				: 1008
	F9				: 1009
	F10				: 1010
	F11				: 633
	F12				: 634
	Ctrl A ... Z	: 1 ... 26
	*/

char * kbget_text( char **p_inout, char **p_values, int field_len, int max_len, int **exit_keys );
	// Non encore implémenté


//------------------------------------------------------------------------------
// STRING

#define nvl( v, def )	(( v ) ? v : def )

char* 	trim( char* );				// Agit directement sur la chaine en entrée
char* 	lower( char* );				// idem
char* 	upper( char* );				// idem

bool	is_numeric( char* );
bool	is_alphanum( char* in, char *comp );

#define atoi( s )	((s) ? atol(s) : 0 )	// Réécriture pour éviter certain plantage
#define itoa		elib_itoa		// Réécriture pour simplification de l'utilisation
char*	itoa( long long n );				// Transforme un nombre en décimal.
char*	itona( long long n, int d );		// Transforme un nombre en décimal.
char*	utoa( uint n );				// Transforme un nombre non signé en décimal
char*	utona( uint n, int d );		// Transforme un nombre non signé en décimal
char*	ito36( int n, int d );		// Transforme un nombre non signé en décimal
int		ifrom36( char *s36 );		// Transforme un nombre non signé en décimal
bool	is_base_36( char* s36 );
char*	btoa( char v );

char *	substr( char *in, int pos, int len );

#ifdef OS_LINUX
#undef strncpy
#undef strncmp
#endif
#define strncpy		elib_strncpy	// La fonction standard ne termine pas par un zéro
#define strncmp		elib_strncmp	// La fonction std ne retourne pas des résultats correctes (signe)
#define stricmp		elib_stricmp	// Inexistant sur unix
#define strnicmp	elib_strnicmp	// Inexistant sur unix
#define charicmp( c1, c2 )	(elib_upper_table[(uchar)c1] - elib_upper_table[(uchar)c2])

int		strncmp( char *, char *, int );			// La fonction standard ne retourne pas les bonnes valeurs
int		stricmp( char *s1, char *s2 );
int		strnicmp( char* s1, char* s2, int );	// Fonction inexistante sous unix, un paramètre optionnel pour indiquer la longeur. Si non indiqué, on prend la longueur de s2
bool	strpcmp( char* pattern, char* in );		// Pattern compare (  exemple *.txt )
bool	strmcmp( char *in, char *sList ); 		// sList = chaine1|chaine2|"toto titi"
bool	strmicmp( char *in, char *sList );

char *	strncpy( char *, char *, int );	// La fonction standard ne met pas de zéro à la fin
char *	strdcpy( char **out, ... );
char *	strdncpy( char **out, ... );
char *	str( char *, ... );
char *	strdcat( char **inout, ... );
char *	strdncat( char **inout, ... );

int		strpos( char* ref, char* to_search, int pstart );
int		stripos( char* ref, char* to_search, int pstart );
int		strrpos( char* ref, char* to_search, int pstart );
int		strripos( char* ref, char* to_search, int pstart );

bool	str_exists( char* ref, char* sList ); // sListe= chaine1|chaine2 & c4|c5 & ...
bool	str_iexists( char* ref, char* sList );

int		strcount( char* in, char *sSearch, int pstart );
int		stricount( char* in, char *sSearch, int pstart );

char *	str_replace( char* in,  char* sSearch, char *sReplace );
char *	str_ireplace( char *in, char* sSearch, char *sReplace );

char *	strd_replace( char** inout,  char* sSearch, char *sReplace );
char *	strd_ireplace( char **inout, char* sSearch, char *sReplace );

char **	str_explode( char *in, char *separator );	// ATTENTION : Limite de 2000 colonnes

char *	strins( char** inout, int p, char* sToInsert );
char *	strdel( char** inout, int p, int len );
char *	strpad( char* in, int p, char *sPad );
char *	strlpad( char* in, int p, char *sPad );

char *	str_quote( char *p_in );
char *	strd_quote( char **p_inout );
#define quote( s ) str_quote( s )
char *	str_C_quote( char *p_in );
char *	strd_C_quote( char **p_inout );
#define C_quote( s ) str_C_quote( s )
char *	str_unquote( char *p_in );
char *	strd_unquote( char **p_inout );
#define unquote( s ) str_unquote( s )

char *	str_to_utf8( char **inout );
#define to_utf8( s ) str_to_utf8( &s )
char *	str_from_utf8( char **inout );
#define from_utf8( s ) str_from_utf8( &s )

handle	text_init( char *txt, char *keys );
char *	text_getword( char **out, handle );
#define getword		text_getword
int		text_getlineno( handle );
int		text_getpos( handle );
int		text_setpos( handle, int pos, int lineno );
int		text_getwpos( handle );
void	text_free( handle );

char* 	sgets( char **out, char* in, long int *pstart );

char* 	grep( char **out, char* in, char * search );
char* 	grepi( char **out, char* in, char * search );
char* 	grepv( char **out, char* in, char * search );
char* 	grepvi( char **out, char* in, char * search );
#define grepiv grepiv

char *toBase64( char **p_out, char *in, int l );
int   fromBase64( char **p_out, char *in );

//------------------------------------------------------------------------------
// BIGINT

handle	bigint_new( void );
handle	bigint_add( handle, uint value );
void	bigint_from10( handle, char * );
char *	bigint_to10( handle );
char*	bigint_print( handle );

int		rnd( int min, int max );
char *	srnd( int nDigit );
char *	snrnd( int nDigit );


//------------------------------------------------------------------------------
// DATES

#ifdef OS_AIX
	#undef  itod
#endif


#define sysdate			time(NULL)
#define currdate( f )	itod( sysdate, f )

char*	itod( long int timestamp, char* format );	// Transforme un timestamp en chaine date
	// format par defaut : dd/mm/yyyy hh:mm:ss
	// autres formats : aaaa/mm/jj hh:mm:ss
	// 	(Fr) WD jj mmm aaaa    Pays par défaut = France, (En) = English, Autres pays à implémenter plus tard

double tt( char * date_begin, char *date_end, char* format );   // Calcule la diff entre 2 dates (en secondes) 

int		dtoi( char *d, char* format );	// Transforme un timestamp en chaine date

int		year( long int t );			// si t = 0 => on prend la date systeme
int		month( long int t );
int		day( long int t );
int		wday( long int t );			// 0=Lundi
int		lday( long int t );			// Dernier jour du mois de cette date (28, 29, 30 ou 31)
int		fwday( long int t, int w );	// Retourne la date du premier Lundi, mardi, ... du mois
int		lwday( long int t, int w );	// Retourne la date du dernier Lundi, mardi, ... du mois
int		hday( long int t );			// Retourne l'heure

bool	est_ferie( char *d );
int		planif_compute( char *planif, int last_exec );


//------------------------------------------------------------------------------
// TABLEAU DE VARIABLE

handle	var_new( void );
void	var_free( handle );
handle	var_dup( handle hVar );
bool 	var_is_updated( handle hVar );
char *	var_get( handle, char *vname );
char *	var_getc( handle, char *vname, char **out );
char **	var_geta( handle, char *vname );
char * 	var_getn( handle, int vnum, char **vname );
char **	var_getna( handle, int vnum, char **vname );
void	var_set( handle, char *pname, char *value );
void 	var_delete( handle, char *pname );
handle	var_load( char *fname );
handle	var_reload( handle, char *fname );
bool	var_save( handle, char *fname );
handle	var_from_str( handle, char *in );
handle	var_from_cmd( handle, char *in );
char *	var_to_str( handle, char **p_out );
char *	var_to_str_nq( handle, char **p_out );
char *	var_to_js_array( handle, char **p_out );
void	var_print( handle, char *sep );
char *	var_sprint( handle hVar, char *sep, char **p_out );
void	var_tprint( handle hVar, char *sep, int pad );
char *	var_list_replace( handle hVar, char **inout );
char *	var_replace( handle hVar, char *in, char **out );

//------------------------------------------------------------------------------
// FICHIERS

#ifdef OS_WINDOWS
char * fname_verify( char *fname );
#else
#define fname_verify( fname ) fname
#endif

bool		file_exists( char* fname );
bool		dir_exists( char* fname );
long int	file_count( char* fname );

long long	file_size( char* fname );	// renvoi la taille du fichier en nb d'octets
char*		file_getdir( char* fname ); // renvoi le chemin sans le nom du fichier
char*		file_get_fullpath( char* fname );
char*		file_getname( char* fname ); // renvoi le nom du fichier sans le chemin
char*		file_getext( char* fname ); // retourne l'extension du fichier

EXTERN char *dirtmp	init_null;
char*		file_tmpname( char **p_out ); // retourne un nom de fichier temporaire unique

bool		file_is_used( char* fname ); // Retourne true si le fichier est ouvert en écriture par un autre process

#define	fopen		elib_fopen		// Prise en compte des noms avec / ou anti slash
#define	fwrite		elib_fwrite		// fwrite( buf, n1, n2, fp ) --> fwrite( fp, buf1, len1, buf1,len2, ... )
#define	fread		elib_fread		// fread( buf, n1, n2, fp ) --> fread( fp, buf, len )

FILE *	fopen( char *fname, char *mode );
int		fread( FILE *fp, char *data, int size );
long	fwrite( FILE *fp, ... ); // fwrite( fp, n, s1, l1, s2, l2, ..., sn, ln )
bool	fprint( FILE *fp, ... ); // fprint( fp, n, s1, s2, ..., sn )

#ifdef OS_WINDOWS
#define fseek		fseeko64
#define ftell		ftello64
#endif

#define	fseek_top( fp, pos ) fseek( fp, pos, SEEK_SET )
#define	fseek_cur( fp, pos ) fseek( fp, pos, SEEK_CUR )
#define	fseek_end( fp, pos ) fseek( fp, pos, SEEK_END )

#ifdef OS_WINDOWS
#define read		elib_read
#define write		elib_write
#define seek_top	elib_seek_top
#define close		elib_close

#else

#define seek_top	elib_seek_top

#endif

HANDLE	file_lock( char *fname );
bool	file_unlock( HANDLE );
int		get_uid( char *fname );
#define getuid get_uid
int		read_uid( char *fname );


char*	file_load( char **out, char *fname );
int		file_save_str( char *fname, char *in, int l );

typedef struct
	{
	char name[201];
	char dir[201];
	char sdir[201];
	unsigned long long size;	// Non implementé
	bool is_dir;
	bool is_updatable;			// Non implémenté
	}
	T_FILE_INFO;

T_FILE_INFO *file_list( T_FILE_INFO * t_file, char* fname, char *options );
	// fname peut contenir des jokers
	// fname = NULL -> fname = "*"
	// fname = /rep/nomfic.txt
	// fname = /rep/*.txt
	// fname = /rep/*.txt *.c *.h -*.o 		le - signifie que l'on exclu ces fichiers
	// si t_file==NULL ==> Une structure T_FILE_INFO est initialisée
	// retourne NULL si plus de fichiers trouvé et la mémoire t_file est récupérée
	// ne retourne pas . et ..
	// options : nodir, nofile, recursif, nohide, absdir
	// is_dir est toujours calculé
	// size pour un répertoire est toujours 0 (non implémenté)

void	file_list_free( T_FILE_INFO * t_file );
bool	file_delete( char *fname );
int		file_copy( char *fname, char * fdest );
int		file_move( char *fname, char * fdest );
int		file_rename( char *fname, char * fnewname );

char *	file_cat( char **p_out, char *fname, char * grep, char *grepv );
char *	file_icat( char **p_out, char *fname, char * grep, char *grepv );

int		autoextract_add_files( char* autoextract_fname, char *fname, char *dirdest );
int		autoextract_extract( char* autoextract_fname, char *filter, char *dirdest );


#define	mkdir		elib_mkdir		// Construit tous les répertoires intermédiaires
int		mkdir( char *dname );



//------------------------------------------------------------------------------
// PROCESS

char **	memptr( int p_size );
char *	memget( int p_size );
	// retourne un pointeur sur de la mémoire à utiliser rapidement (ne jamais faire de free)
	
void	memll( char *m );
	// transforme une mémoire rapide (tmp_memget) en mémoire à durée de vie longue (il faudra faire le free)

char *	System( char **out, char *cmd ); // Idem system, mais récupère la sortie standard dans out
#ifdef OS_UNIX
#define SystemH		System
#else
char *	SystemH( char ** p_out, char *cmd );
#endif

int		Start( char *cmd );		// Démarre un process parallèle. Retourne le pid
int		StartH( char *cmd ); 	// Pas de création de fenêtre ni d'affichage à l'écran ni de lien à un terminal

int		script_exec( char *cmd, handle hVar, int timeout, char **p_out );

int		process_count( char * filter );
bool	process_is_alive( int pid );
void	process_kill( int pid );
void	process_killc( int pid ); // Kill le process et les enfants
void	process_kill_all( char * filter ); // Ne kill pas le process courant

/*
char *	Script( char *result, char *cmd );
handle	process_list( char *out, handle, char * filter );
int		process_getid( char * filter ); // Retourne le premier pid trouvé
int		process_waitend( int pid, uint timeout );
*/

char *	sysuser();



#ifdef OS_WINDOWS

EXTERN CRITICAL_SECTION elib_cs;
EXTERN int elib_cs_init  init_false;


#define BeginCriticalSection()	\
	if ( elib_cs_init == false )\
		{\
		InitializeCriticalSection(&elib_cs);\
		elib_cs_init = true;\
		}\
    EnterCriticalSection(&elib_cs)

#define EndCriticalSection()	LeaveCriticalSection(&elib_cs)

#else	// OS_UNIX

#define BeginCriticalSection()
#define EndCriticalSection()

#endif



//------------------------------------------------------------------------------
// LOG HIERARCHIQUE

handle	hlog_init( char *logdir, char* hlog_id );
			// => retourne un hlog
			// Crée le fichier s'il n'existe pas avec un premier élément log_id=0

int		hlog_free( handle hlog );
//#define	hlog_free( hBD ) bd_close( hBD )

char *	hlog_getid( handle hlog );
char *	hlog_getdir( handle hlog );

int		hlogc( handle hlog, int parent_id, char *status, char *log, ... );
int		hlogu( handle hlog, int log_id, char *status, char *log, ... );
//int		hlogu_date( handle hlog, int log_id, int dt );
int		hlogu_status( handle hlog, int log_id, char *status, ... );
int		hlog_raise_status( handle hlog, int log_id, char *status, ... );
		// Met à jour le status de log_id et remonte ce status avec le préfixe aux log méres
		// jusqu'à la racine

int		hlogr( handle hlog, int log_id, char **p_status, char ** p_log );
/*
int		logc_nb( int log_id );
		// Retourne le nombre d'enfants
*/

int		hlogc_stat( handle hlog, int log_id, /* p_status, int * nb, */ ... );
	// Compte le nb d'enfant dont le status commence par p_status
	// Permet de déterminer le status de la log mère
	// Retourne le nombre d'enfant

char *	hlogc_last_status( handle hlog, int log_id );
	// Retourne le status du dernier enfant;


char *	hlog_export( char **p_out, char *logdir, char* hlog_id, int format );
char *	hlog_index( char **p_out, char *logdir, char *p_date,
					char *status, char *filter, int max_item, int format );

int		hlog_purge( char *logdir, int delay );



//------------------------------------------------------------------------------
// TCP

char *	tcp_myIP( uint nOrder );
bool	is_ip_valide( char *ip );
bool	is_free_port( char *port );
char *	tcp_dns_address( char * dns );

handle 	tcp_connect( char * ipport );
void 	tcp_disconnect( handle hCnx );

int 	tcp_send( handle hCnx, char *in, int len );
int		tcp_recv( handle hCnx, char **out, int maxlen );
int		tcp_nrecv( handle hCnx, char **out, int len );

int 	tcp_ssend( handle hCnx, char *in, int len );
int		tcp_srecv( handle hCnx, char **out );

int 	tcp_fsend( handle hCnx, char *fname );
int		tcp_frecv( handle hCnx, char *fname, int len );

char * 	tcp_request( char *ipport, char *cmd, char **out );
char * 	tcp_frequest( char *ipport, char *cmd, char *fname );

handle 	tcp_server_start( char *port );
handle 	tcp_cnx_wait( handle hSrv, char *ip_cli );


//------------------------------------------------------------------------------
// SSL

char *	ssl_pgm( char *openssl_fname );

#define ssl_conf_load( hVar, certif, certif_lib ) \
	ssl_conf_load2( hVar, certif, certif_lib, "ssl" )

int		ssl_conf_load2( handle hVar, char *certif, char *certif_lib, char *var_prefix );
	// Si ssl_private_key, n'existe pas => la fonction génère une clef privée et un csr dans ssl_csr
	// Si ssl_certif_server n'existe pas => Essai de le charger avec le paramètre certif
		// crée variable ssl_certif_server, ssl_certif_server_label, ssl_certif_server_end_date, ssl_certif_server_ca
	// Si ssl_certif_ca1 n'existe pas => Essai de le charger avec le paramètre certif
		// crée variable ssl_certif_ca1, ssl_certif_ca1_label, ssl_certif_ca1_end_date, ssl_certif_ca1_ca
	// Si ssl_certif_caN n'existe pas => Essai de le charger avec le paramètre certif
		// crée variable ssl_certif_caN, ssl_certif_caN_label, ssl_certif_caN_end_date, ssl_certif_caN_ca
	// Si ssl_certif_caN_label = ssl_certif_caN_ca, c'est un certificat root
	
	// certif, peut-être un PEM, une clef privée, une chaine de certification,
	//    un nom de fichier pem ou der, un répertoire


int		ssl_certif_load( handle hVar, char *certif, char *certif_lib, char *var_prefix );

int 	ssl_end_delay( handle hVar, char *var_prefix ); // Retourne le nombre de secondes avant expiration de l'un des certificats

char *	ssl_encrypt( char **p_out, handle hVar, char *in, char *var_prefix );
char *	ssl_decrypt( char **p_out, handle hVar, char *in, char *var_prefix );

char *	ssl_encrypt_file( handle hPrivate, handle hPublic, char *src_fname, char *res_fname, char *var_prefix );
char *	ssl_decrypt_file( handle hPrivate, handle hPublic, char *src_fname, char *res_fname, char *var_prefix );


char *	crypte64( char**, char *, int );
int		uncrypte64( char**, char * );




//------------------------------------------------------------------------------
// BD : Big Data

handle	bd_open( char *p_fname );
handle	bd_hopen( handle hVar );
int		bd_close( handle hBD );
int		bd_insert( handle hBD, handle hData, char **p_out );
int		bd_update( handle hBD, handle hData, char *id_list, uint id_nb );
int		bd_delete( handle hBD, char *id_list, uint id_nb );
int		bd_select( handle hBD, char *Cond, char *OrderBy, char **id_list );
int		bd_load( handle hBD, char *id_list, uint id_nb, char *of, char **p_out );
int		bd_clean( handle hBD );



//------------------------------------------------------------------------------
// BDOC : Base documentaire
// bdoc_id, type=dir|doc, class, parent_id,
// info : dt_crea, dt_upd, parent_id, status, class, children_nb,
//			title, msg, body, body_type=Code|Mémo|Html|Doc|..., position,
//			keywords, authors, rights
// index : parent_id/status/day_upd
// index_key : class,dt_upd,keywords,position (pour tri),authors
// base_descr = description de la base class=base, parent_id=[null]
// class_descr = description des classes class=class, parent_id=[null], title={class_name}
// class_item = class=class, parent_id={class_id}, title= ...

/*------------------------------------------------------------------------------
  INDEXATION DES DROITS :
  ------------------------------------------------------------------------------
  
{dirdat}/xx/xx/{login}.rights

fichiers comme le fichier des données.
La position des données est selon les 2 derniers caractères de l'IDA.
100 caractères pour la liste des roles. On raccourcie 'read' => 'r', 'create' => 'c', ...

Mise à jour de l'indexe par la fonction bdoc_xindex()
*/

/*------------------------------------------------------------------------------
  THESAURUS :
  ------------------------------------------------------------------------------

$diridx/kwords/{word} : Liste des id

Mise à jour de l'indexe par la fonction bdoc_xindex()
*/


handle	bdoc_open( handle hCfg );
#define	bdoc_close( hBD ) bd_close( hBD )

int		bdoc_save( handle hBD, char *user_id, handle hData );
int		bdoc_load( handle hBD, char *id_list, uint id_nb, char *of, char **p_out );

int		bdoc_set_pos( handle hBD, char *user_id, char **id_list, int new_pos );
int		bdoc_move( handle hBD, char *user_id, char *id_list, char *new_parent_id, int pos );
int		bdoc_delete( handle hBD, char *user_id, char *id_list, uint id_nb );

int		bdoc_select( handle hBD, char *user_id, char *Cond, char *OrderBy, char **id_list );

int		bdoc_xindex( handle hBD ); // Met à jour les indexs thésaurus et droits

#endif













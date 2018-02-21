
#define STATUS_MAXLEN		4095	//200
#define LOG_MAXLEN			4095	//500

typedef struct
	{
	int parent_id;
	int first_child_id;
	int last_child_id;
	int next_id;
	char when[21];
	char status[STATUS_MAXLEN + 1];
	char log[LOG_MAXLEN + 1];
	}
	t_elib_log;


typedef struct
	{
	t_elib_log elib_log;
	char *hlog_id;
	char *hlog_dir;
	char *hlog_fname;
	}
	t_elib_log_wrk;

#define elib_log 		(((t_elib_log_wrk*)hlog)->elib_log)
#define hlog_fname		(((t_elib_log_wrk*)hlog)->hlog_fname)
#define hlog_dir		(((t_elib_log_wrk*)hlog)->hlog_dir)
#define hlog_id	 		(((t_elib_log_wrk*)hlog)->hlog_id)


//------------------------------------------------------------------------------
handle 	hlog_init( char *logdir, char *p_hlog_id )
//------------------------------------------------------------------------------
	{
	handle hlog;
	HANDLE fLock;
	#ifdef OS_UNIX
	static char *cmd = null;
	#endif

	tr( "hlog_init1" );
//print( "hlog_init1" BR, _ );
	mkdir( logdir );
	if ( ! dir_exists( logdir ) )
		{
//		logw( "ERREUR : hlog_init 1 : ", logdir, _ );
		return ERROR;
		}
	
	tr( "hlog_init2" );
//print( "hlog_init2" BR, _ );
	hlog = (handle)alloc0( sizeof( t_elib_log_wrk ) );
	
	strdcpy( &hlog_dir, logdir, _ );
	
	if ( p_hlog_id )
		{
//		strdcpy( &hlog_fname, logdir, DIR_CHAR, p_hlog_id, DIR_CHAR, p_hlog_id, ".log", _ );
		if ( strlen( p_hlog_id ) > 3 )
			strdncpy( &hlog_fname, logdir, -1, DIR_CHAR, 1,
					p_hlog_id, 3, DIR_CHAR, 1, &(p_hlog_id[3]), -1,
					DIR_CHAR, 1, p_hlog_id, -1, ".log", 4, _ );
		else
			strdcpy( &hlog_fname, logdir, DIR_CHAR,
					p_hlog_id, DIR_CHAR, p_hlog_id, ".log", _ );
		
		tr2( "hlog_init3:", hlog_fname );
		if ( !file_exists( hlog_fname ) )
			{
			tr( "hlog_init3a" );
//print( "hlog_init3a:", hlog_fname, BR, _ );
//			logw( "ERREUR : hlog_init 3 : fichier n'existe pas : ", hlog_fname, _ );
			hlog_free( hlog );
			return ERROR;
			}
			
		strdcpy( &hlog_id, p_hlog_id, _ );
		return hlog;
		}
	

	// OBTENTION D'UN HLOG_ID
	
	tr( "hlog_init4" );
	strdcpy( &hlog_id, itoa( getuid( str( logdir, DIR_CHAR "log.id", _ ) ) ), _ );
//print( "hlog_init:hlog_id=", hlog_id, BR, _ );
	
	if ( strlen( hlog_id ) > 3 )
		strdncpy( &hlog_fname, logdir, -1, DIR_CHAR, 1, hlog_id, 3,
					DIR_CHAR, 1, &(hlog_id[3]), -1,  _ );
	else
		strdcpy( &hlog_fname, logdir, DIR_CHAR, hlog_id, _ );
	mkdir( hlog_fname );
	
	strdcat( &hlog_fname, DIR_CHAR, hlog_id, ".log", _ );
	
	
	// VERROUILLE LE FICHIER
	
	tr2( "hlog_init5:", hlog_fname );
//print( "hlog_init5" BR, _ );
	
	fLock = file_lock( hlog_fname );
	if ( fLock == (HANDLE)-1 )
		{
//		logw( "ERREUR : hlog_init : pb ouverture du fichier : ", hlog_fname, _ );
		return ERROR;
		}
	
	
	tr( "hlog_init6" );
	// INITIALISE LA LOG
	
	elib_log.parent_id = elib_log.first_child_id = elib_log.last_child_id = elib_log.next_id = 0;
	elib_log.when[0] = 0;
	elib_log.status[0] = 0;
	elib_log.log[0] = 0;
	
	// ENREGISTRE ET DEVEROUILLE
	
	tr( "hlog_init7" );
	
	write( fLock, &elib_log, sizeof( elib_log ) );
	file_unlock( fLock );

	#ifdef OS_UNIX
	strdcpy( &cmd, "chmod 700 ", hlog_fname, _ );
	system( cmd );
	#endif

	return hlog;
	}




//------------------------------------------------------------------------------
int		hlog_free( handle hlog )
//------------------------------------------------------------------------------
	{
	if ( hlog_id )		free( hlog_id );
	if ( hlog_fname )	free( hlog_fname );
	if ( hlog_dir )		free( hlog_dir );
	free((t_elib_log_wrk*)hlog) ;
	}
	


//------------------------------------------------------------------------------
char *	hlog_getid( handle hlog )
//------------------------------------------------------------------------------
	{
	return hlog_id;
	}
	
	
	
//------------------------------------------------------------------------------
char *	hlog_getdir( handle hlog )
//------------------------------------------------------------------------------
	{
	return file_getdir(hlog_fname);
	}
	
	
	
//------------------------------------------------------------------------------
int		hlogu( handle hlog, int log_id, char *status, char *log, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	FILE *fp;
	char *arg, *s = null, *w =null;
	int n = 0, elib_log_nb;
	HANDLE fLock;

	tr( "hlogu1" );
//if ( !log_id )
//logw( "hlogu:", hlog_id, ":", status, _ );

	// LECTURE DE LA TAILLE DU FICHIER LOG
	
	elib_log_nb = file_size( hlog_fname );
	if ( elib_log_nb % sizeof( elib_log ) )
		{
		// Le fichier fourni n'est pas un fichier LOG valide.
		return false;
		}

	elib_log_nb /= sizeof( elib_log );
	if ( log_id >= elib_log_nb )
		{
		return false;
		}

	// VERROUILLE LE FICHIER LOG

	fLock = file_lock( hlog_fname );
	
	tr2( "hlogu2:", hlog_fname);
	
	// CHARGE LA LOG

	seek_top( fLock, log_id * sizeof( elib_log ) );
	read( fLock, (char*)(&elib_log), sizeof( elib_log ) );
	
	tr( "hlogu3" );


	// PREPARE LA LOG
	
	strcpy( elib_log.when, currdate("log") );
	if ( status )	strncpy( elib_log.status, status, STATUS_MAXLEN );

	tr( "hlogu4" );
	
	if (( log )&&( log != _ )&&( log != __ ))
		{
		strdcpy( &s, log, _ );
		
		va_start(args, log); 
		for(;;)
			{
			arg = va_arg(args, char *);
			if ( arg == _ ) break;
			if ( arg == __ ) break;
			strdcat( &s, arg, _ );
			}
	
		if ( (n = strlen( s )) <= LOG_MAXLEN )
			strcpy( elib_log.log, s );
		else
			{
			elib_log.log[0] = '@';
			elib_log.log[1] = 0;
			strcat( elib_log.log, hlog_fname );
			strcat( elib_log.log, "." );
			strcat( elib_log.log, itoa(log_id) );
			strcat( elib_log.log, ".dat" );
			}
		}
	
	// ECRIT LA LOG

	tr( "hlogu5" );
	
	seek_top( fLock, log_id * sizeof( elib_log ) );
	write( fLock, &elib_log, sizeof( elib_log ) );
	
	file_unlock( fLock );

	tr( "hlogu6" );

	// ECRIT LA LOG DANS UN FICHIER EXTERNE SI ELLE EST TROP GRANDE

	if ( n > LOG_MAXLEN )
		{
		tr( "hlogu7" );
		fp = fopen( &(elib_log.log[1]), "wb" );
		if ( fp == NULL )	return false;
		fprint( fp, *s, _ );
		fclose( fp );
		}
	
	tr( "hlogu8" );

	if ( log_id ) goto fin;
	
	// ECRIT FICHIER INDEX
	
	tr( "hlogu9" );

	strdcpy( &s, hlog_dir, DIR_CHAR, hlog_id, ".log.", _ );

	n = strlen( s );
	
	strdcat( &s, "*", _ );
	file_delete( s );
	
	s[n] = 0;

	strdncat( &s, elib_log.when, 4,
				&((elib_log.when)[5]), 2,
				&((elib_log.when)[8]), 2, // Nouvelle date
				 _ );
				 

	if ( !strnicmp( elib_log.status, "OK", 2 ) )
		strdcat( &s, ".ENDOK", _ );
	else
	if (( !strnicmp( elib_log.status, "ERREUR", 6 ) )
		||( !strnicmp( elib_log.status, "ERROR",  5 ) ))
		strdcat( &s, ".ENDERR", _ );
	else
	if ( !strnicmp( elib_log.status, "ALERT", 5 ) )
		strdcat( &s, ".ENDERR", _ );
	else
	if ( !strnicmp( elib_log.status, "ANNULE",  6 ) )
		strdcat( &s, ".ENDERR", _ );
	else
		strdcat( &s, ".RUN", _ );

	fp = fopen( s, "wb" );
	if ( fp ) fclose( fp );

	fin:
		
	if ( s ) free( s );
	if ( w ) free( w );

	return log_id;
	}



//------------------------------------------------------------------------------
int		hlogu_date( handle hlog, int log_id, int dt )
//------------------------------------------------------------------------------
	{
	HANDLE fLock;
	int	elib_log_nb, n;
	char *s = null, *w = null;
	FILE *fp;

	
	// LECTURE DE LA TAILLE DU FICHIER LOG
	
	elib_log_nb = file_size( hlog_fname );
	if ( elib_log_nb % sizeof( elib_log ) )
		// Le fichier fourni n'est pas un fichier LOG valide.
		return false;

	elib_log_nb /= sizeof( elib_log );
	
	if ( log_id >= elib_log_nb )
		return false;

	
	// VERROUILLE LE FICHIER
	
	fLock = file_lock( hlog_fname );
	
	
	// CHARGE LA LOG

	seek_top( fLock, log_id * sizeof( elib_log ) );
	read( fLock, (char*)(&elib_log), sizeof( elib_log ) );
	

	if ( dt > 0 )
		{
/*
		if ( elib_log.when[0] )
			{
			strdncpy( &w, elib_log.when, 4,
						&((elib_log.when)[5]), 2,
						&((elib_log.when)[8]), 2, // Ancienne date
						 _ );
						 
			if ( !strnicmp( elib_log.status, "OK", 2 ) )
				strdcat( &w, ".ENDOK", _ );
			else
			if (( !strnicmp( elib_log.status, "ERREUR", 6 ) )
				||( !strnicmp( elib_log.status, "ERROR",  5 ) ))
				strdcat( &w, ".ENDERR", _ );
			else
			if ( !strnicmp( elib_log.status, "ALERT", 5 ) )
				strdcat( &w, ".ENDERR", _ );
			else
			if ( !strnicmp( elib_log.status, "ANNULE",  6 ) )
				strdcat( &w, ".ENDERR", _ );
			else
				strdcat( &w, ".RUN", _ );
			}
*/		
		// CHANGE LA DATE
		
		strcpy( elib_log.when, itod( dt, "log") );
		
		
		// ECRIT LA LOG
		
		seek_top( fLock, log_id * sizeof( elib_log ) );
		write( fLock, &elib_log, sizeof( elib_log ) );
		}
	
	file_unlock( fLock );
	
	if ( log_id ) goto fin;
	
	// ECRIT FICHIER INDEX
	
	tr( "hlogu9" );

	strdcpy( &s, hlog_dir, DIR_CHAR, hlog_id, ".log.*", _ );
	file_delete( s );
	
	s[strlen(s)-1] = 0;
	strdncat( &s, elib_log.when, 4,
				&((elib_log.when)[5]), 2,
				&((elib_log.when)[8]), 2, // Nouvelle date
				_ );
				 

	if ( !strnicmp( elib_log.status, "OK", 2 ) )
		strdcat( &s, ".ENDOK", _ );
	else
	if (( !strnicmp( elib_log.status, "ERREUR", 6 ) )
		||( !strnicmp( elib_log.status, "ERROR",  5 ) ))
		strdcat( &s, ".ENDERR", _ );
	else
	if ( !strnicmp( elib_log.status, "ALERT", 5 ) )
		strdcat( &s, ".ENDERR", _ );
	else
	if ( !strnicmp( elib_log.status, "ANNULE",  6 ) )
		strdcat( &s, ".ENDERR", _ );
	else
		strdcat( &s, ".RUN", _ );

	fp = fopen( s, "wb" );
	if ( fp ) fclose( fp );

	fin:
		
	if ( s ) free( s );
	if ( w ) free( w );

	return true;
	}


//bool bDebug = false;

//------------------------------------------------------------------------------
int		hlogu_status( handle hlog, int log_id, char *status, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg;
	char *s = null /*, *w = null*/;
	HANDLE fLock;
	int	elib_log_nb, n;
	FILE *fp;


//if ( bDebug ) logw( "hlogu_status debug1", _ );
	
	// LECTURE DE LA TAILLE DU FICHIER LOG
	
	elib_log_nb = file_size( hlog_fname );
	if ( elib_log_nb % sizeof( elib_log ) )
		{
		// Le fichier fourni n'est pas un fichier LOG valide.
		return false;
		}
	elib_log_nb /= sizeof( elib_log );
	
	if ( log_id >= elib_log_nb )
		{
		return false;
		}

//if ( bDebug ) logw( "hlogu_status debug2", _ );

	
	// VERROUILLE LE FICHIER
	
	fLock = file_lock( hlog_fname );
	
	
	// CHARGE LA LOG

	seek_top( fLock, log_id * sizeof( elib_log ) );
	if ( read( fLock, (char*)(&elib_log), sizeof( elib_log ) ) == ERROR )
		{//logw( "hlogu_status read ERROR", _ );
		}
//if ( bDebug ) logw( "mft_send_part : OK", _ );


	// PREPARE LA LOG
	
	strcpy( elib_log.when, currdate("log") );
//if ( bDebug ) logw( "hlogu_status debug3a", _ );
	if ( status )
		{
		strdcpy( &s, status, _ );
		va_start(args, status); 
		for(;;)
			{
			arg = va_arg(args, char *);
			if ( arg == _ ) break;
			if ( arg == __ ) break;
			strdcat( &s, arg, _ );
			}
		strncpy( elib_log.status, s, STATUS_MAXLEN );
		}
	
	// ECRIT LA LOG
//if ( bDebug ) logw( "hlogu_status debug4", _ );
	
	seek_top( fLock, log_id * sizeof( elib_log ) );
	write( fLock, &elib_log, sizeof( elib_log ) );

	file_unlock( fLock );

//if ( bDebug ) logw( "hlogu_status debug fin", _ );
	
	if ( s ) free( s );
//	free( w );

	return true;
	}



//------------------------------------------------------------------------------
int		hlogc( handle hlog, int mere_id, char *status, char *log, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	FILE *fp;
	char *arg, *s = null;
	int n, logc_id, id;
	int	elib_log_nb;
	HANDLE fLock;

	if ( hlog_id == null )
		{
		print( currdate("log"), " : ", _ );

		if (( log )&&( log != _ )&&( log != __ ))
			{
			print( log, _ );
			va_start(args, log); 
			for(;;)
				{
				arg = va_arg(args, char *);
				if ( arg == _ ) break;
				if ( arg == __ ) break;

				print( arg, _ );
				}

			print( " : ", status, BR, _ );
			}
		else
			print( status, BR, _ );
		
		return true;
		}
	

	// LECTURE DE LA TAILLE DU FICHIER LOG
	
	for( n = 0 ; ; Sleep(1), n++ )
		{
		elib_log_nb = file_size( hlog_fname );
		if ( elib_log_nb % sizeof( elib_log ) )
			{
			// Le fichier fourni n'est pas un fichier LOG valide.
			if ( n < 10000 )		continue;
//logw( "hlogc error1 : hlog_id=", hlog_id, _ );
			return false;
			}
		break;
		}


	elib_log_nb /= sizeof( elib_log );
	
	if ( mere_id >= elib_log_nb )
		{
//logw( "hlogc error2 : hlog_id=", hlog_id, _ );
		return false;
		}

tr2("hlogc1:", hlog_fname);	
	// OUVRE ET VERROUILLE LE FICHIER

	if ( ( fLock = file_lock( hlog_fname ) ) == (HANDLE)(-1) )
{
//logw( "hlogc error3 : hlog_id=", hlog_id, _ );
tr2("hlogc1a", hlog_fname );	
		return false;
}
	
tr("hlogc2");	

	
	// OBTENTION D'UN LOG_ID
	
	logc_id = elib_log_nb;

			
	// PREPARE LA LOG
	
	elib_log.parent_id = mere_id;
	elib_log.first_child_id = elib_log.last_child_id = elib_log.next_id = 0;
	elib_log.when[0] = 0;
	elib_log.status[0] = 0;
	elib_log.log[0] = 0;

tr("hlogc3");	
	strcpy( elib_log.when, currdate("log") );
	if ( status )	strncpy( elib_log.status, status, STATUS_MAXLEN );

	if (( log )&&( log != _ )&&( log != __ ))
		{
tr("hlogc4");
		strdcpy( &s, log, _ );
		
		va_start(args, log); 
		for(;;)
			{
			arg = va_arg(args, char *);
			if ( arg == _ ) break;
			if ( arg == __ ) break;
			strdcat( &s, arg, _ );
			}
tr("hlogc5");	
		if ( (n = strlen( s )) <= LOG_MAXLEN )
			strcpy( elib_log.log, s );
		else
			{
tr("hlogc6");
			elib_log.log[0] = '@';
			elib_log.log[1] = 0;
			strcat( elib_log.log, hlog_fname );
			strcat( elib_log.log, "." );
			strcat( elib_log.log, itoa(logc_id) );
			strcat( elib_log.log, ".dat" );
			}
		}
	
	// ECRIT LA LOG
tr("hlogc7");
	
	seek_top( fLock, logc_id * sizeof( elib_log ) );
tr("hlogc7a");
	write( fLock, &elib_log, sizeof( elib_log ) );

tr("hlogc7b");


	// ECRIT LA LOG DANS UN FICHIER EXTERNE SI ELLE EST TROP GRANDE
	
	if ( n > LOG_MAXLEN )
		{
tr("hlogc7c");
		fp = fopen( &(elib_log.log[1]), "wb" );
		if ( fp == NULL )
			{
//			close( fLock );
			file_unlock( fLock );
			free( s );
//logw( "hlogc error4 : hlog_id=", hlog_id, _ );
			return false;
			}
		fprint( fp, s, _ );
		fclose( fp );
		}

tr("hlogc7d");

	if ( s ) free( s );
	
tr( "hlogc8" );
	
	// MET A JOUR LA MERE
	
	seek_top( fLock, mere_id * sizeof( elib_log ) );
	read( fLock, (char*)(&(elib_log)), sizeof( elib_log ) );

tr( "hlogc8a" );
	
	if ( elib_log.first_child_id == 0 ) elib_log.first_child_id = logc_id;
	id = elib_log.last_child_id;
	elib_log.last_child_id = logc_id;

tr( "hlogc8b" );
	
	seek_top( fLock, mere_id * sizeof( elib_log ) );
	write( fLock, &elib_log, sizeof( elib_log ) );
	
tr( "hlogc9" );
	
	// MET A JOUR LE PRECEDENT S'IL EXISTE

	if ( id )
		{
tr( "hlogc9a" );
		seek_top( fLock, id * sizeof( elib_log ) );
		read( fLock, (char*)(&elib_log), sizeof( elib_log ) );
		
		elib_log.next_id = logc_id;
		
		seek_top( fLock, id * sizeof( elib_log ) );
		write( fLock, &elib_log, sizeof( elib_log ) );
		}

tr( "hlogc10" );

//	close( fLock );
	file_unlock( fLock );
tr( "hlogc11" );
	
	return logc_id;
	}



//------------------------------------------------------------------------------
int	hlog_raise_status( handle hlog, int log_id, char *status, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg, *s = null, *v, *w = null;
	HANDLE fLock;
	int	elib_log_nb, n;
	FILE *fp;


tr( "hlog_raise_status1" );
	
	// LECTURE DE LA TAILLE DU FICHIER LOG
	
	elib_log_nb = file_size( hlog_fname );
	if ( elib_log_nb % sizeof( elib_log ) )
		{
		// Le fichier fourni n'est pas un fichier LOG valide.
		return false;
		}
	elib_log_nb /= sizeof( elib_log );
	
tr( "hlog_raise_status2" );

	// OUVRE ET VERROUILLE LE FICHIER
	
	fLock = file_lock( hlog_fname );
	
	
	// PREPARATION DU STATUS
	
	if ( status )
		{
tr( "hlog_raise_status3" );
		strdcpy( &s, status, _ );
		va_start(args, status); 
		for(;;)
			{
			arg = va_arg(args, char *);
			if ( arg == _ ) break;
			if ( arg == __ ) break;
			strdcat( &s, arg, _ );
			}
		}
	
tr( "hlog_raise_status4" );
	
	// MISE A JOUR DES STATUS JUSQU'A LA RACINE
	
	v = currdate("log");
	
	for( ; log_id ; log_id = elib_log.parent_id )
		{
tr( "hlog_raise_status5" );
		seek_top( fLock, log_id * sizeof( elib_log ) ),
		read( fLock, (char*)(&elib_log), sizeof( elib_log ) );
/*
		if (( !w )&&( elib_log.when[0] ))
			{
			strdncpy( &w, elib_log.when, 4,
						&((elib_log.when)[5]), 2,
						&((elib_log.when)[8]), 2, // Ancienne date
						 _ );
						 
			if ( !strnicmp( elib_log.status, "OK", 2 ) )
				strdcat( &w, ".ENDOK", _ );
			else
			if (( !strnicmp( elib_log.status, "ERREUR", 6 ) )
				||( !strnicmp( elib_log.status, "ERROR",  5 ) ))
				strdcat( &w, ".ENDERR", _ );
			else
			if ( !strnicmp( elib_log.status, "ALERT", 5 ) )
				strdcat( &w, ".ENDERR", _ );
			else
			if ( !strnicmp( elib_log.status, "ANNULE",  6 ) )
				strdcat( &w, ".ENDERR", _ );
			else
				strdcat( &w, ".RUN", _ );
			}
*/		
		strcpy( elib_log.when, v );
		if ( s )	strncpy( elib_log.status, s, STATUS_MAXLEN );
		else elib_log.status[0] = 0;
		
		seek_top( fLock, log_id * sizeof( elib_log ) ),
		write( fLock, (char*)(&elib_log), sizeof( elib_log ) );
		}

tr( "hlog_raise_status6" );
	
	file_unlock( fLock );
	
	if ( s ) free( s );
	if ( w ) free( w );

	return true;
	}
	
	
	
//------------------------------------------------------------------------------
int		hlogr( handle hlog, int log_id, char **p_status, char ** p_log )
//------------------------------------------------------------------------------
	{
	FILE *fp;
	int n;
	int	elib_log_nb;
	
	if ( ( !p_status )&&( !p_log ) ) return false;

	// VERIFIE LA VALIDITE DU LOG_ID
	
	elib_log_nb = file_size( hlog_fname );
	if ( elib_log_nb % sizeof( elib_log ) )
		return false;

	elib_log_nb /= sizeof( elib_log );
	
	if ( log_id >= elib_log_nb )
		return false;


	// OUVRE LE FICHIER

	for( n = 100 ; ( ( fp = fopen( hlog_fname, "rb" ) ) == NULL )&&( n ) ; n--, Sleep(5) );
	if (!n) return false;

	// LECTURE DE LA LOG
	
	fseek_top( fp, log_id * sizeof( elib_log ) ),
	fread( fp, (char*)(&elib_log), sizeof( elib_log ) );

	if ( p_status )
		strdcpy( p_status, elib_log.status, _ );
	
	if ( p_log )
		{
		if (( elib_log.log[0] != '@' )||( !file_exists( &(elib_log.log[1]) ) ))
			strdcpy( p_log, elib_log.log, _ );
		else
			file_load( p_log, &(elib_log.log[1]) );
		}
	
	
	// FIN
	fclose( fp );
	return true;
	}
	
	
//------------------------------------------------------------------------------
int		hlogc_stat( handle hlog, int log_id, ... )
//------------------------------------------------------------------------------
	{
	char *p_status;
	FILE *fp;
	long int n, *cpt;
	va_list args;
	int	elib_log_nb;
	
	// INITIALISE LES COMPTEURS
	
	va_start(args, log_id); 
	for(;;)
		{
		p_status = va_arg(args, char *);
		if ( p_status == _ ) break;
		if ( p_status == __ ) break;
		cpt = va_arg(args, long int *);
		*cpt = 0;
		}


	
	// VERIFIE LA VALIDITE DU LOG_ID
	
	elib_log_nb = file_size( hlog_fname );
	if ( elib_log_nb % sizeof( elib_log ) )
		return false;


	elib_log_nb /= sizeof( elib_log );

	
	if ( log_id >= elib_log_nb )
		return false;


	// OUVRE LE FICHIER

	for( n = 100 ; ( ( fp = fopen( hlog_fname, "rb" ) ) == NULL )&&( n ) ; n--, Sleep(5) );
	if (!n) return false;


	// LECTURE DE LA LOG
	
	fseek_top( fp, log_id * sizeof( elib_log ) );
	if ( fread( fp, (char*)(&elib_log), sizeof( elib_log ) ) != sizeof( elib_log ) )
		return false;;


	// BOUCLE SUR LES ENFANTS
	
	log_id = elib_log.first_child_id;
	for( n = 0 ; log_id ; n++ )
		{
		// LECTURE DE LA LOG ENFANT
		
		fseek_top( fp, log_id * sizeof( elib_log ) );
		if ( fread( fp, (char*)(&elib_log), sizeof( elib_log ) ) != sizeof( elib_log ) )
			break;
		
		
		// CALCUL LES STAT
		
		va_start(args, log_id); 
		for(;;)
			{
			p_status = va_arg(args, char *);
			if ( p_status == _ ) break;
			if ( p_status == __ ) break;
			cpt = va_arg(args, long int *);
			if ( strnicmp( elib_log.status, p_status, -1 ) ) continue;
			(*cpt)++;
			}
		
		
		// ENFANT SUIVANT
		log_id = elib_log.next_id;
		}
	
	// FIN
	fclose( fp );
	return n;
	}


//------------------------------------------------------------------------------
char *	hlogc_last_status( handle hlog, int log_id )
//------------------------------------------------------------------------------
	{
	char **last_alert=memptr(0), **last_status=memptr(0);
	FILE *fp;
	int n, *cpt;
	va_list args;
	int	elib_log_nb;
	
	

	// VERIFIE LA VALIDITE DU LOG_ID
	
	elib_log_nb = file_size( hlog_fname );
	if ( elib_log_nb % sizeof( elib_log ) )
		return null;

	elib_log_nb /= sizeof( elib_log );

	if ( log_id >= elib_log_nb )
		return null;

	// OUVRE LE FICHIER

	for( n = 100 ; ( ( fp = fopen( hlog_fname, "rb" ) ) == NULL )&&( n ) ; n--, Sleep(5) );
	if (!n) return null;

	// LECTURE DE LA LOG
	
	fseek_top( fp, log_id * sizeof( elib_log ) );
	if ( fread( fp, (char*)(&elib_log), sizeof( elib_log ) ) != sizeof( elib_log ) )
		{
		fclose( fp );
		return "null1";
		}


	// PARCOURT LES ENFANTS POUR RECHERCHE D'ALERTE
	strdcpy( last_alert, "", _ );

	for( log_id = elib_log.first_child_id ; log_id ; log_id = elib_log.next_id )
		{
		fseek_top( fp, (log_id) * sizeof( elib_log ) );
		if ( fread( fp, (char*)(&elib_log), sizeof( elib_log ) ) != sizeof( elib_log ) )
			{
			fclose( fp );
			return "null2";
			}

		if ( ! (elib_log.status[0]) ) continue;
		if ( ( strnicmp( elib_log.status, "OK", 2 ) )
			&&( strnicmp( elib_log.status, "ERREUR", 6 ) )
			&&( strnicmp( elib_log.status, "ERROR", 5 ) )
			&&( strnicmp( elib_log.status, "ALERTE", 6 ) )
			&&( strnicmp( elib_log.status, "ALERT", 5 ) )
			)
			continue;
			
		strdcpy( last_status, elib_log.status, _ );
			
		if ( ! strnicmp( *last_status, "ALERT", -1 ) )
			strdcpy( last_alert, *last_status, _ );
		}

	// FIN
	fclose( fp );
	
	
	if ( ( *last_alert )&&( *last_alert[0] )&&( ! strnicmp( *last_status, "OK", -1 ) ) )
		return *last_alert;
	
	return *last_status;
	}



	
//------------------------------------------------------------------------------
void _hlog_export( char **p_out, FILE *fp, int log_id, int level, int format )
//------------------------------------------------------------------------------
	{
	char *ind = "                                                                 ";
	char *s = null;
	int id;
	t_elib_log wlog;
	
	for( ; ; )
		{
		fseek_top( fp, log_id * sizeof( wlog ) );
		fread( fp, (char*)(&wlog), sizeof( wlog ) );

//print( "hlog_export : child:", itoa(wlog.first_child_id), ", log = ", wlog.log, BR, _ );
	
		if ( ! format )
			{
			strdcat( p_out, substr( ind, 0, level*3 ), itoa(log_id), " - ", wlog.when, " : ", wlog.status, " : ", _ );
			if (( wlog.log[0] != '@' )||( !file_exists( &(wlog.log[1]) ) ))
				strdcat( p_out, wlog.log, BR, _ );
			else
//{
//print( "hlog_export : loadfile ", &(wlog.log[1]), BR, _ );
				strdcat( p_out, file_load( &s, &(wlog.log[1]) ), BR, _ );
//}
			}
		else
			{
			strdcat( p_out, "[", itoa(log_id), ",", str_quote(wlog.when), ",", str_quote(wlog.status),	",", _ );
			if (( wlog.log[0] != '@' )||( !file_exists( &(wlog.log[1]) ) ))
				strdcat( p_out, str_quote(wlog.log), ",[", _ );
			else
//{
//print( "hlog_export : loadfile ", &(wlog.log[1]), BR, _ );
				strdcat( p_out, str_quote(file_load( &s, &(wlog.log[1]) )), ",[", _ );
//}
			}
			
		id = wlog.next_id;
			
		if ( wlog.first_child_id )
			_hlog_export( p_out, fp, wlog.first_child_id, level+1, format );
			
		if ( format )
			strdcat( p_out, "]]", _ );
			
		if ( !id ) return;
		log_id = id;
		
		if ( format )
			strdcat( p_out, ",", _ );
		}
	}



//------------------------------------------------------------------------------
char *	hlog_export( char **p_out, char *logdir, char *log_id, int format )
//------------------------------------------------------------------------------
	{
	char *log_fname = null;
	FILE *fp;
	int n;
	
	if ( p_out == null ) return null;

//	strdcpy( &log_fname, logdir, "/", log_id, "/", log_id, ".log", _ );
	if ( strlen( log_id ) > 3 )
		strdncpy( &log_fname, logdir, -1, DIR_CHAR, 1,
				log_id, 3, DIR_CHAR, 1, &(log_id[3]), -1,
				DIR_CHAR, 1, log_id, -1, ".log", 4, _ );
	else
		strdcpy( &log_fname, logdir, DIR_CHAR,
				log_id, DIR_CHAR, log_id, ".log", _ );
	if ( !file_exists( log_fname ) )
		goto hlog_export_fin_vide;
	
	n = file_size( log_fname );
	if ( n % sizeof( t_elib_log ) )
		goto hlog_export_fin_vide;
	
	for( n = 1000 ; ( ( fp = fopen( log_fname, "rb" ) ) == NULL )&&( n ) ; n--, Sleep(5) );
	if (!n) goto hlog_export_fin_vide;
	
	strdcpy( p_out, "", _ );
	
	_hlog_export( p_out, fp, 0, 0, format );
	fclose( fp );
	
	free( log_fname );
	return *p_out;
	
	hlog_export_fin_vide:
	
	free( log_fname );
	return strdcpy( p_out, "[]", _ );
	}


	
//------------------------------------------------------------------------------
char *	hlog_index( char **p_out, char *logdir, char *p_date,
					char *p_status, char *p_filter, int max_item, int format )
//------------------------------------------------------------------------------
	{
	char *fname = null, *v, *w = null, *w2 = null;
	int log_id, b = false, n = 0, n1 = 0, p;
	FILE *fp = NULL;
	t_elib_log wlog;
	T_FILE_INFO *tf;
	
tr( "hlog_index1" );

	if ( ! p_out ) return null;

	if ( format != 1 )
		strdcpy( p_out, "", _ );
	else
		strdcpy( p_out, "[", _ );

tr( "hlog_index2" );
		
	// SI STATUS EST NUMERIQUE : C'EST UN LOG_ID => ON RETOURNE LE TITRE DE LA LOG DEMANDEE
	
	if ( is_numeric( p_status ) )
		{
tr( "hlog_index3" );
//		strdcpy( &fname, logdir, DIR_CHAR, p_status, DIR_CHAR, p_status, ".log", _ );
		if ( strlen( p_status ) > 3 )
			strdncpy( &fname, logdir, -1, DIR_CHAR, 1,
					p_status, 3, DIR_CHAR, 1, &(p_status[3]), -1,
					DIR_CHAR, 1, p_status, -1, ".log", 4, _ );
		else
			strdcpy( &fname, logdir, DIR_CHAR,
					p_status, DIR_CHAR, p_status, ".log", _ );

		for( ; ; Sleep(1) )
			{
			fp = fopen( fname, "rb" );
			if ( fp ) break;
			if ( !file_exists( fname ) ) break;
			}
tr( "hlog_index4" );
		if ( !fp ) return strdcat( p_out, (char*)(( format ) ? "]" : "" ), _ );
		
tr( "hlog_index5" );
		if ( fread( fp, (char*)(&wlog), sizeof( wlog ) ) != sizeof( wlog ) )
			{
			fclose( fp );
			return strdcat( p_out, (char*)(( format ) ? "]" : "" ), _ );
			}
		
tr( "hlog_index6" );
		if ( format != 1 )
			strdcat( p_out, p_status, " - ", wlog.when, " : ", wlog.status, " : ", wlog.log, BR, _ );
		else
			{
tr( "hlog_index7" );
			if ( b ) strdcat( p_out, ",", _ ); else b = true;
			strdcat( p_out, "[", p_status, ",", str_quote(wlog.when), ",",
					str_quote(wlog.status), ",", str_quote(wlog.log), "]]", _ );
			}

tr( "hlog_index8" );
		if ( fp ) { fclose( fp ); fp = NULL; }
		free( fname );

		return *p_out;
		}
	
	
	// LECTURE DE LA LOG LA PLUS RECENTE VERS LA PLUS ANCIENNE

tr( "hlog_index9" );

	trim( strdcpy( &w, p_date, _ ) );
	if ( w[0] == 0 ) { w[0] = '*', w[1] = 0; }
	else strd_replace( &w, "/", "" );

	if (( !p_status )||( p_status[0] == 0 )) { p_status = "*"; }

	strdcpy( &fname, logdir, DIR_CHAR "*.log.", w, ".", p_status, _ );
	tf = NULL;
tr2( "hlog_index10:", fname );
	while( tf = file_list( tf, fname, "nodir" ) )
		{
		// CHARGE LA LOG
		
		v = itoa(atoi( tf->name ));
		if ( strlen( v ) > 3 )
			strdncpy( &fname, logdir, -1, DIR_CHAR, 1, v, 3,
								DIR_CHAR, 1, &(v[3]), -1,
								DIR_CHAR, 1, v, -1, ".log", 4, _ );
		else
			strdcpy( &fname, logdir, DIR_CHAR, v, DIR_CHAR, v, ".log", _ );
		log_id = atoi( v );

tr2( "hlog_index11:", fname );

		for( p = 100 ; p ; p-- )
			{
			fp = fopen( fname, "rb" );
			if ( fp ) break;
			if ( !file_exists( fname ) ) break;
			Sleep(1);
			}
tr( "hlog_index12" );
		if ( !fp ) continue;
		
tr( "hlog_index13" );
		if ( fread( fp, (char*)(&wlog), sizeof( wlog ) ) != sizeof( wlog ) )
			{
			fclose( fp );
			continue;
			}
		fclose( fp );
		
		// FILTRAGE
		
/*
		if ( ( p_date )&&( !str_iexists( wlog.when, p_date ) ) )
			{
			if ( strncmp( wlog.when, p_date, -1 ) < 0 )
				{
				n1++;
				if ( n1 > 5000 )	break;
				}
			continue;
			}
*/
tr( "hlog_index14" );
//		if ( ( p_status )&&( !str_iexists( wlog.status, p_status ) ) )	continue;
		if ( ( p_filter )&&( p_filter[0] )
			&&( !str_iexists( wlog.status, p_filter ) )
			&&( !str_iexists( wlog.log, p_filter ) )
			)	continue;
		
		
tr( "hlog_index15" );
		// LOG RETENUE
		if ( !format )
			strdcat( p_out, itoa(log_id), " - ", wlog.when, " : ", wlog.status, " : ", wlog.log, BR, _ );
		else
		if ( format == 2 )
			{ // Affichage format mersi v3
tr( "hlog_index16" );
			for( p = 0 ; wlog.log[p] ; p++ )
				{
				if ( wlog.log[p] == '/' )	{ wlog.log[p] = ' '; continue; }
				if ( wlog.log[p] == '@' )	{ wlog.log[p] = ':'; continue; }
				}
			p = strpos( wlog.log, ":", 0 );
			strdcat( p_out, itoa(log_id), " : ", &(wlog.log[p+1]), " ", wlog.status, " ", wlog.when, BR, _ );
			}
		else
			{
tr( "hlog_index17" );
			if ( b ) strdcat( p_out, ",", _ ); else b = true;
			strdcat( p_out, "[", itoa(log_id), ",", str_quote(wlog.when), ",",
					str_quote(wlog.status), ",", str_quote(wlog.log), "]", _ );
			}
tr( "hlog_index18" );
			

		n++;
		if (( max_item > 0 )&&( n >= max_item ))
			{
tr( "hlog_index19" );
			if ( format != 1 )
				strdcat( p_out, ".....", BR, _ );
			else
				{
				if ( b ) strdcat( p_out, ",", _ ); else b = true;
				strdcat( p_out, "[0,'.....','.....','Veuillez affiner votre selection']", _ );
				}
			break;
			}
		}
	
	// FIN DE LECTURE DES LOG
tr( "hlog_index20" );
	
//	if ( fp ) { fclose( fp ); fp = NULL; }
	if ( format == 1 )
		strdcat( p_out, "]", _ );
tr( "hlog_index21" );

	free( w );
	free( fname );
	
tr( "hlog_index22" );

	return *p_out;
	}



//------------------------------------------------------------------------------
	
#undef elib_log
#undef hlog_fname
#undef hlog_id

	




















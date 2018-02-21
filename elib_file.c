
//------------------------------------------------------------------------------

#ifdef OS_WINDOWS
char * fname_verify( char *fname )
	{
	int i, l = strlen(fname);
	char **out = memptr(0);

	if ( strpos( fname, "/", 0 ) < 0 ) return fname;
	if ( strnicmp( fname, "/tmp/", -1 ) )
		{	
		strdcpy( out, fname, _ );
		for( i = 0 ; i < l ; i++ )
			if ( (*out)[i] == '/' )
				(*out)[i] = '\\';
		}
	else
		{
		strdcpy( out, getenv( "TEMP" ), "\\", &(fname[5]), _ );
		for( i = 0 ; (*out)[i] ; i++ )
			if ( (*out)[i] == '/' )
				(*out)[i] = '\\';
		}

	return *out;
	}

int elib_read( HANDLE fd, void *buf, int t )
	{
	DWORD n = 0;
	ReadFile( fd, buf, t, &n, NULL );
	return n;
	}
	
int elib_write( HANDLE fd, void *buf, int t )
	{
	DWORD n = 0;
	WriteFile( fd, buf, t, &n, NULL );
	return n;
	}
	
int elib_seek_top( HANDLE fd, int offset )
	{
	SetFilePointer( fd, offset, NULL, FILE_BEGIN );
	}
	
int elib_close( HANDLE fd )
	{
	CloseHandle( fd );
	}
	
#else

int elib_seek_top( HANDLE fd, int offset )
	{
	lseek( fd, offset, 0 );
	}

#endif
	
	
//------------------------------------------------------------------------------
HANDLE	file_lock( char *p_fname )
//------------------------------------------------------------------------------
	{
	HANDLE fp;
	char b = false;
	int result;
	
	#ifdef OS_WINDOWS
	
	char *fname;
	
	fname = fname_verify( p_fname );

	for( result = 1000 ;
		( result )&&
		(
		( fp = CreateFile( fname, GENERIC_READ | GENERIC_WRITE,
				 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) )
		== INVALID_HANDLE_VALUE
		)
		; Sleep(10), result-- )
		{
		if ( ! b )
			{
			if ( !dir_exists( file_getdir( fname ) ) )
				mkdir(file_getdir( fname ));
			b = true;
			}
		}

	if ( !result ) return (HANDLE)(-1);

	return fp;
	
	
	#else // OS_UNIX
	
	struct flock fl;
	
	memset(&fl, 0, sizeof fl);
	
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;
	fl.l_pid = getpid();
	
	for( result=1000 ; (result)&&( fp = open( p_fname, O_CREAT | O_RDWR ) ) == -1 ; Sleep(10), result-- )
		{
		if ( ! b )
			{
			if ( !dir_exists( file_getdir( p_fname ) ) )
				mkdir(file_getdir( p_fname ));
			b = true;
			}
		}
	if ( !result ) return -1;
	
	result = fcntl(fp, F_SETLKW, &fl);
	
	#endif

	return fp;

	}


//------------------------------------------------------------------------------
HANDLE	file_lock0( char *p_fname )
//------------------------------------------------------------------------------
	{
	
	#ifdef OS_WINDOWS
	
	HANDLE fp;
	char *fname;
	
	fname = fname_verify( p_fname );

	if ( ( fp = CreateFile( fname, GENERIC_READ | GENERIC_WRITE,
			 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) )
		== INVALID_HANDLE_VALUE ) return false;

	return fp;
	
	#endif
/*	
	#ifdef OS_LINUX
	
	HANDLE fp;
	struct flock fl;
	int result;
	
	memset(&fl, 0, sizeof fl);
	
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;
	fl.l_pid = getpid();
	
	if ( ( fp = open( p_fname, O_CREAT | O_RDWR ) ) < 0 ) return false;
	
	
	if ( flock( fp, LOCK_EX | LOCK_NB ) < 0 )
		{
		close( fp ); return false;
		}

	return fp;
	#endif
*/
	
//	#ifdef OS_AIX
	#ifdef OS_UNIX
	
	HANDLE fp0;
	char *fp = null;
	static char *s = null, *s1 = null;
	int p;
	
	strdcpy( &s1, p_fname, "0", _ );
	fp0 = file_lock( s1 );
	if ( file_load( &s, p_fname ) != sERROR )
		{
		if (( p=atoi(s) )&&( process_is_alive( p ) ))
			{
			file_unlock( fp0 );
			unlink( s1 );
			return false;
			}
		unlink( p_fname );
		}
	file_save_str( p_fname, itoa(getpid()), -1 );
	strdcpy( &fp, p_fname, _ );
	file_unlock( fp0 );
	unlink( s1 );
	return (HANDLE)fp;
	
	#endif
	}


//------------------------------------------------------------------------------
bool	file_is_lockable( char *p_fname )
//------------------------------------------------------------------------------
	{
	HANDLE fp;
	
	#ifdef OS_WINDOWS
	
	char *fname;
	
	fname = fname_verify( p_fname );

	if ( ( fp = CreateFile( fname, GENERIC_READ | GENERIC_WRITE,
			 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) )
		== INVALID_HANDLE_VALUE ) return false;

	CloseHandle( fp );
	
	#else // OS_UNIX
	
	struct flock fl;
	int result;
	
	memset(&fl, 0, sizeof fl);
	
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;
	fl.l_pid = getpid();
	
	if ( ( fp = open( p_fname, O_CREAT | O_RDWR ) ) < 0 ) return false;

	result = fcntl(fp, F_SETLKW, &fl);
	close( fp );
	
	#endif

	return true;

	}


//------------------------------------------------------------------------------
bool	file_unlock( HANDLE fp )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	
	if ( fp != INVALID_HANDLE_VALUE )
		{
		CloseHandle( fp );
		}
		
	#else
	
	if ( fp >= 0 )
		{
		close( fp );
		}
	
	#endif
		
	}



//------------------------------------------------------------------------------
bool	file_unlock0( HANDLE fp )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	
	if ( fp != INVALID_HANDLE_VALUE )
		{
		CloseHandle( fp );
		}
		
	#else
	
	unlink( (char*)fp );
	free( (char*)fp );
	
	#endif
		
	}



//------------------------------------------------------------------------------
int		get_uid( char *p_fname )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS

	DWORD n;
	int uid = 0;
	HANDLE fp;
	char *fname, b = false;
	
	fname = fname_verify( p_fname );
	for( ;
		( fp = CreateFile( fname, GENERIC_READ | GENERIC_WRITE,
			 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) )
		== INVALID_HANDLE_VALUE ; Sleep(1) )
		{
		if ( ! b )
			{
			if ( !dir_exists( file_getdir( fname ) ) )
				mkdir(file_getdir( fname ));
			b = true;
			}
		}

	ReadFile( fp, &uid, sizeof( uid ), &n, NULL );
	uid++;
	SetFilePointer( fp, 0, NULL, FILE_BEGIN );
	WriteFile( fp, &uid, sizeof( uid ), &n, NULL );
	CloseHandle( fp );
	
	return uid;
	
	
	
	#else // OS_UNIX

	int uid = 0;
	int fp;
	char *cmd = null;

	fp = file_lock( p_fname );
	read( fp, &uid, sizeof( uid ) );
	uid++;
	lseek( fp, 0, 0 );
	write( fp, &uid, sizeof( uid ) );
	close( fp );
	if ( uid == 1 )
		{
		strdcpy( &cmd, "chmod 600 ", p_fname, _ );
		system( cmd );
		free( cmd );
		}
	
	return uid;
	#endif
	}



//------------------------------------------------------------------------------
int		read_uid( char *p_fname )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS

	DWORD n;
	int uid = 0;
	HANDLE fp;
	char *fname;
	
	fname = fname_verify( p_fname );
	if ( !file_exists( fname ) ) return ERROR;
	for( ;
		( fp = CreateFile( fname, GENERIC_READ | GENERIC_WRITE,
			 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) )
		== INVALID_HANDLE_VALUE ; Sleep(1) );

	ReadFile( fp, &uid, sizeof( uid ), &n, NULL );
	CloseHandle( fp );
	
	return uid;
	
	
	
	#else // OS_UNIX

	int uid = 0;
	int fp;
	char *cmd = null;

	if ( !file_exists( p_fname ) ) return ERROR;
	fp = file_lock( p_fname );
	read( fp, &uid, sizeof( uid ) );
	close( fp );
	return uid;
	#endif
		
	}


//------------------------------------------------------------------------------
int		set_uid( char *p_fname, int uid )
//------------------------------------------------------------------------------
	{
	FILE *fp;
	
	fp = fopen( p_fname, "wb" );
	if ( fp == NULL ) return ERROR;
	fwrite( fp, &uid, sizeof(uid), _ );
	fclose( fp );
	return OK;
	}


//------------------------------------------------------------------------------
int		file_wpid( char *p_fname )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS

	DWORD n;
	HANDLE fp;
	char *fname, *v;
	
	fname = fname_verify( p_fname );
	for( ;
		( fp = CreateFile( fname, GENERIC_READ | GENERIC_WRITE,
			 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) )
		== INVALID_HANDLE_VALUE ; Sleep(1) );

	WriteFile( fp, v=itoa(getpid()), strlen(v), &n, NULL );
	CloseHandle( fp );
	
	return true;
	
	
	
	#else // OS_UNIX
	int fp;
	char *v;

	fp = file_lock( p_fname );
	
	write( fp, v = itoa( getpid() ), strlen( v ) );
	close( fp );
	
	return true;

	#endif
		
	}


	
	
//------------------------------------------------------------------------------
int w2u( char* fin, char *fout )
//------------------------------------------------------------------------------
	{
	FILE *fp1, *fp2;
	char buf[16];
	int n;
	bool b = false;
	
	fp1 = fopen( fin, "rb" );
	if ( fp1 == NULL )
		{
		error_set( 1, "Le fichier ", fin, " n'existe pas ou n'est pas accessible en lecture", _ );
		return ERROR;
		}
	
	if ( !fout ) return ERROR;
		
	fp2 = fopen( fout, "wb" );
	if ( fp2 == NULL )
		{
		error_set( 1, "Problème de création du fichier ", fout, _ );
		return ERROR;
		}
	
	while( ( n = fread( fp1, buf, 1 ) ) == 1 )
		{
		if ( buf[0] != '\r' )
			fwrite( fp2, buf, 1, _ );
		}
		
	fclose( fp1 );
	fclose( fp2 );
	if ( b )
		{
		#ifdef OS_WINDOWS
		system( str( "move /Y ", fout, " ", fin, _ ) );
		#else
		system( str( "mv -f ", fout, " ", fin, _ ) );
		#endif
		}
	return OK;
	}
		
//------------------------------------------------------------------------------
int u2w( char* fin, char *fout )
//------------------------------------------------------------------------------
	{
	FILE *fp1, *fp2;
	char buf[16], *s = null;
	int n;
	bool b = false;
	
	fp1 = fopen( fin, "rb" );
	if ( fp1 == NULL )
		{
		error_set( 1, "Le fichier ", fin, " n'existe pas ou n'est pas accessible en lecture", _ );
		return ERROR;
		}
	
	if ( !fout ) return ERROR;
	fp2 = fopen( fout, "wb" );
	if ( fp2 == NULL )
		{
		error_set( 1, "Problème de création du fichier ", fout, _ );
		return ERROR;
		}
	
	while( ( n = fread( fp1, buf, 1 ) ) == 1 )
		{
		if ( buf[0] == '\n' )
			fwrite( fp2, "\r", 1, _ );
		fwrite( fp2, buf, 1, _ );
		}
		
	fclose( fp1 );
	fclose( fp2 );
	if ( b )
		{
		#ifdef OS_WINDOWS
		system( str( "move /Y ", fout, " ", fin, _ ) );
		#else
		system( str( "mv -f ", fout, " ", fin, _ ) );
		#endif
		}
	return OK;
	}
		
//------------------------------------------------------------------------------
bool	file_is_used( char* fname )
//------------------------------------------------------------------------------
	{
	char *r;
	
	r = System( null, str( "lsof ", fname, " | awk '{print $4}' | grep w | grep -v cwd", _ ) );
	trim( r );
	if ( ( r )&&( r[0] ) ) return true;

	r = System( null, str( "lsof ", fname, " | awk '{print $4}' | grep u", _ ) );
	trim( r );
	if ( ( r )&&( r[0] ) ) return true;
	
	return false;
	}
	
//------------------------------------------------------------------------------
bool file_exists( char* fname )
//------------------------------------------------------------------------------
	{
	FILE *fp;
	struct stat buffer;  
	
	
	if ( fp = fopen( fname, "rb" ) )
		{
		fclose( fp );
		return true;
		}
	if ( stat(fname_verify(fname),&buffer) == 0 ) return true;

	return false;
	}
	
//------------------------------------------------------------------------------
bool dir_exists( char* dname )
//------------------------------------------------------------------------------
	{
	DIR *rep;

	rep = opendir( fname_verify( dname) );
	if ( rep == NULL ) return false;
	closedir( rep );
	return true;
	}
	
//------------------------------------------------------------------------------
bool dir_file_exists( char* dname )
//------------------------------------------------------------------------------
	{
	DIR *rep;
	struct dirent *fic;
	char b = 0;

	rep = opendir( fname_verify( dname) );
	if ( rep == NULL ) return false;
	for( ; ; )
		{
		fic = readdir( rep );
		if ( fic == NULL ) break;
		if ( fic->d_name[0] == '.' ) continue;
		b++;
		break;
		}
	closedir( rep );
	
	return b;
	}
	
//------------------------------------------------------------------------------
long int	file_count( char* fname )
//------------------------------------------------------------------------------
	{
	long int n = 0;
	T_FILE_INFO *fi = NULL;
	
	while( fi = file_list( fi, fname, "nodir nohide" ) ) n++;

	return n;
	}
	
//------------------------------------------------------------------------------
long long	file_size( char* fname )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	FILE *fp;
	llong ll;
	fp = fopen( fname, "rb" );
	if ( !fp ) return ERROR;
	fseek_end( fp, 0 );
	ll = ftell( fp );
	fclose( fp );
	return ll;
	
	#else
	
	struct stat buffer;  

	if ( stat(fname_verify(fname),&buffer) == 0 ) return buffer.st_size;
	return 0;
	
	#endif
	}

	
//------------------------------------------------------------------------------
long int	file_date( char* fname )
//------------------------------------------------------------------------------
	{
	struct stat buffer;  

	if ( stat(fname,&buffer) == 0 ) return buffer.st_mtime;
	return 0;
	}


//------------------------------------------------------------------------------
int	file_purge( char* p_dir, int delay /*en jour*/ )
//------------------------------------------------------------------------------
	{
	int t;
	T_FILE_INFO *tf = NULL;
	char *fname = null;
	
	t = time(NULL) - ( delay * (60*60*24) );
	
	while( tf = file_list( tf, p_dir, "nodir absdir" ) )
		{
		strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
		if ( file_date( fname ) >= t ) continue;
		if ( ! strcmp( &(fname[strlen(fname)-3]), ".id" ) ) continue;
		unlink( fname );
		}
	if ( fname ) free( fname );
	}


//------------------------------------------------------------------------------
char* file_getdir( char* fname )
//------------------------------------------------------------------------------
	{
	char *out = memget(250);
	long int i = strlen(fname) - 1;
	
	while( ( i >= 0 )&&( fname[i] != '\\' )&&( fname[i] != '/' ) )
		i--;

	if ( i < 0 )
		getcwd(out, 200);
	else
		strncpy( out, fname, i );
	
	return out;
	}
	
//------------------------------------------------------------------------------
char* file_get_fullpath( char* fname )
//------------------------------------------------------------------------------
	{
	char *out = memget(250), *odir = memget(250);
	char *path, *name;
	long int i = strlen(fname) - 1;
	
	if ( dir_exists( fname ) )
		{
		getcwd( odir, 200 );
		chdir( fname );
		getcwd( out, 200 );
		chdir( odir );
//		free( odir );
		}
	else
		{
		path = file_getdir( fname );
		name = file_getname( fname );
		getcwd( odir, 200 );
		chdir( path );
		getcwd( out, 200 );
		chdir( odir );
		strcat( out, "/" );
		strcat( out, name );
		}
		
	#ifdef OS_UNIX
	return out;
	#else
	return lower(out);
	#endif
	}
	
//------------------------------------------------------------------------------
char* file_getname( char* fname )
//------------------------------------------------------------------------------
	{
	int i = strlen(fname) - 1;
	char *out = memget(250);
	
	while( ( i >= 0 )&&( fname[i] != '\\' )&&( fname[i] != '/' ) )
		i--;
		
	strcpy( out, &(fname[i+1]) );

	i = strlen( out );
	if ( ( i )&&( out[i-1] == '"' ) )
		out[i-1] = 0;
	
	return out;
	}


//------------------------------------------------------------------------------
char* file_getext( char* fname )
//------------------------------------------------------------------------------
	{
	int i = strlen(fname) - 1;
	char *out = memget(250);
	
	while( ( i >= 0 )&&( fname[i] != '.' ) )
		i--;
	
	if ( i < 0 )
		out[0] = 0;
	else	
		strcpy( out, &(fname[i+1]) );
	
	return out;
	}

//------------------------------------------------------------------------------
char* file_tmpname( char **p_out )
//------------------------------------------------------------------------------
	{
	#ifdef OS_UNIX
	
	return strdcpy( p_out, tempnam(dirtmp, "ftmp"), _ );
	
	#else
	*p_out = Realloc(*p_out, 250);
	strcpy( *p_out, (char*)((dirtmp)?dirtmp:getenv("TEMP")) );
	tmpnam( &((*p_out)[strlen(*p_out)]) ) ;
	return *p_out;
	#endif

	}

//------------------------------------------------------------------------------
FILE *	fopen( char *fname, char *mode )
//------------------------------------------------------------------------------
	{
	#undef fopen
	return fopen( fname_verify(fname), mode );
	#define fopen elib_fopen
	}

//------------------------------------------------------------------------------
int fread( FILE *fp, char *data, int size )
//------------------------------------------------------------------------------
	{
	#undef fread
	return fread( data, 1, size, fp );
	#define fread elib_fread
	}

//------------------------------------------------------------------------------
long fwrite( FILE *fp, ... )
//------------------------------------------------------------------------------
	{
	#undef fwrite

	va_list args;
	char *arg;
	int l, n = 0;

	va_start(args, fp); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		if ( arg == __ ) { fflush( fp ); break; }
		l = va_arg(args, int);
		if ( arg )
			{
			if ( l == -1 ) l = strlen( arg );
			else
			if ( l == -2 ) l = strlen( arg )+1;
			if ( ( l )&&( fwrite( arg, 1, l, fp ) != l ) ) return ERROR;
			}
		n += l;
		}


	#define fwrite elib_fwrite
	
	return n;
	}


//------------------------------------------------------------------------------
bool fprint( FILE *fp, ... )
//------------------------------------------------------------------------------
	{
	#undef fwrite

	va_list args;
	char *arg;
	int l;
	
	va_start(args, fp); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		if ( arg == __ )
			{
			fflush( fp );
			return;
			}
		if ( arg )
			{
			l = strlen( arg );
			if ( ( l )&&( fwrite( arg, 1, l, fp ) != l ) ) return false;
			}
		}

	#define fwrite elib_fwrite
	
	return true;
	}


//------------------------------------------------------------------------------
char* file_load( char **p_out, char *fname )
//------------------------------------------------------------------------------
	{
	FILE *fp;
	unsigned int l;
	char *out, r = 0;

	fp = fopen( fname, "rb" );
	if ( fp == NULL )
		{
		if ( file_exists( fname ) )
			{
			for( r = 100 ; r ; Sleep(10), r-- )
				{
				if ( fp = fopen( fname, "rb" ) ) break;
				}
			}
		if ( !r )	return sERROR;
		}

	fseek_end(fp, 0);
	l = ftell(fp);
	fseek_top(fp, 0);

	out = (( p_out )&&( *p_out )) ? (char*)realloc( *p_out, l+1 ) : alloc( l+1 );
	
	#undef fread
	if ( l )	fread( out, 1, l, fp );
	#define fread	elib_fread
	fclose( fp );
	out[l] = 0;

	if ( p_out ) *p_out = out;
	return out;
	}

//------------------------------------------------------------------------------
int file_save_str( char *fname, char *in, int l )
//------------------------------------------------------------------------------
	{
	FILE *fp;

	if ( l < 0 ) l = strlen( in );
	
	fp = fopen( fname, "wb" );
	if ( fp == NULL )
		{
		mkdir( file_getdir( fname ) );
		fp = fopen( fname, "wb" );
		if ( fp == NULL )	return ERROR;
		}
	
	if ( l )	fwrite( fp, in, l, _ );
	fclose( fp );
	return OK;
	}

//------------------------------------------------------------------------------
int file_append( char *fname, char *in, int l )
//------------------------------------------------------------------------------
	{
	FILE *fp;

	if ( l < 0 ) l = strlen( in );
	
	fp = fopen( fname, "ab" );
	if ( fp == NULL )
		{
		mkdir( file_getdir( fname ) );
		fp = fopen( fname, "wb" );
		if ( fp == NULL )	return ERROR;
		}
	
	if ( l )	fwrite( fp, in, l, _ );
	fclose( fp );
	return OK;
	}

//------------------------------------------------------------------------------

typedef struct
	{
	T_FILE_INFO f;
	char dir[50][201];
	char sel[50][201];
	bool no_dir;
	bool abs_dir;
	bool no_file;
	bool no_hide;
	bool opt_size;
	bool opt_date;
	bool opt_owner;
	bool opt_right;
	bool opt_all;
	bool recursif;
	int level;
	DIR *rep[50];
	struct dirent *fic[50];
	char buf[512];
	int l;
	}
	T_FILE_LIST;


//------------------------------------------------------------------------------
T_FILE_INFO *file_list( T_FILE_INFO * t_file, char* p_fname, char *options )
//------------------------------------------------------------------------------
	{
	int p, n, ns = 0, b;
	char **ts, *s;
	char *fname;
	DIR *r;

	
	#define t ((T_FILE_LIST*)t_file)
	if ( t_file == NULL )
		{
		if ( p_fname == null ) return null;
		fname = fname_verify( p_fname );

		t_file = (T_FILE_INFO *)calloc( 1, sizeof(T_FILE_LIST) );
		if ( t == NULL ) return sERROR;
		
		p = 0;

		// LECTURE DU REPERTOIRE DE RECHERCHE
		ts = str_explode( fname, "," );
		if ( ts[0][0] == 0 )
			strcpy( t->dir[0], "." );
		else
			{
			if ( dir_exists( ts[0] ) )
				strcpy( t->dir[0], ts[0] );
			else
				{
				s = file_getdir( ts[0] );
				if ( ! dir_exists( s ) )
					{
					free( t );
					free( ts );
					return NULL;
					}
				strcpy( t->dir[0], s );
				strcpy( t->sel[ns++], file_getname( ts[0] ) );
				}
			}

		t->l = strlen( t->dir[0] );
		strcpy( t->f.sdir, t->dir[0] );

		// LECTURE DE LA SELECTION
		for( n = 1 ; ts[n] ; n++ )
			strcpy( t->sel[ns++], ts[n] );
		
		free( ts );
		
		// LECTURE DES OPTIONS
		if ( options )
			{
			for( ts = str_explode( options, " " ), n = 0 ; ts[n] ; n++ )
				{
				if ( ! stricmp( ts[n], "no_dir" ) ) t->no_dir = true;
				else
				if ( ! stricmp( ts[n], "nodir" ) ) t->no_dir = true;
				else
				if ( ! stricmp( ts[n], "norep" ) ) t->no_dir = true;
				else
				if ( ! stricmp( ts[n], "no_rep" ) ) t->no_dir = true;
				else
				if ( ! stricmp( ts[n], "nofile" ) ) t->no_file = true;
				else
				if ( ! stricmp( ts[n], "no_file" ) ) t->no_file = true;
				else
				if ( ! stricmp( ts[n], "nofic" ) ) t->no_file = true;
				else
				if ( ! stricmp( ts[n], "no_fic" ) ) t->no_file = true;
				else
				if ( ! stricmp( ts[n], "nohide" ) ) t->no_hide = true;
				else
				if ( ! stricmp( ts[n], "recursif" ) ) t->recursif = true;
				else
				if ( ! stricmp( ts[n], "r" ) ) t->recursif = true;
				else
				if ( ! stricmp( ts[n], "adir" ) ) t->abs_dir = true;
				else
				if ( ! stricmp( ts[n], "absdir" ) ) t->abs_dir = true;
				else
				if ( ! stricmp( ts[n], "abs_dir" ) ) t->abs_dir = true;
				else
				if ( ! stricmp( ts[n], "absolute_dir" ) ) t->abs_dir = true;
				else
				if ( ! stricmp( ts[n], "size" ) ) t->opt_size = true;
				else
				if ( ! stricmp( ts[n], "date" ) ) t->opt_date = true;
				else
				if ( ! stricmp( ts[n], "right" ) ) t->opt_right = true;
				else
				if ( ! stricmp( ts[n], "owner" ) ) t->opt_owner = true;
				else
				if ( ! stricmp( ts[n], "all" ) ) t->opt_all = true;
				}
			
			free( ts );
			}
		}

	while( 1 )
		{
		if ( t->rep[t->level] == NULL ) t->rep[t->level] = opendir( t->dir[t->level] );
		if ( t->rep[t->level] == NULL ) { free( t ); return NULL; }

		t->fic[t->level] = readdir( t->rep[t->level] );
		if ( t->fic[t->level] == NULL )
			{
			closedir(t->rep[t->level]);
			if ( t->level == 0 )
				{ free( t ); return NULL; }
				
			t->rep[t->level] = NULL;
			t->f.is_dir = true;
			t->level--;
			}
		else
			{
			if ( t->fic[t->level]->d_name[0] == '.')
				{
				if ( t->fic[t->level]->d_name[1] == 0 ) continue;
				else
				if ( ( t->fic[t->level]->d_name[1] == '.' )
					&&( t->fic[t->level]->d_name[2] == 0 ) )
					continue;
				}
	
			t->f.is_dir = 2;
			
			if ( t->recursif )
				{
				sprintf( t->buf, "%s" DIR_CHAR "%s", t->dir[t->level], t->fic[t->level]->d_name );
				r = opendir(t->buf);
				t->f.is_dir = ( r ) ? true : false;
				if ( r ) closedir( r );
				
				if ( t->f.is_dir )
					{
					strcpy( t->dir[++(t->level)], t->buf );
					continue;
					}
				}
			}
			
		// VERIFIE SI LE FICHIER CORRESPOND A LA SELECTION DEMANDEE
		
		if ( t->sel[0][0] == 0 ) b = true;
		else
		for( n = 0, b = 2 ; ( t->sel[n][0] ) ; n++ )
			{
			if ( t->sel[n][0] != '-' )
				{
				if ( b != true )
					{
					if ( strpcmp( t->sel[n], t->fic[t->level]->d_name ) )
						b = true;
					else
						b = false;
					}
				}
			else
				{
				if ( strpcmp( &(t->sel[n][1]), t->fic[t->level]->d_name ) )
					{
					b = false;
					break;
					}
				}
			}
			
		// OPTIONS NO_DIR ET NO_FILE ET NO_HIDE
		
		if ( b )
			{
			if ( t->f.is_dir == 2 )
				{
				sprintf( t->buf, "%s" DIR_CHAR "%s", t->dir[t->level], t->fic[t->level]->d_name );
				t->f.is_dir = ( r = opendir(t->buf) ) ? true : false;
				if ( r ) closedir( r );
				}
				
			if ( ( ( t->no_dir )&&( t->f.is_dir ) )
				||( ( t->no_file )&&( ! t->f.is_dir ) ) )
				b = false;
				
			if ( ( t->no_hide )&&( t->fic[t->level]->d_name[0] == '.' ) )
				b = false;
			}
		
		if ( b ) break;
		}


	strcpy( t->f.name, t->fic[t->level]->d_name );
	if ( ! t->abs_dir )
		{
		if ( t->level == 0 ) t->f.dir[0] = 0;
		else
			strcpy( t->f.dir, &(t->dir[t->level][t->l+1]) );
		}
	else
		strcpy( t->f.dir, t->dir[t->level] );

	return t_file;
	
	#undef t
	}


//------------------------------------------------------------------------------
void	file_list_free( T_FILE_INFO * t_file )
//------------------------------------------------------------------------------
	{
	while( t_file = file_list( t_file, null, null ) );
	}



//char *fdel = null;
//extern char *dirtmp;

//------------------------------------------------------------------------------
bool file_delete( char *fname )
//------------------------------------------------------------------------------
	{
	int i;
	
	#ifdef OS_WINDOWS
	char *s, *r = null;

	s = fname_verify(fname);

	if ( dir_exists( s ) )
		SystemH( &r, str( "rmdir /Q /S \"", s, "\" 2>null", _ ) );
	else
	if ( strpos( s, "*", 0 ) < 0 )
		unlink( s );
	else
		SystemH( &r, str( "del /Q /F \"", s, "\" 2>null", _ ) );


	if ( r ) free( r );
	unlink( "null" );
	
	#else //OS_UNIX
	
	if ( dir_exists( fname ) )
		system( str( "rm -Rf ", fname, _ ) );
	else
	if ( strpos( fname, "*", 0 ) < 0 )
		unlink( fname );
	else
		system( str( "rm -f ", fname, _ ) );

	#endif

	return true;	
	}

/*
//------------------------------------------------------------------------------
bool rdel( char *fname )
//------------------------------------------------------------------------------
	{
	char *s = null, *s1 = null;
	T_FILE_INFO *f = NULL;
	
	while( ( f = file_list( f, fname, "absdir" ) ) != NULL )
		{
		strdcpy( &s, f->dir, "/", f->name, _ );
		if ( f->is_dir )
			{
			rdel( strdcpy( &s1, s, DIR_CHAR, "*", _ ) );
			rmdir( s );
			}
		else unlink( s );
		}
	if ( s ) free( s );
	if ( s1 ) free( s1 );
	
	return true;
	}

//------------------------------------------------------------------------------
bool rdel2( char *fname )
//------------------------------------------------------------------------------
	{
	char *s = null, *s1 = null;
	T_FILE_INFO *f = NULL;
	
	while( ( f = file_list( f, fname, "absdir" ) ) != NULL )
		{
		strdcpy( &s, f->dir, "/", f->name, _ );
		if ( f->is_dir )
			{
			rdel2( strdcpy( &s1, s, DIR_CHAR, "*", _ ) );
			}
		else unlink( s );
		}

	if ( s ) free( s );
	if ( s1 ) free( s1 );

	return true;
	}
*/

//------------------------------------------------------------------------------
int file_copy( char *fname, char * fdest )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	
	System( null, str( "copy /Y \"", fname_verify(fname),
						"\" \"", fname_verify(fdest), "\"", _ ) );
	return true;
	
	#else	// OS_UNIX
	
	System( null, str( "cp -f \"", fname, "\" \"", fdest, "\"", _ ) );
	return true;
	
	#endif
	}

//------------------------------------------------------------------------------
int file_move( char *fname, char * fdest )
//------------------------------------------------------------------------------
	{
	char *v;
	
	#ifdef OS_UNIX
	
	static char *msg = null, *r = null;
	int n = 0;
	
	if ( strpos( fname, "*", 0 ) < 0 )
		{
		for(;;Sleep(10), n++)
			{
			unlink( fdest );
			if ( !file_exists( fdest ) ) break;
			if ( n > 50 )
				{
				error_set( 1, "ERROR file_move : can not remove dest file : ", fdest, _ );
				return ERROR;
				}
			}
		rename( fname, fdest );
		v = strdcpy( &msg, "ERROR file_move : ", itoa(errno), " : ", strerror(errno), " : ", fname, " => ", fdest, _ );
		msg = null;
		strdcpy( &r, "", _ );
		if ( !file_exists( fdest ) )
			{
			System( &r, strdcpy( &msg, "mv -f \"", fname, "\" \"", fdest, "\"", _ ) );
			trim(r);
			}
		if ( ( r[0] )||( !file_exists( fdest ) ) )
			{
			error_set( 1, r, " : ", v, _ );
			free( v );
			return ERROR;
			}
		free( v );
		}
	else
		{
		if ( strpos( fname, "*", 0 ) < 0 )
			System( &r, strdcpy( &msg, "mv -f \"", fname, "\" \"", fdest, "\"", _ ) );
		else
			System( &r, strdcpy( &msg, "mv -f \"", fname, "\" \"", fdest, "\"", _ ) );
		trim( r );
		if ( r[0] )
			{
			error_set( 1, msg, _ );
			return ERROR;
			}
		}
	
	return OK;
	
	#else // OS_WINDOWS
	char *r = null;
	
	if ( strpos( fname, "*", 0 ) < 0 )
		{
		unlink( fdest );
		for( ; file_exists( fdest ) ; Sleep(1) );
		rename( fname, fdest );
		v = str( "ERROR file_move : ", itoa(errno), " : ", strerror(errno), " : ", fname, " => ", fdest, _ );
		
		if (( !file_exists( fdest ) )||( dir_exists( fdest ) ))
			{
			SystemH( &r, str( "move /Y \"", fname_verify(fname),
						"\" \"", fname_verify(fdest), "\"", _ ) );
			}

		if ( !file_exists( fdest ) )
			{
			error_set( 1, v, _ );
			return ERROR;
			}
		}
		
	else
		SystemH( null, str( "move /Y \"", fname_verify(fname),
						"\" \"", fname_verify(fdest), "\"", _ ) );

	return OK;
	
	#endif
	}



//------------------------------------------------------------------------------
char * file_cat( char **p_out, char *fname, char * grep, char *grepv )
//------------------------------------------------------------------------------
	{
	T_FILE_INFO *tf = NULL;
	char *s = null, *f = null, bPrint=false;
	FILE *fp;
	
	if ( ! p_out ) bPrint = true;
	else strdcpy( p_out, "", _ );
	
	s = alloc(10000);
	
	while( tf = file_list( tf, fname, "absdir nodir" ) )
		{
		strdcpy( &f, tf->dir, "/", tf->name, _ );
		fp = fopen( f, "rb" );
		if ( fp == NULL ) continue;
		while( fgets( s, 9999, fp ) )
			{
			trim( s );

			if ( ( grep )&&( grep[0] )&&( strpos( s, grep, 0 ) < 0 ) ) continue;
			if ( ( grepv )&&( grepv[0] )&&( strpos( s, grepv, 0 ) >= 0 ) ) continue;
			if ( !bPrint )
				strdcat( p_out, s, BR, _ );
			else
				print( s, BR, _ );
			}
		fclose( fp );
		}
	
	if ( s ) free( s );
	if ( f ) free( f );
	
	return ( p_out ) ? *p_out : null;
	}

//------------------------------------------------------------------------------
char * file_icat( char **p_out, char *fname, char * grep, char *grepv )
//------------------------------------------------------------------------------
	{
	T_FILE_INFO *tf = NULL;
	char *s, *f = null, bPrint = false;
	FILE *fp;
	
	if ( ! p_out ) bPrint = true;
	else strdcpy( p_out, "", _ );
	
	s = alloc(10000);
	
	while( tf = file_list( tf, fname, "absdir nodir" ) )
		{
		strdcpy( &fname, tf->dir, "/", tf->name, _ );
		fp = fopen( fname, "rb" );
		if ( fp == NULL ) continue;
		while( fgets( s, 9999, fp ) )
			{
			trim( s );

			if ( ( grep )&&( grep[0] )&&( stripos( s, grep, 0 ) < 0 ) ) continue;
			if ( ( grepv )&&( grepv[0] )&&( stripos( s, grepv, 0 ) >= 0 ) ) continue;
			if ( !bPrint )
				strdcat( p_out, s, BR, _ );
			else
				print( s, BR, _ );
			}
		fclose( fp );
		}
	
	if ( s ) free( s );
	if ( f ) free( f );
	
	return ( p_out ) ? *p_out : null;
	}

//------------------------------------------------------------------------------
int mkdir( char *dname )
//------------------------------------------------------------------------------
	{
	int p = 0;
	char s[256];
	
	if ( dir_exists( dname ) ) return OK;
	
	dname = fname_verify( dname );

	#ifdef OS_UNIX
	if ( !strncmp( dname, "../", 3 ) )
		{
		getcwd( s, 256 );
		for( p = strlen( s )-1; p > 0 ; p-- )
			if ( s[p] == '/' ) break;
		s[p] = 0;
		strcat( s, &(dname[2]) );
		}
	else
	if ( !strncmp( dname, "./", 2 ) )
		{
		getcwd( s, 256 );
		for( p = strlen( s )-1; p > 0 ; p-- )
			if ( s[p] == '/' ) break;
		s[p] = 0;
		strcat( s, &(dname[1]) );
		}
	else
	#endif
		
	strcpy( s, dname );
	
	#undef mkdir
	while( ( p = strpos( s, DIR_CHAR, p ) ) >= 0 )
		{
		s[p] = 0;
		if ( ( strcmp( s, "." ) )&&( strcmp( s, ".." ) ) )
			{
			#ifdef OS_UNIX
			mkdir( s, 0700 );
			#else
			mkdir( s );
			#endif
			}
		s[p] = ((char*)(DIR_CHAR))[0];
		p++;
		}

	#ifdef OS_UNIX
	mkdir( s, 0700 );
	#else
	mkdir( s );
	#endif
	#define mkdir elib_mkdir
	}
		

//------------------------------------------------------------------------------
int		autoextract_add_files( char* autoextract_fname, char *fname, char *dirdest )
//------------------------------------------------------------------------------
	{
	static char *buf, *fname1 = null;
	int p0, n;
	FILE *fp, *fp2;
	T_FILE_INFO *tf = NULL;
	
	if ( ! file_exists( autoextract_fname ) )

		{
		error_set( 1, "ERREUR : fichier inexistant : ", autoextract_fname, _ );
		return ERROR;
		}

	
	p0 = file_size( autoextract_fname );
	
	fp = fopen( autoextract_fname, "rb+" );
	if ( ! fp )
		{
		error_set( 1, "ERREUR : Pb ouverture en lecture/écriture du fichier : ", autoextract_fname, _ );
		return ERROR;
		}
		
	buf = alloc( 4097 );
	
	fseek_top( fp, p0-16 );
	fread( fp, buf, 16 );

	if ( ! strncmp( buf, "ELIB4:", 6 ) )
		{
		fseek_top( fp, p0-16 );
		p0 = atoi( &(buf[6]) );
		}
	else
		fseek_end( fp, 0 );

	while( tf = file_list( tf, fname, "nodir absdir nohide" ) )
		{
		strdcpy( &fname, dirdest, tf->name, _ );
		strdcpy( &fname1, tf->dir, "/", tf->name, _ );

		print( fname1, BR, _ );
		
		fprint( fp, itona( strlen( fname ), 6 ), fname, itona( file_size( fname1 ), 10 ), __ );
		fp2 = fopen( fname1, "rb" );
		if ( fp2 == NULL )
			{
			error_set( 1, "ERREUR : Pb ouverture en lecture du fichier : ", fname1, _ );
			return ERROR;
			}
			
		while( n = fread( fp2, buf, 4096 ) )
			{
			fwrite( fp, buf, n, _ );
			}
			
		fclose( fp2 );
		}
	
	fprint( fp, "ELIB4:", itona( p0, 10 ), _ );
	fclose( fp );
	
	return OK;
	}



//------------------------------------------------------------------------------
int		autoextract_extract( char* autoextract_fname, char *filter, char *dirdest )
//------------------------------------------------------------------------------
	{
	static char *buf, *fname = null;
	int p0, n;
	FILE *fp, *fp2;
	T_FILE_INFO *tf = NULL;
	
	if ( ! file_exists( autoextract_fname ) )
		{
		error_set( 1, "ERREUR : fichier inexistant : ", autoextract_fname, _ );
		return ERROR;
		}
	
	fp = fopen( autoextract_fname, "rb" );
	if ( ! fp )
		{
		error_set( 1, "ERREUR : Pb ouverture en lecture du fichier (1) : ", autoextract_fname, _ );
		return ERROR;
		}
		
	buf = alloc( 4096 );
	
	fseek_end( fp, -16 );
	fread( fp, buf, 16 );
	if ( strncmp( buf, "ELIB4:", 6 ) )
		{
		fclose( fp );
		error_set( 1, "ERREUR : Ce n'est pas un fichier autoextractible : ", autoextract_fname, _ );
		return ERROR;
		}
		
	p0 = atoi( &(buf[6]) );

	fseek_top( fp, p0 );
	
	while( fread( fp, buf, 6 ) == 6 )
		{
		buf[6] = 0;
		if ( !strcmp( buf, "ELIB4:" ) ) break;
		n = atoi( buf );
		if ( n > 512 )
			{
			error_set( 1, "ERREUR : autoextractible non valide : ", autoextract_fname, _ );
			return ERROR;
			}
			
		if ( fread( fp, buf, n ) != n )
			{
			error_set( 1, "ERREUR : Pb lecture du fichier (2) : ", itoa(n), " : ", autoextract_fname, _ );
			return ERROR;
			}
		
		buf[n] = 0;

		fp2 = NULL;
		if ( ( !filter )||( !strnicmp( buf, filter, -1 ) ) )
			{
			strdcpy( &fname, dirdest, "/", buf, _ );
			
			print( "Extraction de : ", fname, BR, _ );
			mkdir( file_getdir( fname ) );
			
			fp2 = fopen( fname, "wb" );
			if ( fp2 == NULL )
				{
				error_set( 1, "ERREUR : Pb création  du fichier : ", fname, _ );
				return ERROR;
				}
			}
			
		if ( fread( fp, buf, 10 ) != 10 )
			{
			error_set( 1, "ERREUR : Pb lecture du fichier (3) : ", autoextract_fname, _ );
			return ERROR;
			}
		buf[10] = 0;
		if ( !is_numeric( buf ) )
			{
			error_set( 1, "ERREUR : autoextractible non valide : ", autoextract_fname, _ );
			return ERROR;
			}
			
		n = atoi( buf );
		
		while( n > 0 )
			{
			p0 = ( n >= 4096 ) ? 4096 : n;
			if ( fread( fp, buf, p0 ) != p0 )
				{
				error_set( 1, "ERREUR : Pb lecture du fichier (4) : ", autoextract_fname, _ );
				return ERROR;
				}
			if ( fp2 ) fwrite( fp2, buf, p0, _ );
			n -= 4096;
			}
		if ( fp2 ) fclose( fp2 );
		}
	
	fclose( fp );
	
	return OK;
	}



//------------------------------------------------------------------------------
int		autoextract_list( char* autoextract_fname )
//------------------------------------------------------------------------------
	{
	static char *buf, *fname = null;
	int p0, n;
	FILE *fp, *fp2;
	T_FILE_INFO *tf = NULL;
	
	if ( ! file_exists( autoextract_fname ) )
		{
		error_set( 1, "ERREUR : fichier inexistant : ", autoextract_fname, _ );
		return ERROR;
		}
	
	fp = fopen( autoextract_fname, "rb" );
	if ( ! fp )
		{
		error_set( 1, "ERREUR : Pb ouverture en lecture du fichier (1) : ", autoextract_fname, _ );
		return ERROR;
		}
		
	buf = alloc( 4096 );
	
	fseek_end( fp, -16 );
	fread( fp, buf, 16 );
	if ( strncmp( buf, "ELIB4:", 6 ) )
		{
		fclose( fp );
		error_set( 1, "ERREUR : Ce n'est pas un fichier autoextractible : ", autoextract_fname, _ );
		return ERROR;
		}
		
	p0 = atoi( &(buf[6]) );

	fseek_top( fp, p0 );
	
	while( fread( fp, buf, 6 ) == 6 )
		{
		buf[6] = 0;
		if ( !strcmp( buf, "ELIB4:" ) ) break;

		n = atoi( buf );
		if ( n > 512 )
			{
			error_set( 1, "ERREUR : autoextractible non valide : ", autoextract_fname, _ );
			return ERROR;
			}
			
		if ( fread( fp, buf, n ) != n )
			{
			error_set( 1, "ERREUR : Pb lecture du fichier (2) : ", itoa(n), " : ", autoextract_fname, _ );
			return ERROR;
			}
		
		buf[n] = 0;

		print( buf, BR, _ );
			
		if ( fread( fp, buf, 10 ) != 10 )
			{
			error_set( 1, "ERREUR : Pb lecture du fichier (3) : ", autoextract_fname, _ );
			return ERROR;
			}
		buf[10] = 0;
		if ( !is_numeric( buf ) )
			{
			error_set( 1, "ERREUR : autoextractible non valide : ", autoextract_fname, _ );
			return ERROR;
			}
			
		n = atoi( buf );
		
		while( n > 0 )
			{
			p0 = ( n >= 4096 ) ? 4096 : n;
			if ( fread( fp, buf, p0 ) != p0 )
				{
				error_set( 1, "ERREUR : Pb lecture du fichier (4) : ", autoextract_fname, _ );
				return ERROR;
				}
			n -= 4096;
			}
		}
	
	fclose( fp );
	
	return OK;
	}


























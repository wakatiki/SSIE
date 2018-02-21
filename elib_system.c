
#define ELIB_TMP_MEM_SIZE	1024
char *elib_tmp_mem[ELIB_TMP_MEM_SIZE];
int elib_tmp_mem_no = -1;
extern bool elib_print_to_screen;

//------------------------------------------------------------------------------
char **	memptr( int p_size )
//------------------------------------------------------------------------------
	{
	int mem_no;
	
	BeginCriticalSection();
	if ( elib_tmp_mem_no == -1 )
		for( elib_tmp_mem_no = 0 ; elib_tmp_mem_no < ELIB_TMP_MEM_SIZE ; elib_tmp_mem_no++ )
			elib_tmp_mem[elib_tmp_mem_no] = null;
	elib_tmp_mem_no++;
	if ( elib_tmp_mem_no >= ELIB_TMP_MEM_SIZE ) elib_tmp_mem_no = 0;
	mem_no = elib_tmp_mem_no;
	EndCriticalSection();

	if ( p_size )
		{
		if ( elib_tmp_mem[mem_no] )
			elib_tmp_mem[mem_no] = realloc( elib_tmp_mem[mem_no], p_size );
		else
			elib_tmp_mem[mem_no] = alloc0( p_size );
		}

	return( &(elib_tmp_mem[mem_no]) );
	}
	

//------------------------------------------------------------------------------
char *	memget( int p_size )
//------------------------------------------------------------------------------
	{
	int mem_no;
	
	BeginCriticalSection();
	if ( elib_tmp_mem_no == -1 )
		for( elib_tmp_mem_no = 0 ; elib_tmp_mem_no < ELIB_TMP_MEM_SIZE ; elib_tmp_mem_no++ )
			elib_tmp_mem[elib_tmp_mem_no] = null;
	elib_tmp_mem_no++;
	if ( elib_tmp_mem_no >= ELIB_TMP_MEM_SIZE ) elib_tmp_mem_no = 0;
	mem_no = elib_tmp_mem_no;
	EndCriticalSection();

	if ( elib_tmp_mem[mem_no] )
		elib_tmp_mem[mem_no] = realloc( elib_tmp_mem[mem_no], p_size );
	else
		elib_tmp_mem[mem_no] = alloc0( p_size );

	return( elib_tmp_mem[mem_no] );
	}
	

//------------------------------------------------------------------------------
void memll( char *m )
//------------------------------------------------------------------------------
	{
	int n;
	
	if ( elib_tmp_mem_no == -1 ) return;
	for( n = 0 ; n < ELIB_TMP_MEM_SIZE ; n++ )
		if ( elib_tmp_mem[n] == m )
			{
			elib_tmp_mem[n] = null;
			return;
			}
	}
	

#undef free

//------------------------------------------------------------------------------
void memfree( void )
//------------------------------------------------------------------------------
	{
	int n;
	
	for( n = 0 ; n < ELIB_TMP_MEM_SIZE ; n++ )
		if ( elib_tmp_mem[n] ) { free( elib_tmp_mem[n] ); elib_tmp_mem[n] = null; }
	}
	

//------------------------------------------------------------------------------
int elib_free( void *m )
//------------------------------------------------------------------------------
	{
	int n;
	
	if ( elib_tmp_mem_no != -1 )
	for( n = 0 ; n < ELIB_TMP_MEM_SIZE ; n++ )
		if ( elib_tmp_mem[n] == m )
			return;
	
	free( m );
	}


#define free elib_free


#ifdef OS_WINDOWS
#include <tlhelp32.h>

/*
#ifndef MODE_CONSOLE

//------------------------------------------------------------------------------				
bool system( char *cmd )
//------------------------------------------------------------------------------	
	{
	char *s = null;
    STARTUPINFO *si = (STARTUPINFO *)calloc(1,sizeof(STARTUPINFO) );
    PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)calloc(1,sizeof(PROCESS_INFORMATION) );

	strdcpy( &s, "cmd /C ", cmd, _ );

    si->cb = sizeof(STARTUPINFO);
//    si->wShowWindow = SW_HIDE;
    if( !CreateProcess( NULL,   // No module name (use command line)
				        s,            	// Command line
				        NULL,           // Process handle not inheritable
				        NULL,           // Thread handle not inheritable
				        FALSE,          // Set handle inheritance to FALSE
				        CREATE_NO_WINDOW, // ou ligne suivante
//				        0,
				        NULL,           // Use parent's environment block
				        NULL,           // Use parent's starting directory 
				        si,       		// Pointer to STARTUPINFO structure
				        pi      		// Pointer to PROCESS_INFORMATION structure
						)
    	)
		{ free( si ); free( pi ); return false; }

    WaitForSingleObject( pi->hProcess, INFINITE );
    CloseHandle( pi->hProcess );
    CloseHandle( pi->hThread );
    
    free( si );
	free( pi );
	free( s );
	
	return true;
	}
#endif
*/
#endif

//------------------------------------------------------------------------------				
char * System( char ** p_out, char *cmd )
//------------------------------------------------------------------------------	
	{
	if ( ! p_out ) p_out = memptr(0);
	
	#ifdef OS_UNIX
	
	char *fname = null;
	file_tmpname(&fname);
	//sni begin 27/11/2017 gerer les processus residuels
	//#ifdef LINUX
	//error_no = system( str( "timeout 100 ", cmd, " >", fname, " 2>&1", _ ) );
	//#else
	error_no = system( str( cmd, " >", fname, " 2>&1", _ ) );
	//#endif
	//sni: end
	
	
	if ( *p_out ) (*p_out)[0] = 0;

	if ( file_exists( fname ) )
		{
		file_load( p_out, fname );
		unlink( fname );
		}
	else
		strdcpy( p_out, "error create tmp file : ", fname, _ );
	free( fname );
	
	#else // OS_WINDOWS
	
	char *fname1 = null, *fname2 = null;
	
	file_tmpname(&fname1);
	file_tmpname(&fname2);

	error_no = system( str( cmd, " > ", fname1, " 2> ", fname2, _ ) );
	
	if ( *p_out ) (*p_out)[0] = 0;

	file_load( p_out, fname1 );
	strdcat( p_out, file_load( null, fname2 ), _ );
	
	unlink( fname1 );
	unlink( fname2 );
	free( fname1 );
	free( fname2 );
	
	#endif
	
//	if ( elib_print_to_screen ) printf( "%s", *p_out );
	
	return *p_out;
	}


//------------------------------------------------------------------------------				
bool tSystem( char ** p_out, int timeout, char *cmd )
//------------------------------------------------------------------------------	
	{
	int t = sysdate + timeout, p;
	
	if ( ! p_out ) p_out = memptr(0);
	
	#ifdef OS_UNIX

	char *fname = null, *fname2 = null, *s = null;
	
	file_tmpname(&fname);
	file_tmpname(&fname2);

	file_save_str( fname2, strdcpy( &s, cmd, " >", fname, " 2>&1", _ ), -1 );
	
	strdcpy( &s, "chmod 700 ", fname2, _ );
	system( s );

	p = StartH( fname2 );

	for( ; process_is_alive( p ) && ( sysdate < t ) ; Sleep(100) );
	if ( process_is_alive( p ) )
		{
		process_killc( p );
		p = false;
		}
	else
		p = true;
		
	if ( *p_out ) (*p_out)[0] = 0;

	if ( file_exists( fname ) )
		{
		file_load( p_out, fname );
		unlink( fname );
		}
	else
		strdcpy( p_out, "error create tmp file : ", fname, _ );

	unlink( fname );
	unlink( fname2 );
	free( fname );
	free( fname2 );
	
	return p;
	
	#else // OS_WINDOWS
	
	char *fname1 = null, *fname2 = null;
	
	file_tmpname(&fname1);
	file_tmpname(&fname2);

	p = StartH( str( "cmd.exe /C ", cmd, " > ", fname1, " 2> ", fname2, _ ) );
	if ( p == 0 ) return false;
	for( ; process_is_alive( p ) && ( sysdate < t ) ; Sleep(100) );
	if ( process_is_alive( p ) )
		{
		process_kill( p );
		p = false;
		}
	else
		p = true;
	
	if ( *p_out ) (*p_out)[0] = 0;

	file_load( p_out, fname1 );
	strdcat( p_out, file_load( null, fname2 ), _ );
	
	unlink( fname1 );
	unlink( fname2 );
	free( fname1 );
	free( fname2 );

	return p;
	
	#endif
	}


//------------------------------------------------------------------------------				
int Start( char *cmd )
//------------------------------------------------------------------------------	
	{
    int pid;

	#ifdef OS_WINDOWS
    STARTUPINFO *si = (STARTUPINFO *)calloc(1,sizeof(STARTUPINFO) );
    PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)calloc(1,sizeof(PROCESS_INFORMATION) );


    si->cb = sizeof(STARTUPINFO);
    if( !CreateProcess( NULL,   // No module name (use command line)
				        cmd,            	// Command line
				        NULL,           // Process handle not inheritable
				        NULL,           // Thread handle not inheritable
				        FALSE,          // Set handle inheritance to FALSE
//				        CREATE_NO_WINDOW, // ou ligne suivante
				        0,
				        NULL,           // Use parent's environment block
				        NULL,           // Use parent's starting directory 
				        si,       		// Pointer to STARTUPINFO structure
				        pi      		// Pointer to PROCESS_INFORMATION structure
						)
    	)
		{ free( si ); free( pi ); return -1; }

	pid = (int)(pi->dwProcessId);

	CloseHandle(pi->hProcess); // Fermer le handle vers le processus                            
    CloseHandle(pi->hThread);  // Fermer le handle vers le thread principal du processus

    free( si );
	free( pi );
	
	return pid;
	#else

	char **arg_list;
	
	pid = fork ();
	if (pid != 0) return pid;

	arg_list = str_explode( cmd, " " );
	execvp( arg_list[0], arg_list );
	exit(0);
	
	#endif
	
	return;
	}


//------------------------------------------------------------------------------				
int StartH( char *cmd )
//------------------------------------------------------------------------------	
	{
    int pid;

	#ifdef OS_WINDOWS

    STARTUPINFO *si = (STARTUPINFO *)calloc(1,sizeof(STARTUPINFO) );
    PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)calloc(1,sizeof(PROCESS_INFORMATION) );

//	strdcpy( s, cmd, _ );

    si->cb = sizeof(STARTUPINFO);
    si->wShowWindow = SW_HIDE;
    if( !CreateProcess( NULL,   // No module name (use command line)
				        cmd,            	// Command line
				        NULL,           // Process handle not inheritable
				        NULL,           // Thread handle not inheritable
				        FALSE,          // Set handle inheritance to FALSE
				        CREATE_NO_WINDOW, // ou ligne suivante
//				        0,
				        NULL,           // Use parent's environment block
				        NULL,           // Use parent's starting directory 
				        si,       		// Pointer to STARTUPINFO structure
				        pi      		// Pointer to PROCESS_INFORMATION structure
						)
    	)
		{ free( si ); free( pi ); return -1; }

	pid = (int)(pi->dwProcessId);

	CloseHandle(pi->hProcess); // Fermer le handle vers le processus
    CloseHandle(pi->hThread);  // Fermer le handle vers le thread principal du processus

    free( si );
	free( pi );
	
	return pid;
	
	
	#else	// OS_UNIX

	char *fname = null;
	char *s = null;
	FILE *fp;
	int r;
	
	pid = fork();
	if ( pid == -1 ) return -1;
	if (pid != 0)
		{ // Processus père
		for( ;; Sleep(50) )
			{
			#ifdef OS_AIX
			waitpid( pid, &r, 0 );
			#else
			waitpid( pid );
			#endif
			if ( !process_is_alive( pid ) ) break;
			}
		strdcpy( &fname, "/tmp/elib_", itoa(pid), _ );
		
		for( r = 1000 ; (r)&&( ( file_load( &s, fname ) == sERROR )||( atoi( s ) == 0 ) ) ; r-- )
			Sleep(10);
		unlink( fname );
		r = (s) ? atoi( s ) : -1;
		free( fname );
		if ( s ) free( s );
		return r;
		}
//#ifndef OS_AIX
	fclose( stdout );
//#endif
	while( (pid = Start( cmd )) == -1 ) Sleep(100);

	strdcpy( &fname, "/tmp/elib_", itoa(getpid()), _ );
	while( ( fp = fopen( fname, "wb" ) ) == NULL ) Sleep(10);
	fprint( fp, itoa( pid ), _ );
	fclose( fp );
	free( fname );
	
	exit(0);
	
	#endif
	}

/*
//------------------------------------------------------------------------------				
int StartR( char *cmd )
//------------------------------------------------------------------------------	
	{
    int pid;
	char *s = null;
	

	#ifdef OS_WINDOWS

    STARTUPINFO *si = (STARTUPINFO *)calloc(1,sizeof(STARTUPINFO) );
    PROCESS_INFORMATION *pi = (PROCESS_INFORMATION *)calloc(1,sizeof(PROCESS_INFORMATION) );

//	strdcpy( s, cmd, _ );

    si->cb = sizeof(STARTUPINFO);
    si->wShowWindow = SW_HIDE;
    if( !CreateProcess( NULL,   // No module name (use command line)
				        cmd,            	// Command line
				        NULL,           // Process handle not inheritable
				        NULL,           // Thread handle not inheritable
				        FALSE,          // Set handle inheritance to FALSE
				        CREATE_NO_WINDOW, // ou ligne suivante
//				        0,
				        NULL,           // Use parent's environment block
				        NULL,           // Use parent's starting directory 
				        si,       		// Pointer to STARTUPINFO structure
				        pi      		// Pointer to PROCESS_INFORMATION structure
						)
    	)
		{ free( si ); free( pi ); return false; }

	pid = (int)(pi->dwProcessId);

	CloseHandle(pi->hProcess); // Fermer le handle vers le processus
    CloseHandle(pi->hThread);  // Fermer le handle vers le thread principal du processus

    free( si );
	free( pi );
	
	return pid;
	
	
	#else	// OS_UNIX

	System( &s, str( cmd, " &", _ ) );
	
	#endif
	}
*/

#ifdef OS_WINDOWS

//------------------------------------------------------------------------------				
char * SystemH( char ** p_out, char *cmd )
//------------------------------------------------------------------------------	
	{
	int pid;
	
	if ( ! p_out ) p_out = memptr(0);
	
	char *fname1 = null, *fname2 = null;
	
	file_tmpname(&fname1);
	file_tmpname(&fname2);

	pid = StartH( str( "cmd /C ", cmd, " > ", fname1, " 2> ", fname2, _ ) );
	if ( pid != -1 )
	for( ; ; Sleep(10) )
		if ( ! process_is_alive( pid ) ) break;
	
	if ( *p_out ) (*p_out)[0] = 0;

	file_load( p_out, fname1 );
	strdcat( p_out, file_load( null, fname2 ), _ );
		
	unlink( fname1 );
	unlink( fname2 );
	free( fname1 );
	free( fname2 );

	return *p_out;
	}

#endif


//------------------------------------------------------------------------------				
int script_exec( char *cmd, handle hVar, int timeout, char **p_out )
//------------------------------------------------------------------------------	
	{			
	#ifndef OS_WINDOWS
	static char *fname =null, *s = null, *_cmd = null, r = true;
	#else
	char *fname =null, *s = null, *_cmd = null, r = true;
	#endif
	
	FILE *fp;
	bool bSudo = false;
	int p = 0;
	
	if ( !p_out ) return false;
	
	if ( hVar )
		var_replace( hVar, cmd, &_cmd );
	else
		_cmd = cmd;
		
	
	#ifdef OS_WINDOWS
	if ( ( strpos( _cmd, "\n", 0 ) >= 0 )||( ! file_exists( _cmd ) ) )
		{
		file_tmpname(&fname);
		strdcat( &fname, ".bat", _ );
		fp = fopen( fname, "wb" );
		fprint( fp, _cmd, _ );
		fclose( fp );
		if ( timeout )	
			r = tSystem( p_out, timeout, fname );
		else
			SystemH( p_out, fname );
		unlink( fname );
		free( fname );
		}
	else
	if ( timeout )	
		r = tSystem( p_out, timeout, _cmd );
	else
		System( p_out, _cmd );

	if ( hVar ) free( _cmd );
	return r;
	
	#else

	if ( ( strpos( _cmd, "\n", 0 ) >= 0 )||( ! file_exists( _cmd ) ) )
		{
		if ( stripos( _cmd, "sudo ", 0 ) == 0 )
			{
			bSudo = true;
			p = strpos( _cmd, "\n", 0 ) + 1;
			}
	
		file_tmpname(&fname);
		fp = fopen( fname, "wb" );
		strdcpy( &s, &(_cmd[p]), _ );
		strd_replace( &s, "\r\n", "\n" );
	
		fprint( fp, s, _ );
		fclose( fp );
		system( strdcpy( p_out, "chmod 755 ", fname, _ ) );
		strdcpy( &s, fname, _ );

		logw("sni:script a lancer fn(bef)=" , fname,"/", s,_);

		#ifdef OS_AIX
		if ( bSudo ) strdcpy( &fname, "sudo /ssie7/ssie7 \"su - ", trim(substr( _cmd, 5, p-5 )),
                                                " -c ", s, "\"", _ );
		#else
		if ( bSudo )
		strdcpy( &fname, "sudo /logiciels/ssie7/ssie7 \"timeout " ,itoa(timeout)," su - ", trim(substr( _cmd, 5, p-5 )),
                                                " -c ", s, "\"", _ );
		else strdcpy( &fname,"timeout ",itoa(timeout)," ", fname,_);
		#endif

		
		//begin		
		logw("sni:script a lancer fn=" ,fname,"/ tm=", itoa(timeout) ,_);
		

		if ( timeout )	
			r = tSystem( p_out, timeout, fname );
		else
			System( p_out, fname );

print( "file_delete ", s, BR, _ );

		unlink( s );
		}
	else
		if ( timeout )	
			return tSystem( p_out, timeout, _cmd );
		else
			System( p_out, _cmd );
	
	return true;
	
	#endif
	
	}


//------------------------------------------------------------------------------				
int	process_count( char * filter )
//------------------------------------------------------------------------------				
	{
	//..........................................................................
	#ifdef OS_WINDOWS
	//..........................................................................

	uint n = 0;
	HANDLE hProcessSnapPE,hProcessSnapME;
	#define init_me32 {sizeof(me32)}
	MODULEENTRY32 me32 = init_me32;
	#define init_pe32 {sizeof(pe32)}
	PROCESSENTRY32 pe32 = init_pe32;

	hProcessSnapPE = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if(hProcessSnapPE == (HANDLE)-1) return 0;
	if(Process32First(hProcessSnapPE, &pe32))
		{
		do
			{
			if ( ( pe32.th32ProcessID ) 
				&&( ( filter == NULL )
					||( stripos( pe32.szExeFile, filter, 0 ) >= 0 )
					)
					)
					n++;
			}
		while(Process32Next(hProcessSnapPE, &pe32));
		}
	CloseHandle(hProcessSnapPE);
	return n;

	//..........................................................................
	#else	// OS_UNIX
	//..........................................................................
	
	char *s = null;
	int n = 0;
	
	System( &s, str( "ps -ef | grep \"", filter, "\" | grep -v \"su -\" | grep -v grep | wc -l", _ ) );
	n = atoi( s );
	if ( s ) free( s );
	return n;

	#endif
	}

/*
//------------------------------------------------------------------------------				
int	process_list( char * filter )
//------------------------------------------------------------------------------				
	{
	//..........................................................................
	#ifdef OS_WINDOWS
	//..........................................................................

	uint n = 0;
	char buf[512];
	
	HANDLE hProcessSnapPE,hProcessSnapME;
	#define init_me32 {sizeof(me32)}
	MODULEENTRY32 me32 = init_me32;
	#define init_pe32 {sizeof(pe32)}
	PROCESSENTRY32 pe32 = init_pe32;

	hProcessSnapPE = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0); 
	if(hProcessSnapPE == (HANDLE)-1) return 0;
	
	if(Process32First(hProcessSnapPE, &pe32))
		{
		do
			{
			if ( ( pe32.th32ProcessID ) 
				&&( ( filter == NULL )
					||( stripos( pe32.szExeFile, filter, 0 ) >= 0 ) ) )
				{
				QueryFullProcessImageName( pe32.th32ProcessID, 1, &buf, &n );
					
				print( buf, BR, _ );
				}
			}
		while(Process32Next(hProcessSnapPE, &pe32));
		}

	CloseHandle(hProcessSnapPE);
	return OK;

	//..........................................................................
	#else	// OS_UNIX
	//..........................................................................
	
	static char *s = null;
	int n = 0;
	
	if ( System( &s, strdcpy( &s, "ps -ef | grep \"", filter, "\" | grep -v grep | wc -l", _ ) ) )
		n = atoi( s );
	return n;

	#endif
	}
*/

//------------------------------------------------------------------------------				
bool process_is_alive( int pid )
//------------------------------------------------------------------------------				
	{
	if ( !pid ) return false;

	//..........................................................................
	#ifdef OS_WINDOWS
	//..........................................................................

	HANDLE hProcessSnapPE,hProcessSnapME;
	MODULEENTRY32 me32 = init_me32;
	PROCESSENTRY32 pe32 = init_pe32;

	hProcessSnapPE = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if(hProcessSnapPE == (HANDLE)-1) return 0;
	if(Process32First(hProcessSnapPE, &pe32))
		{
		do
			{
			if( pe32.th32ProcessID == pid )
				{
				CloseHandle(hProcessSnapPE);
				return true;
				}
			}
		while(Process32Next(hProcessSnapPE, &pe32));
		}
	CloseHandle(hProcessSnapPE);
	return false;


	//..........................................................................
	#else	// OS_UNIX
	//..........................................................................

	return ( kill( pid, 0 ) ) ? false : true;

	//..........................................................................
	#endif	
	//..........................................................................
	}

//------------------------------------------------------------------------------				
void process_kill( int pid )
//------------------------------------------------------------------------------				
	{
	//..........................................................................
	#ifdef OS_WINDOWS
	//..........................................................................

	HANDLE hProcess;
	
	hProcess = OpenProcess( PROCESS_TERMINATE, 0, pid );
	if( hProcess )
		{
		TerminateProcess( hProcess, 1 );
		CloseHandle( hProcess );
		}
		
	//..........................................................................
	#else	// OS_UNIX
	//..........................................................................

	if ( pid <= 0 ) return;
	system( str( "kill -9 ", itoa(pid) , " 2> /dev/null", _ ) );

	#endif	

	return;
	}

//------------------------------------------------------------------------------				
void process_killc( int pid )
//------------------------------------------------------------------------------				
	{
	//..........................................................................
	#ifdef OS_WINDOWS
	//..........................................................................

	HANDLE hProcess;
	
	hProcess = OpenProcess( PROCESS_TERMINATE, 0, pid );
	if( hProcess )
		{
		TerminateProcess( hProcess, 1 );
		CloseHandle( hProcess );
		}
		
	//..........................................................................
	#else	// OS_UNIX
	//..........................................................................

	char *s = null, *s1 = null, **ts;
	
	if ( pid == -1 ) return;

	System( &s, strdcpy( &s1, "ps -ef | awk '{if ($3==", itoa(pid), ") print $2}'", _ ) );
//print( s1, BR, s, BR, _ );

	strd_replace( &s, "\n", " " );
	strd_replace( &s, "\r", "" );
	strdcpy( &s1, "kill -9 ", s, " ", itoa(pid), _ );
//print( s1, BR, _ );

	system( s1 );

//	system( strdcpy( &s, "kill -9 ", itoa(pid) , " 2> /dev/null", _ ) );

	#endif	

	return;
	}

//------------------------------------------------------------------------------				
void process_kill_all( char * filter ) // Ne kill pas le process courant
//------------------------------------------------------------------------------				
	{
	//..........................................................................
	#ifdef OS_WINDOWS
	//..........................................................................

	HANDLE hProcessSnapPE,hProcessSnapME;
	MODULEENTRY32 me32 = init_me32;
	PROCESSENTRY32 pe32 = init_pe32;
	int pid = getpid();

	hProcessSnapPE = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if(hProcessSnapPE == (HANDLE)-1) return;
	if(Process32First(hProcessSnapPE, &pe32))
		{
		do
			{
			if ( ( pe32.th32ProcessID ) 
				&&( ( filter == NULL )
					||( stripos( pe32.szExeFile, filter, 0 ) >= 0 ) )
				&& ( pe32.th32ProcessID != pid ) )
					process_kill( pe32.th32ProcessID );
			}
		while(Process32Next(hProcessSnapPE, &pe32));
		}
	CloseHandle(hProcessSnapPE);
	return;

	//..........................................................................
	#else	// OS_UNIX
	//..........................................................................

/*
static char *s = null;
logw( "process_kill_all : ", filter, _ );
System( &s, "ps -ef | grep ssie" );
logw( "process_kill_all : ps -ef | grep ssie" BR, s, _ );
System( &s, "ls -l /appli/ssie7/data/wrk" );
logw( "process_kill_all : ls -l /appli/ssie7/data/wrk" BR, s, _ );
*/
	if ( ( filter )&&( filter[0] ) )
		system( str( "kill -9 `ps -ef | grep \"", filter, "\" | grep -v grep | awk '{print $2;}' | grep -v ",
			itoa(getpid()), "` 2> /dev/null 1> /dev/null", _ ) );
	else
		system( str( "kill -9 `ps -ef | grep -v grep | awk '{print $2;}' | grep -v ",
			itoa(getpid()), "` 2> /dev/null 1> /dev/null", _ ) );

	#endif	
	}

//------------------------------------------------------------------------------				
bool process_waitend( int pid, uint timeout )
//------------------------------------------------------------------------------				
	{
	//..........................................................................
	#ifdef OS_WINDOWS
	//..........................................................................

	if ( WaitForSingleObject( OpenProcess( PROCESS_ALL_ACCESS, 0, pid ),
			(timeout) ? timeout*1000 : INFINITE ) == WAIT_TIMEOUT )
		{
//		process_kill( pid );
		return false;
		}
	return true;

	//..........................................................................
	#else	// OS_UNIX
	//..........................................................................

	char *s1 = null, *s2 = null;
	uint t = timeout*1000;
	int n;

	strdcpy( &s1, "ps -ef | grep -v defunct | awk '{print \" \" $2 \" \"}' | grep \" ", itoa(pid), " \"", _ );
	for(;;)
		{
		Sleep( 500 );
		System( &s2, s1 );
		n = ( s2 ) ? atoi( s2 ) : 0;
		if ( pid != n )
			{
			process_kill( pid );
			free( s1 );
			if ( s2 ) free( s2 );
			return true;
			}
		if ( timeout )
			{
			t -= 500;
			if ( t == 0 )
				{
				free( s1 );
				if ( s2 ) free( s2 );
				return false;
				}
			}
		}
		
	#endif	
	}


//------------------------------------------------------------------------------				
handle thread_start( int (ThreadFunc)(void *), void* Param )
//------------------------------------------------------------------------------				
	{
	//..........................................................................
	#ifdef OS_WINDOWS
	//..........................................................................

	HANDLE h;
	DWORD dwThreadId ;
	
	h = CreateThread( NULL, 0, (DWORD WINAPI(*)(void*))ThreadFunc, Param, 0, &dwThreadId );
	return (handle)h;


	//..........................................................................
	#else	// OS_UNIX
	//..........................................................................

	handle h, h1;
	#ifdef OS_AIX
	int r;
	#endif

//print( "pere1", BR, zz );

	h = fork();
	if (h == 0) /* Processus fils      */
    	{
		h1 = fork(); // Double fork pour ne pas avoir de defunc
//print( "fils1", BR, zz );
		if (h1 == 0) /* Processus fils      */
	    	{
//print( "fils2", BR, zz );
			ThreadFunc( Param );
			exit(0);
			}
//print( "fils1:fin", BR, zz );
		exit(0);
		}
	
//print( "pere2", BR, zz );
	for( ;; sleep(1) )
		{
//print( "pere3:", itoa(h), BR, zz );
//system( "ps -ef | grep ssie7c" );
		#ifdef OS_AIX
		waitpid( h, &r, 0 );
		#else
		waitpid( h );	// Pour supprimer le process zombi
		#endif
		if ( !process_is_alive( h ) ) break;
		
		}
	
//print( "pere4", BR, zz );

	return OK;

	#endif	
	}

//------------------------------------------------------------------------------				
bool thread_waitend( handle hThread, uint timeout )
//------------------------------------------------------------------------------				
	{
	//..........................................................................
	#ifdef OS_WINDOWS
	//..........................................................................
	if ( WaitForSingleObject( (HANDLE)hThread, (timeout) ? timeout*1000 : INFINITE )
			== WAIT_TIMEOUT )
		{
		thread_kill( hThread );
		return false;
		}
	return true;

	//..........................................................................
	#else	// OS_UNIX
	//..........................................................................
/*
	int t = timeout*1000;
	char *s;

	s = strcpy( NULL, " ", itoa( hThread ) , " " );
	while( 1 )
		{
		Sleep( 100 );
		if ( process_count( s ) == 0 )
			{ waitpid( hThread, &t, 0 ); free( s ); return true; }
		if ( timeout )
			{
			t -= 100;
			if ( t <= 0 )
				{
				process_kill( hThread );
				waitpid( hThread, &t, 0 );
				free( s );
				return false;
				}
			}
		}
*/
	#endif	
	}
	
//------------------------------------------------------------------------------				
bool thread_kill( handle hThread )
//------------------------------------------------------------------------------				
	{
	//..........................................................................
	#ifdef OS_WINDOWS
	//..........................................................................

	return TerminateThread( (HANDLE)hThread, 0 );

	//..........................................................................
	#else	// OS_UNIX
	//..........................................................................
	int n;
	
	process_kill( hThread );
	waitpid( hThread, &n, 0 );
//	_nb_thread--;
	return true;

	#endif	
	}





//------------------------------------------------------------------------------
char *sysuser()
//------------------------------------------------------------------------------
	{
	static char *user = null;
	
	if ( !user )
		{
		BeginCriticalSection();
		if ( !user )
			{
			#ifdef OS_UNIX
			System( &user, "whoami" );
			trim( user );
			#else
			strdcpy( &user, getenv( "USERNAME" ), _ );
			#endif
			}
		EndCriticalSection();
		}
		
	return user;
	}


























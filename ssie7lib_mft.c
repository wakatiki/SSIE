

#ifdef OS_WINDOWS
handle hThreadMftCat = 0;
#else
handle hMftCat = 0;
#endif

//------------------------------------------------------------------------------
handle mftcat_open()
	{
	handle hCfg ;
	#ifdef OS_WINDOWS
	handle hMftCat;
	char *lv, *lv2;
	
	lv = itoa( GetCurrentThreadId() );
	if ( !hThreadMftCat ) hThreadMftCat = var_new();
	else
	if ( lv2 = var_get( hThreadMftCat, lv ) )
		{
		return atoi( lv2 );
		}
	#else
	if ( hMftCat ) return hMftCat;
	#endif
		
	hCfg = var_from_cmd( 0, str(
		"dirdat=", dirdat, DIR_CHAR "mftcat" DIR_CHAR "info,"
		"diridx=", dirdat, DIR_CHAR "mftcat" DIR_CHAR "index,"
		"bd_lock_fname=", dirwrk, DIR_CHAR "bd_lock,"
		"key_name=ida,"
		"item_size=2048,"
		"vlfields='mft_log',"
		
		"index_path='direct/status/day_upd/part',"
//		"index_path='part/day_upd/status/direct'," // Met plus de 2 fois plus de temps ?????
		"index_key='dt_upd[19],spart[8],rpart[8],sappl[16],rappl[16],idt_mft[8],idf[8],ida[8],parm[120]',",
		_ ) );
	
	hMftCat = bd_hopen( hCfg );
	
	#ifdef OS_WINDOWS

	var_set( hThreadMftCat, lv, itoa( hMftCat ) );

	#endif

	return hMftCat;
	}
	
	
//------------------------------------------------------------------------------
int mftcat_crea( handle hData, char *status, char *p_errno, char*p_errmsg )
	{
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
	
	var_set( hData, "dt_crea", currdate("flog") );
	var_set( hData, "dt_upd", currdate("flog") );
	var_set( hData, "day_upd", currdate("aaaa_mm_jj") );
	var_set( hData, "status", status );
	var_set( hData, "errno", p_errno );
	var_set( hData, "errmsg", p_errmsg );
	if ( bd_insert( hMftCat, hData, null ) == ERROR )
		{
		logw( "mftcat_crea : ", error_msg, _ );
		return ERROR;
		}
	return OK;
	}
	
	

//------------------------------------------------------------------------------

int mftcat_upd( handle hData, char *status, char *p_errno, char*p_errmsg )
	{
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();

//static char *_s = null;

tr( "mftcat_upd1" );

	var_set( hData, "dt_upd", currdate("flog") );
	var_set( hData, "day_upd", currdate("aaaa_mm_jj") );
	if ( status )
		{
		if ( ( !(status[0]) )||( status[1] )
			||(( status[0] != 'C' )&&( status[0] != 'T' )&&( status[0] != 'F' )
			&&( status[0] != 'K' )&&( status[0] != 'H' )&&( status[0] != 'R' )
			&&( status[0] != 'D' )&&( status[0] != 'W' )&&( status[0] != 'X' ))
			)
			{
			logw( "mftcat_upd error bad status (1) : ida=", var_get( hData, "ida" ), ", status=", status, _ );
			status = var_get( hData, "status" );
			if ( strcmp( status, "F" ) )
				return ERROR;
			status = "T"; // Pour éviter les rebouclage infinie en cas de problème de mémoire
			}
		var_set( hData, "status", status );
		}
	if ( p_errno )	var_set( hData, "errno", p_errno );
	if ( p_errmsg )	var_set( hData, "errmsg", p_errmsg );

/*
	status = var_get( hData, "status" );
	if ( ( !status )||( !(status[0]) )||( status[1] )
		||(( status[0] != 'C' )&&( status[0] != 'T' )&&( status[0] != 'F' )
		&&( status[0] != 'K' )&&( status[0] != 'H' )&&( status[0] != 'R' )
		&&( status[0] != 'D' )&&( status[0] != 'W' )&&( status[0] != 'X' ))
		)
{
logw( "mftcat_upd error bad status (2) : ida=", var_get( hData, "ida" ), _ );
		return ERROR;
}
*/
/*
if (( status )&&( status[0] == 'F' ))
	{
	strdcpy( &_s, "ps -ef | grep ", itoa(getpid()), _ );
	System( &_s, _s );
	logw( "mftcat_upd set F : ida=", var_get( hData, "ida" ), ", ps=", _s, _ );
	}
*/
		
tr( "mftcat_upd2" );
	if ( bd_update( hMftCat, hData, var_get( hData, "ida" ), 1 ) == ERROR )
		{
		logw( "mftcat_upd error : ida=", var_get( hData, "ida" ), " : ", error_msg, _ );
		return ERROR;
		}
tr( "mftcat_upd3" );
	return OK;
	}


//------------------------------------------------------------------------------

int mftcat_del( char *idt )
	{
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
	
	return bd_delete( hMftCat, idt, 1 );
	}


//------------------------------------------------------------------------------
/*
int mftcat_purge( int delay )
	{
	char *_w = null, *_w1 = null;
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();

	
/*	
	strdcpy( &_w, "day_upd <= ", itod( sysdate-(delay Jours), "aaaa_mm_jj" ), _ );
	if ( bd_select( hMftCat, _w, null, &_w1 ) == ERROR )
		{
		printf( "%s\n", error_msg );
		if ( _w1 ) free( _w1 );
		free( _w );
		return;
		}

printf( "mftcat purge %s\n", _w1, BR, _ );
printf( "mftcat purge nb = %s", itoa(strlen(_w1)/8) );
* /

return OK;
	
	bd_delete( hMftCat, _w1, strlen(_w1)/8 );
	
	free( _w1 );
	free( _w );
	
	return bd_clean( hMftCat );
	}
*/

//------------------------------------------------------------------------------

handle mftcat_load( char *idt )
	{
	handle lh;
	char *_w = null;
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
	
	if ( bd_load( hMftCat, idt, 1, null, &_w ) <= 0 )
		return ERROR;
	lh = var_from_str( 0, _w );
	free( _w );
	return lh;
	}


//------------------------------------------------------------------------------

int mftcat_logw( handle hData, ... )
	{
	va_list args;
	char *arg;
	char *_w = null, *_v;
	
	_v = var_get( hData, "mft_log" );
	if (( !_v )||( strnicmp( _v, "elib_bd_vlf_concat:", 19 ) ))
		strdcpy( &_w, "elib_bd_vlf_concat:", _ );
	else
		strdcpy( &_w, _v, _ );
		
	strdcat( &_w, currdate("log"), " : ", _ );
	
	va_start(args, hData); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		if ( arg == __ ) break;
		strdcat( &_w, arg, _ );
		}
	
	strdcat( &_w, BR, _ );
	var_set( hData, "mft_log", _w );
	free( _w );
	}

//------------------------------------------------------------------------------

int mftcat_logr( char *idt, char **p_out )
	{
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	hMftCat = mftcat_open();
	
	return bd_load( hMftCat, idt, 1, ":mft_log", p_out );
	}


//------------------------------------------------------------------------------
int mftcat_repair( char *idt )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	handle hMftCat;
	#endif
	HANDLE _hLock;
	handle _h;
	
	hMftCat = mftcat_open();
	
	_hLock = bd_lock( hMftCat, idt );
	if ( _hLock == false ) return ERROR;
	
	bd_unindex( hMftCat, idt );

	if (( (_h = mftcat_load( idt )) == ERROR )
			||( !var_get( _h, "status" ) ))
		{
		printf( "ERREUR : Transfert inexistant : %s\n", idt );
		bd_unlock( hMftCat, idt, _hLock );
		return ERROR;
		}
		
	bd_unlock( hMftCat, idt, _hLock );
	mftcat_upd( _h, null, null, null );

	return OK;
	}


















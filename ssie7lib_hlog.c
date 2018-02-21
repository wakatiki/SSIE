
handle hFluxCat = hNULL;
handle hFluxChildren = hNULL;

//------------------------------------------------------------------------------
int fluxcat_open()
	{
	handle hCfg ;
		
	hCfg = var_from_cmd( 0, str(
		"dirdat=", dirdat, DIR_CHAR "hlog" DIR_CHAR "data,"
		"diridx=", dirdat, DIR_CHAR "hlog" DIR_CHAR "index,"
		"key_name=hdoc_id,"
		"item_size=vlf,"
		"vlfields='body',"
		
		"index_path='hdoc_status/hdoc_type/day_upd/parent_id',"
		"index_key='access_nb[10],dt_upd[19],readers_list[1000]",
		_ ) );
	
	hFluxCat = bd_hopen( hCfg );
	}

// hdoc_status = Running, Error, Canceled, OK, WaitFile, WaitDate, WaitRecv, WaitSendEnd,
	//	WaitAck, SendAck
// hdoc_Title
// Body
// hdoc_children = id1id2id3...
// parent_id
// keywords
// dt_crea
// dt_update
// hdoc_roles = 'role= 'read,create, {role}, ...'
// hdoc_rights = 'nni='role1,role2,...''

// reader_list=
// creater_list=
// updater_list=
// deleter_list=
// role_{role_name}_list = 

// keywords/{mot} = liste des sid qui contienne ce mot trié par ordre croissant (pour recherche dychotomique)

// 

/*
hD = hdoc_open( dirdat, diridx )
hdoc_close( hD )
hdoc_create( hD, parent_id, pos, hData )
hdoc_update( hD, sid_list, hData )
hdoc_delete( hD, sid_list )
hdoc_upd_order( hD, sid_list, nb )

hdoc_select( hD, cond, orderby, &id_list );
hdoc_load( hD, id_list, of, &out )
hdoc_loadbody( hD, sid, &body )
*/

//------------------------------------------------------------------------------
int fluxcat_crea( handle hData, char *status, char *p_errno, char*p_errmsg )
	{
	if ( ! hFluxCat ) fluxcat_open();
	var_set( hData, "dt_crea", currdate("flog") );
	var_set( hData, "dt_upd", currdate("flog") );
	var_set( hData, "day_upd", currdate("aaaa_mm_jj") );
	var_set( hData, "status", status );
	var_set( hData, "errno", p_errno );
	var_set( hData, "errmsg", p_errmsg );
	if ( bd_insert( hFluxCat, hData, null ) == ERROR )
		{
		logw( "fluxcat_crea : ", error_msg, _ );
		return ERROR;
		}
	return OK;
	}
	
	

//------------------------------------------------------------------------------

int fluxcat_upd( handle hData, char *status, char *p_errno, char*p_errmsg )
	{
	if ( ! hFluxCat ) fluxcat_open();
	var_set( hData, "dt_upd", currdate("flog") );
	var_set( hData, "day_upd", currdate("aaaa_mm_jj") );
	if ( status )	var_set( hData, "status", status );
	if ( p_errno )	var_set( hData, "errno", p_errno );
	if ( p_errmsg )	var_set( hData, "errmsg", p_errmsg );
	if ( bd_update( hFluxCat, hData, var_get( hData, "ida" ), 1 ) == ERROR )
		{
		logw( "fluxcat_upd : ", error_msg, _ );
		return ERROR;
		}
	return OK;
	}


//------------------------------------------------------------------------------

int fluxcat_del( char *idt )
	{
	if ( ! hFluxCat ) fluxcat_open();
	return bd_delete( hFluxCat, idt, 1 );
	}


//------------------------------------------------------------------------------

int fluxcat_purge( )
	{
	char *_w = null, *_w1 = null;
	
	if ( ! hFluxCat ) fluxcat_open();
	
	strdcpy( &_w, "day_upd < ", itod( sysdate-(30 Jours), "aaaa_mm_jj" ), _ );
	if ( bd_select( hFluxCat, _w, null, &_w1 ) == ERROR )
		{
		printf( "%s\n", error_msg );
		if ( _w1 ) free( _w1 );
		free( _w );
		return;
		}
	
	bd_delete( hFluxCat, _w1, strlen(_w1)/8 );
	
	if ( _w1 ) free( _w1 );
	free( _w );
	
	return bd_clean( hFluxCat );
	}


//------------------------------------------------------------------------------
/*
handle fluxcat_load( char *idt )
	{
	handle lh;
	char *_w = null;
	
	if ( ! hFluxCat ) fluxcat_open();
	if ( bd_load( hFluxCat, idt, 1, null, &_w ) == ERROR )
		return ERROR;
	lh = var_from_str( 0, _w );
	free( _w );
	return lh;
	}
*/

//------------------------------------------------------------------------------

int fluxcat_logw( handle hData, ... )
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

int fluxcat_logr( char *idt, char **p_out )
	{
	if ( ! hFluxCat ) fluxcat_open();
	return bd_load( hFluxCat, idt, 1, ":mft_log", p_out );
	}


















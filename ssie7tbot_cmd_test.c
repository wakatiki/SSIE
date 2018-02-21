
//------------------------------------------------------------------------------
int tbot_tests_log( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( !( v = var_get( hReq, "parent_id" ) ) )
		{
		print( "ERREUR : syntaxe : tests_log parent_id=..., log=..., status=...", _ );
		return ERROR;
		}
		
	strdcpy( &fname, dirdat, DIR_CHAR "tests", _ );
	strdcpy( &fname1, fname, DIR_CHAR "tests.id", _ );
	n = getuid( fname1 );
	strdcpy( &fname1, fname, DIR_CHAR, v, ".", v1=itoa(n), ".log", _ );
	
	strdcpy( &s, "[", quote(var_get( hReq, "status" )),
					",", v2=quote(itod( sysdate, "aaaa/mm/jj hh:mm:ss" )),
					",", v1,
					",", quote(var_get( hReq, "log" )),
					",[", _ );
/*
	strdcpy( &s, "parent_id=", v,
				",log_id=", v1,
				",date=", v2=quote(itod( sysdate, "aaaa/mm/jj hh:mm:ss" )),
				",log=", quote(var_get( hReq, "log" )),
				",status=", quote(var_get( hReq, "status" )), _ );
*/
	file_save_str( fname1, s, -1 );
	if ( atoi(v) == 0 )
		{
		strdcpy( &fname1, fname, DIR_CHAR, v, ".", v1, ".lst", _ );
		strdcpy( &s, "[", v2,
					",", quote(var_get( hReq, "log" )),
					",", quote(var_get( hReq, "status" )),
					",", v1, "]",
					_ );
		file_save_str( fname1, s, -1 );
		}
	print( v1, _ );
	}

//------------------------------------------------------------------------------
int tbot_tests_list( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, dirdat, DIR_CHAR "tests" DIR_CHAR "*.lst", _ );
	b = false;
	tf = NULL;
	while( tf = file_list( tf, fname, "absdir" ) )
		{
		if ( b ) print( ",", _ ); else b = true;
		
		strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
		file_load( &s, fname );
		print( s, _ );
		}
	}
	

//------------------------------------------------------------------------------
int tbot_tests_print_sublog( T_WORK *t_work, char *p_id )
//------------------------------------------------------------------------------
	{
	char bFirst = true, len;
	T_FILE_INFO *_tf;
	
	len = strlen( p_id )+1;
	strdcpy( &fname, dirdat, DIR_CHAR "tests" DIR_CHAR, p_id, ".*.log", _ );
	_tf = NULL;
	while( _tf = file_list( _tf, fname, "absdir" ) )
		{
		if ( !bFirst ) print( ",", _ ); else bFirst = false;
		
		strdcpy( &fname, _tf->dir, DIR_CHAR, _tf->name, _ );
		file_load( &s, fname );
		print( s, _ );

//print( "tf->name=", _tf->name, ", len=", itoa(len), ",", _ );
		
		v = itoa( atoi( &( _tf->name[len] ) ) );
		
		tbot_tests_print_sublog( t_work, v );
		print( "]]", _ );
		}
	}
	

//------------------------------------------------------------------------------
int tbot_tests_load( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &vn );
	strdcpy( &fname, dirdat, DIR_CHAR "tests" DIR_CHAR, "0.", vn, ".log", _ );
	file_load( &s, fname );
	print( s, _ );
	tbot_tests_print_sublog( t_work, vn );
	print( "]]", _ );
	}
	

//------------------------------------------------------------------------------
int tbot_tests_set_status( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( !var_getn( hReq, 1, &vn ) )
		{
		print( "ERREUR : syntaxe : proc_list {get|set} [value=...]", _ );
		return ERROR;
		}
	}
	

//------------------------------------------------------------------------------
int tbot_purge_log( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, ssie_dirlog, DIR_CHAR "*", _ );
	file_delete( fname );
	}


//------------------------------------------------------------------------------
int tbot_proc_list( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( !var_getn( hReq, 1, &vn ) )
		{
		print( "ERREUR : syntaxe : proc_list {get|set} [value=...]", _ );
		return ERROR;
		}
	
	strdcpy( &fname, dirdat, DIR_CHAR "proc_list.dat", _ );
	if ( stricmp( vn, "set" ) )
		{
		if ( file_load( &s, fname ) == sERROR )
			print( "[]", _ );
		else
			print( s, _ );
		return OK;
		}
	
	mkdir( dirdat );
	file_save_str( fname, var_get( hReq, "value" ), -1 );
	
	print( "OK", _ );
	return OK;
	}


//------------------------------------------------------------------------------
int tbot_proc_save( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( ! ( v = var_get( hReq, "id" ) ) )
		{
		strdcpy( &fname, dirdat, DIR_CHAR "procs", _ );
		mkdir( fname );
		strdcat( &fname, DIR_CHAR "procs.id", _ );
		n = get_uid( fname );
		v = itoa( n );
		var_set( hReq, "id", v );
		}
		
	strdcpy( &fname, dirdat, DIR_CHAR "procs", DIR_CHAR, v, ".dat", _ );
	var_save( hReq, fname );
	print( v, _ );
	}



//------------------------------------------------------------------------------
int tbot_proc_load( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( ! ( v = var_get( hReq, "id" ) ) )
		{
		print( "ERREUR : id obligatoire", _ );
		return ERROR;
		}
		
	strdcpy( &fname, dirdat, DIR_CHAR "procs", DIR_CHAR, v, ".dat", _ );
	var_free( h );
	h = var_load( fname );
	print( "{", var_to_js_array( h, &s ), "}", _ );
	return OK;
	}



//------------------------------------------------------------------------------
int tbot_proc_delete( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	for( i = 1 ; ; i++ )
		{
		if ( ! var_getn( hReq, i, &vn ) ) break;
		strdcpy( &fname, dirdat, DIR_CHAR "procs" DIR_CHAR, vn, ".dat", _ );
		file_delete( fname );
		}
	print( "OK", _ );
	}



//------------------------------------------------------------------------------
int tbot_func_load( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	var_getn( hReq, 1, &vn );
	strdcpy( &w, vn, _ );
	
	strdcpy( &s, "", _ );
	for( i = 2 ; var_getn( hReq, i, &vn ) ; i++ )
		{
		strdcpy( &fname, dirdat, DIR_CHAR "procs", DIR_CHAR, vn, ".dat", _ );
		if ( !file_exists( fname ) )
			{
			print( "ERREUR : proc ", vn, " n'existe pas", _ );
			return ERROR;
			}
		var_free( h );
		h = var_load( fname );
		v = var_get( h, "name" );
		if ( !v )
			{
			print( "ERREUR : proc ", vn, " mal définie", _ );
			return ERROR;
			}
		p = strpos( v, "(", 0 );
		if ( p < 0 ) p = strlen( v );
		strdncat( &s, w, -1, ".", 1, v, p, " = function", -1, &(v[p]), -1, _ );
		strdcat( &s, " {" BR, var_get( h, "script" ), BR "}" BR BR, _ );
		}
	
	print( s, _ );
	return OK;
	}


//------------------------------------------------------------------------------
int tbot_proc_func_load( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	var_getn( hReq, 1, &vn );
	strdcpy( &w, vn, _ );
	
	strdcpy( &s, "", _ );
	for( i = 2 ; var_getn( hReq, i, &vn ) ; i++ )
		{
		strdcpy( &fname, dirdat, DIR_CHAR "procs", DIR_CHAR, vn, ".dat", _ );
		if ( !file_exists( fname ) )
			{
			print( "ERREUR : proc ", vn, " n'existe pas", _ );
			return ERROR;
			}
		var_free( h );
		h = var_load( fname );
		
		
		p = strpos( v, "(", 0 );
		if ( p < 0 ) p = strlen( v );
		strdcat( &s, w, "[", vn, "] = function(){" BR,
					var_get( h, "script" ), BR "}" BR BR, _ );
		}
	
	print( s, _ );
	return OK;
	}


//------------------------------------------------------------------------------
int tbot_purge_cat( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, ssie_dirdat, DIR_CHAR "send", _ );
	file_delete( fname );
	strdcpy( &fname, ssie_dirdat, DIR_CHAR "recv", _ );
	file_delete( fname );
	strdcpy( &fname, ssie_dirdat, DIR_CHAR "mftcat", _ );
	file_delete( fname );
	strdcpy( &fname, ssie_dirwrk, DIR_CHAR "wsend", _ );
	file_delete( fname );
	}

//------------------------------------------------------------------------------
int cmd_test( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 1, &vn );
	if ( !v )
		{
		return ERROR;
		}
		
	if ( !stricmp( vn, "send_en_masse" ) )
		{
		v = var_getn( hReq, 2, &vn );
		if ( !v )
			{
			print( "ERREUR : Il faut donner le nombre" BR, _ );
			return ERROR;
			}
		n = atoi( vn );
		if ( n <= 0 )
			{
			print( "ERREUR : Il faut donner un nombre > 0" BR, _ );
			return ERROR;
			}

		v = var_getn( hReq, 3, &vn );
		if ( !v ) vn = var_get( hSrvConf, "partname" );
		
		strdcpy( &cmd, argv[0], " mft send part=", vn, ", fname=", argv[0], _ );
		for( i = 0 ; i < n ; i++ )
			{
			system( cmd );
			if ( (i)&&( ! ((i)%1000 ) )) print( itoa(i), " send" BR, _ );
			}
		
		return OK;
		}


	if ( !stricmp( vn, "purge_cat" ) )
		{
		strdcpy( &fname, dirdat, DIR_CHAR "send", _ );
		file_delete( fname );
		strdcpy( &fname, dirdat, DIR_CHAR "recv", _ );
		file_delete( fname );
		strdcpy( &fname, dirdat, DIR_CHAR "mftcat", _ );
		file_delete( fname );
		strdcpy( &fname, dirwrk, DIR_CHAR "wsend", _ );
		file_delete( fname );
		}

	if ( !stricmp( vn, "purge_log" ) )
		{
		strdcpy( &fname, dirlog, DIR_CHAR "*", _ );
		file_delete( fname );
		}
	}


//------------------------------------------------------------------------------
int tbot_arti_list( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( !var_getn( hReq, 1, &vn ) )
		{
		print( "ERREUR : syntaxe : arti_list {get|set} [value=...]", _ );
		return ERROR;
		}
	
	strdcpy( &fname, dirdat, DIR_CHAR "arti_list.dat", _ );
	if ( stricmp( vn, "set" ) )
		{
		if ( file_load( &s, fname ) == sERROR )
			print( "[]", _ );
		else
			print( s, _ );
		return OK;
		}
	
	mkdir( dirdat );
	file_save_str( fname, var_get( hReq, "value" ), -1 );
	
	print( "OK", _ );
	return OK;
	}


//------------------------------------------------------------------------------
int tbot_arti_save( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( ! ( v = var_get( hReq, "id" ) ) )
		{
		strdcpy( &fname, dirdat, DIR_CHAR "articles", _ );
		mkdir( fname );
		strdcat( &fname, DIR_CHAR "articles.id", _ );
		n = get_uid( fname );
		v = itoa( n );
		var_set( hReq, "id", v );
		}
		
	strdcpy( &fname, dirdat, DIR_CHAR "articles", DIR_CHAR, v, ".dat", _ );
	var_save( hReq, fname );
	print( v, _ );
	}



//------------------------------------------------------------------------------
int tbot_arti_load( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( ! ( v = var_get( hReq, "id" ) ) )
		{
		print( "ERREUR : id obligatoire", _ );
		return ERROR;
		}
		
	strdcpy( &fname, dirdat, DIR_CHAR "articles", DIR_CHAR, v, ".dat", _ );
	var_free( h );
	h = var_load( fname );
	print( "{", var_to_js_array( h, &s ), "}", _ );
	return OK;
	}



//------------------------------------------------------------------------------
int tbot_arti_delete( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	for( i = 1 ; ; i++ )
		{
		if ( ! var_getn( hReq, i, &vn ) ) break;
		strdcpy( &fname, dirdat, DIR_CHAR "articles" DIR_CHAR, vn, ".dat", _ );
		file_delete( fname );
		}
	print( "OK", _ );
	}



//------------------------------------------------------------------------------
int tbot_wdoc_list( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( !var_getn( hReq, 1, &vn ) )
		{
		print( "ERREUR : syntaxe : wdoc_list {get|set} [value=...]", _ );
		return ERROR;
		}
	
	strdcpy( &fname, dirdat, DIR_CHAR "wdoc_list.dat", _ );
	if ( stricmp( vn, "set" ) )
		{
		if ( file_load( &s, fname ) == sERROR )
			print( "[]", _ );
		else
			print( s, _ );
		return OK;
		}
	
	mkdir( dirdat );
	file_save_str( fname, var_get( hReq, "value" ), -1 );
	
	print( "OK", _ );
	return OK;
	}


//------------------------------------------------------------------------------
int tbot_wdoc_save( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( ! ( v = var_get( hReq, "id" ) ) )
		{
		strdcpy( &fname, dirdat, DIR_CHAR "wdoc", _ );
		mkdir( fname );
		strdcat( &fname, DIR_CHAR "wdoc.id", _ );
		n = get_uid( fname );
		v = itoa( n );
		var_set( hReq, "id", v );
		}
		
	strdcpy( &fname, dirdat, DIR_CHAR "wdoc", DIR_CHAR, v, ".dat", _ );
	var_save( hReq, fname );
	print( v, _ );
	}



//------------------------------------------------------------------------------
int tbot_wdoc_load( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	if ( ! ( v = var_get( hReq, "id" ) ) )
		{
		print( "ERREUR : id obligatoire", _ );
		return ERROR;
		}
		
	strdcpy( &fname, dirdat, DIR_CHAR "wdoc", DIR_CHAR, v, ".dat", _ );
	var_free( h );
	h = var_load( fname );
	print( "{", var_to_js_array( h, &s ), "}", _ );
	return OK;
	}



//------------------------------------------------------------------------------
int tbot_wdoc_delete( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	for( i = 1 ; ; i++ )
		{
		if ( ! var_getn( hReq, i, &vn ) ) break;
		strdcpy( &fname, dirdat, DIR_CHAR "wdoc" DIR_CHAR, vn, ".dat", _ );
		file_delete( fname );
		}
	print( "OK", _ );
	}



















	

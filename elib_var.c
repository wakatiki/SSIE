
typedef struct
	{
	char *name;
	char *data;
	}
	T_ELIB_VAR;

typedef struct
	{
	int 	nb_var;
	int 	max_var;
	bool	is_updated;
	T_ELIB_VAR *tblVar;
	}
	T_ELIB_TVAR;
	



//------------------------------------------------------------------------------
handle 	var_new( void )
//------------------------------------------------------------------------------
	{
	T_ELIB_TVAR *tVar;
	
	tVar = (T_ELIB_TVAR *)alloc( sizeof(T_ELIB_TVAR) );
	tVar->nb_var = 0;
	tVar->max_var = 200;
	tVar->is_updated = false;
	tVar->tblVar = (T_ELIB_VAR *)alloc( sizeof(T_ELIB_VAR)*200 );
	
	return (handle)tVar;
	}
	
//------------------------------------------------------------------------------

#define tVar ((T_ELIB_TVAR*)hVar)

//------------------------------------------------------------------------------
bool 	var_is_updated( handle hVar )
//------------------------------------------------------------------------------
	{
	return tVar->is_updated;
	}
	
//------------------------------------------------------------------------------
void 	var_free( handle hVar )
//------------------------------------------------------------------------------
	{
	int i;
	
	if ( ( !hVar )||( hVar == ERROR ) ) return;
	
	for( i = tVar->nb_var-1 ; i >= 0 ; i-- )
		{
		if ( tVar->tblVar[i].name ) free( tVar->tblVar[i].name );
		if ( tVar->tblVar[i].data ) free( tVar->tblVar[i].data );
		}

	free( tVar->tblVar );
	free( tVar );
	}
	
//------------------------------------------------------------------------------
char * 	var_get( handle hVar, char *vname )
//------------------------------------------------------------------------------
	{
	int i;

	for( i = tVar->nb_var-1 ; i >= 0 ; i-- )
		{
		if ( ! stricmp( vname, tVar->tblVar[i].name ) )
			return tVar->tblVar[i].data;
		}
	return null;
	}

//------------------------------------------------------------------------------
int 	var_count( handle hVar )
//------------------------------------------------------------------------------
	{
	return tVar->nb_var;
	}

//------------------------------------------------------------------------------
char *	var_getc( handle hVar, char *vname, char **p_out )
//------------------------------------------------------------------------------
	{
	int i;
	char *out = null;
	
	if (( p_out )&&( *p_out )) out = *p_out;
	
	for( i = tVar->nb_var-1 ; i >= 0 ; i-- )
		{
		if ( ! stricmp( vname, tVar->tblVar[i].name ) )
			{
			strdcpy( &out, tVar->tblVar[i].data, _ );
			if ( p_out ) *p_out = out;
			return out;
			}
		}
		
	return null;
	}

//------------------------------------------------------------------------------
char **	var_geta( handle hVar, char *vname )
//------------------------------------------------------------------------------
	{
	int i;
	
	for( i = tVar->nb_var-1 ; i >= 0 ; i-- )
		{
		if ( ! stricmp( vname, tVar->tblVar[i].name ) )
			return &(tVar->tblVar[i].data);
		}
	return null;
	}

//------------------------------------------------------------------------------
char * 	var_getn( handle hVar, int vnum, char **vname )
//------------------------------------------------------------------------------
	{
	if ( vnum >= tVar->nb_var ) return null;
	
	if ( vname )
		{
		*vname = tVar->tblVar[vnum].name;
//		if ( ! *vname ) *vname = alloc( strlen( tVar->tblVar[vnum].name ) + 1 );
//		else *vname = (char*)realloc( *vname, strlen( tVar->tblVar[vnum].name ) + 1 );
//		strcpy( *vname, tVar->tblVar[vnum].name );
		}
	return tVar->tblVar[vnum].data;
	}
	
//------------------------------------------------------------------------------
char * 	var_setn( handle hVar, int vnum, char *vname, char *data )
//------------------------------------------------------------------------------
	{
	if ( vnum >= tVar->nb_var ) return null;
	
	if ( vname )
		strdcpy( &(tVar->tblVar[vnum].name), vname, _ );
	if ( data )
		strdcpy( &(tVar->tblVar[vnum].data), data, _ );

	return;
	}
	
//------------------------------------------------------------------------------
char **	var_getna( handle hVar, int vnum, char **vname )
//------------------------------------------------------------------------------
	{
	if ( vnum >= tVar->nb_var ) return null;
	
	if ( vname )
		{
		*vname = tVar->tblVar[vnum].name;
//		if ( ! *vname ) *vname = alloc( strlen( tVar->tblVar[vnum].name ) + 1 );
//		else *vname = (char*)realloc( *vname, strlen( tVar->tblVar[vnum].name ) + 1 );
//		strcpy( *vname, tVar->tblVar[vnum].name );
		}
	return &(tVar->tblVar[vnum].data);
	}
	
//------------------------------------------------------------------------------
void var_set( handle hVar, char *vname, char *value )
//------------------------------------------------------------------------------
	{
	char **data;
	T_ELIB_VAR *tblVar;
	
	data = var_geta( hVar, vname );
	if ( data )
		{
		strdcpy( data, value, _ );
		tVar->is_updated = true;
		return;
		}
	
	if ( tVar->nb_var >= tVar->max_var )
		{
		tVar->max_var += 200;
		tVar->tblVar = (T_ELIB_VAR *)realloc( tVar->tblVar, sizeof(T_ELIB_VAR) * tVar->max_var );
		}
	
	tVar->tblVar[tVar->nb_var].name = alloc( strlen(vname) + 1 );
	strcpy( tVar->tblVar[tVar->nb_var].name, vname );
	if ( value )
		{
		tVar->tblVar[tVar->nb_var].data = alloc( strlen(value) + 1 );
		strcpy( tVar->tblVar[tVar->nb_var].data, value );
		}
	else
		{
		tVar->tblVar[tVar->nb_var].data = alloc( 1 );
		tVar->tblVar[tVar->nb_var].data[0] = 0;
		}

	tVar->nb_var++;
	tVar->is_updated = true;
	}


//------------------------------------------------------------------------------
void var_seta( handle hVar, char *vname, handle h )
//------------------------------------------------------------------------------
	{
	char **data;
	T_ELIB_VAR *tblVar;
	
	data = var_geta( hVar, vname );
	if ( data )
		{
		*data = (char*)(h);
		tVar->is_updated = true;
		return;
		}
	
	if ( tVar->nb_var >= tVar->max_var )
		{
		tVar->max_var += 200;
		tVar->tblVar = (T_ELIB_VAR *)realloc( tVar->tblVar, sizeof(T_ELIB_VAR) * tVar->max_var );
		}
	
	tVar->tblVar[tVar->nb_var].name = str( vname, _ );
	tVar->tblVar[tVar->nb_var].data = (char*)(h);
	tVar->nb_var++;
	tVar->is_updated = true;
	}


//------------------------------------------------------------------------------
void var_add( handle hVar, char *vname, char *value )
//------------------------------------------------------------------------------
	{
	T_ELIB_VAR *tblVar;
	
	if ( tVar->nb_var >= tVar->max_var )
		{
		tVar->max_var += 200;
		tVar->tblVar = (T_ELIB_VAR *)realloc( tVar->tblVar, sizeof(T_ELIB_VAR) * tVar->max_var );
		}
	
	tVar->tblVar[tVar->nb_var].name = str( vname, _ );
	tVar->tblVar[tVar->nb_var].data = str( value, _ );
	tVar->nb_var++;
	tVar->is_updated = true;
	}


//------------------------------------------------------------------------------
void	var_delete( handle hVar, char *vname )
//------------------------------------------------------------------------------
	{
	int i;
	
	for( i = tVar->nb_var-1 ; i >= 0 ; i-- )
		{
		if ( ! stricmp( vname, tVar->tblVar[i].name ) )
			{
			free( tVar->tblVar[i].name );
			free( tVar->tblVar[i].data );
			memcpy( &(tVar->tblVar[i]), &(tVar->tblVar[i+1]),
					sizeof( T_ELIB_VAR ) * ( tVar->nb_var - i - 1 ) );
			tVar->nb_var--;
			tVar->is_updated = true;
			return;
			}
		}
	}

//------------------------------------------------------------------------------
handle var_load( char *fname )
//------------------------------------------------------------------------------
	{
	char *s1 = null, *s2 = null, r = 0;
	long long n;
	
	if ( ! file_load( &s1, fname ) )
		goto var_load_fin_new;
		
	n = file_size( fname );
	if ( !n )
		goto var_load_fin_new;
	if ( uncrypte( &s2, s1, n ) == ERROR )
		goto var_load_fin_new;

	n = var_from_cmd( 0, s2 );
	if ( !n )
		goto var_load_fin_new;
		
	if ( s1 ) free( s1 );
	if ( s2 ) free( s2 );
	
	return n;
	
	var_load_fin_new:

	if ( s1 ) free( s1 );
	if ( s2 ) free( s2 );

	return var_new();
		
	}




//------------------------------------------------------------------------------				
char *	var_to_str0( handle hVar, char **p_out )
//------------------------------------------------------------------------------				
	{
	char *name, *value;
	int i;

	if ( !p_out ) return null;
	
	strdcpy( p_out, "", _ );
	for( i = 0 ; i < tVar->nb_var ; i++ )
		{
		value = var_getn( hVar, i, &name );
		if ( ( strcmp( value, "true" ) )
			&&( strcmp( value, "false" ) )
			&&( ! is_numeric( value ) ) )
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, "=", str_quote(value), _ );
		else
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, "=", value, _ );
		}

	return *p_out;
	}


//------------------------------------------------------------------------------
bool var_save( handle hVar, char *fname )
//------------------------------------------------------------------------------
	{
	char *s1 = null, *s2 = null, *fname1 = null, *v;
	FILE *fp;
	int n;
	
	var_to_str0( hVar, &s1 );

	n = crypte( &s2, s1 , strlen(s1)+1 );

	strdcpy( &fname1, fname, ".tmp", _ );
	fp = fopen( fname1, "wb" );
	if ( !fp )
		{
		v = file_getdir( fname );
		mkdir( v );
		fp = fopen( fname1, "wb" );
		if ( !fp )
			return false;
		}
	if ( fwrite( fp, s2, n, _ ) != n )
		{
		fclose( fp );
		unlink( fname1 );
		return false;
		}
	fclose( fp );
	tVar->is_updated = false;
	
	for(;;)
		{
		unlink( fname );
		if ( ! file_exists( fname ) ) break;
		Sleep(1);
		}

	for(;;)
		{
		rename( fname1, fname );
		if ( file_exists( fname ) ) break;
		Sleep(1);
		}
	
	if ( s1 ) free( s1 );
	if ( s2 ) free( s2 );
	if ( fname1 ) free( fname1 );
	
	return true;
	}

//------------------------------------------------------------------------------				
handle var_from_str( handle hVar, char *in )
//------------------------------------------------------------------------------				
	{
	long int e = 0, p;
	handle h;
	char *w = null, *parm = null;
	
	if ( !hVar ) hVar = var_new();
	
	h = text_init( in, " , = : { }" );
	while( ( text_getword( &w, h ) ) )
		{
		if ( e == 0 )
			{
			if ( ( ! strcmp( w, "," ) )||( ! strcmp( w, "{" ) )||( ! strcmp( w, "}" ) ) )
				continue;

			if ( ( ! strcmp( w, "=" ) )||( ! strcmp( w, ":" ) ) )
				{
				free( (void*)h );
				return var_new();
				}

			strdcpy( &parm, w, _ );
			strd_unquote(&parm);
			e = 1;
			continue;
			}
		if ( e == 1 )
			{
			if ( ( ! strcmp( w, "=" ) )||( ! strcmp( w, ":" ) ) )
				{
				e = 2;
				continue;
				}
			if ( ! strcmp( w, "," ) )
				{
				var_set( hVar, parm, "true" );
				e = 0;
				continue;
				}
			var_set( hVar, parm, "true" );
			strdcpy( &parm, w, _ );
			strd_unquote(&parm);
			e = 1;
			continue;
			}
		if ( e == 2 )
			{
			if ( ( ! strcmp( w, "," ) )||( ! strcmp( w, "{" ) )||( ! strcmp( w, "}" ) ) )
				{
				var_set( hVar, parm, "" );
				e = 0;
				continue;
				}
			var_set( hVar, parm, unquote(w) );
			e = 0;
			continue;
			}
		}
	if ( e == 1 )
		var_set( hVar, parm, "true" );
	if ( e == 2 )
		var_set( hVar, parm, "" );

	if ( w ) free( w );
	if ( parm ) free( parm );

	return hVar;
	}


//------------------------------------------------------------------------------				
handle var_from_cmd( handle hVar, char *in )
//------------------------------------------------------------------------------				
	{
	long int e = 0, p;
	handle h;
	char *w = null, *parm = null, *cond;

	if ( !hVar ) hVar = var_new();

	h = text_init( in, ">=<=!=<>>< , = > <" );

	while( ( text_getword( &w, h ) ) )
		{
		if ( e == 0 )
			{
			if ( ! strcmp( w, "," ) )
				continue;

			if ( ! strcmp( w, "=" ) )
				{
				free( (void*)h );
				return var_new();
				}

			cond = null;
			
			if ( ( parm )&&( ( w[0] == '\'' )||( w[0] == '"' ) ) )
				var_set( hVar, parm, str( var_get( hVar, parm ), unquote(w), _ ) );
			else
				{
				strdcpy( &parm, w, _ );
				strd_unquote(&parm);
				e = 1;
				}
			continue;
			}
		if ( e == 1 )
			{
			if ( ! strcmp( w, "=" ) )
				{
				e = 2; cond = null;
				continue;
				}
			if ( ! strcmp( w, "," ) )
				{
				var_set( hVar, parm, "true" );
				e = 0;
				continue;
				}
			if ( ! strcmp( w, ">" ) )
				{
				e = 2; cond = ">";
				continue;
				}
			if ( ! strcmp( w, "<" ) )
				{
				e = 2; cond = "<";
				continue;
				}
			if ( ! strcmp( w, ">=" ) )
				{
				e = 2; cond = ">=";
				continue;
				}
			if ( ! strcmp( w, "<=" ) )
				{
				e = 2; cond = "<=";
				continue;
				}
			if ( ! strcmp( w, "!=" ) )
				{
				e = 2; cond = "!";
				continue;
				}
			if ( ! strcmp( w, "<>" ) ) // Between
				{
				e = 2; cond = "<>";
				continue;
				}
			if ( ! strcmp( w, "><" ) ) // Not between
				{
				e = 2; cond = "><";
				continue;
				}
			var_set( hVar, parm, "true" );
			strdcpy( &parm, w, _ );
			strd_unquote(&parm);
			e = 1;
			continue;
			}
		if ( e == 2 )
			{
			if ( ! strcmp( w, "," ) )
				{
				var_set( hVar, parm, "" );
				e = 0;
				continue;
				}
			
			if ( !cond )
				var_set( hVar, parm, unquote(w) );
			else
				var_set( hVar, parm, str( cond, unquote(w), _ ) );
				
			e = 0;
			continue;
			}
		}

	if ( e == 1 )
		var_set( hVar, parm, "true" );
	if ( e == 2 )
		var_set( hVar, parm, "" );

	if ( w ) free( w );
	if ( parm ) free( parm );

	return hVar;
	}


//------------------------------------------------------------------------------				
void	var_print( handle hVar, char *sep )
//------------------------------------------------------------------------------				
	{
	char *name, *value;
	int i;

	if ( sep == null ) return;
	for( i = 0 ; i < tVar->nb_var ; i++ )
		{
		value = var_getn( hVar, i, &name );
		if ( strpcmp( "*_private_key", name ) )
			print( (char*)((i)?sep:""), name, ":'########'", _ );
		else
		if ( !strncmp( name, "pw_", 3 ) )
			print( (char*)((i)?sep:""), name, ":'########'", _ );
		else
		if ( ( strpos( value, " ", 0 ) >= 0 )
			||( strpos( value, "'", 0 ) >= 0 )
			||( strpos( value, "\"", 0 ) >= 0 )
			||( strpos( value, ",", 0 ) >= 0 )
			||( strpos( value, "=", 0 ) >= 0 )
			||( strpos( value, "	", 0 ) >= 0 )
			||( strpos( value, "\n", 0 ) >= 0 )
			||( strpos( value, "\r", 0 ) >= 0 )
			)
			print( (char*)((i)?sep:""), name, " = ", str_quote(value), _ );
		else
			print( (char*)((i)?sep:""), name, " = ", value, _ );
		}
	return;
	}



//------------------------------------------------------------------------------				
char *	var_sprint( handle hVar, char *sep, char **p_out )
//------------------------------------------------------------------------------				
	{
	char *name, *value;
	int i;

	if ( sep == null ) return sERROR;
	if ( !p_out ) return sERROR;
	strdcpy( p_out, "", _ );
	
	for( i = 0 ; i < tVar->nb_var ; i++ )
		{
		value = var_getn( hVar, i, &name );
		if ( strpcmp( "*_private_key", name ) )
			strdcat( p_out, (char*)((i)?sep:""), name, ":'########'", _ );
		else
		if ( !strncmp( name, "pw_", 3 ) )
			strdcat( p_out, (char*)((i)?sep:""), name, ":'########'", _ );
		else
		if ( ( strpos( value, " ", 0 ) >= 0 )
			||( strpos( value, "'", 0 ) >= 0 )
			||( strpos( value, "\"", 0 ) >= 0 )
			||( strpos( value, ",", 0 ) >= 0 )
			||( strpos( value, "=", 0 ) >= 0 )
			||( strpos( value, "	", 0 ) >= 0 )
			||( strpos( value, "\n", 0 ) >= 0 )
			||( strpos( value, "\r", 0 ) >= 0 )
			)
			strdcat( p_out, (char*)((i)?sep:""), name, " = ", str_quote(value), _ );
		else
			strdcat( p_out, (char*)((i)?sep:""), name, " = ", value, _ );
		}
	return *p_out;
	}



//------------------------------------------------------------------------------				
void	var_print0( handle hVar, char *sep )
//------------------------------------------------------------------------------				
	{
	char *name, *value;
	int i;

	if ( sep == null ) return;
	for( i = 0 ; i < tVar->nb_var ; i++ )
		{
		value = var_getn( hVar, i, &name );
		if ( ( strpos( value, " ", 0 ) >= 0 )
			||( strpos( value, "'", 0 ) >= 0 )
			||( strpos( value, "\"", 0 ) >= 0 )
			||( strpos( value, ",", 0 ) >= 0 )
			||( strpos( value, "=", 0 ) >= 0 )
			||( strpos( value, "	", 0 ) >= 0 )
			||( strpos( value, "\n", 0 ) >= 0 )
			||( strpos( value, "\r", 0 ) >= 0 )
			)
			print( (char*)((i)?sep:""), name, " = ", str_quote(value), _ );
		else
			print( (char*)((i)?sep:""), name, " = ", value, _ );
		}
	return;
	}



//------------------------------------------------------------------------------				
void	var_tprint( handle hVar, char *sep, int pad )
//------------------------------------------------------------------------------				
	{
	char *name, *value;
	int i;

	if ( sep == null ) return;
	print( sep, _ );
	for( i = 0 ; i < tVar->nb_var ; i++ )
		{
		value = var_getn( hVar, i, &name );
		if ( strpcmp( "*_private_key", name ) )
			print( (char*)((i)?sep:""), strpad(name,pad," "), "= '########'", _ );
		else
		if ( !strncmp( name, "pw_", 3 ) )
			print( (char*)((i)?sep:""), strpad(name,pad," "), "= '########'", _ );
		else
		if ( ( strpos( value, " ", 0 ) >= 0 )
			||( strpos( value, "'", 0 ) >= 0 )
			||( strpos( value, "\"", 0 ) >= 0 )
			||( strpos( value, ",", 0 ) >= 0 )
			||( strpos( value, "=", 0 ) >= 0 )
			||( strpos( value, "	", 0 ) >= 0 )
			||( strpos( value, "\n", 0 ) >= 0 )
			||( strpos( value, "\r", 0 ) >= 0 )
			)
			print( (char*)((i)?sep:""), strpad(name,pad," "), " = ", str_quote(value), _ );
		else
			print( (char*)((i)?sep:""), strpad(name,pad," "), " = ", value, _ );
		}
	print( BR, _ );
	return;
	}



//------------------------------------------------------------------------------				
char *	var_to_str( handle hVar, char **p_out )
//------------------------------------------------------------------------------				
	{
	char *name, *value;
	int i;

	if ( !p_out ) return null;
	
	strdcpy( p_out, "", _ );
	for( i = 0 ; i < tVar->nb_var ; i++ )
		{
		value = var_getn( hVar, i, &name );
		if ( strpcmp( "*_private_key", name ) )
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, "='########'", _ );
		else
		if ( !strncmp( name, "pw_", 3 ) )
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, "='########'", _ );
		else
		if ( ( strcmp( value, "true" ) )
			&&( strcmp( value, "false" ) )
			&&( ! is_numeric( value ) ) )
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, "=", str_quote(value), _ );
		else
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, "=", value, _ );
		}
		
	return *p_out;
	}



//------------------------------------------------------------------------------				
char *	var_to_str_nq( handle hVar, char **p_out )
//------------------------------------------------------------------------------				
	{
	char *name, *value;
	int i;

	strdcpy( p_out, "", _ );
	
	for( i = 0 ; i < tVar->nb_var ; i++ )
		{
		value = var_getn( hVar, i, &name );
		if ( strpcmp( "*_private_key", name ) )
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, ":'########'", _ );
		else
		if ( !strncmp( name, "pw_", 3 ) )
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, ":'########'", _ );
		else
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, "=", value, _ );

//		strdcat( &out, (char*)((out[0])?",":""), name, "=", var_getn( hVar, i, &name ), _ );
		}

	return *p_out;
	}



//------------------------------------------------------------------------------				
char *	var_to_js_array( handle hVar, char **p_out )
//------------------------------------------------------------------------------				
	{
	char *name, *value;
	int i, j, l;

	strdcpy( p_out, "", _ );
	for( i = 0 ; i < tVar->nb_var ; i++ )
		{
		value = var_getn( hVar, i, &name );
		for( j = 0, l = strlen(name) ; j < l ; j++ )
			if ( ( name[j] == ':' )||( name[j] == '\\' ) ) name[j] = '_';
		
		if ( strpcmp( "*_private_key", name ) )
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, ":'########'", _ );
		else
		if ( !strncmp( name, "pw_", 3 ) )
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, ":'########'", _ );
		else
		if ( ( strcmp( value, "true" ) )
			&&( strcmp( value, "false" ) )
			&&(( ! is_numeric( value ) )||( (value[0] == '0')&&(value[1]) ))
			)
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, ":", str_quote(value), _ );
		else
			strdcat( p_out, (char*)(((*p_out)[0])?",":""), name, ":", value, _ );
		}
	return *p_out;
	}


//------------------------------------------------------------------------------
handle var_dup( handle hVar )
//------------------------------------------------------------------------------
	{
	char *s = null;
	handle h;
	
	var_to_str0( hVar, &s );
	h = var_from_str( 0, s );
	free( s );
	return h;
	}



//------------------------------------------------------------------------------
char * var_list_prepare( handle ph )
//------------------------------------------------------------------------------
	{
	char *list = null, *s = null, *v, *v1, *vn, **ts;
	int i, b, b1, n, n1;
	 
	v = var_get( ph, "var_list" );
	if ( !v )
		{
		strdcpy( &list, "", _ );
		for( i = 0 ; v = var_getn( ph, i, &vn ) ; i++ )
			strdcat( &list, (char*)((i)?",":""), vn, _ );
		var_set( ph, "var_list", list );
		v = list;
		}

	b = true;
	b1 = false;
	ts = str_explode( v, "," );
	while( b )
		for( i = 0, b = false ; ts[i+1] ; i++ )
			{
			if ( strlen( ts[i] ) < strlen( ts[i+1] ) )
				{
				v1 = ts[i];
				ts[i] = ts[i+1];
				ts[i+1] = v1;
				b = b1 = true;
				}
			}

	if ( b1 )
		{
		strdcpy( &s, "", _ );
		for( i = 0 ; ts[i] ; i++ )
			{
			strdcat( &s, ",", ts[i], _ );
			}

		n = strlen( s );
		for(;;)
			{
			strd_replace( &s, ",,", "," );
			if ( ( n1 = strlen(s) ) == n ) break;
			n = n1;
			}
			
		var_set( ph, "var_list", s );
		}
	
	free( ts );		
	if ( list ) free( list );
	if ( s ) free( s );	
	}

//------------------------------------------------------------------------------
char * var_list_replace( handle ph, char **inout )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	char *s = null, *v, *vn, **ts;
	int i, p, p0, p1, p2, l, n, b;
	#else
	static char *s = null, *v, *vn, **ts;
	static int i, p, p0, p1, p2, l, n, b;
	#endif
	#define o (*inout)
	
	v = var_get( ph, "var_list" );
	if ( v )
		{
		ts = str_explode( v, "," );
		for( i = 0 ; ts[i] ; i++ )
			{
			if ( ! (ts[i][0]) ) continue;
			v = var_get( ph, strdcpy( &s, "_", ts[i], _ ) ); // Les noms sont précédés un '_'
			if ( !v )	v = var_get( ph, ts[i] ); 
			if ( v )
				{
				strdcpy( &s, "$", ts[i], _ );
				n = strlen( s );
				p = -1;
				b = true;
				while( b )
					{
					b = false;
					while( ( p = stripos( o, s, p+1 ) ) >= 0 )
						{
						b = true;
						if ( o[p+n] != '[' )
							{
							strdcpy( inout, substr( o, 0, p ), v, substr( o, p+strlen(s), -1 ), _ );
							continue;
							}
	
						p1 = strpos( o, "]", p );
						if ( p1 < 0 )
							{
							strdcpy( inout, substr( o, 0, p ), v, substr( o, p+strlen(s), -1 ), _ );
							continue;
							}
						p2 = strpos( o, "$", p+n+1 ) ;
						if ( ( p2 >= 0 )&&( p2 < p1 ) )
							{
							b = false;
							continue;
							}
						
						p2 = strpos( o, "[", p+n+1 );
						if ( ( p2 >= 0 )&&( p2 < p1 ) )
							{
							strdcpy( inout, substr( o, 0, p ), v, substr( o, p+strlen(s), -1 ), _ );
							continue;
							}
						p0 = atoi( &(o[p+n+1]) );
						if ( ( p0 < 0 )||( p0 >= strlen(v) ) )
							{
							strdcpy( inout, substr( o, 0, p ), substr( o, p1+1, -1 ), _ );
							continue;
							}
						p2 = strpos( o, ",", p+n+1 );
						if ( ( p2 < 0 )||( p2 > p1 ) )
							{
							strdcpy( inout, substr( o, 0, p ), substr( v, p0, -1 ), substr( o, p1+1, -1 ), _ );
							continue;
							}
						l = atoi( &(o[p2+1]) );
						if ( ( p0 < 0 )||( p0 >= strlen(v) ) )
							{
							strdcpy( inout, substr( o, 0, p ), substr( v, p0, -1 ), substr( o, p1+1, -1 ), _ );
							continue;
							}
							
						strdcpy( &o, substr( o, 0, p ), substr( v, p0, l ), substr( o, p1+1, -1 ), _ );
						}
					}
				}
			}
		free( ts ); ts = null;
		}

	if ( stripos( *inout, "$date_aaaa_mm_jj", 0 ) >= 0 )
		strd_replace( inout, "$date_aaaa_mm_jj", currdate( "aaaa_mm_jj" ) );
	if ( stripos( *inout, "$date_aaaa_mm_jj_hh_mm_ss", 0 ) >= 0 )
		strd_replace( inout, "$date_aaaa_mm_jj_hh_mm_ss", currdate( "aaaa_mm_jj_hh_mm_ss" ) );
	if ( stripos( *inout, "$date_jj_mm_aaaa", 0 ) >= 0 )
		strd_replace( inout, "$date_jj_mm_aaaa", currdate( "jj_mm_aaaa" ) );
	if ( stripos( *inout, "$date_jj/mm/aaaa", 0 ) >= 0 )
		strd_replace( inout, "$date_jj/mm/aaaa", currdate( "jj/mm/aaaa" ) );
	if ( stripos( *inout, "$date_hh:mm:ss", 0 ) >= 0 )
		strd_replace( inout, "$date_hh:mm:ss", currdate( "hh:mm:ss" ) );
	if ( stripos( *inout, "$date_hh:mm", 0 ) >= 0 )
		strd_replace( inout, "$date_hh:mm", currdate( "hh:mm" ) );

	#ifdef OS_WINDOWS
	if ( s ) free( s );
	#endif

	return *inout;
	}





//------------------------------------------------------------------------------
char * var_replace( handle hVar, char *in, char **out )
//------------------------------------------------------------------------------
	{
	#ifdef OS_WINDOWS
	char *v, *vname, c;
	int p, p0, p1, p2, p3, p4, px, l;
	#else
	static char *v, *vname, c;
	static int p, p0, p1, p2, p3, p4, px, l;
	#endif
	
	if ( *out ) (*out)[0] = 0;
	
	for( p = px = 0 ; ( p = strpos( in, "$$", p ) ) >= 0 ; )
		{
		// LECTURE DU NOM DE LA VARIABLE
		p1 = p + 2;
		for( p2 = p1 ; c = in[p2] ; p2++ )
			{
			if (( c >= 'a' )&&( c <= 'z' )) continue;
			if (( c >= 'A' )&&( c <= 'Z' )) continue;
			if (( c >= '0' )&&( c <= '9' )) continue;
			if (( c == '_' )||( c == '.' )) continue;
			if ( c == ';' ) p2++;
			
			break;
			}
		
		if ( p2 == p1 ) { p++; continue; }
		vname = substr( in, p1, p2-p1 );
		l = strlen( vname ) - 1;
		if ( !l ) { p++; continue; }
		if ( vname[l] == ';' ) vname[l] = 0;
		
		strdncat( out, &(in[px]), p-px, _ );
		
		
		// LECTURE POSITION, LONGUEUR
		p0 = 0; l = -1;
		
		if ( c == '[' )
			{
			p3 = strpos( in, "]", p2+1 );
			if ( p3 > 0 )
				{
				v = trim(substr( in, p2+1, p3-p2-1 ));
				p0 = atoi( v );
				p4 = strpos( v, ",", 0 );
				if ( p4 >= 0 )
					{
					v = trim(substr( v, p4+1, -1 ));
					l = atoi( v );
					}
				px = p3+1;
				}
			else
				px = p2;
			}
		else
			px = p2;
		
		
		// RECHERCHE VARIABLE
		
		if ( stricmp( vname, "sysdate" ) )
			{
			v = var_get( hVar, vname );
			}
		else
			v = itod( 0, "jj/mm/aaaa hh:mm:ss" );
			
		if ( v ) v = substr( v, p0, l );
			
		
		
		// REMPLACEMENT
		
		strdcat( out, v, _ );
		p = px;
		}
	
	strdncat( out, &(in[px]), -1, _ );

	return *out;
	}




















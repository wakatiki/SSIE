
#define base_name	(t_work->u1)

int bdoc_delete_children( handle hB, char *sId )
	{
	char *_w = null, *_w1 = null, *_w2 = null, *_w3 = null;
	int _p;
	
	// RECHERCHE DES ENFANTS
	
	strdcpy( &_w1, "folder=", sId, _ );
	if ( idat_select0( hB, _w1, &_w ) == 0 ) goto fin;

	
	// SUPPRESSION
	for( _p = 1 ; _w[_p] ; _p+=10 )
		{
		strdncpy( &_w1, &(_w[_p]), 9, _ );
		idat_getdir( hB, _w1, &_w2 );
		strdcat( &_w2, DIR_CHAR "TYPE", _ );
		idat_delete( hB, _w1, 10 );
		if ( file_load( &_w3, _w2 ) == sERROR ) continue;
		if (( _w3[1] != 'f' )&&( _w3[1] != 'F' )) continue;
		bdoc_delete_children( hB, _w1 );
		}
	
	
	fin:
		
	if ( _w )	free( _w );
	if ( _w1 )	free( _w1 );
	if ( _w2 )	free( _w2 );
	if ( _w3 )	free( _w3 );
	
	return OK;
	}


//------------------------------------------------------------------------------
int cmd_bdoc( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	// DETERMINE LE DEMANDEUR (POUR LES HABILITATIONS)
	if ( request_mode == MODE_TCP )
		{
		if ( session_partname )
			{
			if ( stricmp( user, "EVERYBODY" ) )
				strdcpy( &user, session_partname, _ );
			}
		else
		if ( !strcmp( ip_cli, "127.0.0.1" ) )
			strdcpy( &user, var_get( hSrvConf, "partname" ), _ );
		else
			strdcpy( &user, "EVERYBODY", _ );
		}
	else
		strdcpy( &user, var_get( hSrvConf, "partname" ), _ );

		
//print( "user=", user, BR, _ );
	
	// LECTURE NOM DE LA BASE
	
	v = var_getn( hReq, 1, &vn );
	if ( !v )
		{
		// AFFICHE LA LISTE DES BASES
		strdcpy( &fname, dirconf, DIR_CHAR "bdoc" DIR_CHAR "*.dat", _ );
		tf = NULL; b = false;
		print( "[", _ );
		while( tf = file_list( tf, fname, "absdir nodir" ) )
			{
			var_free( h );
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			file_load( &s, fname );
			if ( s[0] == '@' )
				strcpy( &(fname[strlen(fname)-3]), "cfg" );
			else
				strdcpy( &fname, s, DIR_CHAR "bdoc.cfg", _ );
			h = var_load( fname );
			tf->name[strlen(tf->name)-4] = 0;
			print( (char*)((b)?",":""), "['", tf->name, "',", quote(var_get( h, "title" )), "]", _ );
			b = true;
			}
		print( "]", _ );
			
		return OK;
		}
	

	//--------------------------------------------------------------------------------------
	if ( !strcmp( vn, "purge" ) )
	//--------------------------------------------------------------------------------------
		{
		strdcpy( &fname, dirconf, DIR_CHAR "bdoc" DIR_CHAR "*.dat", _ );
		tf = NULL; b = false;
		n = 0;
		while( tf = file_list( tf, fname, "absdir nodir" ) )
			{
			var_free( h );
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			file_load( &s, fname );
			if ( s[0] == '@' )
				strcpy( &(fname[strlen(fname)-3]), "cfg" );
			else
				strdcpy( &fname, s, DIR_CHAR "bdoc.cfg", _ );
			h = var_load( fname );
			v = var_get( h, "dirdat" );
			if (( !v )||( !(v[0]) )) continue;
			
			print( fname, " : ", v,  BR, _ );
			
			tf2 = NULL;
			strdcpy( &fname, v, DIR_CHAR "data" DIR_CHAR "deleted", _ );
			while( tf2 = file_list( tf2, fname, "absdir nodir recursif" ) )
				{
				print( tf2->dir, BR, _ );
				file_delete( tf2->dir );
				n++;
				}
			
			}
		print( BR, itoa(n), " documents supprimés" BR, _ );
		return OK;
		}
		

	strdcpy( &fname1, dirconf, DIR_CHAR "bdoc" DIR_CHAR, vn, ".dat", _ );
	if ( file_load( &s1, fname1 ) != sERROR )
		{
		if ( b = (s1[0] == '@') )
			strdncpy( &fname, fname1, strlen(fname1)-3, "cfg", 3, _ );
		else
			strdcpy( &fname, s1, DIR_CHAR "bdoc.cfg", _ );
		
		var_free( h );
		h = var_load( fname );
		}
	else
		h = var_new();

	
	// LECTURE ACTION

	v = var_getn( hReq, 2, &vn );
	if ( !v )
		{
		// AFFICHE INFOS BASE
		if ( !file_exists( fname1 ) )
			{
			print( "ERREUR : base bdoc ", vn, " inexistante.", _ );
			return ERROR;
			}
		
		print( "bdoc base : ", vn, BR,
				"   title : ", var_get( h, "title" ), BR
				"   body  : ", var_get( h, "body" ), BR
				"   dirdat: ", var_get( h, "dirdat" ), BR
				"   rights: ", var_get( h, "rights" ), BR
				, _ );
		
		return OK;
		}
	
	strdcpy( &s, lower(vn), _ );
	strd_replace( &s, "_", "" );
	
	//------------------------------------------------------------------------------
	if ( ! strcmp( s, "basesave" ) )
	//------------------------------------------------------------------------------
		{
		if ( v = var_get( hReq, "dirdat" ) )
			{
			file_save_str( fname1, v, -1 );

			if ( !( b = (v[0] == '@') ) )
				{ // BASE LOCALE
				strdcpy( &fname, v, DIR_CHAR "bdoc.cfg", _ );
				var_free( h );
				h = var_load( fname );
				var_set( h, "dirdat", v );
				}
			else
				{ // BASE DISTANTE
				strdncpy( &fname, fname1, strlen(fname1)-3, "cfg", 3, _ );
				}
			}
		
		if ( !b ) // BASE LOCALE => Sauvegarde des info
			{
			var_set( h, "dt_upd", v1 = itoa(sysdate) );
			if ( !var_get( h, "dt_crea" ) )
				{
				var_set( h, "dt_crea", v1 );
				var_set( h, "idx_fields", "folder,type,class" );
				var_set( h, "pv_fields", "type,title,dt_upd,class" );
				idat_create( h );
				}
			
			bdoc_save:
			if ( v = var_get( hReq, "title" ) )		var_set( h, "title", v );
			if ( v = var_get( hReq, "body" ) )		var_set( h, "body", v );
			if ( v = var_get( hReq, "backup" ) ) 	var_set( h, "backup", v );
			if ( v = var_get( hReq, "rights" ) ) {	var_set( h, "rights", v ); b = true; }
			
			var_set( h, "id", "0" );
			var_set( h, "type", "folder" );
			var_save( h, fname );

			if ( b )
				{
				strdcpy( &fname, s1, DIR_CHAR "bdoc.cfg", _ );
				hI = idat_open( fname );
bdoc_save_rights( hI, h );

				idat_close( hI );
				}
			
			print( "OK", _ );
			return OK;
			}
		else
			{ // BASE DISTANTE => DEMANDE LES INFO SUR LA BASE POUR ENREGISTREMENT EN LOCALE
			if ( v = var_get( hReq, "title" ) ) var_set( h, "title", v );
			if ( v = var_get( hReq, "body" ) ) var_set( h, "body", v );
			var_save( h, fname );
			return OK;
			}
		}
	
	if ( b )
		{ // BASE DISTANTE => TRANSMET LA REQUETE
		
		p = strpos( s1, ":", 0 );
		if ( p < 0 )
			{
			print( "ERREUR : Mauvaise définition du partenaire distant", _ );
			return ERROR;
			}
			
		strdncpy( &partname, &(s1[1]), p-1, _ );

		p1 = strpos( cmd, " ", 0 );
		p1 = strpos( cmd, " ", p1+1 );
		
		strdcpy( &cmd1, "bdoc ", &(s1[p+1]), &(cmd[p1]), " user=", user, _ );

		// CONNEXION
			
		if ( ssie7connect( t_work, false ) == ERROR )
			{
			// PROBLEME DE CONNEXION
			print( "ERREUR : ", error_msg, _ );
			return ERROR;
			}

		
		// TRANSMET LA COMMANDE

		if ( tcp_ssend( hCnx, cmd1, -1 ) == ERROR )
			{
			ssie7disconnect( t_work );
			print( "ERREUR : Connexion perdue", _ );
			return ERROR;
			}
		
		if ( ( n = tcp_srecv( hCnx, &s ) ) == ERROR )
			{
			ssie7disconnect( t_work );
			print( "ERREUR : Connexion perdue", _ );
			return OK;
			}
				
		print( s, _ );
		
		// DECONNEXION
		ssie7disconnect( t_work );
		return OK;
		}
	
	//------------------------------------------------------------------------------
	if ( ! strcmp( s, "docsave" ) )
	//------------------------------------------------------------------------------
		{
		v = var_get( hReq, "id" );
		if (( v )&&(( !strcmp( v, "0" ) )||( !strcmp( v, "1" ) )))
			{
			b = false;
			goto bdoc_save;
			}

		hI = idat_open( fname );
		if ( v )
			{
			h2 = idat_load( hI, v, h2 );
			}
		else
			h2 = var_new();
		
		p = strpos( cmd, " ", 0 );
		p = strpos( cmd, " ", p+1 );
		p = strpos( cmd, " ", p+1 );
		if ( p < 0 ) p = 0;
		
		h2 = var_from_cmd( h2, &(cmd[p]) );
		
		if ( v )
			{
			idat_update( hI, h2, v );
			print( v, _ );
			}
		else
			{
			idat_insert( hI, h2, &s, true );
			print( s, _ );
			v = s;
			}

bdoc_save_rights( hI, h2 );

		idat_close( hI );

		return OK;
		}
	
	
	//------------------------------------------------------------------------------
	if ( ! strcmp( s, "docload" ) )
	//------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 3, &vn );
		if ( !v )
			{
			print( "ERREUR : bdoc {basename} docload {doc_id}", _ );
			return ERROR;
			}

		if (( !strcmp( vn, "0" ) )||( !strcmp( vn, "1" ) ))
			{
			print( "{", var_to_js_array( h, &s ), ",part_roles:''}", _ );
			return OK;
			}
			
		hI = idat_open( fname );
		
		h2 = idat_load( hI, vn, h2 );
		if ( h2 == hNULL )
			{
			print( "ERREUR : élément inexistant.", _ );
			}
		else
			{
			print( "{", var_to_js_array( h2, &s ), ",part_roles:", _ );
			
			idat_getdir( hI, vn, &fname );
			strdcat( &fname, DIR_CHAR "ROLES_*", _ );
			strdcpy( &s, "", _ );
			tf = NULL;
			while( tf = file_list( tf, fname, "absdir nodir" ) )
				{
				strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
				file_load( &w, fname );
				strdcat( &s, "<b>", &(tf->name[6]), ":</b>", w, "<br>", _ );
				}
			
			print( quote(s), "}", _ );
			}
		
		idat_close( hI );
		return OK;
		}
	
	
	//------------------------------------------------------------------------------
	if ( ! strcmp( s, "docdel" ) )
	//------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 3, &vn );
		if ( !v )
			{
			print( "ERREUR : bdoc {basename} docdel {type F ou D}{id1} {type}{id2} ...", _ );
			return ERROR;
			}

		
		hI = idat_open( fname );
		
		for( i = 3 ; var_getn( hReq, i, &vn ) ; i++ )
			{
			idat_delete( hI, &(vn[1]), 10 );
//print( "docdel:", vn, BR, _ );
			if ( vn[0] == 'F' )
				bdoc_delete_children( hI, &(vn[1]) );
			}
		
		idat_close( hI );
		
		print( "OK", _ );
		return OK;
		}
	
	
	//------------------------------------------------------------------------------
	if ( ! strcmp( s, "docmove" ) )
	//------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 3, &vn );
		if ( !v )
			{
			print( "ERREUR : bdoc {basename} docmove {new_folder_id} {id1} {id2} ...", _ );
			return ERROR;
			}

		
		hI = idat_open( fname );
		
		for( i = 4 ; var_getn( hReq, i, &v ) ; i++ )
			{
			h2 = idat_load( hI, v, h2 );
			if ( h2 == hNULL ) continue;
			var_set( h2, "folder", vn );
			idat_update( hI, h2, v );
			}
		
		idat_close( hI );
		
		print( "OK", _ );
		return OK;
		}
	
	
	//------------------------------------------------------------------------------
	if ( ! strcmp( s, "doclist" ) )
	//------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 3, &vn );
		if ( !v )
			{
			print( "ERREUR : bdoc {basename} doclist '{cond}'", _ );
			return ERROR;
			}

		
		hI = idat_open( fname );

		strdcpy( &s1, "type,title,class,roles_", user, _ );
		idat_select( hI, vn, s1, &s );

//print( "s=", s, BR, _ );

		if (( p = strpos( vn, "folder=0", 0 ) >= 0 )
			&&(( vn[p+8] < '0' )||( vn[p+8] > '9' ))
			)
			{
			// CHARGE LES DROITS DU DEMANDEUR
			idat_getdir( hI, "000000000", &w );
			strdcat( &w, DIR_CHAR "ROLES_", upper(user), _ );
			strdcpy( &s2, "", _ );
			file_load( &s2, w );
			if ( strpos( s2, ";READ;", 0 ) < 0 )
				{
				print( "Vous n'êtes pas habilité pour accéder à cette base.", _ );
				}
			else
				{
				strdcpy( &s1, "[['000000000','", s2, "']", _ );
				if ( strlen( s ) <= 4 )
					{
					strdcat( &s1, "]", _ );
					}
				else
					strdcat( &s1, ",", &(s[1]), _ );
				
				print( s1, _ );
				}
			}
		else
			print( s, _ );
		
		idat_close( hI );
		
		return OK;
		}
	
	
	//------------------------------------------------------------------------------
	if ( ! strcmp( s, "docup" ) )
	//------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 3, &v1 );
		if ( !v )
			{
			print( "ERREUR : bdoc {basename} docup {folder_id} {id1} ...", _ );
			return ERROR;
			}

		strdcpy( &s1, "", _ );
		for( i = 4 ; var_getn( hReq, i, &vn ) ; i++ )
			strdcat( &s1, (char*)((s1[0])?",":""), vn, _ );

		hI = idat_open( fname );
		
		if ( s1[0] )
			idat_idx_up( hI, "folder", v1, s1 );
			
		strdcpy( &s1, "folder=", v1, _ );
		idat_select( hI, s1, "type,title", &s );
		print( s, _ );
		
		idat_close( hI );
//		hI = hNULL;
		
		return OK;
		}
	
	//------------------------------------------------------------------------------
	if ( ! strcmp( s, "docdown" ) )
	//------------------------------------------------------------------------------
		{
		v = var_getn( hReq, 3, &v1 );
		if ( !v )
			{
			print( "ERREUR : bdoc {basename} docdown {folder_id} {id1} ...", _ );
			return ERROR;
			}

		strdcpy( &s1, "", _ );
		for( i = 4 ; var_getn( hReq, i, &vn ) ; i++ )
			strdcat( &s1, (char*)((s1[0])?",":""), vn, _ );

		hI = idat_open( fname );
		
		if ( s1[0] )
			idat_idx_down( hI, "folder", v1, s1 );
			
		strdcpy( &s1, "folder=", v1, _ );
		idat_select( hI, s1, "type,title", &s );
		print( s, _ );
		
		idat_close( hI );
//		hI = hNULL;
		
		return OK;
		}
	
	//------------------------------------------------------------------------------
	if ( ! strcmp( s, "doctypload" ) )
	//------------------------------------------------------------------------------
		{
		hI = idat_open( fname );
		
		strdcpy( &s1, "folder=1", _ );
		idat_select( hI, s1, "title", &s );
		if ( strlen( s ) < 4 )
			{
			print( s, _ );
			idat_close( hI );
			hI = hNULL;
			return;
			}
		strd_replace( &s, "],[", ";" );
		s[strlen(s)-2] = 0;

		print( "[", _ );
		ts = str_explode( &(s[2]), ";" );
		for( i = 0 ; ts[i] ; i++ )
			{
			strdncpy( &s, (char*)((i)?",":""), -1, "{id:", 4, ts[i], 12,
						"title:", 6, &(ts[i][12]), -1,
						_ );
			print( s, _ );
			
			// Lecture des éléments
			
			strdncpy( &s1, "folder=", 7, ts[i], 12, _ );
			idat_select( hI, s1, "title", &s );
			if ( strlen(s) < 4 )
				{
				print( "}", _ );
				continue;
				}
			strd_replace( &s, "],[", ";" );
			s[strlen(s)-2] = 0;
			ts1 = str_explode( &(s[2]), ";" );
			for( j = 0 ; ts1[j] ; j++ )
				{
				strdncpy( &s, &(ts1[j][13]), strlen(&(ts1[j][13]))-1, _ );
				if ( (p = strpos( s, ":", 0 )) >= 0 ) s[p] = 0;
				print( ",", lower(trim(s)), "::", _ );
				strdncpy( &s1, &(ts1[j][1]), 9, _ );
				h = idat_load( hI, s1, h );
				print( quote(var_get( h, "body" )), _ );
				}
			
			// fin
			print( "}", _ );
			}
		
		print( "]", _ );
		
		idat_close( hI );
//		hI = hNULL;
		
		return OK;
		}
	}


/*
//------------------------------------------------------------------------------
int bdoc_save_rights( T_WORK *t_work, handle hB, char *rights, char *dir_data )
//------------------------------------------------------------------------------
	{
	// MET A JOUR LES DROITS D'UN DOCUMENT
logw( "bdoc_save_rights : ", rights, ", dir_data=", dir_data, _ );

	if ( !rights ) return true;
	
	var_free( h3 );
	h3 = var_new();
	h2 = var_from_cmd( 0, rights );
	for( i = 0 ; v = var_getn( h2, i, &vn ) ; i++ )
		{
		upper( vn );
		ts = str_explode( upper(v), "," );
		for( j = 0 ; ts[j] ; j++ )
			{
			if ( ts[j][0] == 0 ) continue;
			v = var_get( h3, ts[j] );
			if ( v ) strdcpy( &s, v, vn, ";", _ );
			else strdcpy( &s, ";", vn, ";", _ );
			var_set( h3, ts[j], s );
logw( "bdoc_save_rights : ", ts[j], " = ", s, _ );
			}
		}
	
	for( i = 0 ; v = var_getn( h3, i, &vn ) ; i++ )
		{
		strdcpy( &w, dir_data, DIR_CHAR "ROLES_", vn, _ );
		file_save_str( w, v, -1 );
		}
	}
*/	

//------------------------------------------------------------------------------
int bdoc_save_rights( handle hB, handle hData )
//------------------------------------------------------------------------------
	{
	int _i, _j, _k;
	char *_v, *_v1, *_vn, **_ts, *_s = null, *_w = null, *_w1 = null, *dir_data = null, _b, _b1;
	handle _h = hNULL, _h1 = hNULL, _h2 = hNULL, _h3 = hNULL;
	T_FILE_INFO *_tf = NULL;
	
	// MET A JOUR LES DROITS D'UN DOCUMENT
	
	_v = var_get( hData, "id" );
	if ( ! strcmp( _v, "0" ) ) _v = "000000000";
	idat_getdir( hB, _v, &dir_data );
	
	// CHARGE DES DROITS HERITES
	
	strdcpy( &_w, dir_data, DIR_CHAR "ROLES_INHERIT", _ );
	if ( file_load( &_w1, _w ) != sERROR )
		_h1 = var_from_cmd( 0, _w1 );
	else
		{
		_v = var_get( hData, "folder" );
		if ( _v )
			{
			idat_getdir( hB, _v, &_w );
			strdcpy( &_w1, _w, DIR_CHAR "ROLES_INHERIT", _ );
			strdcpy( &_s, "", _ );
			file_load( &_s, _w1 );
			strdcpy( &_w1, _w, DIR_CHAR "data", _ );
			_h1 = var_load( _w1 );
			_v = var_get( _h1, "rights" );
			strdcat( &_s, " ", _v, _ );
			var_free( _h1 );
			_h1 = var_from_cmd( 0, _s );
			strdcpy( &_w1, dir_data, DIR_CHAR "ROLES_INHERIT", _ );
			var_to_str( _h1, &_w );
			file_save_str( _w1, _w, -1 );
			}
		else
			_h1 = var_new();
		}
		
	// CHARGE DES DROITS DU DOC EN TENANT COMPTE DE L'HERITAGE

	_v = var_get( hData, "rights" );
	if (( _v )&&( _v[0] ))
		_h = var_from_cmd( var_dup(_h1), _v );
	else
		_h = var_dup(_h1);
	
//logw( "bdoc_save_rights 1 : ", var_get( hData, "id" ), ", h = ", var_to_str( _h, &_w ), _ );

		
	// MISE A JOUR DES GROUPES ET HERITAGE (INHERIT)

	for( _k = 0 ; _k < 100 ; _k++ ) // _k évite les boucle infinie
		{
		_b = false;
		for( _i = 0 ; _v = var_getn( _h, _i, &_vn ) ; _i++ )
			{
			trim( _v );
			if ( _v[0] == 0 ) continue;
			strdcpy( &_w, (char*)(( _v[0] != ',' )?",":""), _v,
					(char*)(( _v[strlen(_v)-1] != ',' )?",":""),
					_ );
			strd_replace( &_w, " ", "" );
//logw( "bdoc_save_rights a : ", var_get( hData, "id" ), " : ", _vn, "=", _w, _ );
			
			_ts = str_explode( upper(_w), "," );
			_b1 = false;
			for( _j = 0 ; _ts[_j] ; _j++ )
				{
				if ( _ts[_j][0] == 0 ) continue;
				if ( !stricmp( _ts[_j], "inherit" ) )
					{
					_v = var_get( _h1, _vn );
					if ( !_v ) _v = ",";
					strdcpy( &_s, (char*)(( _v[0] != ',' )?",":""), _v,
							(char*)(( _v[strlen(_v)-1] != ',' )?",":""),
							_ );
					strd_replace( &_s, " ", "" );
					
					strd_ireplace( &_w, ",inherit,", _s );
					
//logw( "bdoc_save_rights b : ", var_get( hData, "id" ), " : ", _vn, "=", _w, _ );
					_b = _b1 = true;


					continue;
					}
				if ( _ts[_j][0] == '-' ) // Exclusion
					_v = var_get( _h, &(_ts[_j][1]) );
				else
					_v = var_get( _h, _ts[_j] );
				if (!_v ) continue;
				if ( !(_v[0]) ) _v = ",";
				strdcpy( &_s, (char*)(( _v[0] != ',' )?",":""), _v,
						(char*)(( _v[strlen(_v)-1] != ',' )?",":""),
						_ );
				strd_replace( &_s, " ", "" );
				if ( _ts[_j][0] == '-' )
					{
					strd_replace( &_s, ",", ",-" );
					_k =strlen( _s ) - 1;
					if ( _s[_k] == '-' ) _s[_k] = 0;
					}
					
				strdcpy( &_w1, ",", _ts[_j], ",", _ );
				strd_ireplace( &_w, _w1, _s );
//logw( "bdoc_save_rights c : ", var_get( hData, "id" ), " : ", _vn, "=", _w, _ );
				_b = _b1 = true;
				}
			if ( _b1 )
//{logw( "bdoc_save_rights d : ", var_get( hData, "id" ), " : ", _vn, "=", _w, _ );

						var_set( _h, _vn, _w );
//}
			}
		if ( !_b ) break;
		}
	
//logw( "bdoc_save_rights 2 : ", var_get( hData, "id" ), ", h = ", var_to_str( _h, &_w ), _ );
	

	// CALCUL LES DROITS PAR PARTENAIRE
	
	_h3 = var_new();
	for( _i = 0 ; _v = var_getn( _h, _i, &_vn ) ; _i++ )
		{
		upper( _vn );
		_ts = str_explode( upper(_v), "," );
		for( _j = 0 ; _ts[_j] ; _j++ )
			{
			if ( _ts[_j][0] == 0 ) continue;
			if ( _ts[_j][0] == '-' ) // Exclusion
				{
				_v = var_get( _h3, &(_ts[_j][1]) );
				if ( !_v ) continue;
				// SUPPRIME LE DROIT
				strdcpy( &_s, _v, _ );
				strdcpy( &_w1, ";", _vn, ";", _ );
				strd_replace( &_s, _w1, ";" );
				if ( strlen( _s ) < 3 )
					{
					var_delete( _h3, &(_ts[_j][1]) );
					continue;
					}
				var_set( _h3, &(_ts[_j][1]), _s );
				}
			else
				{
				_v = var_get( _h3, _ts[_j] );
				if ( _v ) strdcpy( &_s, _v, _vn, ";", _ );
				else strdcpy( &_s, ";", _vn, ";", _ );
				var_set( _h3, _ts[_j], _s );
				}
			}
		free( _ts );
		}

//logw( "bdoc_save_rights 3 : ", var_get( hData, "id" ), ", h3 = ", var_to_str( _h3, &_w ), _ );
	
	// SAUVEGARDE DANS LES FICHIERS ROLES_{partname}
	for( _i = 0 ; _v = var_getn( _h3, _i, &_vn ) ; _i++ )
		{
		strdcpy( &_w, dir_data, DIR_CHAR "ROLES_", _vn, _ );
//logw( "bdoc_save_rights 4 : ", var_get( hData, "id" ), ", w = ", _w, _ );
		file_save_str( _w, _v, -1 );
		}
	

	// SUPPRIME LES ROLES DISPARUS
	strdcpy( &_w, dir_data, DIR_CHAR "ROLES_*", _ );
	while( _tf = file_list( _tf, _w, "absdir nodir" ) )
		{
		if ( !strcmp( _tf->name, "ROLES_INHERIT" ) ) continue;
		if ( var_get( _h3, &(_tf->name[6]) ) ) continue;
		strdcpy( &_w, _tf->dir, DIR_CHAR, _tf->name, _ );
		file_delete( _w );
		}	


	// MISE A JOUR RECURSIVE
	_v = var_get( hData, "type" );
	if ( ( _v )&&( !stricmp( _v, "folder" ) ) )
		{
		var_to_str( _h, &_w1 );
		_v = var_get( hData, "id" );
		_b = strcmp( _v, "0" );
		loop1:
		strdcpy( &_w, "folder=", _v, _ );

		idat_select( hB, _w, "", &_s );
		if ( strlen(_s) < 4 ) goto suite;
		strd_replace( &_s, "],[", ";" );
		_s[strlen(_s)-2] = 0;
		_ts = str_explode( &(_s[2]), ";" );
		for( _i = 0 ; _ts[_i] ; _i++ )
			{
			_ts[_i][10] = 0;
			idat_getdir( hB, &(_ts[_i][1]), &dir_data );
			strdcpy( &_w, dir_data, DIR_CHAR "ROLES_INHERIT", _ );
			file_save_str( _w, _w1, -1 );
			strdcpy( &_w, dir_data, DIR_CHAR "data", _ );
			var_free( _h2 );
			_h2 = var_load( _w );
			bdoc_save_rights( hB, _h2 );
			}
		
		suite:
		if ( !_b ) // Maj folder 0 => Maj folder 1
			{
			_b = true;
			_v = "1";
			goto loop1;
			}
			
		}
		
	fin:
		
	if ( _h ) var_free( _h );
	if ( _h1 ) var_free( _h1 );
	if ( _h2 ) var_free( _h2 );
	if ( _h3 ) var_free( _h3 );
	
	if ( _s ) free( _s );
	if ( _w ) free( _w );
	if ( _w1 ) free( _w1 );
	if ( dir_data ) free( dir_data );
	}
	














#define IDAT_VERSION "1.1"

//------------------------------------------------------------------------------
bool idat_create( handle hConf )
//------------------------------------------------------------------------------
	{
	char *w = null, **ts, *dirdat;
	int i;
	
	var_set( hConf, "idat_version", IDAT_VERSION );
	
	// PREPARATION DES FICHIERS ET REPERTOIRES
	
	dirdat = var_get( hConf, "dirdat" );
	strdcpy( &w, dirdat, DIR_CHAR "data", _ );
	mkdir( w );
	
	ts = str_explode( var_get( hConf, "idx_fields" ), "," );
	for( i = 0 ; ts[i] ; i++ )
		{
		if ( ts[i][0] == 0 ) continue;
		strdcpy( &w, dirdat, DIR_CHAR "index" DIR_CHAR, upper(ts[i]), _ );
		mkdir( w );
		}
	
	free( ts );
	free( w );
	
	return true;
	}


//------------------------------------------------------------------------------

typedef struct
	{
	handle	hConf, hoData;
	HANDLE	hLock;
	char	*dirdat;
	char	**ts_idx_fields;
	char	**ts_u_fields;
	char	**ts_pv_fields;
	char	**ts;
	char	*w, *w1, *w2, *w3, *w4;
	char	*v, *v1, *v2;
	int		i, j, n, p, p1, p2;
	}
	T_ELIB_IDAT;

#define hConf				(((T_ELIB_IDAT *)hI)->hConf)
#define hoData				(((T_ELIB_IDAT *)hI)->hoData)
#define hLock				(((T_ELIB_IDAT *)hI)->hLock)
#define dirdat				(((T_ELIB_IDAT *)hI)->dirdat)
#define ts_idx_fields		(((T_ELIB_IDAT *)hI)->ts_idx_fields)
#define ts_u_fields			(((T_ELIB_IDAT *)hI)->ts_u_fields)
#define ts_pv_fields		(((T_ELIB_IDAT *)hI)->ts_pv_fields)
#define ts					(((T_ELIB_IDAT *)hI)->ts)
#define w					(((T_ELIB_IDAT *)hI)->w)
#define w1					(((T_ELIB_IDAT *)hI)->w1)
#define w2					(((T_ELIB_IDAT *)hI)->w2)
#define w3					(((T_ELIB_IDAT *)hI)->w3)
#define w4					(((T_ELIB_IDAT *)hI)->w4)
#define v					(((T_ELIB_IDAT *)hI)->v)
#define v1					(((T_ELIB_IDAT *)hI)->v1)
#define v2					(((T_ELIB_IDAT *)hI)->v2)
#define i					(((T_ELIB_IDAT *)hI)->i)
#define j					(((T_ELIB_IDAT *)hI)->j)
#define n					(((T_ELIB_IDAT *)hI)->n)
#define p					(((T_ELIB_IDAT *)hI)->p)
#define p1					(((T_ELIB_IDAT *)hI)->p1)
#define p2					(((T_ELIB_IDAT *)hI)->p2)


//------------------------------------------------------------------------------
handle idat_open( char *conf_fname )
//------------------------------------------------------------------------------
	{
	T_ELIB_IDAT *t_wrk;
	handle hI;
	
	t_wrk = (T_ELIB_IDAT *)alloc0( sizeof(T_ELIB_IDAT) );
	hI = (handle)t_wrk;
	
	hConf = var_load( conf_fname );
	dirdat = var_get( hConf, "dirdat" );
	ts_idx_fields = str_explode( upper(var_get( hConf, "idx_fields" )), "," );
	v = var_get( hConf, "u_fields" );
	if ( v )
		ts_u_fields = str_explode( upper(v), "," );
	v = var_get( hConf, "pv_fields" );
	if ( v )
		ts_pv_fields = str_explode( upper(v), "," );
	
	return hI;
	}



//------------------------------------------------------------------------------
bool idat_close( handle hI )
//------------------------------------------------------------------------------
	{
	free( ts_idx_fields );
	free( ts_u_fields );
	if ( w ) free( w );
	if ( w1 ) free( w1 );
	if ( w2 ) free( w2 );
	var_free( hConf );
	var_free( hoData );
	
	free( (void*)hI );
	}



//------------------------------------------------------------------------------
bool idat_insert( handle hI, handle hData, char **sId, char bComputeId )
//------------------------------------------------------------------------------
	{
	// VERIFIE UNICITE
	
	// CALCUL UN NOUVEL ID
	
	if ( bComputeId )
		{
		strdcpy( &w, dirdat, DIR_CHAR "idat.id", _ );
		strdcpy( sId, itona( getuid( w ), 9 ), _ );
		}
	
	// SAUVEGARDE DES DONNEES
	
	strdncpy( &w, dirdat, -1, DIR_CHAR "data" DIR_CHAR, -1,
				*sId, 3, DIR_CHAR, 1, &((*sId)[3]), 3, DIR_CHAR, 1,
				&((*sId)[6]), 3, _ );
	
	mkdir( w );
	strdcat( &w, DIR_CHAR "data" );
	
	var_set( hData, "id", *sId );
	var_set( hData, "dt_crea", (v=itoa(n=sysdate)) );
	var_set( hData, "dt_upd", v );
	var_set( hData, "dt_crea_day", v = itod( n, "aaaa_mm_jj" ) );
	var_set( hData, "dt_upd_day", v );
	
	if ( ! var_save( hData, w ) ) return false;
	
	if ( ts_pv_fields )
		{
		w[strlen(w)-4] = 0;
		for( i = 0 ; ts_pv_fields[i] ; i++ )
			{
			if ( !(ts_pv_fields[i][0]) ) continue;
			v = var_get( hData, ts_pv_fields[i] );
			strdcpy( &w1, w, ts_pv_fields[i], _ );
			file_save_str( w1, quote(v), -1 );
			}
		}
	
	// INDEXATION
	
	for( i = 0 ; ts_idx_fields[i] ; i++ )
		{
		if ( !(ts_idx_fields[i][0]) ) continue;
		
		v = upper(var_get( hData, ts_idx_fields[i] ));
		if ( v == null ) v = "NULL";
		
		strdcpy( &w, dirdat, DIR_CHAR "index" DIR_CHAR, ts_idx_fields[i], DIR_CHAR, v, ".lock", _ );
		hLock = file_lock( w );
		w[n=(strlen(w)-5)] = 0;

		if ( file_size( w ) > 0 )
			strdcpy( &w1, *sId, ";", _ );
		else
			strdcpy( &w1, ";", *sId, ";", _ );

		if ( file_append( w, w1, -1 ) == ERROR )
			{
			mkdir( file_getdir( w ) );
			if ( file_append( w, w1, -1 ) == ERROR )
				{
				strdcpy( &error_msg, "ERREUR : Pb indexation, pb création fichier ", w, _ );
				}
			}

		file_unlock( hLock );
		w[n] = '.';
		unlink( w );
		}
	
	return true;
	}



//------------------------------------------------------------------------------
bool idat_update( handle hI, handle hData, char *sId )
//------------------------------------------------------------------------------
	{
	// VERIFIE UNICITE
	
	// CHARGE ANCIENNE VALEUR

	strdncpy( &w, dirdat, -1, DIR_CHAR "data" DIR_CHAR, -1,
				sId, 3, DIR_CHAR, 1, &((sId)[3]), 3, DIR_CHAR, 1,
				&((sId)[6]), 3, DIR_CHAR "data", -1, _ );

	if ( !file_exists( w ) ) return false;
	
	var_free( hoData );
	hoData = var_load( w );
	
	var_set( hData, "dt_upd", itoa(n=sysdate) );
	var_set( hData, "dt_upd_day", itod( n, "aaaa_mm_jj" ) );
	
	
	// SAUVEGARDE DES DONNEES
	
	if ( ! var_save( hData, w ) ) return false;
	
	if ( ts_pv_fields )
		{
		w[strlen(w)-4] = 0;
		for( i = 0 ; ts_pv_fields[i] ; i++ )
			{
			if ( !(ts_pv_fields[i][0]) ) continue;
			v = var_get( hData, ts_pv_fields[i] );
			strdcpy( &w1, w, ts_pv_fields[i], _ );
			file_save_str( w1, quote(v), -1 );
			}
		}
	
	
	// INDEXATION DES DIFFERENCES
	
	for( i = 0 ; ts_idx_fields[i] ; i++ )
		{
		if ( !(ts_idx_fields[i][0]) ) continue;
		
		v = upper(var_get( hData, ts_idx_fields[i] ));
		if ( v == null ) v = "NULL";
		v1 = upper(var_get( hoData, ts_idx_fields[i] ));
		if ( v1 == null ) v1 = "NULL";
		
		if ( ! strcmp( v, v1 ) ) continue;


		// DESINDEXE ANCIENNE VALEUR

		strdcpy( &w, dirdat, DIR_CHAR "index" DIR_CHAR, ts_idx_fields[i], DIR_CHAR, v1, ".lock", _ );
		hLock = file_lock( w );
		w[n=(strlen(w)-5)] = 0;
		
		strdcpy( &w1, ";", sId, ";", _ );
		if ( file_load( &w2, w ) != sERROR )
			{
			strd_replace( &w2, w1, ";" );
			file_save_str( w, w2, -1 );
			}
		
		file_unlock( hLock );
		w[n] = '.';
		unlink( w );
		
		
		//  NOUVELLE INDEXATION
		
		strdcpy( &w, dirdat, DIR_CHAR "index" DIR_CHAR, ts_idx_fields[i], DIR_CHAR, v, ".lock", _ );
		hLock = file_lock( w );
		w[n=(strlen(w)-5)] = 0;
		
		if ( file_size( w ) > 0 ) p = 1; else p = 0;
		if ( file_append( w, &(w1[p]), -1 ) == ERROR )
			{
			mkdir( file_getdir( w ) );
			if ( file_append( w, &(w1[p]), -1 ) == ERROR )
				{
				strdcpy( &error_msg, "ERREUR : Pb indexation, pb création fichier ", w, _ );
				}
			}

		file_unlock( hLock );
		w[n] = '.';
		unlink( w );
		}
	
	return true;
	}



//------------------------------------------------------------------------------
bool idat_delete( handle hI, char *sId, int HistoryDelay )
//------------------------------------------------------------------------------
	{
	// CHARGE L'ELEMENT
	
	strdncpy( &w, dirdat, -1, DIR_CHAR "data" DIR_CHAR, -1,
				sId, 3, DIR_CHAR, 1, &((sId)[3]), 3, DIR_CHAR, 1,
				&((sId)[6]), 3, DIR_CHAR "data", -1, _ );

	if ( !file_exists( w ) ) return false;
	
	var_free( hoData );
	hoData = var_load( w );
	
	// DESINDEXATION
	
	for( i = 0 ; ts_idx_fields[i] ; i++ )
		{
		if ( !(ts_idx_fields[i][0]) ) continue;
		
		v1 = upper(var_get( hoData, ts_idx_fields[i] ));
		if ( v1 == null ) v1 = "NULL";

		strdcpy( &w, dirdat, DIR_CHAR "index" DIR_CHAR, ts_idx_fields[i], DIR_CHAR, v1, ".lock", _ );
		hLock = file_lock( w );
		w[n=(strlen(w)-5)] = 0;
		
		strdcpy( &w1, ";", sId, ";", _ );
		file_load( &w2, w );
		strd_replace( &w2, w1, ";" );
		file_save_str( w, w2, -1 );
		
		file_unlock( hLock );
		w[n] = '.';
		unlink( w );
		}
	
	
	// MARQUE LE FICHIER COMME SUPPRIME

	strdncpy( &w, dirdat, -1, DIR_CHAR "data" DIR_CHAR, -1,
				sId, 3, DIR_CHAR, 1, &((sId)[3]), 3, DIR_CHAR, 1,
				&((sId)[6]), 3, DIR_CHAR "deleted", -1, _ );
	file_save_str( w, itod( sysdate, "flog" ), -1 );
	}



//------------------------------------------------------------------------------
handle idat_load( handle hI, char *sId, handle hData )
//------------------------------------------------------------------------------
	{
	strdncpy( &w, dirdat, -1, DIR_CHAR "data" DIR_CHAR, -1,
				sId, 3, DIR_CHAR, 1, &((sId)[3]), 3, DIR_CHAR, 1,
				&((sId)[6]), 3, DIR_CHAR "data", -1, _ );

	if ( !file_exists( w ) ) return hNULL;

	var_free( hData );
	hData = var_load( w );
	return hData;
	}



//------------------------------------------------------------------------------
bool idat_getdir( handle hI, char *sId, char **p_dir )
//------------------------------------------------------------------------------
	{
	strdncpy( p_dir, dirdat, -1, DIR_CHAR "data" DIR_CHAR, -1,
				sId, 3, DIR_CHAR, 1, &((sId)[3]), 3, DIR_CHAR, 1,
				&((sId)[6]), 3, _ );

	return dir_exists( *p_dir );
	}



//------------------------------------------------------------------------------
bool idat_idx_up( handle hI, char *idx_name, char *idx_id, char *id_list )
//------------------------------------------------------------------------------
	{
	char b = false;
	
	
	strdcpy( &w, dirdat, DIR_CHAR "index" DIR_CHAR, upper(strdcpy(&w1, idx_name, _ )), DIR_CHAR, idx_id, ".lock", _ );
	hLock = file_lock( w );
	w[n=(strlen(w)-5)] = 0;
	
	if ( !file_exists( w ) ) goto fin;
	file_load( &w1, w );

	ts = str_explode( id_list, "," );
	for( i = 0 ; ts[i] ; i++ )
		{
		p = strpos( w1, ts[i], 0 );
		if ( p < 0 ) continue;
		if ( p < 10 ) break;
		b = true;
		memcpy( &(w1[p]), &(w1[p-10]), 9 );
		memcpy( &(w1[p-10]), ts[i], 9 );
		}
	
	if ( b )	file_save_str( w, w1, -1 );
	free( ts );
	
	fin:
	file_unlock( hLock );
	w[n] = '.';
	unlink( w );
	
	return true;
	}



//------------------------------------------------------------------------------
bool idat_idx_down( handle hI, char *idx_name, char *idx_id, char *id_list )
//------------------------------------------------------------------------------
	{
	char b = false;
	
	strdcpy( &w, dirdat, DIR_CHAR "index" DIR_CHAR, upper(strdcpy(&w1, idx_name, _ )), DIR_CHAR, idx_id, ".lock", _ );
	hLock = file_lock( w );
	w[n=(strlen(w)-5)] = 0;

	if ( !file_exists( w ) ) goto fin;

	file_load( &w1, w );

	ts = str_explode( id_list, "," );
	for( i = 0 ; ts[i] ; i++ );
	for( p1 = strlen(w1) ; i >= 0 ; i-- )
		{
		p = strpos( w1, ts[i], 0 );
		if ( p < 0 ) continue;
		if ( p+10 >= p1 ) break;
		b = true;
		memcpy( &(w1[p]), &(w1[p+10]), 9 );
		memcpy( &(w1[p+10]), ts[i], 9 );
		}
	
	if ( b )	file_save_str( w, w1, -1 );
	free( ts );

	
	fin:

	file_unlock( hLock );
	w[n] = '.';
	unlink( w );
	
	return true;
	}



//------------------------------------------------------------------------------
int idat_select( handle hI, char *cond, char *pv_fields, char **id_list )
//------------------------------------------------------------------------------
	{
	char b;
	handle h;
	T_FILE_INFO *tf;
	
	ts = null;
	if ( pv_fields )	ts = str_explode( upper(strdcpy( &w, pv_fields, _ )), "," );
	h = var_from_cmd( 0, cond );
	
	strdcpy( &w4, ";", _ ); // CONTIENDRA LA LISTE RESULTAT
	
	for( i = 0 ; v = var_getn( h, i, &v1 ) ; i++ )
		{
		strdcpy( &w, dirdat, DIR_CHAR "index" DIR_CHAR, upper(v1), DIR_CHAR, upper(v), _ );
		strdcpy( &w3, ";", _ );
		tf = null;
		while( tf = file_list( tf, w, "nodir absdir" ) )
			{
			strdcpy( &w1, tf->dir, DIR_CHAR, tf->name, _ );
			if ( file_load( &w2, w1 ) == sERROR ) continue;
			if ( w2[0] != ';' ) continue;
			strdcat( &w3, &(w2[1]), _ );
			}
		
		if ( ! w3[1] )
			{ // LISTE VIDE
			v = w3; w3 = w4; w4 = v;
			break;
			}
			
		// MERGE DES 2 LISTES w3 et w4
		
		if ( ! w4[1] )
			{ // PREMIERE AFFECTATION => Inutil de faire un merge
			v = w4; w4 = w3; w3 = v;
			continue;
			}
		
		if ( (p2=strlen( w3 )) < (p1=strlen( w4 )) )
			{ // ON RECHERCHE LA CHAINE LA PLUS COURTE POUR ACCELER LE MERGE
			v = w4; w4 = w3; w3 = v;
			p = p1; p1 = p2; p2 = p;
			}
		
		strdcpy( &w, ";", _ );
		for( p = 0 ; p < p1 ; p += 20 )
			{
			if ( strpos( w3, substr( w4, p, 10 ), 0 ) < 0 ) continue;
			strdncat( &w, &(w4[p+1]), 10, _ );
			}
		
		v = w4; w4 = w; w = v;

		if ( ! w4[1] )	break; // LISTE VIDE
		}
	
	var_free( h );

	strdcpy( id_list, "[", _ );
	p2 = false;
	for( p = 1, n = strlen( w4 ) ; p < n ; p += 10 )
		{
		p1 = strlen( *id_list );
		strdncat( id_list, (char*)((p2)?",":""), -1, "['", 2, &(w4[p]), 9, "'", 1, _ );
		if ( ts )
			{
			b = false;
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( ts[i][0] == 0 ) continue;
				strdncpy( &w, dirdat, -1, DIR_CHAR "data" DIR_CHAR, -1,
							&(w4[p]), 3, DIR_CHAR, 1, &(w4[p+3]), 3, DIR_CHAR, 1,
							&(w4[p+6]), 3, DIR_CHAR, 1, ts[i], -1, _ );
				if ( w1 ) w1[0] = 0;
				file_load( &w1, w );

				if ( ( !strnicmp( ts[i], "ROLES_", 6 ) )
					&&( stripos( w1, ";read;", 0 ) < 0 ) )
					{
					// DROIT EN LECTURE NON TROUVE => ON NE RENVOIE PAS L'ELEMENT
					(*id_list)[p1] = 0;
					b = true;
					break;
					}
				
				if ( w1[0] == '\'' )
					strdcat( id_list, ",", w1, _ );
				else
					strdcat( id_list, ",", quote(w1), _ );
				}
			if ( b ) continue;
			p2 = true;
			}
		strdcat( id_list, "]", _ );
		}
	
	strdcat( id_list, "]", _ );
	
	if ( ts ) free( ts );
	
	return n/10;
	}



//------------------------------------------------------------------------------
int idat_select0( handle hI, char *cond, char **id_list )
//------------------------------------------------------------------------------
	{
	char b;
	handle h;
	T_FILE_INFO *tf;
	
	h = var_from_cmd( 0, cond );
	
	strdcpy( &w4, ";", _ ); // CONTIENDRA LA LISTE RESULTAT
	
	for( i = 0 ; v = var_getn( h, i, &v1 ) ; i++ )
		{
		strdcpy( &w, dirdat, DIR_CHAR "index" DIR_CHAR, upper(v1), DIR_CHAR, upper(v), _ );
		strdcpy( &w3, ";", _ );
		tf = null;
		while( tf = file_list( tf, w, "nodir absdir" ) )
			{
			strdcpy( &w1, tf->dir, DIR_CHAR, tf->name, _ );
			if ( file_load( &w2, w1 ) == sERROR ) continue;
			if ( w2[0] != ';' ) continue;
			strdcat( &w3, &(w2[1]), _ );
			}
		
		if ( ! w3[1] )
			{ // LISTE VIDE
			v = w3; w3 = w4; w4 = v;
			break;
			}
			
		// MERGE DES 2 LISTES w3 et w4
		
		if ( ! w4[1] )
			{ // PREMIERE AFFECTATION => Inutil de faire un merge
			v = w4; w4 = w3; w3 = v;
			continue;
			}
		
		if ( (p2=strlen( w3 )) < (p1=strlen( w4 )) )
			{ // ON RECHERCHE LA CHAINE LA PLUS COURTE POUR ACCELER LE MERGE
			v = w4; w4 = w3; w3 = v;
			p = p1; p1 = p2; p2 = p;
			}
		
		strdcpy( &w, ";", _ );
		for( p = 0 ; p < p1 ; p += 20 )
			{
			if ( strpos( w3, substr( w4, p, 10 ), 0 ) < 0 ) continue;
			strdncat( &w, &(w4[p+1]), 10, _ );
			}
		
		v = w4; w4 = w; w = v;

		if ( ! w4[1] )	break; // LISTE VIDE
		}
	
	var_free( h );

//printf( "idat_select0:%s" BR, w4 );

	v = w4; w4 = *id_list; *id_list = v;
	
	return strlen(*id_list)/10;
	}



//------------------------------------------------------------------------------
handle idat_select_join( handle hSel1, char *Field2, handle hSel2, char *Field1, ... )
//------------------------------------------------------------------------------
	{
	}





#undef hConf
#undef hoData
#undef hLock
#undef dirdat
#undef ts_idx_fields
#undef ts_u_fields
#undef ts_pv_fields
#undef ts
#undef w
#undef w1
#undef w2
#undef w3
#undef w4
#undef v
#undef v1
#undef v2
#undef i
#undef j
#undef n
#undef p
#undef p1
#undef p2




/*

- Demandeur : Recherche liste des creations ou modifs depuis la dernière synchro
	et envoi de cette liste au distant.
- Distant : Recherche liste des creations ou modifs depuis la dernière synchro
	et compare avec la liste reçue du demandeur.
- Distant :
	Pour chaque éléments de la liste consolidée :
		- envoi une des requêtes :
			- new {id distant} data="..."
				=> attend {id demandeur}
			- upd {id demandeur} data="..."
				=> Attend OK
			- del {id demandeur}
				=> Attend OK
	
	Envoi les index dont l'ordre à été modifié
		- idx_order {idx_name}

	Envoi : end quand il n'y a plus rien à faire.
		=> Attend OK
	
- Demandeur : Traite chacune des requêtes envoyées par le distant jusqu'au end
	Pour chaque requête, marque les éléments pour ne pas avoir à les retraiter
	au cours d'une synchro ultérieure.


-----------------------------------------------------------------------------------

Marquage des synchro :

$idat_dirdat/{partname}_{basename}/
	{date-heure opération}_new_{local_id}
	{date-heure opération}_upd_{local_id}_{remote_id}
	{date-heure opération}_del_{local_id}
	{date-heure opération}_idx_{idx_name}













*/






















typedef struct
	{
	handle	hConf;
	
	char	*dirdat;		// Répertoire des données
	char	*diridx;		// Répertoire des index
	char	*lock_fname;	// Nom du fichier verrou (Si le fichier existe, toute modif est suspendue, cela permet de fiabilser les arrêts de services)
	char	*KeyName;		// Champ qui contient l'ID
	uint	ItemSize;		// Taille totale des données (sans les données à taille variable)
	char 	**vlFields;		// Liste des champs à taille variable


	char 	**IndexPath;
	ushort 	IndexPathPos[64];
	ushort 	IndexPathLen[64];
	char 	**IndexKeyVal;
	ushort 	IndexKeyValLen[64];
	uint 	IndexKeyLen;

	char	*w, *w0, *w1, *w2, *w3, *w4, *v, *v1, *v2, *vn,
			*sid, *dfname, *fname, *fname1, *fname2,
			**ts, **ts1;
			
	int		i, j, p, b, i1, j1, n, p0, p1, p2, p3, p4, p5;
	handle	h, h0, h1;
	HANDLE	hLock0, hLock;

	FILE *fp, *fp1;
	}
	T_BD_WRK;
	

#define VLF_BLOC_SIZE	4096

#define hConf			(((T_BD_WRK*)hBD)->hConf)
#define dirdat			(((T_BD_WRK*)hBD)->dirdat)
#define diridx			(((T_BD_WRK*)hBD)->diridx)
#define lock_fname		(((T_BD_WRK*)hBD)->lock_fname)
#define KeyName			(((T_BD_WRK*)hBD)->KeyName)
#define KeyLen			8
#define ItemsPerFile	36*36
#define ItemSize		(((T_BD_WRK*)hBD)->ItemSize)
#define vlFields		(((T_BD_WRK*)hBD)->vlFields)

#define IndexPath		(((T_BD_WRK*)hBD)->IndexPath)
#define IndexPathPos	(((T_BD_WRK*)hBD)->IndexPathPos)
#define IndexPathLen	(((T_BD_WRK*)hBD)->IndexPathLen)
#define IndexKeyVal		(((T_BD_WRK*)hBD)->IndexKeyVal)
#define IndexKeyValLen	(((T_BD_WRK*)hBD)->IndexKeyValLen)
#define IndexKeyLen		(((T_BD_WRK*)hBD)->IndexKeyLen)

#define sid				(((T_BD_WRK*)hBD)->sid)
#define dfname			(((T_BD_WRK*)hBD)->dfname)
#define fname			(((T_BD_WRK*)hBD)->fname)
#define fname1			(((T_BD_WRK*)hBD)->fname1)
#define fname2			(((T_BD_WRK*)hBD)->fname2)
#define w				(((T_BD_WRK*)hBD)->w)
#define w0				(((T_BD_WRK*)hBD)->w0)
#define w1				(((T_BD_WRK*)hBD)->w1)
#define w2				(((T_BD_WRK*)hBD)->w2)
#define w3				(((T_BD_WRK*)hBD)->w3)
#define w4				(((T_BD_WRK*)hBD)->w4)
#define v				(((T_BD_WRK*)hBD)->v)
#define vn				(((T_BD_WRK*)hBD)->vn)
#define v1				(((T_BD_WRK*)hBD)->v1)
#define v2				(((T_BD_WRK*)hBD)->v2)
#define ts				(((T_BD_WRK*)hBD)->ts)
#define ts1				(((T_BD_WRK*)hBD)->ts1)
#define i				(((T_BD_WRK*)hBD)->i)
#define j				(((T_BD_WRK*)hBD)->j)
#define p				(((T_BD_WRK*)hBD)->p)
#define p0				(((T_BD_WRK*)hBD)->p0)
#define p1				(((T_BD_WRK*)hBD)->p1)
#define p2				(((T_BD_WRK*)hBD)->p2)
#define p3				(((T_BD_WRK*)hBD)->p3)
#define p4				(((T_BD_WRK*)hBD)->p4)
#define p5				(((T_BD_WRK*)hBD)->p5)
#define b				(((T_BD_WRK*)hBD)->b)
#define n				(((T_BD_WRK*)hBD)->n)
#define i1				(((T_BD_WRK*)hBD)->i1)
#define j1				(((T_BD_WRK*)hBD)->j1)

#define h				(((T_BD_WRK*)hBD)->h)
#define h0				(((T_BD_WRK*)hBD)->h0)
#define h1				(((T_BD_WRK*)hBD)->h1)
#define fp				(((T_BD_WRK*)hBD)->fp)
#define fp1				(((T_BD_WRK*)hBD)->fp1)
#define hLock			(((T_BD_WRK*)hBD)->hLock)
#define hLock0			(((T_BD_WRK*)hBD)->hLock0)

#define idx_block		(((T_BD_WRK*)hBD)->idx_block)

//------------------------------------------------------------------------------------------

handle	bd_hopen( handle hCfg )
	{
	T_BD_WRK *t_bd;
	handle hBD;
	
	t_bd = (T_BD_WRK *)(alloc0(sizeof(T_BD_WRK)));
	hBD = (handle)t_bd;
	hConf = hCfg;

	dirdat = var_get( hConf, "dirdat" );
	diridx = var_get( hConf, "diridx" );
	lock_fname = var_get( hConf, "lock_fname" );

	KeyName = var_get( hConf, "key_name" );
	if ( !KeyName )
		{
		bd_close( hBD );
		error_set( 1, "ERREUR : key_name non défini", _ );
		return hNULL; 
		}
	ItemSize = atoi(var_get( hConf, "item_size" ));
	if ( !ItemSize )
		{
		bd_close( hBD );
		error_set( 1, "ERREUR : item_size non défini", _ );
		return hNULL; 
		}

	v = var_get( hConf, "vlfields" );
	if ( v )
		vlFields = str_explode( v, "," );

	v = var_get( hConf, "index_path" );
	if ( !v )
		{
		bd_close( hBD );
		error_set( 1, "ERREUR : index_path non défini", _ );
		return hNULL; 
		}
	IndexPath = str_explode( v, "/" );

	for( i = 0 ; v1 = IndexPath[i] ; i++ )
		{
		if ( v1[0] == 0 ) continue;
		IndexPathPos[i] = 0;
		IndexPathLen[i] = -1;
		
		p = strpos( v1, "[", 0 );
		if ( p < 0 ) continue;
		IndexPathPos[i] = atoi( &(v1[p+1]) );
		p1 = strpos( v1, ",", p );
		v1[p] = 0;
		if ( p1 >= 0 )
			IndexPathLen[i] = atoi( &(v1[p1+1]) );
		}
	
	v = var_get( hConf, "index_key" );
	if ( !v )
		{
		bd_close( hBD );
		error_set( 1, "ERREUR : index_key non défini", _ );
		return hNULL; 
		}
	IndexKeyVal = str_explode( v, "," );
	
	IndexKeyLen = 0;
	for( i = 0 ; v1 = IndexKeyVal[i] ; i++ )
		{
		if ( v1[0] == 0 ) continue;
		p = strpos( v1, "[", 0 );
		if ( p < 0 )
			{
			bd_close( hBD );
			error_set( 1, "ERREUR : index_key mal défini. Il manque la longueur du champ : ", v1, _ );
			return hNULL; 
			}
		IndexKeyValLen[i] = atoi( &(v1[p+1]) );
		IndexKeyLen += IndexKeyValLen[i];
		v1[p] = 0;
		}
	
	return hBD;
	}

//------------------------------------------------------------------------------------------

handle	bd_open( char *p_fname )
	{
	handle hCfg;
	
	hCfg = var_load( p_fname );
	
	return bd_hopen( hCfg );
	}

//------------------------------------------------------------------------------------------

int	bd_close( handle hBD )
	{
	if ( sid )  free( sid );
	if ( w )  free( w );
	if ( w0 ) free( w0 );
	if ( w1 ) free( w1 );
	if ( w2 ) free( w2 );
	if ( w3 ) free( w3 );
	var_free( h );
	var_free( h1 );
		
	free( (void*)hBD );
	return OK;
	}


//------------------------------------------------------------------------------------------

HANDLE bd_lock( handle hBD, char *id )
	{
	strdcpy( &fname, dirdat, DIR_CHAR, id, ".lock", _ );
	return file_lock0( fname );
	}
	
	
//------------------------------------------------------------------------------------------

int bd_unlock( handle hBD, char *id, HANDLE p_lock )
	{
	file_unlock0( p_lock );
	#ifdef OS_WINDOWS
	strdcpy( &fname, dirdat, DIR_CHAR, id, ".lock", _ );
	unlink( fname );
	#endif
	}
	
	
//------------------------------------------------------------------------------------------

int bd_insert( handle hBD, handle hData, char **p_out )
	{
	if ( lock_fname )
	for( i = 600 ; ( file_exists( lock_fname ) )&&( i ) ; i-- ) Sleep(100); // 1 minute maxi

	strdcpy( &sid, var_get( hData, KeyName ), _ );

	// VEROUILLE LE FICHIER DES DONNEES
	
	strdncpy( &dfname, dirdat, -1, DIR_CHAR, 1,
						sid, 2, DIR_CHAR, 1, &(sid[2]), 2, DIR_CHAR, 1, &(sid[4]), 2, _ );

	strdcpy( &fname1, dfname, ".lock", _ );
	hLock0 = file_lock( fname1 );
	
	// INDEXATION

	// CALCUL NOM DU FICHIER INDEXE
	
	strdcpy( &fname, diridx, _ );
	p1 = strlen( fname );
	
	for( i = 0 ; v1=IndexPath[i] ; i++ )
		{
		if ( !(v1[0]) ) continue;
		v = var_get( hData, v1 );
			
		if ( !v ) strdcat( &fname, DIR_CHAR "[null]", _ );
		else
		if ( ! (v[0]) ) strdcat( &fname, DIR_CHAR "[vide]", _ );
		else
			strdncat( &fname, DIR_CHAR, 1, &(v[IndexPathPos[i]]), IndexPathLen[i], _ );
		}
	upper( &(fname[p1]) );

//print( "insert index fname=", fname, BR, _ );
	
	// CALCULE LE BLOCK D'INDEXE A ECRIRE

	w1 = Realloc( w1, IndexKeyLen + 8 + 1 );
	memset( w1, 0, IndexKeyLen + 8 );
	memcpy( w1, sid, 8 );
	
	for( i = 0, p = 8 ; v1 = IndexKeyVal[i] ; i++ )
		{
		if ( !(v1[0]) ) continue;
		v = var_get( hData, v1 );
			
		if ( !v ) v = "[NULL]";
		else
		if ( ! (v[0]) ) v = "[VIDE]";
		
//print( "insert index key ", v1, " = ", v, BR, _ );
		
		n = strlen(v);
		if ( n >= IndexKeyValLen[i] )
			strncpy( &(w1[p]), v, IndexKeyValLen[i] );
		else
			strcpy( &(w1[p]), v );
		upper( &(w1[p]) );
		p += IndexKeyValLen[i];
		}
	
	
	// AJOUTE L'ELEMENT DANS LE FICHIER INDEXE
	
	strdcpy( &fname2, fname, ".lock", _ );
	hLock = file_lock( fname2 );
	
	p1 = file_size( fname );
//	fp = fopen( fname, "ab" );
	for( ; (fp = fopen( fname, "ab" )) == NULL ; Sleep(1) );
//	if ( fp  )
		{
//		p1 = ftell( fp ); // Ne fonctionne pas quand ouverture du fichier en append
		fwrite( fp, w1, IndexKeyLen + 8, _ );
		fclose( fp );
		}
	
	file_unlock( hLock );
	unlink( fname2 );
	
	strdcpy( &w, itona( p1, 12 ), fname, _ );
	var_set( hData, "_elib_bd_index", w );
//print( "_elib_bd_index=", w, BR, _ );
	
	
	
	// SAUVEGARDE DES DONNEES VARIABLES
	
	if ( vlFields )
		{
		strdcpy( &fname, dfname, "_vlf", _ );
		strdcpy( &fname2, fname, ".lock", _ );
		hLock = file_lock( fname2 );
		if ( hLock == (HANDLE)ERROR )
			{
//			fclose( fp );
			file_unlock( hLock0 );
			unlink( fname1 );
			error_set( 1, "ERREUR verrouillage fichier : ", fname2, _ );
			return ERROR;
			}
		
		b = file_exists( fname );
		fp = fopen( fname, "rb+" );
		if ( fp == NULL )
			{
			mkdir( file_getdir( dfname ) );
			
			// CREATION D'UN FICHIER DE DONNEES 
			if ( !b ) // fp = fopen( fname, "wb" );
			for( n = 1000 ; ( n )&&((fp = fopen( fname, "wb" )) == NULL) ; Sleep(10), n-- );
			if ( fp == NULL )
				{
				file_unlock( hLock0 );
				unlink( fname1 );
				error_set( 1, "ERREUR ouverture fichier : ", fname, _ );
				return ERROR;
				}
			}
				
		if ( !b )
			{
			// Initialise pointeurs des éléments supprimés
			p3 = 0;
			fwrite( fp, &p3, sizeof(p3) /*premier*/, &p3, sizeof(p3) /*dernier*/, _ );
			p3 = 2*sizeof(p3);
			}
		else
			{
			fseek_end( fp, 0 );
			p3 = ftell( fp );
//print( "bd_insert vlf : p3 = ", itoa(p3), BR, _ );
			}
		
		for( i = 0 ; vlFields[i] ; i++ )
			{
			p1 = p2 = p3;	
			v = var_get( hData, vlFields[i] );
			if ( !v ) continue;
			if ( !strnicmp( v, "elib_bd_vlf_concat:", 19 ) )
				v = &(v[19]);
			n = strlen( v )+1;
			for( p = 0 ; ; p += (VLF_BLOC_SIZE-sizeof(p)) )
				{
				p3 += VLF_BLOC_SIZE;
				if ( n > (VLF_BLOC_SIZE-sizeof(p)) )
					{
					p2 += VLF_BLOC_SIZE;
//print( "insert:1:p2=", itoa(p2), BR, _ );
					fwrite( fp, &p2, sizeof(p2), &(v[p]), (VLF_BLOC_SIZE-sizeof(p)), _ );
					n -= (VLF_BLOC_SIZE-sizeof(p));
					}
				else
					{
//print( "insert:2" BR, _ );
					j = 0;
					fwrite( fp, &j, sizeof(j), &(v[p]), -2, _ );
					for( i1 = strlen(&(v[p]))+1+sizeof(p) ; i1 < VLF_BLOC_SIZE ; i1++ )
						fwrite( fp, &j, 1, _ );
//print( "insert:2a" BR, _ );
					break;
					}
				}
			var_delete( hData, vlFields[i] );
			strdcpy( &w1, "_elib_bd_vlf_", vlFields[i], _ );
			strdcpy( &w, itona( p1, 10 ), "|", itona( p2, 10 ), _ );
			var_set( hData, w1, w );
			}
		
		fclose( fp );
		file_unlock( hLock );
		unlink( fname2 );
		}
	
	
	// SAUVEGARDE DES DONNEES FIXES
	// OUVERTURE DU FICHIER DE DONNEES
	
	fp = fopen( dfname, "rb+" );
	if ( fp == NULL )
		{
		mkdir( file_getdir( dfname ) );
		
		// CREATION D'UN FICHIER DE DONNEES 
		fp = fopen( dfname, "wb" );
		if ( fp == NULL )
			{
			file_unlock( hLock0 );
			unlink( fname1 );
			error_set( 1, "ERREUR ouverture fichier : ", dfname, _ );
			return ERROR;
			}
			
		// INITIALISE LA TAILLE
		
		w = Realloc( w, ItemSize );
		memset( w, 0, ItemSize );
		for( i = ItemsPerFile ; i ; i-- )
			{
			fwrite( fp, w, ItemSize, _ );
			}
		}
	

	// CALCUL POSITION DES DONNEES DANS CE FICHIER

	p = ifrom36( &(sid[6]) ) * ItemSize;

//print( "insert data_fname = ", dfname, BR,
//			"pos = ", itoa( p ), BR,
//			_ );
	
	
	// ENREGISTRE LES DONNEES
	
	fseek_top( fp, p );
	var_to_js_array( hData, &w );
	
	fwrite( fp,
				" ", 1,		// Indique que des données sont présentes. "D" ==> Données supprimées
				w, -2,		// Ecrit le 0 de fin de chaine
				_ );
	fclose( fp );
	

	file_unlock( hLock0 );
	unlink( fname1 );
	
	return OK;
	}

//------------------------------------------------------------------------------------------

int bd_update( handle hBD, handle hData, char *id_list, uint id_nb )
	{
	uint i10;
	
	// BOUCLE SUR LA LISTE
tr( "bd_update1" );
	
	for( i10 = 0 ; i10 < id_nb ; i10++ )
		{
tr( "bd_update2" );
		if ( lock_fname )
		for( i = 600 ; ( file_exists( lock_fname ) )&&( i ) ; i-- ) Sleep(100); // 1 minute maxi
	
		strdncpy( &sid, &(id_list[i10*KeyLen]), KeyLen, _ );
		

		// VEROUILLE LE FICHIER DES DONNEES
		
		strdncpy( &dfname, dirdat, -1, DIR_CHAR, 1,
							sid, 2, DIR_CHAR, 1, &(sid[2]), 2, DIR_CHAR, 1, &(sid[4]), 2, _ );
		strdcpy( &fname1, dfname, ".lock", _ );

tr2( "bd_update3", fname1 );

		hLock0 = file_lock( fname1 );
		
tr( "bd_update4" );
		
		// CALCUL POSITION DES DONNEES DANS CE FICHIER
	
		p0 = ifrom36( &(sid[6]) ) * ItemSize;
	
//print( "update data_fname = ", dfname, BR,
//			"pos = ", itoa( p0 ), BR,
//			__ );
		
		
		// CHARGE ANCIENNES DONNEES

		fp = fopen( dfname, "rb+" );
		if ( fp == NULL )
			{
			file_unlock( hLock0 );
			unlink( fname1 );
			error_set( 1, "ERREUR ouverture fichier : ", dfname, _ );
			return ERROR;
			}
		
tr( "bd_update5" );

		fseek_top( fp, p0+1 );
		w = Realloc( w, ItemSize );
		fread( fp, w, ItemSize );
//fclose( fp );
		var_free( h0 );
		h0 = var_from_str( 0, w );
		
		
		// SAUVEGARDE DES DONNEES VARIABLES
tr( "bd_update6" );
		
		if ( vlFields )
			{
			strdcpy( &fname, dfname, "_vlf", _ );
			strdcpy( &fname2, fname, ".lock", _ );
tr( "bd_update7" );
			hLock = file_lock( fname2 );
tr( "bd_update8" );
			if ( hLock == (HANDLE)ERROR )
				{
				fclose( fp );
				file_unlock( hLock0 );
				unlink( fname1 );
				error_set( 1, "ERREUR verrouillage fichier : ", fname2, _ );
				return ERROR;
				}
			
			b = file_exists( fname );
			fp1 = fopen( fname, "rb+" );
			if ( fp1 == NULL )
				{
				mkdir( file_getdir( dfname ) );
				
				// CREATION D'UN FICHIER DE DONNEES 
				if ( !b ) fp1 = fopen( fname, "wb" );
				if ( fp1 == NULL )
					{
					fclose( fp );
					file_unlock( hLock );
					unlink( fname2 );
					file_unlock( hLock0 );
					unlink( fname1 );
					error_set( 1, "ERREUR ouverture fichier : ", fname, _ );
					return ERROR;
					}
				}
			if ( !b )
				{
				// Initialise pointeurs des éléments supprimés
				p3 = 0;
				fwrite( fp1, &p3, sizeof(p3) /*premier*/, &p3, sizeof(p3) /*dernier*/, _ );
				p3 = 2*sizeof(p3);
				}
			else
				{
				fseek_end( fp1, 0 );
				p3 = ftell( fp1 );
				}
			
tr( "bd_update9" );
			for( i = 0 ; vlFields[i] ; i++ )
				{
				v = var_get( hData, vlFields[i] );
				if ( !v ) continue;
				strdcpy( &w, "_elib_bd_vlf_", vlFields[i], _ );
				v1 = var_get( h0, w );
				if ( v1 )
					{
					// Champ avec ancienne valeur
					p1 = atoi( v1 );
					p2 = atoi( &(v1[11]) );
					if ( !strnicmp( v, "elib_bd_vlf_concat:", 19 ) )
						{
//logw( "bd_update:vlf:concat", BR, _ );
						// Concatenation
						v = &(v[19]);
						
						// Charge le dernier bloc
						fseek_top( fp1, p2+sizeof(p) );
//logw( "bd_update:vlf:concat:fseek_top:", itoa(p2+sizeof(p)), BR, _ );
						w = Realloc( w, VLF_BLOC_SIZE );
						fread( fp1, w, VLF_BLOC_SIZE - sizeof(p) );
						strdcat( &w, v, _ );
						v = w;
//logw( "bd_update:vlf:concat:nouvelle valeur:" BR, v, BR "strlen=", itoa(strlen(v)), BR "====================================" BR, _ );
						fseek_top( fp1, p2 );
//logw( "bd_update:vlf:concat:fseek_top:", itoa(p2), ":", itoa(ftell(fp1)), BR, _ );
						
						// Ecriture de la suite
						n = strlen( v )+1;
						for( p = 0 ; ; p += (VLF_BLOC_SIZE-sizeof(p)) )
							{
							if ( p )
								{
//logw( "bd_update:vlf:concat:fseek_top:", itoa(p3), BR, _ );
								fseek_top( fp1, p3 );
								p3 += VLF_BLOC_SIZE;
								}
								
							if ( n > (VLF_BLOC_SIZE-sizeof(p)) )
								{
								p2 = p3;
//logw( "bd_update:vlf:concat:fwrite:", itoa(p2), BR, _ );
								fwrite( fp1, &p2, sizeof(p2), &(v[p]), (VLF_BLOC_SIZE-sizeof(p)), _ );
								n -= (VLF_BLOC_SIZE-sizeof(p));
								}
							else
								{
								j = 0;
//logw( "bd_update:vlf:concat:ftell:", itoa(ftell(fp1)), BR, _ );
								fwrite( fp1, &j, sizeof(j), &(v[p]), -2, _ );
//logw( "bd_update:vlf:concat:ftell:", itoa(ftell(fp1)), BR, _ );
								for( n=0, i1 = strlen(&(v[p]))+1+sizeof(p) ; i1 < VLF_BLOC_SIZE ; i1++, n++ )
									fwrite( fp1, &j, 1, _ );

//logw( "bd_update:vlf:concat:fwrite last block : 4 + ", itoa(strlen(&(v[p]))+1), " + ", itoa(n), " = ", itoa(4+strlen(&(v[p]))+1+n), BR, _ );
//logw( "bd_update:vlf:concat:ftell:", itoa(ftell(fp1)), BR, _ );
								break;
								}
							}
						}
					else
						{
						// Nouvelle valeur
//print( "bd_update:vlf:new val:1:", v, ",p1=", itoa(p1), BR, _ );
						n = strlen( v )+1;
						p4 = p5 = p1;
						for( p = 0, b = false ; ; p += (VLF_BLOC_SIZE-sizeof(p)) )
							{
							if ( !p5 ) b = true;
							if ( !b )
								{
								// Charge next
//print( "bd_update:vlf:new val:fseek_top:", itoa(p4), BR, _ );
								fseek_top( fp1, p4 );
								fread( fp1, (char*)(&p5), sizeof(p) );
								fseek_top( fp1, p4+4 ); // Obligatoire, sinon pb au prochain fwrite
								if ( p5 ) p4 = p5;
//print( "bd_update:vlf:new val:next block:", itoa(p5), BR, _ );
								}
							else
								{
//print( "bd_update:vlf:new val:fseek_top:", itoa(p3), BR, _ );
								fseek_top( fp1, p3 );
								p3 += VLF_BLOC_SIZE;
								p4 = p3;
								}

							if ( n > (VLF_BLOC_SIZE-sizeof(p)) )
								{
//print( "bd_update:vlf:new val:fwrite:big block", BR, _ );
								if ( b )
									{
									fwrite( fp1, &p4, sizeof(p2), _ );
									}
								fwrite( fp1, &(v[p]), (VLF_BLOC_SIZE-sizeof(p)), _ );
								n -= (VLF_BLOC_SIZE-sizeof(p));
								}
							else
								{
								if ( !b ) fseek_cur( fp1, -sizeof(p) );
								j = 0;
								fwrite( fp1, &j, sizeof(j), &(v[p]), -2, _ );
								for( i1 = strlen(&(v[p]))+1+sizeof(p) ; i1 < VLF_BLOC_SIZE ; i1++ )
									fwrite( fp1, &j, 1, _ );
								if (( !b )&&( p5 ))
									{
//print( "bd_update:vlf:new val:delete block", BR, _ );
									// Supprime les bloc restants
									fseek_top( fp1, 0 ); // Lecture premier bloc supprimé pour initialisation si besoin
									fread( fp1, (char*)(&j), sizeof(p) );
									if ( !j )
										{
//print( "bd_update:vlf:new val:delete block:fseek_top:0, p5=", itoa(p5), ", p2=", itoa(p2), BR, _ );
										fseek_top( fp1, 0 );
										fwrite( fp1, &p5, sizeof(p), _ );
										fwrite( fp1, &p2, sizeof(p), _ );
										}
									else
										{
										fseek_top( fp1, sizeof(p) ); // Lecture dernier bloc supprimé
										fread( fp1, (char*)(&j), sizeof(p) );
										fseek_top( fp1, sizeof(p) ); // Le dernier est maintenant l'ancien dernier
										fwrite( fp1, &p2, sizeof(p), _ );
										// Chainage
//print( "bd_update:vlf:new val:delete block:fseek_top:", itoa(j), ", p5=", itoa(p5), BR, _ );
										fseek_top( fp1, j ); // Mise à jour du next du dernier bloc supprimé
										fwrite( fp1, &p5, sizeof(p), _ );
										}
									}

								break;
								}
							}
						p2 = p4;
						}
					}
				else
					{
//print( "bd_update:vlf:new new val", BR, _ );
					// Le champ n'existait pas
tr( "bd_update10" );
					p1 = p2 = p3;
					if ( !strnicmp( v, "elib_bd_vlf_concat:", 19 ) )
						v = &(v[19]);
					n = strlen( v )+1;
					for( p = 0 ; ; p += (VLF_BLOC_SIZE-sizeof(p)) )
						{
						p3 += VLF_BLOC_SIZE;
						if ( n > (VLF_BLOC_SIZE-sizeof(p)) )
							{
							p2 += VLF_BLOC_SIZE;
							fwrite( fp1, &p2, sizeof(p2), &(v[p]), (VLF_BLOC_SIZE-sizeof(p)), _ );
							n -= (VLF_BLOC_SIZE-sizeof(p));
							}
						else
							{
							j = 0;
							fwrite( fp1, &j, sizeof(j), &(v[p]), -2, _ );
							for( i1 = strlen(&(v[p]))+1+sizeof(p) ; i1 < VLF_BLOC_SIZE ; i1++ )
								fwrite( fp1, &j, 1, _ );
							break;
							}
						}
					}
					
tr( "bd_update11" );
//				strdcpy( &w, itona( p1, 10 ), "|", itona( p2, 10 ), _ );
//				var_set( hData, vlFields[i], w );
				var_delete( hData, vlFields[i] );
				strdcpy( &w1, "_elib_bd_vlf_", vlFields[i], _ );
				strdcpy( &w, itona( p1, 10 ), "|", itona( p2, 10 ), _ );
				var_set( hData, w1, w );
				}
//print( "fclose ", fname, BR, _ );			
tr( "bd_update12" );
			fclose( fp1 );

			file_unlock( hLock );
			unlink( fname2 );
			}
	
tr( "bd_update13" );

		// MERGE
		
		var_free( h1 );
		h1 = var_dup( h0 );
		for( i = 0 ; v = var_getn( hData, i, &vn ) ; i++ )
			{
			var_set( h1, vn, v );
			}
//		var_set( h1, "dt_upd", currdate("flog") );
		
		// h1 contient maintenant les nouvelles valeurs, et h0 les anciennes
		
		// INDEXATION
	
		// CALCUL NOM DU FICHIER INDEXE
		
tr( "bd_update14" );
		strdcpy( &fname, diridx, _ );
		p1 = strlen( fname );
		
		for( i = 0 ; v1=IndexPath[i] ; i++ )
			{
			if ( !(v1[0]) ) continue;
			v = var_get( h1, v1 );
				
			if ( !v ) strdcat( &fname, DIR_CHAR "[null]", _ );
			else
			if ( ! (v[0]) ) strdcat( &fname, DIR_CHAR "[vide]", _ );
			else
				strdncat( &fname, DIR_CHAR, 1, &(v[IndexPathPos[i]]), IndexPathLen[i], _ );
			}
		upper( &(fname[p1]) );
		
tr( "bd_update15" );
		
		// CALCULE LE BLOCK D'INDEXE A ECRIRE
	
		w1 = Realloc( w1, IndexKeyLen + 8 + 1 );
		memset( w1, 0, IndexKeyLen + 8 );
		memcpy( w1, sid, 8 );
		
		for( i = 0, p = 8 ; v1 = IndexKeyVal[i] ; i++ )
			{
			if ( !(v1[0]) ) continue;
			v = var_get( h1, v1 );
				
			if ( !v ) v = "[NULL]";
			else
			if ( ! (v[0]) ) v = "[VIDE]";
			
//print( "update index key ", v1, " = ", v, BR, _ );
			
			n = strlen(v);
			if ( n >= IndexKeyValLen[i] )
				strncpy( &(w1[p]), v, IndexKeyValLen[i] );
			else
				strcpy( &(w1[p]), v );
//			strncpy( &(w1[p]), v, IndexKeyValLen[i] );
			upper( &(w1[p]) );
			p += IndexKeyValLen[i];
			}
		
		
		// AJOUTE L'ELEMENT DANS LE FICHIER INDEXE
tr( "bd_update16" );
		
		v = var_get( h0, "_elib_bd_index" );
		if ( ( !v )||( strcmp( fname, &(v[12]) ) ) )
			{
			// NOUVEAU FICHIER INDEX
//print( "update index other fname=", fname, BR, _ );
tr( "bd_update17" );
			strdcpy( &fname2, fname, ".lock", _ );
			hLock = file_lock( fname2 );
			
tr2( "bd_update17a:", fname );
			p1 = file_size( fname );
tr( "bd_update17b" );
			for( ; (fp1 = fopen( fname, "ab" )) == NULL ; Sleep(1) );
tr( "bd_update17c" );
//			if ( fp1 )
				{
//				p1 = ftell( fp1 );	// Bogue:retourne toujours 0 car ouverture en append
//print( "p1=", itoa(p1), BR, _ );
				fwrite( fp1, w1, IndexKeyLen + 8, _ );
				fclose( fp1 );
				}
			
			file_unlock( hLock );
			unlink( fname2 );
			
			strdcpy( &w, itona( p1, 12 ), fname, _ );
			var_set( h1, "_elib_bd_index", w );
//print( "update _elib_bd_index=", w, BR, _ );
			
			// DESINDEXATION
			
			p = atoi( v );
//print( "update desindexation ", itoa(p), ":", &(v[12]), BR,
//	var_to_str( h0, &w ), BR BR, _ );
			strdcpy( &fname2, &(v[12]), ".lock", _ );
tr2( "bd_update17d:", fname2 );
			hLock = file_lock( fname2 );
			
tr( "bd_update17e" );
//			fp1 = fopen( &(v[12]), "rb+" );
			for( n = 100 ; ((fp1 = fopen( &(v[12]), "rb+" )) == NULL)&&( n ) ; Sleep(1), n-- );

tr( "bd_update17f" );
			if ( fp1 )
				{
				fseek_top( fp1, p );
				memset( w1, 0, 8 );
				fwrite( fp1, w1, 8, _ );
				fclose( fp1 );
				}
//else
//print( "ERREUR ouverture fichier : ", &(v[12]), BR, _ );			
			file_unlock( hLock );
			unlink( fname2 );
			}
		else
			{
			// MEME FICHIER INDEX
tr( "bd_update18" );
			
			p = atoi( v );
			strdcpy( &fname2, &(v[12]), ".lock", _ );
			hLock = file_lock( fname2 );
			
//print( "update même index fname=", &(v[12]), BR, _ );
//			fp1 = fopen( &(v[12]), "rb+" );
			for( n = 100 ; ((fp1 = fopen( &(v[12]), "rb+" )) == NULL)&&( n ) ; Sleep(1), n-- );
			if ( fp1 )
				{
				fseek_top( fp1, p );
				fwrite( fp1, w1, IndexKeyLen + 8, _ );
				fclose( fp1 );
				}
			
			file_unlock( hLock );
			unlink( fname2 );
			}
		
		
		// SAUVEGARDE DES DONNEES FIXES
tr( "bd_update19" );
			
		fseek_top( fp, p0 );
		var_to_js_array( h1, &w );
		
		fwrite( fp,
					" ", 1,		// Indique que des données sont présentes. "D" ==> Données supprimées
					w, -2,		// Ecrit le 0 de fin de chaine
					_ );
		fclose( fp );
	
		file_unlock( hLock0 );
		unlink( fname1 );
tr( "bd_update20" );
		}

tr( "bd_update21" );
	
	return OK;
	}


//------------------------------------------------------------------------------------------

int bd_delete( handle hBD, char *id_list, uint id_nb )
	{
	uint i10;
	
	// BOUCLE SUR LA LISTE
	
	for( i10 = 0 ; i10 < id_nb ; i10++ )
		{
		if ( lock_fname )
		for( i = 600 ; ( file_exists( lock_fname ) )&&( i ) ; i-- ) Sleep(100); // 1 minute maxi

		strdncpy( &sid, &(id_list[i10*KeyLen]), KeyLen, _ );
		
		// VEROUILLE LE FICHIER DES DONNEES
//print( "bd_delete:1" BR, _ );
	
		strdncpy( &dfname, dirdat, -1, DIR_CHAR, 1,
							sid, 2, DIR_CHAR, 1, &(sid[2]), 2, DIR_CHAR, 1, &(sid[4]), 2, _ );
		strdcpy( &fname1, dfname, ".lock", _ );
		hLock0 = file_lock( fname1 );
		
		
		// CALCUL POSITION DES DONNEES DANS CE FICHIER
	
		p0 = ifrom36( &(sid[6]) ) * ItemSize;
	
//print( "delete data_fname = ", dfname, BR,
//			"pos = ", itoa( p0 ), BR,
//			_ );
		
		
		// CHARGE DONNEES POUR OBTENIR LE FICHIER INDEXE
//print( "bd_delete:2" BR, _ );		

		fp = fopen( dfname, "rb+" );
		if ( fp == NULL )
			{
			file_unlock( hLock0 );
			unlink( fname1 );
			error_set( 1, "ERREUR ouverture fichier : ", dfname, _ );
			return ERROR;
			}
		
		fseek_top( fp, p0+1 );
		w = Realloc( w, ItemSize );
		fread( fp, w, ItemSize );
		var_free( h0 );
		h0 = var_from_str( 0, w );
		
		// DESINDEXATION
		
		v = var_get( h0, "_elib_bd_index" );
		
//print( "delete desindexation ", &(v[12]), BR, _ );
		p = atoi( v );
		strdcpy( &fname2, &(v[12]), ".lock", _ );
		hLock = file_lock( fname2 );

//		fp1 = fopen( &(v[12]), "rb+" );
		for( ; (fp1 = fopen( &(v[12]), "rb+" )) == NULL ; Sleep(1) );
//		if ( fp1 )
			{
			fseek_top( fp1, p );
			memset( w, 0, 8 );
			fwrite( fp1, w, 8, _ );
			fclose( fp1 );
			}
		
		file_unlock( hLock );
		unlink( fname2 );
		
		// SUPPRIME LA DONNEES
		
		fseek_top( fp, p0 );
		fwrite( fp, "D", 1, _ );
		fclose( fp );
			
		
		// SUPPRIME LES VLF
		
		if ( vlFields )
			{
			strdcpy( &fname, dfname, "_vlf", _ );
			strdcpy( &fname2, fname, ".lock", _ );
			hLock = file_lock( fname2 );
			
			fp1 = fopen( fname, "rb+" );
//print( "bd_delete : lecture fichier : ", fname, BR, _ );
			
			for( i = 0 ; v1 = vlFields[i] ; i++ )
				{
				if ( v1[0] == 0 ) continue;
				strdcpy( &w, "_elib_bd_vlf_", v1, _ );
				v = var_get( h0, w );
				if ( !v ) continue;
				p1 = atoi( v );
				p2 = atoi( &(v[11]) );
			
				fseek_top( fp1, 0 ); // Lecture premier bloc supprimé pour initialisation si besoin
				if ( fread( fp1, (char*)(&j), sizeof(p) ) != sizeof(p) )
					print( "ERREUR : lecture fichier : ", fname, BR, _ );
				if ( !j )
					{
//print( "bd_delete:vlf:delete block:fseek_top:0, p5=", itoa(p5), ", p2=", itoa(p2), BR, _ );
					fseek_top( fp1, 0 );
					fwrite( fp1, &p1, sizeof(p), _ );
					fwrite( fp1, &p2, sizeof(p), _ );
					}
				else
					{
					fseek_top( fp1, sizeof(p) ); // Lecture dernier bloc supprimé
					fread( fp1, (char*)(&j), sizeof(p) );
					fseek_top( fp1, sizeof(p) ); // Le dernier est maintenant l'ancien dernier
					fwrite( fp1, &p2, sizeof(p), _ );
					// Chainage
//print( "bd_delete:vlf:delete block:fseek_top:", itoa(j), ", p1=", itoa(p1), BR, _ );
					fseek_top( fp1, j ); // Mise à jour du next du dernier bloc supprimé
					fwrite( fp1, &p1, sizeof(p), _ );
					}
				}
			
			fclose( fp1 );
			file_unlock( hLock );
			unlink( fname2 );
			}
		
//print( "bd_delete:5" BR, _ );		
		
		// FIN
		
		file_unlock( hLock0 );
		unlink( fname1 );
		}

	
	return OK;
	}

	
//------------------------------------------------------------------------------------------

int		bd_load( handle hBD, char *id_list, uint id_nb, char *of, char **p_out )
	{
	int nb = 0;
	
	strdcpy( p_out, "", _ );
	
	b = 0; // { fieldname:val, ... },{...} : retourne la totalité sauf les vlf;
	//print("sni: bd_load rqt=",of,BR,_);
	if ( of )
		{
		p = strpos( of, ":", 0 );
		if ( p >= 0 )
			{
			strdncpy( &w4, of, p, _ ); // Lecture du séparateur
			if ( !stricmp( w4, "js" ) ) b = 1; // {fieldname:valeur,...},{...}
			else
			if ( !stricmp( w4, "jsa" ) ) b = 2; // [valeur,...],[...]
			else
			if ( !stricmp( w4, "csv" ) ) { b = 3; strdcpy( &w4, ";", _ ); }
			else
			if ( !stricmp( w4, "scr" ) ) b = 4; // affichage à l'écran
			else
			if ( !stricmp( w4, "wscr" ) ) b = 5; // affichage à l'écran par tcp
			else b = 3; // valeur<sep>valeur<sep>... BR
			
			if ( strpos( &(of[p+1]), ";", 0 ) >= 0 )
				ts = str_explode( &(of[p+1]), ";" );
			else
				ts = str_explode( &(of[p+1]), "," );
			}
		else
			if ( !stricmp( of, "js" ) )
				{ b = 1; ts = null; }
		}
	else
		ts = null;

	
	for( i = 0 ; i < id_nb ; i++ )
		{
		strdncpy( &sid, &(id_list[i*KeyLen]), KeyLen, _ );

		// CALCUL NOM DU FICHIER DES DONNEES FIXES ET OUVRE LE FICHIER
		
		strdncpy( &dfname, dirdat, -1, DIR_CHAR, 1, sid, 2, DIR_CHAR, 1,
							&(sid[2]), 2, DIR_CHAR, 1, &(sid[4]), 2, _ );
		
		
		// CALCUL POSITION DES DONNEES DANS CE FICHIER

		p0 = ifrom36( &(sid[6]) ) * ItemSize;
	
//print( "load data_fname = ", dfname, BR,
//			"pos = ", itoa( p0 ), BR,
//			__ );
		
		
		// CHARGE DONNEES

		fp = fopen( dfname, "rb" );
		if ( fp == NULL )
			{
			continue;
//			strdcpy( &w, "{", KeyName, ":", quote(sid), "}", _ );
/*
			error_set( 1, "ERREUR ouverture fichier : ", dfname, " (", sid, ")", _ );
			if ( ts ) free( ts );
			return ERROR;
*/
			}
		else
			{
//			fseek_top( fp, p0+1 );
			fseek_top( fp, p0 );
			w = Realloc( w, ItemSize );
	
			fread( fp, w, ItemSize );
			fclose( fp );
			if ( w[0] != ' ' ) continue;
			}
//print( "w=", w, BR BR, _ );
		nb++;
		
		// FORMATE LES INFO
		
		if ( !b )
			{
			strdcat( p_out, (char*)((i)?"|":""), w, _ );
			continue;
			}
		if (( b == 1 )&&( !ts ))
			{
			strdcat( p_out, (char*)((i)?",":""), "{", w, "}", _ );
			continue;
			}
		
		var_free( h );
		h = var_from_str( 0, w );

		// Ouvre le fichier de données des VLF
		if (( vlFields )&&( fp ))
			{
			fp = NULL;
			strdncpy( &dfname, dirdat, -1, DIR_CHAR, 1,
						sid, 2, DIR_CHAR, 1, &(sid[2]), 2,
						DIR_CHAR, 1, &(sid[4]), 2, "_vlf", 4, _ );
//print( "bd_load vlf_fname=", dfname, BR, _ );
			for( n = 3 ; ( n )&&( !(fp = fopen( dfname, "rb" )) ) ; Sleep(100), n-- )
				if ( !file_exists( dfname ) ) break;

			if ( fp == NULL )
				{
tr2( "bd_load:error1:", dfname );
				error_set( 1, "ERREUR ouverture fichier : ", dfname, _ );
//				if ( ts ) free( ts );
//				return ERROR;
				}
			}
		else
			fp = NULL;

		
		switch( b )
			{
			case 1 : // js
				strdcat( p_out, (char*)((i)?",":""), "{", _ );
				break;
				
			case 2 : // jsa
				strdcat( p_out, (char*)((i)?",":""), "[", _ );
				break;
			
			case 3 : // csv
//				strdcat( p_out, (char*)((i)? BR :""), _ );
				break;
			
			case 4 : // scr
				if ( i ) printf( BR );
				break;
			
			case 5 : // wscr
				strdcat( p_out, (char*)((i)? BR :""), _ );
				break;
			}
		
		for( j = 0 ; ts[j] ; j++ )
			{
			if ( ts[j][0] == 0 ) continue;

			// Est-ce un champs à longueur variable ?
			i1 = false;
			if ( vlFields )
			for( j1 = 0 ; v1=vlFields[j1] ; j1++ )
				{
				if ( stricmp( v1, ts[j] ) ) continue;
				
				// Oui, C'est un champ à longueur variable
				i1 = true;
				strdcpy( &w, "_elib_bd_vlf_", v1, _ );
				v = var_get( h, w );
				if ( !v ) break; // valeur nulle
				
				if ( fp == NULL )
					{
					v = "error : vlf open file";
					break;
					}
				
				// Lecture des données	
				p1 = atoi( v );
				strdcpy( &w, "", _ );
				for( p = 0 ; p1 ;)
					{
//print( "read vlf : ", v1, " : readpos=", itoa(p1), BR, _ );
					fseek_top( fp, p1 );
					// Lecture adresse prochain bloc
					if ( fread( fp, (char*)(&p1), sizeof(p) )!= sizeof(p) )
						{
						p1 = 0;
						strdcpy( &w, "read vlf error", _ );
//						strdcpy( &w, "read vlf error : readpos=", itoa(p1), _ );
						break;
						}

					// Lecture données
					w = Realloc( w, p + VLF_BLOC_SIZE );
					p += fread( fp, &(w[p]), VLF_BLOC_SIZE-sizeof(p) );
					}
				v = w;
				break;
				}
			
			if ( !i1 )
				v = var_get( h, ts[j] ); // C'est un champ normal
			// otrs#2017042810000828: taille(time*) est variable
			if (v) if ( !strncmp( ts[j], "time", 4 ) )  strdcpy( &v, substr( v, 0, 8  ), _ );

			if ( !v ) v = "[null]";
			
			switch( b )
				{
				case 1 : // js
					strdcat( p_out, (char*)((j)?",":""), ts[j], ":", quote( v ), _ );
					break;
				case 2 : // jsa
					strdcat( p_out, (char*)((j)?",":""), quote( v ), _ );
					break;
				case 3 : // csv
					strdcat( p_out, (char*)((j)? w4 :""), v, _ );
					break;
				case 4 : // scr
					strdncpy( &w, (char*)((j)? " " :""), -1, strpad(v,atoi(ts[j+1])," "), atoi(ts[j+1]), _ );
					printf( "%s", w );
					j++;
					break;
				case 5 : // wscr
					strdncpy( &w, (char*)((j)? " " :""), -1, strpad(v,atoi(ts[j+1])," "), atoi(ts[j+1]), _ );
					strdcat( p_out, w, _ );
					j++;
					break;
				}
			}

		if ( fp ) fclose( fp );
		
		switch( b )
			{
			case 1 : // js
				strdcat( p_out, "}", _ );
				break;
			case 2 : // jsa
				strdcat( p_out, "]", _ );
				break;
			case 3 : // csv
				strdcat( p_out, BR, _ );
				break;
			case 4 : // scr
				strdcpy( p_out, "", _ );
				break;
/*			case 5 : // wscr
				strdcat( p_out, BR, _ );
				break;
*/			}
		}

	if ( ts ) free( ts );
	return nb;
	}
	

//------------------------------------------------------------------------------------------
typedef struct
	{
	char id[8];
	uint keylen;
	uchar key[1];
	}
	T_SORT_BLOC;

int keycmp( T_SORT_BLOC *pb1, T_SORT_BLOC *pb2 )
	{
	// Retourne 1 si pb1 > pb2, -1 si pb1 < pb2, 0 si égalité
	uint k = 0, l;
	
	l = ( pb1->keylen < pb2->keylen ) ? pb1->keylen : pb2->keylen;
	
	for( ; k < l ; k++ )
		{
		if ( pb1->key[k] > pb2->key[k] ) return  1;
		if ( pb1->key[k] < pb2->key[k] ) return -1;
		}
	return 0;
	}


char *	_bd_select( handle hBD, handle hCond, int IndexNo, char * sParent );

int		bd_select( handle hBD, char *Cond, char *OrderBy, char **p_out )
	{
	char **lts, **lts1, **lts2,
		*result = null, *cond = null, *pcond = null, *ri[2], rn,
		**tbSort
		;
	int li, li1, li2,
		bLargeFilter = false, bLoadData = false,
		tbSortSize, tbSortLen, sort_bloc_size,
		_n = 0, _n1, _n2;
		;
	T_SORT_BLOC *sort_bloc;


	ri[0] = ri[1] = null;
//	if ( strpos( Cond, "|", 0 ) >= 0 )
//		lts = str_explode( str_replace( Cond, "&", " " ), "|" );
//	else
		lts = str_explode( str_replace( Cond, "&", " " ), " or " );
	strdcpy( &result, "", _ );

//print( "coucou bd_select 1", BR, __ );
tr2("bd_select_1:", Cond);

	for( li = 0 ; lts[li] ; li++ )
		{
		if ( lts[li][0] == 0 )
			{
			if ( li ) continue;
			if ( lts[li+1] ) continue;
			// Laisse passer les conditions vides ==> Retourne tous les éléments
			}

tr("bd_select_2");
	
		var_free( h );
		h = var_from_cmd( 0, lts[li] );
		
			{
			strdcpy( &w1, diridx, _ );
tr2( "bd_select_2a: w1=", w1 );
tr2( ", lts=", lts[li] );
			v = _bd_select( hBD, h, 0, w1 );

tr("bd_select_3");

			// Concatène les résultats (supprime les doublons)
			ts = str_explode( v, "," );
			ts1 = str_explode( result, "," );
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( ts[i][0] == 0 ) continue;
				for( j = 0 ; ts1[j] ; j++ )
					{
					if ( ts1[j][0] == 0 ) continue;
					if ( !strcmp( ts[i], ts1[j] ) ) break;
					}
				if ( !(ts1[j]) )
					strdcat( &(result), (char*)((result[0])?",":""), ts[i], _ );
				}
			free( ts ); free( ts1 );
			free( v );
			}
		}

tr("bd_select_4");

	if ( cond ) free( cond );
	if ( pcond ) free( pcond );
	if ( ri[0] ) free( ri[0] );
	if ( ri[1] ) free( ri[1] );

	if ( !result[0] )
		{
		free( lts );
		goto fin;
		}
			
	
	// FILTRE DES CHAMPS NON INDEXES ET TRI

	// Listes des champs non indexé

tr("bd_select_5");
	
	strdcpy( &w1, ",", _ ); // Champs dans la clef d'index
	strdcpy( &w2, ",", _ ); // Champs ni indexé, ni dans la clef
	
	var_free( h );
	h = var_from_cmd( 0, Cond );
	for( i = 0 ; v = var_getn( h, i, &vn ) ; i++ )
		{
		if (( !stricmp( vn, "or" ) )||( !stricmp( vn, "|" ) )||( !stricmp( vn, "*" ) )) continue;
		for( j = 0 ; v1 = IndexPath[j] ; j++ )
			if ( ! stricmp( vn, v1 ) ) break;
		
		if ( v1 ) continue; // C'est un champ indexé
		
		for( j = 0 ; v1 = IndexKeyVal[j] ; j++ )
			if ( ! stricmp( vn, v1 ) ) break;
		
		bLargeFilter = true;
		
		if ( v1 )	continue; // C'est un champ dans la clef
		if ( !stricmp( v, "true" ) ) continue; // mot recherché, mais uniquement dans la clef d'indexe
		
		bLoadData = true;
		break;
		}
	
	ts1 = null;
	if (( OrderBy )&&( OrderBy[0] ))
		{
		ts1 = str_explode( OrderBy, "," );
		if ( !bLoadData )
		for( i = 0 ; vn=ts1[i] ; i++ )
			{
			if ( vn[0] == 0 ) continue;
			if (( vn[0] == '-' )||( vn[0] == '+' )) vn = &(vn[1]);
			
			for( j = 0 ; v1 = IndexPath[j] ; j++ )
				if ( ! stricmp( vn, v1 ) ) break;
			if ( v1 ) continue; // C'est un champ indexé
			
			for( j = 0 ; v1 = IndexKeyVal[j] ; j++ )
				if ( ! stricmp( vn, v1 ) ) break;
			if ( v1 )	continue; // C'est un champ dans la clef
			
			bLoadData = true;
			break;
			}
		
		// Initialise la table de tri
		
		tbSort = (void*)alloc( sizeof(char*)*4096 );
		tbSortSize = 4096;
		tbSortLen = 0;
		}

//print( "bd_select:result=", result, BR BR, _ );
	
	ts = str_explode( result, "," );

//for( i = 0 ; ts[i] ; i++ );
//print( "nb=", itoa(i), BR, _ );

tr("bd_select_6");

	_n1 = 0;
	_n2 = 0;
	
	strdcpy( &result, "", _ );
	for( i = 0 ; ts[i] ; i++ )
		{
		if ( ts[i][0] == 0 ) continue;
		
		// OUVRE LE FICHIER INDEXE

tr("bd_select_7");
		
		strdcpy( &fname, diridx, DIR_CHAR, ts[i], _ );

		_n1 += (((file_size( fname ) / (IndexKeyLen + 8))+1) * 8);
		if ( p_out )
			result = realloc( result, _n1 );
		
		for( j = 2000 ; (( fp = fopen( fname, "rb" ) ) == NULL)&&( j ) ; Sleep(1), j-- );
		if ( fp == NULL )
			{
tr2( "ERREUR bd_select : ", fname );			
			return ERROR;
			}

tr("bd_select_8");
		
		// PARCOUR ELEMENT PAR ELEMENT
		
		w1 = Realloc( w1, IndexKeyLen + 8 );
		while( fread( fp, w1, IndexKeyLen + 8 ) == IndexKeyLen + 8 )
			{
			if ( w1[0] == 0 ) continue;
			strdncpy( &sid, w1, 8, _ );

tr("bd_select_9");

//print( "select suite sid= ", sid, BR, _ );
			
			if ( p_out )
			{
			strdncpy( &dfname, dirdat, -1, DIR_CHAR, 1,
								sid, 2, DIR_CHAR, 1, &(sid[2]), 2, DIR_CHAR, 1, &(sid[4]), 2, _ );

			// Index sans fichier de données ?
/*			
			if ( !file_exists( dfname ) )
				{
				// Supprime l'élément du fichier index
				
//print( "Unindex : ", fname, " (", sid, ")", BR, _ );						
				strdcpy( &fname2, fname, ".lock", _ );
				hLock = file_lock( fname2 );
				
				p = ftell( fp ) - (IndexKeyLen + 8);
				fclose( fp );
				for( j = 2000 ; ( fp = fopen( fname, "rb+" ) ) == NULL ; Sleep(1), j++ );
				if ( fp == NULL )
					{
					error_set( 1, "ERREUR ouverture fichier : ", fname, _ );
					return ERROR;
					}
				fseek_top( fp, p );
				p1 = 0;
				fwrite( fp, (char*)(&p1), 1, _ );
				fclose( fp );
				
				file_unlock( hLock );
				
				for( j = 2000 ; ( fp = fopen( fname, "rb" ) ) == NULL ; Sleep(1), j++ );
				if ( fp == NULL )
					{
					error_set( 1, "ERREUR ouverture fichier : ", fname, _ );
					return ERROR;
					}
				fseek_top( fp, p + IndexKeyLen + 8 );
//pause();
				continue;
				}
*/
			}
			

			// CHARGE LES DONNEES SI BESOIN
			if ( bLoadData )
				{
				// CALCUL POSITION DES DONNEES DANS CE FICHIER
tr("bd_select_10");
			
				p0 = ifrom36( &(sid[6]) ) * ItemSize;
			
//print( "select LoadData data_fname = ", dfname, BR,
//					"pos = ", itoa( p0 ), BR,
//					_ );
				
				
				// CHARGE DONNEES
		
				fp1 = fopen( dfname, "rb" );
				if ( fp1 == NULL )
					{
					continue;
//					error_set( 1, "ERREUR ouverture fichier : ", dfname, _ );
//					return ERROR;
					}
				
				fseek_top( fp1, p0+1 );
				w2 = Realloc( w2, ItemSize );
				fread( fp1, w2, ItemSize );
				fclose( fp1 );
				var_free( h0 );
				h0 = var_from_str( 0, w2 );

//print( "select LoadData OK", BR, __ );
				}
			
tr("bd_select_11");
			// FILTRAGE
			
			if ( bLargeFilter )
				{
//print( "select LargeFilter", BR, __ );
tr("bd_select_12");
				b = false;
				for( li = 0 ; lts[li] ; li++ )
					{
					if ( lts[li][0] == 0 ) continue;
					
//print( "select LargeFilter cond=", lts[li], BR, __ );
			
					var_free( h );
					h = var_from_cmd( 0, lts[li] );
					
					b = true;
					for( j = 0 ; v = var_getn( h, j, &vn ) ; j++ )
						{
						for( j1 = 0 ; v1 = IndexPath[j1] ; j1++ )
							if ( ! stricmp( vn, v1 ) ) break;
						if ( v1 ) continue; // C'est un champ indexé

//print( "select LargeFilter 1 : ", v, BR, __ );
						lts1 = str_explode( upper(v), "|" );
						
						if ( bLoadData )
							{
//print( "select LargeFilter 2", BR, __ );
							v1 = var_get( h0, vn );
							if ( !v1 ) v1 = "[NULL]";
							else
							if ( !(v1[0]) ) v1 = "[VIDE]";
							else upper( v1 );
//print( "select LargeFilter 3", BR, __ );
							}
						else
							{
							if ( ! stricmp( v, "true" ) )
								{
//print( "select LargeFilter 4 : ", vn, BR, __ );
								strdcpy( &w, "", _ );
								for( j1 = 0, p = 8 ; v1 = IndexKeyVal[j1] ; p += IndexKeyValLen[j1], j1++ )
									strdncat( &w, &(w1[p]), IndexKeyValLen[j1], _ );
								strdcpy( &w2, "*", upper(vn), "*", _ );
								b = strpcmp( w2, w );
//print( "select LargeFilter 4a:strpcmp ", w2, " : ", w, BR, __ );
								v1 = null;
//if ( stripos( w, "_CABE", 0 ) >= 0 )
//if ( b )
//print( "select LargeFilter 4b : b = ", itoa(b), ", w=", w, ", w2=", w2, BR, __ );
								}
							else
								{
								for( j1 = 0, p = 8 ; v1 = IndexKeyVal[j1] ; p += IndexKeyValLen[j1], j1++ )
										if ( ! stricmp( vn, v1 ) ) break;
								if ( !v1 )	b = false; // Champ clef non trouvée
								else
									{
									v1 = strdncpy( &w, &(w1[p]), IndexKeyValLen[j1], _ );
									}
								}
							}
						if ( v1 )
							{
//print( "select LargeFilter 4 : ", v1, BR, __ );
							for( li1 = 0 ; lts1[li1] ; li1++ )
								{
								if ( lts1[li1][0] == 0 ) continue;
								if ( lts1[li1][0] != '!' )
									{
									if ( strpcmp( lts1[li1], v1 ) ) break;
									}
								else
									if ( !strpcmp( &(lts1[li1][1]), v1 ) ) break;
								}
							if ( !(lts1[li1]) )	b = false;
							}
//print( "select LargeFilter 5 : ", vn, "=", v, " (", v1, ") ==> ", itoa(b), BR, _ );

						free( lts1 );
						if ( !b ) break; // ET : si faux donc tout le test est faux
						}
					if ( b ) break; // OU : si vrai donc tout le test est vrai
					}
//print( "select LargeFilter selected ?", BR, _ );
				if ( !b ) continue; // L'élément n'est pas sélectionné
//print( "select LargeFilter selected", BR, _ );
				}
			
//print( "coucou6" BR, _ );
			
tr("bd_select_13");
			// TRI
				
			if ( ts1 )
				{
				// CALCUL LA CLEF POUR LE TRI
tr("bd_select_14");
				
				sort_bloc_size = 256;
				sort_bloc = (T_SORT_BLOC*)alloc0( sort_bloc_size );
				memcpy( sort_bloc, sid, 8 );
				p3 = 8 + sizeof(uint);
				
				for( j = 0 ; vn=ts1[j] ; j++ )
					{
					if ( vn[0] == 0 ) continue;
					b = ( vn[0] == '-' ); // true => tri décroissant
					if (( vn[0] == '-' )||( vn[0] == '+' )) vn = &(vn[1]);
					
					for( j1 = 0 ; v1 = IndexPath[j1] ; j1++ )
						if ( ! stricmp( vn, v1 ) ) break;
					if ( v1 )  // C'est un champ indexé
						{
				
						for( i1 = p = 0 ; i1 < j1 ; i1++ )
							p = strpos( ts[i], DIR_CHAR, p+1 );
						p++;
						p1 = strpos( ts[i], DIR_CHAR, p );
						if ( p1 < 0 ) p1 = strlen( ts[i] );
						p2 = p1 - p;
//print( "tri champ indexé : j1=", itoa(j1), ", p3=", itoa(p3),
//", p=", itoa(p), ", p1=", itoa(p1), ", p2=", itoa(p2), BR, _ );
						memcpy( &(((char*)sort_bloc)[p3]), &(ts[i][p]), p2 );
						memset( &(((char*)sort_bloc)[p3+p2]), 0, 1 );
						if ( b )
							for( i1 = p3+p2 ; i1 >= p3 ; i1-- )
								((char*)sort_bloc)[i1] = 255-((char*)sort_bloc)[i1];
						p3 += p2+1;
						continue;
						}
					
					if ( bLoadData )
						{
						v1 = var_get( h0, vn );
						if ( !v1 ) v1 = "";
						}
					else
						{
						for( j1 = 0, p = 8 ; v1 = IndexKeyVal[j1] ; p += IndexKeyValLen[j1], j1++ )
							if ( ! stricmp( vn, v1 ) ) break;
						if ( !v1 )	v1 = ""; // Champ clef non trouvée
						else
							{
							v1 = strdncpy( &w, &(w1[p]), IndexKeyValLen[j1], _ );
							}
						}
					p = strlen( v1 )+1;
					memcpy( &(((char*)sort_bloc)[p3]), v1, p );
					if ( b )
						for( i1 = p3+p ; i1 >= p3 ; i1-- )
							((char*)sort_bloc)[i1] = 255-((char*)sort_bloc)[i1];
					p3 += p;
					}
				sort_bloc->keylen = p3 - (8 + sizeof(uint));

/*
print( "clef de tri : ", sid, ", keylen=", itoa(sort_bloc->keylen), BR, _ );
for( i1 = 0 ; i1 < sort_bloc->keylen ; i1++ )
	printf( " %d (%c)", sort_bloc->key[i1], sort_bloc->key[i1] );
print( BR "fin de la clef" BR, _ );
*/		
				// RECHERCHE EMPLACEMENT PAR DICHOTOMIE DANS LA TABLE DE TRI
				
				p1 = 0; p2 = tbSortLen; p = -1;
				for(; p2 - p1 ;)
					{
					p = p1 + ( (p2-p1)/2 );
					b = keycmp( sort_bloc, (T_SORT_BLOC*)(tbSort[p]) );
//print( BR "keycmp : p=", itoa( p ), ", p1=", itoa(p1), ", p2=", itoa(p2), ", b=", itoa(b), BR, _ );
					if ( !b ) break;
					if ( b < 0 )
						{
						if ( p == p1 ) { p--; break; }
						p2 = p;
						}
					else
						{
						p1 = p+1;
						if ( p1 == p2 ) { break; }
						}
					}
				
//print( BR "Recherche emplacement : ", itoa( p ), BR,
//	"tbSortLen=", itoa(tbSortLen), BR,
//	__ );

				// AJOUT DANS LA TABLE DE TRI
				
				if ( tbSortLen+1 >= tbSortSize )
					{
					tbSortSize += 4096;
					tbSort = (void*)realloc( tbSort, sizeof(char*)*tbSortSize );
					}
				
				for( p1 = tbSortLen ; p1 > p+1 ; p1-- )
					tbSort[p1] = tbSort[p1-1];

				tbSort[p+1] = (char*)sort_bloc;
				tbSortLen++;
				}
			else
			if ( p_out )
				// Calcul directement le résultat

				{
				memcpy( &(result[_n2]), w1, 8 );
				_n2+=8;
				}
//				strdncat( &result, w1, 8, _ );
			else
				_n++;
			}
		
tr("bd_select_15");
		fclose( fp );
		}
	
	result[_n2] = 0;
	
	free( lts );
	free( ts );
	
tr("bd_select_16");

	// RECUPERE LA LISTE DES ID APRES LE TRI
	
	if ( ts1 )
		{
		for( i = 0 ; i < tbSortLen ; i++ )
			{
			strdncat( &result, ((T_SORT_BLOC*)(tbSort[i]))->id, 8, _ );
			free( tbSort[i] );
			}
		
		free( tbSort );
		free( ts1 );
		}
	
	
	// FIN
	
	fin:
tr("bd_select_17");

	if ( !p_out ) return _n;
			
	if ( *p_out ) free( *p_out );
	*p_out = result;

tr("bd_select_18");
	
	return OK;
	}



char *	_bd_select( handle hBD, handle hCond, int IndexNo, char * sParent )
	{
	T_FILE_INFO *tf;
	char *result = null, *cp;
	char *sdir = null, bLast, **lts;
	int li, lp = strlen( diridx ) + 1;

	for( ; ; IndexNo++ )
		{
		v1 = IndexPath[IndexNo];
		if ( v1[0] ) break;
		}
	
	bLast = (IndexPath[IndexNo+1]) ? false : true;
	strdcpy( &result, "", _ );

	v = var_get( hCond, v1 );
	if ( v )
		{
		if (( v[0] == '>' )&&( v[1] == '=' ))
			{ cp = v; v = "*"; }
		else
		if (( v[0] == '<' )&&( v[1] == '=' ))
			{ cp = v; v = "*"; }
		else
		if (( v[0] == '<' )&&( v[1] == '>' ))
			{ cp = v; v = "*"; }
		else
		if (( v[0] == '>' )&&( v[1] == '<' ))
			{ cp = v; v = "*"; }
		else
		if ( v[0] == '!' )
			{ cp = v; v = "*";}
		else
		if ( v[0] == '>' )
			{ cp = v; v = "*"; }
		else
		if ( v[0] == '<' )
			{ cp = v; v = "*"; }
		else
			cp = "=";
		}
	else
		cp = "=";

	if ( !v ) strdcpy( &w, "*", _ );
	else strdcpy( &w, v, _ );

	lts = str_explode( upper(w), "|" );
	
	for( li = 0 ; lts[li] ; li++ )
		{
		if ( lts[li][0] == 0 ) continue;
		
		strdcpy( &fname, sParent, DIR_CHAR, lts[li], _ );
	
		tf = NULL;
		
		if ( !bLast )
			{
			if ( dir_exists( fname ) )
				{
				strdcpy( &sdir, fname, _ );
				v = _bd_select( hBD, hCond, IndexNo+1, sdir );
				strdcat( &result, (char*)(( result[0] ) ? "," : "" ), v, _ );
				free( v );
				}
			else
			while( tf = file_list( tf, fname, "nofile absdir" ) )
				{
				if ( cp[0] != '=' )
					{
					if ( ( cp[0] == '>' )&&( cp[1] == '=' ) )
						{
						if ( stricmp( tf->name, &(cp[2]) ) < 0 ) continue;
						}
					else
					if ( ( cp[0] == '<' )&&( cp[1] == '=' ) )
						{
						if ( stricmp( tf->name, &(cp[2]) ) > 0 ) continue;
						}
					else
					if ( cp[0] == '!' )
						{
						if ( stricmp( tf->name, &(cp[1]) ) == 0 ) continue;
						}
					else
					if ( cp[0] == '>' )
						{
						if ( stricmp( tf->name, &(cp[1]) ) <= 0 ) continue;
						}
					else
					if ( cp[0] == '<' )
						{
						if ( stricmp( tf->name, &(cp[1]) ) >= 0 ) continue;
						}
					}
				strdcpy( &sdir, tf->dir, DIR_CHAR, tf->name, _ );
				v = _bd_select( hBD, hCond, IndexNo+1, sdir );
				strdcat( &result, (char*)(( result[0] ) ? "," : "" ), v, _ );
				free( v );
				}
			}
		else
			{
			while( tf = file_list( tf, fname, "nodir absdir" ) )
				{
				if ( cp[0] != '=' )
					{
					if ( ( cp[0] == '>' )&&( cp[1] == '=' ) )
						{
						if ( stricmp( tf->name, &(cp[2]) ) < 0 ) continue;
						}
					else
					if ( ( cp[0] == '<' )&&( cp[1] == '=' ) )
						{
						if ( stricmp( tf->name, &(cp[2]) ) > 0 ) continue;
						}
					else
					if ( cp[0] == '!' )
						{
						if ( stricmp( tf->name, &(cp[1]) ) == 0 ) continue;
						}
					else
					if ( cp[0] == '>' )
						{
						if ( stricmp( tf->name, &(cp[1]) ) <= 0 ) continue;
						}
					else
					if ( cp[0] == '<' )
						{
						if ( stricmp( tf->name, &(cp[1]) ) >= 0 ) continue;
						}
					}
				strdcpy( &w, tf->dir, DIR_CHAR, tf->name, _ );
				strdcat( &result, (char*)(( result[0] ) ? "," : "" ), &(w[lp]), _ );
				}
			}
		}
	
	free( lts );
	if ( sdir ) free( sdir );
	return result;
	}
	



//------------------------------------------------------------------------------------------

int		bd_unindex( handle hBD, char *id )
	{
	char **lts, **lts1, **lts2,
		*result = null, *cond = null, *pcond = null, *ri[2], rn,
		**tbSort
		;
	int li, li1, li2,
		bLargeFilter = false, bLoadData = false,
		tbSortSize, tbSortLen, sort_bloc_size,
		_n = 0, _n1, _n2;
		;
	T_SORT_BLOC *sort_bloc;


	ri[0] = ri[1] = null;
	lts = str_explode( "", " or " );
	strdcpy( &result, "", _ );

//print( "coucou bd_select 1", BR, __ );

	for( li = 0 ; lts[li] ; li++ )
		{
		if ( lts[li][0] == 0 )
			{
			if ( li ) continue;
			if ( lts[li+1] ) continue;
			// Laisse passer les conditions vides ==> Retourne tous les éléments
			}
	
		var_free( h );
		h = var_from_cmd( 0, lts[li] );
		
			{
			strdcpy( &w1, diridx, _ );

			v = _bd_select( hBD, h, 0, w1 );


			// Concatène les résultats (supprime les doublons)
			ts = str_explode( v, "," );
			ts1 = str_explode( result, "," );
			for( i = 0 ; ts[i] ; i++ )
				{
				if ( ts[i][0] == 0 ) continue;
				for( j = 0 ; ts1[j] ; j++ )
					{
					if ( ts1[j][0] == 0 ) continue;
					if ( !strcmp( ts[i], ts1[j] ) ) break;
					}
				if ( !(ts1[j]) )
					strdcat( &(result), (char*)((result[0])?",":""), ts[i], _ );
				}
			free( ts ); free( ts1 );
			free( v );
			}
		}


	if ( cond ) free( cond );
	if ( pcond ) free( pcond );
	if ( ri[0] ) free( ri[0] );
	if ( ri[1] ) free( ri[1] );

	if ( !result[0] )
		{
		free( lts );
return;
		}
			

//print( "bd_select:result=", result, BR BR, _ );
	
	ts = str_explode( result, "," );
	
	for( i = 0 ; ts[i] ; i++ )
		{
		if ( ts[i][0] == 0 ) continue;
		
		// OUVRE LE FICHIER INDEXE

		
		strdcpy( &fname, diridx, DIR_CHAR, ts[i], _ );

//print( "select index_fname = ", fname, BR,
//"i = ", itoa(i), BR, _ );
		
		for( j = 2000 ; ( fp = fopen( fname, "rb+" ) ) == NULL ; Sleep(1), j++ );
		if ( fp == NULL ) continue;
		
		// PARCOUR ELEMENT PAR ELEMENT
		
		w1 = Realloc( w1, IndexKeyLen + 8 );
		
		
		while( fread( fp, w1, IndexKeyLen + 8 ) == IndexKeyLen + 8 )
			{
			if ( w1[0] == 0 ) continue;
			if ( strncmp( w1, id, 8 ) ) continue;
			print( "déindexation : ", fname, BR, _ );
//strdcpy( &w, "cp ", fname, " /appli/ssie7/data/", id, ".save", _ );
			p = ftell( fp ) - (IndexKeyLen + 8);
			fseek_top( fp , p );
			memset( w1, 0, 8 );
			fwrite( fp, w1, 8, _ );
			fseek_top( fp , p );
			}
		
		fclose( fp );
		}
	
	}



//------------------------------------------------------------------------------------------

int		bd_clean( handle hBD )
	{
	T_FILE_INFO *tf, *tf2;
	FILE *fp2;
	
	strdcpy( &w4, "", _ );
	
	// NETTOYAGE DES FICHIERS VLF
	
	if ( vlFields )
		{
		w2 = Realloc( w2, VLF_BLOC_SIZE );
		strdcpy( &fname, dirdat, DIR_CHAR "*_vlf", _ );
		tf = null;
		while( tf = file_list( tf, fname, "nodir absdir recursif" ) )
			{
			strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
			
			// Compte le nombre de bloc supprimé
			
			fp = fopen( fname, "rb" );
			if ( !fp ) continue;
			
			fread( fp, (char*)(&p1), sizeof(p) );
			fread( fp, (char*)(&p2), sizeof(p) );
			for( n = 0 ; p1 ; n++ )
				{
				fseek_top( fp, p1 );
				fread( fp, (char*)(&p1), sizeof(p) );
				}
	
			fclose( fp );
			
			if ( ((n*VLF_BLOC_SIZE*100)/file_size(fname)) <= 10 ) continue;
			
			// Commence le ménage
			
			strdncpy( &dfname, fname, strlen(fname)-4, _ );
			strdcpy( &fname1, dfname, ".lock", _ );
			hLock0 = file_lock( fname1 );

//print( "clean ", dfname, BR, _ );
			
			fp = fopen( dfname, "rb+" );
			if ( !fp ) continue;
			fseek_end( fp, 0 );
			p4 = ftell( fp );
			
			fp1 = fopen( fname, "rb" );
			if ( !fp1 )
				{
				fclose( fp ); 
				file_unlock( hLock0 );
				unlink( fname1 );
				continue;
				}
			
			strdcpy( &fname2, fname, ".new", _ );

			fp2 = fopen( fname2, "wb" );
			if ( !fp2 )
				{
				fclose( fp ); 
				fclose( fp1 ); 
				file_unlock( hLock0 );
				unlink( fname1 );
				continue;
				}
			j = 0;
			fwrite( fp2, &j, sizeof(p), &j, sizeof(p), _ );
			p3 = ftell( fp2 );
			
			w3 = Realloc( w3, ItemSize );
			for( p = n = 0 ; p < p4 ; p += ItemSize )
				{
				fseek_top( fp, p );
				fread( fp, w3, ItemSize );
				if ( w3[0] != ' ' ) continue;
				n++;
				var_free( h0 );
				h0 = var_from_str( 0, &(w3[1]) );
				for( i = 0, b = false ; v1 = vlFields[i] ; i++ )
					{
					if ( v1[0] == 0 ) continue;
					v = var_get( h0, v1 );
					if ( !v ) continue;
					b = true;
					p1 = atoi( v );
					strdcpy( &w1, itona( p3, 10 ), "|", _ );
					p2 = p3;
					for( ; p1 ; )
						{
						fseek_top( fp1, p1 );
						fread( fp1, (char*)(&p1), sizeof(p) );
						fread( fp1, w2, VLF_BLOC_SIZE-sizeof(p) );
						p2 = p3;
						p3 += VLF_BLOC_SIZE;
						if ( p1 )
							fwrite( fp2, &p3, sizeof(p), w2, VLF_BLOC_SIZE-sizeof(p), _ );
						else
							fwrite( fp2, &p1, sizeof(p), w2, VLF_BLOC_SIZE-sizeof(p), _ );
						}
					strdcat( &w1, itona( p2, 10 ), _ );
					var_set( h0, v1, w1 );
					}
				
				if ( b )
					{
					fseek_top( fp, p );
					var_to_js_array( h0, &w );
					fwrite( fp, " ", 1, w, -2, _ );
					}
				}

			fclose( fp );
			fclose( fp1 );
			fclose( fp2 );
			
			if ( !n ) // Fichier de données vide
				{
				file_unlock( hLock0 );
				unlink( fname1 );
				
				unlink( dfname );
				unlink( fname );
				unlink( fname2 );
				
				dfname[strlen(dfname)-3] = 0;
				strdcat( &w4, "|D", &(dfname[p=strlen(dirdat)]), _ );
				dfname[strlen(dfname)-3] = 0;
				strdcat( &w4, "|D", &(dfname[p]), _ );
				}
			else
				{
				unlink( fname );
				file_move( fname2, fname );
				file_unlock( hLock0 );
				unlink( fname1 );
				}
			}
		} // Fin clean vlf
	
	// NETTOYAGE DES FICHIERS INDEXES VIDES

	strdcpy( &fname, diridx, DIR_CHAR "*", _ );
	tf = null;
	while( tf = file_list( tf, fname, "nodir absdir recursif" ) )
		{
		strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
//print( "bd_clean:analyse ", fname, BR, _ );
		
		// Cherche un élément non supprimé
		
		fp = fopen( fname, "rb" );
		if ( !fp ) continue;
		fseek_end( fp, 0 );
		p4 = ftell( fp );
		
		w1 = Realloc( w1, 8 );
		w1[0] = 1;
		for( p = 0 ; p < p4 ; p += IndexKeyLen + 8 )
			{
			fseek_top( fp, p );
			fread( fp, w1, 1 );
			if ( w1[0] ) break;
			}
		
		fclose( fp );
		if ( w1[0] ) continue;
		
		// Le fichier ne contient que des éléments supprimés ==> Supprime le fichier

//print( "Supprime ", fname, BR, _ );
		
		unlink( fname );
		p = strrpos( fname, DIR_CHAR, -1 );
		n = strlen( diridx ) + 1;
		for( ; p > n ; p = strrpos( fname, DIR_CHAR, -1 ) )
			{
			fname[p] = 0;
			strdcat( &w4, "|I", &(fname[n-1]), _ );
			}

		}

	// Supprime les répertoires vides
	
	if ( w4[0] == '|' )
		{
		n = strlen( w4 );
		for( p1 = 1 ; p1 < n ; )
			{
			p2 = strpos( w4, "|", p1 );
			if ( p2 < 0 )
				{
				strdcpy( &w3, &(w4[p1]), _ );
				p1 = n;
				}
			else
				{
				strdncpy( &w3, &(w4[p1]), p2-p1, _ );
				p1 = p2+1;
				}
			if ( w3[0] == 0 ) continue;
			if ( w3[0] == 'D' )
				strdcpy( &fname, dirdat, &(w3[1]), _ );
			else
				strdcpy( &fname, diridx, &(w3[1]), _ );
//print( "rmdir ", fname, BR, _ );
			rmdir( fname );
			}
		}
	
	}
	
//------------------------------------------------------------------------------------------




#undef hConf

#undef dirdat
#undef diridx
#undef lock_fname
#undef KeyLen
#undef KeyName
#undef ItemSize
#undef ItemsPerFile
#undef vlFields
#undef IndexPath
#undef IndexKeyVal
#undef IndexKeyValLen
#undef IndexKeyLen

#undef sid
#undef dfname
#undef fname
#undef fname1
#undef fname2
#undef w
#undef w0
#undef w1
#undef w2
#undef w3
#undef w4
#undef v
#undef vn
#undef v1
#undef v2
#undef ts
#undef ts1
#undef i
#undef j
#undef p
#undef p0
#undef p1
#undef p2
#undef p3
#undef p4
#undef p5
#undef i1
#undef j1
#undef b
#undef n
#undef h
#undef h0
#undef h1
#undef fp
#undef fp1
#undef hLock
#undef hLock0














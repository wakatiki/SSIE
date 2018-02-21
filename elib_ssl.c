
char *ssl_pgm_fname = null;

//------------------------------------------------------------------------------
char *ssl_pgm( char *openssl_fname )
//------------------------------------------------------------------------------
	{
	char *v;
	
	if ( !openssl_fname )
		{
		#ifdef OS_UNIX
		if ( ! ssl_pgm_fname )
			{
			v = trim( System( null, "which openssl" ) );
			if ( v[0] == '/' )
				strdcpy( &ssl_pgm_fname, trim(v), _ );
			}
		#endif
		return ssl_pgm_fname;
		}
		
	if ( ! file_exists( openssl_fname ) ) return null;
	strdcpy( &ssl_pgm_fname, openssl_fname, _ );
	if ( strpos( ssl_pgm_fname, " ", 0 ) >= 0 )
		strd_C_quote( &ssl_pgm_fname );
		
	return ssl_pgm_fname;
	}

//------------------------------------------------------------------------------
char *der_to36( char *in, int l )
//------------------------------------------------------------------------------
	{
	int i;
	char s[4];
	char **o = memptr(0);
	int n = -1;
	
	if ( *o )	(*o)[0] = 0;
	for( i = 0 ; i < l ; i++ )
		{
		sprintf( s, "%03u", (unsigned char)(in[i]) );
		strdcat( o, s, _ );
		}
	return *o;
	}
	




//------------------------------------------------------------------------------
int der_from36( char *in, char *fname )
//------------------------------------------------------------------------------
	{
	FILE *fp;
	int i, n;
	char s[4], o[2];

	fp = fopen( fname, "wb" );
	if ( fp == NULL ) return ERROR;
	s[3] = o[1] = 0;
	for( i = 0 ; in[i] ; )
		{
		s[0] = in[i++];
		s[1] = in[i++];
		s[2] = in[i++];
		o[0] = atoi( s );
		fwrite( fp, o, 1, _ );
		}
	fprint( fp, BR, _ );
	fclose( fp );
	return OK;
	}
	


//------------------------------------------------------------------------------
char *der_to_pem( char *p_in )
//------------------------------------------------------------------------------
	{
	char **o = memptr(0);
	char *fname1 = null, *fname2 = null, *cmd = null, b = false;
	
	file_tmpname(&fname2);

	if ( file_exists( p_in ) )
		{
		strdcpy( &fname1, p_in, _ );	
		}
	else
	if ( strnicmp( p_in, "der:", 4 ) )
		return null;
	else
		{
		file_tmpname(&fname1);
		der_from36( &(p_in[4]), fname1 );
		b = true;
		}	
	
	strdcpy( &cmd, ssl_pgm_fname, " x509 -inform DER -outform PEM -text "
				"-in ", fname1, " -out ", fname2, _ );

	System( o, cmd );
	file_load( o, fname2 );
	
	unlink( fname2 );
	if ( b ) unlink( fname1 );
	
	free( cmd );
	free( fname1 );
	free( fname2 );
	
	return *o;
	}




//------------------------------------------------------------------------------
int ssl_load( char**certif, char* file )
//------------------------------------------------------------------------------
	{
	int n;
	char **s = memptr(0);
	
	if ( !file_exists( file ) ) return ERROR;
	if ( (n=file_size( file )) > 4096 ) return ERROR;

	if ( file_load( certif, file ) == sERROR ) return ERROR;
		
	if ( strpos( *certif, "BEGIN", 0 ) > 0 )
		{
		//# FORMAT PEM
		trim( *certif );
		return OK;
		}

	if ( ( strpos( file, ".pk12", 0 ) > 0 )||( strpos( file, ".p12", 0 ) > 0 ) )
		//# Format PKCS12
		strdcpy( s, "PK12:", _ );
	else
		//# FORMAT DER
		strdcpy( s, "DER :", _ );
		
	strdcat( s, der_to36( *certif, n ), _ );
//	free( *certif );
	*certif = *s;
	
	return OK;
	}
	


//------------------------------------------------------------------------------
int ssl_save( char *certif, char *file )
//------------------------------------------------------------------------------
	{
	FILE *fp;
	
	if ( ( ! strncmp( certif, "DER :", -1 ) )||( ! strncmp( certif, "PK12:", -1 ) ) )
		return der_from36( &(certif[5]), file );

	if ( strpos( certif, "-BEGIN ", 0 ) < 0 ) return ERROR;
	
	fp = fopen( file, "wb" );
	if ( fp == NULL ) return ERROR;
	fprint( fp, certif, _ );
	fclose( fp );
	
	return OK;
	}



//------------------------------------------------------------------------------
int ssl_prop_load( handle hVar, char *prefix_var, char *in )
//------------------------------------------------------------------------------
	{
	int p, p1, n;
	char **s = memptr(0), **s1 = memptr(0), *v, *v1, **ts;

	if ( ( p = stripos( in, "Issuer: ", 0 ) ) > 0 )
		{
		strdcpy( s, prefix_var, "_from", _ );
		p1 = strpos( in, "\n", p+8 );
		v = trim(substr( in, p+8, p1-p-8 ));
		var_set( hVar, *s, v );
		
		strdcpy( s, prefix_var, "_certificater", _ );
		p = strpos( v, "CN=", 0 );
		if ( p >= 0 )	v = &(v[p+3]);
		var_set( hVar, *s, v );
		}
	else
		return ERROR;

//	var_delete( hVar, "ssl_config_ok" );


	if ( ( p = stripos( in, "-----BEGIN", 0 ) ) > 0 )
		{
		p1 = strpos( in, "END CERTIFICAT-----", p+9 );
		v = trim(substr( in, p, p1+19-p ));
		var_set( hVar, prefix_var, v );
		}
		
	
	if ( ( p = stripos( in, "Subject: ", 0 ) ) > 0 )
		{
		strdcpy( s, prefix_var, "_to", _ );
		p1 = strpos( in, "\n", p+9 );
		v = trim(substr( in, p+9, p1-p-9 ));
		var_set( hVar, *s, v );
		
		strdcpy( s, prefix_var, "_owner", _ );
		p = strpos( v, "CN=", 0 );
		if ( p >= 0 ) v1 = &(v[p+3]);
		else v1 = "";
		if ( trim(v1)[0] )
			var_set( hVar, *s, v1 );
		else
			var_set( hVar, *s, v );
		}
	
	if ( ( p = stripos( in, "Not after : ", 0 ) ) > 0 )
		{
		strdcpy( s, prefix_var, "_enddate", _ );
		p1 = strpos( in, "\n", p+12 );
		
		strdcpy( s1, trim(substr( in, p+12, p1-p-12 )), _ );
		strd_replace( s1, "  ", " " );
		
		ts = str_explode( *s1, " " );
		
		strdcpy( s1, (char*)((ts[1][1])? "": "0"), ts[1], "/", _ );
		if ( ! stricmp( ts[0], "Jan" ) )
			strdcat( s1, "01/", _ );
		else
		if ( ! stricmp( ts[0], "Feb" ) )
			strdcat( s1, "02/", _ );
		else
		if ( ! stricmp( ts[0], "Mar" ) )
			strdcat( s1, "03/", _ );
		else
		if ( ! stricmp( ts[0], "Apr" ) )
			strdcat( s1, "04/", _ );
		else
		if ( ! stricmp( ts[0], "May" ) )
			strdcat( s1, "05/", _ );
		else
		if ( ! stricmp( ts[0], "Jun" ) )
			strdcat( s1, "06/", _ );
		else
		if ( ! stricmp( ts[0], "Jul" ) )
			strdcat( s1, "07/", _ );
		else
		if ( ! stricmp( ts[0], "Aug" ) )
			strdcat( s1, "08/", _ );
		else
		if ( ! stricmp( ts[0], "Sep" ) )
			strdcat( s1, "09/", _ );
		else
		if ( ! stricmp( ts[0], "Oct" ) )
			strdcat( s1, "10/", _ );
		else
		if ( ! stricmp( ts[0], "Nov" ) )
			strdcat( s1, "11/", _ );
		else
		if ( ! stricmp( ts[0], "Dec" ) )
			strdcat( s1, "12/", _ );

		strdcat( s1, ts[3], " ", ts[2], _ );		
		var_set( hVar, *s, *s1 );

		strdcpy( s, prefix_var, "_endtime", _ );
		n = dtoi( *s1, "dd/mm/yyyy hh:mm:ss" );
		var_set( hVar, *s, itoa(n) );
		}

	return OK;
	}
	

//------------------------------------------------------------------------------
int ssl_build_certif_lib( handle hVar, char *certif_lib )
//------------------------------------------------------------------------------
	{
	char *v, **s = memptr(0), **s3 = memptr(0), **fname = memptr(0);
	int i, j = 0;
	T_FILE_INFO *tf;

	if ( ! certif_lib ) return ERROR;
	mkdir( certif_lib );
	if ( !dir_exists( certif_lib ) ) return ERROR;

	for( i = 1 ; ; i++ )
		{	
		strdcpy( s3, "ssl_certif_ca", itoa( i ), _ );
		v = var_get( hVar, *s3 );
		if ( !v ) return OK;
		trim(v);
		
		tf = NULL;
		while( tf = file_list( tf, certif_lib, "nodir absdir" ) )
			{
			strdcpy( fname, tf->dir, DIR_CHAR, tf->name, _ );
			if ( file_size( *fname ) > 30 Ko ) continue;
			file_load( s, *fname );
			if ( !strcmp( trim( *s ), v ) ) break;
			}
		if ( tf ) 	file_list_free( tf );
		else
			{
			for( j++ ; ; j++ )
				{
				strdcpy( fname, certif_lib, DIR_CHAR, "ca_", itoa(j), ".pem", _ );
				if ( !file_exists( *fname ) ) break;
				}
			file_save_str( *fname, v, -1 );
			}
		}
	}




//openssl x509 -text -inform PEM -in certif/root.pem

//------------------------------------------------------------------------------

#define ssl_pubpriv_verify_der( fname )	\
	{\
	System( s1, strdcpy( cmd, ssl_pgm_fname, " x509 -noout -modulus -inform DER -in ", fname, _ ) );\
	r = ( ! strcmp( *s2, *s1 ) ) ? true : false;\
	if ( r )\
		{\
		System( s, strdcpy( cmd, ssl_pgm_fname, " x509 -text -inform DER -in ", fname, _ ) );\
		strdcpy( &w, var_prefix, "_config_ok", _ );\
		var_delete( hVar, w );\
		strdcpy( &w, var_prefix, "_certif_server", _ );\
		ssl_prop_load( hVar, w, *s );\
		}\
	}\



#define ssl_pubpriv_verify_pem( c )	\
	{\
	file_save_str( *fname2, c, -1 );\
	System( s1, strdcpy( cmd, ssl_pgm_fname, " x509 -noout -modulus -inform PEM -in ", *fname2, _ ) );\
	\
	r = ( ! strcmp( *s2, *s1 ) ) ? true : false;\
	if ( r )\
		{\
		System( s, strdcpy( cmd, ssl_pgm_fname, " x509 -text -inform PEM -in ", *fname2, _ ) );\
		strdcpy( &w, var_prefix, "_config_ok", _ );\
		var_delete( hVar, w );\
		strdcpy( &w, var_prefix, "_certif_server", _ );\
		ssl_prop_load( hVar, w, *s );\
		}\
	}\



#define ssl_ca_verify \
	if ( ( p2 = stripos( *s1, "Subject: ", 0 ) ) > 0 )\
	p3 = strpos( *s1, "\n", p2+9 );\
	v = trim(substr( *s1, p2+9, p3-p2-9 ));\
	r = ( strcmp( v1, v ) ) ? false : true;\
	if ( !r )\
		{\
		p2 = strpos( v, "CN=", 0 );\
		if ( p2 >= 0 ) v = &(v[p2+3]);\
		r = ( strcmp( v1, v ) ) ? false : true;\
		}\
	if ( r )\
		{\
		ssl_prop_load( hVar, *s3, *s1 );\
		}\


#define ssl_ca_verify_der( fname )	\
	{\
	System( s1, strdcpy( cmd, ssl_pgm_fname, " x509 -text -inform DER -in ", fname, _ ) );\
	ssl_ca_verify\
	}\



#define ssl_ca_verify_pem( c )	\
	{\
	file_save_str( *fname2, c, -1 );\
	System( s1, strdcpy( cmd, ssl_pgm_fname, " x509 -text -inform PEM -in ", *fname2, _ ) );\
	ssl_ca_verify\
	}\

	
//------------------------------------------------------------------------------
int ssl_conf_load2( handle hVar, char *certif, char *certif_lib, char *var_prefix )
//------------------------------------------------------------------------------
	{
	char *v, *v1, **cmd = memptr(0), **s = memptr(0), **s1 = memptr(0), **s2 = memptr(0), **s3 = memptr(0),
		**fname = memptr(0), **fname1 = memptr(0), **fname2 = memptr(0), *w = null;
	int p, p0, p1, p2, p3, r, n;
	FILE *fp;
	T_FILE_INFO *tf;
	

	if ( ! ssl_pgm( null ) )
		{
		error_set( 1, "ERREUR : Le programme openssl n'a pas été trouvé", _ );
		return ERROR;
		}
	
	
	//--------------------------------------------------------------------------
	// CHARGE LA CLEF PRIVEE
	//--------------------------------------------------------------------------
	
	if ( ( certif )&&
		(
			( ( p = strpos( certif, "-----BEGIN RSA PRIVATE KEY", 0 ) ) >= 0 )
			||
			( ( p = strpos( certif, "-----BEGIN PRIVATE KEY", 0 ) ) >= 0 )
		)
		)
		{
		//----------------------------------------------------------------------
		// UNE CLEF PRIVEE EST FOURNIE EN PARAMETRE
		//----------------------------------------------------------------------
		
		p1 = strpos( certif, " PRIVATE KEY-----", p + 22 );
		if ( p1 > p )
			{
			strdcpy( &w, var_prefix, "_private_key", _ );
			var_set( hVar, w, v = substr( certif, p, p1+18-p ) );
			strdcpy( &w, var_prefix, "_csr", _ );
			var_delete( hVar, w );
			strdcpy( &w, var_prefix, "_certif_server", _ );
			var_delete( hVar, w );
			for( n = 1 ; ; n++ )
				{
				strdcpy( s, var_prefix, "_certif_ca", itoa(n), _ );
				if ( !var_get( hVar, *s ) ) break;
				var_delete( hVar, *s );
				}
			strdcpy( &w, var_prefix, "_config_ok", _ );
			var_delete( hVar, w );
			}
		}
		

	if ( ( certif )&&( file_exists( certif ) ) )
		{
		//----------------------------------------------------------------------
		// CHARGE LE FICHIER FOURNI
		//----------------------------------------------------------------------
		if ( *s ) (*s)[0] = 0;
		file_load( s, certif );
		
		if ( 
			(
				( ( p = strpos( *s, "-----BEGIN RSA PRIVATE KEY", 0 ) ) >= 0 )
				||
				( ( p = strpos( *s, "-----BEGIN PRIVATE KEY", 0 ) ) >= 0 )
			)
			)
			{
			// LE FICIHIER DONNE EN PARAMETRE CONTIENT UNE CLEF PRIVEE
			
			p1 = strpos( *s, " PRIVATE KEY-----", p + 22 );
			if ( p1 > p )
				{
				strdcpy( &w, var_prefix, "_private_key", _ );
				var_set( hVar, w, v = substr( *s, p, p1+18-p ) );
				strdcpy( &w, var_prefix, "_csr", _ );
				var_delete( hVar, w );
				strdcpy( &w, var_prefix, "_certif_server", _ );
				var_delete( hVar, w );
				for( n = 1 ; ; n++ )
					{
					strdcpy( s, var_prefix, "_certif_ca", itoa(n), _ );
					if ( !var_get( hVar, *s ) ) break;
					var_delete( hVar, *s );
					}
				strdcpy( &w, var_prefix, "_config_ok", _ );
				var_delete( hVar, w );
				}
			}
		}

	
	file_tmpname(fname2);


	//--------------------------------------------------------------------------
	// SI PAS CLEF PRIVEE
	//--------------------------------------------------------------------------
	
	strdcpy( &w, var_prefix, "_private_key", _ );
	if ( ! ( v = var_get( hVar, w ) ) )
		{
		//----------------------------------------------------------------------
		// GENERATION D'UNE CLEF PRIVEE
		//----------------------------------------------------------------------
		
		strdcpy( cmd, ssl_pgm_fname, " genrsa 2048", _ );
		System( s, *cmd );
			
		p = strpos( *s, "-----BEGIN", 0 );
		if ( p < 0 )
			{
			error_set( 1, "ERREUR : Problème génération d'une clef RSA : ", *s, _ );
			return ERROR;
			}
		
		strdcpy( &w, var_prefix, "_private_key", _ );
		var_set( hVar, w, *s );
		v = var_get( hVar, w );
		
		strdcpy( &w, var_prefix, "_csr", _ );
		var_delete( hVar, w );
		strdcpy( &w, var_prefix, "_certif_server", _ );
		var_delete( hVar, w );
		for( n = 1 ; ; n++ )
			{
			strdcpy( &w, var_prefix, "_certif_ca", _ );
			strdcpy( s, w, itoa(n), _ );
			if ( !var_get( hVar, *s ) ) break;
			var_delete( hVar, *s );
			}
		strdcpy( &w, var_prefix, "_config_ok", _ );
		var_delete( hVar, w );

		file_tmpname(fname1);
		fp = fopen( *fname1, "wb" );
		fprint( fp, v, BR, _ );
		fclose( fp );
		
		
		//----------------------------------------------------------------------
		// GENERE UN CSR
		//----------------------------------------------------------------------
		
		fp = fopen( *fname2, "wb" );

		fprint( fp, "[ req ]" BR
					"distinguished_name         = req_distinguished_name" BR
					"prompt                     = no" BR
					"[ req_distinguished_name ]" BR
					"countryName                = FR" BR
					"stateOrProvinceName        = PARIS" BR
					"localityName               = PARIS" BR
					"organizationName           = ELECTRICITE DE FRANCE" BR
					"commonName                 = ", certif, BR
					, _ );

		fclose( fp );

		file_tmpname(fname);

		strdcpy( cmd, ssl_pgm_fname, " req -out ", *fname,
				" -new -config ", *fname2, " -batch -key ", *fname1, _ );

		System( s, *cmd );
	
		unlink( *fname2 );

		if ( ! file_exists( *fname ) )
			{
			error_set( 1, "ERREUR : Problème génération d'un CSR : ", *s, _ );
			unlink( *fname1 );
			return ERROR;
			}
	

		file_load( s, *fname );
		unlink( *fname );
		
		strdcpy( &w, var_prefix, "_csr", _ );
		var_set( hVar, w, *s );
		}
	else
		{
		file_tmpname(fname1);
		fp = fopen( *fname1, "wb" );
		fprint( fp, v, BR, _ );
		fclose( fp );
		}
	
	
	
	//--------------------------------------------------------------------------
	// Modulus de la clef privée (permet de vérifier le certificat serveur)
	//--------------------------------------------------------------------------

	System( s2, strdcpy( cmd, ssl_pgm_fname, " rsa -noout -modulus -inform PEM -in ", *fname1, _ ) );\

	
	
	
	//--------------------------------------------------------------------------
	// CHARGE LE CERTIFICAT SERVEUR
	//--------------------------------------------------------------------------
	
	n = 1;
	if ( certif )
		{
		//----------------------------------------------------------------------
		// LE CERTIF EST-IL DANS LES PARAMETRES
		//----------------------------------------------------------------------
		for( p = 0 ; ; )
			{
			p = strpos( certif, "-----BEGIN CERTIFICATE", p );
			if ( p < 0 ) break;
			p1 = strpos( certif, "-----END CERTIFICATE-----", p );
			if ( p1 < 0 ) break;
			ssl_pubpriv_verify_pem( substr( certif, p, p1+25-p ) );
			if ( r ) break;
			p = p1 + 25;
			}
		}

		
	if ( ( certif )&&( file_exists( certif ) )&&( !dir_exists( certif ) ) )
		certif = file_getdir( certif );
	

	if ( ( certif )&&( dir_exists( certif ) ) )
		{
		//----------------------------------------------------------------------
		// LE CERTIF EST-IL DANS UN FICHIER
		//----------------------------------------------------------------------
		tf = null;
		strdcpy( fname, certif, "/*", _ );
		while( tf = file_list( tf, *fname, "nodir absdir" ) )
			{
			if ( *s ) (*s)[0] = 0 ;
			strdcpy( fname, tf->dir, "/", tf->name, _ );
			if ( file_size( *fname ) > (512 Ko) ) continue;
			if ( file_load( s, *fname ) == sERROR ) continue;
			
			if ( strpos( *s, "-----BEGIN CERTIFICATE", 0 ) < 0 )
				{
				ssl_pubpriv_verify_der( *fname );
				}
			else
			for( p = 0 ; ; )
				{
				p = strpos( *s, "-----BEGIN CERTIFICATE", p );
				if ( p < 0 ) break;
				p1 = strpos( *s, "-----END CERTIFICATE-----", p );
				if ( p1 < 0 ) break;
				ssl_pubpriv_verify_pem( substr( *s, p, p1+25-p ) );
				if ( r ) break;
				
				p = p1 + 25;
				}
			}
		}
	

	strdcpy( &w, var_prefix, "_certif_server", _ );
	if ( ! ( v1 = var_get( hVar, w ) ) )
		{
		//----------------------------------------------------------------------
		error_set( 1, "ERREUR : le certificat serveur n'a pas été fourni", _ );
		//----------------------------------------------------------------------
		
		unlink( *fname1 );
		unlink( *fname2 );
		return ERROR;
		}
	



	//--------------------------------------------------------------------------
	// CHARGE LES CA
	//--------------------------------------------------------------------------
	
	strdcpy( &w, var_prefix, "_certif_server_from", _ );
	v1 = var_get( hVar, w );
	strdcpy( s3, var_prefix, "_certif_ca", itoa( n ), _ );

	
	for( n = 1 ; ; n++ )
		{
		r = false;
		
		//----------------------------------------------------------------------
		// RECHERCHE DANS LES PARAMETRES FOURNIS
		//----------------------------------------------------------------------
		if ( certif )
			{
			for( p = 0 ; ; )
				{
				p = strpos( certif, "-----BEGIN CERTIFICATE", p );
				if ( p < 0 ) break;
				p1 = strpos( certif, "-----END CERTIFICATE-----", p );
				if ( p1 < 0 ) break;
				ssl_ca_verify_pem( substr( certif, p, p1+25-p ) );
				if ( r ) break;
				
				p = p1 + 25;
				}
			}
	
		//----------------------------------------------------------------------
		// RECHERCHE DANS LE REPERTOIRE DU CERTIF FOURNI
		//----------------------------------------------------------------------
		if ( ( !r )&&( certif )&&( dir_exists( certif ) ) )
			{
			tf = null;
			strdcpy( fname, certif, "/*", _ );
			
			while( tf = file_list( tf, *fname, "nodir absdir" ) )
				{
				if ( *s ) (*s)[0] = 0 ;
				strdcpy( fname, tf->dir, "/", tf->name, _ );
				if ( file_size( *fname ) > 512 Ko ) continue;
				if ( file_load( s, *fname ) == sERROR ) continue;

				
				if ( strpos( *s, "-----BEGIN CERTIFICATE", 0 ) < 0 )
					{
					ssl_ca_verify_der( *fname );
					}
				else
				for( p = 0 ; ; )
					{
					p = strpos( *s, "-----BEGIN CERTIFICATE", p );
					if ( p < 0 ) break;
					p1 = strpos( *s, "-----END CERTIFICATE-----", p );
					if ( p1 < 0 ) break;
					ssl_ca_verify_pem( substr( *s, p, p1+25-p ) );
					if ( r ) break;
					
					p = p1 + 25;
					}
				if ( r )
					{
					file_list_free( tf );
					break;
					}
				}
			}

		
		//----------------------------------------------------------------------
		// RECHERCHE DANS LE REPERTOIRE BIBLIOTHEQUE DE CERTIF
		//----------------------------------------------------------------------
		if ( ( !r )&&( certif_lib )&&( dir_exists( certif_lib ) ) )
			{
			tf = null;
			strdcpy( fname, certif_lib, "/*", _ );
			
			while( tf = file_list( tf, *fname, "nodir absdir" ) )
				{
				if ( *s ) (*s)[0] = 0 ;
				strdcpy( fname, tf->dir, "/", tf->name, _ );
				if ( file_size( *fname ) > 512 Ko ) continue;
				if ( file_load( s, *fname ) == sERROR ) continue;
				
				if ( strpos( *s, "-----BEGIN CERTIFICATE", 0 ) < 0 )
					{
					ssl_ca_verify_der( *fname );
					}
				else
				for( p = 0 ; ; )
					{
					p = strpos( *s, "-----BEGIN CERTIFICATE", p );
					if ( p < 0 ) break;
					p1 = strpos( *s, "-----END CERTIFICATE-----", p );
					if ( p1 < 0 ) break;
					ssl_ca_verify_pem( substr( *s, p, p1+25-p ) );
					if ( r ) break;
					
					p = p1 + 25;
					}
				if ( r )
					{
					file_list_free( tf );
					break;
					}
				}
			}

		
		if ( ! ( v1 = var_get( hVar, *s3 ) ) )
			{
			//------------------------------------------------------------------
			error_set( 1, "ERREUR : les CA n'ont pas tous été fournis", _ );
			//------------------------------------------------------------------
			
			unlink( *fname1 );
			unlink( *fname2 );
			return ERROR;
			}


		//----------------------------------------------------------------------
		// VERIFIE SI C'EST LE CERTIFICAT ROOT
		//----------------------------------------------------------------------
		
		v1 = var_get( hVar,	strdcpy( s, *s3, "_from", _ ) );
		v = var_get( hVar,	strdcpy( s, *s3, "_to", _ ) );
		if ( ! strcmp( v, v1 ) )
			{
			//------------------------------------------------------------------
			// OUI => MET A JOUR LA BIBLIOTHEQUE DE CERTIFICAT ET FIN
			//------------------------------------------------------------------
			
			strdcpy( &w, var_prefix, "_config_ok", _ );
			var_set( hVar, w, "true" );
			ssl_build_certif_lib( hVar, certif_lib );
			break;
			}
		
		//----------------------------------------------------------------------
		// RECHERCHE DE CERTIFICATEUR SUIVANT
		//----------------------------------------------------------------------
		
		strdcpy( s3, var_prefix, "_certif_ca", itoa( n+1 ), _ );
		}
	
		
	//--------------------------------------------------------------------------
	// FIN DU CHARGEMENT
	//--------------------------------------------------------------------------
	
	unlink( *fname1 );
	unlink( *fname2 );
	
	return OK;
	}





	
//------------------------------------------------------------------------------
int		ssl_certif_load( handle hVar, char *certif, char *certif_lib, char *var_prefix )
//------------------------------------------------------------------------------
	{
	char *v, *v1, **cmd = memptr(0), **s = memptr(0), **s1 = memptr(0),
		**s2 = memptr(0), **s3 = memptr(0), *certif0,
		**fname = memptr(0), **fname1 = memptr(0), **fname2 = memptr(0), *w = null;
	int p, p1, p2, p3, r, n;
	FILE *fp;
	T_FILE_INFO *tf;
	
	
	if ( ! ssl_pgm( null ) )
		{
		error_set( 1, "ERREUR : Le programme openssl n'a pas été trouvé", _ );
		return ERROR;
		}
	
	file_tmpname(fname2);

	
	//--------------------------------------------------------------------------
	// CHARGE LE CERTIFICAT SERVEUR
	//--------------------------------------------------------------------------

	strdcpy( &w, var_prefix, "_config_ok", _ );
	var_delete( hVar, w );
	strdcpy( &w, var_prefix, "_certif_server", _ );
	var_delete( hVar, w );
	strdcpy( &w, var_prefix, "_certif_server_from", _ );
	var_delete( hVar, w );

	
	if ( ( certif )&&( file_exists( certif ) )&&( !dir_exists( certif ) ) )
		{
		//----------------------------------------------------------------------
		// LE CERTIFICAT EST-IL DANS UN FICHIER
		//----------------------------------------------------------------------
		System( s, strdcpy( cmd, ssl_pgm_fname, " x509 -text -inform PEM -in ", certif, _ ) );
		strdcpy( &w, var_prefix, "_certif_server", _ );
		ssl_prop_load( hVar, w, *s );
		
		certif = file_getdir( certif );
		}
	else
		//----------------------------------------------------------------------
		// LE CERTIFICAT EST-IL FOURNI EN PARAMETRE
		//----------------------------------------------------------------------
		for( p = 0 ; ; )
			{
			p = strpos( certif, "-----BEGIN CERTIFICATE", p );
			if ( p < 0 ) break;
			p1 = strpos( certif, "-----END CERTIFICATE-----", p );
			if ( p1 < 0 ) break;

			file_save_str( *fname2, substr( certif, p, p1+25-p ), -1 );			
			System( s, strdcpy( cmd, ssl_pgm_fname, " x509 -text -inform PEM -in ", *fname2, _ ) );
			strdcpy( &w, var_prefix, "_certif_server", _ );
			ssl_prop_load( hVar, w, *s );
			break;
			}
	
	
	strdcpy( &w, var_prefix, "_certif_server", _ );
	if ( ! ( v1 = var_get( hVar, w ) ) )
		{
		//----------------------------------------------------------------------
		error_set( 1, "ERREUR : le certificat serveur n'a pas été fourni", _ );
		//----------------------------------------------------------------------
		return ERROR;
		}

	
	strdcpy( &w, var_prefix, "_config_ok", _ );
	var_delete( hVar, w );
	
	
	
	//--------------------------------------------------------------------------
	// CHARGE LES CA
	//--------------------------------------------------------------------------
	
	strdcpy( &w, var_prefix, "_certif_server_from", _ );
	v1 = var_get( hVar, w );
	strdcpy( &w, var_prefix, "_certif_server_to", _ );
	v = var_get( hVar, w );
	if ( !strcmp( v1, v ) )
		{
		strdcpy( &w, var_prefix, "_certif_ca1_from", _ );
		var_set( hVar, w, v1 );
		strdcpy( &w, var_prefix, "_certif_ca1_to", _ );
		var_set( hVar, w, v );
		strdcpy( &w, var_prefix, "_certif_server_enddate", _ );
		v = var_get( hVar, w );
		strdcpy( &w, var_prefix, "_certif_ca1_enddate", _ );
		var_set( hVar, w, v );
		strdcpy( &w, var_prefix, "_certif_server", _ );
		v = var_get( hVar, w );
		strdcpy( &w, var_prefix, "_certif_ca1", _ );
		var_set( hVar, w, v );
		strdcpy( &w, var_prefix, "_certif_server", _ );
		var_delete( hVar, w );
		return OK;
		}
		
	strdcpy( s3, var_prefix, "_certif_ca1", _ );
	
	for( n = 1 ; ; n++ )
		{
		if ( ( certif )&&( dir_exists( certif ) ) )
			{
			tf = null;
			strdcpy( fname, certif, "/*", _ );
			
			while( tf = file_list( tf, *fname, "nodir absdir" ) )
				{
				strdcpy( fname, tf->dir, "/", tf->name, _ );

				strdcpy( s, *s3, "_to", _ );
				var_delete( hVar, *s );
				if ( file_size( *fname ) > 512 Ko ) continue;
				if ( file_load( s, *fname ) == sERROR ) continue;
				if ( strpos( *s, "-----BEGIN CERTIFICATE", 0 ) < 0 )
					{
					System( s, strdcpy( cmd, ssl_pgm_fname, " x509 -text -inform DER -in ", *fname, _ ) );
					if ( ssl_prop_load( hVar, *s3, *s ) == ERROR )
						continue;
					}
				else
				for( p = 0 ; ; )
					{
					p = strpos( *s, "-----BEGIN CERTIFICATE", p );
					if ( p < 0 ) break;
					p1 = strpos( *s, "-----END CERTIFICATE-----", p );
					if ( p1 < 0 ) break;

					file_save_str( *fname2, substr( *s, p, p1+25-p ), -1 );			
					System( s1, strdcpy( cmd, ssl_pgm_fname, " x509 -text -inform PEM -in ", *fname2, _ ) );
					ssl_prop_load( hVar, *s3, *s1 );
					p = p1 + 25;
					strdcpy( s1, *s3, "_to", _ );
					v = var_get( hVar, *s1 );
					if ( !v ) continue;
					if ( ! strcmp( v1, v ) ) break;
					}

				strdcpy( s, *s3, "_to", _ );
				v = var_get( hVar, *s );
				if ( !v ) continue;

				if ( ! strcmp( v1, v ) ) break;
				}
			if ( ( !v )||( strcmp( v1, v ) ) )
				var_delete( hVar, *s3 );
			}
		else
			{
			for( p = 0 ; ; )
				{
				p = strpos( certif, "-----BEGIN CERTIFICATE", p );
				if ( p < 0 ) break;
				p1 = strpos( certif, "-----END CERTIFICATE-----", p );
				if ( p1 < 0 ) break;

				file_save_str( *fname2, substr( certif, p, p1+25-p ), -1 );			
				System( s1, strdcpy( cmd, ssl_pgm_fname, " x509 -text -inform PEM -in ", *fname2, _ ) );
				ssl_prop_load( hVar, *s3, *s1 );
				strdcpy( s1, *s3, "_to", _ );

				p = p1 + 25;

				v = var_get( hVar, *s1 );
				if ( !v ) continue;
				if (( !v1 )||( ! strcmp( v1, v ) )) break;
				}
				
			strdcpy( s, *s3, "_to", _ );
			v = var_get( hVar, *s );
			if ( ( !v )||(( v1 )&&( strcmp( v1, v ) )) )
				var_delete( hVar, *s3 );
			}
		
		if ( ! var_get( hVar, *s3 ) )
			{
			if ( ( certif_lib )&&( certif_lib != certif ) )
				{
				certif0 = certif;
				certif = certif_lib;
				n--;
				continue;
				}
			error_set( 1, "ERREUR : les CA n'ont pas tous été fournis", _ );
			return ERROR;
			}

		v1 = var_get( hVar,	strdcpy( s, *s3, "_from", _ ) );
		v = var_get( hVar,	strdcpy( s, *s3, "_to", _ ) );
		if ( ! strcmp( v, v1 ) )
			{
			strdcpy( &w, var_prefix, "_config_ok", _ );
			var_set( hVar, w, "true" );
			ssl_build_certif_lib( hVar, certif_lib );
			break;
			}
		
		strdcpy( &w, var_prefix, "_certif_ca", _ );
		strdcpy( s3, w, itoa( n+1 ), _ );
		if ( ( certif_lib )&&( certif_lib == certif ) )
			certif = certif0;
		}
	
	return OK;
	}




//------------------------------------------------------------------------------
int ssl_end_delay( handle hVar, char *var_prefix ) // Retourne le nombre de secondes avant expiration de l'un des certificats
//------------------------------------------------------------------------------
	{
	char *v;
	
	v = var_get( hVar, str( var_prefix, "_certif_server_endtime", _ ) );
	if ( !v ) return 0;
	return atoi(v) - time(NULL);
	}




//------------------------------------------------------------------------------
char * ssl_encrypt( char **p_out, handle hSslConf, char *in, char *var_prefix )
//------------------------------------------------------------------------------
	{
	char *f_in = null, *f_out = null, *f_cert = null,
			*cmd = null;
	char *v;
	
	v = var_get( hSslConf, str( var_prefix, "_certif_server", _ ) );
	if ( ! v ) return null;
	file_tmpname(&f_cert);
	file_save_str( f_cert, v, -1 );
	
	
	file_tmpname(&f_in);
	file_save_str( f_in, in, -1 );
	
	file_tmpname(&f_out);
	
	strdcpy( &cmd, ssl_pgm_fname, " smime -encrypt -aes256 -in ", f_in,
				" -out ", f_out, " ", f_cert, _ );
	
	System( p_out, cmd );
	if ( !file_exists( f_out ) )
		{
		error_set( 1, *p_out, _ );
		if ( f_in ) free( f_in );
		if ( f_out ) free( f_out );
		if ( f_cert ) free( f_cert );
		if ( cmd ) free( cmd );
		return null;
		}
	
	file_load( p_out, f_out );
	unlink( f_in );
	unlink( f_out );
	unlink( f_cert );
	
	if ( f_in ) free( f_in );
	if ( f_out ) free( f_out );
	if ( f_cert ) free( f_cert );
	if ( cmd ) free( cmd );
	
	return *p_out;
	}




//------------------------------------------------------------------------------
char * ssl_decrypt( char **p_out, handle hSslConf, char *in, char *var_prefix )
//------------------------------------------------------------------------------
	{
	char *f_in = null, *f_out = null, *f_cert = null,
		*f_key = null, *cmd = null;
	char *v;
	
	v = var_get( hSslConf, str( var_prefix, "_certif_server", _ ) );
	if ( ! v ) return null;

	file_tmpname(&f_cert);
	file_save_str( f_cert, v, -1 );
	

	v = var_get( hSslConf, str( var_prefix, "_private_key", _ ) );
	if ( ! v ) { unlink( f_cert ); return null; }

	file_tmpname(&f_key);
	file_save_str( f_key, v, -1 );
	
	
	file_tmpname(&f_in);
	file_save_str( f_in, in, -1 );
	
	file_tmpname(&f_out);
	
	strdcpy( &cmd, ssl_pgm_fname, " smime -decrypt -in ", f_in,
				" -out ", f_out, " -recip ", f_cert, " -inkey ", f_key, _ );
	
	System( p_out, cmd );
	
	unlink( f_key );
	unlink( f_in );
	unlink( f_cert );
	
	if ( !file_exists( f_out ) )
		{
		error_set( 1, *p_out, _ );
		if ( f_in ) free( f_in );
		if ( f_out ) free( f_out );
		if ( f_cert ) free( f_cert );
		if ( cmd ) free( cmd );
		return null;
		}
	

	file_load( p_out, f_out );

	unlink( f_out );
	
	if ( f_in ) free( f_in );
	if ( f_out ) free( f_out );
	if ( f_cert ) free( f_cert );
	if ( cmd ) free( cmd );
	
	return *p_out;
	}


//------------------------------------------------------------------------------
char *	ssl_encrypt_file( handle hPrivate, handle hPublic, char *src_fname, char *res_fname, char *var_prefix )
//------------------------------------------------------------------------------
	{
	char *v, *s = null, *fname = null, *fname1 = null;
	
	v = var_get( hPrivate, str( var_prefix, "_private_key", _ ) );
	strdcpy( &fname, res_fname, ".tmp.key", _ );
	file_save_str( fname, v, -1 );

	v = var_get( hPublic, str( var_prefix, "_certif_server", _) );
	strdcpy( &fname, res_fname, ".tmp.pubcer", _ );
	file_save_str( fname, v, -1 );
	
	
	strdcpy( &s,
		"echo Generation de la clef de chiffrement de session ..." BR,
		ssl_pgm(null), " rand 3000 > ", res_fname, ".tmp.rnd1" BR,
		ssl_pgm(null), " dgst -hex -sha1 ", res_fname, ".tmp.rnd1 > ", res_fname, ".tmp.rnd2" BR,
		"for /F \"tokens=1,2\" %%a in (", res_fname, ".tmp.rnd2) do SET cfrnd1=%%b" BR,
		
		ssl_pgm(null), " rand 3000 > ", res_fname, ".tmp.rnd1" BR,
		ssl_pgm(null), " dgst -hex -sha1 ", res_fname, ".tmp.rnd1 > ", res_fname, ".tmp.rnd2" BR,
		"for /F \"tokens=1,2\" %%a in (", res_fname, ".tmp.rnd2) do SET cfrnd2=%%b" BR,
		"SET cfrnd2=%cfrnd2:~0,16%" BR,
		
		"echo Chiffrement du fichier ", src_fname, " ..." BR,
		ssl_pgm(null), " aes-256-cbc -e -K %cfrnd1% -iv %cfrnd2% -in ", src_fname, " -out ", res_fname, ".dat" BR,
		
		"echo Chiffrement de la clef de session ..." BR,
		"echo %cfrnd1% %cfrnd2% > ", res_fname, ".tmp.asess.dat" BR,
		
		ssl_pgm(null), " x509 -in ", res_fname, ".tmp.pubcer -pubkey >", res_fname, ".tmp.pubkey" BR,
		ssl_pgm(null), " rsautl -encrypt -in ", res_fname, ".tmp.asess.dat -out ", res_fname, ".session.dat -inkey ", res_fname, ".tmp.pubkey -pubin" BR,
		
		"echo Generation de la signature ..." BR,
		ssl_pgm(null), " dgst -sha512 -sign ", res_fname, ".tmp.key -out ", res_fname, ".digest.dat ", res_fname, ".dat" BR,
		_ );
	
	strdcpy( &fname, res_fname, ".tmp.bat", _ );
	file_save_str( fname, s, -1 );
	
	system( fname );

	strdcpy( &fname, res_fname, ".tmp.*", _ );
	file_delete( fname );
	
	strdcpy( &fname, res_fname, ".digest.dat", _ );
	if ( ! file_exists( fname ) ) return sERROR;

	strdcpy( &fname, res_fname, ".session.dat", _ );
	if ( ! file_exists( fname ) ) return sERROR;

	strdcpy( &fname, res_fname, ".dat", _ );
	if ( ! file_exists( fname ) ) return sERROR;

	return OK;
	}

//------------------------------------------------------------------------------
char *	ssl_decrypt_file( handle hPrivate, handle hPublic, char *src_fname, char *res_fname, char *var_prefix )
//------------------------------------------------------------------------------
	{
	}















//------------------------------------------------------------------------------
char* trim( char* s )
//------------------------------------------------------------------------------
	{
	int i, j;

	if ( !s ) return null;
	
	if ( s[0] )
		{
		i = strlen(s)-1;
		for( ; ( i >= 0 )
				&&( ( s[i] == ' ' )||( s[i] == '	' )||( s[i] == 13 )||( s[i] == 10 ) )
			 ; i-- );
		s[i+1] = 0;
		
		for( i = 0 ; ( s[i] )
				&&( ( s[i] == ' ' )||( s[i] == '	' )||( s[i] == 13 )||( s[i] == 10 ) )
			 ; i++ );
			 
		if ( i )
			{
			for( j = 0 ; s[i] ; i++, j++ ) s[j] = s[i];
			s[j] = 0;
			}
			
//			{ strcpy( s, &(s[i]) ); s[l-i] = 0; }
		}

	return s;
	}



//------------------------------------------------------------------------------
char* lower( char* s )
//------------------------------------------------------------------------------
	{
	char *o;
	if ( ! s ) return null;
	for( o = s ; *o ; o++ )
		if ( ( *o >= 'A' ) && ( *o <= 'Z' ) )
			*o += 'a' - 'A';

	return s;
	}

//------------------------------------------------------------------------------

uchar elib_upper_table[256] =
	{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	65, 225, 65, 227, 65, 229, 230, 67, 69, 69, 69, 69, 236, 237, 73, 73,
	240, 241, 242, 243, 79, 245, 79, 247, 248, 85, 250, 85, 85, 253, 254, 255
	};



//------------------------------------------------------------------------------
char* upper( char* s )
//------------------------------------------------------------------------------
	{
	char *o;

	if ( ! s ) return null;
	
	for( o = s ; *o ; o++ )
		*o = elib_upper_table[((uchar)*o)];
		
	return s;
	}


//------------------------------------------------------------------------------
bool	is_numeric( char* v )
//------------------------------------------------------------------------------
	{
	int i, o = 0;

	if ( (!v) || (!v[0]) ) return false;

	i = strlen(v)-1;
	if ( v[0] == '-' ) o = 1;

	for( ; i >= o ; i-- )
		if ( ( v[i] < '0' )||( v[i] > '9' ) ) return false;
	return true;
	}
	
	
//------------------------------------------------------------------------------
bool	is_alphanum( char* in, char *comp )
//------------------------------------------------------------------------------
	{
	int i, j;
	
	if ( !in ) return false;
	
	for( i = 0 ; in[i] ; i++ )
		{
		if ( ( in[i] >= 'A' )&&( in[i] <= 'Z' ) ) continue;
		if ( ( in[i] >= '0' )&&( in[i] <= '9' ) ) continue;
		if ( ( in[i] >= 'a' )&&( in[i] <= 'z' ) ) continue;
		if ( !comp ) return false;

		for( j = 0 ; comp[j] ; j++ )
			if ( in[i] == comp[j] ) break;
		if ( !comp[j] ) return false;
		}
			
	return true;	
	}
	
	
//------------------------------------------------------------------------------
char* itoa( long long  v )
//------------------------------------------------------------------------------
	{
	char *o = memget(200);

	sprintf( o, "%lld", v );
	return o;
	}


//------------------------------------------------------------------------------
char* btoa( char v )
//------------------------------------------------------------------------------
	{
	char *o = memget(200);
	int i, j = 128;
	
	for( i = 0 ; i < 8 ; i++, j = j >> 1 )
		o[i] = ( v & j ) ? '1' : '0';
	o[i] = 0;
	
	return o;
	}


//------------------------------------------------------------------------------
char* itona( long long v, int d )
//------------------------------------------------------------------------------
	{
	char *o = memget(200), s[20];
	int c, l;

	sprintf( s, "%lld", v );
	if ( d )
		{
		c = d-(l=strlen(s));
		if ( c >= 0 )
			{
			memcpy( &(o[c]), s, l+1 );
			for( l = d-l-1 ; l >= 0 ; l-- ) o[l] = '0';
			}
		}
	else
		strcpy( o, s );
	
	return o;
	}

//------------------------------------------------------------------------------
char* utona( uint v, int d )
//------------------------------------------------------------------------------
	{
	char *o = memget(16);
	int c, l;

	sprintf( o, "%u", v );
	if ( d )
		{
		c = d-(l=strlen(o));
		if ( c >= 0 )
			{
			memcpy( &(o[c]), o, l+1 );
			for( l = d-l-1 ; l >= 0 ; l-- ) o[l] = '0';
			}
		}
	
	return o;
	}

//------------------------------------------------------------------------------
char* ito36( int v, int d )
//------------------------------------------------------------------------------
	{
	static char t_ito36[36];
	static bool b_ito36_init = false;
	char *o = memget(200);
	int i = 0;

	BeginCriticalSection();
	if ( ! b_ito36_init )
		{
		t_ito36[i++] = '0';
		t_ito36[i++] = '1';
		t_ito36[i++] = '2';
		t_ito36[i++] = '3';
		t_ito36[i++] = '4';
		t_ito36[i++] = '5';
		t_ito36[i++] = '6';
		t_ito36[i++] = '7';
		t_ito36[i++] = '8';
		t_ito36[i++] = '9';
		t_ito36[i++] = 'A';
		t_ito36[i++] = 'B';
		t_ito36[i++] = 'C';
		t_ito36[i++] = 'D';
		t_ito36[i++] = 'E';
		t_ito36[i++] = 'F';
		t_ito36[i++] = 'G';
		t_ito36[i++] = 'H';
		t_ito36[i++] = 'I';
		t_ito36[i++] = 'J';
		t_ito36[i++] = 'K';
		t_ito36[i++] = 'L';
		t_ito36[i++] = 'M';
		t_ito36[i++] = 'N';
		t_ito36[i++] = 'O';
		t_ito36[i++] = 'P';
		t_ito36[i++] = 'Q';
		t_ito36[i++] = 'R';
		t_ito36[i++] = 'S';
		t_ito36[i++] = 'T';
		t_ito36[i++] = 'U';
		t_ito36[i++] = 'V';
		t_ito36[i++] = 'W';
		t_ito36[i++] = 'X';
		t_ito36[i++] = 'Y';
		t_ito36[i++] = 'Z';
		b_ito36_init = true;
		}
	EndCriticalSection();
		
	i = d-1;
	while( ( i >= 0 )&&( v >= 36 ) )
		{
		o[i--] = t_ito36[(uchar)(((uint)v) % 36)];
		v /= 36;
		}
	o[i--] = t_ito36[(uchar)(((uint)v) % 36)];
	for( ; i >=0 ; i-- ) o[i] = '0';
	o[d] = 0;
	return o;
	}


//------------------------------------------------------------------------------
int ifrom36( char *s )
//------------------------------------------------------------------------------
	{
	static int t_36toi[256];
	static bool b_36toi_init = false;
	int r = 0, i = 0, n, j;

	BeginCriticalSection();

	if ( ! b_36toi_init )
		{
		t_36toi['0'] = i++;
		t_36toi['1'] = i++;
		t_36toi['2'] = i++;
		t_36toi['3'] = i++;
		t_36toi['4'] = i++;
		t_36toi['5'] = i++;
		t_36toi['6'] = i++;
		t_36toi['7'] = i++;
		t_36toi['8'] = i++;
		t_36toi['9'] = i++;
		t_36toi['A'] = i++;
		t_36toi['B'] = i++;
		t_36toi['C'] = i++;
		t_36toi['D'] = i++;
		t_36toi['E'] = i++;
		t_36toi['F'] = i++;
		t_36toi['G'] = i++;
		t_36toi['H'] = i++;
		t_36toi['I'] = i++;
		t_36toi['J'] = i++;
		t_36toi['K'] = i++;
		t_36toi['L'] = i++;
		t_36toi['M'] = i++;
		t_36toi['N'] = i++;
		t_36toi['O'] = i++;
		t_36toi['P'] = i++;
		t_36toi['Q'] = i++;
		t_36toi['R'] = i++;
		t_36toi['S'] = i++;
		t_36toi['T'] = i++;
		t_36toi['U'] = i++;
		t_36toi['V'] = i++;
		t_36toi['W'] = i++;
		t_36toi['X'] = i++;
		t_36toi['Y'] = i++;
		t_36toi['Z'] = i++;
		b_36toi_init = true;
		i = 0;
		}
	EndCriticalSection();

	j = 1;	
	for( i = strlen(s)-1 ; i >= 0 ; i--, j *= 36 )
		{
		r += j * t_36toi[s[i]];
		}

	return r;
	}


//------------------------------------------------------------------------------
char* utoa( uint v )
//------------------------------------------------------------------------------
	{
	char *o = memget(200);

	sprintf( o, "%u", v );
	return o;
	}

//------------------------------------------------------------------------------
bool	is_base_36( char* s36 )
//------------------------------------------------------------------------------
	{
	int i = 0;
	
	for( ; s36[i] ; i++ )
		{
		if (( s36[i] >= '0' )&&( s36[i] <= '9' )) continue;
		if (( s36[i] >= 'A' )&&( s36[i] <= 'Z' )) continue;
		return false;
		}
	
	return true;
	}


//------------------------------------------------------------------------------
char *	substr( char *in, int pos, int len )
//------------------------------------------------------------------------------
	{
	char *o;
	int l;
	
	if ( !in ) return "";
	
	l = strlen(in);

	if ( pos < 0 )
		{ pos = l + pos;	if ( pos < 0 ) pos = 0; }
	if ( len < 0 ) len = l - pos;
	if ( pos + len > l ) len = l - pos;
	if ( ( len <= 0 )||( pos > l ) )
		return "";
	
	o = memget(len+1);
	strncpy( o, &(in[pos]), len );
	return o;
	}


//------------------------------------------------------------------------------				
int strncmp( char *s1, char *s2, int n )
//------------------------------------------------------------------------------				
	{
	int i = 0, d;
	
	if ( n < 0 ) n = strlen( s2 );
	for( n-- ; ( s1[i] )&&( s2[i] ) ; i++ )
		{
		if ( d = ( s1[i] - s2[i] ) ) return d;
		if ( i >= n ) return 0;
		}
	
	return s1[i] - s2[i];
	}

//------------------------------------------------------------------------------				
int stricmp( char *s1, char *s2 )
//------------------------------------------------------------------------------				
	{
	int i = 0, d;

	for(  ; ( s1[i] )&&( s2[i] ) ; i++ )
		if ( d = ( elib_upper_table[(uchar)s1[i]]
				- elib_upper_table[(uchar)s2[i]] ) ) return d;

	return elib_upper_table[(unsigned char)s1[i]]
				- elib_upper_table[(unsigned char)s2[i]];
	}

//------------------------------------------------------------------------------				
int strnicmp( char *s1, char *s2, int n )
//------------------------------------------------------------------------------				
	{
	int i = 0, d;

	if ( n < 0 ) n = strlen( s2 );
	for( n-- ; ( s1[i] )&&( s2[i] ) ; i++ )
		{
		if ( d = elib_upper_table[(unsigned char)s1[i]]
				- elib_upper_table[(unsigned char)s2[i]] )
			return d;
		if ( i >= n ) return 0;
		}
	
	return elib_upper_table[(unsigned char)s1[i]]
				- elib_upper_table[(unsigned char)s2[i]];
	}


//------------------------------------------------------------------------------				
bool strjncmp( char *in1, char *in2, int n )
//------------------------------------------------------------------------------				
	{
	for( ; ; in1++, in2++, n-- )
		{
		if ( !n ) return true;
		if ( *in1 != *in2 )
			{
			if ( *in1 != '?' ) return false;
			if (( !(*in1) )||( !(*in2) )) return false;
			continue;
			}
		
		if ( !(*in1) ) return true;
		}
	}
	
//------------------------------------------------------------------------------				
bool strjcmp( char *in1, char *in2 )
//------------------------------------------------------------------------------				
	{
	for( ; ; in1++, in2++ )
		{
		if ( *in1 != *in2 )
			if ( *in1 != '?' ) return false;
		
		if ( !(*in1) ) return true;
		}
	}



#define p_in (*p_in_out)

//------------------------------------------------------------------------------				
int strRegExpr( char *p_pattern /*ne doit pas être une constante*/,
				char **p_in_out, int *pIn, int length,
				char bSearch, char bTild, char Level,
				int *nbReplace )
//------------------------------------------------------------------------------				
	{
	int  pPatt			// Position de lecture du pattern
		,StartPos		// Position de relecture pour les OU
		,pModl			// Position de début de définition du modèle
		,pModlE			// Position de fin de définition du modèle
		,nbOccur		// Nb d'occurence demandé ?N ou *N
		,n, p			// Variables de travail temporaire
		,p0				// Position à remettre si recherche OK
		,pr0 = -1, pr1 = -1		// Positions début et fin du mot trouvé
		;
	
	char c, c0, c1, bInQuote = false, bNot = false, rc = true, *w = null;
static char *s = null;

	tr( "strRegExpr" );
	tr2( "p_pattern=", p_pattern );
	tr2( "pIn=", itoa(*pIn) );
	tr2( "bSearch=", itoa(bSearch) );
	
	if ( !(p_pattern[0]) )
		{
		tr( "strRegExpr0" );
		if ( p_in[*pIn] ) { (*pIn)++; return true; }
		return false;
		}

strdcpy( &s, " ", _ );
	
	p0 = StartPos = *pIn;	
	pModl		= -1; // Tous les caractères sont autorisés
	
	
	if (( bSearch )&&( Level == 0 )&&( p_pattern[0] != '*' ))
		{
		nbOccur = 999999999;
		pPatt = -1;
//		c = '*';
		goto search;
		}
	
	for( pPatt = 0 ; c = p_pattern[pPatt] ; pPatt++ )
		// Boucle de lecture du pattern ou d'un modèle
		{
		tr( "strRegExpr1" );
		tr2( "pPatt=", itoa(pPatt) );
		tr2( "pIn=", itoa(*pIn) );
		s[0] = c;
		tr2( "c=", s );

		switch( c )
			{
			case '"' :
			case '\'' :
				tr( "strRegExpr2:case \"" );
				if ( !bInQuote )
					{
					tr( "strRegExpr3" );
					bInQuote = c;
					continue;
					}
				if ( c == bInQuote )
					{
					tr( "strRegExpr4" );
					bInQuote = false;
					continue;
					}
				goto defaut;
				
				
			case ' ' :
			case '\t' :
			case '\n' :
			case '\r' :
				
				tr( "strRegExpr5:case espace" );
				if ( bInQuote ) goto defaut;
				continue; // Ignore les espaces et tabulation
				
				
			
			case '~' :
				
				tr( "strRegExpr55:case ~" );
				if ( bInQuote ) goto defaut;
				bTild = true;
				continue; // Ignore la casse
				
				
			
			case '!' :
				
				tr( "strRegExpr50:case !" );
				if ( bInQuote ) goto defaut;
				bNot = true;
				continue;
				
				
			
			case '[': // Définition de sous-modèle
				
				tr( "strRegExpr6:case [" );
				if ( bInQuote ) goto defaut;
				
				pModl = ++pPatt;
				// cherche ] de fin
				for( n = 0 /*nb crochet ouvrant*/, bInQuote = false, c0 = 0
						; c = p_pattern[pPatt] ; pPatt++, c0 = c )
					{
					if (( c == '[' )&&( c0 != '\\' )&&( !bInQuote ))
						{ n++; continue; }
					if (( c == '"' )&&( c0 != '\\' ))
						{ bInQuote = 1 - bInQuote; continue; }
					if (( c == ']' )&&( c0 != '\\' )&&( !bInQuote ))
						{
						if ( !n ) break;
						n--; continue;
						}
					}
				
				if (( c != ']' )||( n )||( bInQuote ))
					return ERROR;
				
				pModlE = pPatt;
				
				continue; // fin case '['
				
				
			
			case '?':
				
				tr( "strRegExpr7:case ?" );
				if ( bInQuote ) goto defaut;

				nbOccur = 1;
				
				// Lecture nombre d'occurence demandé
				c = p_pattern[pPatt+1];
				if ( ( c >= '0' )&&( c <= '9' ) )
					{
					nbOccur = c - '0';
					pPatt++;
					c = p_pattern[pPatt+1];
					if ( ( c >= '0' )&&( c <= '9' ) )
						{
						nbOccur = (nbOccur*10) + (c - '0');
						pPatt++;
						c = p_pattern[pPatt+1];
						if ( ( c >= '0' )&&( c <= '9' ) )
							{
							nbOccur = (nbOccur*10) + (c - '0');
							pPatt++;
							}
						}
					}
				
				// Vérifie que l'on trouve bien le nombre d'occurence demandé
				
				for( ; nbOccur ; nbOccur-- )
					{
					tr( BR BR "strRegExpr7a" );
					tr2( "pPatt=", itoa(pPatt) );
					tr2( "pIn=", itoa(*pIn) );
					tr2( "nbOccur=", itoa(nbOccur) );
					tr2( BR, BR );
					if ( pModl >= 0 )
						{
						tr( "strRegExpr7b" );
						p_pattern[pModlE] = 0;
						n = strRegExpr( &(p_pattern[pModl]),
										p_in_out, pIn, length,
										true, bTild, 1, nbReplace );
						p_pattern[pModlE] = ']';
						}
					else
						n = strRegExpr( "",
										p_in_out, pIn, length,
										true, bTild, 0, nbReplace );
					
					tr( "strRegExpr7c" );
					if ( !n ) { rc = false; goto fin; }
					tr( "strRegExpr7d" );
					}
				
				tr( "strRegExpr7e" );	
				break; // fin case '?'
			
			
			
			case '*':
				
				tr( "strRegExpr8:case *" );
				if ( bInQuote ) goto defaut;
				
				nbOccur = 999999999;
				
				// Lecture nombre d'occurence demandé
				c = p_pattern[pPatt+1];
				if ( ( c >= '0' )&&( c <= '9' ) )
					{
					nbOccur = c - '0';
					pPatt++;
					c = p_pattern[pPatt+1];
					if ( ( c >= '0' )&&( c <= '9' ) )
						{
						nbOccur = (nbOccur*10) + (c - '0');
						pPatt++;
						c = p_pattern[pPatt+1];
						if ( ( c >= '0' )&&( c <= '9' ) )
							{
							nbOccur = (nbOccur*10) + (c - '0');
							pPatt++;
							}
						}
					}
				
				// Vérifie que l'on trouve bien le nombre d'occurence demandé
				
				search:
					
				for( p0 = -1 ; nbOccur ; nbOccur-- )
					{
					tr( BR BR "strRegExpr8a" );
					tr2( "pPatt=", itoa(pPatt) );
					tr2( "pIn=", itoa(*pIn) );
					s[0] = c;
					tr2( "c=", s );
					c = p_pattern[pPatt+1];
					tr2( BR, BR );
					if ( c )
						{
						tr( "strRegExpr8b" );
						p = *pIn;
						n = strRegExpr( &(p_pattern[pPatt+1]),
										p_in_out, &p, length,
										Level+bSearch, bTild, Level+bSearch, nbReplace );
						if ( n )
							{
							tr( "strRegExpr8c" );
							if ( nbReplace ) continue;
							pPatt = strlen(p_pattern)-1; // On arrête les recherches car tout est ok
							if ( p0 < 0 ) p0 = *pIn;
							*pIn = p;
							break;
							}
						}
					tr2( "strRegExpr8d:p_pattern=", p_pattern );

					if ( pModl >= 0 )
						{
						tr( "strRegExpr8e" );
						p_pattern[pModlE] = 0;
						n = strRegExpr( &(p_pattern[pModl]),
										p_in_out, pIn, length,
										true, bTild, 1, nbReplace );
						p_pattern[pModlE] = ']';
						}
					else
						{
						tr( "strRegExpr8f" );
						n = strRegExpr( "",
										p_in_out, pIn, length,
										true, bTild, 0, nbReplace );
						}
					tr( "strRegExpr8g" );
					if ( !n ) break;
					}

				tr( "strRegExpr8h" );	
				break; // fin case '*'
			
			
			case ',': 
			case '|': // OU
				
				tr( "strRegExpr9:case |" );
				if ( bInQuote ) goto defaut;
				goto fin;
				break; // fin case ','
				
				
			case '&': // ET
				
				tr( "strRegExpr9a:case &" );
				if ( bInQuote ) goto defaut;
				tr( "strRegExpr9b" );
				*pIn = StartPos;
				bTild = false;
				bNot = false;
				continue;
				
				
				
			case '\\':
				
				tr( "strRegExpr10:case \\" );
				c = p_pattern[++pPatt];
				switch( c )
					{
					case 'n' : c = '\n'; break;
					case 'r' : c = '\r'; break;
					case 't' : c = '\t'; break;
					}
				goto defaut;
				
				
			case '>':
				
				tr( "strRegExpr11:case >" );
				if ( bInQuote ) goto defaut;

				// Fin du controle demandé
				if ( (c=p_pattern[pPatt+1]) == '#' )
					{
					rc = true; goto fin;
					}

				if (( c == '$' )||( c == '^' ))
					{ // On va vers la fin de ligne
					pPatt++;
					for(;;)
						{
						tr( "strRegExpr11d" );
						switch( p_in[*pIn] )
							{
							case '\n':
								(*pIn)++;
							case 0:
								break;
							
							default :
								tr( "strRegExpr11e" );
								(*pIn)++;
								continue;
							}
						break;
						}
					}
				else
					for(;;)
						{
						tr( "strRegExpr11a" );
						switch( p_in[*pIn] )
							{
							case ' ' :
							case '	':
							case '\n':
							case '\r':
								tr( "strRegExpr11b" );
								(*pIn)++;
								continue;
							}
						break;
						}
				tr( "strRegExpr11c" );	
				break;

			
			case '<':
				
				tr( "strRegExpr20:case <" );
				if ( bInQuote ) goto defaut;

				switch( p_in[*pIn] )
					{
					case 0 :
					case ' ' :
					case '	':
					case '\n':
					case '\r':
						tr( "strRegExpr21 <" );
						continue;
					}
				tr( "strRegExpr22 <" );
				goto next_patt;
				
				
			case '^': // Vérifie début de ligne
				
				tr( "strRegExpr25:case ^" );
				if ( bInQuote ) goto defaut;
				
				if (( !(*pIn) )||( p_in[(*pIn-1)] == '\n' ))
					{
					tr( "strRegExpr26" );
					continue;
					}
					
				tr( "strRegExpr27" );
				goto next_patt;
				
				
			case '#': // Vérifie début de texte
				
				tr( "strRegExpr35:case ^" );
				if ( bInQuote ) goto defaut;
				if ( p_pattern[pPatt+1] != '^' ) goto defaut;
				
				pPatt++;
				if ( !(*pIn) ) continue;
				
				tr( "strRegExpr37" );
				goto next_patt;
				
				
			case '$': // Vérifie début de ligne
				
				tr( "strRegExpr30:case $" );
				if ( bInQuote ) goto defaut;
				if ( p_pattern[pPatt+1] == '#' ) // Fin du texte ?
					{
					pPatt++;
					if ( !(c=p_in[*pIn]) )
						{
						tr( "strRegExpr31" );
						continue;
						}
					}
				else
				if (( !(c=p_in[*pIn]) )||( c == '\n' )||( c == '\r' ))
					{
					tr( "strRegExpr32" );
					continue;
					}
					
				tr( "strRegExpr33" );
				goto next_patt;
				
				
			case '(':
				
				if ( bInQuote ) goto defaut;
				if ( nbReplace ) { pr0 = *pIn; pr1 = -1; }
				continue;
				
				
			case ')':
				
				if ( bInQuote ) goto defaut;
				if (( pr0 >= 0 )&&( nbReplace )) { pr1 = *pIn; }
				continue;
				
				
			case '=':
				
				if ( bInQuote ) goto defaut;
				if ( (c=p_pattern[pPatt+1]) != '>' ) goto defaut;
				tr( "strRegExpr40:case =>" );
				if ( pr1 < 0 ) pr1 = *pIn;
				tr2( "pr0=", itoa(pr0) );
				tr2( "pr1=", itoa(pr1) );
				
				rc = true;
				if ( ! nbReplace ) goto fin;
				
				// Recherche le texte de remplacement
				for( p = -1, pPatt += 2 ; c = p_pattern[pPatt] ; pPatt++ )
					{
					switch( c )
						{
						case ' ' :
						case '	':
						case '\r':
						case '\n':
							if ( p < 0 ) continue;
							if ( bInQuote ) continue;
							break;

						case ',' :
						case '|':
						case ']' :
							if ( bInQuote ) continue;
							break;

						case '"' :
						case '\'':
							if (( !bInQuote )&&( p < 0 ))
								{
								p = pPatt+1;
								bInQuote = c;
								continue;
								}
							if ( c == bInQuote ) break;
							continue;
							
						default:
							if ( p < 0 ) p = pPatt;
							continue;
						}
					break;
					}
					
				if ( p < 0 ) p = pPatt;
				
				tr2( "p=", itoa(p) );
				tr2( "pPatt=", itoa(pPatt) );
				
				// Remplacement		
				
				strdncpy( &w, (*p_in_out), pr0, &(p_pattern[p]), pPatt-p,
								&((*p_in_out)[pr1]), -1, _ );
				(*pIn) = pr0 + pPatt - p;
				
				free( p_in );
				p_in = w; w = null;
				
				(*nbReplace)++;
				
				goto fin;
				
			default:
				// Tout les autres caractères
				defaut:
				
				tr( "strRegExpr12:case defaut" );	
				
				if (( p_pattern[pPatt+1] == '-' )
					&&( c1 = p_pattern[pPatt+2] ))
					{
					tr( "strRegExpr12e" );
					pPatt += 2;
					
					// traitements des intervals
					c0 = c;
					
					if ( ( c0 == 'A' )&&( c1 == 'z' ) )
						{
						tr( "strRegExpr12f" );
						c = elib_upper_table[p_in[*pIn]];
						if (( c >= 'A' )&&( c <= 'Z' ))
							{ if ( pr0 < 0 ) pr0 = *pIn; (*pIn)++; continue; }
						}
					else
					if ( ( c0 == '0' )&&( c1 == 'z' ) )
						{
						tr( "strRegExpr12g" );
						c = elib_upper_table[p_in[*pIn]];
						if ((( c >= 'A' )&&( c <= 'Z' ))
							||(( c >= '0' )&&( c <= '9' )))
							{ if ( pr0 < 0 ) pr0 = *pIn; (*pIn)++; continue; }
						}
					else
						{
						tr( "strRegExpr12h" );
						c = ( bTild ) ? elib_upper_table[p_in[*pIn]] : p_in[*pIn];
						if (( c >= c0 )&&( c <= c1 ))
							{ if ( pr0 < 0 ) pr0 = *pIn; (*pIn)++; continue; }
						}
					goto next_patt;
					}
				
				// Vérifie que ce caractère de masque existe à l'emplacement courant de la chaine d'entrée
				if ( ( p_in[*pIn] )&&
					( (( !bTild )&&( p_in[*pIn] == c ))
						||(( bTild )&&( !charicmp( p_in[*pIn], c ) ))
						)
					)
					{
					tr( "strRegExpr12a" );
					if ( pr0 < 0 ) pr0 = *pIn; (*pIn)++; continue;
					}
				else
					{
					next_patt:
						
					pr0 = pr1 = -1;
					tr( "strRegExpr12b" );
					// Cherche un '|' ou ',' dans le modèle
					for( pPatt++, c0 = c ; c = p_pattern[pPatt] ; pPatt++, c0 = c )
						{
						if ((( c == ',' )||( c == '|' )||( c == '&' ))&&( !bInQuote )) break;
						if (( bInQuote )&&( c0 != '\\' )&&( c == bInQuote ))
							{ bInQuote = false; continue; }
						}
					if (( c == ',' )||( c == '|' ))
						{
						tr( "strRegExpr12c" );
						*pIn = StartPos;
						bTild = false;
						bNot = false;
						continue;
						}
					}
				
				tr( "strRegExpr12d" );
				rc = false;
				goto fin;
			
			} // Fin switch sur caractère en cours du pattern
		
		} // Fin boucle lecture du pattern
	
	
	tr( "strRegExpr13:fin boucle" );
	if (( !bSearch )&&( p_in[*pIn] )) rc = false;
	
	fin:
		
	tr( "strRegExpr14" );
	
	if ( bNot ) rc = 1 - rc;
	if (( bSearch )&&( rc )&&( !Level ))	*pIn = p0;

	tr2( "rc=", itoa(rc) );
	tr2( "pIn=", itoa(*pIn) );
	
	return rc;	
	}

#undef p_in
	










	
//------------------------------------------------------------------------------				
bool strpcmp( char *pattern, char *in )
//------------------------------------------------------------------------------				
	{
	int i = 0, j = 0, i0 = -1, b = 1, p, p1 = 0, p2, l, l0, bFirst = true;

	p2 = strpos( pattern, "*", 0 );
	if ( p2 < 0 )
		{
		return strjcmp( in, pattern );
		}
	
	l = strlen( in );
	for( ; ; )
		{
		if ( p1 != p2 )
			{
//print( "cherche ", substr( pattern, p1, p2-p1 ), ", i=", itoa(i), BR, _ );
			if ( p1 == 0 )
				{
//print( "strpcmp1" BR, _ );
				if ( !strjncmp( pattern, in, p2 ) ) return false;
//print( "strpcmp2" BR, _ );
				}
			else
				{
//print( "strpcmp2b:l0=", itoa(p2-p1), BR, _ );
/*
				for( b = -1, l0 = p2-p1 ;
					( i < (l-l0) )&&( !( b = strjncmp( &(pattern[p1]), &(in[i]), l0 ) ) ) ;
					i++ );
*/
				for( b = -1, l0 = p2-p1 ;
					( i <= (l-l0) ) ;
					i++ )
{
//print( "strpcmp2b:strjncmp(", &(pattern[p1]), ", ", &(in[i]), ", ", itoa(l0), ")", BR, _ );
					if ( b = strjncmp( &(pattern[p1]), &(in[i]), l0 ) ) break;
}					
//print( "strpcmp3: b=", itoa(b), ", p1=", itoa(p1), ", p2=", itoa(p2), ", l0=", itoa(l0), ", l=", itoa(l), ", i=", itoa(i), BR, _ );
				if ( i > (l-l0) )
					{
//print( "strpcmp4" BR, _ );
					if ( b != -1 ) return false;
//print( "strpcmp5" BR, _ );
					if ( ! strjncmp( &(pattern[p1]), &(in[i]), l0 ) )
						return false;
//print( "strpcmp6" BR, _ );
					}
				i += l0;
				}
			}
		
		p1 = p2+1;
		if ( pattern[p1] == 0 ) return true;
		p2 = strpos( pattern, "*", p1 );
//print( "strpcmp7: p2 = ", itoa(p2), BR, _ );
		if ( p2 < 0 )
			{
			l0 = strlen(&(pattern[p1]));
//print( "strpcmp8: i=", itoa(i), ", l0=", itoa(l0), ", l-l0=", itoa(l-l0), BR, _ );
			if ( i > (l-l0) ) return false;
//print( "strpcmp9" BR, _ );
			return ( strjcmp( &(pattern[p1]) ,&(in[l-l0]) ) ) ? true : false;
			}
		}
	}

/*
//------------------------------------------------------------------------------				
bool strpcmp( char *pattern, char *in )
//------------------------------------------------------------------------------				
	{
	int i = 0, j = 0, i0 = -1, b = 1;

	for(  ; ( pattern[i] )&&( in[j] ) ; )
		{
		if ( pattern[i] == '?' )
			{
			i++; j++; continue;
			}
		if ( pattern[i] != '*' )
			{
			if ( pattern[i] == in[j] )
				{
				i++; j++; continue;
				}
			if ( i0 == -1 )
				{
				b = 0;
				break;
				}
			else
				{
				i = i0+1;
				j++;
				continue;
				}
			}
		if ( !pattern[i+1] )
			{
			i++;
			j = strlen( in );
			break;
			}
		if ( pattern[i+1] == in[j] )
			{
			i0 = i;
			i+=2;
			}
		j++;
		}

//	return ( ( b )&&( !pattern[i] )&&( !in[j] ) ) ? true : false;
    return ( ( b )&&( ( !pattern[i] )||( ( ( pattern[i] == '*' )||( pattern[i] == '?' ) )&&( !pattern[i+1] ) ) )&&( !in[j] ) ) ? true : false;
	}
*/


//------------------------------------------------------------------------------
bool	strmcmp( char *in, char *sList ) // sList = chaine1|chaine2|"toto titi"
//------------------------------------------------------------------------------
	{
	char **ts;
	int i;
	
	ts = str_explode( sList, "|" );
	
	for( i = 0 ; ts[i] ; i++ )
		if ( ! strcmp( in, ts[i] ) ) { free( ts ); return true; }
	free( ts );
	return false;
	}

//------------------------------------------------------------------------------
bool	strmicmp( char *in, char *sList ) // sList = chaine1|chaine2|"toto titi"
//------------------------------------------------------------------------------
	{
	char **ts;
	int i;
	
	ts = str_explode( sList, "|" );
	
	for( i = 0 ; ts[i] ; i++ )
		if ( ! stricmp( in, ts[i] ) ) { free( ts ); return true; }
	free( ts );
	return false;
	}

//------------------------------------------------------------------------------
char *	strncpy( char *out, char *in, int len )
//------------------------------------------------------------------------------
	{
	memcpy( out, in, len );
	out[len] = 0;
	return out;
	}
	
//------------------------------------------------------------------------------
char *	strdcpy( char **p_out, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg, *out_mem, *out;
	int l = 0, b = false;

	va_start(args, p_out); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		if ( arg )
			{
			l += strlen( arg );
			if ( ( p_out )&&( arg == *p_out ) ) b = true;
			}
		}

	if ( ( p_out )&&( *p_out ) )
		{
		if ( !b ) out = (char*)realloc( *p_out, l+1 );
		else { out_mem = *p_out; out = alloc( l+1 ); }
		}
	else
		out = alloc( l+1 );
	
	out[0] = 0;
	va_start(args, p_out);
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		if ( arg ) strcat( out, arg );
		}
		
	if ( b ) free( out_mem );
	if ( p_out ) *p_out = out;
	return out;
	}
	
//------------------------------------------------------------------------------
char *	strdncpy( char **p_out, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg, *out_mem, *out;
	int l = 0, i, b = false;

	va_start(args, p_out); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		i = va_arg(args, int);
		if ( arg )
			{
			l += ( i >= 0 ) ? i : strlen( arg );
			if ( ( p_out )&&( arg == *p_out ) ) b = true;
			}
		}

	if ( ( p_out )&&( *p_out ) )
		{
		if ( !b ) out = (char*)realloc( *p_out, l+1 );
		else { out_mem = *p_out; out = alloc( l+1 ); }
		}
	else
		out = alloc( l+1 );
	
	out[0] = 0;
	va_start(args, p_out);
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		i = va_arg(args, int);
		if ( arg )
			{
			if ( i >= 0 )	strncat( out, arg, i );
			else strcat( out, arg );
			}
		}
		
	if ( b ) free( out_mem );
	if ( p_out ) *p_out = out;
	return out;
	}


//------------------------------------------------------------------------------
char *	str( char * s, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg, *out;
	int l = 0;

	if ( s ) l += strlen( s );
	
	va_start(args, s); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		if ( arg ) l += strlen( arg );
		}

	out = memget( l+1 );
	
	if ( s ) strcpy( out, s );
	else out[0] = 0;
	
	va_start(args, s); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		if ( arg ) strcat( out, arg );
		}
		
	return out;
	}
	
//------------------------------------------------------------------------------
char *	strdcat( char **p_out, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg, *out;
	int l = 0;

	if (( p_out )&&( *p_out )) l = strlen( *p_out );

	va_start(args, p_out); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		if ( arg ) l += strlen( arg );
		}

	if (( p_out )&&( *p_out ))
		out = (char*)realloc( *p_out, l+1 );
	else
		{
		out = alloc( l+1 );
		out[0] = 0;
		}
	
	va_start(args, p_out); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		if ( arg ) strcat( out, arg );
		}
	
	if ( p_out ) *p_out = out;
	return out;
	}

//------------------------------------------------------------------------------
char *	strdncat( char **p_out, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg, *out;
	int l = 0, i;

	if (( p_out )&&( *p_out )) l = strlen( *p_out );

	va_start(args, p_out); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		i = va_arg(args, int);
		if ( arg ) l += ( i >= 0 ) ? i : strlen( arg );
		}

	if (( p_out )&&( *p_out ))
		out = (char*)realloc( *p_out, l+1 );
	else
		{
		out = alloc( l+1 );
		out[0] = 0;
		}
	
	va_start(args, p_out); 
	for( ; ; )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) break;
		i = va_arg(args, int);
		if ( arg )
			{
			if ( i >= 0 )	strncat( out, arg, i );
			else strcat( out, arg );
			}
		}
	
	if ( p_out ) *p_out = out;
	return out;
	}


//------------------------------------------------------------------------------				
int 	strpos( char *s, char *search, int offset )
//------------------------------------------------------------------------------				
	{
	int i, l, l0;

	if ( ( !s )||( !search ) ) return -1;
	l0 = strlen(search);
	l = strlen(s)-l0+1;
	for( i = offset ; i < l ; i++ )
		if ( ! strncmp( &(s[i]), search, l0 ) )
			return i;
	return -1;
	}


//------------------------------------------------------------------------------				
int 	stripos( char *s, char *search, int offset )
//------------------------------------------------------------------------------				
	{
	int i, l, l0;
   
	if ( ( !s )||( !search ) ) return -1;
	l0 = strlen(search);
	l = strlen(s)-l0+1;
	for( i = offset ; i < l ; i++ )
		if ( ! strnicmp( &s[i], search, l0 ) )
			return i;
	return -1;
	}


//------------------------------------------------------------------------------				
int		strrpos( char* in, char* search, int offset )
//------------------------------------------------------------------------------				
	{
	int i, l, li;
	
	if ( ( !in )||( !search ) ) return -1;
	l = strlen(search);
	li = strlen( in );
	if (( offset > li )||( offset < 0 )) offset = li;

	for( i = offset-l ; i >= 0 ; i-- )
		if ( ! strncmp( &in[i], search, l ) )
			return i;
	return -1;
	}
	
//------------------------------------------------------------------------------				
int		strripos( char* in, char* search, int offset )
//------------------------------------------------------------------------------				
	{
	int i, l, li;
   
	if ( ( !in )||( !search ) ) return -1;
	l = strlen(search);
	li = strlen( in );
	if (( offset > li )||( offset < 0 )) offset = li;

	for( i = offset-l ; i >= 0 ; i-- )
		if ( ! strnicmp( &in[i], search, l ) )
			return i;
	return -1;
	}
	
//------------------------------------------------------------------------------				
int		strcount( char* in, char* search, int offset )
//------------------------------------------------------------------------------				
	{
	int n = 0, p = offset;
	
	if ( ( !in )||( !search ) ) return 0;
	while( ( p = strpos( in, search, p )+1 ) > 0 ) n++;
	return n;
	}
	
//------------------------------------------------------------------------------				
int		stricount( char* in, char* search, int offset )
//------------------------------------------------------------------------------				
	{
	int n = 0, p = offset;
	
	if ( ( !in )||( !search ) ) return 0;
	while( ( p = stripos( in, search, p )+1 ) > 0 ) n++;
	return n;
	}
	
//------------------------------------------------------------------------------				
bool	str_exists( char* in, char* sList )
//------------------------------------------------------------------------------				
	{
	char **ts1, **ts2;
	int i1, i2;
	
	ts1 = str_explode( str_replace(str_replace(sList, " =", "=" ), "= ", "="), "&" );
	if ( !ts1 ) return false;
	for( i1 = 0 ; ts1[i1] ; i1++ )
		{
		if ( ! ts1[i1][0] ) continue;
		ts2 = str_explode( ts1[i1], "|" );
		if ( !ts2 ) return false;
		for( i2 = 0 ; ts2[i2] ; i2++ )
			{
			if ( ! ( ts2[i2][0] ) ) continue;
			if ( ts2[i2][strlen(ts2[i2])-1] != '*' )
				{
				if (( ts2[i2][0] == '!')&&( strpos( in, &(ts2[i2][1]), 0 ) < 0 ) ) break;
				if ( strpos( in, ts2[i2], 0 ) >= 0 ) break;
				}
			else
				{
				if (( ts2[i2][0] == '!')&&( strncmp( in, &(ts2[i2][1]), strlen(ts2[i2])-2 ) ) ) break;
				if ( !strncmp( in, ts2[i2], strlen(ts2[i2])-1 ) ) break;
				}
			}
			
		if ( ! ts2[i2] ) { free( ts1 ); free( ts2 ); return false; }
		free( ts2 );
		ts2 = NULL;
		}
	
	if ( ts1 ) free( ts1 );
	if ( ts2 ) free( ts2 );
	
	return true;
	}
	
//------------------------------------------------------------------------------				
bool	str_iexists( char* in, char* sList )
//------------------------------------------------------------------------------				
	{
	char **ts1, **ts2;
	int i1, i2;

	ts1 = str_explode( str_replace(str_replace(sList, " =", "=" ), "= ", "="), "&" );
	if ( !ts1 ) return false;
	for( i1 = 0 ; ts1[i1] ; i1++ )
		{
		if ( ! ts1[i1][0] ) continue;
		ts2 = str_explode( ts1[i1], "|" );
		if ( !ts2 ) return false;
		for( i2 = 0 ; ts2[i2] ; i2++ )
			{
			if ( ! ( ts2[i2][0] ) ) continue;
			if ( ts2[i2][strlen(ts2[i2])-1] != '*' )
				{
				if (( ts2[i2][0] == '!')&&( stripos( in, &(ts2[i2][1]), 0 ) < 0 ) ) break;
				if ( stripos( in, ts2[i2], 0 ) >= 0 ) break;
				}
			else
				{
				if (( ts2[i2][0] == '!')&&( strnicmp( in, &(ts2[i2][1]), strlen(ts2[i2])-2 ) ) ) break;
				if ( !strnicmp( in, ts2[i2], strlen(ts2[i2])-1 ) ) break;
				}
			}
			
		if ( ! ts2[i2] ) { free( ts1 ); free( ts2 ); return false; }
		free( ts2 );
		ts2 = NULL;
		}
	
	if ( ts1 ) free( ts1 );
	if ( ts2 ) free( ts2 );
	
	return true;
	}
	
//------------------------------------------------------------------------------				
char * str_replace( char *in, char* sSearch, char *sReplace )
//------------------------------------------------------------------------------				
	{
	int p0 = 0, p1, l1, l2;
	char **pout;

	if ( !sReplace ) sReplace = "";
	if ( in == null ) return null;

	pout = memptr(0);
	if ( *pout )	(*pout)[0] = 0;
	
	l1 = strlen( sSearch );
	l2 = strlen( sReplace );

	while( ( p1 = strpos( in, sSearch, p0 ) ) >= 0 )
		{
		strdncat( pout, &(in[p0]), p1-p0, sReplace, l2, _ );
		p0 = p1 + l1;
		}
	strdcat( pout, &(in[p0]), _ );

	return *pout;
	}


//------------------------------------------------------------------------------				
char * str_ireplace( char *in, char* sSearch, char *sReplace )
//------------------------------------------------------------------------------				
	{
	int p0 = 0, p1, l1, l2;
	char **pout;

	if ( !sReplace ) sReplace = "";
	if ( in == null ) return null;

	pout = memptr(0);
	if ( *pout )	(*pout)[0] = 0;
	
	l1 = strlen( sSearch );
	l2 = strlen( sReplace );

	while( ( p1 = stripos( in, sSearch, p0 ) ) >= 0 )
		{
		strdncat( pout, &(in[p0]), p1-p0, sReplace, l2, _ );
		p0 = p1 + l1;
		}
	strdcat( pout, &(in[p0]), _ );

	return *pout;
	}


//------------------------------------------------------------------------------				
char * strd_replace( char **inout, char* sSearch, char *sReplace )
//------------------------------------------------------------------------------				
	{
	int p0 = 0, p1, l1, l2;
	char *out, *in;

	if ( !sReplace ) sReplace = "";
	
	if (( inout == null )||( *inout == null )||( sSearch == null )) return null;

	l1 = strlen( sSearch );
	l2 = strlen( sReplace );

	in = *inout;
	out = alloc(200);
	out[0] = 0;
	
	while( ( p1 = strpos( in, sSearch, p0 ) ) >= 0 )
		{
		strdncat( &out, &(in[p0]), p1-p0, sReplace, l2, _ );
		p0 = p1 + l1;
		}
	strdcat( &out, &(in[p0]), _ );

	free( in );
	return *inout = out;
	}

//------------------------------------------------------------------------------				
char * strd_ireplace( char** inout, char* sSearch, char *sReplace )
//------------------------------------------------------------------------------				
	{
	int p0 = 0, p1, l1, l2;
	char *out, *in;

	if ( !sReplace ) sReplace = "";
	if (( inout == null )||( *inout == null )||( sSearch == null )) return null;

//	if (( inout == null )||( sSearch == null )||( sReplace == null )) return null;

	in = *inout;
	l1 = strlen( sSearch );
	l2 = strlen( sReplace );

	out = alloc(200);
	out[0] = 0;

	while( ( p1 = stripos( in, sSearch, p0 ) ) >= 0 )
		{
		strdncat( &out, &(in[p0]), p1-p0, sReplace, l2, _ );
		p0 = p1 + l1;
		}
	strdcat( &out, &(in[p0]), _ );

	free( in );
	return *inout = out;
	}


//------------------------------------------------------------------------------				
char **	str_explode( char *in, char *separator )
//------------------------------------------------------------------------------				
	{
	char **ts, *s;
	int n = 0, p = 0, p0 = 0, l, c;
	
	if (( !in )||( !separator )) return NULL;
	
	c = strcount( in, separator, 0 )+2;

	ts = (char**)calloc( 1, strlen(in)+1+(sizeof(char*)*(c + 1)) );
	if ( ts == NULL ) return NULL;
	s = (char*)((unsigned long int)ts + (unsigned long int)(sizeof(char*)*c));

	l = strlen( separator );
	
	while( ( p = strpos( in, separator, p0 ) ) >= 0 )
		{
		memcpy( s, &(in[p0]), p - p0 );
		s[p - p0] = 0;
		trim( s );

		
		if ( ( s[0] == '"' )&&( strpos( s, "\"", 1 ) < 0 ) )
			{
			p = strpos( in, "\"", p + 1 );
			if ( p < 0 ) break;

			p = strpos( in, separator, p + 1 );
			if ( p < 0 ) break;
			
			memcpy( s, &(in[p0]), p - p0 );
			s[p - p0] = 0;
			trim( s );
			strd_unquote( &s );
			}
		else
		if ( ( s[0] == '\'' )&&( strpos( s, "'", 1 ) < 0 ) )
			{
			p = strpos( in, "'", p + 1 );
			if ( p < 0 ) break;

			p = strpos( in, separator, p + 1 );
			if ( p < 0 ) break;
			
			memcpy( s, &(in[p0]), p - p0 );
			s[p - p0] = 0;
			trim( s );
			strd_unquote( &s );
			}

		p0 = p + l;
		if ( ( !s[0] )&&( separator[0] == ' ' )&&( separator[1] == 0 ) )
			continue;

		ts[n++] = s;
		s += strlen( s ) + 1;
//		if ( n == 2000 ) break;
		}


	if ( p < 0 )
		{
		strcpy( s, &(in[p0]) );
		ts[n] = trim( s );
		}
	

	return ts;
	}

//------------------------------------------------------------------------------				
char *	str_quote( char *p_in )
//------------------------------------------------------------------------------				
	{
	int i = 0, j = 0, k;
	char *out, **pout;

	#define esc		'\\'
	#define q		'\''
		
	if ( p_in == null ) return "''";

	k = strlen( p_in );

	pout = memptr( k+100 );
	out = *pout;
	
	out[j++] = q;
	for( ; p_in[i] ; i++ )
		{
		if ( p_in[i] == esc )
			{
			out[j++] = esc;
			out[j++] = esc;
			}
		else
		if ( p_in[i] == q )
			{
			out[j++] = esc;
			out[j++] = q;
			}
		else
		if ( p_in[i] == '\n' )
			{
			out[j++] = '\\';
			out[j++] = 'n';
			}
		else
		if ( p_in[i] == '\r' )
			{
			out[j++] = '\\';
			out[j++] = 'r';
			}
		else
			out[j++] = p_in[i];
			
		if ( j - k > 98 )
			{
			k += 100;
			out = *pout = realloc( *pout, k+100 );
			}
		}
		
	out[j++] = q;
	out[j] = 0;

	#undef esc
	#undef q	

	return out;
	}


//------------------------------------------------------------------------------				
char *	strd_quote( char **p_inout )
//------------------------------------------------------------------------------				
	{
	int i = 0, j = 0, k;
	char *out, **pout, *in;

	#define esc		'\\'
	#define q		'\''
		
	if (( p_inout == null )||( *p_inout == null )) return "''";

	k = strlen( *p_inout );
	
	in = str( *p_inout, _ );
	out = *p_inout = realloc( *p_inout, k+100 );
	
	out[j++] = q;
	for( ; in[i] ; i++ )
		{
		if ( in[i] == esc )
			{
			out[j++] = esc;
			out[j++] = esc;
			}
		else
		if ( in[i] == q )
			{
			out[j++] = esc;
			out[j++] = q;
			}
		else
		if ( in[i] == '\n' )
			{
			out[j++] = '\\';
			out[j++] = 'n';
			}
		else
		if ( in[i] == '\r' )
			{
			out[j++] = '\\';
			out[j++] = 'r';
			}
		else
			out[j++] = in[i];
			
		if ( j - k > 98 )
			{
			k += 100;
			out = *p_inout = realloc( *p_inout, k+100 );
			}
		}
		
	out[j++] = q;
	out[j] = 0;

	#undef esc
	#undef q	

	return out;
	}


//------------------------------------------------------------------------------				
char *	str_C_quote( char *p_in )
//------------------------------------------------------------------------------				
	{
	int i = 0, j = 0, k;
	char *out, **pout;

	#define esc		'\\'
	#define q		'"'
		
	if ( p_in == null ) return "\"\"";

	k = strlen( p_in );
	
	pout = memptr( k+100 );
	out = *pout;
	
	out[j++] = q;
	for( ; p_in[i] ; i++ )
		{
		if ( p_in[i] == esc )
			{
			out[j++] = esc;
			out[j++] = esc;
			}
		else
		if ( p_in[i] == q )
			{
			out[j++] = esc;
			out[j++] = q;
			}
		else
		if ( p_in[i] == '\n' )
			{
			out[j++] = '\\';
			out[j++] = 'n';
			}
		else
		if ( p_in[i] == '\r' )
			{
			out[j++] = '\\';
			out[j++] = 'r';
			}
		else
			out[j++] = p_in[i];
			
		if ( j - k > 98 )
			{
			k += 100;
			out = *pout = realloc( *pout, k+100 );
			}
		}
		
	out[j++] = q;
	out[j] = 0;

	#undef esc
	#undef q	

	return out;
	}


//------------------------------------------------------------------------------				
char *	strd_C_quote( char **p_inout )
//------------------------------------------------------------------------------				
	{
	int i = 0, j = 0, k;
	char *out, **pout, *in;

	#define esc		'\\'
	#define q		'"'
		
	if (( p_inout == null )||( *p_inout == null )) return "\"\"";

	k = strlen( *p_inout );
	
	in = str( *p_inout, _ );
	out = *p_inout = realloc( *p_inout, k+100 );
	
	out[j++] = q;
	for( ; in[i] ; i++ )
		{
		if ( in[i] == esc )
			{
			out[j++] = esc;
			out[j++] = esc;
			}
		else
		if ( in[i] == q )
			{
			out[j++] = esc;
			out[j++] = q;
			}
		else
		if ( in[i] == '\n' )
			{
			out[j++] = '\\';
			out[j++] = 'n';
			}
		else
		if ( in[i] == '\r' )
			{
			out[j++] = '\\';
			out[j++] = 'r';
			}
		else
			out[j++] = in[i];
			
		if ( j - k > 98 )
			{
			k += 100;
			out = *p_inout = realloc( *p_inout, k+100 );
			}
		}
		
	out[j++] = q;
	out[j] = 0;

	#undef esc
	#undef q	

	return out;
	}


//------------------------------------------------------------------------------
char *	str_unquote( char *in )
//------------------------------------------------------------------------------
	{
	int l, pi = 1, po = 0, i;
	char *out, **pout;

	if ( in == null )
		return "";
	
	if ( ( in[0] != '\'' )&&( in[0] != '"' ) )
		return in;

	l = strlen( in );
	if ( ( l < 2 )||( ( in[l-1] != '\'' )&&( in[l-1] != '"' ) ) ) return in;

	pout = memptr( l+1 );
	out = *pout;
	
	for( ; in[pi+1] ; )
		{
		if ( in[pi] == '\\' )
			{
			pi++;
			if ( in[pi] == 'n' )
				{ out[po++] = '\n'; pi++; continue; }

			if ( in[pi] == 'r' )
				{ out[po++] = '\r'; pi++; continue; }

			if ( in[pi] == 't' )
				{ out[po++] = '\t'; pi++; continue; }

			if ( ( in[pi] == '0' )&&( in[pi+1] == 'x' )
				&&( ( ( in[pi+2] >= '0' )&&( in[pi+2] <= '9' ) )
					||( ( in[pi+2] >= 'A' )&&( in[pi+2] <= 'F' ) ) )
				&&( ( ( in[pi+3] >= '0' )&&( in[pi+3] <= '9' ) )
					||( ( in[pi+3] >= 'A' )&&( in[pi+3] <= 'F' ) ) ) )
				{	// \0xFF
				pi += 2;
				i = 16 * ( in[pi] - 
					( ( ( in[pi] >= '0' )&&( in[pi] <= '9' ) ) ? '0' : 'A' ) );
				pi++;
				i += ( in[pi] - 
					( ( ( in[pi] >= '0' )&&( in[pi] <= '9' ) ) ? '0' : 'A' ) );
				out[po++] = (char)i;
				pi++;
				continue;
				}

			if ( ( ( in[pi] >= '0' )&&( in[pi] <= '2' ) )
				&&( ( in[pi+1] >= '0' )&&( in[pi+1] <= '9' ) )
				&&( ( in[pi+2] >= '0' )&&( in[pi+2] <= '9' ) )
				)
				{	// \999
				out[po++] = (char)( 100 * ( in[pi] - '0' )
					+ 10 * ( in[pi+1] - '0' )
					+ ( in[pi+2] - '0' ) );
				pi += 3;
				continue;
				}
			}

		out[po++] = in[pi++];
		}
	out[po] = 0;
	
	return out;
	}

//------------------------------------------------------------------------------
char *	strd_unquote( char **p_inout )
//------------------------------------------------------------------------------
	{
	int l, pi = 1, po = 0, i;
	char *out, **pout, *in;

	if ( p_inout == null )
		return null;
	
	in = *p_inout;
	
	if ( ( in[0] != '\'' )&&( in[0] != '"' ) )
		return in;

	l = strlen( in );
	if ( ( l < 2 )||( ( in[l-1] != '\'' )&&( in[l-1] != '"' ) ) ) return in;

	out = *p_inout;
	
	for( ; in[pi+1] ; )
		{
		if ( in[pi] == '\\' )
			{
			pi++;
			if ( in[pi] == 'n' )
				{ out[po++] = '\n'; pi++; continue; }

			if ( in[pi] == 'r' )
				{ out[po++] = '\r'; pi++; continue; }

			if ( in[pi] == 't' )
				{ out[po++] = '\t'; pi++; continue; }

			if ( ( in[pi] == '0' )&&( in[pi+1] == 'x' )
				&&( ( ( in[pi+2] >= '0' )&&( in[pi+2] <= '9' ) )
					||( ( in[pi+2] >= 'A' )&&( in[pi+2] <= 'F' ) ) )
				&&( ( ( in[pi+3] >= '0' )&&( in[pi+3] <= '9' ) )
					||( ( in[pi+3] >= 'A' )&&( in[pi+3] <= 'F' ) ) ) )
				{	// \0xFF
				pi += 2;
				i = 16 * ( in[pi] - 
					( ( ( in[pi] >= '0' )&&( in[pi] <= '9' ) ) ? '0' : 'A' ) );
				pi++;
				i += ( in[pi] - 
					( ( ( in[pi] >= '0' )&&( in[pi] <= '9' ) ) ? '0' : 'A' ) );
				out[po++] = (char)i;
				pi++;
				continue;
				}

			if ( ( ( in[pi] >= '0' )&&( in[pi] <= '2' ) )
				&&( ( in[pi+1] >= '0' )&&( in[pi+1] <= '9' ) )
				&&( ( in[pi+2] >= '0' )&&( in[pi+2] <= '9' ) )
				)
				{	// \999
				out[po++] = (char)( 100 * ( in[pi] - '0' )
					+ 10 * ( in[pi+1] - '0' )
					+ ( in[pi+2] - '0' ) );
				pi += 3;
				continue;
				}
			}

		out[po++] = in[pi++];
		}
	out[po] = 0;
	
	return out;
	}

//------------------------------------------------------------------------------
char *	str_to_utf8( char **inout )
//------------------------------------------------------------------------------
	{
	uint i, j, l, n;
	char *out, *in;

	if ( ( inout == NULL )||( (in=*inout) == NULL ) ) return NULL;
	
	l = strlen(in);
	
	out = alloc( (l*2)+1 );
	
	for( i = j = 0 ; i < l ; i++ )
		{
		if ( (uint)(in[i]) < 128 ) out[j++] = in[i];
		else
			{
			n = (uint)((unsigned char)in[i]);
			out[j++] = (char)( (n>=192) ? 195 : 194 );
			out[j++] = ( n & 63 ) | 128;
			} 
		}
	out[j] = 0;
	
	free( in );
	return *inout = out;
	}
	
//------------------------------------------------------------------------------
char *	str_from_utf8( char **inout )
//------------------------------------------------------------------------------
	{
	uint i, j, l, n;
	char *out, *in;

	if ( ( inout == NULL )||( (in=*inout) == NULL ) ) return NULL;
	
	l = strlen(in);
	out = in;
	
	for( i = j = 0 ; i < l ; i++ )
		{
		if ( (uint)(in[i]) < 128 ) out[j++] = in[i];
		else
			{
			out[j++] = (( in[i] & 3 ) * 64) + ( in[i+1] & 63 );
			i++;
			} 
		}
	out[j] = 0;
	return out;	
	}
	
//------------------------------------------------------------------------------

typedef struct
	{
	char *	txt;
	FILE *	fp;
	char *	keys;
	uint 	pos;
	uint	wpos;
	uint 	line;
	uint 	col;
	char	c[1];
	uint	p, p0, p1, i;
	}
	T_ELIB_TEXTE;
				
//------------------------------------------------------------------------------				
handle text_init( char *txt, char *keys )
//------------------------------------------------------------------------------				
	{
	T_ELIB_TEXTE *t;
	
	t = (T_ELIB_TEXTE*)calloc( 1, sizeof(T_ELIB_TEXTE) );
	t->keys		= keys;
	t->txt = txt;
/*	
	if ( txt[0] == '@' )
		{
		t->fp = fopen( &(txt[1]), "rb" );
		if ( t->fp == NULL )
			{
			free( t );
			return ERROR;
			}
		}
*/		
	return (handle)t;
	}
	
//------------------------------------------------------------------------------				

#define txt		((T_ELIB_TEXTE*)hTxt)->txt
#define fp		((T_ELIB_TEXTE*)hTxt)->fp
#define keys	((T_ELIB_TEXTE*)hTxt)->keys
#define pos		((T_ELIB_TEXTE*)hTxt)->pos
#define wpos	((T_ELIB_TEXTE*)hTxt)->wpos
#define line	((T_ELIB_TEXTE*)hTxt)->line
#define col		((T_ELIB_TEXTE*)hTxt)->col
#define c		((T_ELIB_TEXTE*)hTxt)->c[0]
#define p0		((T_ELIB_TEXTE*)hTxt)->p0
#define p1		((T_ELIB_TEXTE*)hTxt)->p1
#define i		((T_ELIB_TEXTE*)hTxt)->i

//------------------------------------------------------------------------------				
char *	text_getword( char **p_out, handle hTxt )
//------------------------------------------------------------------------------				
    {
	char *out;
	
	if (( p_out )||( *p_out ))	out = *p_out;
	else	out = alloc(128);
	
	while( 1 )
		{
		// RECHERCHE LE PREMIER CARACTERE QUI NE SOIT PAS UN ESPACE, TAB, RETOUR CHARIOT
		
		do
			{
			if ( ( c = txt[pos++] ) == 0 ) goto fin_texte;
			if ( c == '\n' ) line++;
			}
		while( ( c == ' ' )||( c == '	' )||( c == 10 )||( c == 13 ) );

		// TRAITEMENT DES MOTS CLEF
		if ( keys != NULL )
			{
			for( i = 0 ; keys[i] ; i += 2 )
				{
				if	( keys[i] != ' ' )
					{
					if ( ( keys[i] == c )&&( keys[i+1] == txt[pos] ) )
						{
						wpos = pos-1;
						strdncpy( &out, &(c), 1, &(txt[pos++]), 1, _ );
						if ( p_out ) *p_out = out;
						return out;
						}
					}
				else
				if ( keys[i+1] == c )
					{
					wpos = pos-1;
					strdncpy( &out, &c, 1, _ );
					if ( p_out ) *p_out = out;
					return out;
					}
				}
			}
	
	    // TRAITEMENT DES GUILLEMETS " OU '

		if ( ( c == '"' )||( c == '\'' ) )
			{
			// RETOURN LE TEXTE AVEC LES GUILLEMETS
			for( p0 = pos-1 ; txt[pos] ; pos++ )
				{
				if ( txt[pos] == c )
					{
					for( i = pos-1 ; i >= 0 ; i-- )
						{
						if ( txt[i] != '\\' ) break;
						}
//					if (( txt[pos-1] != '\\' )||(( pos > 1 )&&( txt[pos-2] == '\\' )))
					if ( ( (pos-i-1) % 2 ) == 0 )
						{
						pos++;
						wpos = p0;
						strdncpy( &out, &(txt[p0]), pos-p0, _ );
						if ( p_out ) *p_out = out;
						return out;
						}
					}
				}
			wpos = p0;
			strdcpy( &out, &(txt[p0]), _ );
			if ( p_out ) *p_out = out;
			return out;
			}
	
		// TRAITEMENTS DES COMMENTAIRES LIGNES
		
		if ( ( ( c == '/' )&&( txt[pos] == '/' ) ) )
			{
			for( ; ; )
				{
				if ( ! txt[++pos] ) goto fin_texte;
				if ( txt[pos] == '\n' ) {line++; break;}
				}
			pos++;
			continue;
			}
			
		// TRAITEMENT DES COMMENTAIRES BLOC
		
		if ( ( c == '/' )&&( txt[pos] == '*' ) )
			{
			for( p1 = pos+1 ; txt[p1] ; p1++ )
				{
				if (( txt[p1] == '*' )&&( txt[p1+1] == '/' ))
					break;
				if ( txt[p1] == '\n' ) line++;
				}
	
			if ( ! txt[p1] ) goto fin_texte;
			pos = p1 + 2;
			continue;
			}
			
		// TRAITEMENT DES MOTS
		
		p0 = pos-1;
		
		while( 1 )
			{
				
			// FIN DU TEXTE ??
			
			if ( !txt[pos] )
				{
				wpos = p0;
				strdncpy( &out, &(txt[p0]), pos-p0, _ );
				if ( p_out ) *p_out = out;
				return out;
				}
			
			c = txt[pos];
			
			// SI ESPACE OU TABULATION OU RETOUR CHARIOT
			
			if(( c == ' ' )||( c == '	' )
				||( c == '\n' )||( c == '\r' ) )
				{
				wpos = p0;
				strdncpy( &out, &(txt[p0]), (pos++)-p0, _ );
				if ( p_out ) *p_out = out;
				return out;
				}
				
			// SI MOT CLEF ??
			
            if ( keys != NULL )
				{
				for( i = 0 ; keys[i] ; i += 2 )
					{
					if ( ( ( keys[i] == ' ' )&&( keys[i+1] == c ) )
						|| ( ( keys[i] != ' ' )&&( keys[i] == c )
								&&( keys[i+1] == txt[pos+1] ) ) )
						{
						wpos = p0;
						strdncpy( &out, &(txt[p0]), pos-p0, _ );
						if ( p_out ) *p_out = out;
						return out;
						}
					}
				}
			
			// SUITE DE LA LECTURE DU MOT
			
			pos++;	
			}
		}
	
	fin_texte:
		
		if ( fp ) fclose( fp );
		free((void*)hTxt);
		return NULL;
					
	}	

//------------------------------------------------------------------------------
int		text_getlineno( handle hTxt )
//------------------------------------------------------------------------------
	{
	return line;
	}

//------------------------------------------------------------------------------
int		text_getpos( handle hTxt )
//------------------------------------------------------------------------------
	{
	return pos;
	}

//------------------------------------------------------------------------------
int		text_setpos( handle hTxt, int newpos, int lineno )
//------------------------------------------------------------------------------
	{
	pos = newpos;
	line = lineno;
	
	return OK;
	}
	
//------------------------------------------------------------------------------
int		text_getwpos( handle hTxt )
//------------------------------------------------------------------------------
	{
	return wpos;
	}

//------------------------------------------------------------------------------
void	text_free( handle hTxt )
//------------------------------------------------------------------------------
	{
	if ( fp ) fclose( fp );
	free((void*)hTxt);
	}

//------------------------------------------------------------------------------

#undef txt
#undef fp
#undef keys
#undef pos
#undef wpos
#undef line
#undef col
#undef c
#undef p0
#undef p1
#undef i


//------------------------------------------------------------------------------
char* 	sgets( char **p_out, char* in, long int *pstart )
//------------------------------------------------------------------------------
	{
	int p, l;
	char *out = null;
	
	if ( ! in ) return null;
	if ( *pstart >= (l = strlen(in)) ) return null;
	if ( p_out ) out = *p_out;

	p = strpos( in, "\n", *pstart );
	if ( p >= 0 )
		{
		strdncpy( &out, &(in[*pstart]), (l = p-(*pstart)), _ );
		
		if ( ( l > 0 )&&( out[l-1] == '\r' ) )
			out[l-1] = 0;
		*pstart = p+1;
		if ( p_out ) *p_out = out;
		return out;
		}
		
	strdcpy( &out, &(in[*pstart]), _ );
	*pstart = l;
	if ( p_out ) *p_out = out;
	return out;
	}


//------------------------------------------------------------------------------
char* 	grep( char **p_out, char* in, char * search )
//------------------------------------------------------------------------------
	{
	long int p = 0;
	char **line = memptr(0);
	char *out = null;
	
	if (( ! in )||( ! search )) return null;

	if ( p_out ) out = *p_out;
	if ( out ) out[0] = 0;
	while( sgets( line, in, &p ) )
		{
		if ( str_exists( *line, search ) )
			strdcat( &out, *line, BR , _);
		}
	if ( p_out ) *p_out = out;
	return out;
	}


//------------------------------------------------------------------------------
char* 	grepi( char **p_out, char* in, char * search )
//------------------------------------------------------------------------------
	{
	long int p = 0;
	char **line = memptr(0);
	char *out = null;
	
	if (( ! in )||( ! search )) return null;

	if ( p_out ) out = *p_out;
	if ( out ) out[0] = 0;
	while( sgets( line, in, &p ) )
		{
		if ( str_iexists( *line, search ) )
			strdcat( &out, *line, BR, _ );
		}
	if ( p_out ) *p_out = out;
	return out;
	}


//------------------------------------------------------------------------------
char* 	grepv( char **p_out, char* in, char * search )
//------------------------------------------------------------------------------
	{
	char **line = memptr(0);
	long int p = 0;
	char *out = null;
	
	if (( ! in )||( ! search ))
		{
		strdcpy( p_out, "", _ );
		return *p_out;
		}

	if ( p_out ) out = *p_out;
	if ( out ) out[0] = 0;
	while( sgets( line, in, &p ) )
		{
		if ( ! str_exists( *line, search ) )
			strdcat( &out, *line, BR, _ );
		}
	if ( p_out ) *p_out = out;
	return out;
	}


//------------------------------------------------------------------------------
char* 	grepvi( char **p_out, char* in, char * search )
//------------------------------------------------------------------------------
	{
	long int p = 0;
	char **line = memptr(0), *out;
	
	if (( ! in )||( ! search )) return null;

	if ( p_out ) out = *p_out;
	if ( out ) out[0] = 0;
	while( sgets( line, in, &p ) )
		{
		if ( ! str_iexists( *line, search ) )
			strdcat( &out, *line, BR, _ );
		}
	if ( p_out ) *p_out = out;
	return out;
	}


//------------------------------------------------------------------------------				
char *	strins( char** inout, int p, char *sToInsert )
//------------------------------------------------------------------------------				
	{
	char *out = strdncpy( null, *inout, p, sToInsert, strlen( sToInsert ), _ );
	free(*inout);
	return *inout = out;
	}

//------------------------------------------------------------------------------				
char *	strdel( char**inout, int p, int len )
//------------------------------------------------------------------------------				
	{
	strcpy( &((*inout)[p]), &((*inout)[p+len]) );
	return *inout;
	}

//------------------------------------------------------------------------------				
char *	strpad( char* in, int p, char *sPad )
//------------------------------------------------------------------------------				
	{
	char **o;
	int l = p*strlen(sPad);

	if ( !in ) in = "";
	o = memptr(0);
	
	strdcpy( o, in, _ );
	while( strlen( *o ) < l )
		strdcat( o, sPad, _ );
	return *o;
	}

//------------------------------------------------------------------------------				
char *	strlpad( char* in, int p, char *sPad )
//------------------------------------------------------------------------------				
	{
	char **o;
	int l = p*strlen(sPad);

	if ( !in ) in = "";
	o = memptr(0);
	
	strdcpy( o, in, _ );
	while( strlen( *o ) < l )
		strdcpy( o, sPad, *o, _ );
	return *o;
	}






















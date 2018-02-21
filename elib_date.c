//------------------------------------------------------------------------------
double tt( char * date_begin, char *date_end, char* format )
//------------------------------------------------------------------------------
{
        struct tm tm_b, tm_e;

        #ifdef UNIX
        strptime(date_begin,"%d/%m/%Y %H:%M:%S", &tm_b);
        strptime(date_end,"%d/%m/%Y %H:%M:%S", &tm_e);

        return (difftime(mktime(&tm_e), mktime(&tm_b)));
        #endif // UNIX
        return 0;//windows
}


//------------------------------------------------------------------------------
char* itod( long int t, char* format )
//------------------------------------------------------------------------------
	{
	char *out;
	int year, month, day, hour, min, sec;
	struct tm *st;

	out = memget( 200 );

	if ( !t ) t = (long int)time(NULL);

	st = localtime( (time_t*)(&t) );

	year = st->tm_year + 1900;
	month = st->tm_mon + 1;
	day = st->tm_mday;
	hour = st->tm_hour;
	min = st->tm_min;
	sec = st->tm_sec;

	if ( ( format == NULL )||( ! stricmp( format, "dd/mm/yyyy hh:mm:ss" ) )
		||( ! stricmp( format, "jj/mm/aaaa hh:mm:ss" ) ) )
		sprintf( out, "%02d/%02d/%04d %02d:%02d:%02d",
						day, month, year, hour, min, sec );
	else
	if ( ( ! stricmp( format, "dd/mm/yyyy" ) )
		||( ! stricmp( format, "jj/mm/aaaa" ) ) )
		sprintf( out, "%02d/%02d/%04d", day, month, year );
	else
	if ( ( ! stricmp( format, "flog" ) )
		||( ! stricmp( format, "yyyy_mm_dd_hh_mm_ss" ) )
		||( ! stricmp( format, "aaaa_mm_jj_hh_mm_ss" ) ) )
		sprintf( out, "%04d_%02d_%02d_%02d_%02d_%02d",
						year, month, day, hour, min, sec );
	else
	if ( ( ! stricmp( format, "dd_mm_yyyy" ) )
		||( ! stricmp( format, "jj_mm_aaaa" ) ) )
		sprintf( out, "%02d_%02d_%04d", day, month, year );
	else
	if ( ( ! stricmp( format, "ddmmyyyy" ) )
		||( ! stricmp( format, "jjmmaaaa" ) ) )
		sprintf( out, "%02d%02d%04d", day, month, year );
	else
	if ( ( ! stricmp( format, "yyyy/mm/dd" ) )
		||( ! stricmp( format, "aaaa/mm/jj" ) ) )
		sprintf( out, "%04d/%02d/%02d", year, month, day );
	else
	if ( ( ! stricmp( format, "yyyy_mm_dd" ) )
		||( ! stricmp( format, "aaaa_mm_jj" ) ) )
		sprintf( out, "%04d_%02d_%02d", year, month, day );
	else
	if ( ( ! stricmp( format, "yyyymmdd" ) )
		||( ! stricmp( format, "aaaammjj" ) ) )
		sprintf( out, "%04d%02d%02d", year, month, day );
	else
	if ( ( ! stricmp( format, "log" ) )
		||( ! stricmp( format, "yyyy/mm/dd hh:mm:ss" ) )
		||( ! stricmp( format, "aaaa/mm/jj hh:mm:ss" ) ) )
		sprintf( out, "%04d/%02d/%02d %02d:%02d:%02d",
						year, month, day, hour, min, sec );
	else
	if ( ( ! stricmp( format, "yyyymm" ) )
		||( ! stricmp( format, "aaaamm" ) ) )
		sprintf( out, "%04d%02d", year, month );
	else
	if ( ! stricmp( format, "hh:mm:ss" ) )
		sprintf( out, "%02d:%02d:%02d", hour, min, sec );
	else
	if ( ! stricmp( format, "hhmmss" ) )
		sprintf( out, "%02d%02d%02d", hour, min, sec );

	return out;
	}


//------------------------------------------------------------------------------
int		dtoi( char *p_dt, char* format )
//------------------------------------------------------------------------------
	{
	int t;
	char s[5], *dt;
	struct tm st, st1;

	if ( p_dt ) dt = p_dt;
	else
		dt = itod( sysdate, format );

	st.tm_yday = st.tm_wday = st.tm_isdst = -1;

	if ( ( format == NULL )||( ! stricmp( format, "dd/mm/yyyy hh:mm:ss" ) )
		||( ! stricmp( format, "jj/mm/aaaa hh:mm:ss" ) ) )
		{
		st.tm_year = atoi(strncpy(s, &(dt[6]), 4)) - 1900;
		st.tm_mon = atoi(strncpy(s, &(dt[3]), 2)) - 1;
		st.tm_mday = atoi(strncpy(s, dt, 2));
		st.tm_hour = atoi(strncpy(s, &(dt[11]), 2));
		st.tm_min = atoi(strncpy(s, &(dt[14]), 2));
		st.tm_sec = atoi(strncpy(s, &(dt[17]), 2));
		}
	else
	if ( ( ! stricmp( format, "dd/mm/yyyy" ) )
		||( ! stricmp( format, "jj/mm/aaaa" ) ) )
		{
		st.tm_year = atoi(strncpy(s, &(dt[6]), 4)) - 1900;
		st.tm_mon = atoi(strncpy(s, &(dt[3]), 2)) - 1;
		st.tm_mday = atoi(strncpy(s, dt, 2));
		st.tm_hour = 0;
		st.tm_min = 0;
		st.tm_sec = 0;
		}
	else
	if ( ( ! stricmp( format, "flog" ) )
		||( ! stricmp( format, "yyyy_mm_dd_hh_mm_ss" ) )
		||( ! stricmp( format, "aaaa_mm_jj_hh_mm_ss" ) ) )
		{
		st.tm_year = atoi(strncpy(s, &(dt[0]), 4)) - 1900;
		st.tm_mon = atoi(strncpy(s, &(dt[5]), 2)) - 1;
		st.tm_mday = atoi(strncpy(s, &(dt[8]), 2));
		st.tm_hour = atoi(strncpy(s, &(dt[11]), 2));
		st.tm_min = atoi(strncpy(s, &(dt[14]), 2));
		st.tm_sec = atoi(strncpy(s, &(dt[17]), 2));
		}
	else
	if ( ( ! stricmp( format, "dd_mm_yyyy" ) )
		||( ! stricmp( format, "jj_mm_aaaa" ) ) )
		{
		st.tm_year = atoi(strncpy(s, &(dt[6]), 4)) - 1900;
		st.tm_mon = atoi(strncpy(s, &(dt[3]), 2)) - 1;
		st.tm_mday = atoi(strncpy(s, dt, 2));
		st.tm_hour = 0;
		st.tm_min = 0;
		st.tm_sec = 0;
		}
	else
	if ( ( ! stricmp( format, "ddmmyyyy" ) )
		||( ! stricmp( format, "jjmmaaaa" ) ) )
		{
		st.tm_year = atoi(strncpy(s, &(dt[4]), 4)) - 1900;
		st.tm_mon = atoi(strncpy(s, &(dt[2]), 2)) - 1;
		st.tm_mday = atoi(strncpy(s, dt, 2));
		st.tm_hour = 0;
		st.tm_min = 0;
		st.tm_sec = 0;
		}
	else
	if ( ( ! stricmp( format, "yyyy/mm/dd" ) )
		||( ! stricmp( format, "aaaa/mm/jj" ) ) )
		{
		st.tm_year = atoi(strncpy(s, &(dt[0]), 4)) - 1900;
		st.tm_mon = atoi(strncpy(s, &(dt[5]), 2)) - 1;
		st.tm_mday = atoi(strncpy(s, &(dt[8]), 2));
		st.tm_hour = 0;
		st.tm_min = 0;
		st.tm_sec = 0;
		}
	else
	if ( ( ! stricmp( format, "yyyy_mm_dd" ) )
		||( ! stricmp( format, "aaaa_mm_jj" ) ) )
		{
		st.tm_year = atoi(strncpy(s, &(dt[0]), 4)) - 1900;
		st.tm_mon = atoi(strncpy(s, &(dt[5]), 2)) - 1;
		st.tm_mday = atoi(strncpy(s, &(dt[8]), 2));
		st.tm_hour = 0;
		st.tm_min = 0;
		st.tm_sec = 0;
		}
	else
	if ( ( ! stricmp( format, "yyyymmdd" ) )
		||( ! stricmp( format, "aaaammjj" ) ) )
		{
		st.tm_year = atoi(strncpy(s, &(dt[0]), 4)) - 1900;
		st.tm_mon = atoi(strncpy(s, &(dt[4]), 2)) - 1;
		st.tm_mday = atoi(strncpy(s, &(dt[6]), 2));
		st.tm_hour = 0;
		st.tm_min = 0;
		st.tm_sec = 0;
		}
	else
	if ( ( ! stricmp( format, "log" ) )
		||( ! stricmp( format, "yyyy/mm/dd hh:mm:ss" ) )
		||( ! stricmp( format, "aaaa/mm/jj hh:mm:ss" ) ) )
		{
		st.tm_year = atoi(strncpy(s, &(dt[0]), 4)) - 1900;
		st.tm_mon = atoi(strncpy(s, &(dt[5]), 2)) - 1;
		st.tm_mday = atoi(strncpy(s, &(dt[8]), 2));
		st.tm_hour = atoi(strncpy(s, &(dt[11]), 2));
		st.tm_min = atoi(strncpy(s, &(dt[14]), 2));
		st.tm_sec = atoi(strncpy(s, &(dt[17]), 2));
		}

	memcpy( &st1, &st, (long int)sizeof(struct tm) );
	t = mktime( &st1 );

	// HEURES D'ETE D'HIVERS

	if ( st.tm_hour != st1.tm_hour )
		{
		t += (st.tm_hour - st1.tm_hour)*60*60;
		if ( st.tm_hour == 0 )
			t += 24*60*60;
		}

	return t;
	}

//------------------------------------------------------------------------------
int		year( long int t )
//------------------------------------------------------------------------------
	{
	struct tm * st;

	if ( !t ) t = (int)time(NULL);
	st = localtime( (time_t*)(&t) );
	return st->tm_year + 1900;
	}

//------------------------------------------------------------------------------
int		month( long int t )
//------------------------------------------------------------------------------
	{
	struct tm * st;

	if ( !t ) t = (int)time(NULL);
	st = localtime( (time_t*)(&t) );
	return st->tm_mon + 1;
	}

//------------------------------------------------------------------------------
int		day( long int t )
//------------------------------------------------------------------------------
	{
	struct tm * st;

	if ( !t ) t = (int)time(NULL);
	st = localtime( (time_t*)(&t) );
	return st->tm_mday;
	}

//------------------------------------------------------------------------------
int		wday( long int t )
//------------------------------------------------------------------------------
	{
	struct tm * st;

	if ( !t ) t = (int)time(NULL);
	st = localtime( (time_t*)(&t) );
	return (st->tm_wday+6)%7;
	}

//------------------------------------------------------------------------------
int		lday( long int t )
//------------------------------------------------------------------------------
	{
	int m = month(t);

	switch( m )
		{
		case 4 :
		case 6 :
		case 9 :
		case 11 :
			return 30;
		}
	if ( m != 2 ) return 31;
	if ( year( t ) % 4 ) return 28;
	return 29;
	}

//------------------------------------------------------------------------------
int		fwday( long int t, int w )
//------------------------------------------------------------------------------
	{
	char s[32];

	if ( ( w < 0 )||( w > 6 ) ) return 0;
	if ( !t ) t =(int)time(NULL);
	sprintf( s, "01/%02d/%04d", month(t), year(t) );
	t = dtoi( s, "jj/mm/aaaa" );
	while( wday(t) != w )
		t += (60*60*24);

	return( t );
	}

//------------------------------------------------------------------------------
int		lwday( long int t, int w )
//------------------------------------------------------------------------------
	{
	char s[32];

	if ( ( w < 0 )||( w > 6 ) ) return 0;
	if ( !t ) t =(int)time(NULL);
	sprintf( s, "%02d/%02d/%04d", lday(t), month(t), year(t) );
	t = dtoi( s, "jj/mm/aaaa" );
	while( wday(t) != w )
		t -= (60*60*24);

	return( t );
	}


//------------------------------------------------------------------------------
int hday( long int t )
//------------------------------------------------------------------------------
	{
	struct tm *st;

	if ( !t ) t = (long int)time(NULL);

	st = localtime( (time_t*)(&t) );
	return st->tm_hour;
	}


//------------------------------------------------------------------------------
bool est_ferie( char *d )
//------------------------------------------------------------------------------
	{
	char s[12];

	strncpy( s, d, 5 );
	s[5] = ','; s[6] = 0;

	if ( strpos( "01/01,01/05,08/05,14/07,15/08,01/11,11/11,25/12,", s, 0 ) >= 0 )
			return true;

	strncpy( s, d, 10 );
	s[10] = ','; s[11] = 0;

	if ( strpos( "13/04/2009,21/05/2009,01/06/2009,"
			"05/04/2010,13/05/2010,24/05/2010,"
			"25/04/2011,02/06/2011,13/06/2011,"
			"09/04/2012,17/05/2012,28/05/2012,"
			"01/04/2013,09/05/2013,20/05/2013,"
			"20/04/2014,28/05/2014,09/06/2014,"
			"05/04/2015,13/05/2015,24/05/2015,"
			"28/03/2016,05/05/2016,16/05/2016,"
			"17/04/2017,25/05/2017,05/06/2017,"
			"02/04/2018,10/05/2018,21/05/2018,"
			"22/04/2019,30/05/2019,10/06/2019,"
			"13/04/2020,21/05/2020,01/06/2020,",
			s, 0 ) >= 0 )
			return true;

	return false;
	}


//------------------------------------------------------------------------------
bool verif_date_hh_mm( int *in, char *ph1, char *ph2 )
// L'HEURE DE LA DATE in, DOIT ETRE COMPRISE ENTRE LES HORAIRES ph1 et ph2
// SI L'HEURE EST PLUS RECENTE QUE ph1, ALORS ON RETOURNE LA DATE in AVEC L'HEURE ph1
// SINON ON RETOURNE FAUX OU VRAI
// ph1 et ph2 sont de la forme HH_MM
//------------------------------------------------------------------------------
	{
	char *dt0, dt1[20];

	dt0 = itod( *in, "aaaa_mm_jj_hh_mm_ss" );

	strncpy( dt1, dt0, 11 );
	strncat( dt1, ph1, 5 );
	strcat( dt1, "_00" );

	if ( strcmp( dt0, dt1 ) < 0 )
		{
		*in = dtoi( dt1, "aaaa_mm_jj_hh_mm_ss" );
		return true;
		}

	strncpy( dt1, dt0, 11 );
	strncat( dt1, ph2, 5 );
	strcat( dt1, "_00" );

	if ( strcmp( dt0, dt1 ) >= 0 ) return false;
	return true;
	}


//------------------------------------------------------------------------------
bool verif_date2_hh_mm( int in, char *ph1, char *ph2 )
// L'HEURE DE LA DATE in, DOIT ETRE COMPRISE ENTRE LES HORAIRES ph1 et ph2
//------------------------------------------------------------------------------
	{
	char *dt0, dt1[20];

	dt0 = itod( in, "aaaa_mm_jj_hh_mm_ss" );

	strncpy( dt1, dt0, 11 );
	strncat( dt1, ph1, 5 );
	strcat( dt1, "_00" );

	if ( strcmp( dt0, dt1 ) < 0 )
		return false;

	strncpy( dt1, dt0, 11 );
	strncat( dt1, ph2, 5 );
	strcat( dt1, "_00" );

	if ( strcmp( dt0, dt1 ) >= 0 ) return false;
	return true;
	}


//------------------------------------------------------------------------------
int planif_compute( char *planif, int last_exec )
//------------------------------------------------------------------------------
	{
	int nn, h, o, p, p1, p2, r, i, id, j, l, n, n1, n2, n3, n4, b, b1, tm;
	handle hConf1 = 0, k;
	char *lconf, *nconf, *s = null, *s1 = null, *s2 =null, *s3 = null,
		*s4 = null, *v, *v1, *v2, *v3, **ts, *sid;

tr("planif_compute1");
	ts = str_explode( planif, "|" );

	r = nn = 0;
	h = last_exec;
	tm = (int)time(NULL);

	// Calcul de la prochaine date
	hConf1 = 0;
	for( ; ; )
		{
tr("planif_compute2");
		for( id = 0 ; ts[id] != null ; id++ )
			{
tr("planif_compute3");
			sid = itoa( id+1 );
tr("planif_compute3a");
			if ( hConf1 ) var_free( hConf1 );
tr2("planif_compute3b:", ts[id]);
			hConf1 = var_from_str( 0, ts[id] );
tr2("planif_compute3c:", itoa(hConf1));
			if ( ! hConf1 ) continue;
tr("planif_compute3d");

			v = var_get( hConf1, "time" );
tr("planif_compute3e");
			if ( v ) tm = dtoi( v, "jj/mm/aaaa hh:mm:ss" );
tr("planif_compute3f");

			if ( h == 0 ) h = tm;
tr("planif_compute3g");

			v = var_get( hConf1, "type" );
tr("planif_compute3h");
			if ( !v ) break;
tr("planif_compute3i");
			n = atoi( v );
tr("planif_compute3j");
			if ( n == 0 ) break;
tr("planif_compute3k");
			if ( n > 99 ) continue;
tr("planif_compute4");
			switch( n )
				{
				//------------------------------------------------------------------
				case 1 : // toutes les N minutes
				//------------------------------------------------------------------
				//------------------------------------------------------------------
				case 2 : // toutes les N heures
				//------------------------------------------------------------------

					// Calcul la période en secondes
					v = var_get( hConf1, "n" );
					if ( v == null )
						{
						error_set( 1, "ERREUR: ", sid, " Mauvais parametre N", _ );
						goto planif_fin_error;
						}
					p = atoi( v );
					if (( p == 0 )||( p > 24*60 ))
						{
						error_set( 1, "ERREUR: ", sid, " Le parametre N (minutes) doit etre compris entre 1 et 1440", _ );
						goto planif_fin_error;
						}
					p *= (( n == 1 )? 60 : 60*60);

					// Recherche la date de prochaine exécution postérieur à la date actuelle

					o = h;
					if ( o < (tm-p) ) o = tm;
					o += p;

					// ARRONDI A LA MINUTE ANTERIEURE

					v = itod( o, "jj/mm/aaaa hh:mm:ss" );
					strdncpy( &s, v, 17, "00", 2, _ );
					o = dtoi( s, "jj/mm/aaaa hh:mm:ss" );

					// Vérifie que la date se trouve dans l'une des plages horaires
					v = upper(trim(var_get( hConf1, "horaire" )));
					if (( v )&&( v[0] ))
						{
						k = text_init( v, "DE AET : H , ;" );
						i = 0;
						while( ( text_getword( &s3, k ) ) != null )
							{
							if ( i == 0 ) // Lecture de l'heure de départ
								{
								if ( ( strcmp( s3, "DE" ) )&&( strcmp( s3, "ET" ) )&&( strcmp( s3, "," ) )&&( strcmp( s3, ";" ) ) )
									{
									if ( ! is_numeric( s3 ) )
										{
										error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 1", _ );
										goto planif_fin_error;
										}
									strdcpy( &s1, itona(atoi(s3),2), "_", _ );
									i = 1;
									}
								continue;
								}
							if ( i == 1 ) // Lecture séparateur minutes de départ
								{
								if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
									{
									i = 2;
									continue;
									}

								if ( ! strcmp( s3, "A" ) )
									{
									i = 4;
									strdcat( &s1, "00", _ );
									continue;
									}

								error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 2", _ );
								goto planif_fin_error;
								}
							if ( i == 2 ) // Lecture minutes de départ
								{
								if ( ! strcmp( s3, "A" ) )
									{
									i = 4;
									strdcat( &s1, "00", _ );
									continue;
									}

								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 3", _ );
									goto planif_fin_error;
									}
								strdcat( &s1, itona(atoi(s3),2), _ );
								i = 3;
								continue;
								}
							if ( i == 3 ) // Lecture du A
								{
								if ( ! strcmp( s3, "A" ) )
									{
									i = 4;
									continue;
									}
								else
									{
									error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 4", _ );
									goto planif_fin_error;
									}
								}
							if ( i == 4 ) // Lecture heure de fin
								{
								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 5", _ );
									goto planif_fin_error;
									}
								strdcpy( &s2, itona(atoi(s3),2), "_", _ );
								i = 5;
								continue;
								}
							if ( i == 5 ) // Lecture séparateur minutes de départ
								{
								if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
									{
									i = 6;
									continue;
									}

								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									strdcat( &s2, "00", _ );
									if ( verif_date_hh_mm( &o, s1, s2 ) == true )
										{
										i = 1000;
										break;
										}
									i = 0;
									continue;
									}

								error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 6", _ );
								goto planif_fin_error;
								}
							if ( i == 6 ) // Lecture minutes fin
								{
								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									strdcat( &s2, "00", _ );
									if ( verif_date_hh_mm( &o, s1, s2 ) == true )
										{
										i = 1000;
										break;
										}
									i = 0;
									continue;
									}
								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 7", _ );
									goto planif_fin_error;
									}
								strdcat( &s2, itona(atoi(s3),2), _ );
								if ( strcmp( s1, s2 ) > 0 )
									{
									error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 7b", _ );
									goto planif_fin_error;
									}
								if ( verif_date_hh_mm( &o, s1, s2 ) == true )
									{
									i = 1000;
									break;
									}
								i = 0;
								continue;
								}
							}

						if ( i != 1000 )
							{
							b = true;
							if ( i )
								{
								if ( i < 5 )
									{
									error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 8", _ );
									goto planif_fin_error;
									}
								strdcat( &s2, "00", _ );
								if ( strcmp( s1, s2 ) > 0 )
									{
									error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 8b", _ );
									goto planif_fin_error;
									}
								if ( verif_date_hh_mm( &o, s1, s2 ) == false )
									b = false;
								}
							else
								b = false;
							if ( !b )
								{
								// RECHERCHE LE JOUR SUIVANT
								o = dtoi(itod( o + (60*60*24), "jj/mm/aaaa" ),  "jj/mm/aaaa" );
								if ( verif_date_hh_mm( &o, s1, s2 ) == false )
									o = 0;
								}
							}
						}

					break;

				//------------------------------------------------------------------
				case 3 : // tous les jours
				//------------------------------------------------------------------

tr("planif_compute5");

				    o = 0;
					v = upper(trim(var_get( hConf1, "horaire" )));
					if (( !v )||( ! v[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " aucun horaire n'a été donné.", _ );
						goto planif_fin_error;
						}

tr("planif_compute5a");

					for( j = 0 ; ( j <= 24*60*60 )&&( o == 0 ) ; j += 24*60*60 )
						{
tr("planif_compute5b");
						k = text_init( v, " AET : H , ;" );
						i = 0;
tr("planif_compute5c");
						while( ( text_getword( &s3, k ) ) != null )
							{
tr("planif_compute5d");
							if ( i == 0 ) // Lecture de l'heure de départ
								{
								if ( ( ! strcmp( s3, "A" ) )||( ! strcmp( s3, "ET" ) )
									||( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) ))
									continue;
								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 1", _ );
									goto planif_fin_error;
									}
								strdcpy( &s2, itona(atoi(s3),2), "_", _ );
								i = 1;
								continue;
								}
tr("planif_compute5e");
							if ( i == 1 ) // Lecture du séparateur des minutes
								{
								if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
									{
									i = 2;
									continue;
									}

								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
									if ( o )
										v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
									else
										v1 = itod( h+j, "aaaa_mm_jj_hh_mm_ss" );
									strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
									if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
										o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
									i = 0;
									continue;
									}

								error_set( 1, "ERREUR: ", sid, " horaires incorrectes 2", _ );
								goto planif_fin_error;
								}

tr("planif_compute5f");
							if ( i == 2 )
								{
								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									strdcat( &s2, "00_00", _ );
									}
								else
								if ( ! is_numeric(s3) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 3", _ );
									goto planif_fin_error;
									}
								else
									strdcat( &s2, itona(atoi(s3),2), "_00", _ );

								v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
								if ( o )
									v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
								else
									v1 = itod( h+j, "aaaa_mm_jj_hh_mm_ss" );
								strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, _ );
								if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
									o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );

								i = 0;
								continue;
								}
							}
tr("planif_compute5g");
						if ( i != 0 )
							{
							v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
							if ( o )
								v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
							else
								v1 = itod( h+j, "aaaa_mm_jj_hh_mm_ss" );
							strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
							if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
								o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
							}
						}
tr("planif_compute5h");

					break;

				//------------------------------------------------------------------
				case 4 : // tous les J de la semaine
				//------------------------------------------------------------------

					v = upper(trim(var_get( hConf1, "horaire" )));
					if (( !v )||( ! v[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " : aucun horaire n'a été donné.", _ );
						goto planif_fin_error;
						}

				    n = time(NULL);
				    if ( h > n ) n = h;

					for( j = o = 0 ; ( j <= 7  )&&( o == 0 ) ; j++ )
						{
						if ( j ) n += 24*60*60;
						if ( j == 1 )
							n = dtoi( itod( n, "aaaa_mm_jj" ), "aaaa_mm_jj" ); // Jour suivant à 00h00

						// vérifie que le jour est demandé
						v3 = null;
						switch( wday( n ) )
							{
							case 0 : v3 = var_get( hConf1, "lundi" ); break;
							case 1 : v3 = var_get( hConf1, "mardi" ); break;
							case 2 : v3 = var_get( hConf1, "mercredi" ); break;
							case 3 : v3 = var_get( hConf1, "jeudi" ); break;
							case 4 : v3 = var_get( hConf1, "vendredi" ); break;
							case 5 : v3 = var_get( hConf1, "samedi" ); break;
							case 6 : v3 = var_get( hConf1, "dimanche" ); break;
							}

						if ( ( !v3 )||( strcmp(v3, "true") ) ) continue;

						// lecture des horaires
						k = text_init( v, " AET : H , ;" );
						i = 0;
						while( ( text_getword( &s3, k ) ) != null )
							{
							if ( i == 0 ) // Lecture de l'heure de départ
								{
								if ( ( ! strcmp( s3, "A" ) )||( ! strcmp( s3, "ET" ) )
									||( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) ))
									continue;
								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 1", _ );
									goto planif_fin_error;
									}
								strdcpy( &s2, itona(atoi(s3),2), "_", _ );
								i = 1;
								continue;
								}
							if ( i == 1 ) // Lecture du séparateur des minutes
								{
								if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
									{
									i = 2;
									continue;
									}

								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
									if ( o )
										v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
									else
										v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
									strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
									if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
										o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
									i = 0;
									continue;
									}

								error_set( 1, "ERREUR: ", sid, " horaires incorrectes 2", _ );
								goto planif_fin_error;
								}

							if ( i == 2 )
								{
								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									strdcat( &s2, "00_00", _ );
									}
								else
								if ( ! is_numeric(s3) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 3", _ );
									goto planif_fin_error;
									}
								else
									strdcat( &s2, itona(atoi(s3),2), "_00", _ );

								v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
								if ( o )
									v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
								else
									v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
								strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, _ );
								if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
									o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );

								i = 0;
								continue;
								}
							}
						if ( i != 0 )
							{
							v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
							if ( o )
								v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
							else
								v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
							strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
							if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
								o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
							}
						}

					break;

				//------------------------------------------------------------------
				case 5 : // tous les premiers J du mois ou de l'annèe
				//------------------------------------------------------------------

					v = upper(trim(var_get( hConf1, "horaire" )));
					if (( !v )||( ! v[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " : aucun horaire n'a été donné.", _ );
						goto planif_fin_error;
						}

					lconf = trim(var_get( hConf1, "periode" ));
					if (( !lconf )||(( strcmp( lconf, "m" ) )&&( strcmp( lconf, "a" ) )))
						{
						error_set( 1, "ERREUR: ", sid, " : la période (mois ou année) n'a été donnée.", _ );
						goto planif_fin_error;
						}

				    n = time(NULL);
				    if ( h > n ) n = h;

					for( j = 0 ; j < 8 ; j++, n += 60*60*24 )
						{
						if ( ! strcmp( lconf, "m" ) )
							{
							if ( day(n) > 7 )
								{
								if ( (n1 = month(n)) < 12 )
									strdcpy( &s4, "01/", itona(n1+1, 2), "/", itoa(year(n)), _ );
								else
									strdcpy( &s4, "01/01/", itoa(year(n)+1), _ );
								n = dtoi( s4, "jj/mm/aaaa" );
								j = 0;
								}
							}
						else
							{
							if ( ( month(n) != 1 )||( day(n) > 7 ) )
								{
								strdcpy( &s4, "01/01/", itoa(year(n)+1), _ );
								n = dtoi( s4, "jj/mm/aaaa" );
								j = 0;
								}
							}
						if ( j == 1 )
							n = dtoi( itod( n, "aaaa_mm_jj" ), "aaaa_mm_jj" ); // Jour suivant à 00h00

						switch( wday( n ) )
							{
							case 0 : v3 = var_get( hConf1, "lundi" ); break;
							case 1 : v3 = var_get( hConf1, "mardi" ); break;
							case 2 : v3 = var_get( hConf1, "mercredi" ); break;
							case 3 : v3 = var_get( hConf1, "jeudi" ); break;
							case 4 : v3 = var_get( hConf1, "vendredi" ); break;
							case 5 : v3 = var_get( hConf1, "samedi" ); break;
							case 6 : v3 = var_get( hConf1, "dimanche" ); break;
							}
						if ( ( !v3 )||( strcmp(v3, "true") ) ) continue;

						// lecture des horaires
						k = text_init( v, " AET : H , ;" );
						i = o = 0;
						while( ( text_getword( &s3, k ) ) != null )
							{
							if ( i == 0 ) // Lecture de l'heure de départ
								{
								if ( ( ! strcmp( s3, "A" ) )||( ! strcmp( s3, "ET" ) )
									||( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) ))
									continue;
								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 1", _ );
									goto planif_fin_error;
									}
								strdcpy( &s2, itona(atoi(s3),2), "_", _ );
								i = 1;
								continue;
								}
							if ( i == 1 ) // Lecture du séparateur des minutes
								{
								if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
									{
									i = 2;
									continue;
									}

								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
									if ( o )
										v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
									else
										v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
									strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
									if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
										o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
									i = 0;
									continue;
									}

								error_set( 1, "ERREUR: ", sid, " horaires incorrectes 2", _ );
								goto planif_fin_error;
								}

							if ( i == 2 )
								{
								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									strdcat( &s2, "00_00", _ );
									}
								else
								if ( ! is_numeric(s3) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 3", _ );
									goto planif_fin_error;
									}
								else
									strdcat( &s2, itona(atoi(s3),2), "_00", _ );

								v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
								if ( o )
									v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
								else
									v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
								strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, _ );
								if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
									o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );

								i = 0;
								continue;
								}
							}
						if ( i != 0 )
							{
							v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
							if ( o )
								v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
							else
								v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
							strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
							if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
								o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
							}
						if ( o ) break;
						}

					break;

				//------------------------------------------------------------------
				case 6 : // tous les derniers J du mois ou de l'annèe
				//------------------------------------------------------------------

					v = upper(trim(var_get( hConf1, "horaire" )));
					if (( !v )||( ! v[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " : aucun horaire n'a été donné.", _ );
						goto planif_fin_error;
						}

					lconf = trim(var_get( hConf1, "periode" ));
					if (( !lconf )||(( strcmp( lconf, "m" ) )&&( strcmp( lconf, "a" ) )))
						{
						error_set( 1, "ERREUR: ", sid, " : la période (mois ou année) n'a été donnée.", _ );
						goto planif_fin_error;
						}

					n = tm;
					if ( ! strcmp( lconf, "m" ) )
						strdcpy( &s4, itona(lday(n)-6,2), "/", itona(month(n), 2),
									"/", itoa(year(n)), _ );
					else
						strdcpy( &s4, "25/12/", itoa(year(n)), _ );
					n = dtoi( s4, "jj/mm/aaaa" );
				    if ( h > n ) n = h;

					for( j = 0 ; j < 8 ; j++, n += 60*60*24 )
						{
						if ( day(n) == 1 )
							if ( ! strcmp( lconf, "m" ) )
								{
								strdcpy( &s4, itona(lday(n)-6,2),
										"/", itona(month(n), 2), "/", itoa(year(n)), _ );
								n = dtoi( s4, "jj/mm/aaaa" );
								j = 0;
								}
							else
								{
								strdcpy( &s4, "25/12/", itoa(year(n)), _ );
								n = dtoi( s4, "jj/mm/aaaa" );
								j = 0;
								}
						if ( j == 1 )
							n = dtoi( itod( n, "aaaa_mm_jj" ), "aaaa_mm_jj" ); // Jour suivant à 00h00

						switch( wday( n ) )
							{
							case 0 : v3 = var_get( hConf1, "lundi" ); break;
							case 1 : v3 = var_get( hConf1, "mardi" ); break;
							case 2 : v3 = var_get( hConf1, "mercredi" ); break;
							case 3 : v3 = var_get( hConf1, "jeudi" ); break;
							case 4 : v3 = var_get( hConf1, "vendredi" ); break;
							case 5 : v3 = var_get( hConf1, "samedi" ); break;
							case 6 : v3 = var_get( hConf1, "dimanche" ); break;
							}
						if ( ( !v3 )||( strcmp(v3, "true") ) ) continue;

						// lecture des horaires
						k = text_init( v, " AET : H , ;" );
						i = o = 0;
						while( ( text_getword( &s3, k ) ) != null )
							{
							if ( i == 0 ) // Lecture de l'heure de départ
								{
								if ( ( ! strcmp( s3, "A" ) )||( ! strcmp( s3, "ET" ) )
									||( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) ))
									continue;
								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 1", _ );
									goto planif_fin_error;
									}
								strdcpy( &s2, itona(atoi(s3),2), "_", _ );
								i = 1;
								continue;
								}
							if ( i == 1 ) // Lecture du séparateur des minutes
								{
								if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
									{
									i = 2;
									continue;
									}

								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
									if ( o )
										v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
									else
										v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
									strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
									if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
										o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
									i = 0;
									continue;
									}

								error_set( 1, "ERREUR: ", sid, " horaires incorrectes 2", _ );
								goto planif_fin_error;
								}

							if ( i == 2 )
								{
								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									strdcat( &s2, "00_00", _ );
									}
								else
								if ( ! is_numeric(s3) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 3", _ );
									goto planif_fin_error;
									}
								else
									strdcat( &s2, itona(atoi(s3),2), "_00", _ );

								v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
								if ( o )
									v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
								else
									v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
								strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, _ );
								if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
									o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );

								i = 0;
								continue;
								}
							}
						if ( i != 0 )
							{
							v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
							if ( o )
								v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
							else
								v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
							strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
							if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
								o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
							}
						if ( o ) break;
						}

					break;

				//------------------------------------------------------------------
				case 7 : // tous les jj du mois
				//------------------------------------------------------------------

					nconf = upper(trim(var_get( hConf1, "n" )));
					if (( !nconf )||( ! nconf[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " aucune date n'a été donnée.", _ );
						goto planif_fin_error;
						}

					v = upper(trim(var_get( hConf1, "horaire" )));
					if (( !v )||( ! v[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " aucun horaire n'a été donné.", _ );
						goto planif_fin_error;
						}

				    n = time(NULL);
				    if ( h > n ) n = h;

					// recherche sur les 31 jours qui suivent
					for( j = o = 0 ; ( j <= 31  )&&( o == 0 ) ; j++ )
						{
						if ( j ) n += 24*60*60;
						if ( j == 1 )
							n = dtoi( itod( n, "aaaa_mm_jj" ), "aaaa_mm_jj" );
						// vérifie que le jour est demandé
						k = day(n);
						l = text_init( nconf, " , ;ET" );
						b = false;
						while( ( text_getword(&s3, l) ) != null )
							{
							if ( k == atoi(s3) ) { b = true; break; }
							if ( ( atoi(s3) == 31 )&&( k == lday(n) ) ) { b = true; break; }
							}

						if ( ! b ) continue;

						// lecture des horaires
						k = text_init( v, " AET : H , ;" );
						i = 0;
						while( ( text_getword( &s3, k ) ) != null )
							{
							if ( i == 0 ) // Lecture de l'heure de départ
								{
								if ( ( ! strcmp( s3, "A" ) )||( ! strcmp( s3, "ET" ) )
									||( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) ))
									continue;
								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 1", _ );
									goto planif_fin_error;
									}
								strdcpy( &s2, itona(atoi(s3),2), "_", _ );
								i = 1;
								continue;
								}
							if ( i == 1 ) // Lecture du séparateur des minutes
								{
								if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
									{
									i = 2;
									continue;
									}

								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
									if ( o )
										v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
									else
										v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
									strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
									if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
										o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
									i = 0;
									continue;
									}

								error_set( 1, "ERREUR: ", sid, " horaires incorrectes 2", _ );
								goto planif_fin_error;
								}

							if ( i == 2 )
								{
								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									strdcat( &s2, "00_00", _ );
									}
								else
								if ( ! is_numeric(s3) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 3", _ );
									goto planif_fin_error;
									}
								else
									strdcat( &s2, itona(atoi(s3),2), "_00", _ );

								v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
								if ( o )
									v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
								else
									v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
								strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, _ );
								if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
									o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );

								i = 0;
								continue;
								}
							}
						if ( i != 0 )
							{
							v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
							if ( o )
								v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
							else
								v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
							strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
							if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
								o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
							}
						}

					break;

				//------------------------------------------------------------------
				case 8 : // tous les jj/mm
				//------------------------------------------------------------------

					nconf = upper(trim(var_get( hConf1, "n" )));

					if (( !nconf )||( ! nconf[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " aucune date n'a été donnée.", _ );
						goto planif_fin_error;
						}

					v = upper(trim(var_get( hConf1, "horaire" )));
					if (( !v )||( ! v[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " aucun horaire n'a été donné.", _ );
						goto planif_fin_error;
						}

				    n = time(NULL);
				    if ( h > n ) n = h;

					// recherche sur les 366 jours qui suivent
					for( j = o = 0 ; ( j <= 366  )&&( o == 0 ) ; j++ )
						{
						if ( j ) n += 24*60*60;
						if ( j == 1 )
							n = dtoi( itod( n, "aaaa_mm_jj" ), "aaaa_mm_jj" );
						// vérifie que le jour est demandé
						v1 = itod( n, "jj/mm/aaaa" );
						v1[5] = 0;
						l = text_init( nconf, " , ;ET" );
						b = false;
						while( ( text_getword(&s3, l) ) != null )
							{
							if ( ( !strcmp( s3, "," ) )||( !strcmp( s3, ";" ) )||( !strcmp( s3, "ET" ) ) )
								continue;
							if ( ( p1 = strpos(s3, "/", 0) ) < 0 )
								{
								error_set( 1, "ERREUR: ", sid, " une des dates données n'est pas correcte.", _ );
								goto planif_fin_error;
								}
							s3[p1] = 0;
							strdcpy( &s3, itona(atoi(s3),2), "/", itona(atoi(&(s3[p1+1])),2), _ );
							if ( ! strcmp( s3, v1 ) ) { b = true; break; }
							}

						if ( !b ) continue;

						// lecture des horaires
						k = text_init( v, " AET : H , ;" );
						i = 0;
						while( ( text_getword( &s3, k ) ) != null )
							{
							if ( i == 0 ) // Lecture de l'heure de départ
								{
								if ( ( ! strcmp( s3, "A" ) )||( ! strcmp( s3, "ET" ) )
									||( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) ))
									continue;
								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 1", _ );
									goto planif_fin_error;
									}
								strdcpy( &s2, itona(atoi(s3),2), "_", _ );
								i = 1;
								continue;
								}
							if ( i == 1 ) // Lecture du séparateur des minutes
								{
								if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
									{
									i = 2;
									continue;
									}

								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
									if ( o )
										v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
									else
										v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
									strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
									if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
										o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
									i = 0;
									continue;
									}

								error_set( 1, "ERREUR: ", sid, " horaires incorrectes 2", _ );
								goto planif_fin_error;
								}

							if ( i == 2 )
								{
								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									strdcat( &s2, "00_00", _ );
									}
								else
								if ( ! is_numeric(s3) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 3", _ );
									goto planif_fin_error;
									}
								else
									strdcat( &s2, itona(atoi(s3),2), "_00", _ );
								v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
								if ( o )
									v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
								else
									v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
								strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, _ );
								if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
									o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );

								i = 0;
								continue;
								}
							}
						if ( i != 0 )
							{
							v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
							if ( o )
								v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
							else
								v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
							strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
							if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
								o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
							}
						}

					break;

				//------------------------------------------------------------------
				case 9 : // les jj/mm/aaaa
				//------------------------------------------------------------------

					nconf = upper(trim(var_get( hConf1, "n" )));
					if (( !nconf )||( ! nconf[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " aucune date n'a été donnée.", _ );
						goto planif_fin_error;
						}

					v = upper(trim(var_get( hConf1, "horaire" )));
					if (( !v )||( ! v[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " aucun horaire n'a été donné.", _ );
						goto planif_fin_error;
						}

				    n = time(NULL);
				    if ( h > n ) n = h;

					// recherche sur les dates demandées

					l = text_init( nconf, " , ;ET" );
					while( ( text_getword(&s3, l) ) != null )
						{
						if ( ( !strcmp( s3, "," ) )||( !strcmp( s3, ";" ) )||( !strcmp( s3, "ET" ) ) )
							continue;

						if ( strnicmp( s3, "ferie", -1 ) )
							{
							if ( ( p1 = strpos(s3, "/", 0) ) < 0 )
								{
								error_set( 1, "ERREUR: ", sid, " une des dates données n'est pas correcte.", _ );
								goto planif_fin_error;
								}
							if ( ( p2 = strpos(s3, "/", p1+1) ) < 0 )
								{
								error_set( 1, "ERREUR: ", sid, " une des dates données n'est pas correcte.", _ );
								goto planif_fin_error;
								}
							s3[p1] = 0;
							k = atoi( &(s3[p2+1]) );
							strdcpy( &s3, itona(atoi(s3),2), "/", itona(atoi(&(s3[p1+1])),2), "/",
										itoa((k<100)? k+2000 : k), _ );
							if ( dtoi( s3, "jj/mm/aaaa" ) < dtoi( itod( h, "aaaa_mm_jj" ), "aaaa_mm_jj" ) )
								continue;
							n = dtoi(s3, "jj/mm/aaaa");
							}
						else
							{
							n1 = h;
							while(1)
								{
								n1 += 24*60*60;
								v3 = itod( n1, "jj/mm/aaaa" );
								if( est_ferie( v3 ) ) break;
								}
							n = dtoi(v3, "jj/mm/aaaa");
							}

						if ( n < h ) n = h;

						// lecture des horaires
						k = text_init( v, " AET : H , ;" );
						i = o = 0;
						while( ( text_getword( &s3, k ) ) != null )
							{
							if ( i == 0 ) // Lecture de l'heure de départ
								{
								if ( ( ! strcmp( s3, "A" ) )||( ! strcmp( s3, "ET" ) )
									||( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) ))
									continue;
								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 1", _ );
									goto planif_fin_error;
									}
								strdcpy( &s2, itona(atoi(s3),2), "_", _ );
								i = 1;
								continue;
								}
							if ( i == 1 ) // Lecture du séparateur des minutes
								{
								if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
									{
									i = 2;
									continue;
									}

								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
									if ( o )
										v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
									else
										v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
									strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
									if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
										o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
									i = 0;
									continue;
									}

								error_set( 1, "ERREUR: ", sid, " horaires incorrectes 2", _ );
								goto planif_fin_error;
								}

							if ( i == 2 )
								{
								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									strdcat( &s2, "00_00", _ );
									}
								else
								if ( ! is_numeric(s3) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 3", _ );
									goto planif_fin_error;
									}
								else
									strdcat( &s2, itona(atoi(s3),2), "_00", _ );
								v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
								if ( o )
									v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
								else
									v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
								strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, _ );
								if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
									o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );

								i = 0;
								continue;
								}
							}
						if ( i != 0 )
							{
							v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
							if ( o )
								v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
							else
								v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
							strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
							if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
								o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
							}
						if (o) break;
						}

					break;

				//------------------------------------------------------------------
				case 10 : // les jours fériés
				//------------------------------------------------------------------

					v = upper(trim(var_get( hConf1, "horaire" )));
					if (( !v )||( ! v[0] ))
						{
						error_set( 1, "ERREUR: ", sid, " aucun horaire n'a été donné.", _ );
						goto planif_fin_error;
						}

					// recherche sur le prochain jour férié

					n1 = h;
					while(1)
						{
						v3 = itod( n1, "jj/mm/aaaa" );
						n1 += 24*60*60;
						if( ! est_ferie( v3 ) ) continue;
						n = dtoi(v3, "jj/mm/aaaa");

						// lecture des horaires
						k = text_init( v, " AET : H , ;" );
						i = o = 0;
						while( ( text_getword( &s3, k ) ) != null )
							{
							if ( i == 0 ) // Lecture de l'heure de départ
								{
								if ( ( ! strcmp( s3, "A" ) )||( ! strcmp( s3, "ET" ) )
									||( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) ))
									continue;
								if ( ! is_numeric( s3 ) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 1", _ );
									goto planif_fin_error;
									}
								strdcpy( &s2, itona(atoi(s3),2), "_", _ );
								i = 1;
								continue;
								}
							if ( i == 1 ) // Lecture du séparateur des minutes
								{
								if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
									{
									i = 2;
									continue;
									}

								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
									if ( o )
										v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
									else
										v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
									strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
									if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
										o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
									i = 0;
									continue;
									}

								error_set( 1, "ERREUR: ", sid, " horaires incorrectes 2", _ );
								goto planif_fin_error;
								}

							if ( i == 2 )
								{
								if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
									{
									strdcat( &s2, "00_00", _ );
									}
								else
								if ( ! is_numeric(s3) )
									{
									error_set( 1, "ERREUR: ", sid, " horaires incorrectes 3", _ );
									goto planif_fin_error;
									}
								else
									strdcat( &s2, itona(atoi(s3),2), "_00", _ );
								v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
								if ( o )
									v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
								else
									v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
								strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, _ );
								if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
									o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );

								i = 0;
								continue;
								}
							}
						if ( i != 0 )
							{
							v3 = itod( h, "aaaa_mm_jj_hh_mm_ss" );
							if ( o )
								v1 = itod( o, "aaaa_mm_jj_hh_mm_ss" );
							else
								v1 = itod( n, "aaaa_mm_jj_hh_mm_ss" );
							strdcpy( &s2, strdncpy( &s4, v1, 11, _ ), s2, "00_00", _ );
							if ( ( strcmp( v3, s2 ) < 0 )&&( ( !o )||( strcmp( v1, s2 ) > 0 ) ) )
								o = dtoi( s2, "aaaa_mm_jj_hh_mm_ss" );
							}
						if ( o ) break;
						}

					break;
				}
			if ( o < 0 ) o = 0;
			if ( o )
				if ( ( !r )||( r > o ) ) r = o;
//print( "o=", itoa(o), ", r=", itoa(r), BR, _ );
			}

		if ( !r )
			{
			error_set( 1, "non planifiée", _ );
			goto planif_fin_error;
			}

		o = r;
tr("planif_compute100");

		for( id = 0 ; ts[id] != null ; id++ )
			{
			sid = itoa( id+1 );
			if ( hConf1 ) var_free( hConf1 );
			hConf1 = var_from_str( 0 , ts[id] );
			v = var_get( hConf1, "type" );
			if ( !v ) break;
			n = atoi( v );
			if ( n == 0 ) break;
			if ( n < 100 ) continue;

			nconf = upper(var_get( hConf1, "n" ));
			lconf = upper(var_get( hConf1, "horaire" ));

			b = false;

			switch( n )
				{
				//------------------------------------------------------------------
				case 101 : // sauf aux heures
				//------------------------------------------------------------------
					b = true;
					break;

				//------------------------------------------------------------------
				case 102 : // sauf les J de la semaine
				//------------------------------------------------------------------
					switch( wday( o ) )
						{
						case 0 : v3 = var_get( hConf1, "lundi" ); break;
						case 1 : v3 = var_get( hConf1, "mardi" ); break;
						case 2 : v3 = var_get( hConf1, "mercredi" ); break;
						case 3 : v3 = var_get( hConf1, "jeudi" ); break;
						case 4 : v3 = var_get( hConf1, "vendredi" ); break;
						case 5 : v3 = var_get( hConf1, "samedi" ); break;
						case 6 : v3 = var_get( hConf1, "dimanche" ); break;
						}
					if ( (v3)&&( ! strcmp(v3, "true") ) ) b = true;

					break;

				//------------------------------------------------------------------
				case 103 : // sauf les premiers J de la semaine du mois ou de l'année
				//------------------------------------------------------------------

					v1 = var_get( hConf1, "periode" );
					if ( !v1 )
						{
						error_set( 1, "ERREUR: ", sid, " La période (m ou a) n'a pas été donnée.", _ );
						goto planif_fin_error;
						}
					if ( day(o) <= 7 )
						{
						if ( ( strcmp( v1, "a" ) )||( month(o) == 1 ) )
							{
							switch( wday( o ) )
								{
								case 0 : v3 = var_get( hConf1, "lundi" ); break;
								case 1 : v3 = var_get( hConf1, "mardi" ); break;
								case 2 : v3 = var_get( hConf1, "mercredi" ); break;
								case 3 : v3 = var_get( hConf1, "jeudi" ); break;
								case 4 : v3 = var_get( hConf1, "vendredi" ); break;
								case 5 : v3 = var_get( hConf1, "samedi" ); break;
								case 6 : v3 = var_get( hConf1, "dimanche" ); break;
								}
							if ( (v3)&&( ! strcmp(v3, "true") ) ) b = true;
							}
						}

					break;

				//------------------------------------------------------------------
				case 104 : // sauf les derniers J de la semaine du mois ou de l'année
				//------------------------------------------------------------------

					v1 = var_get( hConf1, "periode" );
					if ( !v1 )
						{
						error_set( 1, "ERREUR: ", sid, " La période (m ou a) n'a pas été donnée.", _ );
						goto planif_fin_error;
						}
					if ( lday(o)-day(o) < 7 )
						{
						if ( ( strcmp( v1, "a" ) )||( month(o) == 12 ) )
							{
							switch( wday( o ) )
								{
								case 0 : v3 = var_get( hConf1, "lundi" ); break;
								case 1 : v3 = var_get( hConf1, "mardi" ); break;
								case 2 : v3 = var_get( hConf1, "mercredi" ); break;
								case 3 : v3 = var_get( hConf1, "jeudi" ); break;
								case 4 : v3 = var_get( hConf1, "vendredi" ); break;
								case 5 : v3 = var_get( hConf1, "samedi" ); break;
								case 6 : v3 = var_get( hConf1, "dimanche" ); break;
								}
							if ( (v3)&&( ! strcmp(v3, "true") ) ) b = true;
							}
						}

					break;

				//------------------------------------------------------------------
				case 105 : // sauf les jj du mois
				//------------------------------------------------------------------

					k = day(o);
					l = text_init( nconf, " , ;ET" );
					while( ( text_getword(&s3, l) ) != null )
						{
						if ( k == atoi(s3) ) { b = true; break; }
						}

					break;
				//------------------------------------------------------------------
				case 106 : // sauf les jj/mm
				//------------------------------------------------------------------

					v1 = itod( o, "jj/mm/aaaa" );
					v1[5] = 0;

					l = text_init( nconf, " , ;ET" );
					while( ( text_getword(&s3, l) ) != null )
						{
						if ( ( !strcmp( s3, "," ) )||( !strcmp( s3, ";" ) )||( !strcmp( s3, "ET" ) ) )
							continue;
						if ( ( p1 = strpos(s3, "/", 0) ) < 0 )
							{
							error_set( 1, "ERREUR: ", sid, " une des dates données n'est pas correcte.", _ );
							goto planif_fin_error;
							}
						s3[p1] = 0;
						strdcpy( &s3, itona(atoi(s3),2), "/", itona(atoi(&(s3[p1+1])),2), _ );
						if ( ! strcmp( s3, v1 ) ) { b = true; break; }
						}

					break;
				//------------------------------------------------------------------
				case 107 : // sauf les jj/mm/aaaa
				//------------------------------------------------------------------

					v1 = itod( o, "jj/mm/aaaa" );

					l = text_init( nconf, " , ;ET" );
					while( ( text_getword(&s3, l) ) != null )
						{
						if ( ( !strcmp( s3, "," ) )||( !strcmp( s3, ";" ) )||( !strcmp( s3, "ET" ) ) )
							continue;
						if ( strnicmp( s3, "ferie", -1 ) )
							{
							if ( ( p1 = strpos(s3, "/", 0) ) < 0 )
								{
								error_set( 1, "ERREUR: ", sid, " une des dates données n'est pas correcte.", _ );
								goto planif_fin_error;
								}
							if ( ( p2 = strpos(s3, "/", p1+1) ) < 0 )
								{
								error_set( 1, "ERREUR: ", sid, " une des dates données n'est pas correcte.", _ );
								goto planif_fin_error;
								}
							s3[p1] = 0;
							k = atoi( &(s3[p2+1]) );
							strdcpy( &s3, itona(atoi(s3),2), "/", itona(atoi(&(s3[p1+1])),2), "/",
										itoa((k<100)? k+2000 : k), _ );

							if ( ! strcmp( s3, v1 ) ) { b = true; break; }
							}
						else
							if ( est_ferie( v1 ) ) { b = true; break; }
						}

					break;

				//------------------------------------------------------------------
				case 108 : // sauf les jours fériés
				//------------------------------------------------------------------

					v1 = itod( o, "jj/mm/aaaa" );
					if ( est_ferie( v1 ) ) { b = true; break; }

					break;
				}

			if ( !b ) continue;

			if ( ( lconf == null )||( ! lconf[0] ) )
				{
				if ( n == 8 )
					{
					error_set( 1, "ERREUR: ", sid, " la plage horaire doit etre donnee", _ );
					goto planif_fin_error;
					}
				o = 0;
				break;
				}

			k = text_init( lconf, "DE AET : H , ;" );
			i = 0;
			while( ( text_getword( &s3, k ) ) != null )
				{
				if ( i == 0 ) // Lecture de l'heure de départ
					{
					if ( ( strcmp( s3, "DE" ) )&&( strcmp( s3, "ET" ) )&&( strcmp( s3, "," ) )&&( strcmp( s3, ";" ) ) )
						{
						if ( ! is_numeric( s3 ) )
							{
							error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 1", _ );
							goto planif_fin_error;
							}
						strdcpy( &s1, itona(atoi(s3),2), "_", _ );
						i = 1;
						}
					continue;
					}
				if ( i == 1 ) // Lecture séparateur minutes de départ
					{
					if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
						{
						i = 2;
						continue;
						}

					if ( ! strcmp( s3, "A" ) )
						{
						i = 4;
						strdcat( &s1, "00", _ );
						continue;
						}

					error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 2", _ );
					goto planif_fin_error;
					}
				if ( i == 2 ) // Lecture minutes de départ
					{
					if ( ! strcmp( s3, "A" ) )
						{
						i = 4;
						strdcat( &s1, "00", _ );
						continue;
						}

					if ( ! is_numeric( s3 ) )
						{
						error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 3", _ );
						goto planif_fin_error;
						}
					strdcat( &s1, itona(atoi(s3),2), _ );
					i = 3;
					continue;
					}
				if ( i == 3 ) // Lecture du A
					{
					if ( ! strcmp( s3, "A" ) )
						{
						i = 4;
						continue;
						}
					else
						{
						error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 4", _ );
						goto planif_fin_error;
						}
					}
				if ( i == 4 ) // Lecture heure de fin
					{
					if ( ! is_numeric( s3 ) )
						{
						error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 5", _ );
						goto planif_fin_error;
						}
					strdcpy( &s2, itona(atoi(s3),2), "_", _ );
					i = 5;
					continue;
					}
				if ( i == 5 ) // Lecture séparateur minutes de départ
					{
					if ( ( ! strcmp( s3, ":" ) )||( ! strcmp( s3, "H" ) ) )
						{
						i = 6;
						continue;
						}

					if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
						{
						strdcat( &s2, "00", _ );
						if ( verif_date2_hh_mm( o, s1, s2 ) == true )
							{
							strdcpy(&s4, itod(o,"aaaa_mm_jj" ), "_", s2, "_01", _ );
							r = dtoi( s4, "aaaa_mm_jj_hh_mm_ss" );
							b1 = false;
							o = 0;
							break;
							}
						i = 0;
						continue;
						}

					error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 6", _ );
					goto planif_fin_error;
					}
				if ( i == 6 ) // Lecture minutes fin
					{
					if (( ! strcmp( s3, "," ) )||( ! strcmp( s3, ";" ) )||( ! strcmp( s3, "ET" ) ))
						{
						strdcat( &s2, "00", _ );
						if ( verif_date2_hh_mm( o, s1, s2 ) == true )
							{
							strdcpy(&s4, itod(o,"aaaa_mm_jj" ), "_", s2, "_01", _ );
							r = dtoi( s4, "aaaa_mm_jj_hh_mm_ss" );
							b1 = false;
							o = 0;
							break;
							}
						i = 0;
						continue;
						}
					if ( ! is_numeric( s3 ) )
						{
						error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 7", _ );
						goto planif_fin_error;
						}
					strdcat( &s2, itona(atoi(s3),2), _ );
					if ( verif_date2_hh_mm( o, s1, s2 ) == true )
						{
						strdcpy(&s4, itod(o,"aaaa_mm_jj" ), "_", s2, "_01", _ );
						r = dtoi( s4, "aaaa_mm_jj_hh_mm_ss" );
						b1 = false;
						o = 0;
						break;
						}
					i = 0;
					continue;
					}
				}

			if (( o )&&( i ))
				{
				if ( i < 5 )
					{
					error_set( 1, "ERREUR: ", sid, " La tranche horaire est incorrecte 8", _ );
					goto planif_fin_error;
					}
				strdcat( &s2, "00", _ );
				if ( verif_date2_hh_mm( o, s1, s2 ) == true )
					{
					strdcpy(&s4, itod(o,"aaaa_mm_jj" ), "_", s2, "_01", _ );
					r = dtoi( s4, "aaaa_mm_jj_hh_mm_ss" );
					b1 = false;
					o = 0;
					}
				}

			if ( !o ) break;
			}

		if ( o ) break;

		h = r;
		if ( h < tm )
			{
			error_set( 1, "???", _ );
			goto planif_fin_error;
			}

		r = 0;

//		if ( nn > 5000 )
		if ( nn > 50 )
			{
			error_set( 1, "???", _ );
			goto planif_fin_error;
			}
		nn++;
		}

	if ( s ) free( s );
	if ( s1 ) free( s1 );
	if ( s2 ) free( s2 );
	if ( s3 ) free( s3 );
	if ( s4 ) free( s4 );

	return r;


	planif_fin_error :

	if ( s ) free( s );
	if ( s1 ) free( s1 );
	if ( s2 ) free( s2 );
	if ( s3 ) free( s3 );
	if ( s4 ) free( s4 );

	return ERROR;
	}

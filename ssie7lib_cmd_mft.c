//#include "ssie7lib.h"

// cmd_send

#define Action				v7
//#define bRecv				p6

#define cat_fname			fname3
#define log_fname			fname4
#define tran_id				(t_work->u1)
#define src_fname			(t_work->u3)


extern FILE *fpd;

int cmd_mft_stat( T_WORK * t_work );
int mftcat_purge( T_WORK * t_work );

//------------------------------------------------------------------------------
int cmd_mft( T_WORK * t_work )

// COMMANDE MFT
//------------------------------------------------------------------------------
	{
	// TRAITEMENTS DU CLIENT SEND
	// Syntaxe : send [partname] [fname] [binary/ascii] [parm1=val1] [...]
	// Syntaxe : send [status|cancel|log|execute] [IDA]

tr( "cmd_mft1" );

	v = var_getn( hReq, 1, &Action );
	if ( !v )
		{
		if ( !log_id )
			print( "ERREUR : syntaxe : mft {send|log|status|cat|setx|setw|retry|cancel|delete|recv} ...", _ );
		else
			hlogu_status( hLog, log_id, "ERREUR : syntaxe : {send|log|status|info|list|retry|cancel|delete|recv} ...", _ );
		
		return ERROR;
		}

	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "cat" ) )
	//--------------------------------------------------------------------------
		{
		return cmd_mft_cat( t_work );
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "rlogw" ) )
	//--------------------------------------------------------------------------
		{
		if ( ! ( v = var_get( hReq, "ida" ) ) )
			{
			if ( !log_id )
				print( "ERREUR : syntaxe : mft rlogw ida=[ida]", _ );
			else
				hlogu_status( hLog, log_id, "ERREUR : syntaxe : mft rlogw ida=[ida]", _ );
			return ERROR;
			}
		
		strdcpy( &ida, upper(v), _ );

		var_free( h );

		if ( (h = mftcat_load( ida )) == ERROR )
			{
			print( "ERREUR : Transfert inexistant", _ );
			return ERROR;
			}
		
		strdcpy( &fname, dirlog, DIR_CHAR "reveil_", ida, ".log", _ );
		file_load( &s, fname );
		mftcat_logw( h, s, _ );
		mftcat_upd( h, null, null, null );
		return OK;
		}

	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "log" ) )
	//--------------------------------------------------------------------------
		{
		if ( ! ( v = var_get( hReq, "ida" ) ) )
			{
			if ( !log_id )
				print( "ERREUR : syntaxe : mft log ida=[ida]", _ );
			else
				hlogu_status( hLog, log_id, "ERREUR : syntaxe : mft log ida=[ida]", _ );
			return ERROR;
			}
		
		strdcpy( &ida, v, _ );

		var_free( h );

		if ( (h = mftcat_load( ida )) == ERROR )
			{
			print( "ERREUR : Transfert inexistant", _ );
			return ERROR;
			}

		if ( !var_get( hReq, "debug" ) )
			{
			for( ; ; )
				{
				b = false;
				for( i = 0 ; v = var_getn( h, i, &vn ) ; i++ )
					{
					if ( vn[0] != '_' ) continue;
					var_delete( h, vn );
					b = true;
					break;
					}
				if ( !b ) break;
				}
			var_delete( h, "day_upd" );
			}
		
		var_sprint( h, BR, &s1 );

		mftcat_logr( ida, &s );
		print( s1, BR BR BR, s, BR, _ );

		return OK;
		}

	
	//--------------------------------------------------------------------------
	
	#ifdef OS_UNIX
	if (( request_mode == MODE_TCP )&&( strcmp( ip_cli, "127.0.0.1" ) ))
		{
		print( "ERREUR : commande non autorisée" BR, _ );
		return ERROR;
		}
	#endif
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "send_part" ) )
	//--------------------------------------------------------------------------
		{
tr( "cmd_mft10" );
		r = cmd_mft_send_part( t_work );
tr( "cmd_mft11" );
		return r;
		}
	
/*
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "stat" ) )
	//--------------------------------------------------------------------------
		{
		return cmd_mft_stat( t_work );
		}
*/	
tr( "cmd_mft2" );
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "send" ) )
	//--------------------------------------------------------------------------
		{
		log_id = 0;
		return cmd_mft_send( t_work );
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "recv" ) )
	//--------------------------------------------------------------------------
		{
		return cmd_mft_recv( t_work );
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "status" ) )
	//--------------------------------------------------------------------------
		{
		return cmd_mft_status( t_work );
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "cancel" ) )
	//--------------------------------------------------------------------------
		{
		return cmd_mft_cancel( t_work );
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "retry" ) )
	//--------------------------------------------------------------------------
		{
		return cmd_mft_retry( t_work );
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "setx" ) )
	//--------------------------------------------------------------------------
		{
		return cmd_mft_setx( t_work );
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "setw" ) )
	//--------------------------------------------------------------------------
		{
		return cmd_mft_setw( t_work );
		}
	
tr( "cmd_mft3" );
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "set" ) )
	//--------------------------------------------------------------------------
		{
tr( "cmd_mft4" );
		return cmd_mft_set( t_work );
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "delete" ) )
	//--------------------------------------------------------------------------
		{
		return cmd_mft_delete( t_work );
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "purge" ) )
	//--------------------------------------------------------------------------
		{
		return mftcat_purge( t_work );
		}
	
	//--------------------------------------------------------------------------
	if ( !stricmp( Action, "repair" ) )
	//--------------------------------------------------------------------------
		{
		v = var_get( hReq, "ida" );
		if (( !v )||( strlen( v ) != 8 )) return ERROR;
		return mftcat_repair( v );
		}
	
	
	
	
	print( "ERREUR : commande mft inconnue." BR, _ );
	}


/*
//------------------------------------------------------------------------------
int cmd_mft_stat( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	long NbTrf[25];
	long long nTotalSize[25];
	
	if ( var_get( hReq, "compute" ) )
		{
		// STATISTIQUES JOURNALIERES
		// UN FICHIER PAR JOUR : AU FORMAT JAVASCRIPT ARRAY
			// [ 'date', 'WeekDay', NbTrf, TotalFileSize, NbTrf0, ... NbTrf23, TotalSize0, ... ]
		
		strdcpy( &fname, dirdat, DIR_CHAR "mft_stat", _ );
		mkdir( fname );
		
		for( n = 1 ; n <= 30 ; n++ )
			{
			p = time(NULL) - ( n * (24 Heures) );
			strdcpy( &fname, dirdat, DIR_CHAR "mft_stat" DIR_CHAR, v5 = itod( p, "aaaa_mm_jj" ), _ );
			strdcpy( &s3, v5, _ );
			if ( file_exists( fname ) ) continue;
			strdcpy( &s4, "['", s3, "',", itoa(wday( p )), _ );
			for( i = 0 ; i < 25 ; i++ )
				{ NbTrf[i] = 0; nTotalSize[i] = 0; }
			
			strdcpy( &fname1, dirsend, DIR_CHAR "*.info", _ );
			tf= NULL;
			while( tf = file_list( tf, fname1, "absdir nodir" ) )
				{
				strdcpy( &fname1, tf->dir, DIR_CHAR, tf->name, _ );
				v = itod( n1 = file_date( fname1 ), "aaaa_mm_jj_hh_mm_ss" );
				v[10] = 0;
				if ( strcmp( s3, v ) ) continue;
				NbTrf[0]++;
				n2 = atoi( &(v[11]) ) + 1;
				NbTrf[n2]++;
				
				var_free( h );
				h = var_load( fname1 );
				n3 = atoi(var_get( h, "file_size" ));
				
				nTotalSize[0] += n3;
				nTotalSize[n2] += n3;
				}
			
			strdcpy( &fname1, dirrecv, DIR_CHAR "*.info", _ );
			tf= NULL;
			while( tf = file_list( tf, fname1, "absdir nodir" ) )
				{
				strdcpy( &fname1, tf->dir, DIR_CHAR, tf->name, _ );
				v = itod( n1 = file_date( fname1 ), "aaaa_mm_jj_hh_mm_ss" );
				v[10] = 0;
				if ( strcmp( s3, v ) ) continue;
				NbTrf[0]++;
				n2 = atoi( &(v[11]) ) + 1;
				NbTrf[n2]++;
				
				var_free( h );
				h = var_load( fname1 );
				n3 = atoi(var_get( h, "file_size" ));
				
				nTotalSize[0] += n3;
				nTotalSize[n2] += n3;
				}
			
			for( i = 0 ; i < 25 ; i++ )
				{
				strdcat( &s4, ",", itoa( NbTrf[i] ), ",", itoa( nTotalSize[i] ), _ );
				}
			strdcat( &s4, "],", _ );
			file_save_str( fname, s4, -1 );
			}
		
		return OK;
		}

	#ifdef OS_UNIX
	strdcpy( &cmd, "cat ", dirdat, DIR_CHAR "mft_stat" DIR_CHAR "*", _ );
	print( System( &s, cmd ), _ );
	#else
	strdcpy( &fname, dirdat, DIR_CHAR "mft_stat" DIR_CHAR "*", _ );
	tf= NULL;
	while( tf = file_list( tf, fname, "absdir nodir" ) )
		{
		strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
		file_load( &s, fname );
		print( s, _ );
		}
	
	#endif
	}
*/


#undef Action
#undef bRecv

#undef cat_fname
#undef log_fname
#undef tran_id
#undef src_fname


//------------------------------------------------------------------------------
int mft_save( T_WORK * t_work, handle hMft, char *direc, char *p_part, char *fcat )
//------------------------------------------------------------------------------
	{
	// A SUPPRIMER LORS DE LA MISE EN ROUTE DE SSIE v7.1.H
	}




#define Action				v7
//#define bAscii				b1
#define bGateway			b2
#define bPsurv				b3
#define cat_fname			fname3
#define log_fname			fname4
#define tran_id				(t_work->u1)
#define src_fname			(t_work->u3)


//------------------------------------------------------------------------------
int cmd_mft_status( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	//----------------------------------------------------------------------
	// STATUS : AFFICHE LE STATUS DU TRANSFERT
	//----------------------------------------------------------------------
	
	if ( ! ( strdcpy( &ida, var_get( hReq, "ida" ), _ ) ) )
		{
		print( "ERREUR : syntaxe : mft status ida=[ida]", _ );
		return ERROR;
		}
	
	var_free( h );
//	if ( (h = mftcat_load( ida )) == ERROR )
	if (( (h = mftcat_load( ida )) == ERROR )
			||( !var_get( h, "status" ) ))
		{
		print( "ERREUR : Transfert inexistant", _ );
		return ERROR;
		}
	
	print( var_get( h, "status" ), " ", var_get( h, "errno" ), " ", var_get( h, "errmsg" ), _ );	
	return OK;
	}



//------------------------------------------------------------------------------
int cmd_mft_cancel( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	if ( ( !var_get( hReq, "ida" ) )
		&&( !var_get( hReq, "part" ) )
		&&( !var_get( hReq, "spart" ) )
		&&( !var_get( hReq, "rpart" ) )
		&&( !var_get( hReq, "idt" ) )
		)
		{
		print( "ERREUR : syntaxe : mft cancel [ida={ida}] [{param}={value}]", _ );
		return ERROR;
		}
	
	// RECHERCHE DES TRANSFERTS
	
	p = stripos( cmd, "cancel", 0 );
	strdcpy( &cmd1, "mftcat ", &(cmd[p+6]), " status=CDH of=ssie", _ );
	var_free( hReq );
	hReq = var_from_cmd( 0, cmd1 );
	cmd_mft_cat( t_work );
	
	if ( s4[1] == 0 )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		return ERROR;
		}
	
	log_id = 0;
	for( p = 0, l = strlen(s4), n = 0 ; p < l ; p += 8 )
		{
		strdncpy( &ida, &(s4[p]), 8, _ );
		
		// VERIFIE QUE LE TRANSFERT EXISTE VRAIMENT
		
		var_free( h );
		if (( (h = mftcat_load( ida )) == ERROR )
				||( !var_get( h, "status" ) ))
			{
			print( "ERREUR : Transfert inexistant : ", ida, BR, _ );
			continue;
			}
		
		v3 = var_get( h, "status" );
		if ( !v3 ) continue;
		if ( ( v3[0] != 'C' )&&( v3[0] != 'D' )&&( v3[0] != 'H' ) )
			{
			mftcat_repair( ida );
			continue;
			}
			
		// ANNULATION

		mftcat_logw( h, "TRANSFERT ANNULE", _ );
		log_id		= atoi(var_get( h, "_log_id" ));
		if ( log_id )
			{
			hLog = hlog_init( var_get( h, "_hlog_dir" ), var_get( h, "_hlog_id" ) );
			if ( hLog != ERROR )
				{
				hlogc( hLog, log_id, "", "TRANSFERT ANNULE", _ );
				hlogu_status( hLog, log_id, "ANNULE", _ );
				hlog_free( hLog );
				}
			}
		
		if ( atoi( var_get( h, "errno" ) ) ) v = null;
		else v = "200";
		v1 = var_get( h, "errmsg" );
		strdcpy( &s, "ANNULE", (char*)(((v1)&&(v1[0])) ? ":" : ""), v1, _ );

		// SI TRANSFERT VIA LA PASSERELLE => ANNULATION SUR LA PASSERELLE AUSSI
		
		if (
			(( v1 = var_get( h, "mft_gateway" ) )&&( v1[0] )
				&&( var_get( h, "tran_id" ) )
				&&( v2 = var_get( h, "direct" ) )
				&&( ! stricmp( v2, "send" ) )
				)
			||( v3[0] == 'D' )
			)
			{
			var_set( h, "_transfert_canceled", "1" );
			mftcat_upd( h, null, v, s );
			}
		else
			mftcat_upd( h, "K", v, s );
		
		
		// AFFICHAGE POUR LE DEMANDEUR
		
		print( ida, " ", var_get( h, "part" ), " ", var_get( h, "fname" ), " : annulé." BR, _ );
		n++;
		}

	if ( n == 0 )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		return ERROR;
		}
	
	print( BR, itoa(n), " transfert", (char*)((n>1)?"s":""), " annulé", (char*)((n>1)?"s":""), _ );
	
	return OK;
	}


//------------------------------------------------------------------------------
int cmd_mft_retry( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	if ( ( !var_get( hReq, "ida" ) )
		&&( !var_get( hReq, "part" ) )
		&&( !var_get( hReq, "spart" ) )
		&&( !var_get( hReq, "rpart" ) )
		&&( !var_get( hReq, "idt" ) )
		)
		{
		print( "ERREUR : syntaxe : mft retry [ida={ida}] [{param}={value}]", _ );
		return ERROR;
		}
	
	// RECHERCHE DES TRANSFERTS
	
	p = stripos( cmd, "retry", 0 );
	
	if ( ! mft_exists() )
		strdcpy( &cmd1, "mftcat ", &(cmd[p+6]), ", status=K, direct=send, of=ssie", _ );
	else
		strdcpy( &cmd1, "mftcat ", &(cmd[p+6]), ", status=K, of=ssie", _ );
		
	var_free( hReq );
	hReq = var_from_cmd( 0, cmd1 );
	cmd_mft_cat( t_work );
	
	if ( s4[1] == 0 )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		return ERROR;
		}
	
	log_id = 0;
	for( p = 0, l = strlen(s4), n = 0 ; p < l ; p += 8 )
		{
		strdncpy( &ida, &(s4[p]), 8, _ );
		
		// CHARGE LE TRANSFERT
		
		var_free( h );
//		if ( (h = mftcat_load( ida )) == ERROR )
		if (( (h = mftcat_load( ida )) == ERROR )
				||( !var_get( h, "status" ) ))
			{
			print( "ERREUR : Transfert inexistant : ", ida, BR, _ );
			continue;
			}
		
		v = var_get( h, "status" );
		if ( !v ) continue;
		if ( v[0] != 'K' )		continue;
			
		if ( mft_exists() )
			{
			v = var_get( h, "direct" );
			if ( !v ) continue;
			if ( !stricmp( v, "RECV" ) )
				{
				if ( ! var_get( h, "mft_gateway" ) )
					continue;
				if ( ! var_get( h, "_mft_send_etape_no" ) ) // On ne peut reprendre que si on est en phase d'envoi MFT
					continue;
				}
			}
		
		// SUPPRIME UNE EVENTUELLE ANCIENNE DEMANDE D'ANNULATION
		
		var_set( h, "_transfert_canceled", "0" );
		
			
		// LOG L'ACTION
		
		mftcat_logw( h, "TRANSFERT RETRY", _ );
		if ( log_id ) hlog_free( hLog );
		if ( log_id	= atoi(var_get( h, "_log_id" )) )
			{
			hLog = hlog_init( var_get( h, "_hlog_dir" ), var_get( h, "_hlog_id" ) );
			hlogc( hLog, log_id, "", "TRANSFERT RETRY", _ );
			hlogu_status( hLog, log_id, "En cours", _ );
			}
		
		
		// CHANGE LE STATUS
		
		v = var_get( h, "direct" );
/*
		if ( !stricmp( v, "RECV" ) )
			partname = "mft";
		else
			partname = var_get( h, "part" );
*/			
		if ( var_get( h, "_transfert_en_attente_passerelle" ) )
			var_set( h, "_transfert_en_attente_passerelle", "0" );

		if ( var_get( h, "_mft_send_etape_no" ) )
			var_set( h, "_mft_send_etape_no", "0" );
			
		var_set( h, "_dt_ref_timeout", itoa(sysdate) );

//mftcat_logw( h, "TRANSFERT RETRY : " BR, var_to_str( h, &s ), _ );

		
		// PLACE LE TRANSFERT DANS LA LISTE D'ATTENTE POUR LE PARTENAIRE

		if (( mft_exists() )&&( var_get( h, "mft_gateway" ) ))
			{
			mftcat_logw( h, "Replace le transfert dans la file d'attente MFT", _ );
			var_set( h, "faction", "delete" ); // Demande suppression fichier après transfert
			
			// PLACE LE TRANSFERT DANS LA FILE D'ATTENTE DES ENVOIS MFT
			strdcpy( &fname2, dirwrk, DIR_CHAR "wsend" DIR_CHAR "mft" DIR_CHAR,
					var_get( h, "ida" ), _ );
			file_save_str( fname2, "", 0 );
			if ( !file_exists( fname2 ) )
				{
				logw( "mft_server_recv : Pb création fichier : ", fname2, _ );
				mftcat_logw( h, "Pb mise dans file d'attente MFT", _ );
				mftcat_upd( h, "K", "120", "ERREUR:Pb mise dans file d'attente MFT" );
				}
			else
				mftcat_upd( h, "D", "0", "En cours : MFT" );
			}
		else
			mftcat_upd( h, "C", "0", "En cours" );
		
		
		// AFFICHAGE POUR LE DEMANDEUR
			
		print( ida, " ", var_get( h, "part" ), " ", var_get( h, "fname" ), " : transfert relancé" BR, _ );
		n++;
		}
	if ( log_id ) hlog_free( hLog );
	
	if ( !n )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		return ERROR;
		}
	
	print( BR, itoa(n), " transfert", (char*)((n>1)?"s":""), " relancé", (char*)((n>1)?"s":""), _ );
	
	return OK;
	}


//------------------------------------------------------------------------------
int cmd_mft_delete( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	if ( ( !var_get( hReq, "ida" ) )
		&&( !var_get( hReq, "part" ) )
		&&( !var_get( hReq, "spart" ) )
		&&( !var_get( hReq, "rpart" ) )
		&&( !var_get( hReq, "direct" ) )
		&&( !var_get( hReq, "idt" ) )
		)
		{
		print( "ERREUR : syntaxe : mft delete [ida={ida}] [{param}={value}]", _ );
		return ERROR;
		}
	
	// RECHERCHE DES TRANSFERTS
	
	p = stripos( cmd, "delete", 0 );
	strdcpy( &cmd1, "mftcat ", &(cmd[p+6]), " status=TFKXH of=ssie", _ );
	var_free( hReq );
	hReq = var_from_cmd( 0, cmd1 );
	cmd_mft_cat( t_work );
	
	if ( s4[1] == 0 )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		return ERROR;
		}
	
	for( p = 0, l = strlen(s4), n = 0 ; p < l ; p += 8 )
		{
		strdncpy( &ida, &(s4[p]), 8, _ );

		// VERIFIE QUE LE TRANSFERT EXISTE VRAIMENT
		
		var_free( h );
//		if ( (h = mftcat_load( ida )) == ERROR )
		if (( (h = mftcat_load( ida )) == ERROR )
				||( !var_get( h, "status" ) ))
			{
			print( "ERREUR : Transfert inexistant : ", ida, BR, _ );
			continue;
			}
		
		
		v = var_get( h, "status" );
		if ( !v ) continue;
		if ( ( v[0] != 'T' )&&( v[0] != 'F' )&&( v[0] != 'K' )&&( v[0] != 'X' )&&( v[0] != 'H' ) )
			{
			mftcat_repair( ida );
			continue;
			}
			
		mftcat_del( ida );
		
		
		// AFFICHAGE POUR LE DEMANDEUR
		
		print( ida, " ", var_get( h, "part" ), " ", var_get( h, "fname" ), " : supprimé." BR, _ );
		n++;
		}
	
	if ( !n )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		return ERROR;
		}
	
	print( BR, itoa(n), " transfert", (char*)((n>1)?"s":""), " supprimé", (char*)((n>1)?"s":""), _ );
	
	return OK;
	}



//------------------------------------------------------------------------------
int cmd_mft_setx( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	// PASSE LE STATUS A X
	
	if ( ( !var_get( hReq, "ida" ) )
		&&( !var_get( hReq, "part" ) )
		&&( !var_get( hReq, "spart" ) )
		&&( !var_get( hReq, "rpart" ) )
		&&( !var_get( hReq, "idt" ) )
		)
		{
		print( "ERREUR : syntaxe : mft setx [ida={ida}] [{param}={value}]", _ );
		return ERROR;
		}
	
		
	// RECHERCHE DES TRANSFERTS
	
	p = stripos( cmd, "setx", 0 );
	strdcpy( &cmd1, "mftcat ", &(cmd[p+4]), " status=TRF of=ssie", _ );
	var_free( hReq );
	hReq = var_from_cmd( 0, cmd1 );
	cmd_mft_cat( t_work );
	
	if ( s4[1] == 0 )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		return ERROR;
		}
	
	for( p = 0, l = strlen(s4), n = 0 ; p < l ; p += 8 )
		{
		strdncpy( &ida, &(s4[p]), 8, _ );

		// VERIFIE QUE LE TRANSFERT EXISTE VRAIMENT
		
		var_free( h );
//		if ( (h = mftcat_load( ida )) == ERROR )
		if (( (h = mftcat_load( ida )) == ERROR )
				||( !var_get( h, "status" ) ))
			{
			print( "ERREUR : Transfert inexistant : ", ida, BR, _ );
			continue;
			}
			
		v = var_get( h, "status" );
		if ( !v ) continue;
		if (( v[0] != 'T' )&&( v[0] != 'R' )&&( v[0] != 'F' ))		continue;
			
		// PASSE LE STATUS A X
		
		mftcat_upd( h, "X", null, null );
		
		
		// AFFICHAGE POUR LE DEMANDEUR
		
		print( ida, " ", var_get( h, "part" ), " ", var_get( h, "fname" ), " : X." BR, _ );
		n++;
		}
	
	if ( !n )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		return ERROR;
		}
	
	print( BR, itoa(n), " transfert", (char*)((n>1)?"s":""), " traité", (char*)((n>1)?"s":""), _ );
	
	return OK;
	}



//------------------------------------------------------------------------------
int cmd_mft_setw( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	// PASSE LE STATUS A W ==> Pour réexécuter un éventuel réveil déclenché par le robot wrecv
	
	if ( ( !var_get( hReq, "ida" ) )
		&&( !var_get( hReq, "part" ) )
		&&( !var_get( hReq, "spart" ) )
		&&( !var_get( hReq, "rpart" ) )
		&&( !var_get( hReq, "idt" ) )
		&&( !var_get( hReq, "status" ) )
		)
		{
		print( "ERREUR : syntaxe : mft setw [ida={ida}] [{param}={value}]", _ );
		return ERROR;
		}
	
		
	// RECHERCHE DES TRANSFERTS A L'ETAT T
	
	p = stripos( cmd, "setw", 0 );
	strdcpy( &cmd1, "mftcat ", &(cmd[p+4]), " status=TRF of=ssie", _ );
	var_free( hReq );
	hReq = var_from_cmd( 0, cmd1 );
	cmd_mft_cat( t_work );

	if ( s4[1] == 0 )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		return ERROR;
		}
	
	for( p = 0, l = strlen(s4), n = 0 ; p < l ; p += 8 )
		{
		strdncpy( &ida, &(s4[p]), 8, _ );

		// VERIFIE QUE LE TRANSFERT EXISTE VRAIMENT
		
		var_free( h );
//		if ( (h = mftcat_load( ida )) == ERROR )
		if (( (h = mftcat_load( ida )) == ERROR )
				||( !var_get( h, "status" ) ))
			{
			print( "ERREUR : Transfert inexistant : ", ida, BR, _ );
			continue;
			}
			
		v = var_get( h, "status" );
		if ( !v ) continue;
		if (( v[0] != 'T' )&&( v[0] != 'R' )&&( v[0] != 'F' ))		continue;
			
		// PASSE LE STATUS A W
		
		mftcat_upd( h, "W", null, null );
		
		
		// AFFICHAGE POUR LE DEMANDEUR
		
		print( ida, " ", var_get( h, "part" ), " ", var_get( h, "fname" ), " : W." BR, _ );
		n++;
		}
	
	if ( !n )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		return ERROR;
		}
	
	print( BR, itoa(n), " transfert", (char*)((n>1)?"s":""), " traité", (char*)((n>1)?"s":""), _ );
	
	return OK;
	}



//------------------------------------------------------------------------------
int cmd_mft_set( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	// PASSE LE STATUS A ???

tr( "cmd_mft_set1" );
	
	v1 = var_getn( hReq, 2, &vn );
	if ( ( !v1 )||( !( v = var_get( hReq, "ida" ) ) ) )
		{
		print( "ERREUR : syntaxe : mft set {status} ida={ida}", _ );
		return ERROR;
		}

	if ( strmcmp( vn, "W|T|F|X|K" ) == false )
		{
		print( "ERREUR : valeur du status invalide", _ );
		return ERROR;
		}
		
	strdcpy( &u7, vn, _ );
	strdcpy( &ida, v, _ );
	
	// VERIFIE QUE LE TRANSFERT EXISTE VRAIMENT
	
tr( "cmd_mft_set2" );

	var_free( h );
	if (( (h = mftcat_load( ida )) == ERROR )
			||( !var_get( h, "status" ) ))
		{
		print( "ERREUR : Transfert inexistant : ", ida, BR, _ );
		return ERROR;
		}
	
tr( "cmd_mft_set3" );
		
	// PASSE LE STATUS A LA VALEUR DEMANDEE
	
	mftcat_upd( h, u7, null, null );

tr( "cmd_mft_set4" );

	print( ida, " ", var_get( h, "part" ), " ", var_get( h, "fname" ), " : ", u7, BR, _ );

	return OK;
	}



//------------------------------------------------------------------------------
int mftcat_purge( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	v = var_get( hSrvConf, "mft_log_delay" );
	n1 = 30;
	if ( v ) n1 = atoi( v );
	
	// Purge les fichiers index vieux de plus de 30 jours
	
	strdcpy( &fname, dirdat, DIR_CHAR "mftcat" DIR_CHAR "index" DIR_CHAR "*", _ );

	n = time(NULL) - (n1 Jours);
	tf = NULL;
	while( tf = file_list( tf, fname, "nodir recursif absdir" ) )
		{
		strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
		if ( file_date( fname ) < n )
			{
			print( "purge : ", fname, BR, _ );
			file_delete( fname );
			}
		}

	// Supprime les répertoires indexes vides
	
	strdcpy( &fname, dirdat, DIR_CHAR "mftcat" DIR_CHAR "index" DIR_CHAR "*", _ );

	n = time(NULL) - (30 Jours);
	tf = NULL;
	while( tf = file_list( tf, fname, "nofic recursif absdir" ) )
		{
		strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
		if ( file_date( fname ) < n )
			{
			print( "purge : ", fname, BR, _ );
			rmdir( fname );
			}
		}

	
	// Purge les fichiers de données vieux de plus de 30 jours
	
	strdcpy( &fname, dirdat, DIR_CHAR "mftcat" DIR_CHAR "info" DIR_CHAR "*", _ );
	tf = NULL;
	while( tf = file_list( tf, fname, "nodir recursif absdir" ) )
		{
		strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
		if ( file_date( fname ) < n )
			{
			print( "purge : ", fname, BR, _ );
			file_delete( fname );
			}
		}

	
	// Purge les répertoires vides
	strdcpy( &fname, dirdat, DIR_CHAR "mftcat" DIR_CHAR "info" DIR_CHAR "*", _ );
	tf = NULL;
	while( tf = file_list( tf, fname, "nofic recursif absdir" ) )
		{
		strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, _ );
		if ( file_date( fname ) < n )
			{
			print( "purge : ", fname, BR, _ );
			rmdir( fname );
			}
		}

	}


#undef Action
//#undef bAscii
#undef bGateway
#undef bPsurv
#undef cat_fname
#undef log_fname
#undef tran_id
#undef src_fname








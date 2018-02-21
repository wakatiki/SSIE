#define cat_fname			fname3
#define log_fname			fname4


//------------------------------------------------------------------------------
int cmd_tsend( T_WORK * t_work )
//------------------------------------------------------------------------------
	{
	if ( v = var_get( hReq, "ida" ) )
		{
		strdcpy( &ida, v, _ );
		strdncpy( &cat_fname, dirdat, -1, DIR_CHAR "mftcat" DIR_CHAR, -1,
					ida, 3,	DIR_CHAR, 1, &(ida[3]), 3, DIR_CHAR, 1,
					&(ida[6]), 2, DIR_CHAR, 1, ida, 8,
					_ );
		if ( !file_exists( cat_fname ) )
			{
			print( "ERREUR : Transfert inconnu" BR, _ );
			return ERROR;
			}
		var_free( h );
		h = var_load( cat_fname );
		var_set( h, "_mft_send_etape_no", "0" );		
		v = var_get( h, "fname" );
		strdcpy( &fname, v, ".copy", _ );
		file_copy( v, fname );
		var_set( h, "recep_fname", fname );
		mft_save2( t_work, h, "R", "0", "En cours", cat_fname );
		strdcpy( &fname, cat_fname, ".log", _ );
		file_delete( fname );
		}
	else
		{
		strdcpy( &fname, dirdat, DIR_CHAR "mftcat" DIR_CHAR "ida.id", _ );
		n = get_uid( fname );
		strdcpy( &ida, ito36( n, 8 ), _ );
		ida[0] = 'A' + year(sysdate) - 2013;
		if ( (n=month(sysdate)) < 10 )
			ida[1] = '0' + n;
		else
			ida[1] = 'A' + n - 10;
		ida[2] = ito36( day(sysdate), 1 )[0];
	
		print( "IDA=", ida, BR, _ );
		
		var_set( hReq, "ida", ida );
		var_set( hReq, "direct", "RECV" );

		strdncpy( &cat_fname, dirdat, -1, DIR_CHAR "mftcat" DIR_CHAR, -1,
					ida, 3,	DIR_CHAR, 1, &(ida[3]), 3, DIR_CHAR, 1,
					&(ida[6]), 2, DIR_CHAR, 1, ida, 8,
					_ );
					
		v = var_get( hReq, "fname" );
		strdcpy( &fname, v, ".copy", _ );
		file_copy( v, fname );
		var_set( hReq, "recep_fname", fname );
		mft_save2( t_work, hReq, "R", "0", "En cours", cat_fname );
		}

	strdcpy( &fname, dirwrk, DIR_CHAR "wsend" DIR_CHAR "mft" DIR_CHAR, ida, _ );
	file_save_str( fname, "", 0 );
	}

	
#undef cat_fname
#undef log_fname























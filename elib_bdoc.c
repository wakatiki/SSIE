
handle	bdoc_open( handle hCfg )
	{
	var_from_cmd( hCfg, str(
//		"dirdat=", dirdat, DIR_CHAR "mftcat" DIR_CHAR "info,"
//		"diridx=", dirdat, DIR_CHAR "mftcat" DIR_CHAR "index,"
//		"bd_lock_fname=", dirwrk, DIR_CHAR "bd_lock,"
		"key_name=bdoc_id,"
		"item_size=4096,"
		"vlfields='title,body',"
		
		"index_path='parent_id/status/day_upd',"
		"index_key='dt_upd[19],class[50],keywords[300],position[10],authors[300]',",
		_ ) );
	
	return bd_hopen( hCfg );
	}
	
	
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
	
//------------------------------------------------------------------------------------------

int		bdoc_save( handle hBD, char *user_id, handle hData )
	{
	v = var_get( hData, KeyName );
	if ( !v )
		{
		strdcpy( &fname, dirdat, DIR_CHAR "bdoc.id", _ );
		n = get_uid( fname );
		strdcpy( &sid, ito36( n, 8 ), _ );
		var_set( hData, KeyName, sid );
		if ( !var_get( hData, "dt_crea" ) )
			var_set( hData, "dt_crea", currdate("flog") );
		if ( !(v=var_get( hData, "dt_upd" )) )
			{
			var_set( hData, "dt_upd", currdate("flog") );
			var_set( hData, "day_upd", currdate("aaaa_mm_jj") );
			}
		else
			var_set( hData, "day_upd", itod(dtoi(v,"flog"), "aaaa_mm_jj") );
		
		return bd_insert( hBD, hData, null );
		}
	else
		{
		var_set( hData, "dt_upd", currdate("flog") );
		var_set( hData, "day_upd", currdate("aaaa_mm_jj") );
		return bd_update( hBD, hData, var_get( hData, KeyName ), 1 );
		}
	}
	
	
//------------------------------------------------------------------------------------------

int		bdoc_set_pos( handle hBD, char *user_id, char **id_list, int new_pos )
	{
	}

	
//------------------------------------------------------------------------------------------

int		bdoc_move( handle hBD, char *user_id, char *id_list, char *new_parent_id, int pos )
	{
	}
	
//------------------------------------------------------------------------------------------

int		bdoc_delete( handle hBD, char *user_id, char *id_list, uint id_nb )
	{
	}
	
//------------------------------------------------------------------------------------------

int		bdoc_select( handle hBD, char *user_id, char *Cond, char *OrderBy, char **id_list )
	{
	}
	
//------------------------------------------------------------------------------------------

int		bdoc_load( handle hBD, char *id_list, uint id_nb, char *of, char **p_out )
	{
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


















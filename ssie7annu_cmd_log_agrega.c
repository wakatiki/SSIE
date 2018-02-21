#include "ssie7lib.h"



//------------------------------------------------------------------------------
int cmd_log_agregate( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	strdcpy( &fname, "/appli/ssie7annu/data/log_agrega/", var_get( hReq, "direc" ),
				"-", ip_cli, "-", var_get( hReq, "spart" ), "-", var_get( hReq, "partname" ),
				"-", var_get( hReq, "id" ), "-*", _ );
	
	file_delete( fname );
	
	strdcpy( &fname, "/appli/ssie7annu/data/log_agrega/", var_get( hReq, "direc" ),
				"-", ip_cli, "-", var_get( hReq, "spart" ), "-", var_get( hReq, "partname" ),
				"-", var_get( hReq, "id" ), "-", var_get( hReq, "date" ), _ );
				
	var_delete( hReq, "log_agrega" );
	var_to_js_array( hReq, &s );
	file_save_str( fname, str("{", s, "},",_), -1 );
	system( str( "chmod 644 ", fname, _ ) );
	
	return OK;
	}


//------------------------------------------------------------------------------
int cmd_bd( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	v = var_getn( hReq, 2, &vn );
	if ( !v ) return OK;
	
	//--------------------------------------------------------------------------------------
	if ( !strcmp( vn, "create" ) ) 
	//--------------------------------------------------------------------------------------
		{
		}
		

	
	print( "ERREUR BD : Commande inconnue." BR, _ );
	return ERROR;
	}

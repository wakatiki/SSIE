//------------------------------------------------------------------------------
int cmd_annu_send( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	// syntaxe : annu_send {ip:port}

	v = var_getn( hReq, 1, &v5 );
	if ( !v )
		{
		print( "ERREUR : syntaxe : annu_send {ip:port}", _ );
		return ERROR;
		}

	tf = NULL;
	while( tf = file_list( tf, dirpart, "nodir" ) )
		{
		p = strpos( tf->name, ".", 0 );
		tf->name[p] = 0;
		strdcpy( &cmd, "part ", tf->name, " loadnat user=" NNI_ADM, _ );
		
		v = tcp_request( v5, cmd, &s );
		print( cmd, BR BR, s, BR BR BR, _ );
		}
	}

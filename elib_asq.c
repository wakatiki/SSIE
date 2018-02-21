
// ASQ = Asynchrone Services Queue
//------------------------------------------------------------------------------------------

int	asq_post( char *dirdat, char *cmd )
	{
	char *s = null;
	int uid;
	
	strdcpy( &s, dirdat, DIR_CHAR "asq.id", _ );
	uid = getuid( s );
	strdcpy( &s, dirdat, DIR_CHAR, itoa( uid ), _ );
	file_save_str( s, cmd, -1 );
	free( s );
	return uid;
	}

//------------------------------------------------------------------------------------------

int	asq_listner( char *dirdat, int(*requester)(char *) )
	{
	char *s = null, *cmd = null;
	T_FILE_INFO *tf = NULL;
	int n = 0;
	
	strdcpy( &s, dirdat, DIR_CHAR "*", _ );
	if ( !dir_exists( dirdat ) ) mkdir( dirdat );
	while( tf = file_list( tf, s, "nodir absdir" ) )
		{
		if ( !is_numeric( tf->name ) ) continue;
		strdcpy( &s, dirdat, DIR_CHAR, tf->name, _ );
		if ( file_load( &cmd, s ) == sERROR ) continue;
		file_delete( s );
		requester( cmd );
		n++;
		}
	
	return n;
	}















//------------------------------------------------------------------------------
void error_set( int err_no, ... )
//------------------------------------------------------------------------------
	{
	va_list args;
	char *arg;

	error_no = err_no;
	if ( error_msg ) error_msg[0] = 0;
	
	va_start(args, err_no); 
	while( 1 )
		{
		arg = va_arg(args, char *);
		if ( arg == _ ) return;
		if ( arg ) strdcat( &error_msg, arg, _ );
		}
	}
	

//------------------------------------------------------------------------------


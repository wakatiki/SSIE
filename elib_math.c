
#define base10 ((char*)bi)

//------------------------------------------------------------------------------				
handle	bigint_new( void )
//------------------------------------------------------------------------------				
	{
	char *bi = alloc0( 256 );
	
	bi[0] = '0';
	return (handle)bi;
	}
	
//------------------------------------------------------------------------------				
handle	bigint_add( handle bi, uint value )
//------------------------------------------------------------------------------				
	{
	char in[12], res[256];
	int i, j, n, k = 0, r = 0;
	
	sprintf( in, "%u", value );

	for( i =strlen(in)-1 , j = strlen( base10 ) - 1; i >= 0 ; i--, j--, k++ )
		{
		if ( j >= 0 )
			n = (base10[j]-'0') + (in[i]-'0') + r;
		else
			n = (in[i]-'0') + r;
		if ( n > 9 )
			{ res[k] = (n - 10)+'0'; r = 1; }
		else
			{ res[k] = n + '0'; r = 0; }
		}
	for(  ; j >= -1 ; j--, k++ )
		{
		if ( j >= 0 )
			n = (base10[j]-'0') + r;
		else
			if ( !r ) break;
			else n = r;
			
		if ( n > 9 )
			{ res[k] = (n - 10)+'0'; r = 1; }
		else
			{ res[k] = n + '0'; r = 0; }
		}
	for( j = 0 , k-- ; k >= 0 ; k--, j++ )
		{
		base10[j] = res[k];
		}		
	
	base10[j] = 0;	
	
	return bi;
	}
	
//------------------------------------------------------------------------------				
void	bigint_from10( handle bi, char *in )
//------------------------------------------------------------------------------				
	{
	strcpy( base10, in );
	}
	
//------------------------------------------------------------------------------				
char*	bigint_to10( handle bi )
//------------------------------------------------------------------------------				
	{
	return base10;
	}
	

//------------------------------------------------------------------------------				
int		rnd( int min, int max )
//------------------------------------------------------------------------------				
	{
	static int bInit = false;
	int n;
	
	BeginCriticalSection();
	if ( !bInit )
		{
		srand((unsigned int)time(NULL));
		bInit = true;
		}
	EndCriticalSection();
		
	for( ; ; )
		{
		n = rand();
		if ( ( n >= min )&&( n <= max ) ) return n;
		}
		
	}


//------------------------------------------------------------------------------				
char *	srnd( int nDigit )
//------------------------------------------------------------------------------				
	{
	char **out = memptr(nDigit+1),
		*s = "abcdefghijkmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_@=+.";
	int i = 0, n, l = strlen(s);
	
	for( ; i < nDigit ; i++ )
		{
		n = rnd( 0, 65535 );
		(*out)[i] = s[ (unsigned)(n%l) ];
		}
	(*out)[nDigit] = 0;
	return *out;
	}


//------------------------------------------------------------------------------				
char *	snrnd( int nDigit )
//------------------------------------------------------------------------------				
	{
	char **out = memptr(nDigit+1), *s = "0123456789";
	int i = 0, n, l = strlen(s);
	
	for( ; i < nDigit ; i++ )
		{
		n = rnd( 0, 65535 );
		(*out)[i] = s[ (unsigned)(n%l) ];
		}
	(*out)[nDigit] = 0;
	return *out;
	}











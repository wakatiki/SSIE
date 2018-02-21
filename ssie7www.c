#define MAIN
#include "elib.h"
#include "elib4.c"
//char *dirtmp = null;

#define SRV_IP		"10.121.0.70:3131"
//#define SRV_IP		"10.122.0.81:3131"

char* mersi3_putfile( char *ip_dest, char *fsrc, char *fdest, int bAscii );
char* mersi3_getfile( char *ip_srv, char *fsrc, char *fdest, int bAscii );

#define putfile	mersi3_putfile
#define getfile	mersi3_getfile


//------------------------------------------------------------------------------
int main(int ac, char *av[])
//------------------------------------------------------------------------------
	{
	print( "GENERATION DE L'INTRANET SSIE7" BR BR, _ );
	
	chdir( "C:\\E\\Src\\ssie7\\portail" );
	
	system( "C:\\E\\Bin\\Windows\\make_js_appl.exe ssie7" );

//	pause();
	}

	















#include "elib.h"

//------------------------------------------------------------------------------

char *s = null, *s1 = null, *s2 = null, *s3 = null,
		*v, *v1, *v2, *v3, *v4, *w = null, *cmd = null, **ts,
		*fname = null, *fname1 = null, *fname2 = null,
		*sTxtIn, *sTxtOut, *module_name;
		
int i, p, p1, p2, p3, p4, p5, n, n1, n2, h, nbAccolade = 0;
bool b, b1;

FILE *fp;
T_FILE_INFO *tf = null;

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
	{
	// Arg1 = nom de l'exé initiale
	// Arg2 = Fichier1 à ajouter
	// Arg3 = Fichier2 à ajouter
	// ...
	
	if ( argc < 4 )
		{
		print( "Syntaxe : create_autoextract <SetupDestName> <SetupInitName> <File1|Pattern> <File2|Pattern> ..." BR, pend );
		exit( 1 );
		}
	
	if ( ! file_exists( argv[2] ) )
		{
		print( "ERREUR : fichier inexistant ou inaccessible : ", argv[1], BR, pend );
		exit( 2 );
		}
	
	system( strdcpy( &cmd, "copy /Y ", argv[2], " ", argv[1], pend ) );
	
	n1 = file_size( argv[1] );
	if ( n1 == 0 )
		{
		print( "ERREUR : fichier setup est vide : ", argv[2], BR, pend );
		exit( 2 );
		}
		
	fp = fopen( argv[1], "ab" );
	if ( fp == NULL )
		{
		print( "ERREUR : Impossible d'ouvrir le fichier : ", argv[1], BR, pend );
		exit( 2 );
		}
	
	for( i = 3 ; i < argc ; i++ )
		{
		tf = NULL;
		while( ( tf = file_list( tf, argv[i], "nodir adir nohide" ) ) != NULL )
			{
			if ( ! file_load( &s, strdcpy( &fname, tf->dir, DIR_CHAR, tf->name, pend ) ) )
				{
				print( "ERREUR : lecture du fichier : ", fname, BR, pend );
				exit( 2 );
				}
			n = file_size( fname );
			fprint( fp, strpad( file_getname(fname), 256, " " ), itona( n, 10 ), pend );
			fwrite( fp, s, n, pend );
			print( fname, " (", itona(n,10), ")" BR, pend );
			}
		}
	
	fprint( fp, strpad( "AUTOEXTRACT_FIN", 256, " " ), itona( n1, 10 ), pend );
	fclose( fp );
	}































#include "ssie7lib.h"


extern char *ssl_pgm_fname;


// GENERATION D'UN CERTIFICAT SSL


#define csr			s3

//------------------------------------------------------------------------------
int cmd_get_ssl_certif( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	// gen_ssl_certif partname 'CSR' => retourne la config SSL standard ssie7
	
//	if ( send_private_port( t_work ) ) return OK;
	
		
	//--------------------------------------------------------------------------
	// LECTURE DU CSR
	//--------------------------------------------------------------------------
	
	v = var_getn( hReq, 1, &partname );
	if ( !v )
		{
		print( "ERREUR : syntaxe : gen_ssl_certif {partname} {csr}", _ );
		return ERROR;
		}
	
	v = var_getn( hReq, 2, &csr );
	if ( !v )
		{
		print( "ERREUR : syntaxe : gen_ssl_certif {partname} {csr}", _ );
		return ERROR;
		}
	
	logw( "get_ssl_certif ", partname, _ );
	

	// CREATION D'UN REPERTOIRE DE TRAVAIL
	
	strdcpy( &fname, dirdat, "/gen_certif/certif", itoa(getpid()), _ );
	mkdir( fname );
	if ( !dir_exists( fname ) )
		{
		print( "ERREUR SRVANNU : Pb génération du certificat", BR, _ );
		logw( "get_ssl_certif ", partname, " : ERREUR : Pb création ", fname, _ );
		return ERROR;
		}
	
	
	// PREPARATION
	
	chdir( fname );
	file_save_str( "index.txt", "", 0 );
	file_save_str( "index.txt.attr", "", 0 );
	file_save_str( "serial", "01", 2 );
	
	file_load( &s, "../cspit_certif/cspit_gen_certif.cfg" );
	strd_replace( &s, "[PARTNAME]", partname );
	file_save_str( "cspit_gen_certif.cfg", s, -1 );
	
	strdcpy( &fname1, "csr", _ );
	file_save_str( "csr", csr, -1 );
	
	// GENERATION DU CERTIFICAT
	
	strdcpy( &cmd, ssl_pgm_fname, " ca -out certif.pem",
				" -config cspit_gen_certif.cfg"
				" -in csr -notext -batch", _ );
// openssl ca -out certif.pem -config cspit_gen_certif.cfg -in csr -notext -batch

	System( &s, cmd );
	strdcpy( &fname1, "certif.pem", _ );
	if ( ( !file_exists( fname1 ) )||( file_size( fname1 ) == 0 ) )
		{
		print( "ERREUR SRVANNU : Pb génération certificat", BR BR, _ );
		logw( "get_ssl_certif ", partname, " : ERREUR OpenSSL : ", cmd, _ );
		logw( "get_ssl_certif ", partname, " : ERREUR OpenSSL : ", s, _ );
		logw( "get_ssl_certif ", partname, " : ERREUR OpenSSL : ", fname, _ );
		return ERROR;
		}
	
	if ( h ) var_free( h );
	h = var_new();
	
	strdcpy( &s2, "../cspit_certif", _ );
	if ( ssl_certif_load( h, fname1, s2 ) == ERROR )
		{
		print( "ERREUR SRVANNU : Pb génération certificat" BR, fname, BR BR, _ );
		logw( "get_ssl_certif ", partname, " : ERREUR ssl_certif_load : ", error_msg, _ );
		logw( "get_ssl_certif ", partname, " : ERREUR ssl_certif_load : ", fname, _ );
		return ERROR;
		}
	chdir( dirdat );
	file_delete( fname );
	
	print( var_get( h, "ssl_certif_server" ), BR BR, _ );
	for( i = 1 ; ; i++ )
		{
		strdcpy( &s, "ssl_certif_ca", itoa(i), _ );
		if ( !(v=var_get( h, s )) ) break;
		print( v, BR BR, _ );
		}
	
	
	//--------------------------------------------------------------------------
	
	return OK;
	}





















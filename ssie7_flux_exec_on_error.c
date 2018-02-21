#include "ssie7.h"

extern char  /* *etape,*/ *flux_etape;
extern int etape_no, etape_log_id;


//------------------------------------------------------------------------------
int on_error_execute( T_WORK* t_work )

// EXECUTE UNE ETAPE DE FLUX DE TYPE ONERREOR (Script à exécuter s'il y a eu une erreur)
//------------------------------------------------------------------------------
	{
	// RECHERCHE LE SCRIPT A EXECUTER
	for( i = atoi(flux_etape)+1 ; i < 10000 ; i++ )
		{
		strdcpy( &fname, dircflux, "/", appname, "_flux_", fluxname, "_etape_", itoa(i), ".dat", _ );
		if ( ! file_exists( fname ) )
			{
			hlogr( hLog, etape_log_id, &s, null );
			v = var_get( hFlux, "etape_next" );
			if ( ( !v )||( ! v[0] ) )
				{
				if ( !strnicmp( s, "ALERT", -1 ) )
					var_set( hFlux, "etape_next", "continue" );
				else
					var_set( hFlux, "etape_next", "stop" );
				}
			return true;
			}
		if ( hEtape ) var_free( hEtape );
		hEtape = var_load( fname );
		v = var_get( hEtape, "etape_type" );
		if ( strcmp( v, "on_error" ) ) continue;
		break;
		}
		

	// EXECUTE LE SCRIPT ON_ERROR
	
	v = var_get( hEtape, "etape_script" );
	id = hlogc( hLog, etape_log_id , "", "Exécute le script de traitement des erreurs", _ );
	var_set( hFlux, "etape_next", "stop" );
	ssie7_script_exec( t_work, false );
	
	return;
	}





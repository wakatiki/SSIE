#include "ssie7.h"

extern char  *flux_etape;
extern int etape_no, etape_log_id;


//------------------------------------------------------------------------------
int start_execute( T_WORK* t_work )

// EXECUTE UNE ETAPE DE FLUX DE TYPE START (Démarrage d'un flux)
//------------------------------------------------------------------------------
	{
	v = fluxname;
	strdcpy( &s4, fluxname = var_get( hEtape, "etape_flux_name" ), _ );
	if ( ( !fluxname )||( !(fluxname[0]) ) )
		{
		hlog_raise_status( hLog, etape_log_id, "ERREUR : Aucun flux de définit", _ );
		var_set( hFlux, "etape_next", "stop" );
		fluxname = v;
		return;
		}
		
	var_list_replace( hFlux, &s4 );
	fluxname = s4;
	if ( ! stricmp( v, fluxname ) )
		var_set( hFlux, "flux_do_restart", "false" );
	
	id = hlogc( hLog, etape_log_id, "", "Démarre le flux : ", fluxname, _ );
		
	System( &s3, strdcpy( &s, argv[0], " flux \"", appname, "\" \"", fluxname, "\" start no_verify", _ ) );
	hlog_raise_status( hLog, etape_log_id, s3, _ );
	
	fluxname = v;

	if (( stripos( s3, "ERREUR", 0 ) >= 0 )||( stripos( s3, "ERROR", 0 ) >= 0 ))
		on_error_execute( t_work );


	return;
	}















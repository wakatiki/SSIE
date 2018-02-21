#include "ssie7.h"

extern char  /* *etape,*/ *flux_etape;
extern int etape_no, etape_log_id;



//------------------------------------------------------------------------------
int startinside_execute( T_WORK* t_work )

// EXECUTE UNE ETAPE DE FLUX DE TYPE STARTINSIDE (Démarrage d'un flux dans la même log que le flux en-cours)
//------------------------------------------------------------------------------
	{
	v = var_get( hEtape, "etape_flux_name" );
	if ( ( !v )||( !(v[0]) ) )
		{
		hlog_raise_status( hLog, etape_log_id, "ERREUR : Aucun flux de définit", _ );
		var_set( hFlux, "etape_next", "stop" );
		return;
		}
	
	strdcpy( &s4, v, _ );	
	var_list_replace( hFlux, &s4 );
	
	hlogu( hLog, etape_log_id, "", "Etape ", flux_etape, " : Exécute le flux : ", s4, _ );

	strdcpy( &s, fluxname, ",", itoa(logroot), ",", flux_etape, ",", flux_etape,
				 "|", var_get( hFlux, "startinside_stack" ), _);
				 
	if ( strcount( s, "|", 0 ) > 100 )
		{
		hlog_raise_status( hLog, etape_log_id, "ERREUR : Trop d'appels imbriqués de StartInside", _ );
		var_set( hFlux, "etape_next", "stop" );
		return;
		}

	var_set( hFlux, "startinside_stack", s );
	
	strd_replace( &s4, "/", "@" );
	fluxname = str( s4, _ );
	var_set( hFlux, "flux_name", fluxname );
	
	etape_no = 0;
//	var_set( hFlux, "flux_etape", "0" );
	var_delete( hFlux, "etape_1_conf_fname" );
	
//	logroot = etape_log_id;

	return;
	}







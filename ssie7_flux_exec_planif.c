#include "ssie7.h"

extern char *flux_etape;
extern int	etape_no, etape_log_id;



//------------------------------------------------------------------------------
int planif_execute( T_WORK* t_work )

// EXECUTE UNE ETAPE DE FLUX DE TYPE PLANIF (Calcul d'une date de démarrage et attente de cette date)
//------------------------------------------------------------------------------
	{
	tr("planif_execute_1");
	if ( var_get( hFlux, strdcpy( &s, "etape", flux_etape, "waitdate", _ ) ) )
		{
		tr("planif_execute_2");
		var_free( hConf2 );
		hConf2 = var_load( strdcpy( &fname, dircflux, "/", appname, "_flux_",
							fluxname, ".dat", _ ) );
		var_set( hConf2, strdcpy( &s, "etape", flux_etape, "last_exec", _ ), itoa( time(NULL) ) );
		var_save( hConf2, fname );
		var_set( hFlux, "etape_next", "continue" );
		var_set( hFlux, "etape_no", itoa( etape_no+1 ) );
		hlogu_status( hLog, etape_log_id, "OK", _ );
		return true;
		}

	tr("planif_execute_2b");
	v = var_get( hEtape, "etape_planif_enable" );
	if ( ( v )&&( strcmp( v, "Oui" ) ) )
		{
		tr("planif_execute_3");
		hlogu( hLog, etape_log_id, "OK", "Planification désactivée", _ );
		
		// DEMARRAGE		
		if ( hConf2 ) var_free( hConf2 );
		hConf2 = var_load( strdcpy( &fname, dircflux, "/", appname, "_flux_",
							fluxname, ".dat", _ ) );
		var_set( hConf2, strdcpy( &s, "etape", flux_etape, "last_exec", _ ), itoa( time(NULL) ) );
		var_save( hConf2, fname );
		var_set( hFlux, "etape_next", "continue" );
		return true;
		}
	
	tr("planif_execute_3b");
	if ( ! var_get( hFlux, "flux_do_restart" ) )
		var_set( hFlux, "flux_do_restart", "true" );
			
	// CALCUL LA DATE DE LA PROCHAINE EXECUTION
	
	if ( hConf2 ) var_free( hConf2 );
	hConf2 = var_load( strdcpy( &fname, dircflux, "/", appname, "_flux_",
						fluxname, ".dat", _ ) );
	v = var_get( hConf2, strdcpy( &s, "etape", flux_etape, "last_exec", _ ) );
	if ( v ) h = atoi( v );
	else h = 0;
	v = var_get( hEtape, "etape_planif" );
	tr("planif_execute_3c");
	if ( !v )
		{
		tr("planif_execute_4");
		var_set( hFlux, "flux_do_restart", "false" );
		hlog_raise_status( hLog, etape_log_id, "ERREUR", "Planification non défine", _ );
		var_set( hFlux, "etape_next", "stop" );
		return true;
		}

	strdcpy( &s2, v, _ );
	var_list_replace( hFlux, &s2 );
	
	tr2("planif_execute_4b:", s2);
	tr2("h =", itoa(h));
	r = planif_compute( s2, h );
	tr("planif_execute_4c");
	if ( r == ERROR )
		{
		tr("planif_execute_5");
		var_set( hFlux, "flux_do_restart", "false" );
		hlog_raise_status( hLog, etape_log_id, "ERREUR", error_msg, _ );
		var_set( hFlux, "etape_next", "stop" );
		return true;
		}

	tr("planif_execute_5b");
	var_set( hFlux, "wait_date", v=itoa(r) );
	var_set( hFlux, strdcpy( &s, "etape", flux_etape, "waitdate", _ ), v );
	
	hlog_raise_status( hLog, etape_log_id, strdcpy( &s, "ATTEND : ", itod( r, "jj/mm/aaaa hh:mm:ss" ), _ ), _ );
	hlogu_status( hLog, 0, "Etape ", flux_etape, " : planif : ", s, _ );
	
/*
	// PLACE LE FLUX DANS LA LISTE D'ATTENTE WDATE

	strdcpy( &fname, dirtmp, DIR_CHAR, flux_id, "_", itoa(getpid()), _ );
	fp = fopen( fname, "wb" );
	if ( !fp )
		{
		hlog_raise_status( hLog, etape_log_id, "ERREUR : Création fichier ", fname, _ );
		var_set( hFlux, "etape_next", "stop" );
		var_set( hFlux, "flux_do_restart", "false" );
		return true;
		}

	fprint( fp, argv[0], " flux ", flux_id, " execute", _ );
	fclose( fp );
	
	strdcpy( &fname1, dirwdate, DIR_CHAR, "flux_", flux_id, ".", itoa(r), _ );
	
	if ( file_move( fname, fname1 ) == ERROR )
		logw( "flux_exec_planif : ", error_msg, _ );
	
	
	// PLACE LE FLUX EN ATTENTE

	strdcpy( &fname, dirrflux, DIR_CHAR, flux_id, _ );
	fp = fopen( fname, "wb" );
	if ( !fp )
		{
		hlog_raise_status( hLog, etape_log_id, "ERREUR : Création fichier ", fname, _ );
		var_set( hFlux, "etape_next", "stop" );
		return true;
		}

	fprint( fp, "wait_date:", fname1, _ ); // L'écriture fichier permettra d'enlever le flux dans la liste des process en attente
	fclose( fp );
	
		
	// PREPARE LE REDEMARRAGE
			
	var_set( hFlux, strdcpy( &s, "etape", flux_etape, "waitdate", _ ), "true" );
	var_save( hFlux, strdcpy( &fname, dirflux, "/", flux_id, ".dat", _ ) );
*/
	tr("planif_execute_6");
	return false; // Indique que l'on ne passe pas à l'étape suivante
	}

















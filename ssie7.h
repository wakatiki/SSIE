#ifndef __SSIE7_H__
#define __SSIE7_H__


//------------------------------------------------------------------------------

#include "elib4.c"
#define MFT
#include "ssie7lib.c"



//------------------------------------------------------------------------------
// PARAMETRAGE DU MODULE SSIE7

#define MODULE_NAME			"ssie7"
#define DEFAULT_PORT		"1765"


#ifdef OS_WINDOWS
	#define APPLI_DIR		"c:\\e\\Apps"
	#define LOGI_DIR		""
	#define BIN_DIR			"."
	#define OPENSSL			"c:\\e\\Apps\\OpenSSL\\openssl.exe"
#endif

#ifdef OS_AIX
	#define APPLI_DIR		"/appli"
	#define LOGI_DIR		""
	#define BIN_DIR			"/ssie7"
	#define OPENSSL			null
#endif

#ifdef OS_LINUX
	#define APPLI_DIR		"/appli"
	#define LOGI_DIR		"/logiciels"
	#define BIN_DIR			"/logiciels/ssie7"
	#define OPENSSL			null
#endif

#ifdef OS_SOLARIS
	#define APPLI_DIR		"/appli"
	#define LOGI_DIR		"/logiciels"
	#define BIN_DIR			"/logiciels/ssie7"
	#define OPENSSL			null
#endif



//------------------------------------------------------------------------------
// DEFINITION DES VARIABLES GLOBALES


EXTERN char * appname			init_null;
EXTERN char * dircflux			init_null;
EXTERN char * dirflux			init_null;
EXTERN char * dirrflux			init_null;
EXTERN char * flux_id			init_null;
EXTERN char * fluxname			init_null;
EXTERN char * direxport			init_null;
EXTERN char * dirimport			init_null;
EXTERN char * export_id			init_null;
EXTERN char * import_id			init_null;

EXTERN char * diridf			init_null;

#define hFlux	hConf
#define hEtape	hConf1


#include "ssie7lib_cmd.c"
#include "ssie7lib_cmd_connect.c"
#include "ssie7lib_cmd_exploit.c"

#include "ssie7lib_mft.c"
#include "ssie7lib_cmd_mft.c"
#include "ssie7lib_cmd_mft_send.c"
#include "ssie7lib_cmd_mft_send_part.c"
#include "ssie7lib_cmd_mft_cat.c"
#include "ssie7lib_cmd_mft_server_recv.c"

#include "ssie7lib_cmd_mft_recv.c"

#include "ssie7lib_cmd_request.c"
#include "ssie7lib_gateway.c"
#include "ssie7lib_run.c"
//#include "ssie7lib_vdisk.c"
//#include "ssie7lib_vdisk_client.c"
//#include "ssie7lib_vdisk_server.c"

#include "ssie7_cmd_mftutil.c"
#include "ssie7_cmd_mersi.c"

#ifdef OS_WINDOWS
#include "ssie7_bdoc.c"
#endif

#ifdef TESTS
#include "ssie7_cmd_test.c"
#endif

#include "ssie7_cmd_app.c"
#include "ssie7_cmd_mft.c"
#include "ssie7_cmd_exploit.c"
#include "ssie7_cmd_export.c"
#include "ssie7_cmd_flux.c"
#include "ssie7_cmd_gateway.c"
#include "ssie7_cmd_import.c"
#include "ssie7_cmd_login.c"
#include "ssie7_cmd_part.c"
#include "ssie7_cmd_server.c"
//#include "ssie7_cmd_upgrade.c"

#include "ssie7_flux_exec.c"
#include "ssie7_flux_exec_ack.c"
#include "ssie7_flux_exec_emi.c"
#include "ssie7_flux_exec_on_error.c"
#include "ssie7_flux_exec_planif.c"
#include "ssie7_flux_exec_rec.c"
#include "ssie7_flux_exec_script.c"
#include "ssie7_flux_exec_start.c"
#include "ssie7_flux_exec_startinside.c"
#include "ssie7_flux_exec_waitfile.c"
#include "ssie7_lib.c"
#include "ssie7_report.c"
#include "ssie7_robot.c"


#endif























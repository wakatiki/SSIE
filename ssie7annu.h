#ifndef __SSIE7ANNU_H__
#define __SSIE7ANNU_H__


#include "elib4.c"
#define MFT
#include "ssie7lib.c"


//------------------------------------------------------------------------------
// PARAMETRAGE DU MODULE SSIE7ANNU

#define MODULE_NAME			"ssie7annu"
#define PRODUCT_NAME		"SSIE ANNUAIRE v7.1_2015_09_15"

#define DEFAULT_PORT		"1760"


#ifdef OS_WINDOWS
	#define APPLI_DIR		"c:\\e\\apps"
	#define BIN_DIR			"c:\\e\\bin\\windows"
	#define OPENSSL			"c:\\e\\bin\\windows\\ssie7sec.exe"
#endif

#ifdef OS_AIX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/ssie7"
	#define OPENSSL			null
#endif

#ifdef OS_LINUX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/logiciels/ssie7"
	#define OPENSSL			null
#endif

#ifdef OS_SOLARIS
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/logiciels/ssie7"
	#define OPENSSL			null
#endif



#include "ssie7lib_cmd.c"
//#include "ssie7lib_cmd_connect.c"
#include "ssie7lib_cmd_exploit.c"
//#include "ssie7lib_cmd_request.c"
#include "ssie7lib_cmd_server.c"
#include "ssie7lib_run.c"

#include "ssie7annu_cmd_get_ssl_certif.c"
#include "ssie7annu_cmd_login.c"
#include "ssie7annu_cmd_mersi3nat.c"
#include "ssie7annu_cmd_part.c"
#include "ssie7annu_robot.c"



#endif




































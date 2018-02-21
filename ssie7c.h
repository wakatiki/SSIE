#ifndef __SSIE7C_H__
#define __SSIE7C_H__


#include "elib4.c"
#define MFT
#include "ssie7lib.c"

//------------------------------------------------------------------------------
// PARAMETRAGE DU MODULE SSIE7NAT

#define MODULE_NAME			"ssie7c"
#define PRODUCT_NAME		"SSIE Controleur v7.1.a"
#define DEFAULT_PORT		"1770"



#ifdef OS_WINDOWS
	#define APPLI_DIR		"c:\\e\\apps"
	#define BIN_DIR			"c:\\e\\bin\\windows"
	#define OPENSSL			"c:\\e\\bin\\windows\\ssie7sec.exe"
#endif

#ifdef OS_AIX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/"
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
#include "ssie7lib_cmd_part.c"
#include "ssie7lib_cmd_connect.c"
//#include "ssie7lib_cmd_exploit.c"
#include "ssie7lib_cmd_request.c"
#include "ssie7lib_cmd_server.c"
#include "ssie7lib_run.c"
//#include "ssie7lib_robot.c"
#include "ssie7lib_vdisk.c"
#include "ssie7lib_vdisk_client.c"
#include "ssie7lib_vdisk_server.c"

#include "ssie7c_cmd_gw_inst.c"
#include "ssie7c_cmd_initialize.c"


#endif































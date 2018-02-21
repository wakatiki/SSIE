
//------------------------------------------------------------------------------

#define MODULE_NAME			"ssie7"


#ifdef OS_WINDOWS
	#define APPLI_DIR		"c:\\edf\\appli"
	#define BIN_DIR			"c:\\edf\\logiciels"
#endif

#ifdef OS_AIX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/"
#endif

#ifdef OS_LINUX
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/logiciels"
#endif

#ifdef OS_SOLARIS
	#define APPLI_DIR		"/appli"
	#define BIN_DIR			"/logiciels"
#endif



//------------------------------------------------------------------------------
// DEFINITION DES VARIABLES GLOBALES


EXTERN char * mft_dirbin				init_null;
EXTERN char * mft_dirconf				init_null;
EXTERN char * mft_dirdat				init_null;
EXTERN char * mft_dirlog				init_null;
EXTERN char * mft_dirrecv				init_null;

EXTERN char * mft_conf_fname			init_null;
EXTERN int	  hMFT						init_false;



#ifdef OS_UNIX
#define CNX	"48"
#define MFTCHAR	"@"
#else
#define CNX	"16"
#define MFTCHAR	"#"
#endif



























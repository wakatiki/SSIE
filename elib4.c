#include "elib.h"

#define free			elib_free

#include "elib_console.c"
#include "elib_crypto.c"
#include "elib_date.c"
#include "elib_error.c"
#include "elib_file.c"
#include "elib_hlog.c"
#include "elib_math.c"
#include "elib_ssl.c"
#include "elib_system.c"
#include "elib_string.c"
#include "elib_tcp.c"
#include "elib_var.c"
#include "elib_idat.c"
#include "elib_bd.c"
#include "elib_bdoc.c"

#ifdef v72
#include "elib_crypto2.c"
#endif

#undef free

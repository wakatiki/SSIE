#ifndef __SSIE7LIB_H__
#define __SSIE7LIB_H__

#include "elib.h"


#define PROTOCOLE_VERSION	"101"
//#define PROTOCOLE_VERSION	"100"

//------------------------------------------------------------------------------
// DEFINITION DES VARIABLES GLOBALES

EXTERN char *		module_name				init_null;
EXTERN char *		module_short_name		init_null;	// Pour v7.2
EXTERN char *		product_name			init_null;
EXTERN char *		default_port			init_null;
EXTERN char *		ssie_user				init_null;

bool bPEPSIS = false;

EXTERN char * 		dirapp					init_null;
EXTERN char * 		dirbin					init_null;
EXTERN char * 		dirlog					init_null;
EXTERN char * 		dirdat					init_null;
EXTERN char * 		dirconf					init_null;
EXTERN char * 		dirtemp					init_null;
EXTERN char * 		dirwrk					init_null;
//EXTERN char *		dirpsurv				init_null;
//EXTERN char *		dirwdate				init_null;
EXTERN char * 		dirpart					init_null;
//EXTERN char *		dirwfile				init_null;
EXTERN char * 		dirrecv					init_null;
//EXTERN char * 		dirwrecv				init_null;
//EXTERN char * 		dirsend					init_null;

EXTERN char * 		dirwww					init_null;


EXTERN long int		hSrvConf				init_false;
EXTERN char	*		server_conf_fname		init_null;

EXTERN int			request_mode;
	#define MODE_CONSOLE		1
	#define MODE_CMDE			2
	#define MODE_TCP			3
	#define MODE_ROBOT			4


EXTERN char *cmd_arg;


//------------------------------------------------------------------------------
// STRUCTURE DE TRAVAIL

typedef struct
	{
	char		*user, *what, *profile;
	bool		is_admin;
	char		*cmd, *cmd1, *ip_cli, *srv_ip, *srv_port, *partname, *part, *ssie7out, *trace_fname;
	long int	log_id, logroot;
	handle		hReq, hLog;
	HANDLE		hLock, hLock1;

	char 		*session_partname, *session_ipport, *session_version, *session_id,
				*session_key, *session_files_dir, protocol;
	handle		session_cnx;

	char 		*s, *s1, *s2, *s3, *s4, *vvv,
				*v, *v1, *v2, *v3, *v4, *v5, *v6, *v7, *vn, *w, // Ne pas faire de free sur ces variables
				**ts, **ts1, **ts2,
				*fname, *fname1, *fname2, *fname3, *fname4,
				*ip, *sid, *ida,
				*u1, *u2, *u3, *u4, *u5, *u6, *u7, *u8, *u9, *u10,
				*u11, *u12, *u13, *u14, *u15 , *VLI;

	handle		hPart, hConf, hConf1, hConf2, hCnx, hCnxOld, h, h1, h2, h3, hI; // Bien remettre à zéro si on fait un var_free
	long int	id,	c, e, i, j, k, r, p, p0, p1, p2, p3, p4, p5, p6,
				l, n, n1, n2, n3, n4, n5;
	long long	ll, ll1, ll2;

	bool		bDisconnect, b, b1, b2, b3, b4, pid, t;
	FILE		*fp, *fp1, *fp2, *fpTrace;
	T_FILE_INFO *tf, *tf2;
	}
	T_WORK;




//------------------------------------------------------------------------------

#define T_WORK_ALLOC	T_WORK *t_work = (T_WORK*)calloc( 1, sizeof(T_WORK) )


#define tLog		(t_work->tLog)
#define log_id		(t_work->log_id)
#define logroot		(t_work->logroot)
#define user		(t_work->user)
#define profile		(t_work->profile)
#define is_admin	(t_work->is_admin)
#define what		(t_work->what)

#define session_partname		(t_work->session_partname)
#define session_ipport			(t_work->session_ipport)
#define session_version			(t_work->session_version)
#define session_id				(t_work->session_id)
#define session_key				(t_work->session_key)
#define session_files_dir		(t_work->session_files_dir)
#define session_cnx				(t_work->session_cnx)
#define protocol				(t_work->protocol)

#define s			(t_work->s)
#define VLI                    (t_work->VLI)
#define s1			(t_work->s1)
#define s2			(t_work->s2)
#define s3			(t_work->s3)
#define s4			(t_work->s4)
#define v			(t_work->v)
#define vn			(t_work->vn)
#define v1			(t_work->v1)
#define v2			(t_work->v2)
#define v3			(t_work->v3)
#define v4			(t_work->v4)
#define v5			(t_work->v5)
#define v6			(t_work->v6)
#define v7			(t_work->v7)
#define u4			(t_work->u4)
#define u5			(t_work->u5)
#define u6			(t_work->u6)
#define u7			(t_work->u7)
#define u8			(t_work->u8)
#define u9			(t_work->u9)
#define u10			(t_work->u10)
#define u11			(t_work->u11)
#define u12			(t_work->u12)
#define u13			(t_work->u13)
#define u14			(t_work->u14)
#define u15			(t_work->u15)
#define w			(t_work->w)
#define cmd			(t_work->cmd)
#define cmd1		(t_work->cmd1)
#define ts			(t_work->ts)
#define ts1			(t_work->ts1)
#define ts2			(t_work->ts2)
#define fname		(t_work->fname)
#define fname1		(t_work->fname1)
#define fname2		(t_work->fname2)
#define fname3		(t_work->fname3)
#define fname4		(t_work->fname4)
#define trace_fname	(t_work->trace_fname)
#define ip			(t_work->ip)
#define ip_cli		(t_work->ip_cli)
#define srv_ip		(t_work->srv_ip)
#define srv_port	(t_work->srv_port)
#define partname	(t_work->partname)
#define part		(t_work->part)
#define ssie7out	(t_work->ssie7out)
#define hPart		(t_work->hPart)
#define hConf		(t_work->hConf)
#define hConf1		(t_work->hConf1)
#define hConf2		(t_work->hConf2)
#define hReq		(t_work->hReq)
#define hCnx		(t_work->hCnx)
#define hCnxOld		(t_work->hCnxOld)
#define hLog		(t_work->hLog)
#define hLock		(t_work->hLock)
#define hLock1		(t_work->hLock1)
#define h			(t_work->h)
#define h1			(t_work->h1)
#define h2			(t_work->h2)
#define h3			(t_work->h3)
#define hI			(t_work->hI)
#define id			(t_work->id)
#define sid			(t_work->sid)
#define ida			(t_work->ida)
#define c			(t_work->c)
#define e			(t_work->e)
#define i			(t_work->i)
#define j			(t_work->j)
#define k			(t_work->k)
#define l			(t_work->l)
#define ll			(t_work->ll)
#define ll1			(t_work->ll1)
#define ll2			(t_work->ll2)
#define n			(t_work->n)
#define n1			(t_work->n1)
#define n2			(t_work->n2)
#define n3			(t_work->n3)
#define n4			(t_work->n4)
#define n5			(t_work->n5)
#define pid			(t_work->pid)
#define p32			(t_work->p32)
#define p			(t_work->p)
#define p0			(t_work->p0)
#define p1			(t_work->p1)
#define p2			(t_work->p2)
#define p3			(t_work->p3)
#define p4			(t_work->p4)
#define p5			(t_work->p5)
#define p6			(t_work->p6)
#define r			(t_work->r)
#define t			(t_work->t)
#define bDisconnect	(t_work->bDisconnect)
#define b			(t_work->b)
#define b1			(t_work->b1)
#define b2			(t_work->b2)
#define b3			(t_work->b3)
#define b4			(t_work->b4)
#define fpTrace		(t_work->fpTrace)
#define fp			(t_work->fp)
#define fp1			(t_work->fp1)
#define fp2			(t_work->fp2)
#define tf			(t_work->tf)
#define tf2			(t_work->tf2)




//------------------------------------------------------------------------------
// FONCTIONS SSIE7LIB


void	ssie7_init( T_WORK* t_work );
int		logw( char *msg, ... );
char * 	nat_request( char **p_out, char *p_cmd );
char *	nat_search( );

#define ssie7exit( rc )		{ ssie7free( t_work ); exit(rc); }


#define trace( ... )		ssie7trace( t_work, __FILE__, " : ", __VA_ARGS__ )
#define trace_close( )		ssie7trace_close( t_work )
EXTERN int		trace_no_delete				init_false;
#define trace_nodelete( )		trace_no_delete = true

#define is_authorized( app, right ) is_authorized0( t_work, app, right )
#define NNI_ADM "DSPCSPITCCWEBEXPESMMERSIMFT"


bool	part_exists( T_WORK * );
char *	part_get_address( T_WORK *, char ** );



#ifdef MFT
char*	mft_exe( );
#endif


#endif







#include "ssie7.h"

/*
int cmd_mersi_emission( T_WORK *t_work )
int cmd_mersi_liste( T_WORK *t_work )
int cmd_mersi_reception( T_WORK *t_work )
*/


//------------------------------------------------------------------------------
int cmd_mersi_emission( T_WORK *t_work )

// COMMANDE MERSI_EMISSION (REPRISE A L'IDENTIQUE DU FONCTIONNEMENT DU SCRIPT mersi_emission.ksh)
//------------------------------------------------------------------------------
	{
	/*
	# Syntaxe    : mersi_emission.ksh
	#                   -f <fichier à émettre>
	#                   -appr|-m <appr>
	#                   -r|-adlr|-part <adlr ou part>
	#                   [-e|-adle <adle>]
	#                   [-appe <appe>]
	#                   [-idf|-i <idf>] 
	#                   [-ascii]
	#                   [-bin]
	#                   [-u ou -parm <zutile>]        
	#                   [-opt ou -o <mft param>]
	#                   [-c(odex)]
	#                   [-dcb ou -d <NRECFM>/<NBLKSIZE>/<NLRECL>]
	#                   [-wait]
	#                   [-nocopy]
	#
	# Parametres obligatoires:
	# <adlr|part>: Adresse logique stor réceptrice ou nom partenaire récepteur
	# <appr>     : Nom de l'application réceptrice
	# <fichier>  : Nom du fichier à transférer
	#
	# Parametres optionnels:
	# <adle>     : Adresse logique stor émettrice (par défaut c'est le nom de partenaire local)
	# <idf>      : identifiant de transfert (par defaut c'est R2MMSB)
	# <appe>     : Nom de l'application émettrice
	# <zutile>   : Zone utilisateur (informations applicatives)
	# <dcb>      : définition du DCB MVS
	#              <NRECFM = F, V ou U> / <NBLKSIZE> / <NLRECL> 
	# <mft param>: Autre(s) paramètre(s) éventuel(s) à passer à MFT 
	# <codex>    : Mot-clef signalant un envoi vers une application DEGS CODEX
	#              (la zone utilisateur est préfixée par 'AEP' suivi de 39 caractères '*')
	# <nc>       : no control : ne fait pas les controles de validités des partenaires et idf
	#
	# CRetour    : 0:OK   >0:Erreur, l'opération n'a pu se réaliser, voir l'affichage ou la log
	#                        log mersi = /home/<votre login unix>/mersi3.log 
	*/
	
	
	
	//--------------------------------------------------------------------------
	// LECTURE DES PARAMETRES
	//--------------------------------------------------------------------------
	int der_el=0;
	var_free( h );
	h = var_new();
	strdcpy( &s2, "", _ );

	ts = str_explode( cmd, " " );

	for( i = 1 ; vn = ts[i] ; i++ )
		{
		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-part" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe" BR, _ );
				r = 50;
				return ERROR;
				}
			var_set( h, "part", vn );
			if ( ! var_get( h, "adlr" ) )
				var_set( h, "adlr", vn );
				
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-f" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -f" BR, _ );
				r = 50;
				return ERROR;
				}
			if( ts[i+1]) der_el=1;// pas le dernier element
			if ( !file_exists( unquote(vn) ) )
				{
				print( "ERREUR : Le fichier à envoyer n'existe pas : ", vn, BR, _ );
				r = 52;
				return ERROR;
				}
			var_set( h, "fname", vn );
			continue;
			}

		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-appr" ) )||( ! strcmp( vn, "-m" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -appr" BR, _ );
				r = 50;
				return ERROR;
				}
			var_set( h, "appr", vn );
			continue;
			}

		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-adlr" ) )||( ! strcmp( vn, "-r" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -adlr" BR, _ );
				r = 50;
				return ERROR;
				}
			var_set( h, "adlr", vn );

			if ( ! var_get( h, "part" ) )
				var_set( h, "part", vn );

			
			continue;
			}

		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-adle" ) )||( ! strcmp( vn, "-e" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -adle" BR, _ );
				r = 50;
				return ERROR;
				}
			var_set( h, "adle", vn );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-appe" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -appe" BR, _ );
				r = 50;
				return ERROR;
				}
			var_set( h, "appe", vn );
			continue;
			}

		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-idf" ) )||( ! strcmp( vn, "-i" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -idf" BR, _ );
				r = 50;
				return ERROR;
				}
			strdcat( &s2, " idf=", vn, _ );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-ascii" ) )
		//----------------------------------------------------------------------
			{
			strdcat( &s2, " ascii", _ );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-bin" ) )
		//----------------------------------------------------------------------
			{
			strdcat( &s2, " binary", _ );
			continue;
			}

		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-parm" ) )||( ! strcmp( vn, "-u" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -parm" BR, _ );
				r = 50;
				return ERROR;
				}
			strdcat( &s2, " parm=", quote( vn ), _ );
			continue;
			}

		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-opt" ) )||( ! strcmp( vn, "-o" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -opt" BR, _ );
				r = 50;
				return ERROR;
				}

			strdcat( &s2, " ", vn, _ );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-c" ) )
		//----------------------------------------------------------------------
			{
			var_set( h, "codex", "true" );
			continue;
			}

		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-dcb" ) )||( ! strcmp( vn, "-d" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -dcb" BR, _ );
				r = 50;
				return ERROR;
				}
			// <NRECFM>/<NBLKSIZE>/<NLRECL>			
			ts2 = str_explode( vn, "/" );
			if ( ts2[0] ) strdcat( &s2, " nrecfm=", ts2[0], _ );
			if (( ts2[0] )&&( ts2[1] )&&( ts2[1][0] )) strdcat( &s2, " nblksize=", ts2[1], _ );
			if (( ts2[0] )&&( ts2[1] )&&( ts2[2] )&&( ts2[2][0] )) strdcat( &s2, " nlrecl=", ts2[2], _ );
			free( ts2 ); ts2 = null;
			
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-wait" ) )
		//----------------------------------------------------------------------
			{
			strdcat( &s2, " waitend", _ );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-nocopy" ) )
		//----------------------------------------------------------------------
			{
			strdcat( &s2, " nocopy", _ );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strnicmp( vn, "user", 4 ) )
		//----------------------------------------------------------------------
			{
			if ( (p=strpos( vn, "=", 4 )) > 0 )
				{
				if ( vn[p+1] )	continue;
				i++;
				if ( !ts[i] ) break;
				continue;
				}
			i++;
			vn = ts[i];
			if ( !vn ) break;
			if ( vn[0] != '=' ) continue;
			if ( vn[1] )	continue;
			i++;
			if ( !ts[i] ) break;
			continue;
			}
			
		//----------------------------------------------------------------------
		print( "ERREUR : Option inconnue : ", vn, BR, _ );
		r = 50;
		return ERROR;
		}

	free( ts ); ts = null;
	
	if (( ! (v=var_get( h, "appr" )) )||( ! (v[0]) ))
		{
		print( "ERREUR : paramètre -appr obligatoire." BR, _ );
		r = 50;
		return ERROR;
		}
	
	if (( ! (v=var_get( h, "adle" )) )||( ! (v[0]) ))
		{
		if ( v = var_get( hSrvConf, "partname" ) )
			var_set( h, "adle", v );
		else
			var_set( h, "adle", "[$part]" );
		}

	
	//--------------------------------------------------------------------------
	// PREPARATION DE LA COMMANDE D'ENVOI
	//--------------------------------------------------------------------------
	
	if (( ! (v1=var_get( h, "part" )) )||( ! (v1[0]) ))
		{
		print( "ERREUR : paramètre -part ou -adlr obligatoire" BR, _ );
		r = 50;
		return ERROR;
		}
	
	if (( ! (v=var_get( h, "fname" )) )||( ! (v[0]) ))
		{
		print( "ERREUR : paramètre -f obligatoire" BR, _ );
		r = 50;
		return ERROR;
		}
	
	if ( stripos( s2, "nocopy", 0 ) < 0 )
		{
		strdcat( &s2, " copyfile", _ );
		}
	//fname avec espace #2017042010000584 & #2017040410000373
	if (der_el) strd_quote(&v);
	strdcpy( &cmd, "mft send part=", v1, " fname=", v,
			" ", s2,
			" sappl='", var_get( h, "adle" ), var_get( h, "appe" ), "'",
			" rappl='", var_get( h, "adlr" ), var_get( h, "appr" ), "'",
			" suser='[$ida]", var_get( h, "adle" ), "'",
			" ruser='[$ida]001001'",
			_ );

		
	if (( stripos( cmd, " idf=", 0 ) < 0 )&&( stripos( cmd, ",idf=", 0 ) < 0 )
			&&( stripos( cmd, " idf =", 0 ) < 0 )
			&&( stripos( cmd, ",idf =", 0 ) < 0 )
			)
		{
		if ( var_get( hReq, "-ascii" ) )
			strdcat( &cmd, " idf=R2MMS", _ );
		else
			strdcat( &cmd, " idf=R2MMSB", _ );
		}

	var_free( hReq );
	hReq = var_from_cmd( 0, cmd );
	r = 0;
	if ( cmd_mft( t_work ) == ERROR ) r = 50;
	
	//logw("sni message dattente=",var_get( hReq,"errmsg" ),_);
	//v = var_get( hReq, "status" );
        //if ( stricmp( v, "En cours" ) ) sleep(5);
	//logw("sni message dattente2=",var_get( hReq,"errmsg" ),"/",cmd,_);
	
	//var_set( hReq, "errmsg", "OK" );
	mft_reveil_compute( t_work, "SEND" );//lancer les reveils ACR 
	//var_free(h);
	//var_free( hReq );
	//logw("sni: fin de mersi_emission ----> ",_);
	return OK;
	}

// mersi_emission.ksh -part CCIEUX1I -f c:\tmp\test.txt -adle ADLTEST1 -appe APPTEST1 -appr ADLTEST2


//#undef print	// On prend le print standard ELIB

//------------------------------------------------------------------------------
int cmd_mersi_liste( T_WORK *t_work )

// COMMANDE MERSI_LISTE (REPRISE A L'IDENTIQUE DU FONCTIONNEMENT DU SCRIPT mersi_liste.ksh)
//------------------------------------------------------------------------------
	{
	/*
	#-------------------------------------------------------------------------------
	# Produit    : MERSI_LISTE
	# Version    : v3.1.01
	#
	# Fonction : Liste les transferts au catalogue MFT
	#              - les paramètres de sélection sont liés par un "ET" logique
	#
	# Syntaxe : mersi_liste.ksh
	#                  [-f <fichier>]   [-adle <adle>] [-adlr <adlr>]
	#                  [-appe <appe>] [-appr <appr>] [-u <zutile>]
	#                  [-both|-send|-recv]  [-etat <etat>]
	#                  [-idt <idt>] [-ida <ida>]
	#
	# Parametres obligatoires:
	# aucun
	#
	# Parametres optionnels:
	# <fichier>  : Nom du fichier qui contiendra la liste à restituer
	#            : si pas indiqué, le résultat s'affiche à l'écran
	# <adle>     : Adresse logique stor émettrice ou partenaire émetteur
	# <adlr>     : Adresse logique stor réceptrice ou partenaire récepteur
	# <appe>     : Nom de l'application émettrice
	# <appr>     : Nom de l'application réceptrice
	# <zutile>   : Zone utile utilisateur
	# -both      : Selectionne les transferts en émission et réception
	# -send      : Selectionne les transferts en émission seulement
	# -recv      : Sélectionne les transferts en réception
	# -etat      : Selectionne les transferts à l'état MFT <etat> (T, H, C, D ou X )
	#
	# CRetour    : 0:OK	>0:Erreur grave
	#-------------------------------------------------------------------------------
	*/

	//--------------------------------------------------------------------------
	// LECTURE DES PARAMETRES
	//--------------------------------------------------------------------------
	
	ts = str_explode( cmd, " " );
	strdcpy( &cmd, "mftcat ", _ );
	fp = NULL;
	
	for( i = 1 ; vn = ts[i] ; i++ )
		{
		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-f" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -f" BR, _ );
				r = 50;
				return ERROR;
				}
			if ( request_mode != MODE_TCP )
				{
				unlink( vn );
				print_to_file( vn );
				}
			else
				{
				file_tmpname( &fname3 );
				fp = fopen( fname3, "wb" );
				strdcpy( &fname2, vn, _ );
				}
			continue;
			}


		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-part" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -part" BR, _ );
				r = 50;
				return ERROR;
				}
			
			strdcat( &cmd, "part='", vn, "' ", _ );
			continue;
			}


		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-appr" ) )||( ! strcmp( vn, "-m" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -appr" BR, _ );
				r = 50;
				return ERROR;
				}
				
			strdcat( &cmd, "rappl='*", vn, "' ", _ );
			continue;
			}


		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-adlr" ) )||( ! strcmp( vn, "-r" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -adlr" BR, _ );
				r = 50;
				return ERROR;
				}
			
			strdcat( &cmd, "rappl='", vn, "*' ", _ );
			continue;
			}


		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-adle" ) )||( ! strcmp( vn, "-e" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -adle" BR, _ );
				r = 50;
				return ERROR;
				}
			strdcat( &cmd, "sappl='", vn, "*' ", _ );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-appe" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -appe" BR, _ );
				r = 50;
				return ERROR;
				}
			strdcat( &cmd, "sappl='*", vn, "' ", _ );
			continue;
			}


		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-both" ) )
		//----------------------------------------------------------------------
			{
			strdcat( &cmd, "direct=both ", _ );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-send" ) )
		//----------------------------------------------------------------------
			{
			strdcat( &cmd, "direct=send ", _ );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-recv" ) )
		//----------------------------------------------------------------------
			{
			strdcat( &cmd, "direct=recv ", _ );
			continue;
			}

		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-parm" ) )||( ! strcmp( vn, "-u" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -parm" BR, _ );
				r = 50;
				return ERROR;
				}
			strdcat( &cmd, "parm=", quote( vn ), " ", _ );
			continue;
			}

		//----------------------------------------------------------------------
		if (! strcmp( vn, "-etat" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -etat" BR, _ );
				r = 50;
				return ERROR;
				}
			strdcat( &cmd, "state=", quote( vn ), " ", _ );
			continue;
			}



		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-ida" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -ida" BR, _ );
				r = 50;
				return ERROR;
				}
			strdcat( &cmd, "ida=", quote( vn ), " ", _ );
			continue;
			}


		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-idt" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -idt" BR, _ );
				r = 50;
				return ERROR;
				}
			strdcat( &cmd, "idt=", quote( vn ), " ", _ );
			continue;
			}


		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-idtu" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe -idtu" BR, _ );
				r = 50;
				return ERROR;
				}
			strdcat( &cmd, "idtu=", quote( vn ), " ", _ );
			continue;
			}


		//----------------------------------------------------------------------
		if ( ! strnicmp( vn, "user", 4 ) )
		//----------------------------------------------------------------------
			{
			if ( (p=strpos( vn, "=", 4 )) > 0 )
				{
				if ( vn[p+1] )	continue;
				i++;
				if ( !ts[i] ) break;
				continue;
				}
			i++;
			vn = ts[i];
			if ( !vn ) break;
			if ( vn[0] != '=' ) continue;
			if ( vn[1] )	continue;
			i++;
			if ( !ts[i] ) break;
			continue;
			}
			
		//----------------------------------------------------------------------
		print( "ERREUR : Option inconnue : ", vn, BR, _ );
		r = 50;
		return ERROR;
		}

	free( ts ); ts = null;
	
	
	strdcat( &cmd, "of=ssie", _ );
	
	var_free( hReq );
	hReq = var_from_cmd( 0, cmd );
	cmd_mft_cat( t_work );
	
	v = u10; u10 = s4; s4 = v;
	if ( u10[1] )
	for( p6 = 0, l = strlen(u10) ; p6 < l ; p6 += 8 )
		{
		strdncpy( &ida, &(u10[p6]), 8, _ );

		//----------------------------------------------------------------------
		// CHARGE INFO SUR LE TRANSFERT
		//----------------------------------------------------------------------
		
		var_free( h );
		if ( (h = mftcat_load( ida )) == ERROR )
			continue;
		
/*
	strdcat( &cmd, "of='csv:adle;adlr;appe;appr;parm;status;direct;idie;"
			"dateb;timeb;datee;timee;fname;idt;spart;ida;"
			"file_size;rpart;errno;errmsg'", _ );
*/
		
		if ( !var_get( h, "status" ) ) continue;
		v1 = var_get( h, "dateb" );
		v2 = var_get( h, "datee" );
		
		if ( fp == NULL )
			print( substr(var_get( h, "sappl" ), 0, 8), ";",
				substr(var_get( h, "rappl" ), 0, 8), ";",
				substr(var_get( h, "sappl" ), 8, 8), ";",
				substr(var_get( h, "rappl" ), r, 8), ";",
				var_get( h, "parm" ), ";",
				var_get( h, "status" ), ";",
				var_get( h, "direct" ), ";",
				var_get( h, "ida" ), "001001;",
				substr(v1,6,4), "/", substr( v1, 3, 2 ), "/", 
					substr( v1, 0, 2 ), " ", var_get( h, "timeb" ), ";",
				substr(v2,6,4), "/", substr( v2, 3, 2 ), "/", 
					substr( v2, 0, 2 ), " ", var_get( h, "timee" ), ";",
				var_get( h, "fname" ), ";",
				var_get( h, "ida" ), ";",
				var_get( h, "spart" ), ";",
				var_get( h, "ida" ), ";",
				var_get( h, "file_size" ), ";",
				var_get( h, "rpart" ), ";",
				var_get( h, "errno" ), ";",
				var_get( h, "errmsg" ), BR,
				_ );
		else
			fprint( fp, substr(var_get( h, "sappl" ), 0, 8), ";",
				substr(var_get( h, "rappl" ), 0, 8), ";",
				substr(var_get( h, "sappl" ), 8, 8), ";",
				substr(var_get( h, "rappl" ), r, 8), ";",
				var_get( h, "parm" ), ";",
				var_get( h, "status" ), ";",
				var_get( h, "direct" ), ";",
				var_get( h, "ida" ), "001001;",
				substr(v1,6,4), "/", substr( v1, 3, 2 ), "/", 
					substr( v1, 0, 2 ), " ", var_get( h, "timeb" ), ";",
				substr(v2,6,4), "/", substr( v2, 3, 2 ), "/", 
					substr( v2, 0, 2 ), " ", var_get( h, "timee" ), ";",
				var_get( h, "fname" ), ";",
				var_get( h, "ida" ), ";",
				var_get( h, "spart" ), ";",
				var_get( h, "ida" ), ";",
				var_get( h, "file_size" ), ";",
				var_get( h, "rpart" ), ";",
				var_get( h, "errno" ), ";",
				var_get( h, "errmsg" ), BR,
				_ );
		}
		
	#ifdef OS_UNIX
	if ( fp )
		{
		fclose( fp );
		strdcpy( &s, "sudo ", dirbin, DIR_CHAR "ssie7 "
			"mv -f ", fname3, " ", fname2, _ );
		system( s );
		strdcpy( &s, "sudo ", dirbin, DIR_CHAR "ssie7 chown ", user, ":", user, " ", fname2, _ );
		system( s );
		print( "OK", _ );
		}
	#endif
	
	return OK;
	}


#define cat_fname	fname3


//------------------------------------------------------------------------------
int cmd_mersi_reception( T_WORK *t_work )

// COMMANDE MERSI_RECEPTION (REPRISE A L'IDENTIQUE DU FONCTIONNEMENT DU SCRIPT mersi_reception.ksh)
//------------------------------------------------------------------------------
	{
	/*
	#-------------------------------------------------------------------------------
	# Produit    : MERSI_RECEPTION
	# Version    : v3.1.01
	#
	# Fonction   : Réception d'un fichier transféré par MFT
	#              - le fichier est détruit de l'espace réseau 
	#              - le statut du transfert est passé à l'état exécuté
	#
	# Syntaxe    : mersi_reception.ksh  -(adl)e <adle>  -idt <idt>  -f <fichier>
	#
	# Parametres obligatoires:
	# <adle>     : Adresse logique stor émettrice ou partenaire émetteur
	# <idt>      : Identifiant MFT associé au transfert à recevoir
	#           OU identifiant MERSI associé au transfert à recevoir
	# <fichier>  : Nom du fichier sous lequel sera restitué le fichier transféré
	#
	# CRetour    : 0:Aucun fichier reçu, 1:Un fichier reçu, Autre:Erreur grave
	#-------------------------------------------------------------------------------
	*/


	//--------------------------------------------------------------------------
	// LECTURE DES PARAMETRES
	//--------------------------------------------------------------------------
	
	var_free( h1 );
	h1 = var_new();
	var_set( h1, "direct", "both" );

	
	ts = str_explode( cmd, " " );
	strdcpy( &cmd1, "mftcat status=WRT direct=RECV", _ );

	for( i = 1 ; vn = ts[i] ; i++ )
		{

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-f" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe" BR, _ );
				r = 50;
				return ERROR;
				}
			if (( vn[0] == '-' )||( !strnicmp( vn, "user", 4 ) ))
				{
				print( "ERREUR de syntaxe" BR, _ );
				r = 50;
				return ERROR;
				}
			
			var_set( h1, "fname", vn );
//			strdcat( &cmd1, " fname=", quote( vn ), _ );
			continue;
			}

		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-adle" ) )||( ! strcmp( vn, "-e" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe" BR, _ );
				r = 50;
				return ERROR;
				}
			var_set( h1, "adle", vn );
			strdcat( &cmd1, " sappl=", vn, "*", _ );
			continue;
			}


		//----------------------------------------------------------------------
		if (( ! strcmp( vn, "-ida" ) )||( ! strcmp( vn, "-idtu" ) ))
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe" BR, _ );
				r = 50;
				return ERROR;
				}
			var_set( h1, "ida", vn );
			strdcat( &cmd1, " ida=", vn, _ );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-idt" ) )
		//----------------------------------------------------------------------
			{
			i++;
			if ( ! (vn = ts[i]) )
				{
				print( "ERREUR de syntaxe" BR, _ );
				r = 50;
				return ERROR;
				}
			var_set( h1, "idt", vn );
			strdcat( &cmd1, " idt=", vn, _ );
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-keep" ) )
		//----------------------------------------------------------------------
			{
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strcmp( vn, "-del" ) )
		//----------------------------------------------------------------------
			{
			continue;
			}

		//----------------------------------------------------------------------
		if ( ! strnicmp( vn, "user", 4 ) )
		//----------------------------------------------------------------------
			{
			if ( (p=strpos( vn, "=", 4 )) > 0 )
				{
				if ( vn[p+1] )	continue;
				i++;
				if ( !ts[i] ) break;
				continue;
				}
			i++;
			vn = ts[i];
			if ( !vn ) break;
			if ( vn[0] != '=' ) continue;
			if ( vn[1] )	continue;
			i++;
			if ( !ts[i] ) break;
			continue;
			}
			

		//----------------------------------------------------------------------
		print( "ERREUR : Option inconnue : ", vn, BR, _ );
		r = 50;
		return ERROR;
		}


	if ( !( v3=var_get( h1, "fname" ) ) )
		{
		r = 50;
		print( "ERREUR : Option -f obligatoire" BR, _ );
		return ERROR;
		}
		
	if (( !( v2 = var_get( h1, "ida" ) ) )&&( !( v2 = var_get( h1, "idt" ) ) ))
		{
		r = 50;
		print( "ERREUR : Option -idt ou -idtu ou -ida obligatoire" BR, _ );
		return ERROR;
		}

	strdcat( &cmd1, " of=ssie", _ );

//print( "cmd1 = ", cmd1, BR, _ );

	var_free( hReq );
	hReq = var_from_cmd( 0, cmd1 );
	cmd_mft_cat( t_work );
	
	if ( s4[1] == 0 )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		r = 50;
		return ERROR;
		}
		

	if ( strlen( s4 ) > 8 )
		{
		print( "ERREUR : Plus d'un transfert qui correspond", _ );
		r = 50;
		return ERROR;
		}
	
	strdcpy( &ida, s4, _ );
	
	
	// CHARGE LES INFO SUR LE TRANSFERT

	
	var_free( h );
	if ( (h = mftcat_load( ida )) == ERROR )
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		r = 50;
		return ERROR;
		}

	v = var_get( h, "status" );
	if (( !v )||(( strcmp( v, "R" ) )&&( strcmp( v, "W" ) )&&( strcmp( v, "T" ) )))
		{
		print( "ERREUR : Aucun transfert éligible trouvé", _ );
		r = 50;
		return ERROR;
		}
	

		
	// DEPLACEMENT DU FICHIER
	
	strdcpy( &fname1, var_get( h, "_recep_fname" ), _ );
	strdcpy( &fname2, var_get( h1, "fname" ), _ );

	print( "file_move : ", fname1, " ==> ", fname2, BR, _ );

	#ifdef OS_UNIX
	if ( request_mode == MODE_TCP )
		{
		strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
				"mv -f ", fname1, " ", fname2, _ );
		System( &s3, s );
		if ( trim(s3)[0] )
			{
			print( "mersi_reception : ERREUR : mv -f ", fname1, " ", fname2, BR, s3, BR, _ );
			return ERROR;
			}
		
		strdcpy( &s, "sudo " LOGI_DIR "/ssie7/ssie7 "
				"chown -R ", user, ":", user, " ", fname2, _ );
		
		System( &s3, s );
		}
	else
	#endif
	
	for( b = false, n = time(NULL) ; file_move( fname1, fname2 ) == ERROR ; Sleep( 10 ) )
		{
		if ( ( !b )&&( ( time(NULL) - n ) > 5 ) ) // RETENTATIVE PENDANT 5 SECONDES
			{
//			print( "mersi_reception : PROBLEME file_move : ", error_msg, BR "On retente." BR, _ );
			mftcat_logw( h, "mersi_reception : file_move ", fname1, " => ", fname2, " : ", error_msg, _ );
			b = true;
			continue;
			}
		if ( ( time(NULL) - n ) > 10*60 ) // RETENTATIVE PENDANT 10 MINUTES
			{
//			print( "mersi_reception : ERREUR file_move : ", error_msg, BR, _ );
			mftcat_logw( h, "mersi_reception : file_move ", fname1, " => ", fname2, " : ", error_msg, _ );
			mftcat_upd( h, "T", null, null );
			return ERROR;
			}
		}

	// PASSE LE TRANSFERT A L'ETAT X
	
	mftcat_logw( h, "mersi_reception : file_move ", fname1, " => ", fname2, " : OK", _ );
	mftcat_upd( h, "X", null, null );
		
	r = 1;
	print( "OK", BR, _ );
	return OK;
	}



#undef cat_fname















#include "ssie7.h"

int mftutil_execute( T_WORK *t_work );

#define cat_fname			fname3

//------------------------------------------------------------------------------
int cmd_mftutil( T_WORK *t_work )

// EXECUTE LA SIMULTATION DES COMMANDES MFTUTIL DU PRODUIT MFT
//------------------------------------------------------------------------------
	{
	char **_ts;
	int _i;
	
	tr( "cmd_mftutil_1" );
	#ifdef OS_WINDOWS
	if ( strpos( cmd, "=//", 0 ) >= 0 )
		{
		tr( "cmd_mftutil_2" );
		strd_replace( &cmd, "=//", "=\\\\" );
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd );
		}
	#endif
	
	if ( strpos( cmd, "<mftutil>", 0 ) >= 0 )
		{
		tr( "cmd_mftutil_3" );
		_ts = str_explode( cmd, "<mftutil>" );
		for( _i = 1 ; _ts[_i] ; _i++ )
			{
			if ( _ts[_i][0] == 0 ) continue;
			tr( "cmd_mftutil_4" );
			if ( stripos( _ts[_i], "type=output", 0 ) < 0 )
				{
				tr( "cmd_mftutil_5" );
				// EXECUTE LA COMMANDE
				
//				trim( _ts[_i] );
				strdcpy( &cmd, _ts[_i], _ );
				
				var_free( hReq );
				hReq = var_from_cmd( 0, _ts[_i] );
				v = var_getn( hReq, 0, &vn );
				mftutil_execute( t_work );
				
				cmd[0] = 0;
				}
			else
				{
				// TYPE=OUTPUT : REDIRIGE LA SORTIE VERS LE FICHIER DEMANDE
				
				p1 = stripos( _ts[_i], "fname=", 0 );
				print_to_file_and_screen( v = trim(substr( _ts[_i], p1+6, -1 )) );
				}
			}
		
		free( _ts );
		
		tr( "cmd_mftutil_6" );
		return OK;
		}
		
	v = var_getn( hReq, 1, &vn );
	if ( !v )
		{
		tr( "cmd_mftutil_7" );
		
		// MODE CONSOLE

		strdcpy( &s1, argv[0], " mftutil", _ );
		
		if ( cmd ) free( cmd );
		cmd = alloc( 4096 );
	
		for( ; ; )
			{
			print( BR BR, "MFTUTIL > ", _ );
			cmd = realloc( cmd, 4096 );
	
			fgets( cmd, 4096, stdin );
	
			if ( trim( cmd )[0] == 0 ) continue;
			
			//------------------------------------------------------------------
			if ( strmicmp( cmd, "q|quit|exit" ) )
			//------------------------------------------------------------------
				{
				free( cmd );
				cmd = null;
				return OK;
				}
	
			//------------------------------------------------------------------
			
			var_free( hReq );
			
			hReq = var_from_cmd( 0, cmd );
			v = var_getn( hReq, 0, &vn );
			mftutil_execute( t_work );
			}
		}
	

	//--------------------------------------------------------------------------
	if (( vn[0] == '#' )||( vn[0] == '@' ))
	//--------------------------------------------------------------------------
		{
		tr( "cmd_mftutil_8" );
		
		// LECTURE D'UN FICHIER COMMANDE OU UNE SERIE DE COMMANDE DANS UN SEUL PARAMETRE
		
		if ( vn[1] )
			{
			tr( "cmd_mftutil_9" );
			if ( s1 ) s1[0] = 0;
			file_load( &s1, &(vn[1]) );
			v1 = s1;
			}
		else
			v = var_getn( hReq, 2, &v1 );
		
		
		
		// EXECUTION COMMANDE PAR COMMANDE (UNE COMMANDE PAR LIGNE)

tr2( "cmd_mftutil_9a : ", v1 );
		
		p5 = 0;
		if ( cmd ) cmd[0] = 0;
		while( sgets( &cmd, v1, &p5 ) )
			{
			tr( "cmd_mftutil_10" );
tr2( "cmd_mftutil_10a : ", cmd );
			if ( stripos( cmd, "type=output", 0 ) < 0 )
				{
				tr( "cmd_mftutil_11" );
				// EXECUTE LA COMMANDE
				
				trim( cmd );
				
				var_free( hReq );
				hReq = var_from_cmd( 0, cmd );
				v = var_getn( hReq, 0, &vn );
tr2( "cmd_mftutil_11a : dirdat=", dirdat );
tr2( "cmd_mftutil_11b : cmd=", cmd );
				mftutil_execute( t_work );
				
				cmd[0] = 0;
				}
			else
				{
				// TYPE=OUTPUT : REDIRIGE LA SORTIE VERS LE FICHIER DEMANDE
				
				p1 = stripos( cmd, "fname=", 0 );
				print_to_file_and_screen( v = trim(substr( cmd, p1+6, -1 )) );
				}
			}
		
		tr( "cmd_mftutil_12" );
		return OK;
		}

	mftutil_execute( t_work );
	
	tr( "cmd_mftutil_13." );
	}


//------------------------------------------------------------------------------
int cmd_mftmsg( T_WORK *t_work )
//------------------------------------------------------------------------------
	{
	tr( "cmd_cfmsg_1" );
	v = var_get( hReq, "msg" );
	if ( ! v )
		{
		tr( "cmd_cfmsg_2" );
		print( "ERREUR : paramètre msg obligatoire" BR, _ );
		return ERROR;
		}
	
	vn = "send";
	var_set( hReq, "type", "message" );
	var_set( hReq, "idm", "message" );
	
	strd_replace( &cmd, "mftmsg", "mft send type=message,idm=message," );
	
	tr( "cmd_cfmsg_3." );
	return mftutil_execute( t_work );
	}
	
	
//------------------------------------------------------------------------------
int mftutil_execute( T_WORK *t_work )
	{
	tr( "mftutil_execute_1" );

	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "about" ) )
	//--------------------------------------------------------------------------
		{
		tr( "mftutil_execute_35" );
		if ( !mft_exists() )
			print(
				"    ", product_name,  BR
				BR
				, _ );
		else
			{
			tr( "mftutil_execute_36" );
			strdcpy( &s, argv[0], " mft about", _ );
			System( &s1, s );
			print( s1, BR, _ );
			}

		return OK;
		}

	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "listcat" ) )
	//--------------------------------------------------------------------------
		{
		tr( "mftutil_execute_14" );
		
		b1 = ( ( v = var_get( hReq, "content" ) )&&( !stricmp( v, "full" ) ) );
		b2 = ( ( v = var_get( hReq, "force_status_t" ) )&&( !stricmp( v, "true" ) ) );

		p = stripos( cmd, "listcat", 0 );
		strdcpy( &cmd1, "mftcat ", &(cmd[p+7]), _ );

		v5 = var_get( hReq, "of" );
		if ( !v5 )	strdcat( &cmd1, ", of=ssie", _ );
		
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd1 );
		if ( b1 ) var_delete( hReq, "content" );
		
		tr( "mftutil_execute_15" );
		
		cmd_mft_cat( t_work );

		if ( v5 ) return OK;
		
		tr( "mftutil_execute_16" );
		
		// s4 contient la liste des ida.

		if ( ! b1 )
			{
			tr( "mftutil_execute_17" );
			
			// AFFICHAGE TETE DE LISTE
			print(	"  Date = ", currdate( "jj/mm/aaaa" ), "   Time = ", currdate( "hh:mm:ss" ), BR
					BR
					"  Partner  DTSA File     Transfer         Records       Diags        Appli." BR
					"                Id.      Id.       Transmit     Total   MFT Protocol Id." BR
					"  -------- ---- -------- -------- ---------- ---------- --- -------- --------" BR
				, _ );
			}
		
		v = u10; u10 = s4; s4 = v;
		n = strlen(u10) / 8;
		if ( u10[1] )
		for( p6 = 0, l = strlen(u10) ; p6 < l ; p6 += 8 )
			{
			tr( "mftutil_execute_18" );
			
			strdncpy( &ida, &(u10[p6]), 8, _ );
	
			//----------------------------------------------------------------------
			// CHARGE INFO SUR LE TRANSFERT
			//----------------------------------------------------------------------
			
			var_free( h );
			if ( (h = mftcat_load( ida )) == ERROR )
				{
				n--;
				continue;
				}
	
			tr2( "mftutil_execute_19:", ida );
			
			// AFFICHE MODE LISTE
			
			if ( ! b1 )
				{
				tr( "mftutil_execute_20" );
/*
  MERSISUQ SFT  R2MMSB   D2409210        123        123   0 CP 03%   ND00000O			
*/				
				v1 = var_get( h, "part" );
				if ( !v1 )
					{
					n--;
					continue;
					}
				v1[8] = 0;
				if ( b2 ) var_set( h, "status", "T" );
				
				v2 = var_get( h, "direct" );
				if ( v2[0] == 'S' ) v2 = "S"; else v2 = "R";
				v3 = var_get( h, "type" );
				if ( ( v3 )&&( !stricmp( v3, "message" ) ) )
					print( "  ", strpad(v1, 8, " "),
							" ", v2, v3="M", var_get( h, "status" ), "  ",
							strpad(strpad(var_get( h, "idm" ), 8, " "), 8, " "),
							_ );
				else
					print( "  ", strpad(v1, 8, " "),
							" ", v2, v3="F", var_get( h, "status" ), "  ",
							strpad(strpad(var_get( h, "idf" ), 8, " "), 8, " "),
							_ );
				

				v = var_get( h, "ida" );
				print( " ", strpad(v, 8, " "), _ );
				
				v = var_get( h, "dt_upd" ); // aaaa_mm_jj_hh_mm_ss
				strdcpy( &s3, substr( v, 8, 2 ), "/", substr( v, 5, 2 ),
						"-", substr( v, 11, 2 ), substr( v, 14, 2 ), _ );
				print( " ", s3, _ );
				
				if ( v3[0] == 'F' )
					print( " ", strlpad(substr(var_get(h,"file_size"), 0, 10 ), 10, " "), " ", _ );
				else
					print( strpad(substr(var_get(h,"msg"), 0, 12 ), 12, " "), _ );

				print( strlpad(substr(itoa(atoi(var_get(h,"errno"))), 0, 3 ), 3, " "), _ );
				print( " ", strlpad(substr(var_get(h,"errmsg"), 0, 8 ), 8, " "), _ );
				
				if ( v2[0] == 'R' )
					print( " ", var_get( h, "sida" ), _ );
				else
					print( " ", var_get( h, "rida" ), _ );
				
				if ( v = var_get( h, "idt_mft" ) )
					print( "/", v, _ );
				
				print( BR, _ );
				}
			
			// AFFICHAGE MODE CONTENT = FULL
			else
				{
				tr( "mftutil_execute_21" );
				
				v7 =
				
				"  Transfer id.                      IDT      = [$idt]" BR
				"  Local transfer id.                IDTU     = [$ida]" BR
				"  Network transfer id.              NIDT     = 1509210" BR
				"  Block number                      BLKNUM   = 1062" BR
				BR
				"     Direct                         DIRECT   = [$direct]" BR
				"     Type                           TYPE     = [$type]" BR
				"     Priority                       PRI      = 128" BR
				BR
				"     State                          STATE    = [$status]" BR
				"     Local diagnosis                DIAGI    = [$diagi]" BR
				"     Protocol diagnosis             DIAGP    = [$error_msg]" BR
				BR
				"     Item type                      DIFTYP   = SINGLE" BR
				"     For partner list : id.         DEST     =" BR
				"                        In progress LNUM     = 0" BR
				BR
				"     Remote transfer id.            IDTD     =" BR
				"     Remote transfer state          STATED   =" BR
				BR
				"     Logical file id                IDF      = [$idf]" BR
				"     Network file id.               NIDF     = [$nidf]" BR
				BR
				"     File data base                 FDBNAME  =" BR
				"     *" BR
				"     File name                      FNAME    =" BR
				"     *[$fname]" BR
				"     Temporary file name            WFNAME   =" BR
				"     *" BR
				"     Local Select list              SELFNAME =" BR
				"     *" BR
				"     Remote file name               NFNAME   =" BR
				"     *" BR
				"     Signature file name            SIGFNAME =" BR
				"     *" BR
				"     File version                   NFVER    = 0" BR
				BR
				"     Private parameter              PARM     =" BR
				"     *[$parm]" BR
				"     *" BR
				BR
				"     Application id.                IDA      =" BR
				"     *[$ida]" BR
				BR
				"     Partner                        PART     = [$part]" BR
				"     Sender                         SPART    = [$spart]" BR
				"     Receiver                       RPART    = [$rpart]" BR
				"     Intermediate partner           IPART    = [$part]" BR
				"     Connection partner             NPART    = [$part]" BR
				BR
				"     Partner call time                    Command deposure" BR
				"     -----------------                    ----------------" BR
				"     After        TIMMC    = 00:00:00.00  Date     DATED    = [$dateb]" BR
				"     Before       TIMMAXC  = 23:59:59.99  Time     TIMED    = [$timeb].00" BR
				BR
				"     Transfer validity                    Activation time" BR
				"     -----------------                    ---------------" BR
				"     After        MINDATE  = [$date]   Before   CYCDATE  =" BR
				"                  MINTIME  = 00:00:00.00  Time     CYCTIME  = 00:00:00.00" BR
				"     Before       MAXDATE  = 31/12/9999   Each     CYCLE    = 0" BR
				"                  MAXTIME  = 23:59:59.99  Unit     TCYCLE   = DAYS" BR
				BR
				"     Next retry date                NEXTDAT  = [$date]" BR
				"     Next retry hour                NEXTTIM  = 00:00:00.00" BR
				BR
				"                         Transfer" BR
				"                         --------" BR
				BR
				"     Begin date   DATEB    = [$dateb]   End date DATEE    = [$datee]" BR
				"     Begin time   TIMEB    = [$timeb]  End time TIMEE    = [$timee]" BR
				BR
				"     Connection retry                     Connection status" BR
				"     ----------------                     -----------------" BR
				"     Current      RETRY    = 0" BR
				"     Max number   RETRYN   = 0            Restart  :" BR
				"     Max number   RETRYM   = 0             Allowed (YES/NO) = NO" BR
				"     Time between RETRYW   = 0             Current          = 1" BR
				"     Last dial nb DIALNUM  = 0" BR
				BR
				"     Transmitted                                   To transmit" BR
				"     -----------                                   -----------" BR
				"     Records      NRECS    = 123                   Records  NREC     = 123" BR
				BR
				"     Bytes        NCARS    = [$file_size]" BR
				BR
				"     Read or Written" BR
				"     ---------------" BR
				"     Records      FRECS    = 123" BR
				"     Bytes        FCARS    = [$file_size]" BR
				BR
				"     Synchronization" BR
				"     ---------------" BR
				"     Nb of checkpoints              NCHKPT   = 1" BR
				"     Nb of resynchro.               NRST     = 0" BR
				BR
				"     User id.                       USERID   = SSIE" BR
				"     Group id.                      GROUPID  =" BR
				"     Origin owner transfer          ORIGIN   = LOCAL" BR
				"     Requester user                 REQUSER  = SSIE" BR
				"     APPL identifier used           APPL     =" BR
				"     Translation table              XLATE    =" BR
				"     Exec name                      EXEC     =" BR
				"     *" BR
				BR
				"     Exit id.                       EXIT     =" BR
				BR
				"     Exita id.                      EXITA    =" BR
				BR
				"     Exite id.                      EXITE    =" BR
				BR
				"     Exiteot id.                    EXITEOT  =" BR
				BR
				"     Partner Group                  GROUP    =" BR
				BR
				"     Job name                       JOBNAME  = 15480" BR
				BR
				"     Sender user                    SUSER    = [$suser]" BR
				"            password                SPASSWD  =" BR
				"     Receiver user                  RUSER    = [$ruser]" BR
				"            password                RPASSWD  =" BR
				"     Sender application             SAPPL    =" BR
				"     *[$sappl]" BR
				"     Receiver application           RAPPL    =" BR
				"     *[$rappl]" BR
				BR
				"    Security informations" BR
				"    ---------------------" BR
				"    SSL Mode                       SSLMODE  =" BR
				"    SSL Auth Policy                SSLAUTH  =" BR
				"    SSL Cipher Type                SSLCIPH  =" BR
				"    SSL Profil Id.                 SSLPROF  =" BR
				"    SSL Remote CA Alias.           SSLRMCA  =" BR
				"    *" BR
				"    SSL Local User Alias.          SSLUSER  =" BR
				"    *" BR
				"    SSL Cert. Filename.            SSLCFNAM =" BR
				"    *" BR
				"    SSL User Parameter             SSLPARM  =" BR
				"    *" BR
				"    Comments                       MSG      =" BR
				"    *[$msg]" BR
				BR
				"    Network Partner  informations" BR
				"    -----------------------------" BR
				"    Network sender   partner       NSPART   =" BR
				"    Network receiver partner       NRPART   = [$rpart]" BR
				
				"    Protocol Partner  informations" BR
				"    ------------------------------" BR
				"    Protocol Id                    PROT     = PSITETCP" BR
				"    Protocol Type                  TYPE     = PESIT" BR
				"    Profil   Type                  PROFIL   = ANY" BR
				"    Synchronization interval       PACING   = 1024" BR
				"    Synchronization window         CHKW     = 3" BR
				"    Resynchronization              RESYNC   = NO" BR
				"    Compression negotiated         COMPNEG  = 15" BR
				"    Network message size           RUSIZE   = 32750" BR
				BR;

				// AFFICHAGE FULL DES EMISSIONS
			
				strdcpy( &s4, v7, _ );
				
				v = var_get( h, "errno" );
				if ( ( !v )||( !(v[0]) ) ) v = "0";
				strd_replace( &s4, "[$diagi]", v );
				n2 = atoi( v );
				
				v4 = var_get( h, "status" );
				if ( !v4 ) v4 = "C";
				switch( v4[0] )
					{
					case 'C' : v3 = "C"; break;
					case 'D' : v3 = "MFT"; break;
					case 'K' : v3 = "KEEP"; break;
					case 'X' : v3 = "END"; break;
					case 'W' : v3 = "WAIT"; break;
					case 'T' : v3 = "TERM"; break;
					case 'R' : v3 = "REVEIL"; break;
					default : v3 = v4;
					}
					
				strd_replace( &s4, "[$status]", v3 );
					


				n2 = file_date( fname );
//				strd_replace( &s4, "[$date]", itod( n2, "jj/mm/aaaa" ) );
//				strd_replace( &s4, "[$time]", itod( n2, "hh:mm:ss" ) );
				strd_replace( &s4, "[$dateb]", var_get( h, "dateb" ) );
				strd_replace( &s4, "[$timeb]", var_get( h, "timeb" ) );
				strd_replace( &s4, "[$datee]", var_get( h, "datee" ) );
				strd_replace( &s4, "[$timee]", var_get( h, "timee" ) );
				strd_replace( &s4, "[$direct]", var_get( h, "direct" ) );

				if ( ! ( v1 = var_get( h, "type" ) ) )
					v1 = "FILE";
				strd_replace( &s4, "[$type]", v1 );
				
				if ( v = var_get(h,"ida") )
					strd_replace( &s4, "[$ida]", v );
				else
					strd_replace( &s4, "[$ida]", substr(tf->name, 0, 8 ) );
				
				if ( v1 = var_get(h,"idt_mft") )	
					strd_replace( &s4, "[$idt]", v1 );
				else
					strd_replace( &s4, "[$idt]", v );
				

				strd_replace( &s4, "[$error_msg]", var_get( h, "errmsg" ) );
				strd_replace( &s4, "[$msg]", var_get( h, "msg" ) );

				v1 = var_get( h, "idf" );
				if ( !v1 )	v1 = ( var_get( h, "ascii" ) ) ? "R2MMS" : "R2MMSB";

				strd_replace( &s4, "[$idf]", v1 );

				v = var_get( h, "nidf" );
				if ( ( !v )||( !(v[0]) ) ) v = v1;
				strd_replace( &s4, "[$nidf]", v );
				
				strdcpy( &fname1, var_get( h, "fname" ), _ );
				
				#ifdef OS_WINDOWS
				if ( !file_exists( fname1 ) )
					{
					tr( "mftutil_execute_22" );
					
					strdcpy( &fname1, "d:\\r2000\\reseau\\", substr(var_get( h, "rappl" ), 0, 8),
							DIR_CHAR, substr(var_get( h, "sappl" ), 0, 8),
							DIR_CHAR, substr(var_get( h, "suser" ), 0, 8),
							_ );
					if ( !file_exists( fname1 ) )
						{
						tr( "mftutil_execute_23" );
						
						strdcpy( &fname1, "r:\\reseau\\", substr(var_get( h, "rappl" ), 0, 8),
								DIR_CHAR, substr(var_get( h, "sappl" ), 0, 8),
								DIR_CHAR, substr(var_get( h, "suser" ), 0, 8),
								_ );

						if ( !file_exists( fname1 ) )
							strdcpy( &fname1, var_get( h, "fname" ), _ );
						}
					}

				#endif
				
				strd_replace( &s4, "[$fname]", fname1 );
				strd_replace( &s4, "[$parm]", var_get( h, "parm" ) );
				strd_replace( &s4, "[$part]", var_get( h, "partname" ) );
				strd_replace( &s4, "[$spart]", var_get( h, "spart" ) );
				strd_replace( &s4, "[$rpart]", var_get( h, "rpart" ) );
				strd_replace( &s4, "[$file_size]", var_get( h, "file_size" ) );
				strd_replace( &s4, "[$suser]", upper(var_get( h, "suser" )) );
				strd_replace( &s4, "[$ruser]", upper(var_get( h, "ruser" )) );
				strd_replace( &s4, "[$sappl]", upper(var_get( h, "sappl" )) );
				strd_replace( &s4, "[$rappl]", upper(var_get( h, "rappl" )) );
				
				print( s4, BR, _ );
				}
			}
		
		// AFFICHAGE FIN
		
		if ( n < 0 ) n = 0;
		n2 = ((n/20000)+1)*20000;
		print(	BR
				"  ", strlpad( itoa(n), 8, " " ), " record(s) selected", BR
				"  ", strlpad( itoa(n2), 8, " " ), " record(s) in Catalog file", BR
				"  ", strlpad( itoa(n2-n), 8, " " ), " record(s) free (", itoa(((n2-n)*100)/n2), "%)", BR
				BR "MFTU00I LISTCAT  _ Correct ()" BR
				, _ );

		
		tr( "mftutil_execute_23" );
		return OK;
		}

	
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "send" ) )
	//--------------------------------------------------------------------------
		{
		// part, fname, nfname, idf, nidf, flrecl, frecfm, fblksize, nspace,
		// suser, sappl, ruser, rappl
		// TRANSFORME UN "MFTUTIL SEND" EN "SSIE7 MFT SEND" ET EXECUTE
		tr( "mftutil_execute_2" );
		
		v = var_get( hReq, "fname" );
		if ( ( !v )&&
			(( v1 = var_get( hReq, "type" ) )&&( !stricmp( v1, "message" ) )
			||( var_get( hReq, "msg" )	))
			)
			{
            tr( "mftutil_execute_3" );
			if ( strlen( var_get( hReq, "msg" ) ) == 0 )
				{
				print( "ERREUR : message vide" BR, _ );
        		return ERROR;
				}

			tr( "mftutil_execute_4" );
			
			v = str( dirwrk, DIR_CHAR "vide.txt", _ );
			file_save_str( v, "", 0 );
			var_set( hReq, "fname", v );
			var_set( hReq, "nocopy", "true" );
			if ( !v1 )	var_set( hReq, "type", "message" );
			}
			

		strdcpy( &cmd, "mft send part=", v1=var_get( hReq, "part" ), " fname=", v, _ );

		if ( !v1 )
			{
			print( "ERREUR : paramètre part obligatoire" BR, _ );
			return ERROR;
			}
		tr( "mftutil_execute_5" );
		if ( !v )
			{
			print( "ERREUR : paramètre fname obligatoire" BR, _ );
			return ERROR;
			}
		
		tr( "mftutil_execute_6" );	
			
		for( i = 1 ; v = var_getn( hReq, i, &vn ) ; i++ )
			{
			if ( !stricmp( vn, "part" ) ) continue;
			if ( !stricmp( vn, "fname" ) ) continue;
			if ( !stricmp( vn, "nfname" ) ) continue;
			if ( !stricmp( vn, "idf" ) )
				{
				if (( v[n=(strlen(v)-1)] != 'B' )&&( v[n] != 'b' ))
					strdcat( &cmd, " ascii", _ );
				}
			strdcat( &cmd, " ", vn, "=", quote(v), _ );
			}
		
		
		if ( ( v = var_get( hReq, "state" ) )&&( !stricmp( v, "hold" ) ) )
			strdcat ( &cmd, " publish", _ );
			
		tr( "mftutil_execute_7" );
		
		// EXECUTE LE SEND
		
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd );	
		log_id = 0;
		
		cmd_mft( t_work );
		
		
		// FIN DU SEND
		
		print( "MFTU00I SEND     _ Correct ()" BR, _ );
		
		tr( "mftutil_execute_8" );
		return OK;
		}
	
/*
	#ifdef OS_UNIX
	if ( strcmp( sysuser(), ssie_user ) )
		{
		tr( "mftutil_execute_8" );
		
		var_to_str( hReq, &cmd );

		v = var_get( hSrvConf, "port" );
		if ( ( !v )||( !(v[0]) ) )
			{
			tr( "mftutil_execute_9" );
			
			System( &s, "ps -ef | grep \"ssie7 server \" | grep -v grep" );
			p = strpos( s, "ssie7 server ", 0 );
			if ( p >= 0 )
				v = itoa( atoi( &(s[p+13]) ) );
			}

		strdcpy( &s, "127.0.0.1:", v, _ );
		v = tcp_request( s, cmd, &s );
		if ( ! v )
			print( "ERREUR : ", error_msg, _ );
		else
			print( v, _ );
		
		tr( "mftutil_execute_10" );	
		return OK;
		}
	#endif
*/	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "recv" ) )
	//--------------------------------------------------------------------------
		{
		tr( "mftutil_execute_11" );
		
		// TRANSFORME UN "MFTUTIL RECV" EN "SSIE7 RECV" ET EXECUTE
		
		strdcpy( &cmd, "mft recv part=", v1=var_get( hReq, "part" ), " idf=", v = var_get( hReq, "idf" ), _ );
		if ( !v1 )
			{
			print( "ERREUR : paramètre part obligatoire" BR, _ );
			return ERROR;
			}
			
		tr( "mftutil_execute_12" );
		
		// EXECUTE LE RECV
		
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd );	
		log_id = 0;
		
		cmd_mft( t_work );
		
		
		// FIN DU RECV
		
		print( BR "MFTU00I RECV     _ Correct ()" BR, _ );
		
		tr( "mftutil_execute_13" );
		return OK;
		}

	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "delete" ) )
	//--------------------------------------------------------------------------
		{
		tr( "mftutil_execute_24" );
		
		// TRANSFORME UN "MFTUTIL DELETE" EN "SSIE7 MFT DELETE" ET EXECUTE
		
		p = stripos( cmd, "delete", 0 );
		strdcpy( &cmd1, "mft ", &(cmd[p]), _ );
		v = cmd; cmd = cmd1; cmd1 = v;

		
		// EXECUTE LE DELETE
		
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd );	
		log_id = 0;
		
		cmd_mft( t_work );
		
		print( BR "MFTU00I DELETE   _ Correct ()" BR, _ );
		
		tr( "mftutil_execute_25" );
		return OK;
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "start" ) )
	//--------------------------------------------------------------------------
		{
		tr( "mftutil_execute_26" );
		
		// TRANSFORME UN "MFTUTIL START" EN "SSIE7 MFT RETRY" ET EXECUTE
		
		p = stripos( cmd, "start", 0 );
		strdcpy( &cmd1, "mft retry ", &(cmd[p+5]), _ );
		v = cmd; cmd = cmd1; cmd1 = v;

		
		// EXECUTE
		
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd );	
		log_id = 0;
		
		cmd_mft( t_work );
		
		print( BR "MFTU00I START    _ Correct ()" BR, _ );
		
		tr( "mftutil_execute_27" );
		return OK;
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "end" ) )
	//--------------------------------------------------------------------------
		{
		tr( "mftutil_execute_28" );
		
		// TRANSFORME UN "MFTUTIL END" EN "SSIE7 MFT SETX" ET EXECUTE
		
		p = stripos( cmd, "end", 0 );
		strdcpy( &cmd1, "mft setx ", &(cmd[p+3]), _ );
		v = cmd; cmd = cmd1; cmd1 = v;

		
		// EXECUTE
		
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd );	
		log_id = 0;
		
		cmd_mft( t_work );
		
		print( BR "MFTU00I END      _ Correct ()" BR, _ );
		
		tr( "mftutil_execute_29" );
		return OK;
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "submit" ) )
	//--------------------------------------------------------------------------
		{
		tr( "mftutil_execute_30" );
		
		// TRANSFORME UN "MFTUTIL SUBMIT" EN "SSIE7 MFT SETW" ET EXECUTE
		
		p = stripos( cmd, "submit", 0 );
		strdcpy( &cmd1, "mft setw ", &(cmd[p+6]), _ );
		v = cmd; cmd = cmd1; cmd1 = v;

		
		// EXECUTE
		
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd );	
		log_id = 0;
		
		cmd_mft( t_work );
		
		print( BR "MFTU00I SUBMIT   _ Correct ()" BR, _ );
		
		tr( "mftutil_execute_31" );
		return OK;
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "cancel" ) )
	//--------------------------------------------------------------------------
		{
		tr( "mftutil_execute_32" );
		
		// TRANSFORME UN "MFTUTIL CANCEL" EN "SSIE7 MFT CANCEL" ET EXECUTE
		
		p = stripos( cmd, "cancel", 0 );
		strdcpy( &cmd1, "mft ", &(cmd[p]), _ );
		v = cmd; cmd = cmd1; cmd1 = v;

		
		// EXECUTE
		
		var_free( hReq );
		hReq = var_from_cmd( 0, cmd );	
		log_id = 0;
		
		cmd_mft( t_work );
		
		print( BR "MFTU00I CANCEL   _ Correct ()" BR, _ );
		
		tr( "mftutil_execute_33" );
		return OK;
		}
	
	
	//--------------------------------------------------------------------------
	if ( !stricmp( vn, "wait" ) )
	//--------------------------------------------------------------------------
		{
		tr( "mftutil_execute_34" );
		
		// ATTEND LE NOMBRE DE SECONDES DEMANDE
		
		n = atoi( var_get( hReq, "during" ) );
		sleep( n );
		return OK;
		}
		
	
	tr( "mftutil_execute_37." );
	
	print( "ERREUR MFTUTIL : Commande inconnue." BR, _ );
	return ERROR;
	}
	


#undef cat_fname



















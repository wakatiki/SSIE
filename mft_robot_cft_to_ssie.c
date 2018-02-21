
//------------------------------------------------------------------------------
int robot_mft_to_ssie( T_WORK* t_work )
//------------------------------------------------------------------------------
	{
	// SURVEILLE LES FICHIERS RECU PAR MFT A TRANSMETTRE PAR SSIE V7
	
	strdcpy( &module_name, "mft_robot", _ );
	strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot_mft_to_ssie.pid", _ );
	if ( file_load( &s, fname ) != sERROR )
		{
		if ( process_is_alive( atoi(s) ) )
			exit(0);
		}
	file_save_str( fname, itoa(getpid()), -1 );
		
	for( n5 = 1 ; ; sleep(3), n5++ )
		{
		if ( ! (n5%(20*20)) ) exit(0); // Redémarrage toutes les 20 minutes

		strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot_mft_to_ssie.pid", _ );
		file_save_str( fname, itoa(getpid()), -1 );
		
		//----------------------------------------------------------------------
		// PARCOURS LA LISTE DES TRANSFERTS EN RECEPTION
		//----------------------------------------------------------------------

		strdcpy( &cmd, mft_dirbin, DIR_CHAR "MFTUTIL" EXE
					" listcat content=full,direct=RECV | grep IDTU", _ );
		System( &s4, cmd ); 

		p = 0;
		while( sgets( &s, s4, &p ) )
			{
			p1 = strpos( s, "IDTU", 0 );
			if ( p1 < 0 ) continue;
			p1 = strpos( s, "=", p1+4 );
			if ( p1 < 0 ) continue;
			strdcpy( &sid, &(s[p1+1]), _ );
			trim( sid );
			
			strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot_mft_to_ssie.pid", _ );
			file_save_str( fname, itoa(getpid()), -1 );
			
			// CHARGE LES INFO MFT DU TRANSFERT
			var_free( h );
			h = mft_idtu_load( sid );
			if ( h == ERROR )
				{
				strdcpy( &fname, mft_dirrecv, DIR_CHAR, sid, _ );
				if (( file_exists( fname ) )
					&&( ( time(NULL) - file_date( fname ) ) >= (24 Heures) )
						// Fichier vieux de + de 1 jour
					)
					{
					logw( "mft_to_ssie : ", sid, " : Pb lecture du transfert : rename ", fname, " => ", fname1, _ );
					strdcpy( &fname1, fname, ".error", _ );
					file_move( fname, fname1 );
					}
				else
					logw( "mft_to_ssie : ", sid, " : Pb lecture du transfert", _ );
				continue;
				}
			
        	
			// PURGE LES ERREURS 610 ET 660
			
			n = atoi( var_get(h, "DIAGI") );
			if ( n == 610 || n == 660 || n == 405 )
				{
				strdcpy( &cmd, "delete part=*,idtu=", sid, _ );
				mftutil( cmd, &s );
				logw( "mft_to_ssie : ", sid, " : diagi=", itoa(n), " : purge", _ );
				continue;
				}
			
			
			// ON NE RETIENT QUE LES TRANSFERTS TERMINES SANS ERREUR
			
			v = var_get(h, "STATE");
			if ( (strcmp( v, "TERM" )) || (n != 0) )
				{
				if ( ( !strcmp( v, "KEEP" ) )||( n != 0 ) )
					{
					strdcpy( &cmd, "delete part=*,idtu=", sid, _ );
					mftutil( cmd, &s );
					logw( "mft_to_ssie : ", sid, " : state=", v, ", diagi=", itoa(n), " : purge", _ );
					}
				continue;
				}


			// PURGE LES REPLY
			
			n = atoi( var_get(h, "DIAGI") );
			if ( !strcmp( var_get( h, "type" ), "REPLY" ) )
				{
				strdcpy( &cmd, "delete part=*,idtu=", sid, _ );
				mftutil( cmd, &s );
				logw( "mft_to_ssie : ", sid, " : type=REPLY : purge", _ );
				continue;
				}

        	
			// VERIFIE SI LE TRANSFERT EST POUR NOUS

logw( "recv MFT ", sid, " : ", var_to_str( h, &s ), _ );
			
			v4 = var_get( h, "NRPART" );
			partname  = var_get( h, "RPART" );
			
			if ( ( v4[0] )&&( stricmp( var_get( hSrvConf, "partname" ), v4 ) ) )
				partname = v4;
			else
			if ( !stricmp( "LOCAL", partname ) )
				{
				//--------------------------------------------------------------
				// LE TRANSFERT ET VRAIMENT POUR NOUS
				//--------------------------------------------------------------
					
				// CREATION D'UN ELEMENT DANS MFTCAT
				
				strdcpy( &fname, dirdat, DIR_CHAR "mftcat" DIR_CHAR "ida.id", _ );
				n = get_uid( fname );
				strdcpy( &ida, ito36( n, 8 ), _ );
				ida[0] = 'A' + year(sysdate) - 2013;
				if ( (n=month(sysdate)) < 10 )
					ida[1] = '0' + n;
				else
					ida[1] = 'A' + n - 10;
				ida[2] = ito36( day(sysdate), 1 )[0];
			
				var_set( h, "direct", "RECV" );
				var_set( h, "ida", ida );
				var_set( h, "rida", ida );
				var_set( h, "sida", sid );
				var_set( h, "idt_mft", var_get( h, "idt" ) );
				var_set( h, "recep_fname", var_get( h, "fname" ) );
				
				mftcat_logw( h, "Réception MFT : OK", _ );
				mftcat_crea( h, "W", "0", "OK" );
				
				logw( "mft_to_ssie : ", sid, " : transfert pour nous => Ajout dans mftcat en réception : ida=", ida, _ );

				// Supprime l'élément du catalogue MFT
				strdcpy( &cmd, "delete part=*,idtu=", sid, _ );
				mftutil( cmd, &s );
				
				continue;
				}

			if ( ( !partname )||( !(partname[0]) )
//				||( !(v = var_get( h, "fname" )) )||( !(v[0]) )
				)
				{
				logw( "mft_to_ssie : ", sid, " : transfert non exploitable => suppression", _ );
				strdcpy( &cmd, "delete part=*,idtu=", sid, _ );
				mftutil( cmd, &s );
				continue;
				}
			
			//--------------------------------------------------------------
			// TRANSMET LE FICHIER AU VRAI DESTINATAIRE
			//--------------------------------------------------------------

			// SOUMISSION DU TRANSFERT SSIE
			
			strdcpy( &cmd, "mft send part=", partname,
				" nocopy ssie_mandatory", // Le partenaire doit obligatoirement être un SSIE7
				" type=", quote( var_get( h, "type" ) ),
				" spart=", quote( var_get( h, "spart" ) ),
				" nfname=", quote( var_get( h, "nfname" ) ),
				" parm=", quote( var_get( h, "parm" ) ),
				" sappl=", quote( var_get( h, "sappl" ) ),
				" rappl=", quote( var_get( h, "rappl" ) ),
				" suser=", quote( var_get( h, "suser" ) ),
				" ruser=", quote( var_get( h, "ruser" ) ),
				" sida=",  sid,
				" idt_mft=",  quote( var_get( h, "idt" ) ),
				" dateb=",  quote( var_get( h, "dateb" ) ),
				" timeb=",  quote( var_get( h, "timeb" ) ),
				_ );
				
			if ( !strcmp( var_get( h, "type" ), "FILE" ) )
				{
				strdcat( &cmd, " fname=", var_get( h, "fname" ), " faction=delete",
					" idf=", quote( var_get( h, "nidf" ) ),
					_ );
				if ( strmicmp( var_get( h, "nidf" ), "R2MMS|R2MFS|R2IS|R2FE|R2FC|R2FR|R2MMA|R2MFA|R2IA" ) )
					strdcat( &cmd, " ascii", _ );
				}
			else
				strdcat( &cmd, " fname=", dirwrk, DIR_CHAR "vide.txt",
					" idm=", quote( var_get( h, "idm" ) ),
					" msg=", quote( var_get( h, "msg" ) ),
					_ );
				
			
			file_tmpname( &fname2 );
			file_save_str( fname2, cmd, -1 );
			strdcpy( &cmd1, dirbin, DIR_CHAR "ssie7" EXE " @", fname2, _ );
			System( &s, cmd1 );
			logw( "mft_to_ssie : ", sid, " : ", cmd, BR "==>", s, _ );
			
			// SUPPRIME L'ELEMENT DU CATALOGUE MFT
			
			strdcpy( &cmd, "delete part=*,idtu=", sid, _ );
			mftutil( cmd, &s );
			}
		}
	
	strdcpy( &fname, dirwrk, DIR_CHAR "mft_robot_mft_to_ssie.pid", _ );
	file_delete( fname );
	
	return;
	}
	











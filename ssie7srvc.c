#define MAIN
#include "ssie7.h"
void ssie7_init( T_WORK* t_work ) {}
int ssie7_cmd( T_WORK* t_work ) {}

char *ServiceName = "SSIE";
char *ServiceDescription = "Service SSIE v7 (EDF-DSP-CSP-IT)";
long ServiceStartFlag = SERVICE_AUTO_START; // SERVICE_AUTO_START SERVICE_DEMAND_START
long ServiceStartRightNow = FALSE;
long ServiceTypeFlag = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;

//------------------------------------------------------------------------------

SC_HANDLE ServiceHandle;
SC_HANDLE ServicesDatabase;
OSVERSIONINFO OsVer;
DWORD ServiceCurrentStatus;
HANDLE ServiceEvent;
HANDLE hServiceThread;
SERVICE_DESCRIPTION ServiceDesc;
SERVICE_STATUS_HANDLE ServiceStatus;
SERVICE_TABLE_ENTRY ServiceTable[2];
SERVICE_STATUS ServiceStatusTable;
char FileName[MAX_PATH + 1];
char cwd[MAX_PATH + 1];
char *ErrStartMsg = "ERREUR Initialisation du control dispatcher.";
char *ErrServiceDBMsg = "ERREUR ouverture du services database.";
char *ErrCreateServiceMsg = "ERREUR création du service.";
char *ErrOpenServiceMsg = "ERREUR ouverture du service.";
char *ErrRemoveServiceMsg = "Problème pour supprimer le service.";
char *ServiceInstalledMsg = "Le service SSIE est installé";
char *ServiceRemovedMsg = "Le service SSIE est supprimé";



void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);

//------------------------------------------------------------------------------
void RaiseError(char *ErrorMsg)
//------------------------------------------------------------------------------
	{
	if ( ( argc > 1 )&&( strpos( argv[1], "-muet", 0 ) >= 0 ) ) return;
	if ( ( argc > 2 )&&( strpos( argv[2], "-muet", 0 ) >= 0 ) ) return;

	MessageBox(NULL, ErrorMsg, ServiceName, MB_OK | MB_ICONERROR);
	return;
	}

 
//------------------------------------------------------------------------------
void RaiseInformation(char *InfoMsg)
//------------------------------------------------------------------------------
	{
	if ( ( argc > 1 )&&( strpos( argv[1], "-muet", 0 ) >= 0 ) ) return;
	if ( ( argc > 2 )&&( strpos( argv[2], "-muet", 0 ) >= 0 ) ) return;

	MessageBox(NULL, InfoMsg, ServiceName, MB_OK | MB_ICONINFORMATION);
	return;
	}


 
//------------------------------------------------------------------------------

long ServiceStart(void)
	{
	return(1);
	}

long ServiceRemove(void) {
    return(1);
}
 

T_WORK * t_work;


//------------------------------------------------------------------------------
DWORD __stdcall ServiceThread(LPVOID Param)
// TRAITEMENTS DU SERVICE SSIE
//------------------------------------------------------------------------------
	{
	ServiceStatus = SERVICE_RUNNING;
		
	// SURVEILLANCE DU ROBOT
	
	for( ; ; Sleep(1000) );
	
	
/*
	// SI SSIE7 DOIT ETRE DEMARRE, ON LE DEMARRE (CAS D'UN REDEMARRAGE DE LA MACHINE)
	
	if ( file_exists( "ssie7_is_started" ) )
		{
		unlink( "ssie7_is_started" );
		strdcpy( &cmd, dirbin, DIR_CHAR "ssie7.exe do_start", _ );
		for(;;)
			{
			pid = Start( cmd );
			for( n = 0;; n++)
				{
				Sleep( 100 );
				if ( !process_is_alive( pid ) ) break;
				}
			if ( file_exists( "ssie7_is_started" ) ) break;
			Sleep(100);
			}
		}
		
	
	
	// BOUCLE INFINIE D'ATTENTE DES COMMANDES STOP ET START
	
	for( ; ; Sleep(300) )
		{
		// RECHERCHE UN FICHIER *.ASK DANS LE REPERTOIRE DIRWRK
		
		tf = NULL;
		strdcpy( &fname, "*.ask", _ );

		while( tf = file_list( tf, fname, "nodir nohide absdir" ) )
			{
			// SI LE FICHIER SSIE7START.ASK EXISTE => DEMARRAGE DE SSIE7
			
			if ( !strcmp( tf->name, "ssie7start.ask" ) )
				{
				unlink( tf->name );
				strdcpy( &cmd, dirbin, DIR_CHAR "ssie7.exe do_start", _ );
				pid = Start( cmd );
				}
			else
			
			// SI LE FICHIER SSIE7STOP.ASK EXISTE => ARRET DE SSIE7

			if ( !strcmp( tf->name, "ssie7stop.ask" ) )
				{
				unlink( tf->name );
				strdcpy( &cmd, dirbin, DIR_CHAR "ssie7.exe do_stop", _ );
				Start( cmd );
				}
			}
		}
	
	return(0);
*/
	}






/*
//------------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//------------------------------------------------------------------------------
	{
*/

//------------------------------------------------------------------------------
int main(int ac, char *av[])
//------------------------------------------------------------------------------
	{
	argc = ac;
	argv = av;

    char *_s = null;
    
//	ServicesDatabase = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	ServicesDatabase = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if(!ServicesDatabase) {
        RaiseError(ErrServiceDBMsg);
        return(0);
    }

    
//    if ( strpos( lpCmdLine, "-delete", 0 ) >= 0 )
    if ( ( argc > 1 )&&( strpos( argv[1], "-delete", 0 ) >= 0 ) )
    	goto RemoveProceed;
    	
    
    
    ServiceHandle = OpenService(ServicesDatabase, ServiceName, SERVICE_ALL_ACCESS);
    if(!ServiceHandle)      goto InstallProceed;


    if(QueryServiceStatus(ServiceHandle, &ServiceStatusTable)) {
        /* Feed service manager with our thread if starting state */
        if(ServiceStatusTable.dwCurrentState == SERVICE_START_PENDING) {
            CloseServiceHandle(ServiceHandle);
            CloseServiceHandle(ServicesDatabase);
            ServiceTable[0].lpServiceName = ServiceName;
            ServiceTable[0].lpServiceProc = &ServiceMain;
            if(!StartServiceCtrlDispatcher(&ServiceTable[0])) RaiseError(ErrStartMsg);
            return(0);
        }
    }
    CloseServiceHandle(ServiceHandle);
    return(0);


    
	//--------------------------------------------------------------------------
	InstallProceed:
	//--------------------------------------------------------------------------

    GetModuleFileName(0, FileName, MAX_PATH);
    /* Try to install */
    
    ServiceHandle = CreateService(ServicesDatabase, ServiceName, ServiceName,
									SERVICE_ALL_ACCESS, ServiceTypeFlag,
									ServiceStartFlag, SERVICE_ERROR_NORMAL,
									FileName,
									NULL, NULL, NULL, NULL, NULL);
    if(ServiceHandle) {
        OsVer.dwOSVersionInfoSize = sizeof(OsVer);
        if(GetVersionEx(&OsVer) != 0) {
                if(OsVer.dwMajorVersion >= 5) {
                // Add a description if OS >= Win2k
                if(OsVer.dwPlatformId == VER_PLATFORM_WIN32_NT) {
                    ServiceDesc.lpDescription = ServiceDescription;
                    ChangeServiceConfig2(ServiceHandle, SERVICE_CONFIG_DESCRIPTION, &ServiceDesc);
                }
            }
        }
        if(ServiceStartRightNow) StartService(ServiceHandle, 0, 0);
        CloseServiceHandle(ServiceHandle);
        CloseServiceHandle(ServicesDatabase);
        RaiseInformation(ServiceInstalledMsg);
        return(0);
    }
    if(GetLastError() != ERROR_SERVICE_EXISTS) {
        CloseServiceHandle(ServicesDatabase);
        RaiseError(ErrCreateServiceMsg);
        return(0);
    }
    
	//--------------------------------------------------------------------------
	RemoveProceed:
	//--------------------------------------------------------------------------

	if ( ( ( argc <= 1 )||( strpos( argv[1], "-muet", 0 ) < 0 ) )
		&&( ( argc <= 2 )||( strpos( argv[2], "-muet", 0 ) < 0 ) ) )
		MessageBox(NULL, "RemoveProceed", "SSIE v7", MB_OK | MB_ICONINFORMATION);

    /* Perform removal */
    ServiceHandle = OpenService(ServicesDatabase, ServiceName, SERVICE_ALL_ACCESS | DELETE);
    if(!ServiceHandle) {
        CloseServiceHandle(ServicesDatabase);
        RaiseError(ErrOpenServiceMsg);
        return(0);
    }
    QueryServiceStatus(ServiceHandle, &ServiceStatusTable);
    if(ServiceStatusTable.dwCurrentState != SERVICE_STOPPED) {
        ControlService(ServiceHandle, SERVICE_CONTROL_STOP, &ServiceStatusTable);
        Sleep(500);
    }

    /* Call user procedure */
    if(!ServiceRemove()) {
        CloseServiceHandle(ServiceHandle);
        CloseServiceHandle(ServicesDatabase);
	    RaiseError(ErrRemoveServiceMsg);
        return(0);
    }
    if(DeleteService(ServiceHandle)) {
        CloseServiceHandle(ServiceHandle);
        CloseServiceHandle(ServicesDatabase);
        RaiseInformation(ServiceRemovedMsg);
        return(0);
    }

    CloseServiceHandle(ServiceHandle);
    CloseServiceHandle(ServicesDatabase);
    RaiseError(ErrRemoveServiceMsg);
    return(0);
	}
 


//------------------------------------------------------------------------------
/* --------------- Initialize service thread */
long InitServiceThread(void) {
//------------------------------------------------------------------------------

    DWORD ThreadID;
    
    t_work = (T_WORK*)alloc0( sizeof( T_WORK ) );
    
	
	// CHARGE LE FICHIER DE CONF SSIE7DIR.CFG (qui doit être dans le même répertoire que cet exécutable)
	
	strdcpy( &fname, file_getdir( argv[0] ), DIR_CHAR "ssie7dir.cfg", _ );
	if ( !file_exists( fname ) )
		{
		file_save_str( "d:/r2000/mft/ssie/log/SSIE7SRVC.ERREUR", "ERREUR : fichier ssie7dir.cfg non trouvé" BR, -1 );
		exit(1);
		}
	
	file_load( &s, fname );
	h = var_from_cmd( 0, s );

	// LECTURE DES PARAMETRES DIRBIN, DIRDAT et DIRWRK
	
	strdcpy( &dirbin, var_get( h, "dirbin" ), _ );
	strdcpy( &dirdat, var_get( h, "dirdat" ), _ );
	
	v = var_get( h, "dirwrk" );
	if ( v ) strdcpy( &dirwrk, v, _ );
	else strdcpy( &dirwrk, dirdat, DIR_CHAR "wrk", _ );

/*
	// CREATION DU REPERTOIRE DIRWRK ET POSITIONNEMENT DANS CE REPERTOIRE
	
	mkdir( dirwrk );
	chdir( dirwrk );
*/	
	
	// DEMARRE SSIE7
	
	strdcpy( &cmd, dirbin, DIR_CHAR "ssie7.exe do_start", _ );
	strdcpy( &fname, dirwrk, DIR_CHAR "ssie7_is_started", _ );
	unlink( fname );
	
	pid = Start( cmd );
	for( sleep(1) ; ; sleep(1) )
		{
		if ( file_exists( fname ) ) break;
		if ( ! process_is_alive( pid ) )
			return(1);
		}
    
    
    
    hServiceThread = CreateThread(0, 0, &ServiceThread, 0, 0, &ThreadID);
    if(hServiceThread) {
        ServiceCurrentStatus |= 1;
        return(1);
    }
    return(0);
}

 
//------------------------------------------------------------------------------
/* --------------- Resume service */
void ResumeService(void) {
//------------------------------------------------------------------------------

    ServiceCurrentStatus &= 0xfffffffd;
    ResumeThread(hServiceThread);
    return;
}


 
//------------------------------------------------------------------------------
/* --------------- Pause service */
void PauseService(void) {
//------------------------------------------------------------------------------
    ServiceCurrentStatus |= 2;
    SuspendThread(hServiceThread);
    return;
}
 
 
 
//------------------------------------------------------------------------------
/* --------------- Stop service */
void StopService(void) {
//------------------------------------------------------------------------------

	strdcpy( &cmd, dirbin, DIR_CHAR "ssie7.exe do_stop", _ );
	system( cmd );
	
//	process_kill_all( "ssie7" );
//	process_kill_all( "T0_SSIE" );
	Sleep(100);
	
    ServiceCurrentStatus &= 0xfffffffe;
    SetEvent(ServiceEvent);
    return;
}
     


//------------------------------------------------------------------------------
/* --------------- Send message to system */
long SendStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint, DWORD dwWaitHint) {
//------------------------------------------------------------------------------

    ServiceStatusTable.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatusTable.dwCurrentState = dwCurrentState;
    if(dwCurrentState == SERVICE_START_PENDING) ServiceStatusTable.dwControlsAccepted = 0;
    else ServiceStatusTable.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;
    if(dwServiceSpecificExitCode == 0) ServiceStatusTable.dwWin32ExitCode = dwWin32ExitCode;
    else ServiceStatusTable.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
    ServiceStatusTable.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
    ServiceStatusTable.dwCheckPoint = dwCheckPoint;
    ServiceStatusTable.dwWaitHint = dwWaitHint;
    SetServiceStatus(ServiceStatus, &ServiceStatusTable);
    return(1);
}


//------------------------------------------------------------------------------
/* --------------- Terminate service */
long TerminateService(DWORD ProvidedErr) {
//------------------------------------------------------------------------------

    if(ServiceEvent) CloseHandle(ServiceEvent);
    if(ServiceStatus) SendStatus(SERVICE_STOPPED, ProvidedErr, 0, 0, 0);
    if(hServiceThread) CloseHandle(hServiceThread);
    return(0);
}



//------------------------------------------------------------------------------
/* --------------- Answer to system messages */
void __stdcall CtrlHandler(DWORD CtrlCode) {
//------------------------------------------------------------------------------

//    DWORD StatetoSend = 0;
    DWORD StatetoSend = SERVICE_RUNNING;
    switch(CtrlCode) {
        case SERVICE_CONTROL_STOP:
            SendStatus(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 5000);
            StopService();
            StatetoSend = SERVICE_STOPPED;
            break;
        case SERVICE_CONTROL_PAUSE:
            if(ServiceCurrentStatus == 1) {
                SendStatus(SERVICE_PAUSE_PENDING, NO_ERROR, 0, 1, 1000);
                PauseService();
                StatetoSend = SERVICE_PAUSED;
            }
            break;
        case SERVICE_CONTROL_CONTINUE:
            if(ServiceCurrentStatus == 3) {
                SendStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0, 1, 1000);
                ResumeService();
                StatetoSend = SERVICE_RUNNING;
            }
            break;
        case SERVICE_CONTROL_SHUTDOWN:
            return;
    }
    SendStatus(StatetoSend, NO_ERROR, 0, 0, 0);
}



//------------------------------------------------------------------------------
/* --------------- Service main handler */
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
//------------------------------------------------------------------------------
	{
	ServiceStatus = RegisterServiceCtrlHandler(ServiceName, &CtrlHandler);
	if(!ServiceStatus)
		{
		TerminateService(GetLastError());
		return;
		}
	SendStatus(SERVICE_START_PENDING, NO_ERROR, 0, 1, 5000);
	ServiceEvent = CreateEvent(0, TRUE, FALSE, 0);
	if(!ServiceEvent)
		{
		TerminateService(GetLastError());
		return;
		}
	SendStatus(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);
	SendStatus(SERVICE_START_PENDING, NO_ERROR, 0, 3, 5000);
	if(!InitServiceThread())
		{
		TerminateService(GetLastError());
		return;
		}
	SendStatus(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
	/* Endless loop */
	WaitForSingleObject(ServiceEvent, INFINITE);
	TerminateService(0);
	return;
	}
























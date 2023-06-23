/*--
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    x36driversevice.c

Funcion : define el punto de entrada del servicio y recibe mensajes del x36map

Notes:

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

--*/

#include "stdafx.h"
#include "x36.h"
#include "idioma.h"

#define SVC_CARGAR		0
#define SVC_RECALIBRAR	1
#define SVC_DESCALIBRAR	2
#define SVC_PAUSA		3
#define SVC_SENSIBILIDAD 4

SERVICE_STATUS          svEstado; 
SERVICE_STATUS_HANDLE   svHandle; 
HWND hWnd=NULL;
Cx36* x36=NULL;


void  InicioServicio (DWORD argc, LPTSTR *argv);
void ConfiguracionHandler(WPARAM wParam, LPARAM lParam);
DWORD WINAPI ServicioCtrlHandler(
  DWORD dwControl,     // requested control code
  DWORD dwEventType,   // event type
  LPVOID lpEventData,  // event data
  LPVOID lpContext     // user-defined context data
); 
 
void main( ) 
{ 
    SERVICE_TABLE_ENTRY DispatchTable[2]=
		{{"X36_KbService",(LPSERVICE_MAIN_FUNCTION)InicioServicio},
		{NULL, NULL}};

    if (!StartServiceCtrlDispatcher( DispatchTable)) 
    { 
		MessageBox(NULL,"StartServiceCtrlDispatcher error","[X36DriverService]",MB_SERVICE_NOTIFICATION|MB_ICONSTOP);
    } 
} 



void InicioServicio (DWORD argc, LPTSTR *argv)
{
	svEstado.dwServiceType        = SERVICE_WIN32_OWN_PROCESS|
									SERVICE_INTERACTIVE_PROCESS; 
    svEstado.dwCurrentState       = SERVICE_START_PENDING; 
    svEstado.dwControlsAccepted   = SERVICE_ACCEPT_STOP |
									SERVICE_ACCEPT_POWEREVENT; 
    svEstado.dwWin32ExitCode      = NO_ERROR; 
    svEstado.dwServiceSpecificExitCode = 0; 
    svEstado.dwCheckPoint         = 0; 
    svEstado.dwWaitHint           = 0;

	svHandle = RegisterServiceCtrlHandlerEx( 
        "X36DriverService", ServicioCtrlHandler,NULL); 
 
    if (svHandle == (SERVICE_STATUS_HANDLE)0) 
    { 
		MessageBox(NULL,"RegisterServiceCtrlHandler error","[X36DriverService]",MB_SERVICE_NOTIFICATION|MB_ICONSTOP);
        return; 
    }

	x36 = new Cx36();
    if (!x36->IniciarServicio()) 
    { 
		delete x36; x36=NULL;

		svEstado.dwCurrentState       = SERVICE_STOPPED; 
        svEstado.dwCheckPoint         = 0; 
        svEstado.dwWaitHint           = 0; 
        svEstado.dwWin32ExitCode      = 1062; 
        svEstado.dwServiceSpecificExitCode = 0; 
 
        SetServiceStatus (svHandle, &svEstado); 

        return; 
    }
    svEstado.dwCurrentState       = SERVICE_RUNNING; 
    svEstado.dwCheckPoint         = 0; 
    svEstado.dwWaitHint           = 0; 
 
    if (!SetServiceStatus (svHandle, &svEstado)) 
    { 
		MessageBox(NULL,"SetServiceStatus error","[X36DriverService]",MB_SERVICE_NOTIFICATION|MB_ICONSTOP);
		delete x36; x36=NULL;
		return;
    }
	

	MSG msg;
	hWnd=CreateWindow("STATIC",NULL,0,0,0,0,0,NULL,NULL,NULL,NULL);
	UINT16 call=RegisterWindowMessage("X36ServiceCall");
	while(GetMessage(&msg,NULL,0,0)!=0)
	{
		if(msg.message==call) 
			ConfiguracionHandler(msg.wParam,msg.lParam);
	}
	DestroyWindow(hWnd);
}

void ConfiguracionHandler(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case SVC_CARGAR:
		{
			x36->CargarMapa();
			break;
		}
		case SVC_RECALIBRAR:
		{
			x36->Recalibrar();
			break;
		}
		case SVC_DESCALIBRAR:
		{
			x36->Descalibrar();
			break;
		}
		case SVC_PAUSA:
		{
			x36->PonerPausa();
			break;
		}
		case SVC_SENSIBILIDAD:
		{
			x36->PonerSensibilidad();
		}
	}
}


DWORD WINAPI ServicioCtrlHandler(
  DWORD dwControl,     // requested control code
  DWORD dwEventType,   // event type
  LPVOID lpEventData,  // event data
  LPVOID lpContext     // user-defined context data
)
{
	bool salir=false;
    switch(dwControl) 
    { 
        case SERVICE_CONTROL_STOP:
			salir=true;
            svEstado.dwWin32ExitCode = 0; 
            svEstado.dwCurrentState  = SERVICE_STOPPED; 
            svEstado.dwCheckPoint    = 0; 
            svEstado.dwWaitHint      = 0;
			break;

         case SERVICE_CONTROL_INTERROGATE: 
            break;
		 
		 case SERVICE_CONTROL_POWEREVENT:
			if(dwEventType==PBT_APMRESUMESUSPEND) 
			{
				x36->IniciarX36();
				MessageBox(NULL,LANZAR,"Saitek X36-F",MB_SERVICE_NOTIFICATION|MB_ICONINFORMATION);
			}
			return NO_ERROR;
		default:
			return ERROR_CALL_NOT_IMPLEMENTED;
    } 
 
    // Send current status. 
    if (!SetServiceStatus (svHandle,  &svEstado)) 
    {
		MessageBox(NULL,"Handler SetServiceStatus error","[X36DriverService]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
		return 6;
	} else {
		if(salir) {
			delete x36; x36=NULL;
			PostMessage(hWnd,WM_QUIT,0,0);
		}
	}

    return NO_ERROR; 
} 
/*--
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    X36DriverServiceDll.cpp

Funcion : Funciones DirectX

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
#include "calibraje.h"

__declspec(dllexport) bool Recalibrado(void);

static HINSTANCE hInst;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	hInst=(HINSTANCE)hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

__declspec(dllexport) bool Recalibrado(UCHAR* ejes,UCHAR* buf)
{
	UCHAR aux[4];
	CReorden recalib;

	CopyMemory(aux,buf,4);
	if(recalib.Recalibrar(aux,hInst)) {
		CopyMemory(ejes,aux,4);
		return true;
	} else {
		return false;
	}
}

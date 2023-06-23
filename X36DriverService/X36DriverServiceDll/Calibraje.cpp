/*--
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    Calibraje.cpp

Funcion : Funciones DirectX (clase)

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
#include <dinput.h>
#include "idioma.h"
#include "Calibraje.h"

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dinput8.lib")

LPDIRECTINPUT8  g_pDI=NULL;
LPDIRECTINPUTDEVICE8 g_pJoystick=NULL; 
BOOL CALLBACK DIEnumDevicesCallback(
  LPCDIDEVICEINSTANCE lpddi,  
  LPVOID pvRef  
);

//-------------------------------------------------//
//-------------------------------------------------//

CReorden::CReorden()
{
	for(UCHAR i=0;i<4;i++)
	{
		datosEje[i].iz=0;
		datosEje[i].de=0;
		datosEje[i].n=0;
		datosEje[i].i=0;
		datosEje[i].d=1000;
		datosEje[i].c=500;
	}
}

CReorden::~CReorden()
{
	if(g_pJoystick!=NULL) {
		g_pJoystick->Release();
		g_pJoystick=NULL;
	}
	if(g_pDI!=NULL) {
		g_pDI->Release();
		g_pDI=NULL;
	}
}

bool CReorden::Recalibrar(UCHAR* ejes, HINSTANCE hInst)
{
	if(!LeerRegistro())
	{
		MessageBox(NULL,ERROR1,"[X36DriverServiceDll]",MB_ICONSTOP);
		return false;
	}
	if(!IniciarDX(hInst)) 
	{
		MessageBox(NULL,ERROR2,"[X36DriverServiceDll]",MB_ICONSTOP);
		return false;
	}
	for(UCHAR i=0;i<nEjes;i++)
		if(!Comprobar(i))
		{
			MessageBox(NULL,ERROR3,"[X36DriverServiceDll]",MB_ICONSTOP);
			return false;
		}
	if(!GuardarDatos(ejes))
	{
		MessageBox(NULL,ERROR4,"[X36DriverServiceDll]",MB_ICONSTOP);
		return false;
	}

	return true;
}

/*************************************************
******************** Métodos *********************
**************************************************/

bool CReorden::LeerRegistro()
{
	HKEY key;
	DWORD tipo,tam;

	if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_READ,&key))
		return false;

	tam=1;
	RegQueryValueEx(key,"ZonaNula_x",0,&tipo,(BYTE*)&datosEje[0].n,&tam);
	RegQueryValueEx(key,"ZonaNula_y",0,&tipo,(BYTE*)&datosEje[1].n,&tam);
	RegQueryValueEx(key,"ZonaNula_z",0,&tipo,(BYTE*)&datosEje[2].n,&tam);
	RegQueryValueEx(key,"ZonaNula_r",0,&tipo,(BYTE*)&datosEje[3].n,&tam);

	RegQueryValueEx(key,"Escala_izq_x",0,&tipo,(BYTE*)&datosEje[0].iz,&tam);
	RegQueryValueEx(key,"Escala_izq_y",0,&tipo,(BYTE*)&datosEje[1].iz,&tam);
	RegQueryValueEx(key,"Escala_izq_z",0,&tipo,(BYTE*)&datosEje[2].iz,&tam);
	RegQueryValueEx(key,"Escala_izq_r",0,&tipo,(BYTE*)&datosEje[3].iz,&tam);

	RegQueryValueEx(key,"Escala_der_x",0,&tipo,(BYTE*)&datosEje[0].de,&tam);
	RegQueryValueEx(key,"Escala_der_y",0,&tipo,(BYTE*)&datosEje[1].de,&tam);
	RegQueryValueEx(key,"Escala_der_z",0,&tipo,(BYTE*)&datosEje[2].de,&tam);
	RegQueryValueEx(key,"Escala_der_r",0,&tipo,(BYTE*)&datosEje[3].de,&tam);

	tam=4;
	RegQueryValueEx(key,"Limite_x_i",0,&tipo,(BYTE*)&datosEje[0].i,&tam);
	RegQueryValueEx(key,"Limite_y_i",0,&tipo,(BYTE*)&datosEje[1].i,&tam);
	RegQueryValueEx(key,"Limite_z_i",0,&tipo,(BYTE*)&datosEje[2].i,&tam);
	RegQueryValueEx(key,"Limite_r_i",0,&tipo,(BYTE*)&datosEje[3].i,&tam);

	RegQueryValueEx(key,"Limite_x_c",0,&tipo,(BYTE*)&datosEje[0].c,&tam);
	RegQueryValueEx(key,"Limite_y_c",0,&tipo,(BYTE*)&datosEje[1].c,&tam);
	RegQueryValueEx(key,"Limite_z_c",0,&tipo,(BYTE*)&datosEje[2].c,&tam);
	RegQueryValueEx(key,"Limite_r_c",0,&tipo,(BYTE*)&datosEje[3].c,&tam);

	RegQueryValueEx(key,"Limite_x_d",0,&tipo,(BYTE*)&datosEje[0].d,&tam);
	RegQueryValueEx(key,"Limite_y_d",0,&tipo,(BYTE*)&datosEje[1].d,&tam);
	RegQueryValueEx(key,"Limite_z_d",0,&tipo,(BYTE*)&datosEje[2].d,&tam);
	RegQueryValueEx(key,"Limite_r_d",0,&tipo,(BYTE*)&datosEje[3].d,&tam);

	RegCloseKey(key);

	return true;
}

/**********************************************************
						DirectX
***********************************************************/

bool CReorden::IniciarDX(HINSTANCE hInst)
{
	HRESULT         hr;
 
	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,IID_IDirectInput8, (void**)&g_pDI, NULL); 
	if(FAILED(hr)) return false;

	hr =g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, DIEnumDevicesCallback,
		NULL, DIEDFL_ATTACHEDONLY);
	if(FAILED(hr)) return false;

    if( NULL == g_pJoystick ) return false;


	DIDEVCAPS devCaps;
	devCaps.dwSize=sizeof(DIDEVCAPS);
	if( FAILED( g_pJoystick->GetCapabilities(&devCaps) ) )
		return false;

	nEjes=(UCHAR)devCaps.dwAxes;

	return true;
}

BOOL CALLBACK DIEnumDevicesCallback(
  LPCDIDEVICEINSTANCE lpddi,  
  LPVOID pvRef  
)
{
	if(strcmp(lpddi->tszProductName,"Saitek X36F + X35T")==0 || strcmp(lpddi->tszProductName,"Saitek X36F")==0)
	{
		g_pDI->CreateDevice(lpddi->guidInstance,&g_pJoystick, NULL);
		return DIENUM_STOP;
	} else {
		return DIENUM_CONTINUE;
	}
}


//-----------------------------------------------------
//-----------------------------------------------------


bool CReorden::Comprobar(UCHAR eje)
{
	//Comprobar valores
	if(datosEje[eje].iz<0 || datosEje[eje].iz>255)
		return false;
	if(datosEje[eje].de<0 || datosEje[eje].de>255)
		return false;
	if((datosEje[eje].d<0 || datosEje[eje].d>20000) || (datosEje[eje].i<0 || datosEje[eje].i>20000) || (datosEje[eje].c<0 || datosEje[eje].c>20000))
		return false;
	if(datosEje[eje].d<datosEje[eje].i)
		return false;
	if((datosEje[eje].n/2)>(datosEje[eje].d-datosEje[eje].i))
		return false;

	LONG l1=(datosEje[eje].d-(datosEje[eje].c+datosEje[eje].n));
	LONG l2=((datosEje[eje].c-datosEje[eje].n)-datosEje[eje].i);
	if(l1<=l2) {
//		l1*=datosEje[eje].mu; l1/=datosEje[eje].di;
		datosEje[eje].d=l1;
		datosEje[eje].i=-l1;
	} else {
//		l2*=datosEje[eje].mu; l2/=datosEje[eje].di;
		datosEje[eje].d=l2;
		datosEje[eje].i=-l2;
	}

	return true;
}


//-------------------------------------------------//
//-------------------------------------------------//


/*************************************************
******************* Eventos **********************
**************************************************/

bool CReorden::GuardarDatos(UCHAR* ejes)
{
	struct {
			DIPROPHEADER diph;
			LONG data[3];
	} dipdw;
	dipdw.diph.dwSize       = sizeof(dipdw); 
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	dipdw.diph.dwHow        = DIPH_BYID; 
	dipdw.data[1]=0;

	// Eje x
	dipdw.diph.dwObj=2;
	dipdw.data[0]=datosEje[ejes[0]].i;
	dipdw.data[2]=datosEje[ejes[0]].d;
	if( FAILED(g_pJoystick->SetProperty( DIPROP_CALIBRATION, &dipdw.diph ) ) ) 
		return false;
	// Eje y
	dipdw.diph.dwObj=258;
	dipdw.data[0]=datosEje[ejes[1]].i;
	dipdw.data[2]=datosEje[ejes[1]].d;	
	if( FAILED(g_pJoystick->SetProperty( DIPROP_CALIBRATION, &dipdw.diph ) ) ) 
		return false;

	if(nEjes==4)
	{
		// Eje z
		dipdw.diph.dwObj=514;
		dipdw.data[0]=datosEje[ejes[2]].i;
		dipdw.data[2]=datosEje[ejes[2]].d;	
		if( FAILED(g_pJoystick->SetProperty( DIPROP_CALIBRATION, &dipdw.diph ) ) ) 
			return false;
		// Eje r
		dipdw.diph.dwObj=1282;
		dipdw.data[0]=datosEje[ejes[3]].i;
		dipdw.data[2]=datosEje[ejes[3]].d;	
		if( FAILED(g_pJoystick->SetProperty( DIPROP_CALIBRATION, &dipdw.diph ) ) ) 
			return false;
	}

	return true;
}
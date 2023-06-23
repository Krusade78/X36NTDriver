/*--
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    x36.c

Funcion : funciones de E/S

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
#include "thread.h"
#include <winioctl.h>

#define IOCTL_X36_JOYINICIAR	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0100, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_TECLA			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0101, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_CONFROT		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0102, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_REPEATS		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0103, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_MODOS			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0104, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_COMBI			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0105, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_RATON_EV		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0106, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_RATON			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0107, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_MAPAB			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0108, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_MAPAS			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0109, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_MAPAE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x010a, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_X36_RECONF		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x010b, METHOD_BUFFERED, FILE_WRITE_ACCESS)


Cx36::Cx36(void)
{
	this->acelPed=0;
	this->ejes[0]=0;
	this->ejes[1]=1;
	this->ejes[2]=2;
	this->ejes[3]=3;
}

Cx36::~Cx36(void)
{
	LimpiarThreads();
}

/****************************************************
			FUNCIONES DE INICIO
*****************************************************/

BOOL Cx36::IniciarServicio()
{
	if(!IniciarDriver()) return FALSE;
	if(AutoCarga())
		CargarMapa(true);
	else
		CargarEstandar();
	if(!ReordenarCalibracion(this->ejes)) return FALSE;

	IniciarX36();
	return TRUE;
}

BOOL Cx36::IniciarDriver()	//Error 0
{
	DWORD ret;
	BOOL ok;
	HANDLE driver;
	UCHAR combis[8];

	combis[0]=(UCHAR)MapVirtualKey(VK_LSHIFT,0);
	combis[1]=(UCHAR)MapVirtualKey(VK_LCONTROL,0);
	combis[2]=(UCHAR)MapVirtualKey(VK_LMENU,0);
	combis[3]=(UCHAR)MapVirtualKey(VK_RSHIFT,0);
	combis[4]=(UCHAR)MapVirtualKey(VK_RCONTROL,0);
	combis[5]=(UCHAR)MapVirtualKey(VK_RMENU,0);
	combis[6]=(UCHAR)MapVirtualKey(VK_LWIN,0);
	combis[7]=(UCHAR)MapVirtualKey(VK_RWIN,0);
	if(!CrearEventosyThreads()) {
		MessageBox(NULL,ERROR1,"[X36DriverService][0]",MB_SERVICE_NOTIFICATION|MB_ICONSTOP);
		return FALSE;
	}

	driver=CreateFile(
			"\\\\.\\X36KbInterface",
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	if(driver==INVALID_HANDLE_VALUE) {
		MessageBox(NULL,ERROR2,"[X36DriverService][0]",MB_SERVICE_NOTIFICATION|MB_ICONSTOP);
		return FALSE;
	}
	ok=DeviceIoControl(driver,IOCTL_X36_COMBI,combis,8,NULL,0,&ret,NULL);
	if(ok) ok=DeviceIoControl(driver,IOCTL_X36_RATON_EV,TomarEventos(),sizeof(HANDLE)*8,NULL,0,&ret,NULL);
	CloseHandle(driver);

	PonerPausa();
	PonerSensibilidad();

	if(!ok) MessageBox(NULL,ERROR3,"[X36DriverService][0]",MB_SERVICE_NOTIFICATION|MB_ICONSTOP);

	return ok;
}

bool Cx36::AutoCarga()
{
	DWORD ret;
	bool autoc=false;

	// Cargar Registro

	HKEY key;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_READ,&key))
	{
		DWORD tam=1;
		BYTE c;
		if(ERROR_SUCCESS==RegQueryValueEx(key,"AutoCarga",0,&ret,&c,&tam))
		{
			if(c==1) autoc=true;
		}
		RegCloseKey(key);
	}

	return autoc;
}

void Cx36::CargarEstandar()		// Error 4
{
	DWORD ret;
	UCHAR buffer[36];
	HANDLE driver=CreateFile(
			"\\\\.\\X36KbInterface",
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	if(driver==INVALID_HANDLE_VALUE) goto mal1;
	for(UCHAR i=0;i<36;i++) buffer[i]=i+1;
	if(!DeviceIoControl(driver,IOCTL_X36_MAPAB,buffer,36,NULL,0,&ret,NULL))
		goto mal;
	ZeroMemory(buffer,36);
	buffer[6]=1;buffer[9]=3;buffer[7]=5;buffer[8]=7;
	buffer[11]=2;buffer[13]=4;buffer[12]=6;buffer[10]=8;
	buffer[14]=17;buffer[17]=19;buffer[15]=21;buffer[16]=23;
	buffer[19]=18;buffer[21]=20;buffer[20]=22;buffer[18]=24;
	if(!DeviceIoControl(driver,IOCTL_X36_MAPAS,buffer,36,NULL,0,&ret,NULL))
		goto mal;
	if(!DeviceIoControl(driver,IOCTL_X36_MAPAE,this->ejes,4,NULL,0,&ret,NULL))
		goto mal;
	CloseHandle(driver);

	return;
mal:
	CloseHandle(driver);
	MessageBox(NULL,ERROR3,"[X36DriverService][4]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
	return;
mal1:
	MessageBox(NULL,ERROR2,"[X36DriverService][4]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
}

/***************************************************
***************************************************/



/***************************************************
					IOCTLS
***************************************************/

void Cx36::IniciarX36()		// Error 5
{
	DWORD ret;

	HANDLE driver=CreateFile(
			"\\\\.\\X36KbInterface",
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	if(driver==INVALID_HANDLE_VALUE) {
		MessageBox(NULL,ERROR2,"[X36DriverService][5]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
		return;
	}
	if(!DeviceIoControl(driver,IOCTL_X36_JOYINICIAR,&this->acelPed,1,NULL,0,&ret,NULL))
		MessageBox(NULL,ERROR3,"[X36DriverService][5]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
	CloseHandle(driver);
}

bool Cx36::Recalibrar()		// Error 3
{
	DWORD ret;
	HANDLE driver=CreateFile(
			"\\\\.\\X36KbInterface",
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	if(driver==INVALID_HANDLE_VALUE) {
		MessageBox(NULL,ERROR2,"[X36DriverService][3]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
		return false;
	}
	UCHAR cal=2;
	if(!DeviceIoControl(driver,IOCTL_X36_RECONF,&cal,1,NULL,0,&ret,NULL))
	{
		MessageBox(NULL,ERROR3,"[X36DriverService][3]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
		CloseHandle(driver);
		return false;
	} else {
		CloseHandle(driver);
		return true;
	}
}

void Cx36::Descalibrar()	// Error 6
{
	DWORD ret;
	HANDLE driver=CreateFile(
			"\\\\.\\X36KbInterface",
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	if(driver==INVALID_HANDLE_VALUE) {
		MessageBox(NULL,ERROR2,"[X36DriverService][6]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
		return;
	}
	UCHAR cal=1;
	if(!DeviceIoControl(driver,IOCTL_X36_RECONF,&cal,1,NULL,0,&ret,NULL))
		MessageBox(NULL,ERROR3,"[X36DriverService][6]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
	CloseHandle(driver);
}

void Cx36::PonerPausa()
{
	DWORD ret;
	HKEY key;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_READ,&key))
	{
		DWORD tam=1;
		BYTE c;
		if(ERROR_SUCCESS==RegQueryValueEx(key,"ratonP",0,&ret,&c,&tam))
		{
			if((c>0) && (c<100)) CambiarPausa(c);
		}
		RegCloseKey(key);
	}
}

void Cx36::PonerSensibilidad()
{
	DWORD ret;
	HKEY key;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_READ,&key))
	{
		DWORD tam=1;
		BYTE c;
		if(ERROR_SUCCESS==RegQueryValueEx(key,"ratonS",0,&ret,&c,&tam))
		{
			if((c>0) && (c<21)) CambiarSensibilidad(c);
		}
		RegCloseKey(key);
	}
}

/***************************************************
***************************************************/



/***************************************************
				FUNCIONES DE MAPA
***************************************************/

void Cx36::CargarMapa(bool ini)		// Error 1
{
	char* ruta=NULL;
	DWORD ret;
	UINT16 pos=0;
	UCHAR buf[37];
	bool lanzar=false;
	PAQUETE_SAITEK paq;
	UINT64* teclas=NULL;
	UCHAR* teclasExt=NULL;
	UINT16 ratonPos[6]={0,0,0,0,0,0};

	// Cargar Registro

	HKEY key;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635",0,KEY_READ,&key))
	{
		DWORD tam=1;
		BYTE c;
		LONG res;
		if(ini)
			res=RegQueryValueEx(key,"MapaIni",0,&ret,&c,&tam);
		else
			res=RegQueryValueEx(key,"Mapa",0,&ret,&c,&tam);
		if(res==ERROR_MORE_DATA)
		{
			ruta=new char[tam];
			if(ini)
				res=RegQueryValueEx(key,"MapaIni",0,&ret,(BYTE*)ruta,&tam);
			else
				res=RegQueryValueEx(key,"Mapa",0,&ret,(BYTE*)ruta,&tam);
			if(res!=ERROR_SUCCESS)
			{
				RegCloseKey(key);
				delete[] ruta; ruta=NULL;
				goto mal;
			}
		}
		RegCloseKey(key);
	}

	// Cargar Mapa

	HANDLE mapa=CreateFile(ruta,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	if(mapa==INVALID_HANDLE_VALUE) goto mal;
	HANDLE driver=CreateFile(
			"\\\\.\\X36KbInterface",
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	if(driver==INVALID_HANDLE_VALUE) goto mal1;

	// Modos & Pinkie
	if(!ReadFile(mapa,buf,1,&ret,NULL) || ret!=1) goto mal2;
	buf[1]=(BOOL)((buf[0]&64)/63);			// Pinkie
	buf[0]=(BOOL)((buf[0]&128)/127);		// Modos
	if(!DeviceIoControl(driver,IOCTL_X36_MODOS,buf,2,NULL,0,&ret,NULL))
		goto mal3;
	// Acelerador
	if(!ReadFile(mapa,buf,1,&ret,NULL) || ret!=1) goto mal2;
	if(this->acelPed!=buf[0])
	{
		this->acelPed=buf[0];
		lanzar=true;
	}
	// Ejes
	if(!ReadFile(mapa,buf,4,&ret,NULL) || ret!=4) goto mal2;
	if(*((DWORD*)this->ejes)!=*((DWORD*)buf))
	{
		if(!ReordenarCalibracion(buf))
		{
			goto mal4;
		} else {
			if(!DeviceIoControl(driver,IOCTL_X36_MAPAE,buf,4,NULL,0,&ret,NULL))
				goto mal3;
			if(!Recalibrar()) goto mal4;
		}
	}

	// Botones
	if(!ReadFile(mapa,buf,36,&ret,NULL) || ret!=36) goto mal2;
	if(!DeviceIoControl(driver,IOCTL_X36_MAPAB,buf,36,NULL,0,&ret,NULL))
		goto mal3;
	// Setas
	if(!ReadFile(mapa,buf,36,&ret,NULL) || ret!=36) goto mal2;
	if(!DeviceIoControl(driver,IOCTL_X36_MAPAS,buf,36,NULL,0,&ret,NULL))
		goto mal3;
	// Rotatorios
	if(!ReadFile(mapa,buf,24,&ret,NULL) || ret!=24) goto mal2;
	if(!DeviceIoControl(driver,IOCTL_X36_CONFROT,buf,24,NULL,0,&ret,NULL))
		goto mal3;

	// Teclas
	if(!ReadFile(mapa,&pos,2,&ret,NULL) || ret!=2) goto mal2;

	teclas=new UINT64[pos+1];
	teclasExt=new UCHAR[pos+1];
	*teclas=0;
	*teclasExt=0;
	for(UINT16 i=1;i<(pos+1);i++) {
		// Buscar posiciones del ratón
		if(!ReadFile(mapa,buf,32,&ret,NULL) || ret!=32) goto mal2;
		buf[33]=0;
		if(strcmp((char*)buf,"<Ratón Arriba>")==0 || strcmp((char*)buf,"<Mouse Up>")==0) ratonPos[0]=i;
		if(strcmp((char*)buf,"<Ratón Abajo>")==0 || strcmp((char*)buf,"<Mouse Down>")==0) ratonPos[1]=i;
		if(strcmp((char*)buf,"<Ratón Izquierda>")==0 || strcmp((char*)buf,"<Mouse Left>")==0) ratonPos[2]=i;
		if(strcmp((char*)buf,"<Ratón Derecha>")==0 || strcmp((char*)buf,"<Mouse Right>")==0) ratonPos[3]=i;
		if(strcmp((char*)buf,"<Ratón, Botón Izquierdo>")==0 || strcmp((char*)buf,"<Mouse Left Button>")==0) ratonPos[4]=i;
		if(strcmp((char*)buf,"<Ratón, Botón Derecho>")==0 || strcmp((char*)buf,"<Mouse Right Button>")==0) ratonPos[5]=i;
		// Guardar teclas
		if(!ReadFile(mapa,&teclas[i],8,&ret,NULL) || ret!=8) goto mal2;
		for(UCHAR j=0;j<8;j++) {
			*(((UCHAR*)&teclas[i])+j)=(UCHAR)MapVirtualKey(*(((UCHAR*)&teclas[i])+j),0);
		}
		if(!ReadFile(mapa,&teclasExt[i],1,&ret,NULL) || ret!=1) goto mal2;
	}

	FillMemory(buf,6,255);
	for(UCHAR i=0;i<6;i++) {
		paq.Modo=i;
		for(UCHAR j=0;j<104;j++) {
			if(!ReadFile(mapa,&pos,2,&ret,NULL) || ret!=2) goto mal2;
			// Buscar asignaciones del ratón
			for(UCHAR k=0;k<6;k++)
				if(pos==ratonPos[k]) buf[k]=j;
			// Enviar teclas
			paq.Posicion=j;
			paq.Dato=teclas[pos];
			paq.DatoExt=teclasExt[pos];
			if(!DeviceIoControl(driver,IOCTL_X36_TECLA,&paq,sizeof(PAQUETE_SAITEK),NULL,0,&ret,NULL))
				goto mal3;
		}
	}
	// Raton
	if(!DeviceIoControl(driver,IOCTL_X36_RATON,buf,6,NULL,0,&ret,NULL))
		goto mal3;

	delete[] teclas; teclas=NULL;
	delete[] teclasExt; teclasExt=NULL;

	for(UCHAR i=0; i<6;i++)
	{
		if(!ReadFile(mapa,&buf[1],36,&ret,NULL) || ret!=36) goto mal2;
		buf[0]=i;
		if(!DeviceIoControl(driver,IOCTL_X36_REPEATS,buf,37,NULL,0,&ret,NULL))
			goto mal3;
	}

	CloseHandle(driver);
	CloseHandle(mapa);

	if(!ini) {
		if(lanzar)
		{
			IniciarX36();
			MessageBox(NULL,LANZAR,"[X36DriverService]",MB_SERVICE_NOTIFICATION|MB_ICONINFORMATION);
		} else {
			MessageBox(NULL,OK,"[X36DriverService]",MB_SERVICE_NOTIFICATION|MB_ICONINFORMATION);
		}
	}

	return;

mal:
	MessageBox(NULL,ERROR7,"[X36DriverService][1]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
	return;
mal1:
	CloseHandle(mapa);
	MessageBox(NULL,ERROR2,"[X36DriverService][1]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
	return;
mal2:
	MessageBox(NULL,ERROR8,"[X36DriverService][1]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
	goto mal4;
mal3:
	MessageBox(NULL,ERROR3,"[X36DriverService][1]",MB_SERVICE_NOTIFICATION|MB_ICONWARNING);
mal4:
	if(teclas!=NULL) {
		delete[] teclas; teclas=NULL;
		delete[] teclasExt; teclasExt=NULL;
	}
	CloseHandle(driver);
	CloseHandle(mapa);
}


/*****************************************************
					CALIBRACION
 *****************************************************/

bool Cx36::ReordenarCalibracion(UCHAR* buf)	// Error 2
{
	typedef bool (*FUNC)(UCHAR*, UCHAR*); 
	bool ok;

	HMODULE dll=LoadLibrary("x36_svc.dll");
	if(dll==NULL) return false;

	FUNC Recalibrado=(FUNC)GetProcAddress(dll,"Recalibrado");
	if(Recalibrado==NULL) {
		MessageBox(NULL,ERROR6,"[X36DriverService][2]",MB_SERVICE_NOTIFICATION|MB_ICONSTOP);
		ok=false;
	} else {
		ok=Recalibrado(this->ejes,buf);
	}

	FreeLibrary(dll);
	return ok;
}
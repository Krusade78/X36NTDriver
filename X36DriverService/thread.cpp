/*--
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    thread.c

Funcion : Emulacion del raton

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
#include "thread.h"

typedef struct _PAQUETE_THREAD
{
	UCHAR tipo;
	HANDLE handle;
} PAQUETE_THREAD,*PPAQUETE_THREAD;


DWORD pausa=50;
UCHAR sensibilidad=4;
bool salir=false;
char fuera;
HANDLE evento[8];
HANDLE thread[8];

bool CrearEventosyThreads()
{
	HANDLE sinc;
	PAQUETE_THREAD pth;

	sinc=CreateEvent(NULL,FALSE,FALSE,NULL);
	if(sinc==NULL) return FALSE;

	evento[0]=CreateEvent(NULL,TRUE,FALSE,NULL);
	evento[1]=CreateEvent(NULL,TRUE,FALSE,NULL);
	evento[2]=CreateEvent(NULL,TRUE,FALSE,NULL);
	evento[3]=CreateEvent(NULL,TRUE,FALSE,NULL);
	evento[4]=CreateEvent(NULL,FALSE,FALSE,NULL);
	evento[5]=CreateEvent(NULL,FALSE,FALSE,NULL);
	evento[6]=CreateEvent(NULL,FALSE,FALSE,NULL);
	evento[7]=CreateEvent(NULL,FALSE,FALSE,NULL);

	pth.handle=sinc;
	for(char i=0;i<8;i++)
	{
		if(evento[i]==NULL) return FALSE;
		pth.tipo=i;
		if(i<4) {
			thread[i]=CreateThread(NULL,0,Mover,(LPVOID)&pth,0,NULL);
		} else {
			thread[i]=CreateThread(NULL,0,Boton,(LPVOID)&pth,0,NULL);
		}
		if(thread[i]==NULL)
		{
			CloseHandle(sinc);
			return false;
		}
		WaitForSingleObject(sinc,INFINITE);
	}

	CloseHandle(sinc);

	return true;
}

HANDLE* TomarEventos()
{
	return evento;
}

void CambiarSensibilidad(UCHAR s)
{
	sensibilidad=s;
}

void CambiarPausa(UCHAR p)
{
	pausa=p;
}

void LimpiarThreads()
{
	salir=true;
	fuera=0;
	char i;
	for(i=0;i<8;i++) if(thread[i]==NULL) fuera++;
	while(fuera<8)
	{
		for(i=0;i<8;i++) if(evento[i]!=NULL) SetEvent(evento[i]);
		Sleep(100);
	}
	for(i=0;i<8;i++)
	{
		if(evento[i]!=NULL)
		{
			CloseHandle(evento[i]);
			evento[i]=NULL;
		}
	}
}

DWORD WINAPI Mover(void* pParam)
{
	char tipo;
	{
		PPAQUETE_THREAD p=(PPAQUETE_THREAD)pParam;
		tipo=p->tipo;
	    SetEvent(p->handle);
	}
	INPUT inp;
	char x=0,y=0;
	inp.type=INPUT_MOUSE;
	ZeroMemory(&inp.mi,sizeof(MOUSEINPUT));
	inp.mi.dwFlags=MOUSEEVENTF_MOVE;

	switch(tipo)
	{
		case 0:
			y=-1;
			break;
		case 1:
			y=1;
			break;
		case 2:
			x=-1;
			break;
		case 3:
			x=1;
	}
	while(true)
	{
		WaitForSingleObject(evento[tipo],INFINITE);
		if(salir) break;
		Sleep(pausa);
		inp.mi.dx=x*sensibilidad;
		inp.mi.dy=y*sensibilidad;
		SendInput(1,&inp,sizeof(INPUT));
	}

	fuera++;
	ExitThread(0);
}

DWORD WINAPI Boton(void* pParam)
{
	char tipo;
	{
		PPAQUETE_THREAD p=(PPAQUETE_THREAD)pParam;
		tipo=p->tipo;
	    SetEvent(p->handle);
	}
	INPUT inp;
	inp.type=INPUT_MOUSE;
	ZeroMemory(&inp.mi,sizeof(MOUSEINPUT));

	switch(tipo)
	{
		case 4:
			inp.mi.dwFlags=MOUSEEVENTF_LEFTDOWN;
			break;
		case 5:
			inp.mi.dwFlags=MOUSEEVENTF_RIGHTDOWN;
			break;
		case 6:
			inp.mi.dwFlags=MOUSEEVENTF_LEFTUP;
			break;
		case 7:
			inp.mi.dwFlags=MOUSEEVENTF_RIGHTUP;
	}
	while(true)
	{
		WaitForSingleObject(evento[tipo],INFINITE);
		if(salir) break;
		SendInput(1,&inp,sizeof(INPUT));
	}

	fuera++;
	ExitThread(0);
}
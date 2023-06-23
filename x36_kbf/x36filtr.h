/*++
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    x36filter.h

Abstract:

    Este modulo contiene las declaraciones de funciones especificamente diseñadas
	para controlar el X36

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

#define IOCTL_HID_LEER			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0100, METHOD_NEITHER, FILE_READ_ACCESS)

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


typedef struct _PAQUETE_SAITEK
{
	UCHAR Modo;
	UCHAR Posicion;
	UINT64 Dato;
	UCHAR DatoExt;
} PAQUETE_SAITEK, *PPAQUETE_SAITEK;

typedef struct _POOLS
{
	PVOID pool;
	UCHAR tam;
} POOL, *PPOOL;


typedef struct _X36DEVICE_EXTENSION
{
	PDEVICE_EXTENSION kDevExt;

	// Variables de estado
	BOOLEAN faseIni;
	UCHAR bytesIni;
	UCHAR fase;

	UCHAR stSeta1;
	UCHAR stSeta2;
	USHORT gPosicion;
	UCHAR gEje;
	UCHAR gPosAnt[4];
	UCHAR gGirAnt[4];
	UCHAR stPinkie;
	UCHAR stModo;
	UCHAR autoRepeatsOn[36];
	UCHAR boton[5];
	UCHAR seta[2];
	UCHAR reConf;
	PRKEVENT ratonEv[8];

	// Variable de configuración
	UCHAR ratonPos[8];
	UCHAR mapaB[36];
	UCHAR mapaS[36];
	UCHAR mapaE[4];
	BOOLEAN modo;
	BOOLEAN pinkie;
	UCHAR combis[8];
	UCHAR confRot[4][6];
	UINT64 teclas[6][104];
	UCHAR teclasExt[6][104];
	BOOLEAN autoRepeat[6][36];

}X36DEVICE_EXTENSION,*PX36DEVICE_EXTENSION;


VOID
X36Filter_Iniciar(
    IN PX36DEVICE_EXTENSION x36DevExt
	);

NTSTATUS
X36Filter_InterfaceDispatch (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
X36Filter_ServiceCallback(
    IN PDEVICE_OBJECT DeviceObject,
    IN PKEYBOARD_INPUT_DATA InputDataStart,
    IN PKEYBOARD_INPUT_DATA InputDataEnd,
    IN OUT PULONG InputDataConsumed
    );

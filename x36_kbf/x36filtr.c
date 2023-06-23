/*--
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    x36filtr.c

Abstract:

	Funciones nuevas del filtro para el X36

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

#include "x36filtr.h"
#include "x36.c"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, X36Filter_Iniciar)
#pragma alloc_text (PAGE, X36Filter_InterfaceDispatch)
#endif

VOID X36Filter_Iniciar(
	IN PX36DEVICE_EXTENSION x36DevExt
	)
{
	PAGED_CODE();

	// Variables de estado
	x36DevExt->faseIni = FALSE;
	x36DevExt->bytesIni = 0;
	x36DevExt->fase = 0;

	x36DevExt->stSeta1=0;
	x36DevExt->stSeta2=0;
	x36DevExt->gPosicion = 0;
	x36DevExt->gEje = 0;
	x36DevExt->gPosAnt[0]=11;
	x36DevExt->gPosAnt[1]=11;
	x36DevExt->gPosAnt[2]=11;
	x36DevExt->gPosAnt[3]=11;
	x36DevExt->gGirAnt[0]=0;
	x36DevExt->gGirAnt[1]=0;
	x36DevExt->gGirAnt[2]=0;
	x36DevExt->gGirAnt[3]=0;
	x36DevExt->stPinkie=0;
	x36DevExt->stModo=2;

	x36DevExt->reConf=0;
	RtlFillMemory(x36DevExt->autoRepeatsOn,sizeof(UCHAR)*36,255);

	// Variables de configuración
	RtlZeroMemory(x36DevExt->ratonEv,sizeof(PRKEVENT)*8);
	RtlFillMemory(x36DevExt->ratonPos,6,255);
	RtlZeroMemory(x36DevExt->mapaB,36);
	RtlZeroMemory(x36DevExt->mapaS,16);
	RtlZeroMemory(x36DevExt->mapaE,4);
	RtlZeroMemory(x36DevExt->boton,5);
	x36DevExt->seta[0]=0; x36DevExt->seta[1]=0;
	x36DevExt->modo=FALSE;
	x36DevExt->pinkie=FALSE;
	RtlZeroMemory(x36DevExt->combis,sizeof(UCHAR)*8);
	RtlZeroMemory(x36DevExt->confRot,sizeof(UCHAR)*24);
	RtlZeroMemory(x36DevExt->teclas,sizeof(UINT64)*6*104);
	RtlZeroMemory(x36DevExt->teclasExt,sizeof(UCHAR)*6*104);
	RtlZeroMemory(x36DevExt->autoRepeat,sizeof(BOOLEAN)*6*36);

}

NTSTATUS
X36Filter_InterfaceDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    Interfaz de control X36                                          
                                         
Arguments:

    DeviceObject - Pointer to the device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/
{
    PIO_STACK_LOCATION          irpStack;
    PX36DEVICE_EXTENSION        x36DevExt=(PX36DEVICE_EXTENSION) DeviceObject->DeviceExtension;
    NTSTATUS					status=STATUS_SUCCESS;

	PAGED_CODE();

    Irp->IoStatus.Information = 0;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

	if(irpStack->MajorFunction==IRP_MJ_INTERNAL_DEVICE_CONTROL)
	{
		if(irpStack->Parameters.DeviceIoControl.IoControlCode==IOCTL_HID_LEER)
		{
			UCHAR buf[12];
			buf[0]=x36DevExt->seta[0] ; buf[1]=x36DevExt->seta[1];
			RtlCopyMemory(&buf[2],x36DevExt->boton,5);
			if(x36DevExt->reConf==1)
			{
				buf[7]=1; x36DevExt->reConf=0;
			} else if (x36DevExt->reConf==2) {
				buf[7]=2; x36DevExt->reConf=0;
				RtlCopyMemory(&buf[8],x36DevExt->mapaE,4);
			} else {
				buf[7]=0;
			}
			RtlCopyMemory(Irp->UserBuffer,buf,12);
            Irp->IoStatus.Information = 12;
		}
	}
	if(irpStack->MajorFunction==IRP_MJ_DEVICE_CONTROL)
	{
    switch (irpStack->Parameters.DeviceIoControl.IoControlCode)
	{
		case IOCTL_X36_JOYINICIAR:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==1)
			{
	            PDEVICE_EXTENSION devExt=x36DevExt->kDevExt;
				x36DevExt->faseIni=TRUE;

				// Encabezado
				devExt->IsrWritePort(devExt->CallContext,110);
				devExt->IsrWritePort(devExt->CallContext,16);
				devExt->IsrWritePort(devExt->CallContext,11);
				// Botones 1,2,3,4,5,6
				devExt->IsrWritePort(devExt->CallContext,0);
				devExt->IsrWritePort(devExt->CallContext,0);
				devExt->IsrWritePort(devExt->CallContext,0);
				devExt->IsrWritePort(devExt->CallContext,0);
				devExt->IsrWritePort(devExt->CallContext,0);
				devExt->IsrWritePort(devExt->CallContext,0);
				// POVs
				devExt->IsrWritePort(devExt->CallContext,0);
				devExt->IsrWritePort(devExt->CallContext,0);
				// Acelerador y Timón
				devExt->IsrWritePort(devExt->CallContext,*((UCHAR*)Irp->AssociatedIrp.SystemBuffer));
				// Fin
				devExt->IsrWritePort(devExt->CallContext,8);
				devExt->IsrWritePort(devExt->CallContext,16);
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}
		case IOCTL_X36_COMBI:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==8) {
				UCHAR *combis_aux=((UCHAR *)Irp->AssociatedIrp.SystemBuffer);
				RtlCopyMemory(x36DevExt->combis,combis_aux,8);
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}
		case IOCTL_X36_TECLA:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==sizeof(PAQUETE_SAITEK)) {
				PPAQUETE_SAITEK paquete=(PPAQUETE_SAITEK)Irp->AssociatedIrp.SystemBuffer;
				x36DevExt->teclas[paquete->Modo][paquete->Posicion]=paquete->Dato;
				x36DevExt->teclasExt[paquete->Modo][paquete->Posicion]=paquete->DatoExt;
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;

		}
		case IOCTL_X36_CONFROT:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==24) {
				RtlCopyMemory(x36DevExt->confRot,(UCHAR *)Irp->AssociatedIrp.SystemBuffer,24);
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}
		case IOCTL_X36_MODOS:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==2)
			{
				x36DevExt->modo=(BOOLEAN)(*((UCHAR *)Irp->AssociatedIrp.SystemBuffer));
				x36DevExt->pinkie=(BOOLEAN)(*((UCHAR *)Irp->AssociatedIrp.SystemBuffer+1));
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}
		case IOCTL_X36_REPEATS:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==37)
			{
				UCHAR idM=*((UCHAR *)Irp->AssociatedIrp.SystemBuffer);
				UCHAR *autor_aux=((UCHAR *)Irp->AssociatedIrp.SystemBuffer)+1;
				RtlCopyMemory(&x36DevExt->autoRepeat[idM],autor_aux,36);
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}
		case IOCTL_X36_RATON:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==6)
			{
				x36DevExt->ratonPos[0]=*((UCHAR*)Irp->AssociatedIrp.SystemBuffer);
				x36DevExt->ratonPos[1]=*((UCHAR*)Irp->AssociatedIrp.SystemBuffer+1);
				x36DevExt->ratonPos[2]=*((UCHAR*)Irp->AssociatedIrp.SystemBuffer+2);
				x36DevExt->ratonPos[3]=*((UCHAR*)Irp->AssociatedIrp.SystemBuffer+3);
				x36DevExt->ratonPos[4]=*((UCHAR*)Irp->AssociatedIrp.SystemBuffer+4);
				x36DevExt->ratonPos[5]=*((UCHAR*)Irp->AssociatedIrp.SystemBuffer+5);
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}
		case IOCTL_X36_RATON_EV:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==(sizeof(HANDLE)*8))
			{
				UCHAR i=0;
				HANDLE *evento=(HANDLE*)Irp->AssociatedIrp.SystemBuffer;
				for(i=0;i<8;i++)
				{
					if(x36DevExt->ratonEv[i]!=NULL)
					{
						ObDereferenceObject(x36DevExt->ratonEv[i]);
						x36DevExt->ratonEv[i]=NULL;
					}
					status=ObReferenceObjectByHandle(
						evento[i],
						EVENT_MODIFY_STATE,
						*ExEventObjectType,
						UserMode,
						&x36DevExt->ratonEv[i],
						NULL);
					if(!NT_SUCCESS(status)) {
						x36DevExt->ratonEv[i]=NULL;
						i=9; //break
					}
				}
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}
		case IOCTL_X36_MAPAB:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==36)
			{
				RtlCopyMemory(x36DevExt->mapaB,Irp->AssociatedIrp.SystemBuffer,36);
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}
		case IOCTL_X36_MAPAS:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==36)
			{
				RtlCopyMemory(x36DevExt->mapaS,Irp->AssociatedIrp.SystemBuffer,36);
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}
		case IOCTL_X36_MAPAE:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==4)
			{
				RtlCopyMemory(x36DevExt->mapaE,Irp->AssociatedIrp.SystemBuffer,4);
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}
		case IOCTL_X36_RECONF:
		{
			if(irpStack->Parameters.DeviceIoControl.InputBufferLength==1)
			{
				x36DevExt->reConf=*((UCHAR*)Irp->AssociatedIrp.SystemBuffer);
			} else {
				status= STATUS_INVALID_BUFFER_SIZE;
			}
			break;
		}

		default:
                status=STATUS_NOT_SUPPORTED;
    }
	}
	
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}


VOID
X36Filter_ServiceCallback(
    IN PDEVICE_OBJECT DeviceObject,
    IN PKEYBOARD_INPUT_DATA InputDataStart,
    IN PKEYBOARD_INPUT_DATA InputDataEnd,
    IN OUT PULONG InputDataConsumed
    )
/*++

Routine Description:

    Called when there are keyboard packets to report to the RIT.
                    
Arguments:

    DeviceObject - Context passed during the connect IOCTL
    
    InputDataStart - First packet to be reported
    
    InputDataEnd - One past the last packet to be reported.  Total number of
                   packets is equal to InputDataEnd - InputDataStart
    
    InputDataConsumed - Set to the total number of packets consumed by the RIT
                        (via the function pointer we replaced in the connect
                        IOCTL)

Return Value:

    Status is returned.

--*/
{
    PDEVICE_EXTENSION   devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
	PX36DEVICE_EXTENSION   x36DevExt = (PX36DEVICE_EXTENSION) devExt->x36Obj->DeviceExtension;
	UCHAR				npaquetes =InputDataEnd-InputDataStart;
	UCHAR				i = 0;
	BOOLEAN				cambiar = FALSE;
	POOL				lPool;

	lPool.pool=NULL;
	lPool.tam=0;

	KeAcquireSpinLockAtDpcLevel(&devExt->SpinLock);

	for(i=0;i<npaquetes;i++) {

		if (x36DevExt->faseIni) {
			x36DevExt->bytesIni++;
			if(x36DevExt->bytesIni>9) {
				x36DevExt->bytesIni=0;
				x36DevExt->faseIni=FALSE;
			}
		        cambiar=TRUE;
		}

		switch(x36DevExt->fase)
		{
			case 6:
				x36DevExt->fase=0;
				cambiar=TRUE;
				TraducirGiratorio(x36DevExt,InputDataStart+i,&lPool);
				break;
			case 5:
				x36DevExt->gPosicion=(InputDataStart+i)->MakeCode;
				x36DevExt->fase=6;
				cambiar=TRUE;
				break;
			case 4:
				if((InputDataStart+i)->MakeCode==33) x36DevExt->gEje=0;
				if((InputDataStart+i)->MakeCode==34) x36DevExt->gEje=1;
				if((InputDataStart+i)->MakeCode==35) x36DevExt->gEje=2;
				if((InputDataStart+i)->MakeCode==36) x36DevExt->gEje=3;
				if(((InputDataStart+i)->MakeCode==33)||((InputDataStart+i)->MakeCode==34)
				  ||((InputDataStart+i)->MakeCode==35)||((InputDataStart+i)->MakeCode==36)) {
					x36DevExt->fase=5;
					cambiar=TRUE;
				} else {
					x36DevExt->fase=0;
				}
				break;
			case 3:
				x36DevExt->fase=0;
                cambiar=TRUE;
                TraducirBoton(x36DevExt,InputDataStart+i,&lPool);
				break;
			case 2:
				if ((InputDataStart+i)->MakeCode==1) {
					cambiar=TRUE;
					x36DevExt->fase=3;
					break;
				}
				if ((InputDataStart+i)->MakeCode==3) {
					cambiar=TRUE;
					x36DevExt->fase=4;
				} else {
					x36DevExt->fase=0;
				}
				break;
			case 1:
				if((InputDataStart+i)->MakeCode==16) {
					x36DevExt->fase=2;
					cambiar=TRUE;
				} else {
					x36DevExt->fase=0;
				}
				break;
		}

		if ((InputDataStart+i)->MakeCode==111) {
			x36DevExt->fase=1;
			cambiar=TRUE;
		}

		if(cambiar) {
			(InputDataStart+i)->MakeCode=111;
			cambiar=FALSE;
		}
	}

	for(i=0;i<npaquetes;i++) {
		if((InputDataStart+i)->MakeCode==111)
		{
			npaquetes--;
			RtlCopyMemory(InputDataStart+i,InputDataStart+i+1,sizeof(KEYBOARD_INPUT_DATA)*npaquetes);
			i--;
		}
	}

	i=InputDataEnd-InputDataStart;

	if(lPool.pool==NULL) {
			InputDataEnd=InputDataStart+npaquetes;
	} else {
		if(npaquetes>0) {
			PVOID auxPool=ExAllocatePool(NonPagedPool,(npaquetes+lPool.tam)*sizeof(KEYBOARD_INPUT_DATA));
			if(auxPool!=NULL) {
				RtlCopyMemory(auxPool,(PVOID)InputDataStart,npaquetes*sizeof(KEYBOARD_INPUT_DATA));
				RtlCopyMemory((PVOID)(((PKEYBOARD_INPUT_DATA)auxPool)+npaquetes),lPool.pool,lPool.tam*sizeof(KEYBOARD_INPUT_DATA));
				ExFreePool(lPool.pool);
				lPool.tam+=npaquetes;
				lPool.pool=auxPool;
			}
		}
		InputDataStart=(PKEYBOARD_INPUT_DATA)lPool.pool;
		InputDataEnd=InputDataStart+lPool.tam;
	}

	KeReleaseSpinLockFromDpcLevel(&devExt->SpinLock);

	(*(PSERVICE_CALLBACK_ROUTINE) devExt->UpperConnectData.ClassService)(
			devExt->UpperConnectData.ClassDeviceObject,
			InputDataStart,
			InputDataEnd,
			InputDataConsumed);

	// Para paquetes extra largos que no se pueden procesar
	// enteros, rellamar con lo que falte.

	if(*InputDataConsumed!=(InputDataEnd-InputDataStart))
	{
		InputDataStart+=*InputDataConsumed;
		(*(PSERVICE_CALLBACK_ROUTINE) devExt->UpperConnectData.ClassService)(
			devExt->UpperConnectData.ClassDeviceObject,
			InputDataStart,
			InputDataEnd,
			InputDataConsumed);
	}

	if(lPool.pool!=NULL)
		ExFreePool(lPool.pool);
	*InputDataConsumed=i;
}

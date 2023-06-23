/*--
Copyright (c) 2003-2004 Alfredo Costalago

Module Name:

    kbfiltr.c

Abstract:

	Funciones del filtro de teclado mínimamente modificadas para
	el x36

Notes:

	Original file by Microsoft Corporation in Windows XP DDK 2.6
	Copyright (c) 1998. 1999  Microsoft Corporation

License:

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

#include "kbfiltr.h"
#include "x36filtr.c"

NTSTATUS DriverEntry (PDRIVER_OBJECT, PUNICODE_STRING);

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, KbFilter_AddDevice)
#pragma alloc_text (PAGE, KbFilter_Unload)
#pragma alloc_text (PAGE, KbFilter_DispatchPassThrough)
#pragma alloc_text (PAGE, KbFilter_InternIoCtl)
#pragma alloc_text (PAGE, KbFilter_PnP)
#pragma alloc_text (PAGE, KbFilter_Power)

#endif

NTSTATUS
DriverEntry (
    IN  PDRIVER_OBJECT  DriverObject,
    IN  PUNICODE_STRING RegistryPath
    )
/*++
Routine Description:

    Initialize the entry points of the driver.

--*/
{
    ULONG i;

    UNREFERENCED_PARAMETER (RegistryPath);

    // 
    // Fill in all the dispatch entry points with the pass through function
    // and the explicitly fill in the functions we are going to intercept
    // 
    for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
        DriverObject->MajorFunction[i] = KbFilter_DispatchPassThrough;
    }

    DriverObject->MajorFunction [IRP_MJ_INTERNAL_DEVICE_CONTROL] = KbFilter_InternIoCtl;
    DriverObject->MajorFunction [IRP_MJ_PNP] =          KbFilter_PnP;
    DriverObject->MajorFunction [IRP_MJ_POWER] =        KbFilter_Power;
    DriverObject->DriverUnload = KbFilter_Unload;
    DriverObject->DriverExtension->AddDevice = KbFilter_AddDevice;

    return STATUS_SUCCESS;
}

NTSTATUS
KbFilter_AddDevice(
    IN PDRIVER_OBJECT   Driver,
    IN PDEVICE_OBJECT   PDO
    )
{
    PDEVICE_EXTENSION       devExt;
    IO_ERROR_LOG_PACKET     errorLogEntry;
    PDEVICE_OBJECT          device;
    NTSTATUS                status = STATUS_SUCCESS;

    PAGED_CODE();

    status = IoCreateDevice(Driver,                   
                            sizeof(DEVICE_EXTENSION), 
                            NULL,                    
                            FILE_DEVICE_KEYBOARD,   
                            0,                     
                            FALSE,                
                            &device              
                            );

    if (!NT_SUCCESS(status)) {
        return (status);
    }

    RtlZeroMemory(device->DeviceExtension, sizeof(DEVICE_EXTENSION));

    devExt = (PDEVICE_EXTENSION) device->DeviceExtension;
    devExt->TopOfStack = IoAttachDeviceToDeviceStack(device, PDO);

    if (devExt->TopOfStack == NULL) {
        IoDeleteDevice(device);
        return STATUS_DEVICE_NOT_CONNECTED; 
    }

    
    ASSERT(devExt->TopOfStack);

    devExt->Self =          device;
    devExt->PDO =           PDO;
    KeInitializeSpinLock(&devExt->SpinLock);

    device->Flags |= (DO_BUFFERED_IO | DO_POWER_PAGABLE);
    device->Flags &= ~DO_DEVICE_INITIALIZING;

	// Registrar x36 device
	{
	    PX36DEVICE_EXTENSION	x36DevExt;
		UNICODE_STRING      DeviceName,link;

		RtlInitUnicodeString(&DeviceName, L"\\Device\\X36_Kb");
		RtlInitUnicodeString(&link, L"\\??\\X36KbInterface");

		status = IoCreateDevice(Driver,                   
                            sizeof(X36DEVICE_EXTENSION), 
                            &DeviceName,                    
                            FILE_DEVICE_UNKNOWN,   
                            0,                     
                            FALSE,                
                            &devExt->x36Obj              
                            );
		if (NT_SUCCESS( status )) {
			x36DevExt = (PX36DEVICE_EXTENSION)devExt->x36Obj->DeviceExtension;
			x36DevExt->kDevExt = devExt;

			status = IoCreateSymbolicLink( &link, &DeviceName );
			if ( !NT_SUCCESS( status )) {
				IoDeleteDevice(devExt->x36Obj);
				return status;
	        }

			X36Filter_Iniciar(x36DevExt);

			devExt->x36Obj->Flags |= DO_BUFFERED_IO;
			devExt->x36Obj->Flags &= ~DO_DEVICE_INITIALIZING;
		} 
	}

    return status;
}

VOID
KbFilter_Unload(
   IN PDRIVER_OBJECT Driver
   )
/*++

Routine Description:

   Free all the allocated resources associated with this driver.

Arguments:

   DriverObject - Pointer to the driver object.

Return Value:

   None.

--*/

{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(Driver);

    ASSERT(NULL == Driver->DeviceObject);
}



NTSTATUS
KbFilter_DispatchPassThrough(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
        )
/*++
Routine Description:

    Passes a request on to the lower driver.     
--*/
{
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);

	PAGED_CODE()

	if(DeviceObject->DeviceType==FILE_DEVICE_UNKNOWN)
	{
		return X36Filter_InterfaceDispatch(DeviceObject, Irp);
	}

    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(((PDEVICE_EXTENSION) DeviceObject->DeviceExtension)->TopOfStack, Irp);
}           



NTSTATUS
KbFilter_InternIoCtl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is the dispatch routine for internal device control requests.
    There are two specific control codes that are of interest:
    
    IOCTL_INTERNAL_KEYBOARD_CONNECT:
        Store the old context and function pointer and replace it with our own.
        This makes life much simpler than intercepting IRPs sent by the RIT and
        modifying them on the way back up.
                                      
    IOCTL_INTERNAL_I8042_HOOK_KEYBOARD:
        Add in the necessary function pointers and context values so that we can
        alter how the ps/2 keyboard is initialized.  
                                            
                                         
Arguments:

    DeviceObject - Pointer to the device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/
{
    PIO_STACK_LOCATION              irpStack;
    PDEVICE_EXTENSION               devExt;
    PINTERNAL_I8042_HOOK_KEYBOARD   hookKeyboard; 
    KEVENT                          event;
    PCONNECT_DATA                   connectData;
    NTSTATUS                        status = STATUS_SUCCESS;

    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    Irp->IoStatus.Information = 0;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {

    //
    // Connect a keyboard class device driver to the port driver.
    //
    case IOCTL_INTERNAL_KEYBOARD_CONNECT:
        //
        // Only allow one connection.
        //
        if (devExt->UpperConnectData.ClassService != NULL) {
            status = STATUS_SHARING_VIOLATION;
            break;
        }
        else if (irpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof(CONNECT_DATA)) {
            //
            // invalid buffer
            //
            status = STATUS_INVALID_PARAMETER;
            break;
        }

        //
        // Copy the connection parameters to the device extension.
        //
        connectData = ((PCONNECT_DATA)
            (irpStack->Parameters.DeviceIoControl.Type3InputBuffer));

        devExt->UpperConnectData = *connectData;

        //
        // Hook into the report chain.  Everytime a keyboard packet is reported
        // to the system, KbFilter_ServiceCallback will be called
        //
        connectData->ClassDeviceObject = devExt->Self;
        connectData->ClassService = X36Filter_ServiceCallback;	// Modified for X36 Driver

        break;

    //
    // Disconnect a keyboard class device driver from the port driver.
    //
    case IOCTL_INTERNAL_KEYBOARD_DISCONNECT:
        status = STATUS_NOT_IMPLEMENTED;
        break;

    //
    // Attach this driver to the initialization and byte processing of the 
    // i8042 (ie PS/2) keyboard.  This is only necessary if you want to do PS/2
    // specific functions, otherwise hooking the CONNECT_DATA is sufficient
    //
    case IOCTL_INTERNAL_I8042_HOOK_KEYBOARD:
        if (irpStack->Parameters.DeviceIoControl.InputBufferLength < sizeof(INTERNAL_I8042_HOOK_KEYBOARD)) {
            status = STATUS_INVALID_PARAMETER;
            break;
        }
        hookKeyboard = (PINTERNAL_I8042_HOOK_KEYBOARD) 
        irpStack->Parameters.DeviceIoControl.Type3InputBuffer;         

        //
        // Store all of the other important stuff
        //
        devExt->IsrWritePort = hookKeyboard->IsrWritePort;
//        devExt->QueueKeyboardPacket = hookKeyboard->QueueKeyboardPacket;
        devExt->CallContext = hookKeyboard->CallContext;

        status = STATUS_SUCCESS;
        break;

    case IOCTL_KEYBOARD_QUERY_INDICATOR_TRANSLATION:
    case IOCTL_KEYBOARD_SET_INDICATORS:
    case IOCTL_KEYBOARD_QUERY_ATTRIBUTES:
    case IOCTL_KEYBOARD_QUERY_INDICATORS:
    case IOCTL_KEYBOARD_QUERY_TYPEMATIC:
    case IOCTL_KEYBOARD_SET_TYPEMATIC:
        break;
    }

    if (!NT_SUCCESS(status)) {
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }
	
    return KbFilter_DispatchPassThrough(DeviceObject, Irp);
}



NTSTATUS
KbFilter_PnP(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine is the dispatch routine for plug and play irps 

Arguments:

    DeviceObject - Pointer to the device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/
{
    PDEVICE_EXTENSION           devExt; 
    PIO_STACK_LOCATION          irpStack;
    NTSTATUS                    status = STATUS_SUCCESS;
    KEVENT                      event;        

    PAGED_CODE();

    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    switch (irpStack->MinorFunction) {
    case IRP_MN_START_DEVICE:
    {
        IoCopyCurrentIrpStackLocationToNext(Irp);
        KeInitializeEvent(&event,
                          NotificationEvent,
                          FALSE
                          );

        IoSetCompletionRoutine(Irp,
                               (PIO_COMPLETION_ROUTINE) KbFilter_Complete, 
                               &event,
                               TRUE,
                               TRUE,
                               TRUE); // No need for Cancel

        status = IoCallDriver(devExt->TopOfStack, Irp);

        if (STATUS_PENDING == status) {
            KeWaitForSingleObject(
               &event,
               Executive, // Waiting for reason of a driver
               KernelMode, // Waiting in kernel mode
               FALSE, // No allert
               NULL); // No timeout
        }


		Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        break;
    }

    case IRP_MN_REMOVE_DEVICE:
    {    
        Irp->IoStatus.Status = STATUS_SUCCESS;
        
        IoSkipCurrentIrpStackLocation(Irp);
        status = IoCallDriver(devExt->TopOfStack, Irp);

		// Desactivar interfaz de control
		{
			UNICODE_STRING      SymbolicLinkName;
			UCHAR i=0;
			PX36DEVICE_EXTENSION x36DevExt = (PX36DEVICE_EXTENSION)devExt->x36Obj->DeviceExtension;
			RtlInitUnicodeString(&SymbolicLinkName, L"\\??\\X36KbInterface");
			for(i=0;i<8;i++)
			{
				if(x36DevExt->ratonEv[i]!=NULL)	ObDereferenceObject((PVOID)x36DevExt->ratonEv[i]);
			}
			IoDeleteSymbolicLink(&SymbolicLinkName);
			IoDeleteDevice(devExt->x36Obj);
		}

        IoDetachDevice(devExt->TopOfStack); 
        IoDeleteDevice(DeviceObject);

        break;
    }
    case IRP_MN_SURPRISE_REMOVAL:
    case IRP_MN_QUERY_REMOVE_DEVICE:
    case IRP_MN_QUERY_STOP_DEVICE:
    case IRP_MN_CANCEL_REMOVE_DEVICE:
    case IRP_MN_CANCEL_STOP_DEVICE:
    case IRP_MN_FILTER_RESOURCE_REQUIREMENTS: 
    case IRP_MN_STOP_DEVICE:
    case IRP_MN_QUERY_DEVICE_RELATIONS:
    case IRP_MN_QUERY_INTERFACE:
    case IRP_MN_QUERY_CAPABILITIES:
    case IRP_MN_QUERY_DEVICE_TEXT:
    case IRP_MN_QUERY_RESOURCES:
    case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
    case IRP_MN_READ_CONFIG:
    case IRP_MN_WRITE_CONFIG:
    case IRP_MN_EJECT:
    case IRP_MN_SET_LOCK:
    case IRP_MN_QUERY_ID:
    case IRP_MN_QUERY_PNP_DEVICE_STATE:
    default:
        IoSkipCurrentIrpStackLocation(Irp);
        status = IoCallDriver(devExt->TopOfStack, Irp);
        break;
    }

    return status;
}

NTSTATUS
KbFilter_Complete(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN PVOID            Context
    )
/*++
Routine Description:

    Generic completion routine that allows the driver to send the irp down the 
    stack, catch it on the way up, and do more processing at the original IRQL.
    
--*/
{
    PKEVENT  event;

    event = (PKEVENT) Context;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    KeSetEvent(event, 0, FALSE);

    return STATUS_MORE_PROCESSING_REQUIRED;
}



NTSTATUS
KbFilter_Power(
    IN PDEVICE_OBJECT    DeviceObject,
    IN PIRP              Irp
    )
{
    PDEVICE_EXTENSION           devExt; 

    PAGED_CODE();
	
    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    PoStartNextPowerIrp(Irp);
    IoSkipCurrentIrpStackLocation(Irp);
    return PoCallDriver(devExt->TopOfStack, Irp);
}

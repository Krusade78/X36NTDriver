
/*++

Copyright (c) 1998 - 1999  Microsoft Corporation

Module Name:

    hidgame.c

Abstract: Human Interface Device (HID) Gameport driver

Environment:

    Kernel mode


--*/


#include "hidgame.h"

#ifdef ALLOC_PRAGMA
    #pragma alloc_text( INIT, DriverEntry )
    #pragma alloc_text( PAGE, HGM_CreateClose)
    #pragma alloc_text( PAGE, HGM_AddDevice)
    #pragma alloc_text( PAGE, HGM_Unload)
    #pragma alloc_text( PAGE, HGM_SystemControl)
#endif /* ALLOC_PRAGMA */

HIDGAME_GLOBAL Global;

/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   NTSTATUS  | DriverEntry |
 *
 *          Installable driver initialization entry point.
 *          <nl>This entry point is called directly by the I/O system.
 *
 *  @parm   IN PDRIVER_OBJECT | DriverObject |
 *
 *          Pointer to the driver object
 *
 *  @parm   IN PUNICODE_STRING | RegistryPath |
 *
 *          Pointer to a unicode string representing the path,
 *          to driver-specific key in the registry.
 *
 *  @rvalue   STATUS_SUCCESS | success
 *  @rvalue   ???            | returned HidRegisterMinidriver()
 *
 *****************************************************************************/
NTSTATUS EXTERNAL
    DriverEntry
    (
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS                        ntStatus;
    HID_MINIDRIVER_REGISTRATION     hidMinidriverRegistration;

    ntStatus = HGM_DriverInit();

    if( NT_SUCCESS(ntStatus) )
    {
        DriverObject->MajorFunction[IRP_MJ_CREATE]    =
            DriverObject->MajorFunction[IRP_MJ_CLOSE] = HGM_CreateClose;
        DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] =
        DriverObject->MajorFunction[IRP_MJ_WRITE] =
            HGM_InternalIoctl;
        DriverObject->MajorFunction[IRP_MJ_PNP]   = HGM_PnP;
        DriverObject->MajorFunction[IRP_MJ_POWER] = HGM_Power;
        DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = HGM_SystemControl;
        DriverObject->DriverUnload                = HGM_Unload;
        DriverObject->DriverExtension->AddDevice  = HGM_AddDevice;

        /*
         * Register  with HID.SYS module
         */
        RtlZeroMemory(&hidMinidriverRegistration, sizeof(hidMinidriverRegistration));

        hidMinidriverRegistration.Revision            = HID_REVISION;
        hidMinidriverRegistration.DriverObject        = DriverObject;
        hidMinidriverRegistration.RegistryPath        = RegistryPath;
        hidMinidriverRegistration.DeviceExtensionSize = sizeof(DEVICE_EXTENSION);
        hidMinidriverRegistration.DevicesArePolled    = TRUE;


        ntStatus = HidRegisterMinidriver(&hidMinidriverRegistration);

        if( NT_SUCCESS(ntStatus) )
        {
            /*
             *  Protect the list with a Mutex
             */
            ExInitializeFastMutex (&Global.Mutex);

            /*
             *  Initialize the device list head
             */
            InitializeListHead(&Global.DeviceListHead);

            /*
             *  Initialize gameport access spinlock
             */
            KeInitializeSpinLock(&Global.SpinLock);
        }
    }

    return ntStatus;
} /* DriverEntry */


/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   NTSTATUS  | HGM_CreateClose |
 *
 *          Process the create and close IRPs sent to this device.
 *
 *  @parm   IN PDEVICE_OBJECT | DeviceObject |
 *
 *          Pointer to the device object
 *
 *  @parm   IN PIRP | Irp |
 *
 *          Pointer to an I/O Request Packet.
 *
 *  @rvalue   STATUS_SUCCESS | success
 *  @rvalue   STATUS_INVALID_PARAMETER  | Irp not handled
 *
 *****************************************************************************/
NTSTATUS EXTERNAL
    HGM_CreateClose
    (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PIO_STACK_LOCATION   IrpStack;
    NTSTATUS             ntStatus = STATUS_SUCCESS;

    PAGED_CODE ();

    /*
     * Get a pointer to the current location in the Irp.
     */
    IrpStack = IoGetCurrentIrpStackLocation(Irp);

    switch(IrpStack->MajorFunction)
    {
        case IRP_MJ_CREATE:
            Irp->IoStatus.Information = 0;
            break;

        case IRP_MJ_CLOSE:
            Irp->IoStatus.Information = 0;
            break;

        default:
            ntStatus = STATUS_INVALID_PARAMETER;
            break;
    }

    /*
     * Save Status for return and complete Irp
     */

    Irp->IoStatus.Status = ntStatus;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return ntStatus;
} /* HGM_CreateClose */


/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   NTSTATUS  | HGM_AddDevice |
 *
 *          Called by hidclass, allows us to initialize our device extensions.
 *
 *  @parm   IN PDRIVER_OBJECT | DriverObject |
 *
 *          Pointer to the driver object
 *
 *  @parm   IN PDEVICE_OBJECT | FunctionalDeviceObject |
 *
 *          Pointer to a functional device object created by hidclass.
 *
 *  @rvalue   STATUS_SUCCESS | success
 *
 *****************************************************************************/
NTSTATUS  EXTERNAL
    HGM_AddDevice
    (
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT FunctionalDeviceObject
    )
{
    NTSTATUS                ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT          DeviceObject;
    PDEVICE_EXTENSION       DeviceExtension;

    PAGED_CODE ();

    DeviceObject = FunctionalDeviceObject;

    /*
     * Initialize the device extension.
     */
    DeviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION (DeviceObject);

    DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    /*
     * Initialize the list
     */
    InitializeListHead(&DeviceExtension->Link);

    /*
     *  Acquire mutex before modifying the Global Linked list of devices
     */
    ExAcquireFastMutex (&Global.Mutex);

    /*
     * Add this device to the linked list of devices
     */
    InsertTailList(&Global.DeviceListHead, &DeviceExtension->Link);

    /*
     *  Release the mutex
     */
    ExReleaseFastMutex (&Global.Mutex);

    /*
     * Initialize the remove lock 
     */
    DeviceExtension->RequestCount = 1;
    KeInitializeEvent(&DeviceExtension->RemoveEvent,
                      SynchronizationEvent,
                      FALSE);

    return ntStatus;
} /* HGM_AddDevice */


/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   NTSTATUS  | HGM_SystemControl |
 *
 *          Process the WMI IRPs sent to this device.
 *
 *  @parm   IN PDEVICE_OBJECT | DeviceObject |
 *
 *          Pointer to the device object
 *
 *  @parm   IN PIRP | Irp |
 *
 *          Pointer to an I/O Request Packet.
 *
 *  @rvalue   STATUS_SUCCESS | success
 *  @rvalue   STATUS_INVALID_PARAMETER  | Irp not handled
 *
 *****************************************************************************/
NTSTATUS EXTERNAL
    HGM_SystemControl
    (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PAGED_CODE ();

    IoSkipCurrentIrpStackLocation(Irp);

    return IoCallDriver(GET_NEXT_DEVICE_OBJECT(DeviceObject), Irp);
} /* HGM_SystemControl */

/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   void  | HGM_Unload |
 *
 *          Free all the allocated resources, etc.
 *
 *  @parm   IN PDRIVER_OBJECT | DeviceObject |
 *
 *          Pointer to the driver object
 *
 *
 *****************************************************************************/
VOID EXTERNAL
    HGM_Unload
    (
    IN PDRIVER_OBJECT DriverObject
    )
{
    PAGED_CODE();

    /*
     * All the device objects should be gone
     */

    ASSERT ( NULL == DriverObject->DeviceObject);

    return;
} /* HGM_Unload */



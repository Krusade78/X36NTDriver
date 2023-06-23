#include "hidgame.h"

NTSTATUS
QueryValueKey (
    IN  HANDLE  Handle,
    IN  PWCHAR  ValueNameString,
    OUT PVOID   Data,
    IN  ULONG   DataLength
    );

#ifdef ALLOC_PRAGMA
    #pragma alloc_text( PAGE, LeerRegistro)
    #pragma alloc_text( PAGE, QueryValueKey)
#endif /* ALLOC_PRAGMA */


NTSTATUS
QueryValueKey (
    IN  HANDLE  Handle,
    IN  PWCHAR  ValueNameString,
    OUT PVOID   Data,
    IN  ULONG   DataLength
    )
{
    NTSTATUS        status;
    UNICODE_STRING  valueName;
    ULONG           length;
    PKEY_VALUE_FULL_INFORMATION fullInfo;

    PAGED_CODE();

    RtlInitUnicodeString (&valueName, ValueNameString);

    length = sizeof (KEY_VALUE_FULL_INFORMATION)
           + valueName.MaximumLength
           + DataLength;

    fullInfo = ExAllocatePool (PagedPool, length);

    if (fullInfo) {
        status = ZwQueryValueKey (Handle,
                                  &valueName,
                                  KeyValueFullInformation,
                                  fullInfo,
                                  length,
                                  &length);

        if (NT_SUCCESS (status)) {
            ASSERT (DataLength == fullInfo->DataLength);
            RtlCopyMemory (Data,
                           ((PUCHAR) fullInfo) + fullInfo->DataOffset,
                           fullInfo->DataLength);
        }

        ExFreePool (fullInfo);
    } else {
        status = STATUS_NO_MEMORY;
    }

    return status;
}


VOID
LeerRegistro ( PDEVICE_EXTENSION devExt)
{
    NTSTATUS        sta;
	UNICODE_STRING  keyst;
	OBJECT_ATTRIBUTES oa;
	HANDLE key;

	PAGED_CODE();

    RtlInitUnicodeString(&keyst, L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_06A3&PID_3635");
    InitializeObjectAttributes (&oa,
                                &keyst,
                                OBJ_CASE_INSENSITIVE,
                                NULL,
                                (PSECURITY_DESCRIPTOR) NULL);
	sta = ZwOpenKey (&key, KEY_READ, &oa);
	if(!NT_SUCCESS(sta)) { goto error0; }

        sta=QueryValueKey(key,L"Limite_x_i",&devExt->Limite[0].i,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[0].i>20000) || (devExt->Limite[0].i<0)) { goto error1; }
        sta=QueryValueKey(key,L"Limite_x_c",&devExt->Limite[0].c,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[0].c>20000) || (devExt->Limite[0].c<0)) { goto error1; }
        sta=QueryValueKey(key,L"Limite_x_d",&devExt->Limite[0].d,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[0].d>20000) || (devExt->Limite[0].d<0)) { goto error1; }

        sta=QueryValueKey(key,L"Limite_y_i",&devExt->Limite[1].i,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[1].i>20000) || (devExt->Limite[1].i<0)) { goto error1; }
        sta=QueryValueKey(key,L"Limite_y_c",&devExt->Limite[1].c,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[1].c>20000) || (devExt->Limite[1].c<0)) { goto error1; }
        sta=QueryValueKey(key,L"Limite_y_d",&devExt->Limite[1].d,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[1].d>20000) || (devExt->Limite[1].d<0)) { goto error1; }

        sta=QueryValueKey(key,L"Limite_z_i",&devExt->Limite[2].i,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[2].i>20000) || (devExt->Limite[2].i<0)) { goto error1; }
        sta=QueryValueKey(key,L"Limite_z_c",&devExt->Limite[2].c,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[2].c>20000) || (devExt->Limite[2].c<0)) { goto error1; }
        sta=QueryValueKey(key,L"Limite_z_d",&devExt->Limite[2].d,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[2].d>20000) || (devExt->Limite[2].d<0)) { goto error1; }

        sta=QueryValueKey(key,L"Limite_r_i",&devExt->Limite[3].i,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[3].i>20000) || (devExt->Limite[3].i<0)) { goto error1; }
        sta=QueryValueKey(key,L"Limite_r_c",&devExt->Limite[3].c,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[3].c>20000) || (devExt->Limite[3].c<0)) { goto error1; }
        sta=QueryValueKey(key,L"Limite_r_d",&devExt->Limite[3].d,sizeof(ULONG));
        if(!NT_SUCCESS(sta) || (devExt->Limite[3].d>20000) || (devExt->Limite[3].d<0)) { goto error1; }


        sta=QueryValueKey(key,L"ZonaNula_x",&devExt->ZonaNula[0],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }
        sta=QueryValueKey(key,L"ZonaNula_y",&devExt->ZonaNula[1],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }
        sta=QueryValueKey(key,L"ZonaNula_z",&devExt->ZonaNula[2],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }
        sta=QueryValueKey(key,L"ZonaNula_r",&devExt->ZonaNula[3],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }

        sta=QueryValueKey(key,L"Escala_der_x",&devExt->Escala_der[0],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }
        sta=QueryValueKey(key,L"Escala_der_y",&devExt->Escala_der[1],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }
        sta=QueryValueKey(key,L"Escala_der_z",&devExt->Escala_der[2],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }
        sta=QueryValueKey(key,L"Escala_der_r",&devExt->Escala_der[3],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }

        sta=QueryValueKey(key,L"Escala_izq_x",&devExt->Escala_izq[0],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }
        sta=QueryValueKey(key,L"Escala_izq_y",&devExt->Escala_izq[1],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }
        sta=QueryValueKey(key,L"Escala_izq_z",&devExt->Escala_izq[2],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }
        sta=QueryValueKey(key,L"Escala_izq_r",&devExt->Escala_izq[3],sizeof(UCHAR));
        if(!NT_SUCCESS(sta)) { goto error1; }

	ZwClose(key);
        RtlFillMemory(devExt->LastOldAxis,sizeof(LONG)*4,200);
	devExt->calibrado=TRUE;
	return;
error1:
	ZwClose(key);
error0:
	devExt->calibrado=FALSE;
	RtlZeroMemory(devExt->Limite,sizeof(ULONG)*12);
	RtlZeroMemory(devExt->ZonaNula,sizeof(UCHAR)*4);
	RtlZeroMemory(devExt->Escala_izq,sizeof(UCHAR)*4);
	RtlZeroMemory(devExt->Escala_der,sizeof(UCHAR)*4);
}

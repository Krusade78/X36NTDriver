
/*++

Copyright (c) 1998 - 1999  Microsoft Corporation

Module Name:

    pnp.c

Abstract: This module contains routines Generate the HID report and
    configure the joystick.

Environment:

    Kernel mode


--*/

#include "hidgame.h"

#define IOCTL_HID_LEER	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0100, METHOD_NEITHER, FILE_READ_ACCESS)

#ifdef ALLOC_PRAGMA
    #pragma alloc_text (INIT, HGM_DriverInit)
    #pragma alloc_text (PAGE, HGM_MapAxesFromDevExt)
    #pragma alloc_text (PAGE, HGM_GenerateReport)
    #pragma alloc_text (PAGE, HGM_JoystickConfig)
    #pragma alloc_text (PAGE, HGM_InitAnalog)
    #pragma alloc_text (PAGE, HGM_ChangeHandler)
    #pragma alloc_text (PAGE, HGM_DeviceRead)
#endif





/*
 *  A few look up tables to translate the JOY_HWS_* flags into axis masks.
 *  These flags allow any axis to be polled on any of the four axis bits in 
 *  the gameport.  For example, the X axis on a standard joystick is found on 
 *  bit 0 (LSB) and the Y axis is on bit 1; however many steering wheel/pedal 
 *  controllers have X on bit 0 but Y on bit 2.  Although very few of these
 *  combinations are known to be used, supporting all the flags only causes a 
 *  little extra work on setup.  For each axis, there are three flags, one for 
 *  each of the possible non-standard bit masks.  Since it is possible that 
 *  more than one of these may be set the invalid combinations are marked so 
 *  that they can be refused.
 */



#define NA ( 0x80 )

/*
 *  Short versions of bit masks for axes
 */
#define X1 AXIS_X
#define Y1 AXIS_Y
#define X2 AXIS_R
#define Y2 AXIS_Z

/*
 *  Per axis flag masks and look up tables.
 *  In each case, combinations with more than one bit set are invalid
 */
#define XMAPBITS    (JOY_HWS_XISJ2Y |   JOY_HWS_XISJ2X |   JOY_HWS_XISJ1Y)
/*
 *                          0                   0                   0           0001
 *                          0                   0                   1           0010
 *                          0                   1                   0           0100
 *                          1                   0                   0           1000
 */
static const unsigned char XLU[8] = { X1,Y1,X2,NA,Y2,NA,NA,NA };
#define XMAPSHFT 7

#define YMAPBITS    (JOY_HWS_YISJ2Y |   JOY_HWS_YISJ2X |   JOY_HWS_YISJ1X)
/*                          0                   0                   0           0010
 *                          0                   0                   1           0001
 *                          0                   1                   0           0100
 *                          1                   0                   0           1000
 */
static const unsigned char YLU[8] = { Y1,X1,X2,NA,Y2,NA,NA,NA };
#define YMAPSHFT 10

#define RMAPBITS    (JOY_HWS_RISJ2Y |   JOY_HWS_RISJ1X |   JOY_HWS_RISJ1Y)
/*                          0                   0                   0           0100
 *                          0                   0                   1           0010
 *                          0                   1                   0           0001
 *                          1                   0                   0           1000
 */
static const unsigned char RLU[8] = { X2,Y1,X1,NA,Y2,NA,NA,NA };
#define RMAPSHFT 20

#define ZMAPBITS    (JOY_HWS_ZISJ2X |   JOY_HWS_ZISJ1X |   JOY_HWS_ZISJ1Y)
/*                          0                   0                   0           1000
 *                          0                   0                   1           0010
 *                          0                   1                   0           0001
 *                          1                   0                   0           0100
 */
static const unsigned char ZLU[8] = { Y2,Y1,X1,NA,X2,NA,NA,NA };
#define ZMAPSHFT 13

#undef X1
#undef Y1
#undef X2
#undef Y2

/*
 *  This translates from an axis bitmask to an axis value index.  The elements 
 *  used should be as follows (X marks unsed)   { X, 0, 1, X, 2, X, X, X, 3 }.
 */
static const unsigned char cAxisIndexTable[9] = { 0, 0, 1, 0, 2, 0, 0, 0, 3 };



/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   NTSTATUS | HGM_DriverInit |
 *
 *          Perform global initialization.
 *          <nl>This is called from DriverEntry.  Try to initialize a CPU 
 *          specific timer but if it fails set up default
 *
 *  @rvalue   STATUS_SUCCESS | success
 *  @rvalue   STATUS_UNSUCCESSFUL | not success
 *
 *****************************************************************************/
NTSTATUS EXTERNAL
    HGM_DriverInit()
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    if( !HGM_CPUCounterInit() )
    {
        LARGE_INTEGER QPCFrequency;

        KeQueryPerformanceCounter( &QPCFrequency );

        if( ( QPCFrequency.HighPart == 0 )
         && ( QPCFrequency.LowPart <= 10000 ) )
        {
            ntStatus = STATUS_UNSUCCESSFUL;
        }
        else
        {
            Global.CounterScale = CALCULATE_SCALE( QPCFrequency.QuadPart );
            Global.ReadCounter = (COUNTER_FUNCTION)&KeQueryPerformanceCounter;
        }
    }

    return ntStatus;
}


/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   NTSTATUS  | HGM_MapAxesFromDevExt |
 *
 *          Use the flags in the DeviceExtension to generate mappings for each 
 *          axis.  
 *          <nl>This is called both from HGM_JoystickConfig to validate the 
 *          configuration and HGM_GenerateReport to use the axis maps.
 *          
 *
 *  @parm   IN OUT PDEVICE_EXTENSION | DeviceExtension |
 *
 *          Pointer to the minidriver device extension
 *
 *  @rvalue   STATUS_SUCCESS | success
 *  @rvalue   STATUS_DEVICE_CONFIGURATION_ERROR | The configuration is invalid
 *
 *****************************************************************************/
NTSTATUS EXTERNAL
    HGM_MapAxesFromDevExt
    (
    IN OUT PDEVICE_EXTENSION DeviceExtension 
    )
{
    NTSTATUS    ntStatus;
    ULONG       dwFlags;
    int         nAxis;
    UCHAR       AxisMask;

    ntStatus = STATUS_SUCCESS;

    dwFlags = DeviceExtension->HidGameOemData.OemData[(DeviceExtension->fSiblingFound!=0)].joy_hws_dwFlags;  

#define XIS (0)
#define YIS (1)
#define ZIS (2)
#define RIS (3)

    /* 
     *  Check X and Y last as Z, R and POV must not overlap
     *  The are no flags to indicate the presence of X or Y so if they 
     *  overlap, this indicates that they are not used,
     */

    DeviceExtension->resistiveInputMask = 0;
    for( nAxis=MAX_AXES; nAxis>=0; nAxis-- )
    {
        DeviceExtension->AxisMap[nAxis] = INVALID_INDEX;
    }
    nAxis = 0;

    if( dwFlags & JOY_HWS_HASZ )
    {
        AxisMask = ZLU[(dwFlags & ZMAPBITS) >> ZMAPSHFT];
        if( AxisMask >= NA )
        {
            ntStatus = STATUS_DEVICE_CONFIGURATION_ERROR; 
        }
        else
        {
            nAxis = 1;
            DeviceExtension->resistiveInputMask = AxisMask;
            DeviceExtension->AxisMap[ZIS] = cAxisIndexTable[AxisMask];
        }
    }


    if( dwFlags & JOY_HWS_HASR )
    {
        AxisMask = RLU[(dwFlags & RMAPBITS) >> RMAPSHFT];
        if( AxisMask >= NA )
        {
            ntStatus = STATUS_DEVICE_CONFIGURATION_ERROR; 
        }
        else
        {
            if( DeviceExtension->resistiveInputMask & AxisMask )
            {
                ntStatus = STATUS_DEVICE_CONFIGURATION_ERROR ; 
            }
            else
            {
                nAxis++;
                DeviceExtension->resistiveInputMask |= AxisMask;
                DeviceExtension->AxisMap[RIS] = cAxisIndexTable[AxisMask];
            }
        }
    }


    AxisMask = XLU[( dwFlags & XMAPBITS ) >> XMAPSHFT];
    if( AxisMask >= NA )
    {
        ntStatus = STATUS_DEVICE_CONFIGURATION_ERROR ; 
    }
    else
    {
        if( DeviceExtension->resistiveInputMask & AxisMask )
        {
            //("HGM_MapAxesFromDevExt: X axis mapped to same as another axis") );
        }
        else
        {
            nAxis++;
            DeviceExtension->resistiveInputMask |= AxisMask;
            DeviceExtension->AxisMap[XIS] = cAxisIndexTable[AxisMask];
        }
    }


    AxisMask = YLU[( dwFlags & YMAPBITS ) >> YMAPSHFT];
    if( AxisMask >= NA )
    {
        ntStatus = STATUS_DEVICE_CONFIGURATION_ERROR ; 
    }
    else
    {
        if( DeviceExtension->resistiveInputMask & AxisMask )
        {
            //("HGM_MapAxesFromDevExt: Y axis mapped to same as another axis") );
        }
        else
        {
            nAxis++;
            DeviceExtension->resistiveInputMask |= AxisMask;
            DeviceExtension->AxisMap[YIS] = cAxisIndexTable[AxisMask];
        }
    }

#undef XIS
#undef YIS
#undef ZIS
#undef RIS

#undef NA

    /*
     *  Don't fail for this if CHANGE_DEVICE is defined because an exposed 
     *  sibling will always have an nAxis of zero.
     */
        if( nAxis != DeviceExtension->nAxes )
        {
            ntStatus = STATUS_DEVICE_CONFIGURATION_ERROR ;
        }

    LeerRegistro(DeviceExtension);
	DeviceExtension->AxisMapV[0]=0;
	DeviceExtension->AxisMapV[1]=1;
	DeviceExtension->AxisMapV[2]=2;
	DeviceExtension->AxisMapV[3]=3;
//        RtlZeroMemory(DeviceExtension->LastGoodBtHat,7);

    return( ntStatus );
} /* HGM_MapAxesFromDevExt */


/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   NTSTATUS  | HGM_GenerateReport |
 *
 *          Generates a hid report descriptor for a n-axis, m-button joystick,
 *          depending on number of buttons and joy_hws_flags field.
 *
 *  @parm   IN PDEVICE_OBJECT | DeviceObject |
 *
 *          Pointer to the device object
 *
 *  @parm   IN OUT UCHAR * | rgGameReport[MAXBYTES_GAME_REPORT] |
 *
 *          Array that receives the HID report descriptor
 *
 *  @parm   OUT PUSHORT | pCbReport |
 *          
 *          Address of a short integer that receives size of 
 *          HID report descriptor. 
 *
 *  @rvalue   STATUS_SUCCESS  | success
 *  @rvalue   STATUS_BUFFER_TOO_SMALL  | Need more memory for HID descriptor
 *
 *****************************************************************************/
NTSTATUS INTERNAL
    HGM_GenerateReport
    (
    IN PDEVICE_OBJECT       DeviceObject, 
    OUT UCHAR               rgGameReport[MAXBYTES_GAME_REPORT],
    OUT PUSHORT             pCbReport
    )
{
    NTSTATUS    ntStatus;
    PDEVICE_EXTENSION DeviceExtension;
    UCHAR       *pucReport; 
    int         Idx;
    int         UsageIdx;
    POEMDATA    OemData;

    int         InitialAxisMappings[MAX_AXES];


    typedef struct _USAGES
    {
        UCHAR UsagePage;
        UCHAR Usage;
    } USAGES, *PUSAGE;

	USAGES FlightYokeParams[4]={
		{HID_USAGE_PAGE_GENERIC,HID_USAGE_GENERIC_X},
		{HID_USAGE_PAGE_GENERIC,HID_USAGE_GENERIC_Y},
		{HID_USAGE_PAGE_SIMULATION,HID_USAGE_SIMULATION_THROTTLE},
		{HID_USAGE_PAGE_SIMULATION,HID_USAGE_SIMULATION_RUDDER}
		};

    PAGED_CODE();


    /*
     *  Get a pointer to the device extension
     */

    DeviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    /*
     *  Although the axes have already been validated and mapped in 
     *  HGM_JoystickConfig this function destroys the mapping when it compacts 
     *  the axes towards the start of the descriptor report.  Since this 
     *  function will be called once to find the descriptor length and then 
     *  again to read the report, the mappings are regenerated again each 
     *  time through.  Although this results in the parameters being 
     *  interpreted three times (for validation, descriptor size and 
     *  descriptor content) it avoids the possibility of a discrepancy in 
     *  implementation of separate functions.
     */

    ntStatus = HGM_MapAxesFromDevExt( DeviceExtension );

    pucReport = rgGameReport;


#define NEXT_BYTE( pReport, Data )   \
            *pReport++ = Data;    

#define NEXT_LONG( pReport, Data )   \
            *(((LONG UNALIGNED*)(pReport))++) = Data;

#define ITEM_DEFAULT        0x00 /* Data, Array, Absolute, No Wrap, Linear, Preferred State, Has no NULL */
#define ITEM_VARIABLE       0x02 /* as ITEM_DEFAULT but value is a variable, not an array */
#define ITEM_HASNULL        0x40 /* as ITEM_DEFAULT but values out of range are considered NULL */
#define ITEM_ANALOG_AXIS    ITEM_VARIABLE
#define ITEM_DIGITAL_POV    (ITEM_VARIABLE|ITEM_HASNULL)
#define ITEM_BUTTON         ITEM_VARIABLE
#define ITEM_PADDING        0x01 /* Constant (nothing else applies) */


    /* USAGE_PAGE (Generic Desktop) */
    NEXT_BYTE(pucReport,    HIDP_GLOBAL_USAGE_PAGE_1);
    NEXT_BYTE(pucReport,    HID_USAGE_PAGE_GENERIC);

    /* USAGE (Joystick ) */
    NEXT_BYTE(pucReport,    HIDP_LOCAL_USAGE_1);
    NEXT_BYTE(pucReport,    HID_USAGE_GENERIC_JOYSTICK);

    NEXT_BYTE(pucReport,    HIDP_MAIN_COLLECTION); 
    NEXT_BYTE(pucReport,    0x1 ); 

	/* Logical Min is the smallest value that could be produced by a poll */
    NEXT_BYTE(pucReport,    HIDP_GLOBAL_LOG_MIN_4);
    NEXT_LONG(pucReport,    0 );

    /* Logical Max is the largest value that could be produced by a poll */
    NEXT_BYTE(pucReport,    HIDP_GLOBAL_LOG_MAX_4);
    NEXT_LONG(pucReport,    AXIS_FULL_SCALE );


    /* Define one axis at a time */
    NEXT_BYTE(pucReport,    HIDP_GLOBAL_REPORT_COUNT_1);
    NEXT_BYTE(pucReport,    0x1);  

    /* Each axis is a 32 bits value */
    NEXT_BYTE(pucReport,    HIDP_GLOBAL_REPORT_SIZE);
    NEXT_BYTE(pucReport,    8 * sizeof(ULONG) );

	/* 
     *  Do the axis 
     *  Although HID could cope with the "active" axes being mixed with the 
     *  dummy ones, it makes life simpler to move them to the start.
     *  Pass through all the axis maps generated by HGM_JoystickConfig 
     *  and map all the active ones into the descriptor, copying the usages 
     *  appropriate for the type of device.
     *  Since a polled POV is nothing more than a different interpretation 
     *  of axis data, this is added after any axes.
     */
    RtlCopyMemory( InitialAxisMappings, DeviceExtension->AxisMap, sizeof( InitialAxisMappings ) );



    Idx = 0;
    for( UsageIdx = 0; UsageIdx < MAX_AXES; UsageIdx++ )
    {
        if( InitialAxisMappings[UsageIdx] >= INVALID_INDEX )
        {
            continue;
        }

        DeviceExtension->AxisMap[Idx] = InitialAxisMappings[UsageIdx];

        NEXT_BYTE(pucReport,    HIDP_LOCAL_USAGE_1);
        NEXT_BYTE(pucReport,   FlightYokeParams[UsageIdx].Usage);
        NEXT_BYTE(pucReport,    HIDP_GLOBAL_USAGE_PAGE_1);
        NEXT_BYTE(pucReport,   FlightYokeParams[UsageIdx].UsagePage);

        /* Data Field */
        NEXT_BYTE(pucReport,    HIDP_MAIN_INPUT_1);
        NEXT_BYTE(pucReport,    ITEM_ANALOG_AXIS);

        Idx++;
    }

    /*
     *  Now fill in any remaining axis values as dummys
     */
    while( Idx < MAX_AXES )
    {
        /* Constant Field */
        NEXT_BYTE(pucReport,    HIDP_MAIN_INPUT_1);
        NEXT_BYTE(pucReport,    ITEM_PADDING);

        Idx++;
    }
        

	/* POVS */
    {
        /*
         *  Redefine the logical and physical ranges from now on 
         *  A digital POV has a NULL value (a value outside the logical range) 
         *  when the POV is centered.  To make life easier call the NULL value 
         *  zero, so the logical range is from 1 to 4.

        /* Logical Min */
        NEXT_BYTE(pucReport,    HIDP_GLOBAL_LOG_MIN_1);
        NEXT_BYTE(pucReport,    1 );

        /* Logical Max */
        NEXT_BYTE(pucReport,    HIDP_GLOBAL_LOG_MAX_1);
        NEXT_BYTE(pucReport,    8 );

        /* 
         *  report for digital POV is 3 bits data plus 5 constant bits to fill 
         *  the byte.  
         */
        NEXT_BYTE(pucReport,    HIDP_LOCAL_USAGE_1);
        NEXT_BYTE(pucReport,    HID_USAGE_GENERIC_HATSWITCH);
        NEXT_BYTE(pucReport,    HIDP_GLOBAL_USAGE_PAGE_1);
        NEXT_BYTE(pucReport,    HID_USAGE_PAGE_GENERIC);
        NEXT_BYTE(pucReport,    HIDP_GLOBAL_REPORT_SIZE);
        NEXT_BYTE(pucReport,    0x8);

        /* Data Field */
        NEXT_BYTE(pucReport,    HIDP_MAIN_INPUT_1);
        NEXT_BYTE(pucReport,    ITEM_DIGITAL_POV);

        NEXT_BYTE(pucReport,    HIDP_LOCAL_USAGE_1);
        NEXT_BYTE(pucReport,    HID_USAGE_GENERIC_HATSWITCH);
        NEXT_BYTE(pucReport,    HIDP_MAIN_INPUT_1);
        NEXT_BYTE(pucReport,    ITEM_DIGITAL_POV);

    } 

    /* 
     * Now the buttons 
     */
    NEXT_BYTE(pucReport,    HIDP_GLOBAL_USAGE_PAGE_1);
    NEXT_BYTE(pucReport,    HID_USAGE_PAGE_BUTTON);
    NEXT_BYTE(pucReport,    HIDP_LOCAL_USAGE_MIN_1);
    NEXT_BYTE(pucReport,    1);
    NEXT_BYTE(pucReport,    HIDP_LOCAL_USAGE_MAX_1);
    NEXT_BYTE(pucReport,    (UCHAR)DeviceExtension->nButtons );
    NEXT_BYTE(pucReport,    HIDP_GLOBAL_LOG_MIN_1);
    NEXT_BYTE(pucReport,    0 );
    NEXT_BYTE(pucReport,    HIDP_GLOBAL_LOG_MAX_1);
    NEXT_BYTE(pucReport,    1 );
	NEXT_BYTE(pucReport,    HIDP_GLOBAL_REPORT_SIZE);
    NEXT_BYTE(pucReport,    1);
    NEXT_BYTE(pucReport,    HIDP_GLOBAL_REPORT_COUNT_1);
    NEXT_BYTE(pucReport,    (UCHAR)DeviceExtension->nButtons);  
    NEXT_BYTE(pucReport,    HIDP_MAIN_INPUT_1);
    NEXT_BYTE(pucReport,    ITEM_BUTTON);

    if( DeviceExtension->nButtons <40 )
    {
        /* Constant report for 8 * unused buttons bits */
	    NEXT_BYTE(pucReport,    HIDP_GLOBAL_REPORT_COUNT_1);
		NEXT_BYTE(pucReport,    1);  
        NEXT_BYTE(pucReport,    HIDP_GLOBAL_REPORT_SIZE);
        NEXT_BYTE(pucReport,    (UCHAR)((40-DeviceExtension->nButtons)*1) );

        /* Constant Field */
        NEXT_BYTE(pucReport,    HIDP_MAIN_INPUT_1);
        NEXT_BYTE(pucReport,    ITEM_PADDING);
    }

    /* End of collection,  We're done ! */
    NEXT_BYTE(pucReport,  HIDP_MAIN_ENDCOLLECTION); 

#undef NEXT_BYTE
#undef NEXT_LONG

    if( pucReport - rgGameReport > MAXBYTES_GAME_REPORT)
    {
        ntStatus   = STATUS_BUFFER_TOO_SMALL;
        *pCbReport = 0x0;
        RtlZeroMemory(rgGameReport, sizeof(rgGameReport));
    } else
    {
        *pCbReport = (USHORT) (pucReport - rgGameReport);
        ntStatus = STATUS_SUCCESS;
    }

    return ( ntStatus );
} /* HGM_GenerateReport */



/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   NTSTATUS  | HGM_JoystickConfig |
 *
 *          Check that the configuration is valid whilst there is still time 
 *          to refuse it.
 *          <nl>HGM_GenerateReport uses the results generated here if the 
 *          settings are OK.
 *
 *  @parm   IN PDEVICE_OBJECT | DeviceObject |
 *
 *          Pointer to the device object
 *
 *  @rvalue   STATUS_SUCCESS  | success
 *  @rvalue   STATUS_DEVICE_CONFIGURATION_ERROR  | Invalid configuration specified
 *
 *****************************************************************************/
NTSTATUS INTERNAL
    HGM_JoystickConfig 
    (
    IN PDEVICE_OBJECT         DeviceObject
    )
{
    PDEVICE_EXTENSION   DeviceExtension;
    POEMDATA            OemData;
    NTSTATUS            ntStatus;
    int                 Idx;

    PAGED_CODE();


    /*
     * Get a pointer to the device extension
     */

    DeviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    ntStatus = HGM_MapAxesFromDevExt( DeviceExtension );

    if( DeviceExtension->ReadAccessorDigital )
    {
        DeviceExtension->ScaledTimeout = AXIS_TIMEOUT;
    }
    else
    {
        /*
         * Calculate time thresholds for analog device
         */
        if( ( DeviceExtension->HidGameOemData.OemData[0].Timeout < ANALOG_POLL_TIMEOUT_MIN )
          ||( DeviceExtension->HidGameOemData.OemData[0].Timeout > ANALOG_POLL_TIMEOUT_MAX ) )
        {
            DeviceExtension->ScaledTimeout = (ULONG)( ( (ULONGLONG)ANALOG_POLL_TIMEOUT_DFT
                                                      * (ULONGLONG)(AXIS_FULL_SCALE<<SCALE_SHIFT) )
                                                    / (ULONGLONG)ANALOG_POLL_TIMEOUT_MAX );
        }
        else
        {
            DeviceExtension->ScaledTimeout = (ULONG)( ( (ULONGLONG)DeviceExtension->HidGameOemData.OemData[0].Timeout
                                                      * (ULONGLONG)(AXIS_FULL_SCALE<<SCALE_SHIFT) )
                                                    / (ULONGLONG)ANALOG_POLL_TIMEOUT_MAX );
        }

        /*
         *  Use one quarter of the minimum poll timeout as a starting value 
         *  for the time between two polls which will be considered to have 
         *  been interrupted.
         */
        DeviceExtension->ScaledThreshold = (ULONG)( ( (ULONGLONG)ANALOG_POLL_TIMEOUT_MIN
                                                    * (ULONGLONG)AXIS_FULL_SCALE )
                                                  / (ULONGLONG)ANALOG_POLL_TIMEOUT_MAX )>>2;
    }


    /*
     *  Set initial values of LastGoodAxis so that the device will not show
     *  up as present until we get at least one valid poll.
     */
    for( Idx = MAX_AXES; Idx >= 0; Idx-- )
    {
        DeviceExtension->LastGoodAxis[Idx] = AXIS_TIMEOUT;
    }

    return ntStatus;
} /* HGM_JoystickConfig */


/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   NTSTATUS  | HGM_InitAnalog |
 *
 *          Check that the configuration is valid whilst there is still time 
 *          to refuse it.  
 *          <nl>Detect and validate sibling relationships and call 
 *          HGM_JoystickConfig for the rest of the work.
 *
 *  @parm   IN PDEVICE_OBJECT | DeviceObject |
 *
 *          Pointer to the device object
 *
 *  @rvalue   STATUS_SUCCESS  | success
 *  @rvalue   STATUS_DEVICE_CONFIGURATION_ERROR  | Invalid configuration specified
 *
 *****************************************************************************/
/*
 *  Disable warning for variable used before set as it is hard for a compiler 
 *  to see that the use of DeviceExtension_Sibling is gated by a flag which 
 *  can only be set after DeviceExtension_Sibling is initialized.
 */
#pragma warning( disable:4701 )
NTSTATUS EXTERNAL
    HGM_InitAnalog
    (
    IN PDEVICE_OBJECT         DeviceObject
    )
{
    NTSTATUS            ntStatus;
    PDEVICE_EXTENSION   DeviceExtension;
    PDEVICE_EXTENSION   DeviceExtension_Sibling;
    PLIST_ENTRY         pEntry;

#define ARE_WE_RELATED(_x_, _y_)                                \
    (                                                           \
        (_x_)->GameContext     == (_y_)->GameContext      &&    \
        (_x_)->WriteAccessor   == (_y_)->WriteAccessor    &&    \
        (_x_)->ReadAccessor    == (_y_)->ReadAccessor           \
    )

    PAGED_CODE ();
    
    /*
     * Get a pointer to the device extension
     */
    DeviceExtension = GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);
    

    /*
     *  No modifications to the Global List while we are looking at it
     */
    ExAcquireFastMutex (&Global.Mutex);

    /*
     *  For two joysticks interface two fdos are created to service them 
     *  but physically they both share the same port.
     *  For the second sibling certain extra rules must be applied so we 
     *  search our list of devices for another device using the same port 
     *  and if we find one mark this one as a sibling.
     */
    for(pEntry = Global.DeviceListHead.Flink;
       pEntry != &Global.DeviceListHead;
       pEntry = pEntry->Flink)
    {

        /*
         * Obtain the device Extension of the Sibling
         */
        DeviceExtension_Sibling = CONTAINING_RECORD(pEntry, DEVICE_EXTENSION, Link);

        if(       DeviceExtension_Sibling != DeviceExtension
               && TRUE == ARE_WE_RELATED(DeviceExtension, DeviceExtension_Sibling)
               && TRUE == DeviceExtension_Sibling->fStarted )
        {

                DeviceExtension->fSiblingFound = TRUE;
           break;
        }
    }

    /*
     *  We are done, release the Mutex
     */
    ExReleaseFastMutex (&Global.Mutex);

    /*
     * check the axis and button configuration for the joystick
     */
    ntStatus = HGM_JoystickConfig(DeviceObject);

    if( NT_SUCCESS( ntStatus ) )
    {
        /*
         *  Make sure that sibling axes are not overlapped
         */
        if(  DeviceExtension->fSiblingFound &&
             (DeviceExtension_Sibling->resistiveInputMask & DeviceExtension->resistiveInputMask) != 0x0 )
        {

            ntStatus = STATUS_DEVICE_CONFIGURATION_ERROR;

        }
    }

    return( ntStatus );

} /* HGM_InitAnalog */



/*****************************************************************************
 *
 *  @doc    EXTERNAL
 *
 *  @func   VOID  | HGM_Game2HID |
 *
 *          Process the data returned from polling the gameport into values 
 *          and buttons for returning to HID.
 *          <nl>The meaning of the data is interpreted according to the 
 *          characteristics of the device described in the hardware settings
 *          flags.
 *
 *  @parm   IN      PDEVICE_EXTENSION | DeviceExtension | 
 *
 *          Pointer to the mini-driver device extension.
 *
 *  @parm   IN  OUT PUHIDGAME_INPUT_DATA | pHIDData | 
 *
 *          Pointer to the buffer into which the HID report should be written.
 *          This buffer must be assumed to be unaligned.
 *
 *****************************************************************************/
VOID 
    HGM_Game2HID
    (
    IN      PDEVICE_OBJECT       DeviceObject,
    IN  OUT PUHIDGAME_INPUT_DATA    pHIDData
    )
{
    LONG    Idx;
	PDEVICE_EXTENSION DeviceExtension =GET_MINIDRIVER_DEVICE_EXTENSION(DeviceObject);

    /*
     *  Use a local buffer to assemble the report as the real buffer may not 
     *  be aligned.
     */
    HIDGAME_INPUT_DATA  LocalBuffer;

    RtlZeroMemory( &LocalBuffer, sizeof( LocalBuffer ) );

	/*
     * Remap axis
     */
	if(!DeviceExtension->calibrado)
	{
	    for(Idx = 0x0; Idx < DeviceExtension->nAxes; Idx++ )
		{
			LocalBuffer.Axis[Idx] = (LONG)DeviceExtension->LastGoodAxis[DeviceExtension->AxisMap[Idx]];
	    }
	} else {
		LONG ancho1,ancho2;
		LONG ej[4];
		for(Idx=0; Idx<DeviceExtension->nAxes; Idx++)
		{
            LocalBuffer.Axis[Idx] = (LONG)DeviceExtension->LastGoodAxis[DeviceExtension->AxisMap[Idx]];

			// Antivibraciones

			if (LocalBuffer.Axis[Idx]>DeviceExtension->LastOldAxis[Idx])
			{
                if (LocalBuffer.Axis[Idx]>(DeviceExtension->LastOldAxis[Idx]+DeviceExtension->Escala_der[Idx])) {
					DeviceExtension->LastOldAxis[Idx]=LocalBuffer.Axis[Idx];
				} else {
					LocalBuffer.Axis[Idx]=DeviceExtension->LastOldAxis[Idx];
				}
			} else {
                if (LocalBuffer.Axis[Idx]<(DeviceExtension->LastOldAxis[Idx]-DeviceExtension->Escala_izq[Idx])) {
					DeviceExtension->LastOldAxis[Idx]=LocalBuffer.Axis[Idx];
				} else {
					LocalBuffer.Axis[Idx]=DeviceExtension->LastOldAxis[Idx];
				}
			}

			//-------------------

			if((LocalBuffer.Axis[Idx]>=(DeviceExtension->Limite[Idx].c-DeviceExtension->ZonaNula[Idx])) && (LocalBuffer.Axis[Idx]<=(DeviceExtension->Limite[Idx].c+DeviceExtension->ZonaNula[Idx])))
			{
				LocalBuffer.Axis[Idx]=0;
			} else {
                if(LocalBuffer.Axis[Idx]<DeviceExtension->Limite[Idx].i)
				{
					LocalBuffer.Axis[Idx]=DeviceExtension->Limite[Idx].i;
				} else {
					if(LocalBuffer.Axis[Idx]>DeviceExtension->Limite[Idx].d)
					{
						LocalBuffer.Axis[Idx]=DeviceExtension->Limite[Idx].d;
					}
				}
                ancho1=(DeviceExtension->Limite[Idx].c-DeviceExtension->ZonaNula[Idx])-DeviceExtension->Limite[Idx].i;
                ancho2=DeviceExtension->Limite[Idx].d-(DeviceExtension->Limite[Idx].c+DeviceExtension->ZonaNula[Idx]);
				if(LocalBuffer.Axis[Idx]<DeviceExtension->Limite[Idx].c)
				{
					LocalBuffer.Axis[Idx]=0-((DeviceExtension->Limite[Idx].c-DeviceExtension->ZonaNula[Idx])-LocalBuffer.Axis[Idx]);
					if(ancho1>ancho2)
					{
						LocalBuffer.Axis[Idx]=(LocalBuffer.Axis[Idx]*ancho2)/ancho1;
					}
				} else {
					LocalBuffer.Axis[Idx]-=(DeviceExtension->Limite[Idx].c+DeviceExtension->ZonaNula[Idx]);
					if(ancho2>ancho1)
					{
						LocalBuffer.Axis[Idx]=(LocalBuffer.Axis[Idx]*ancho1)/ancho2;
					}
				}
			}
			ej[Idx]=LocalBuffer.Axis[Idx];
		}
		LocalBuffer.Axis[0]=ej[DeviceExtension->AxisMapV[0]];
		LocalBuffer.Axis[1]=ej[DeviceExtension->AxisMapV[1]];
		LocalBuffer.Axis[2]=ej[DeviceExtension->AxisMapV[2]];
	    LocalBuffer.Axis[3]=ej[DeviceExtension->AxisMapV[3]];
	}

    if((DeviceExtension->xfo!=NULL) && (KeGetCurrentIrql()==PASSIVE_LEVEL))
	{
	    NTSTATUS                ntStatus = STATUS_SUCCESS;
	    KEVENT                  IoctlCompleteEvent;
	    IO_STATUS_BLOCK         IoStatus;
	    PIRP                    pIrp;
		UCHAR buf[12]={0,0,0,0,0,0,0,0,0,0,0,0};

		KeInitializeEvent(&IoctlCompleteEvent, NotificationEvent, FALSE);
		pIrp=IoBuildDeviceIoControlRequest(IOCTL_HID_LEER,DeviceExtension->xdo,NULL,0,buf,12,TRUE,&IoctlCompleteEvent,&IoStatus);
		if(pIrp!=NULL)
		{
			ntStatus = IoCallDriver(DeviceExtension->xdo, pIrp);
			if (ntStatus == STATUS_PENDING) {
                ntStatus = KeWaitForSingleObject (&IoctlCompleteEvent,Executive,KernelMode,FALSE,NULL);
			}
			if (ntStatus == STATUS_SUCCESS )
			{
				RtlCopyMemory(LocalBuffer.hatswitch,buf,2);
				RtlCopyMemory(LocalBuffer.Button,&buf[2],5);
				if(buf[7]==1) {
					DeviceExtension->calibrado=FALSE;
				} else {
					if(buf[7]==2) {
						RtlCopyMemory(DeviceExtension->AxisMapV,&buf[8],4);
						HGM_DeviceRead(DeviceExtension);
					}
				}
			}
		}
	}

    RtlCopyMemory( pHIDData, &LocalBuffer, sizeof( LocalBuffer ) );

} /* HGM_Game2HID */




VOID
    HGM_ChangeHandler
    ( 
    IN OUT PDEVICE_EXTENSION   DeviceExtension
    )
{
    PAGED_CODE ();

	LeerRegistro(DeviceExtension);

	HGM_DecRequestCount( DeviceExtension );
}

VOID
    HGM_DeviceRead
    ( 
    IN  OUT PDEVICE_EXTENSION   DeviceExtension
    )
{
    NTSTATUS    ntStatus;

    /*
     *  Since the work item will use the device extension, bump the usage 
     *  count up one in case anyone tries to remove the device between 
     *  now and when the work item gets to run.  If that fails, forget it.
     */
    ntStatus = HGM_IncRequestCount( DeviceExtension );

    if( NT_SUCCESS(ntStatus) )
    {
        ExInitializeWorkItem( &DeviceExtension->WorkItem, 
            (PWORKER_THREAD_ROUTINE)HGM_ChangeHandler, DeviceExtension );

        ExQueueWorkItem( &DeviceExtension->WorkItem, DelayedWorkQueue );
    }
}

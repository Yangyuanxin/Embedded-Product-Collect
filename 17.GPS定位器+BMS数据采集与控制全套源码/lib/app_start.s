;/***************************************************************************************
; ** DATE:           05/22/2019                                                			*
; ** Copyright: Copyright @ Goome Technologies Co., Ltd. 2009-2019. All rights reserved.*
; ** Description:    ServiceHeader define                                                          			*
; ***************************************************************************************


    PRESERVE8 
    AREA ServiceHeader, CODE, READONLY

    IMPORT    Service_Entry        
    IMPORT    |Image$$ER_RO$$Length|
    IMPORT    |Image$$ER_RW$$Length|
    IMPORT    |Image$$ER_ZI$$ZI$$Length|

    EXPORT service_feature




;******************************************************************************    
service_version
    DCD       0x00000002
service_feature
    DCD       0x00000001

service_entry_offset
    DCD       Service_Entry - {PC}
    
ro_size
    DCD       |Image$$ER_RO$$Length|    
rw_size    
    DCD       |Image$$ER_RW$$Length|
zi_size    
    DCD       |Image$$ER_ZI$$ZI$$Length|



;******************************************************************************    
;The Kernel Export function pointer table

        MACRO
        Subfun  $name
        LCLS    appfun
        LCLS    libfun
appfun SETS    "$name"
libfun SETS    "|$$Super$$$$" :CC: appfun :CC: "|"
appfun SETS    "|$$Sub$$$$" :CC: appfun :CC: "|"
        EXPORT  $appfun
        IMPORT  $libfun

            CODE32
$appfun     LDR     pc, =$libfun
            LTORG            

        MEND
        
;       /*Add other export api according to the format of above*/ 

	Subfun GM_Trace
	Subfun GM_ReleaseBuild
	Subfun GM_ReleaseBranch
	Subfun GM_ReleaseHwVer
	Subfun GM_ReleaseVerno
	Subfun GM_BuildDateTime
	Subfun GM_atoi
	Subfun GM_atof
	Subfun GM_memset
	Subfun GM_memcpy
	Subfun GM_memcmp
	Subfun GM_memmove
	Subfun GM_strcpy
	Subfun GM_strncpy
	Subfun GM_strcat
	Subfun GM_strncat
	Subfun GM_strcmp
	Subfun GM_strncmp
	Subfun GM_strchr
	Subfun GM_strlen
	Subfun GM_strstr
	Subfun GM_sprintf
	Subfun GM_snprintf
	Subfun GM_vsprintf
	Subfun GM_sscanf
	Subfun GM_GpioInit
	Subfun GM_GetGpioMode
	Subfun GM_GpioSetLevel
	Subfun GM_GpioGetLevel
	Subfun GM_GpioSetDirection
	Subfun GM_GpioGetDirection
	Subfun GM_GpioSetPullSelection
	Subfun GM_RegisterCallBack
	Subfun GM_UartOpen
	Subfun GM_UartClose
	Subfun GM_UartWrite
	Subfun GM_UartRead
	Subfun GM_UartClrRxBuffer
	Subfun GM_UartClrTxBuffer
	Subfun GM_CheckSimValid
	Subfun GM_GetImei
	Subfun GM_GetPlmn
	Subfun GM_GetServiceAvailability
	Subfun GM_SetLocalTime
	Subfun GM_GetLocalTime
	Subfun GM_SetUartTrace
	Subfun GM_CloseUartTrace
	Subfun GM_MemoryAlloc
	Subfun GM_MemoryFree
	Subfun GM_Ucs2ToAsc
	Subfun GM_AscToUcs2
	Subfun GM_SendSMS
	Subfun GM_CallAnswer
	Subfun GM_CallHangUp
	Subfun GM_MakeCall
	Subfun GM_SystemReboot
	Subfun GM_SystemPowerOff
	Subfun GM_FS_Open
	Subfun GM_FS_Close
	Subfun GM_FS_Read
	Subfun GM_FS_Write
	Subfun GM_FS_Seek
	Subfun GM_FS_Commit
	Subfun GM_FS_GetFileSize
	Subfun GM_FS_GetFilePosition
	Subfun GM_GetDiskFreeSpace
	Subfun GM_FS_Delete  
	Subfun GM_FS_CheckFile
	Subfun GM_FS_GetCurrentDir
	Subfun GM_FS_SetCurrentDir
	Subfun GM_FS_CreateDir
	Subfun GM_FS_RemoveDir
	Subfun GM_FS_Rename
	Subfun GM_FS_XDelete
	Subfun GM_IsinkBacklightCtrl
	Subfun GM_SocketCreate
	Subfun GM_SocketConnect
	Subfun GM_SocketRecv
	Subfun GM_SocketClose
	Subfun GM_SocketSend
	Subfun GM_SocketRegisterCallBack
	Subfun GM_GetLocalIP
	Subfun GM_SleepEnable
	Subfun GM_SleepDisable
	Subfun GM_KpledLevelCtrl
	Subfun GM_CreateKalTimer
	Subfun GM_StartKalTimer
	Subfun GM_StopKalTimer
	Subfun GM_StartTimer
	Subfun GM_StopTimer
	Subfun GM_GetSignalValue
	Subfun GM_GetLbsInfo
	Subfun GM_I2cInit
	Subfun GM_I2cConfig
	Subfun GM_I2cWriteByte
	Subfun GM_I2cReadBytes
	Subfun GM_I2cGetMode
	Subfun GM_I2cSetMode
	Subfun GM_AdcRead
	Subfun GM_EintRegister
	Subfun GM_EintPinInit
	Subfun GM_EintSetPolarity
	Subfun GM_SysUsdelay
	Subfun GM_SysMsdelay
	Subfun GM_GetImsi
	Subfun GM_GetIccid
	Subfun GM_GetPdpStatus
	Subfun GM_GetGsmStatus
	Subfun GM_ImageZiLimit
	Subfun GM_ImageDummyBase
	Subfun GM_GetBarcode
	Subfun GM_IsGprsAutoOn
	Subfun GM_GetTcpStatus
	Subfun GM_GetLibStatus
	Subfun GM_SetUartFunc
	Subfun GM_GetHostByName
	Subfun GM_GetCregStatus
	Subfun GM_GetBearerStatus
	Subfun GM_AccountIdClose
	Subfun GM_GetFakeCellFeature
	Subfun GM_ApnConfig
	Subfun GM_ReadWriteNvram
	Subfun GM_GetNewAttachStatus
	Subfun GM_IsWdtPwrOn
	Subfun GM_DnsParserCallback
	Subfun GM_GetSetFlightMode
	Subfun GM_CloseBearerAppId
	Subfun GM_GetApnMatch
	Subfun GM_DeleteApn
	Subfun GM_SoftwareWdtCounterEnable
    Subfun GM_SetSoftwareWdtCounter
    Subfun GM_GetSoftwareWdtCounter
    Subfun GM_SetSwLogicCounterEnable
    Subfun GM_SetSwLogicCounter
    Subfun GM_SetSwLogicCounterMax
    Subfun GM_GetSwLogicCounter
    Subfun GM_GetSwLogicCounterMax
	Subfun GM_SpiReadData
	Subfun GM_SpiWriteData
	Subfun GM_SpiFlashReadID
	Subfun GM_SpiPinInit
	Subfun GM_StopRecord
	Subfun GM_StartRecord
	Subfun GM_RecordRegister
	Subfun GM_GetChrStatus
    Subfun GM_PowerKeyRegister
    Subfun GM_ConvertIpAddr
    Subfun GM_ReadNvramLid
    Subfun GM_ChrCurrentConfig
    Subfun GM_ChrCurrentGet
    Subfun GM_AutoApnCheck
    Subfun GM_Set_Mic_Volume
    Subfun GM_log10
    Subfun GM_log
    Subfun GM_sqrt
    Subfun GM_fabs
    Subfun wlan_init
    Subfun wlan_deinit
    Subfun wlan_scan
	Subfun GM_MemoryRealloc
	Subfun GM_GetTime
	Subfun GM_GetChipRID
	Subfun GM_Func19
	Subfun GM_Func20
	Subfun GM_Func21
	Subfun GM_Func22
	Subfun GM_Func23
	Subfun GM_Func24
	Subfun GM_Func25
	Subfun GM_Func26
	Subfun GM_Func27
	Subfun GM_Func28
	Subfun GM_Func29
	Subfun GM_Func30
	Subfun GM_Func31
	Subfun GM_Func32
	Subfun GM_Func33
	Subfun GM_Func34
	Subfun GM_Func35
	Subfun GM_Func36
	Subfun GM_Func37
	Subfun GM_Func38
	Subfun GM_Func39
	Subfun GM_Func40
	Subfun GM_Func41
	Subfun GM_Func42
	Subfun GM_Func43
	Subfun GM_Func44
	Subfun GM_Func45
	Subfun GM_Func46
	Subfun GM_Func47
	Subfun GM_Func48
	Subfun GM_Func49
	Subfun GM_Func50
	Subfun GM_CallbackFunc1
	Subfun GM_CallbackFunc2
	Subfun GM_CallbackFunc3
	Subfun GM_CallbackFunc4
	Subfun GM_CallbackFunc5
	Subfun GM_CallbackFunc6
	Subfun GM_CallbackFunc7
	Subfun GM_CallbackFunc8
	Subfun GM_CallbackFunc9
	Subfun GM_CallbackFunc10
	Subfun GM_CallbackFunc11
	Subfun GM_CallbackFunc12
	Subfun GM_CallbackFunc13
	Subfun GM_CallbackFunc14
	Subfun GM_CallbackFunc15
	Subfun GM_CallbackFunc16
	Subfun GM_CallbackFunc17
	Subfun GM_CallbackFunc18
	Subfun GM_CallbackFunc19
	Subfun GM_CallbackFunc20
	Subfun GM_CallbackFunc21
	Subfun GM_CallbackFunc22
	Subfun GM_CallbackFunc23
	Subfun GM_CallbackFunc24
	Subfun GM_CallbackFunc25
	Subfun GM_CallbackFunc26
	Subfun GM_CallbackFunc27
	Subfun GM_CallbackFunc28
	Subfun GM_CallbackFunc29
	Subfun GM_CallbackFunc30
	Subfun GM_CallbackFunc31
	Subfun GM_CallbackFunc32
	Subfun GM_CallbackFunc33
	Subfun GM_CallbackFunc34
	Subfun GM_CallbackFunc35
	Subfun GM_CallbackFunc36
	Subfun GM_CallbackFunc37
	Subfun GM_CallbackFunc38
	Subfun GM_CallbackFunc39
	Subfun GM_CallbackFunc40
	Subfun GM_CallbackFunc41
	Subfun GM_CallbackFunc42
	Subfun GM_CallbackFunc43
	Subfun GM_CallbackFunc44
	Subfun GM_CallbackFunc45
	Subfun GM_CallbackFunc46
	Subfun GM_CallbackFunc47
	Subfun GM_CallbackFunc48
	Subfun GM_CallbackFunc49
	Subfun GM_CallbackFunc50
	
;---------------------------------------------------------
;	adding new api under this line
;	for ex: Subfun xxxxxxx

;---------------------------------------------------------

	EXPORT TK_dll_set_sb


TK_dll_set_sb
	STMDB SP!, {R1, LR}
	MOV R1, SB
	MOV SB, R0
	MOV R0, R1
	LDMIA SP!, {R1, PC}
        END

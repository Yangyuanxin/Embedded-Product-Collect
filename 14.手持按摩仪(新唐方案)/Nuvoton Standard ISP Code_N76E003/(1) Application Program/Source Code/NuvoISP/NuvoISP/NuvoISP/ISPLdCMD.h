#ifndef INC__ISP_LD_H__
#define INC__ISP_LD_H__
#pragma once

#include "CScopedMutex.hpp"
#include "Interface\CHidIO.h"
#include "Interface\CUartIO.h"
#define HID_MAX_PACKET_SIZE_EP 64
class CUartIO;

class ISPLdCMD
{
protected:
    CHAR	m_acBuffer[HID_MAX_PACKET_SIZE_EP + 1];
    unsigned long m_uCmdIndex;
    USHORT	m_usCheckSum;

	// Interface
	ULONG			m_uInterface;
	CString			m_strComNum;
	CHidIO			m_hidIO;
	CUartIO			m_comIO;
    BOOL			m_bOpenPort;
    CMutex2			m_Mutex;


	BOOL ReadFile(char *pcBuffer, size_t szMaxLen, DWORD dwMilliseconds, BOOL bCheckIndex = TRUE);
	BOOL WriteFile(unsigned long uCmd, const char *pcBuffer = NULL, DWORD dwLen = 0, DWORD dwMilliseconds = 20/*USBCMD_TIMEOUT*/);


public:

	BOOL bResendFlag;	// This flag is set by ReadFile
    ISPLdCMD();
    virtual ~ISPLdCMD();
	
    bool Check_USB_Link();
    bool Open_Port(BOOL bErrorMsg = FALSE);
    void Close_Port();

    unsigned short Checksum(const unsigned char *buf, int len) {
        int i;
        unsigned short c;

        for (c=0, i=0; i < len; i++) {
            c += buf[i];
        }
        return (c);
    }

    enum {
        CMD_GET_VERSION     = 0x000000A6,
        CMD_UPDATE_APROM	= 0x000000A0,
        CMD_SYNC_PACKNO		= 0x000000A4,
        CMD_UPDATE_CONFIG   = 0x000000A1,
        CMD_UPDATE_WOERASE	= 0x000000C2,
        CMD_READ_CHECKSUM 	= 0x000000C8,
        CMD_WRITE_CHECKSUM	= 0x000000C9,
        CMD_ERASE_ALL 	    = 0x000000A3,
        CMD_READ_CONFIG     = 0x000000A2,
        CMD_APROM_SIZE      = 0x000000AA,
        CMD_GET_DEVICEID    = 0x000000B1,
        CMD_GET_FLASH_DID	= 0x000000B2,
        CMD_RUN_APROM		= 0x000000AB,
        CMD_RUN_LDROM		= 0x000000AC,
        CMD_RESET			= 0x000000AD,
		CMD_CONNECT			= 0x000000AE,
		CMD_UPDATE_DATAFLASH = 0x000000C3,
		CMD_RESEND_PACKET   = 0x000000FF,
    };

	BOOL CMD_Connect(DWORD dwMilliseconds = 30);
	BOOL CMD_Resend();


    void SyncPackno();
    unsigned char GetVersion();
    unsigned long GetDeviceID();
    void ReadConfig(unsigned int config[2]);
    void UpdateConfig(unsigned int config[2], unsigned int response[2]);
    void UpdateAPROM(unsigned long start_addr,
                     unsigned long total_len,
                     unsigned long cur_addr,
                     const char *buffer,
                     unsigned long *update_len);
    void UpdateNVM(unsigned long start_addr,
                     unsigned long total_len,
                     unsigned long cur_addr,
                     const char *buffer,
                     unsigned long *update_len);

	BOOL EraseAll();
					 
	void Test();
	void SetInterface(unsigned int it, CString str) {m_uInterface = it; m_strComNum = str;};
};

#endif

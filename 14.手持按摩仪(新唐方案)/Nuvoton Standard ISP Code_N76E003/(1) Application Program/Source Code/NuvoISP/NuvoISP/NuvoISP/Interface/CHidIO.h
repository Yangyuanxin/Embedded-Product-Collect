#ifndef INC__HID_IO_HPP__
#define INC__HID_IO_HPP__

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <vector>
//#include "dbt.h"
#include "OneInstance.h"

extern "C" {
#include "setupapi.h"
#include "hidsdi.h"
}


#define HID_MAX_PACKET_SIZE_EP 64

struct CHidShare {
    unsigned char m_ucCmdIndex;
    unsigned char m_udDummp[3];
};

class CHidIO
{
protected:
    BOOL m_bUseTwoHandle;
    HANDLE m_hReadEvent;
    HANDLE m_hWriteEvent;
    HANDLE m_hAbordEvent;
    size_t m_szActiveDeviceIndex;
    std::vector<HANDLE> m_hReadHandle;
    std::vector<HANDLE> m_hWriteHandle;
    std::vector<std::basic_string<TCHAR> > m_sNames;
    ShareArea<CHidShare> m_DeviceData;		/* Mutext for this object */
public:
    HANDLE GetActiveHandle() const;
    CHidIO();
    virtual ~CHidIO();
    size_t GetDeviceLength() const;
    size_t SetActiveDevice(size_t szIndex);
    size_t GetActiveDevice() const;
    ShareArea<CHidShare> &GetActiveDeviceData();
    void CloseDevice();
    void CloseInactiveDevice();
    BOOL OpenDevice(BOOL bUseTwoHandle, USHORT usVID, USHORT usPID0, USHORT usPID1 = NULL, USHORT usPID2 = NULL);
    BOOL ReadFile(char *pcBuffer, size_t szMaxLen, DWORD *pdwLength, DWORD dwMilliseconds);
    BOOL WriteFile(const char *pcBuffer, size_t szLen, DWORD *pdwLength, DWORD dwMilliseconds);
};



#endif


#include "stdafx.h"
#include "CUartIO.h"


CUartIO::CUartIO()
    : m_hCOMHandle(INVALID_HANDLE_VALUE)
{
}

CUartIO::~CUartIO()
{
    CloseDevice();
}

void CUartIO::CloseDevice()
{
    if(m_hCOMHandle != INVALID_HANDLE_VALUE) {
        //PurgeComm( m_hCOMHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR
        //           | PURGE_RXCLEAR );
        CloseHandle(m_hCOMHandle);
        m_hCOMHandle = INVALID_HANDLE_VALUE;
    }
}

BOOL CUartIO::OpenDevice(CString strComNum)
{
	//printf("OpenDevice COM - ");
    //CommTimeOuts.ReadIntervalTimeout = 200;
    strComNum=_T("\\\\.\\")+strComNum;

    if(m_hCOMHandle!=INVALID_HANDLE_VALUE) {
        CloseHandle(m_hCOMHandle);
        m_hCOMHandle=INVALID_HANDLE_VALUE;
    }

    m_hCOMHandle =CreateFile(strComNum, GENERIC_READ | GENERIC_WRITE, // 允許讀寫
                             0,						// 此項必須為0
                             NULL,					// no security attrs
                             OPEN_EXISTING,         //設置產生方式
                             FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,  // 我們準備使用非同步通信
                             NULL);
    if(m_hCOMHandle == INVALID_HANDLE_VALUE) {
        //LastErrorNum = GetLastError();
        //CString tmp;
        //tmp.Format(_T("%d"),LastErrorNum);
        //AddToInfOut(_T("Failed to open ")+strComNum+_T(";Error code:")+tmp,1,1);
        //AddToInfOut(ConvertErrorCodeToString(LastErrorNum),0,1);
        return FALSE;
    } else {
			
		//printf("CreateFile OK - ");


		COMMTIMEOUTS CommTimeOuts ; //定義超時結構，並填寫該結構
		memset(&CommTimeOuts, 0, sizeof(CommTimeOuts));
		CommTimeOuts.ReadTotalTimeoutConstant = 8000;//ms
		//printf("SetCommTimeouts - ");
        SetCommTimeouts( m_hCOMHandle, &CommTimeOuts ) ;	//設置讀寫操作所允許的超時
		DCB dcb;                    //定義資料控制塊結構

		//memset(&dcb, 0, sizeof(dcb));
   //  Initialize the DCB structure.
   SecureZeroMemory(&dcb, sizeof(DCB));
   dcb.DCBlength = sizeof(DCB);

		//printf("GetCommState - ");
        GetCommState(m_hCOMHandle, &dcb ) ;       //讀串口原來的參數設置
        dcb.BaudRate =115200;                     //Baudrate;
        dcb.ByteSize =8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT ;
        dcb.fBinary = TRUE ;
        dcb.fParity = FALSE;
        
		
		//dcb.fDtrControl = 0;



		//printf("SetCommState - ");
        SetCommState(m_hCOMHandle, &dcb );					//串口參數配置
		//printf("SetCommMask - ");
        SetCommMask(m_hCOMHandle, EV_RXCHAR|EV_TXEMPTY );	//設置事件驅動的類型
		//printf("SetupComm - ");
        SetupComm( m_hCOMHandle, 1024,128) ;				//設置輸入,輸出緩衝區的大小
		//printf("PurgeComm - ");

        PurgeComm( m_hCOMHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR
                   | PURGE_RXCLEAR );               //清乾淨輸入、輸出緩衝區

    }

	//printf("Return True\n");

    return TRUE;
}

BOOL CUartIO::ReadFile(char *pcBuffer, DWORD szMaxLen, DWORD *pdwLength, DWORD dwMilliseconds)
{
	//DWORD dwStart = GetTickCount();

	BOOL success = FALSE;	// success status is used for Debug purpose
	OVERLAPPED o = {0};
	o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    //if(!::ReadFile(m_hCOMHandle, pcBuffer, szMaxLen, NULL, &m_overlapped)) {
	if (!::ReadFile(m_hCOMHandle, pcBuffer, szMaxLen, pdwLength, &o))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			ResetEvent(o.hEvent);
			if (WaitForSingleObject(o.hEvent, dwMilliseconds) == WAIT_OBJECT_0)
				success = 2;
		}
		success += GetOverlappedResult(m_hCOMHandle, &o, pdwLength, FALSE);
	}
	else
		success = 1;
	
	CloseHandle(o.hEvent);
	
	// if(!success)
		// printf("GetLastError() = %D\n", GetLastError());
	//printf("Time %d, success %d\n",	(GetTickCount() - dwStart), success);
	return success;
}

BOOL CUartIO::WriteFile(const char *pcBuffer, DWORD szLen, DWORD *pdwLength, DWORD dwMilliseconds)
{
    PurgeComm( m_hCOMHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR
                   | PURGE_RXCLEAR );     
	DWORD dwLength = 0;

    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if(pdwLength != NULL)
        *pdwLength = 0;

    if(::WriteFile(m_hCOMHandle, pcBuffer, szLen, NULL, &overlapped) == FALSE) {
        if(GetLastError() == ERROR_IO_PENDING)
            GetOverlappedResult(m_hCOMHandle, &overlapped, &dwLength, TRUE);
    }

    CloseHandle(overlapped.hEvent);
    return TRUE;
}

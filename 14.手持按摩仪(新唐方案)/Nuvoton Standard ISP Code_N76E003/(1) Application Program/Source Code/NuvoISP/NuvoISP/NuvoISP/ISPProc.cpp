#include "stdafx.h"
#include "ISPProc.h"
#include "thread_proxy.h"	// thread_proxy
#include "PartNumID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

template <class T>
inline T *vector_ptr(std::vector<T> &v)
{
    if(v.size() > 0)
        return &v[0];
    else
        return NULL;
}

template <class T>
inline const T *vector_ptr(const std::vector<T> &v)
{
    if(v.size() > 0)
        return &v[0];
    else
        return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CDialog_NUC4xx dialog

CISPProc::CISPProc(HWND *pWnd)
{
    MainHWND = pWnd;
    m_hThreadMutex = ::CreateMutex(NULL, FALSE, NULL);
    m_fnThreadProcStatus = &CISPProc::Thread_Pause;
    m_fnThreadProcStatus_backup = &CISPProc::Thread_Pause;

    DWORD dwThreadID;
    m_pAssistThread = thread_proxy(
                          this,
                          NULL,
                          //LPSECURITY_ATTRIBUTES lpThreadAttributes,  // pointer to security attributes
                          0,
                          //DWORD dwStackSize,                         // initial thread stack size
                          &CISPProc::AssistThread,
                          //DWORD (T::*lpStartAddress)(LPVOID),        // pointer to thread function
                          NULL,
                          //LPVOID lpParameter,                        // argument for new thread
                          0,
                          //DWORD dwCreationFlags,                     // creation flags
                          &dwThreadID
                          //LPDWORD lpThreadId                         // pointer to receive thread ID
                      );

	m_CONFIG[0] = 0xFFFFFFFF;
	m_CONFIG[1] = 0xFFFFFFFF;
	m_CONFIG_User[0] = 0xFFFFFFFF;
	m_CONFIG_User[1] = 0xFFFFFFFF;

	m_eProcSts = EPS_OK;
}

CISPProc::~CISPProc()
{
    Set_ThreadAction(NULL);
}

DWORD CISPProc::AssistThread(LPVOID pArg)
{
    while(1) {
        void (CISPProc::*fnThreadProcStatus)() = m_fnThreadProcStatus;

        {
            if(fnThreadProcStatus != NULL) {
                (this->*fnThreadProcStatus)();
            } else {
                break;
            }
        }
    }
    return 0;
}

void CISPProc::Set_ThreadAction(void (CISPProc::*fnThreadProcStatus)())
{
    LockGUI();
    m_fnThreadProcStatus_backup = m_fnThreadProcStatus;
    if(m_fnThreadProcStatus != NULL) {
        m_fnThreadProcStatus = fnThreadProcStatus;
    }
    UnlockGUI();
}

void CISPProc::Thread_Pause()
{
    while(m_fnThreadProcStatus == &CISPProc::Thread_Pause)
        Sleep(100);
}

void CISPProc::Thread_Idle()
{
    if(m_fnThreadProcStatus != &CISPProc::Thread_Idle)
        return;

    PostMessage(*MainHWND, MSG_USER_EVENT, MSG_UPDATE_CONNECT_STATUS, CONNECT_STATUS_NONE);

    /* Delete previous ICE object */
    m_ISPLdDev.Close_Port();
	m_eProcSts = EPS_OK;

    while(m_fnThreadProcStatus == &CISPProc::Thread_Idle)
        Sleep(100);
}


void CISPProc::Thread_CheckUSBConnect()
{
    if(m_fnThreadProcStatus != &CISPProc::Thread_CheckUSBConnect)
        return;

    PostMessage(*MainHWND, MSG_USER_EVENT, MSG_UPDATE_CONNECT_STATUS, CONNECT_STATUS_USB);

    /* Delete previous ICE object */
    m_ISPLdDev.Close_Port();

    DWORD dwWait = 0;
    while(m_fnThreadProcStatus == &CISPProc::Thread_CheckUSBConnect) {
        if(m_ISPLdDev.Open_Port(false)) {
			m_eProcSts = EPS_ERR_CONNECT;
            if(m_ISPLdDev.CMD_Connect(40)) {
                Set_ThreadAction(&CISPProc::Thread_CheckDeviceConnect);
            }
        } else {
			m_eProcSts = EPS_ERR_OPENPORT;
            Sleep(1000);
        }
    }
}


void CISPProc::Thread_CheckDeviceConnect()
{
    if(m_fnThreadProcStatus != &CISPProc::Thread_CheckDeviceConnect)
        return;

    PostMessage(*MainHWND, MSG_USER_EVENT, MSG_UPDATE_CONNECT_STATUS, CONNECT_STATUS_CONNECTING);

    while(m_fnThreadProcStatus == &CISPProc::Thread_CheckDeviceConnect) {
        if(m_ISPLdDev.Check_USB_Link()) {

			// Re-Open COM Port to clear previous status
			m_ISPLdDev.Close_Port();
			m_ISPLdDev.Open_Port();

            m_ISPLdDev.SyncPackno();
            m_ucFW_VER = m_ISPLdDev.GetVersion();
            //printf("GetVersion: %X\n", m_ucFW_VER);

            m_ulDeviceID = m_ISPLdDev.GetDeviceID();
            //printf("GetDeviceID: %X\n", m_ulDeviceID);

            m_ISPLdDev.ReadConfig(m_CONFIG);
            //printf("ReadConfig: %X, %X\n", m_CONFIG[0], m_CONFIG[1]);
			memcpy(m_CONFIG_User, m_CONFIG, sizeof(m_CONFIG));
			m_eProcSts = EPS_OK;

            Set_ThreadAction(&CISPProc::Thread_CheckDisconnect);
        } else
            Set_ThreadAction(&CISPProc::Thread_CheckUSBConnect);
    }
}

void CISPProc::Thread_CheckDisconnect()
{
    if(m_fnThreadProcStatus != &CISPProc::Thread_CheckDisconnect)
        return;

    PostMessage(*MainHWND, MSG_USER_EVENT, MSG_UPDATE_CONNECT_STATUS, CONNECT_STATUS_OK);

    while(m_fnThreadProcStatus == &CISPProc::Thread_CheckDisconnect)
        Sleep(100);
}

void CISPProc::Thread_ProgramFlash()
{
    if(m_fnThreadProcStatus != &CISPProc::Thread_ProgramFlash)
        return;

    unsigned int uAddr, uSize;
    unsigned char *pBuffer;

	m_eProcSts = EPS_OK;

    try {

		if(m_bErase) {
			if(m_ISPLdDev.EraseAll())
				m_ISPLdDev.ReadConfig(m_CONFIG);
			else
			{
				m_eProcSts = EPS_ERR_ERASE;
				Set_ThreadAction(&CISPProc::Thread_CheckDisconnect);
				return;
			}
		}

		if(m_bProgram_Config) {
			m_ISPLdDev.UpdateConfig(m_CONFIG_User, m_CONFIG);
			if((m_CONFIG_User[0] != m_CONFIG[0]) || (m_CONFIG_User[1] != m_CONFIG[1]))
			{
				m_eProcSts = EPS_ERR_CONFIG;
				Set_ThreadAction(&CISPProc::Thread_CheckDisconnect);
				return;
			}
			
			UpdateSizeInfo(m_ulDeviceID, m_CONFIG[0], m_CONFIG[1],
					   &m_uNVM_Addr,
					   &m_uAPROM_Size, &m_uNVM_Size);
		}

        if(m_bProgram_APROM) {
            uAddr = 0;
            uSize = m_sFileInfo[0].st_size;
            pBuffer = vector_ptr(m_sFileInfo[0].vbuf);

			if(m_uAPROM_Size < uSize)
			{
				m_eProcSts = EPS_ERR_SIZE;
				Set_ThreadAction(&CISPProc::Thread_CheckDisconnect);
				return;
			}

            PostMessage(*MainHWND, MSG_USER_EVENT, MSG_UPDATE_WRITE_STATUS, 0);

			m_ISPLdDev.SyncPackno();
            for(unsigned long i = 0; i < uSize; ) {
                if(m_fnThreadProcStatus != &CISPProc::Thread_ProgramFlash)
                    break;

                unsigned long uLen;
                m_ISPLdDev.UpdateAPROM(uAddr, uSize, i,
                                       (const char*)(pBuffer + i),
                                       &uLen);

				if(m_ISPLdDev.bResendFlag)
				{
					if(i == 0 || !m_ISPLdDev.CMD_Resend())
					{
						m_eProcSts = EPS_ERR_APROM;
						Set_ThreadAction(&CISPProc::Thread_CheckDisconnect);
						return;
					}
				}

                i += uLen;

                PostMessage(*MainHWND, MSG_USER_EVENT, MSG_UPDATE_WRITE_STATUS, (i * 100) / uSize);
            }
        }

        if(m_bProgram_NVM) {
            uAddr = m_uNVM_Addr;
            uSize = m_sFileInfo[1].st_size;
            pBuffer = vector_ptr(m_sFileInfo[1].vbuf);

			if(m_uNVM_Size < uSize)
			{
				m_eProcSts = EPS_ERR_SIZE;
				Set_ThreadAction(&CISPProc::Thread_CheckDisconnect);
				return;
			}

            PostMessage(*MainHWND, MSG_USER_EVENT, MSG_UPDATE_WRITE_STATUS, 0);

			m_ISPLdDev.SyncPackno();
            for(unsigned long i = 0; i < uSize; ) {
                if(m_fnThreadProcStatus != &CISPProc::Thread_ProgramFlash)
                    break;

                unsigned long uLen;
                m_ISPLdDev.UpdateNVM(uAddr, uSize, uAddr + i,
                                     (const char*)(pBuffer + i),
                                     &uLen);

				if(m_ISPLdDev.bResendFlag)
				{
					if(i == 0 || !m_ISPLdDev.CMD_Resend())
					{
						m_eProcSts = EPS_ERR_NVM;
						Set_ThreadAction(&CISPProc::Thread_CheckDisconnect);
						return;
					}
				}

                i += uLen;

                PostMessage(*MainHWND, MSG_USER_EVENT, MSG_UPDATE_WRITE_STATUS, (i * 100) / uSize);
            }
        }


        if(m_fnThreadProcStatus == &CISPProc::Thread_ProgramFlash) {
            m_eProcSts = EPS_PROG_DONE;
            Set_ThreadAction(&CISPProc::Thread_CheckDisconnect);
        }
    } catch(const TCHAR *szMSG) {

        MessageBox(*MainHWND, szMSG, NULL, MB_ICONSTOP);
        Set_ThreadAction(&CISPProc::Thread_CheckUSBConnect);
    }
}

void CISPProc::LockGUI()
{
    ::WaitForSingleObject(m_hThreadMutex, INFINITE);
}

void CISPProc::UnlockGUI()
{
    ::ReleaseMutex(m_hThreadMutex);
}

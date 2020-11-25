// ICPToolDlg.h : header file
//

#if !defined(_ISP_PROC_H_)
#define _ISP_PROC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISPLdCMD.h"

#include "fileinfo.h"
#define NUM_VIEW 2

#define MSG_USER_EVENT				(WM_APP+1)
#define MSG_UPDATE_ERASE_STATUS		3
#define MSG_UPDATE_WRITE_STATUS		4
#define MSG_UPDATE_CONNECT_STATUS	7
#define CONNECT_STATUS_NONE			0
#define CONNECT_STATUS_USB			1
#define CONNECT_STATUS_CONNECTING	2
#define CONNECT_STATUS_OK			3

/////////////////////////////////////////////////////////////////////////////
// CISPProc dialog
enum EProcSts {
	EPS_OK = 0,
	// Thread_CheckUSBConnect
	EPS_ERR_OPENPORT = 1,
	EPS_ERR_CONNECT = 2,
	// Thread_ProgramFlash
	EPS_ERR_ERASE = 8,
	EPS_ERR_CONFIG = 3,
	EPS_ERR_APROM = 4,
	EPS_ERR_NVM = 5,
	EPS_ERR_SIZE = 6,
	EPS_PROG_DONE = 7,

};	// m_eProcSts

class CISPProc
{
// Construction
public:
    CISPProc(HWND *pWnd);	// standard constructor
    virtual ~CISPProc();
    fileinfo m_sFileInfo[NUM_VIEW];
protected:
    HWND* MainHWND;
    /* State machine */
    void Set_ThreadAction(void (CISPProc::*fnThreadProcStatus)());
    void Call_ThreadAction(void (CISPProc::*fnThreadProcStatus)());
    void (CISPProc::*m_fnThreadProcStatus)();
    void (CISPProc::*m_fnThreadProcStatus_backup)();
public:
    HANDLE m_hThreadMutex;
    void LockGUI();
    void UnlockGUI();

    CWinThread *m_pAssistThread;
    DWORD AssistThread(LPVOID pArg);
    void Thread_Pause();
    void Thread_Idle();
    void Thread_CheckUSBConnect();
    void Thread_CheckDeviceConnect();
    void Thread_CheckDisconnect();
    void Thread_ProgramFlash();

    unsigned char m_ucFW_VER;
    unsigned int m_ulDeviceID;
    unsigned int m_CONFIG[2];
    unsigned int m_CONFIG_User[2];

    unsigned int m_uAPROM_Size;
    unsigned int m_uNVM_Addr;
    unsigned int m_uNVM_Size;

    // Programming Option is binding with UI
    BOOL	m_bProgram_APROM;
    BOOL	m_bProgram_NVM;
    BOOL	m_bProgram_Config;
    BOOL	m_bErase;

    ISPLdCMD	m_ISPLdDev;
    void SetInterface(unsigned int it, CString str) {
        m_ISPLdDev.SetInterface(it, str);
    };

	EProcSts m_eProcSts;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_ISP_PROC_H_)

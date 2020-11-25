// ICPToolDlg.h : header file
//

#if !defined(_DLG_NUVOISP_H_)
#define _DLG_NUVOISP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#if (CMD_SECURITY)
//#else
//#endif // #if (CMD_SECURITY)

#include "ResourceISP.h"
#include "Resource.h"
#include "DialogHex.h"
//#include "..\DialogScrollable.h"
#include "DialogResize.h"
#include "ISPProc.h"

//#include "..\PartNumID.h"

#include "FlashInfo.h"	// Size & Addres of APROM, LDROM, SPROM
#include "PPTooltip.h"

#include "HyperLink.h"

#define NUM_VIEW 2
/////////////////////////////////////////////////////////////////////////////
// CNuvoISPDlg dialog

//class CNuvoISPDlg:public CDialogResize, public CISPProc
class CNuvoISPDlg:public CDialogResize, public CISPProc
{
// Construction
public:
    //enum { IDD = IDD_DIALOG_NUC100};
    //enum { IDD = IDD_DIALOG_NUC400};
    enum { IDD = IDD_DIALOG_NUVOISP};

    CNuvoISPDlg(UINT Template = CNuvoISPDlg::IDD, CWnd* pParent = NULL);	// standard constructor
    virtual ~CNuvoISPDlg();
    CString	m_sConnect;
    CTabCtrl	m_TabData;
    CDialogHex *pViewer[NUM_VIEW];
    CProgressCtrl	m_Progress;
    CPPToolTip m_tooltip;

    WINCTRLID m_CtrlID[NUM_VIEW];
    // virtual BOOL PreCreateWindow(CREATESTRUCT& cs)

    // Temp
    BOOL m_bConnect;
    CButton	m_ButtonConnect;
    void OnButtonBinFile(int idx, TCHAR *szPath = NULL);
    CString	m_sStatus;
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void EnableProgramOption(BOOL bEnable);

protected:
    HICON m_hIcon;
    // Generated message map functions
    //{{AFX_MSG(CNuvoISPDlg)
    void OnOK() {};
    //afx_msg void OnClose();
    virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    //virtual void OnCancel();
    afx_msg void OnButtonConnect();
    afx_msg void OnButtonLoadFile();
    afx_msg void OnButtonStart();
    afx_msg void OnSelchangeTabData(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnButtonConfig();
    afx_msg void OnPaint();
    //}}AFX_MSG
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

    void ShowChipInfo();

public:
    FLASH_PID_INFO_BASE_T m_sPidInfoBase;
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLG_NUVOISP_H_)

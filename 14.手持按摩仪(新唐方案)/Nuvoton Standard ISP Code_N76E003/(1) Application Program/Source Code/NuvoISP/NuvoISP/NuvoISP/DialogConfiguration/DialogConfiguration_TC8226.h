#if !defined(AFX_DIALOGCONFIGURATION_TC8226_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_)
#define AFX_DIALOGCONFIGURATION_TC8226_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogConfiguration.h : header file
//

#include "DialogResize2.h"

// #include "resource.h"
// #include "DialogResize.h"
// #include "NumEdit.h"



/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_TC8226 dialog

class CDialogConfiguration_TC8226 : public CDialogResize2
{
// Construction
public:
	CDialogConfiguration_TC8226(unsigned int uProgramMemorySize = 512 * 1024,
							  CWnd* pParent = NULL);   // standard constructor

	static CString GetConfigWarning(const CAppConfig::TC8226_configs_t &config);

	CAppConfig::TC8226_configs_t m_ConfigValue;

// Dialog Data
	//{{AFX_DATA(CDialogConfiguration_TC8226)
	enum { IDD = IDD_DIALOG_CONFIGURATION_TC8226 };
	CNumEdit	m_FlashBaseAddress;
	CEdit	m_DataFlashSize;
	int		m_nRadioClk;
	int		m_nRadioBov;
	int		m_nRadioBS;
	CString	m_sConfigValue0;
	CString	m_sConfigValue1;

	CString	m_sFlashBaseAddress;
	CString	m_sDataFlashSize;
	BOOL	m_bCheckBrownOutDetect;
	BOOL	m_bCheckBrownOutReset;
	BOOL	m_bClockFilterEnable;
	BOOL	m_bDataFlashEnable;
	BOOL	m_bSecurityLock;
	BOOL	m_bWDTEnable;
	BOOL	m_bWDTPowerDown;
	BOOL	m_bMassErase;
	BOOL	m_bSpromLockCacheable;
	int		m_nRadioGPG;
	int		m_nRadioGPG32K;
	int		m_nRadioRMIIEnable;
	int		m_nRadioIO;
	CSpinButtonCtrl	m_SpinDataFlashSize;

	unsigned int	m_uProgramMemorySize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfiguration_TC8226)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ConfigToGUI(int nEventID);
	void GUIToConfig(int nEventID);
	void OnGUIEvent(int nEventID = 0);
	unsigned int FMC_CRC8(unsigned int au32Data[], unsigned int i32Count);

	// Generated message map functions
	//{{AFX_MSG(CDialogConfiguration_TC8226)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioBov();
	afx_msg void OnRadioClk();
	afx_msg void OnRadioBs();
	afx_msg void OnRadioGpf();
	afx_msg void OnRadioIO();
	afx_msg void OnCheckClick();
	afx_msg void OnCheckClickWDTPD();
	afx_msg void OnCheckClickWDT();
	afx_msg void OnChangeEditFlashBaseAddress();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGURATION_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_)

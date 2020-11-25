#if !defined(AFX_DIALOGCONFIGURATION_M451_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_)
#define AFX_DIALOGCONFIGURATION_M451_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogConfiguration.h : header file
//

#include "DialogResize2.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_M451 dialog

class CDialogConfiguration_M451 : public CDialogResize2
{
// Construction
public:
	CDialogConfiguration_M451(unsigned int uProgramMemorySize = 256 * 1024,
							  CWnd* pParent = NULL);   // standard constructor

	static CString GetConfigWarning(const CAppConfig::M451_configs_t &config);

	CAppConfig::M451_configs_t m_ConfigValue;

// Dialog Data
	//{{AFX_DATA(CDialogConfiguration_NUC4xx)
	enum { IDD = IDD_DIALOG_CONFIGURATION_M451 };
	CNumEdit	m_FlashBaseAddress;
	CNumEdit	m_FlashWriteProtect;
	CEdit	m_DataFlashSize;
	int		m_nRadioClk;
	int		m_nRadioBov;
	int		m_nRadioBS;
	CString	m_sConfigValue0;
	CString	m_sConfigValue1;
	CString	m_sFlashWriteProtect;
	CString	m_sFlashBaseAddress;
	CString	m_sDataFlashSize;
	BOOL	m_bCheckBrownOutDetect;
	BOOL	m_bCheckBrownOutReset;
	BOOL	m_bClockFilterEnable;
	BOOL	m_bDataFlashEnable;
	BOOL	m_bLDWRProtectEnable;
	BOOL	m_bSecurityLock;
	BOOL	m_bWDTEnable;
	BOOL	m_bWDTPowerDown;
	int		m_nRadioGPG;
	int		m_nRadioGPG32K;
	int		m_nRadioRMIIEnable;
	int		m_nRadioIO;
	CSpinButtonCtrl	m_SpinDataFlashSize;

	unsigned int	m_uProgramMemorySize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfiguration_NUC4xx)
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
	//{{AFX_MSG(CDialogConfiguration_NUC4xx)
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
	afx_msg void OnChangeEditFlashWRProtect();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnRadioB();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGURATION_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_)

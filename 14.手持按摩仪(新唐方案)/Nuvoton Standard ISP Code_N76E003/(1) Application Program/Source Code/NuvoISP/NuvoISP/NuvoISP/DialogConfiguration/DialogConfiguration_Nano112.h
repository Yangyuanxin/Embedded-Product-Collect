#if !defined(AFX_DIALOGCONFIGURATION_NANO112_H__E0CD7A09_C5DC_481E_A871_03B422A615F8__INCLUDED_)
#define AFX_DIALOGCONFIGURATION_NANO112_H__E0CD7A09_C5DC_481E_A871_03B422A615F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogConfiguration.h : header file
//

#include "DialogResize2.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_Nano112 dialog

class CDialogConfiguration_Nano112 : public CDialogResize2
{
// Construction
public:
	CDialogConfiguration_Nano112(unsigned int uProgramMemorySize = 32 * 1024,
								 CWnd* pParent = NULL);   // standard constructor

	static CString GetConfigWarning(const CAppConfig::Nano100_configs_t &config);

	CAppConfig::Nano100_configs_t m_ConfigValue;

// Dialog Data
	//{{AFX_DATA(CDialogConfiguration_Nano112)
	enum { IDD = IDD_DIALOG_CONFIGURATION_NANO112 };
	CNumEdit	m_FlashBaseAddress;
	CEdit	m_DataFlashSize;
	int		m_nRadioClk;
	int		m_nRadioBor;
	int		m_nRadioBS;
	int		m_nRadioHXT;
	CString	m_sConfigValue0;
	CString	m_sConfigValue1;
	CString	m_sFlashBaseAddress;
	CString	m_sDataFlashSize;
	BOOL	m_bClockFilterEnable;
	BOOL	m_bDataFlashEnable;
	BOOL	m_bSecurityLock;
	BOOL	m_bWDTEnable;
	CSpinButtonCtrl	m_SpinDataFlashSize;

	unsigned int	m_uProgramMemorySize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfiguration_Nano112)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ConfigToGUI();
	void GUIToConfig();


	// Generated message map functions
	//{{AFX_MSG(CDialogConfiguration_Nano112)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioBov();
	afx_msg void OnRadioClk();
	afx_msg void OnRadioBs();
	afx_msg void OnCheckClick();
	afx_msg void OnChangeEditFlashBaseAddress();
	virtual void OnOK();
	afx_msg void OnCheckClickWDT();
	afx_msg void OnRadioBor();
	afx_msg void OnRadioHxt();
	afx_msg void OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGURATION_NANO112_H__E0CD7A09_C5DC_481E_A871_03B422A615F8__INCLUDED_)

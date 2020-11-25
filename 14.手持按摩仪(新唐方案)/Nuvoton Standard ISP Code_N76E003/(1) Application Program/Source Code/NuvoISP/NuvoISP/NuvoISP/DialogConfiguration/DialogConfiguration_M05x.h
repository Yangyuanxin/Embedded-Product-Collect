#if !defined(AFX_DIALOGCONFIGURATION_M05X_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_)
#define AFX_DIALOGCONFIGURATION_M05X_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogConfiguration.h : header file
//

#include "DialogResize2.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_M05x dialog

class CDialogConfiguration_M05x : public CDialogResize2
{
// Construction
public:
	CDialogConfiguration_M05x(CWnd* pParent = NULL);   // standard constructor

	static CString GetConfigWarning(const CAppConfig::M05x_configs_t &config);

	CAppConfig::M05x_configs_t m_ConfigValue;

// Dialog Data
	//{{AFX_DATA(CDialogConfiguration_M05x)
	enum { IDD = IDD_DIALOG_CONFIGURATION_M051 };
	CNumEdit	m_FlashBaseAddress;
	int		m_nRadioClk;
	int		m_nRadioBov;
	int		m_nRadioBS;
	CString	m_sConfigValue0;
	CString	m_sConfigValue1;
	BOOL	m_bCheckBrownOutDetect;
	BOOL	m_bCheckBrownOutReset;
	BOOL	m_bClockFilterEnable;
	BOOL	m_bDataFlashEnable;
	BOOL	m_bSecurityLock;
	CString	m_sFlashBaseAddress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfiguration_M05x)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ConfigToGUI();
	void GUIToConfig();


	// Generated message map functions
	//{{AFX_MSG(CDialogConfiguration_M05x)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioBov();
	afx_msg void OnRadioClk();
	afx_msg void OnRadioBs();
	afx_msg void OnCheckClick();
	afx_msg void OnChangeEditFlashBaseAddress();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGURATION_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_)

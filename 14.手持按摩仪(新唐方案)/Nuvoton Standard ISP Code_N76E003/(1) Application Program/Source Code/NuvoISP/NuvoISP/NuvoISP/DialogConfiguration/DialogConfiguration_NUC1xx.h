#if !defined(AFX_DIALOGCONFIGURATION_NUC1XX_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_)
#define AFX_DIALOGCONFIGURATION_NUC1XX_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogConfiguration.h : header file
//

#include "DialogResize2.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_NUC1xx dialog

class CDialogConfiguration_NUC1xx : public CDialogResize2
{
// Construction
public:
	CDialogConfiguration_NUC1xx(BOOL bIsDataFlashFixed = FALSE,
								unsigned int uProgramMemorySize = 128 * 1024,
								unsigned int uDataFlashSize = 0,
								CWnd* pParent = NULL);   // standard constructor
	static CString GetConfigWarning(const CAppConfig::NUC1xx_configs_t &config);

	CAppConfig::NUC1xx_configs_t m_ConfigValue;

// Dialog Data
	//{{AFX_DATA(CDialogConfiguration_NUC1xx)
	enum { IDD = IDD_DIALOG_CONFIGURATION_NUC100 };
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
	CButton	m_DataFlashEnable;
	CSpinButtonCtrl	m_SpinDataFlashSize;

	BOOL	m_bIsDataFlashFixed;
	unsigned int	m_uProgramMemorySize;
	unsigned int	m_uDataFlashSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfiguration_NUC1xx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ConfigToGUI();
	void GUIToConfig();


	// Generated message map functions
	//{{AFX_MSG(CDialogConfiguration_NUC1xx)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioBov();
	afx_msg void OnRadioClk();
	afx_msg void OnRadioBs();
	afx_msg void OnCheckClick();
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

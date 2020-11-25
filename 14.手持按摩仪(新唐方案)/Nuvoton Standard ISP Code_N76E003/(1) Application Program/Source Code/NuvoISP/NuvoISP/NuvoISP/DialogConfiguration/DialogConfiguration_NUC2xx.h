#if !defined(AFX_DIALOGCONFIGURATION_NUC2XX_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_)
#define AFX_DIALOGCONFIGURATION_NUC2XX_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogConfiguration.h : header file
//

#include "DialogResize2.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_NUC2xx dialog

class CDialogConfiguration_NUC2xx : public CDialogResize2
{
// Construction
public:
	CDialogConfiguration_NUC2xx(BOOL bIsDataFlashFixed = FALSE,
								unsigned int uProgramMemorySize = 128 * 1024,
								unsigned int uDataFlashSize = 0,
								CWnd* pParent = NULL);   // standard constructor

	static CString GetConfigWarning(const CAppConfig::NUC1xx_configs_t &config);

	CAppConfig::NUC1xx_configs_t m_ConfigValue;

// Dialog Data
	//{{AFX_DATA(CDialogConfiguration_NUC2xx)
	enum { IDD = IDD_DIALOG_CONFIGURATION_NUC200 };
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
	int		m_nRadioGPF;
	int		m_nRadioIO;
	CButton	m_DataFlashEnable;
	CSpinButtonCtrl	m_SpinDataFlashSize;

	BOOL	m_bIsDataFlashFixed;
	unsigned int	m_uProgramMemorySize;
	unsigned int	m_uDataFlashSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfiguration_NUC2xx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ConfigToGUI(int nEventID);
	void GUIToConfig(int nEventID);
	void OnGUIEvent(int nEventID = 0);


	// Generated message map functions
	//{{AFX_MSG(CDialogConfiguration_NUC2xx)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioBov();
	afx_msg void OnRadioClk();
	afx_msg void OnRadioBs();
	afx_msg void OnRadioGpf();
	afx_msg void OnRadioIO();
	afx_msg void OnCheckClick();
	afx_msg void OnCheckClickWDTPD();
	afx_msg void OnCheckClickWDT();
	//afx_msg void OnChangeEditFlashBaseAddress();
	afx_msg void OnKillfocusEditFlashBaseAddress();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGURATION_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_)

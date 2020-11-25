#if !defined(AFX_DIALOGCONFIGURATION_MT500_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_)
#define AFX_DIALOGCONFIGURATION_MT500_H__E0CD7A09_C5DC_481E_A871_03B422E615F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogResize2.h"
// CDialogConfiguration_MT500 ¹ï¸Ü¤è¶ô

class CDialogConfiguration_MT500 : public CDialogResize2
{
	//DECLARE_DYNAMIC(CDialogConfiguration_MT500)
// Construction
public:
	CDialogConfiguration_MT500(BOOL bIsDataFlashFixed = FALSE,
								unsigned int uProgramMemorySize = 128 * 1024,
								unsigned int uDataFlashSize = 0,
								CWnd* pParent = NULL);   // standard constructor

	static CString GetConfigWarning(const CAppConfig::MT5xx_configs_t &config);

	CAppConfig::MT5xx_configs_t m_ConfigValue;

// Dialog Data
	//{{AFX_DATA(CDialogConfiguration_MT500)
	enum { IDD = IDD_DIALOG_CONFIGURATION_MT500 };
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
	BOOL	m_bCHZ_Even0;
	BOOL	m_bCHZ_Even1;
	BOOL	m_BCHZ_Odd0;
	BOOL	m_BCHZ_Odd1;
	BOOL	m_BCHZ_BPWM;
	BOOL	m_bWDTEnable;
	BOOL	m_bWDTPowerDown;
	CButton	m_DataFlashEnable;
	CSpinButtonCtrl	m_SpinDataFlashSize;

	BOOL	m_bIsDataFlashFixed;
	unsigned int	m_uProgramMemorySize;
	unsigned int	m_uDataFlashSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfiguration_MT500)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ConfigToGUI(int);
	void GUIToConfig(int);


	// Generated message map functions
	//{{AFX_MSG(CDialogConfiguration_MT500)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioBov();
	afx_msg void OnRadioClk();
	afx_msg void OnRadioBs();
	afx_msg void OnCheckClick();
	afx_msg void OnGUIEvent(int);
	afx_msg void OnCheckClickWDTPD();
	afx_msg void OnCheckClickWDT();
	afx_msg void OnChangeEditFlashBaseAddress();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif

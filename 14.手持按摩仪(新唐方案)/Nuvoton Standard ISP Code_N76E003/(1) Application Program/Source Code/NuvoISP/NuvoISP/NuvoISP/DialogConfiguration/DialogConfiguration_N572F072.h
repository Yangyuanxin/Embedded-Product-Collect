#pragma once

#include "DialogResize2.h"
// CDialogConfiguration_N572F072 dialog

class CDialogConfiguration_N572F072 : public CDialogResize2
{
	DECLARE_DYNAMIC(CDialogConfiguration_N572F072)

public:
	CDialogConfiguration_N572F072(CWnd* pParent = NULL);   // standard constructor

	static CString GetConfigWarning(const CAppConfig::N572_configs_t &config);

	CAppConfig::N572_configs_t m_ConfigValue;

	virtual ~CDialogConfiguration_N572F072();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFIGURATION_N572F072 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog ();

	DECLARE_MESSAGE_MAP()
public:
	int m_uCoarseTimingCotrol;
	int m_uFineTimingControl;
	int m_nRadioCVDTV;
	BOOL m_bWatchDogEnable;
	BOOL m_bProtect8K;
	BOOL m_bVoltageDetectorEnable;
	BOOL m_bSecurityLock;
	CString m_csConfigValue0;

protected:
	void ConfigToGUI ();
	void GUIToConfig ();
public:
	afx_msg void OnOK ();
	afx_msg void OnChange();
};

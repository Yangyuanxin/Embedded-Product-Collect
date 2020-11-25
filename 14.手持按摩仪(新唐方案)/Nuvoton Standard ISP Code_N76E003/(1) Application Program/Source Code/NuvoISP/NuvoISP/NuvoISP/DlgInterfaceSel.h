#if !defined(AFX_DIALOGCHIPSELECT_H__456309EB_988C_488F_BF5B_AB63840FB8D5__INCLUDED_)
#define AFX_DIALOGCHIPSELECT_H__456309EB_988C_488F_BF5B_AB63840FB8D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogChipSelect.h : header file
//
#include "ResourceISP.h"
#include "HyperLink.h"
/////////////////////////////////////////////////////////////////////////////
// DlgInterfaceSel dialog

class DlgInterfaceSel : public CDialog
{
// Construction
public:
	DlgInterfaceSel(
		UINT nIDTemplate = DlgInterfaceSel::IDD,
		CWnd* pParent = NULL);   // standard constructor

	unsigned int m_Interface;
	CString m_ComNum;
// Dialog Data
	//{{AFX_DATA(DlgInterfaceSel)
	//enum { IDD = IDD_DIALOG_CHIPSELECT };
	enum { IDD = IDD_DIALOG_INTERFACE };
	CComboBox	m_SelInterface;
	CComboBox	m_SelComPort;
	CHyperLink	m_LinkNuvotonM0;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgInterfaceSel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	LANGID GetSelectLangID();
	//std::vector<std::tstring>	m_chips;
	int							m_nSelect;
	BOOL						m_bNeedChangeLang;
	LANGID						m_langID;

	// Generated message map functions
	//{{AFX_MSG(DlgInterfaceSel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeComboLanguage();
	afx_msg void OnLinkNuvotonM0();
	afx_msg void OnComboChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	UINT ScanPCCom();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCHIPSELECT_H__456309EB_988C_488F_BF5B_AB63840FB8D5__INCLUDED_)

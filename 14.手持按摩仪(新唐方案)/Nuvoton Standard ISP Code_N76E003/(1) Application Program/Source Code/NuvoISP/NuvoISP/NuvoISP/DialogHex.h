#if !defined(AFX_DIALOGHEX_H__6734BB13_4E7E_4EEB_A921_0BE656E8E378__INCLUDED_)
#define AFX_DIALOGHEX_H__6734BB13_4E7E_4EEB_A921_0BE656E8E378__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

// DialogHex.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogHex dialog

class CDialogHex : public CDialog
{
// Construction
public:
	CDialogHex(CWnd* pParent = NULL);   // standard constructor

	void SetHexData(const std::vector<unsigned char> *pHexData, const CString &sAltInfo = _T(""));
	void SetHexData(const std::vector<unsigned char> *pHexData, unsigned int uStartByte, const CString &sAltInfo = _T(""));
	void UpdateHexView(int nWidth);
	BOOL EnableWindow(BOOL bEnable);

// Dialog Data
	//{{AFX_DATA(CDialogHex)
	enum { IDD = IDD_DIALOG_HEX };
	CEdit	m_Edit_DataView;
	CRichEditCtrl	m_REdit_DataView;
	CButton	m_ButtonSaveAs;
	int		m_nRadioUINT;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogHex)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFont m_Font0;
	CFont m_Font1;
	CFont m_Font2;
	std::vector<unsigned char> m_tmpdata_test;
	const std::vector<unsigned char> *m_pHexData;
	CString m_sAltInfo;
	unsigned int m_uStartByte;

	// Generated message map functions
	//{{AFX_MSG(CDialogHex)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRadioUint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGHEX_H__6734BB13_4E7E_4EEB_A921_0BE656E8E378__INCLUDED_)
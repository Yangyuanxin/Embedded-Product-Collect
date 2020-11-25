#if !defined(AFX_NUMEDIT_H__F1B78C9F_DFF8_48C3_8BF4_7A2C19722E25__INCLUDED_)
#define AFX_NUMEDIT_H__F1B78C9F_DFF8_48C3_8BF4_7A2C19722E25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NumEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNumEdit window

class CNumEdit : public CEdit
{
// Construction
public:
	CNumEdit(int nRadix = 16, int nMaxLength = 8, bool bInsertMode = false, bool bUseLowerCase = false);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumEdit)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNumEdit();

	bool IsInsertMode() const;
	void SetInsertMode(bool bInsertMode);

	// Generated message map functions
protected:
	int m_nRadix;
	int m_nMaxLength;
	bool m_bInsertMode;
	bool m_bUseLowerCase;

	//{{AFX_MSG(CNumEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMEDIT_H__F1B78C9F_DFF8_48C3_8BF4_7A2C19722E25__INCLUDED_)

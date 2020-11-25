// NumEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NumEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumEdit

CNumEdit::CNumEdit(int nRadix, int nMaxLength, bool bInsertMode, bool bUseLowerCase)
	: m_nRadix(nRadix)
	, m_nMaxLength(nMaxLength)
	, m_bInsertMode(bInsertMode)
	, m_bUseLowerCase(bUseLowerCase)
{
}

CNumEdit::~CNumEdit()
{
}



BEGIN_MESSAGE_MAP(CNumEdit, CEdit)
	//{{AFX_MSG_MAP(CNumEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


static bool is_digit(int nChar, int nRadix)
{
	if(nRadix <= 10)	/* 0 - 9 */
	{
		if(nChar >= '0' && nChar < '0' + nRadix)
			return true;
	}
	else if(nRadix <= 36)	/* 0 - 9,  A - Z */
	{
		if(nChar >= '0' && nChar <= '9')
			return true;
		if(nChar >= 'a' && nChar < 'a' + nRadix - 10)
			return true;
		if(nChar >= 'A' && nChar < 'A' + nRadix - 10)
			return true;
	}

	return false;
}


void UpdateCaret(CNumEdit* pEdit, BOOL bInsert)
{
	CFont* pFont = pEdit->GetFont();
	CDC* pDC = pEdit->GetDC();
	CFont* pOldFont = pDC->SelectObject(pFont);
	CSize sizeChar = pDC->GetTextExtent(_T("0"));
	if (bInsert)
	{
		// Insert mode, vertical line caret
		sizeChar.cx = 0;
	}
	else
	{
		// Checks whether caret is at the end of current line
		int nLineIndex = pEdit->LineIndex();
		int nLineLength = pEdit->LineLength();
		int nStart, nEnd;
		pEdit->GetSel(nStart, nEnd);
		if (nStart == nEnd && nStart != nLineIndex + nLineLength)
		{
			// No text selected & caret is not at end of line
			// So, gets next character
			TCHAR* strLine = new TCHAR[nLineLength + 1];
			pEdit->GetLine(pEdit->LineFromChar(), strLine, nLineLength);
			strLine[nStart - nLineIndex + 1] = 0;

			// Sets caret size as size of next character
			sizeChar = pDC->GetTextExtent(strLine + (nStart - nLineIndex));
			delete[] strLine;
		}
	}

#if 0
	// Destroys previous caret and displays new caret
	DestroyCaret();
	pEdit->HideCaret();
	pEdit->CreateSolidCaret(sizeChar.cx, sizeChar.cy);
	pEdit->ShowCaret();
#endif

	// Stores caret width in application object
	pDC->SelectObject(pOldFont);
}


/////////////////////////////////////////////////////////////////////////////
// CNumEdit message handlers

void CNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//printf("ON CHAR %d\n", nChar);
	// TODO: Add your message handler code here and/or call default
	if(::isgraph(nChar) && !is_digit(nChar, m_nRadix))
		return;

	if (this->GetStyle() & ES_READONLY)
		return;

	CString str_bak;
	GetWindowText(str_bak);

	int nStartChar, nEndChar;
	GetSel(nStartChar, nEndChar);
	
	this->SetRedraw(FALSE);

	/* Remove next char */
	if(m_bInsertMode == false && nChar >= ' ' && nChar <= 0xFF)
	{
		//GetSel(nStartChar, nEndChar);
		SetSel(nStartChar, nStartChar + 1);
		ReplaceSel(_T(""));
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	CString str;
	GetWindowText(str);
	int i;
	for(i = 0; i < str.GetLength(); ++i)
	{
		if(!is_digit(str.GetAt(i), m_nRadix))
			break;
	}
	this->SetRedraw(TRUE);

	/* Change to lower case or upper case */
	if(m_bUseLowerCase != false)
		str.MakeLower();
	else
		str.MakeUpper();

	/* Check if it's valid */
	if(str.GetLength() > m_nMaxLength || i < str.GetLength())
	{
		/* Restore backup str */
		SetWindowText(str_bak);
		SetSel(nStartChar, nEndChar);
	}
	else
	{
		GetSel(nStartChar, nEndChar);
		SetWindowText(str);
		SetSel(nStartChar, nEndChar);
	}
}


bool CNumEdit::IsInsertMode() const
{
	return m_bInsertMode;
}

void CNumEdit::SetInsertMode(bool bInsertMode)
{
	m_bInsertMode = bInsertMode;
	UpdateCaret(this, m_bInsertMode);
}

LRESULT CNumEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message)
	{
	case (WM_KEYUP):
		if(wParam == VK_INSERT)
			SetInsertMode(!m_bInsertMode);
		//Go on ..
	case (WM_KEYDOWN):
	case (WM_LBUTTONDOWN):
	case (WM_LBUTTONUP):
	case (WM_SETFOCUS):
	case (WM_CHAR):
		if(m_bInsertMode == false)
		{
			UpdateCaret(this, false);
			break;
		}
	}
	return CEdit::WindowProc(message, wParam, lParam);
}

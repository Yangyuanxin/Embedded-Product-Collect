// DialogScrollable.cpp : implementation file
//

#include "stdafx.h"
#include "DialogResize2.h"
#include "resource.h"
#include <cstring>

// CDialogScrollable dialog

IMPLEMENT_DYNAMIC(CDialogResize2, CDialog)

CDialogResize2::CDialogResize2(UINT nIDTemplate, CWnd* pParent /*=NULL*/ )
	: CDialog(nIDTemplate, pParent)
{
	m_bIsInitialized = false;
	m_bShowScrollBar = false;
	m_ScrollBarWidth = 15;
}

CDialogResize2::~CDialogResize2()
{
}

BEGIN_MESSAGE_MAP(CDialogResize2, CDialog)
	//{{AFX_MSG_MAP(CDialogResize2)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDialogResize2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CDialogResize2::OnSize(UINT nType, int cx, int cy)
{
	// TODO: Add your message handler code here.
	CDialog::OnSize(nType, cx, cy);

	int nScrollMaxX;
	int nScrollMaxY;
	CRect rect;

	ScrollWindow(m_nScrollPosX, m_nScrollPosY);
	m_nScrollPosX = 0;
	m_nScrollPosY = 0;
	GetWindowRect(rect);

	if(!m_bIsInitialized)
	{
		nScrollMaxY = 0;
		nScrollMaxX = 0;
	}
	else
	{
		if ( rect.Height() < m_rect.Height() )
			nScrollMaxY = m_rect.Height() - rect.Height() + m_ScrollBarWidth;
		else
			nScrollMaxY = 0;

		if ( rect.Width() < m_rect.Width() )
			nScrollMaxX = m_rect.Width() - rect.Width() + m_ScrollBarWidth;
		else
			nScrollMaxX = 0;

		if(nScrollMaxY != 0 && nScrollMaxX == 0)
			nScrollMaxX = 10;	
		else if(nScrollMaxY == 0 && nScrollMaxX != 0)
			nScrollMaxY = 10;
	}

	if(nScrollMaxX == 0 || nScrollMaxY == 0)
		m_bShowScrollBar = false;
	else
		m_bShowScrollBar = true;

	SCROLLINFO si_y;
	si_y.cbSize = sizeof(SCROLLINFO);
	si_y.fMask = SIF_ALL; // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
	si_y.nMin = 0;
	si_y.nMax = nScrollMaxY;
	si_y.nPage = si_y.nMax/10;
	si_y.nPos = 0;
	SetScrollInfo(SB_VERT, &si_y, TRUE);
	
	SCROLLINFO si_x;
	si_x.cbSize = sizeof(SCROLLINFO);
	si_x.fMask = SIF_ALL; // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
	si_x.nMin = 0;
	si_x.nMax = nScrollMaxX;
	si_x.nPage = si_x.nMax/10;
	si_x.nPos = 0;
	SetScrollInfo(SB_HORZ, &si_x, TRUE);

	// TODO: Add your message handler code here
}

void CDialogResize2::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo);
	int nMaxPos = scrollinfo.nMax;
	int nDelta;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (m_nScrollPosY >= nMaxPos)
			return;
		nDelta = min(nMaxPos/100, nMaxPos - m_nScrollPosY);
		if(nMaxPos/100 == 0)
			nDelta = 1;
		break;

	case SB_LINEUP:
		if (m_nScrollPosY <= 0)
			return;
		nDelta = -min(nMaxPos/100, m_nScrollPosY);
		if(nMaxPos/100 == 0)
			nDelta = -1;
		break;

	case SB_PAGEDOWN:
		if (m_nScrollPosY >= nMaxPos)
			return;
		nDelta = min(nMaxPos/10, nMaxPos - m_nScrollPosY);
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPosY;
		break;

	case SB_PAGEUP:
		if (m_nScrollPosY <= 0)
			return;
		nDelta = -min(nMaxPos/10, m_nScrollPosY);
		break;
	
	default:
		return;
	}
	m_nScrollPosY += nDelta;
	SetScrollPos(SB_VERT, m_nScrollPosY, TRUE);
	ScrollWindow(0, -nDelta);
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDialogResize2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_HORZ, &scrollinfo);
	int nMaxPos = scrollinfo.nMax;
	int nDelta;

	switch (nSBCode)
	{
	case SB_LINERIGHT:
		if (m_nScrollPosX >= nMaxPos)
			return;
		nDelta = min(nMaxPos/100, nMaxPos - m_nScrollPosX);
		if(nMaxPos/100 == 0)
			nDelta = 1;
		break;

	case SB_LINELEFT:
		if (m_nScrollPosX <= 0)
			return;
		nDelta = -min(nMaxPos/100, m_nScrollPosX);
		if(nMaxPos/100 == 0)
			nDelta = -1;
		break;

	case SB_PAGERIGHT:
		if (m_nScrollPosX >= nMaxPos)
			return;
		nDelta = min(nMaxPos/10, nMaxPos - m_nScrollPosX);
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPosX;
		break;

	case SB_PAGELEFT:
		if (m_nScrollPosX <= 0)
			return;
		nDelta = -min(nMaxPos/10, m_nScrollPosX);
		break;
	
	default:
		return;
	}

	m_nScrollPosX += nDelta;
	SetScrollPos(SB_HORZ, m_nScrollPosX, TRUE);
	ScrollWindow(-nDelta, 0);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDialogResize2::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnGetMinMaxInfo(lpMMI);
	if(m_bIsInitialized)
	{
		lpMMI->ptMaxTrackSize.x = m_rect.Width();
		lpMMI->ptMaxTrackSize.y = m_rect.Height();
		lpMMI->ptMinTrackSize.x = 310;
		lpMMI->ptMinTrackSize.y = 310;
	}
}
BOOL CDialogResize2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnMouseWheel(nFlags, zDelta, pt);

	if(m_bShowScrollBar)
	{
		SCROLLINFO scrollinfo;
		GetScrollInfo(SB_VERT, &scrollinfo);
		int nMaxPos = scrollinfo.nMax;
		int nScrollPos = scrollinfo.nPos;
		int nDelta;

		if(zDelta < 0)
		{
			if (m_nScrollPosY >= nMaxPos)
				return 0;
			nDelta = min(nMaxPos/10, nMaxPos - m_nScrollPosY);
		}
		else
		{
			if (m_nScrollPosY <= 0)
				return 0;
			nDelta = -min(nMaxPos/10, m_nScrollPosY);
		}

		m_nScrollPosY += nDelta;
		SetScrollPos(SB_VERT, m_nScrollPosY, TRUE);
		ScrollWindow(0, -nDelta);
	}
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

#define DISABLE_ITEM(ID)	{	\
	pWnd = GetDlgItem(ID);		\
    if(pWnd != NULL)			\
	pWnd->EnableWindow(FALSE); }

void CDialogResize2::AdjustDPI()
{
	int nScreenHeight;
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	if(nScreenHeight < m_rect.Height())
	{
		CRect r1;
		GetDlgItem(IDCANCEL)->GetWindowRect(r1);
		m_rect.bottom = r1.bottom + 30;
		MoveWindow(m_rect.left, m_rect.top, m_rect.Width(), nScreenHeight);
	}

	CWnd* pWnd;
	/* Boot Options */
	DISABLE_ITEM(IDC_RADIO_BS_LDROM);
	DISABLE_ITEM(IDC_RADIO_BS_APROM);
	DISABLE_ITEM(IDC_RADIO_BS_LDROM_APROM);
	DISABLE_ITEM(IDC_RADIO_BS_APROM_LDROM);
	DISABLE_ITEM(IDC_RADIO_BS_MKROM);

	/* 8051 LDROM Size Options*/
	DISABLE_ITEM(IDC_RADIO_LDSIZE_0K);
	DISABLE_ITEM(IDC_RADIO_LDSIZE_1K);
	DISABLE_ITEM(IDC_RADIO_LDSIZE_2K);
	DISABLE_ITEM(IDC_RADIO_LDSIZE_3K);
	DISABLE_ITEM(IDC_RADIO_LDSIZE_4K);

	/* Request by MB20 Lucia Wang 2016.07.14 */
	DISABLE_ITEM(IDC_RADIO_RPD_RESET);
	DISABLE_ITEM(IDC_RADIO_RPD_INPUT);
}
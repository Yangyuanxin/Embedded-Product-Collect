#pragma once

#include "resource.h"
#include "AppConfig.h"
#include "NumEdit.h"

class CDialogResize2 : public CDialog
{
	DECLARE_DYNAMIC(CDialogResize2)

public:
	CDialogResize2(UINT nIDTemplate, CWnd* pParent = NULL ); // standard constructor
	virtual ~CDialogResize2();

// Dialog Data
	int m_nScrollPosY; // to store the current vertical scroll position
	int m_nScrollPosX; // to store the current horizontal scroll position
	int m_ScrollBarWidth;
	BOOL m_bIsInitialized;
	BOOL m_bShowScrollBar;
	CRect m_rect;		//original dialog rect info

protected:
	void AdjustDPI();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	DECLARE_MESSAGE_MAP()
};

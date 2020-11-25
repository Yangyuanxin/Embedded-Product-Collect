#pragma once


struct CPartNumID;

struct WINCTRLID {
    int         btn;
    int         path;
    int         sizecksum;
};

class CDialogResize : public CDialog
{
	DECLARE_DYNAMIC(CDialogResize)

public:
	CDialogResize(UINT nIDTemplate, CWnd* pParent = NULL ); // standard constructor
	virtual ~CDialogResize();

// Dialog Data
	int m_nScrollPosY; // to store the current vertical scroll position
	int m_nScrollPosX; // to store the current horizontal scroll position
	int m_ScrollBarWidth;
	BOOL m_bIsInitialized;
	BOOL m_bShowScrollBar;
	CRect m_rect;		//original dialog rect info

	bool ConfigDlgSel(unsigned int *pConfig, unsigned int size);
	bool ConfigSetting(unsigned int id, unsigned int *pConfig, unsigned int size);

	void ShowDlgItem(int nID, int nCmdShow);
	void EnableDlgItem(int nID, BOOL bEnable);

	void Test();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	DECLARE_MESSAGE_MAP()
};

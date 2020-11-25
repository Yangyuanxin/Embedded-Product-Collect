// DialogScrollable.cpp : implementation file
//

#include "stdafx.h"
#include "DialogResize.h"
#include "resource.h"
#include <cstring>

#include "PartNumID.h"
#include "FlashInfo.h"

// CDialogScrollable dialog

IMPLEMENT_DYNAMIC(CDialogResize, CDialog)

CDialogResize::CDialogResize(UINT nIDTemplate, CWnd* pParent /*=NULL*/ )
    : CDialog(nIDTemplate, pParent)
{
    m_bIsInitialized = false;
    m_bShowScrollBar = false;
    m_ScrollBarWidth = 15;
}

CDialogResize::~CDialogResize()
{
}

BEGIN_MESSAGE_MAP(CDialogResize, CDialog)
    //{{AFX_MSG_MAP(CDialogResize)
    ON_WM_SIZE()
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_GETMINMAXINFO()
    ON_WM_MOUSEWHEEL()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDialogResize::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

void CDialogResize::OnSize(UINT nType, int cx, int cy)
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

    if(!m_bIsInitialized) {
        nScrollMaxY = 0;
        nScrollMaxX = 0;
    } else {
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

void CDialogResize::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT, &scrollinfo);
    int nMaxPos = scrollinfo.nMax;
    int nDelta;

    switch (nSBCode) {
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

void CDialogResize::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_HORZ, &scrollinfo);
    int nMaxPos = scrollinfo.nMax;
    int nDelta;

    switch (nSBCode) {
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

void CDialogResize::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: Add your message handler code here and/or call default

    CDialog::OnGetMinMaxInfo(lpMMI);
    if(m_bIsInitialized) {
        lpMMI->ptMaxTrackSize.x = m_rect.Width();
        lpMMI->ptMaxTrackSize.y = m_rect.Height();
        lpMMI->ptMinTrackSize.x = 310;
        lpMMI->ptMinTrackSize.y = 310;
    }
}
BOOL CDialogResize::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Add your message handler code here and/or call default
    CDialog::OnMouseWheel(nFlags, zDelta, pt);

    if(m_bShowScrollBar) {
        SCROLLINFO scrollinfo;
        GetScrollInfo(SB_VERT, &scrollinfo);
        int nMaxPos = scrollinfo.nMax;
        int nScrollPos = scrollinfo.nPos;
        int nDelta;

        if(zDelta < 0) {
            if (m_nScrollPosY >= nMaxPos)
                return 0;
            nDelta = min(nMaxPos/10, nMaxPos - m_nScrollPosY);
        } else {
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

void CDialogResize::ShowDlgItem(int nID, int nCmdShow)
{
    CWnd* pWnd = GetDlgItem(nID);
    if(pWnd != NULL)
        pWnd->ShowWindow(nCmdShow);
}

void CDialogResize::EnableDlgItem(int nID, BOOL bEnable)
{
    CWnd* pWnd = GetDlgItem(nID);
    if(pWnd != NULL)
        pWnd->EnableWindow(bEnable);
}


#include "DialogConfiguration_AU9100.h"
#include "DialogConfiguration_M058.h"
#include "DialogConfiguration_M05x.h"
#include "DialogConfiguration_M05xCN.h"
#include "DialogConfiguration_M451.h"
#include "DialogConfiguration_Mini51.h"
#include "DialogConfiguration_Mini51BN.h"
#include "DialogConfiguration_Mini51CN.h"
#include "DialogConfiguration_MT500.h"
#include "DialogConfiguration_N570.h"
#include "DialogConfiguration_N572.h"
#include "DialogConfiguration_Nano100.h"
#include "DialogConfiguration_Nano100BN.h"
#include "DialogConfiguration_Nano103.h"
#include "DialogConfiguration_Nano112.h"
#include "DialogConfiguration_NM1120.h"
#include "DialogConfiguration_NM1200.h"
#include "DialogConfiguration_NUC102.h"
#include "DialogConfiguration_NUC103.h"
#include "DialogConfiguration_NUC103BN.h"
#include "DialogConfiguration_NUC131.h"
#include "DialogConfiguration_NUC1xx.h"
#include "DialogConfiguration_NUC2xx.h"
#include "DialogConfiguration_NUC4xx.h"

#include "DialogConfiguration_N76E1T.h"

extern CPartNumID *psChipData;

bool CDialogResize::ConfigDlgSel(unsigned int *pConfig, unsigned int size)
{
	bool ret = false;
    CDialog *pConfigDlg = NULL;
    unsigned int *Config;

	BOOL bIsDataFlashFixed = FALSE;
	unsigned int uProgramMemorySize = 0;
	unsigned int uDataFlashSize = 0;

    if(psChipData) {

		if(gsPidInfo.uPID == psChipData->uID)
		{
			bIsDataFlashFixed = gsPidInfo.uDataFlashSize;
			uProgramMemorySize = gsPidInfo.uProgramMemorySize;
			uDataFlashSize = gsPidInfo.uDataFlashSize;
		}

        switch(psChipData->uProjectCode) {
		case IDD_DIALOG_CONFIGURATION_NUC100:

			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_NUC1xx(bIsDataFlashFixed, uProgramMemorySize, uDataFlashSize);	// "NUC100BN";
			else
				pConfigDlg = new CDialogConfiguration_NUC1xx;	// "NUC100BN";

            Config = (unsigned int*)(&(((CDialogConfiguration_NUC1xx*)pConfigDlg)->m_ConfigValue));
            break;


        case IDD_DIALOG_CONFIGURATION_NUC102:
            pConfigDlg = new CDialogConfiguration_NUC102;
            Config = (unsigned int*)(&(((CDialogConfiguration_NUC102*)pConfigDlg)->m_ConfigValue));
            break;

		case IDD_DIALOG_CONFIGURATION_NUC103:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_NUC103(uProgramMemorySize, uDataFlashSize);
			else
				pConfigDlg = new CDialogConfiguration_NUC103;

			Config = (unsigned int*)(&(((CDialogConfiguration_NUC103*)pConfigDlg)->m_ConfigValue));
            break;

		case IDD_DIALOG_CONFIGURATION_NUC200:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_NUC2xx(bIsDataFlashFixed, uProgramMemorySize, uDataFlashSize);
			else
				pConfigDlg = new CDialogConfiguration_NUC2xx;

            Config = (unsigned int*)(&(((CDialogConfiguration_NUC2xx*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_NUC131:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_NUC131(uProgramMemorySize, uDataFlashSize);
			else
				pConfigDlg = new CDialogConfiguration_NUC131;

            Config = (unsigned int*)(&(((CDialogConfiguration_NUC131*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_NANO100:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_Nano100(uProgramMemorySize);
			else
				pConfigDlg = new CDialogConfiguration_Nano100;

            Config = (unsigned int*)(&(((CDialogConfiguration_Nano100*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_NANO100BN:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_Nano100BN(uProgramMemorySize);
			else
				pConfigDlg = new CDialogConfiguration_Nano100BN;

            Config = (unsigned int*)(&(((CDialogConfiguration_Nano100BN*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_NANO112:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_Nano112(uProgramMemorySize);
			else
				pConfigDlg = new CDialogConfiguration_Nano112;

            Config = (unsigned int*)(&(((CDialogConfiguration_Nano112*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_NANO103:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_Nano103(uProgramMemorySize);
			else
				pConfigDlg = new CDialogConfiguration_Nano103;

            Config = (unsigned int*)(&(((CDialogConfiguration_Nano103*)pConfigDlg)->m_ConfigValue));
            break;

		case IDD_DIALOG_CONFIGURATION_M051:
            pConfigDlg = new CDialogConfiguration_M05x;
            Config = (unsigned int*)(&(((CDialogConfiguration_M05x*)pConfigDlg)->m_ConfigValue));
            break;

		case IDD_DIALOG_CONFIGURATION_M051CN:
            pConfigDlg = new CDialogConfiguration_M05xCN;
            Config = (unsigned int*)(&(((CDialogConfiguration_M05xCN*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_M058:
            pConfigDlg = new CDialogConfiguration_M058;
            Config = (unsigned int*)(&(((CDialogConfiguration_M058*)pConfigDlg)->m_ConfigValue));
           break;

        case IDD_DIALOG_CONFIGURATION_MINI51:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_Mini51(uProgramMemorySize);
			else
				pConfigDlg = new CDialogConfiguration_Mini51;

            Config = (unsigned int*)(&(((CDialogConfiguration_Mini51*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_MINI51BN:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_Mini51BN(uProgramMemorySize);
			else
				pConfigDlg = new CDialogConfiguration_Mini51BN;

            Config = (unsigned int*)(&(((CDialogConfiguration_Mini51BN*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_MINI51CN:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_Mini51CN(uProgramMemorySize, (psChipData->uID & 0xFF00) == 0x5800);
			else if((psChipData->uID & 0x0000FF00) == 0x5800)
				pConfigDlg = new CDialogConfiguration_Mini51CN(32*1024, true);
			else
				pConfigDlg = new CDialogConfiguration_Mini51CN;

            Config = (unsigned int*)(&(((CDialogConfiguration_Mini51CN*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_NM1200:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_NM1200(uProgramMemorySize);
			else
				pConfigDlg = new CDialogConfiguration_NM1200;

			Config = (unsigned int*)(&(((CDialogConfiguration_NM1200*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_MT500:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_MT500(bIsDataFlashFixed, uProgramMemorySize, uDataFlashSize);
			else
				pConfigDlg = new CDialogConfiguration_MT500;

			Config = (unsigned int*)(&(((CDialogConfiguration_MT500*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_NUC400:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_NUC4xx(uProgramMemorySize);
			else
				pConfigDlg = new CDialogConfiguration_NUC4xx;

			Config = (unsigned int*)(&(((CDialogConfiguration_NUC4xx*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_M451:
			if(uProgramMemorySize)
				pConfigDlg = new CDialogConfiguration_M451(uProgramMemorySize);
			else
				pConfigDlg = new CDialogConfiguration_M451;

            Config = (unsigned int*)(&(((CDialogConfiguration_M451*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_N572:
            pConfigDlg = new CDialogConfiguration_N572;
            Config = (unsigned int*)(&(((CDialogConfiguration_N572*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_AU9100:
            pConfigDlg = new CDialogConfiguration_AU9100;
            Config = (unsigned int*)(&(((CDialogConfiguration_AU9100*)pConfigDlg)->m_ConfigValue));
            break;

        case IDD_DIALOG_CONFIGURATION_NM1120:
            pConfigDlg = new CDialogConfiguration_NM1120;
            Config = (unsigned int*)(&(((CDialogConfiguration_NM1120*)pConfigDlg)->m_ConfigValue));
            break;

		case IDD_DIALOG_CONFIGURATION_N76E1T:
			pConfigDlg = new CDialogConfiguration_N76E1T(psChipData->uID);
            Config = (unsigned int*)(&(((CDialogConfiguration_N76E1T*)pConfigDlg)->m_ConfigValue));
			break;

		case 0x505:	// "NUC505";
		printf("NUC505 ");
        default:
			printf("or Unknow Configuration Dialog %X\n", psChipData->uProjectCode);
            return false;

        }

    }

	// Pass User Config to Configuration Dialog
	memcpy(Config, pConfig, size);
	
    if(pConfigDlg != NULL) {
        if(pConfigDlg->DoModal() == IDOK) {
			// Update User Config from Configuration Dialog
            memcpy(pConfig, Config, size);
			ret = true;
        }

        delete pConfigDlg;
    }

	return ret;
}

void CDialogResize::Test()
{
    CDialogConfiguration_NUC1xx *pDlg = NULL;
    unsigned int *Config;
	pDlg = new CDialogConfiguration_NUC1xx;	// "NUC100BN";
	Config = (unsigned int*)(&(((CDialogConfiguration_NUC1xx*)pDlg)->m_ConfigValue));
	//Config[0] = 0x12345678;	// Crash
	Config[0] = 0x00000000;
	Config[1] = 0x87654321;
	
	//GetConfigWarning

	CString str = pDlg->GetConfigWarning(pDlg->m_ConfigValue);
	wprintf(str);
	if(pDlg->DoModal() == IDOK)
	{
		printf("CONFIG01 = %8X, %8X\n", Config[0], Config[1]);
	}
}

/* called by DlgNuvoISP */
bool CDialogResize::ConfigSetting(unsigned int id, unsigned int *pConfig, unsigned int size)
{
	if(QueryDataBase(id))
	{
		return ConfigDlgSel(pConfig, size);
	}

	return false;
}

BOOL CDialogResize::OnInitDialog() 
 { 
	CDialog::OnInitDialog();
	m_bIsInitialized = true;
	GetWindowRect(m_rect);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
};
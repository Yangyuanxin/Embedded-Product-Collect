// DialogConfiguration_N76E1T.cpp : implementation file
//

#include "stdafx.h"
#include <deque>
#include <string>
#include <utility>
#include "Lang.h"
#include "ChipDefs.h"
#include "NumEdit.h"
#include "AppConfig.h"
#include "DialogConfiguration_N76E1T.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* CONFIG 0 */
#define N76E1T_CONFIG_LOCK		0x02
#define N76E1T_CONFIG_RPD		0x04
#define N76E1T_CONFIG_OCDEN		0x10
#define N76E1T_CONFIG_OCDPWM	0x20
#define N76E1T_CONFIG_CBS		0x80

/* CONFIG 1 */
#define N76E1T_CONFIG_LDSIZE	0x07
#define N76E1T_CONFIG_LDSIZE_0K	0x07
#define N76E1T_CONFIG_LDSIZE_1K	0x06
#define N76E1T_CONFIG_LDSIZE_2K	0x05
#define N76E1T_CONFIG_LDSIZE_3K	0x04
#define N76E1T_CONFIG_LDSIZE_4K	0x03

/* CONFIG 2 */
#define N76E1T_CONFIG_CBORST	0x04
#define N76E1T_CONFIG_BOIAP		0x08
#define N76E1T_CONFIG_CBOV		0x70
#define N76E1T_CONFIG_CBOV_7	0x70
#define N76E1T_CONFIG_CBOV_6	0x60
#define N76E1T_CONFIG_CBOV_5	0x50
#define N76E1T_CONFIG_CBOV_4	0x40
#define N76E1T_CONFIG_CBOV_3	0x30
#define N76E1T_CONFIG_CBOV_2	0x20
#define N76E1T_CONFIG_CBOV_1	0x10
#define N76E1T_CONFIG_CBOV_0	0x00
#define N76E1T_CONFIG_CBODEN	0x80

/* CONFIG 3 */
//XTGS[1:0]

/* CONFIG 4 */
#define N76E1T_CONFIG_WDT		0xF0
#define N76E1T_CONFIG_WDT_DIS	0xF0
#define N76E1T_CONFIG_WDT_STOP	0x50
#define N76E1T_CONFIG_WDT_RUN	0x70


/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_N76E1T dialog


CDialogConfiguration_N76E1T::CDialogConfiguration_N76E1T(unsigned int uDID,
														 CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_N76E1T::IDD, pParent)
	, m_uDID(uDID)
{
	//{{AFX_DATA_INIT(CDialogConfiguration_N76E1T)
	m_nRadio_RPD	= -1;
	m_nRadio_OCDPWM	= -1;
	m_nRadio_CBS	= -1;
	m_nRadio_LDSIZE	= -1;
	m_nRadio_CBOV	= -1;
	m_nRadio_WDTEN	= -1;

	m_sConfigValue0 = _T("");
	m_sConfigValue1 = _T("");
	m_sConfigValue2 = _T("");
	m_sConfigValue3 = _T("");
	m_sConfigValue4 = _T("");

	m_bSecurityLock = FALSE;
	m_bOCDEnable = FALSE;
	m_bCheckBrownOutEnable = TRUE;
	m_bCheckBrownOutReset = TRUE;
	m_bCheckBrownOutIAP = TRUE;

	//}}AFX_DATA_INIT
}


void CDialogConfiguration_N76E1T::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfiguration_N76E1T)
	DDX_Radio(pDX, IDC_RADIO_RPD_RESET, m_nRadio_RPD);
	DDX_Radio(pDX, IDC_RADIO_OCDPWM_TRI, m_nRadio_OCDPWM);
	DDX_Radio(pDX, IDC_RADIO_BS_APROM, m_nRadio_CBS);
	DDX_Radio(pDX, IDC_RADIO_LDSIZE_0K, m_nRadio_LDSIZE);
	DDX_Radio(pDX, IDC_RADIO_BOV_17, m_nRadio_CBOV);
	DDX_Radio(pDX, IDC_RADIO_WDT_DISABLE, m_nRadio_WDTEN);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_0, m_sConfigValue0);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_1, m_sConfigValue1);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_2, m_sConfigValue2);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_3, m_sConfigValue3);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_4, m_sConfigValue4);
	DDX_Check(pDX, IDC_CHECK_SECURITY_LOCK, m_bSecurityLock);
	DDX_Check(pDX, IDC_CHECK_OCD_ENABLE, m_bOCDEnable);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_ENABLE, m_bCheckBrownOutEnable);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_IAP, m_bCheckBrownOutIAP);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_RESET, m_bCheckBrownOutReset);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogConfiguration_N76E1T, CDialog)
	//{{AFX_MSG_MAP(CDialogConfiguration_N76E1T)
	ON_BN_CLICKED(IDC_RADIO_RPD_RESET, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_RPD_INPUT, OnRadioClick)

	ON_BN_CLICKED(IDC_RADIO_OCDPWM_TRI, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_OCDPWM_CONTI, OnRadioClick)

	ON_BN_CLICKED(IDC_RADIO_BS_APROM, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM, OnRadioClick)

	ON_BN_CLICKED(IDC_RADIO_LDSIZE_0K, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_LDSIZE_1K, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_LDSIZE_2K, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_LDSIZE_3K, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_LDSIZE_4K, OnRadioClick)

	ON_BN_CLICKED(IDC_RADIO_BOV_17, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_BOV_20, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_BOV_22, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_BOV_24, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_BOV_27, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_BOV_30, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_BOV_37, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_BOV_43, OnRadioClick)

	ON_BN_CLICKED(IDC_RADIO_WDT_DISABLE, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_WDT_ENABLE_STOP, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_WDT_ENABLE_KEEP, OnRadioClick)

	ON_BN_CLICKED(IDC_CHECK_SECURITY_LOCK, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_OCD_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_IAP, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_RESET, OnCheckClick)

	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_N76E1T message handlers

BOOL CDialogConfiguration_N76E1T::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	//UDACCEL pAccel[1];
	//pAccel[0].nInc = 1;
	//pAccel[0].nSec = 0;
	//m_SpinDataFlashSize.SetAccel(1, pAccel);

	unsigned int uDID = m_uDID & 0xFF00;

	switch (uDID)
	{
		case 0x2100:
			GetDlgItem(IDC_GROUP_RPD)->SetWindowText(_I(IDS_RPD_P12));
			GetDlgItem(IDC_RADIO_RPD_RESET)->SetWindowText(_I(IDS_RPD_P12_RESET));
			GetDlgItem(IDC_RADIO_RPD_INPUT)->SetWindowText(_I(IDS_RPD_P12_INPUT));
			break;
		case 0x2F00:
			GetDlgItem(IDC_GROUP_RPD)->SetWindowText(_I(IDS_RPD_P36));
			GetDlgItem(IDC_RADIO_RPD_RESET)->SetWindowText(_I(IDS_RPD_P36_RESET));
			GetDlgItem(IDC_RADIO_RPD_INPUT)->SetWindowText(_I(IDS_RPD_P36_INPUT));
			break;
		case 0x3600:
			GetDlgItem(IDC_GROUP_RPD)->SetWindowText(_I(IDS_RPD_P20));
			GetDlgItem(IDC_RADIO_RPD_RESET)->SetWindowText(_I(IDS_RPD_P20_RESET));
			GetDlgItem(IDC_RADIO_RPD_INPUT)->SetWindowText(_I(IDS_RPD_P20_INPUT));
			break;
	}

	if (uDID != 0x2100)
	{
		GetDlgItem(IDC_RADIO_BOV_17)->SetWindowText(_T("2.2V"));
		GetDlgItem(IDC_RADIO_BOV_20)->SetWindowText(_T("2.7V"));
		GetDlgItem(IDC_RADIO_BOV_22)->SetWindowText(_T("3.8V"));
		GetDlgItem(IDC_RADIO_BOV_24)->SetWindowText(_T("4.3V"));

		GetDlgItem(IDC_RADIO_BOV_27)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_BOV_30)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_BOV_37)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_BOV_43)->ShowWindow(SW_HIDE);
	}

	if (uDID == 0x2F00)
	{
		GetDlgItem(IDC_RADIO_OCDPWM_TRI)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OCDPWM_CONTI)->EnableWindow(FALSE);

		GetDlgItem(IDC_GROUP_OCDPWM)->SetWindowText(_T("None"));
		GetDlgItem(IDC_RADIO_OCDPWM_TRI)->SetWindowText(_T("None"));
		GetDlgItem(IDC_RADIO_OCDPWM_CONTI)->SetWindowText(_T("None"));
	}

	if ((m_uDID & 0xFF) == 0x40)
	{
		GetDlgItem(IDC_RADIO_LDSIZE_0K)->SetWindowText(_I(IDS_LD_0K_DF_10K));
		GetDlgItem(IDC_RADIO_LDSIZE_1K)->SetWindowText(_I(IDS_LD_1K_DF_9K));
		GetDlgItem(IDC_RADIO_LDSIZE_2K)->SetWindowText(_I(IDS_LD_2K_DF_8K));
		GetDlgItem(IDC_RADIO_LDSIZE_3K)->SetWindowText(_I(IDS_LD_3K_DF_7K));
		GetDlgItem(IDC_RADIO_LDSIZE_4K)->SetWindowText(_I(IDS_LD_4K_DF_6K));
	}

	ConfigToGUI();

	UpdateData(FALSE);

	m_bIsInitialized = true;
	GetWindowRect(m_rect);
	AdjustDPI();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogConfiguration_N76E1T::ConfigToGUI()
{
	unsigned char ucConfig0 = _GET_BYTE0(m_ConfigValue.m_value[0]);
	unsigned char ucConfig1 = _GET_BYTE1(m_ConfigValue.m_value[0]);
	unsigned char ucConfig2 = _GET_BYTE2(m_ConfigValue.m_value[0]);
	unsigned char ucConfig3 = _GET_BYTE3(m_ConfigValue.m_value[0]);
	unsigned char ucConfig4 = _GET_BYTE0(m_ConfigValue.m_value[1]);

	m_bSecurityLock = ((ucConfig0 & N76E1T_CONFIG_LOCK) == 0 ? TRUE : FALSE);

	m_nRadio_RPD = ((ucConfig0 & N76E1T_CONFIG_RPD) == 0 ? 1 : 0);

	m_bOCDEnable = ((ucConfig0 & N76E1T_CONFIG_OCDEN) == 0 ? TRUE : FALSE);

	m_nRadio_OCDPWM = ((ucConfig0 & N76E1T_CONFIG_OCDPWM) == 0 ? 1 : 0);

	m_nRadio_CBS = ((ucConfig0 & N76E1T_CONFIG_CBS) == 0 ? 1 : 0);

	switch(ucConfig1 & N76E1T_CONFIG_LDSIZE)
	{
		case N76E1T_CONFIG_LDSIZE_0K:
			m_nRadio_LDSIZE = 0;
			break;
		case N76E1T_CONFIG_LDSIZE_1K:
			m_nRadio_LDSIZE = 1;
			break;
		case N76E1T_CONFIG_LDSIZE_2K:
			m_nRadio_LDSIZE = 2;
			break;
		case N76E1T_CONFIG_LDSIZE_3K:
			m_nRadio_LDSIZE = 3;
			break;
		default:
			m_nRadio_LDSIZE = 4;
			break;
	}

	if ((m_uDID & 0xFF00) != 0x2100)
	{
		ucConfig2 |= 0x40;
	}

	m_bCheckBrownOutReset = ((ucConfig2 & N76E1T_CONFIG_CBORST) != 0 ? TRUE : FALSE);
	m_bCheckBrownOutIAP = ((ucConfig2 & N76E1T_CONFIG_BOIAP) != 0 ? TRUE : FALSE);

	switch(ucConfig2 & N76E1T_CONFIG_CBOV)
	{
		case N76E1T_CONFIG_CBOV_7:
			m_nRadio_CBOV = 0;
			break;
		case N76E1T_CONFIG_CBOV_6:
			m_nRadio_CBOV = 1;
			break;
		case N76E1T_CONFIG_CBOV_5:
			m_nRadio_CBOV = 2;
			break;
		case N76E1T_CONFIG_CBOV_4:
			m_nRadio_CBOV = 3;
			break;
		case N76E1T_CONFIG_CBOV_3:
			m_nRadio_CBOV = 4;
			break;
		case N76E1T_CONFIG_CBOV_2:
			m_nRadio_CBOV = 5;
			break;
		case N76E1T_CONFIG_CBOV_1:
			m_nRadio_CBOV = 6;
			break;
		default:
			m_nRadio_CBOV = 7;
			break;
	}

	m_bCheckBrownOutEnable = ((ucConfig2 & N76E1T_CONFIG_CBODEN) != 0 ? TRUE : FALSE);

	switch(ucConfig4 & N76E1T_CONFIG_WDT)
	{
		case N76E1T_CONFIG_WDT_DIS:
			m_nRadio_WDTEN = 0;
			break;
		case N76E1T_CONFIG_WDT_STOP:
			m_nRadio_WDTEN = 1;
			break;
		default:
			m_nRadio_WDTEN = 2;
			break;
	}

	m_sConfigValue0.Format(_T("0x%02X"), ucConfig0);
	m_sConfigValue1.Format(_T("0x%02X"), ucConfig1);
	m_sConfigValue2.Format(_T("0x%02X"), ucConfig2);
	m_sConfigValue3.Format(_T("0x%02X"), ucConfig3);
	m_sConfigValue4.Format(_T("0x%02X"), ucConfig4);
}

void CDialogConfiguration_N76E1T::GUIToConfig()
{
	unsigned char ucConfig0 = 0xFF;
	unsigned char ucConfig1 = 0xFF;
	unsigned char ucConfig2 = 0xFF;
	unsigned char ucConfig3 = 0xFF;
	unsigned char ucConfig4 = 0xFF;

	/* CONFIG 0 */
	if (m_bSecurityLock)
		ucConfig0 &= ~N76E1T_CONFIG_LOCK;

	if (m_nRadio_RPD)
		ucConfig0 &= ~N76E1T_CONFIG_RPD;

	if (m_bOCDEnable)
		ucConfig0 &= ~N76E1T_CONFIG_OCDEN;

	if (m_nRadio_OCDPWM)
		ucConfig0 &= ~N76E1T_CONFIG_OCDPWM;

	if (m_nRadio_CBS)
		ucConfig0 &= ~N76E1T_CONFIG_CBS;

	/* CONFIG 1 */
	switch(m_nRadio_LDSIZE)
	{
		case 0:
			ucConfig1 = N76E1T_CONFIG_LDSIZE_0K;
			break;
		case 1:
			ucConfig1 = N76E1T_CONFIG_LDSIZE_1K;
			break;
		case 2:
			ucConfig1 = N76E1T_CONFIG_LDSIZE_2K;
			break;
		case 3:
			ucConfig1 = N76E1T_CONFIG_LDSIZE_3K;
			break;
		default:
			ucConfig1 = N76E1T_CONFIG_LDSIZE_4K;
			break;
	}

	ucConfig1 |= 0xF8;

	/* CONFIG 2 */
	switch(m_nRadio_CBOV)
	{
		case 0:
			ucConfig2 = N76E1T_CONFIG_CBOV_7;
			break;
		case 1:
			ucConfig2 = N76E1T_CONFIG_CBOV_6;
			break;
		case 2:
			ucConfig2 = N76E1T_CONFIG_CBOV_5;
			break;
		case 3:
			ucConfig2 = N76E1T_CONFIG_CBOV_4;
			break;
		case 4:
			ucConfig2 = N76E1T_CONFIG_CBOV_3;
			break;
		case 5:
			ucConfig2 = N76E1T_CONFIG_CBOV_2;
			break;
		case 6:
			ucConfig2 = N76E1T_CONFIG_CBOV_1;
			break;
		default:
			ucConfig2 = N76E1T_CONFIG_CBOV_0;
			break;
	}

	ucConfig2 |= 0x8F;

	if (!m_bCheckBrownOutReset)
		ucConfig2 &= ~N76E1T_CONFIG_CBORST;

	if (!m_bCheckBrownOutIAP)
		ucConfig2 &= ~N76E1T_CONFIG_BOIAP;

	if (!m_bCheckBrownOutEnable)
		ucConfig2 &= ~N76E1T_CONFIG_CBODEN;

	/* CONFIG 4 */
	switch(m_nRadio_WDTEN)
	{
		case 0:
			ucConfig4 = N76E1T_CONFIG_WDT_DIS;
			break;
		case 1:
			ucConfig4 = N76E1T_CONFIG_WDT_STOP;
			break;
		default:
			ucConfig4 = N76E1T_CONFIG_WDT_RUN;
			break;
	}

	ucConfig4 |= 0x0F;

	m_ConfigValue.m_value[0] = (ucConfig3 << 24) + (ucConfig2 << 16) + (ucConfig1 << 8) + ucConfig0;
	m_ConfigValue.m_value[1] = 0xFFFFFF00 | ucConfig4;
}

void CDialogConfiguration_N76E1T::OnRadioClick() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig();
	ConfigToGUI();

	UpdateData(FALSE);
}

void CDialogConfiguration_N76E1T::OnCheckClick() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig();
	ConfigToGUI();

	UpdateData(FALSE);
}

void CDialogConfiguration_N76E1T::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	GUIToConfig();

	CDialog::OnOK();
}

CString CDialogConfiguration_N76E1T::GetConfigWarning(const CAppConfig::N76E1T_configs_t &config)
{
	CString str;
	unsigned char ucConfig0 = _GET_BYTE0(config.m_value[0]);

	if (ucConfig0 & N76E1T_CONFIG_LOCK)
		str += _T("   ") + _I(IDS_DISABLE_SECURITY_LOCK);

	return str;
}

void CDialogConfiguration_N76E1T::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//if (pScrollBar != NULL && pScrollBar->GetDlgCtrlID() == m_SpinDataFlashSize.GetDlgCtrlID())
	//	return;

	CDialogResize2::OnVScroll(nSBCode, nPos, pScrollBar);
}


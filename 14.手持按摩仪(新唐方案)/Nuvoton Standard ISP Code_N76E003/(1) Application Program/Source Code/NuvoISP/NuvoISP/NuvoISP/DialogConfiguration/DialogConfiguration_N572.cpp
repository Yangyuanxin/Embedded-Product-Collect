// DialogConfiguration.cpp : implementation file
//

#include "stdafx.h"
#include <deque>
#include <string>
#include <utility>
#include "Lang.h"
#include "ChipDefs.h"
#include "NumEdit.h"
#include "AppConfig.h"
#include "DialogConfiguration_N572.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_N572 dialog


CDialogConfiguration_N572::CDialogConfiguration_N572(CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_N572::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfiguration_N572)
	m_nRadioClk = -1;
	//}}AFX_DATA_INIT
}


void CDialogConfiguration_N572::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfiguration_N572)
	DDX_Control(pDX, IDC_EDIT_FLASH_INFO4, m_FlashInfo4);
	DDX_Control(pDX, IDC_EDIT_FLASH_INFO2, m_FlashInfo2);
	DDX_Control(pDX, IDC_EDIT_FLASH_INFO3, m_FlashInfo3);
	DDX_Control(pDX, IDC_EDIT_FLASH_INFO1, m_FlashInfo1);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_0, m_sConfigValue0);
	DDX_Check(pDX, IDC_CHECK_CLOCK_FILTER_ENABLE, m_bClockFilterEnable);
	DDX_Check(pDX, IDC_CHECK_SECURITY_LOCK, m_bSecurityLock);
	DDX_Check(pDX, IDC_CHECK_WATCHDOG_ENABLE, m_bWatchDogEnable);
	DDX_Check(pDX, IDC_CHECK_CVDEN, m_bCVDEN);
	DDX_Radio(pDX, IDC_RADIO_CLK_E12M6M, m_nRadioClk);
	DDX_Radio(pDX, IDC_RADIO_CVDTV_27, m_nRadioCVDTV);
	DDX_Text(pDX, IDC_EDIT_FLASH_INFO1, m_sFlashInfo1);
	DDX_Text(pDX, IDC_EDIT_FLASH_INFO2, m_sFlashInfo2);
	DDX_Text(pDX, IDC_EDIT_FLASH_INFO3, m_sFlashInfo3);
	DDX_Text(pDX, IDC_EDIT_FLASH_INFO4, m_sFlashInfo4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfiguration_N572, CDialog)
	//{{AFX_MSG_MAP(CDialogConfiguration_N572)
	ON_BN_CLICKED(IDC_RADIO_CLK_E12M6M, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_CLK_I24M, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_CVDTV_27, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_CVDTV_30, OnRadioClick)
	ON_BN_CLICKED(IDC_CHECK_WATCHDOG_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_CLOCK_FILTER_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_CVDEN, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_SECURITY_LOCK, OnCheckClick)
	ON_EN_CHANGE(IDC_EDIT_FLASH_INFO1, OnChangeEditFlashInfo1)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_N572 message handlers

BOOL CDialogConfiguration_N572::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ConfigToGUI();

	UpdateData(FALSE);

	m_bIsInitialized = true;
	GetWindowRect(m_rect);
	AdjustDPI();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CDialogConfiguration_N572::ConfigToGUI()
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];

	switch(uConfig0 & N572_FLASH_CONFIG_CFOSC)
	{
	case N572_FLASH_CONFIG_E12M6M:
		m_nRadioClk = 0; break;
	case N572_FLASH_CONFIG_I24M:
		m_nRadioClk = 1; break;
	default:
		m_nRadioClk = 1;
	}

	if((uConfig0 & N572_FLASH_CONFIG_CVDTV) == 0)
		m_nRadioCVDTV = 0;
	else
		m_nRadioCVDTV = 1;

	m_bWatchDogEnable = ((uConfig0 & N572_FLASH_CONFIG_CWDTEN) == 0 ? FALSE : TRUE);
	m_bClockFilterEnable = ((uConfig0 & N572_FLASH_CONFIG_CKF) == 0 ? FALSE : TRUE);
	m_bCVDEN = ((uConfig0 & N572_FLASH_CONFIG_CVDEN) == 0 ? TRUE : FALSE);
	m_bSecurityLock = ((uConfig0 & N572_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);

	m_sConfigValue0.Format(_T("0x%08X"), m_ConfigValue.m_value[0]);
	m_sFlashInfo1.Format(_T("%08X"), m_ConfigValue.m_value[1]);
	m_sFlashInfo2.Format(_T("%08X"), m_ConfigValue.m_value[2]);
	m_sFlashInfo3.Format(_T("%08X"), m_ConfigValue.m_value[3]);
	m_sFlashInfo4.Format(_T("%08X"), m_ConfigValue.m_value[4]);
}

void CDialogConfiguration_N572::GUIToConfig()
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];

	uConfig0 &= ~N572_FLASH_CONFIG_CFOSC;
	switch(m_nRadioClk)
	{
	case 0:
		uConfig0 |= N572_FLASH_CONFIG_E12M6M; break;
	case 1:
		uConfig0 |= N572_FLASH_CONFIG_I24M; break;
	default:
		uConfig0 |= N572_FLASH_CONFIG_I24M;
	}

	if(m_nRadioCVDTV == 0)
		uConfig0 &= ~N572_FLASH_CONFIG_CVDTV;
	else
		uConfig0 |= N572_FLASH_CONFIG_CVDTV;

	if(m_bWatchDogEnable)
		uConfig0 |= N572_FLASH_CONFIG_CWDTEN;
	else
		uConfig0 &= ~N572_FLASH_CONFIG_CWDTEN;

	if(m_bClockFilterEnable)
		uConfig0 |= N572_FLASH_CONFIG_CKF;
	else
		uConfig0 &= ~N572_FLASH_CONFIG_CKF;

	if(m_bCVDEN)
		uConfig0 &= ~N572_FLASH_CONFIG_CVDEN;
	else
		uConfig0 |= N572_FLASH_CONFIG_CVDEN;

	if(m_bSecurityLock)
		uConfig0 &= ~N572_FLASH_CONFIG_LOCK;
	else
		uConfig0 |= N572_FLASH_CONFIG_LOCK;

	m_ConfigValue.m_value[0] = uConfig0;
	
	TCHAR *pEnd;
	m_ConfigValue.m_value[1] = ::_tcstoul(m_sFlashInfo1, &pEnd, 16);
	m_ConfigValue.m_value[2] = ::_tcstoul(m_sFlashInfo2, &pEnd, 16);
	m_ConfigValue.m_value[3] = ::_tcstoul(m_sFlashInfo3, &pEnd, 16);
	m_ConfigValue.m_value[4] = ::_tcstoul(m_sFlashInfo4, &pEnd, 16);
}


void CDialogConfiguration_N572::OnRadioClick() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig();
	ConfigToGUI();

	UpdateData(FALSE);
}

void CDialogConfiguration_N572::OnCheckClick() 
{
	// TODO: Add your control notification handler code here
	OnRadioClick();
}

void CDialogConfiguration_N572::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	
	GUIToConfig();
	
	CDialog::OnOK();
}


CString CDialogConfiguration_N572::GetConfigWarning(const CAppConfig::N572_configs_t &config)
{
	CString str;
	unsigned int uConfig0 = config.m_value[0];

	BOOL bSecurityLock = ((uConfig0 & N572_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);
	if(!bSecurityLock)
		str += _T("   ") + _I(IDS_DISABLE_SECURITY_LOCK);

	return str;
}


void CDialogConfiguration_N572::OnChangeEditFlashInfo1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);

//	TCHAR *pEnd;
//	unsigned int uConfig1 = ::_tcstoul(m_sFlashInfo1, &pEnd, 16);
//	m_sConfigValue1.Format(_T("0x%08X"), uConfig1);

//	UpdateData(FALSE);	
}

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
#include "DialogConfiguration_M058.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_M058 dialog


CDialogConfiguration_M058::CDialogConfiguration_M058(CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_M058::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfiguration_M058)
	m_nRadioClk = -1;
	m_nRadioBov = -1;
	m_nRadioBS = -1;
	m_sConfigValue0 = _T("");
	m_sConfigValue1 = _T("");
	m_bCheckBrownOutDetect = FALSE;
	m_bCheckBrownOutReset = FALSE;
	m_bClockFilterEnable = FALSE;
	m_bDataFlashEnable = FALSE;
	m_bSecurityLock = FALSE;
	m_sFlashBaseAddress = _T("");
	m_nRadioGP7 = -1;
	m_nRadioIO = -1;
	m_bWDTEnable = FALSE;
	m_bWDTPowerDown = FALSE;
	//}}AFX_DATA_INIT
}


void CDialogConfiguration_M058::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfiguration_M058)
	DDX_Control(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_FlashBaseAddress);
	DDX_Radio(pDX, IDC_RADIO_CLK_E12M, m_nRadioClk);
	DDX_Radio(pDX, IDC_RADIO_BOV_45, m_nRadioBov);
	DDX_Radio(pDX, IDC_RADIO_BS_LDROM, m_nRadioBS);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_0, m_sConfigValue0);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_1, m_sConfigValue1);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_DETECT, m_bCheckBrownOutDetect);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_RESET, m_bCheckBrownOutReset);
	DDX_Check(pDX, IDC_CHECK_CLOCK_FILTER_ENABLE, m_bClockFilterEnable);
	DDX_Check(pDX, IDC_CHECK_DATA_FLASH_ENABLE, m_bDataFlashEnable);
	DDX_Check(pDX, IDC_CHECK_SECURITY_LOCK, m_bSecurityLock);
	DDX_Text(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_sFlashBaseAddress);
	DDX_Radio(pDX, IDC_RADIO_GPF_GPIO, m_nRadioGP7);
	DDX_Radio(pDX, IDC_RADIO_IO_TRI, m_nRadioIO);
	DDX_Check(pDX, IDC_CHECK_WDT_ENABLE, m_bWDTEnable);
	DDX_Check(pDX, IDC_CHECK_WDT_POWER_DOWN, m_bWDTPowerDown);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfiguration_M058, CDialog)
	//{{AFX_MSG_MAP(CDialogConfiguration_M058)
	ON_BN_CLICKED(IDC_RADIO_BOV_45, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_CLK_E12M, OnRadioClk)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM, OnRadioBs)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_DETECT, OnCheckClick)
	ON_EN_CHANGE(IDC_EDIT_FLASH_BASE_ADDRESS, OnChangeEditFlashBaseAddress)
	ON_BN_CLICKED(IDC_CHECK_WDT_ENABLE, OnCheckClickWDT)
	ON_BN_CLICKED(IDC_CHECK_WDT_POWER_DOWN, OnCheckClickWDTPD)
	ON_BN_CLICKED(IDC_RADIO_BOV_38, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_27, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_22, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_CLK_E32K, OnRadioClk)
	ON_BN_CLICKED(IDC_RADIO_CLK_PLL, OnRadioClk)
	ON_BN_CLICKED(IDC_RADIO_CLK_I10K, OnRadioClk)
	ON_BN_CLICKED(IDC_RADIO_CLK_I22M, OnRadioClk)
	ON_BN_CLICKED(IDC_RADIO_BS_APROM, OnRadioBs)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_RESET, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_CLOCK_FILTER_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_DATA_FLASH_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_SECURITY_LOCK, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_WATCHDOG_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_RADIO_GPF_GPIO, OnRadioGp7)
	ON_BN_CLICKED(IDC_RADIO_GPF_CRYSTAL, OnRadioGp7)
	ON_BN_CLICKED(IDC_RADIO_IO_TRI, OnRadioIO)
	ON_BN_CLICKED(IDC_RADIO_IO_BI, OnRadioIO)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM_APROM, OnRadioBs)
	ON_BN_CLICKED(IDC_RADIO_BS_APROM_LDROM, OnRadioBs)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_M058 message handlers

BOOL CDialogConfiguration_M058::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	ConfigToGUI(0);

	UpdateData(FALSE);

	m_bIsInitialized = true;
	GetWindowRect(m_rect);
	AdjustDPI();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CDialogConfiguration_M058::ConfigToGUI(int nEventID)
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];

	switch(uConfig0 & M05X_FLASH_CONFIG_CFOSC)
	{
	case M05X_FLASH_CONFIG_E12M:
		m_nRadioClk = 0; break;
	case M05X_FLASH_CONFIG_E32K:
		m_nRadioClk = 1; break;
	case M05X_FLASH_CONFIG_PLL:
		m_nRadioClk = 2; break;
	case M05X_FLASH_CONFIG_I10K:
		m_nRadioClk = 3; break;
	case M05X_FLASH_CONFIG_I22M:
	default:
		m_nRadioClk = 4; break;
	}

	switch(uConfig0 & M05X_FLASH_CONFIG_CBOV)
	{
	case M05X_FLASH_CONFIG_CBOV_45:
		m_nRadioBov = 0; break;
	case M05X_FLASH_CONFIG_CBOV_38:
		m_nRadioBov = 1; break;
	case M05X_FLASH_CONFIG_CBOV_26:
		m_nRadioBov = 2; break;
	case M05X_FLASH_CONFIG_CBOV_22:
	default:
		m_nRadioBov = 3; break;
	}

	//m_nRadioBS = ((uConfig0 & M05X_FLASH_CONFIG_CBS) == 0 ? 0 : 1);
	switch(uConfig0 & M05X_FLASH_CONFIG_CBS2)
	{
	case M05X_FLASH_CONFIG_CBS_LD:
		m_nRadioBS = 0; break;
	case M05X_FLASH_CONFIG_CBS_AP:
		m_nRadioBS = 1; break;
	case M05X_FLASH_CONFIG_CBS_LD_AP:
		m_nRadioBS = 2; break;
	case M05X_FLASH_CONFIG_CBS_AP_LD:
	default:
		m_nRadioBS = 3; break;
	}

	m_nRadioGP7 = ((uConfig0 & M05X_FLASH_CONFIG_CGP7MFP) == 0 ? 0 : 1);
	m_nRadioIO = ((uConfig0 & M05X_FLASH_CONFIG_CIOINI) == 0 ? 0 : 1);

	m_bWDTPowerDown = ((uConfig0 & M05X_FLASH_CONFIG_CWDTPDEN) == 0 ? TRUE : FALSE);
	m_bWDTEnable = ((uConfig0 & M05X_FLASH_CONFIG_CWDTEN) == 0 ? TRUE : FALSE);;
	if(!m_bWDTEnable) m_bWDTPowerDown = FALSE;

	m_bCheckBrownOutDetect = ((uConfig0 & M05X_FLASH_CONFIG_CBODEN) == 0 ? TRUE : FALSE);
	m_bCheckBrownOutReset = ((uConfig0 & M05X_FLASH_CONFIG_CBORST) == 0 ? TRUE : FALSE);
	m_bClockFilterEnable = ((uConfig0 & M05X_FLASH_CONFIG_CKF) == M05X_FLASH_CONFIG_CKF ? TRUE : FALSE);
	m_bDataFlashEnable = ((uConfig0 & M05X_FLASH_CONFIG_DFEN) == 0 ? TRUE : FALSE);
	m_bSecurityLock = ((uConfig0 & M05X_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);

	m_sFlashBaseAddress.Format(_T("%X"), m_ConfigValue.m_value[1]);

	m_sConfigValue0.Format(_T("0x%08X"), m_ConfigValue.m_value[0]);
	m_sConfigValue1.Format(_T("0x%08X"), m_ConfigValue.m_value[1]);
}

void CDialogConfiguration_M058::GUIToConfig(int nEventID)
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];

	uConfig0 &= ~M05X_FLASH_CONFIG_CFOSC;
	switch(m_nRadioClk)
	{
	case 0:
		uConfig0 |= M05X_FLASH_CONFIG_E12M; break;
	case 1:
		uConfig0 |= M05X_FLASH_CONFIG_E32K; break;
	case 2:
		uConfig0 |= M05X_FLASH_CONFIG_PLL; break;
	case 3:
		uConfig0 |= M05X_FLASH_CONFIG_I10K; break;
	case 4:
		uConfig0 |= M05X_FLASH_CONFIG_CFOSC;	/* New spec! */ 
		break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & M05X_FLASH_CONFIG_CFOSC);
	}

	uConfig0 &= ~M05X_FLASH_CONFIG_CBOV;
	switch(m_nRadioBov)
	{
	case 0:
		uConfig0 |= M05X_FLASH_CONFIG_CBOV_45; break;
	case 1:
		uConfig0 |= M05X_FLASH_CONFIG_CBOV_38; break;
	case 2:
		uConfig0 |= M05X_FLASH_CONFIG_CBOV_26; break;
	case 3:
		uConfig0 |= M05X_FLASH_CONFIG_CBOV_22; break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & M05X_FLASH_CONFIG_CBOV);
	}

	//if(m_nRadioBS == 0)
	//	uConfig0 &= ~M05X_FLASH_CONFIG_CBS;
	//else
	//	uConfig0 |= M05X_FLASH_CONFIG_CBS;

	uConfig0 &= ~M05X_FLASH_CONFIG_CBS2;
	switch(m_nRadioBS)
	{
	case 0:
		uConfig0 |= M05X_FLASH_CONFIG_CBS_LD; break;
	case 1:
		uConfig0 |= M05X_FLASH_CONFIG_CBS_AP; break;
	case 2:
		uConfig0 |= M05X_FLASH_CONFIG_CBS_LD_AP; break;
	case 3:
		uConfig0 |= M05X_FLASH_CONFIG_CBS_AP_LD; break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & M05X_FLASH_CONFIG_CBS2);
	}

	if(m_nRadioGP7 == 0)
		uConfig0 &= ~M05X_FLASH_CONFIG_CGP7MFP;
	else
		uConfig0 |= M05X_FLASH_CONFIG_CGP7MFP;


	if(m_nRadioIO == 0)
		uConfig0 &= ~M05X_FLASH_CONFIG_CIOINI;
	else
		uConfig0 |= M05X_FLASH_CONFIG_CIOINI;

	if(m_bWDTPowerDown)
		uConfig0 &= ~M05X_FLASH_CONFIG_CWDTPDEN;
	else
		uConfig0 |= M05X_FLASH_CONFIG_CWDTPDEN;
	if(m_bWDTEnable)
		uConfig0 &= ~M05X_FLASH_CONFIG_CWDTEN;
	else
		uConfig0 |= M05X_FLASH_CONFIG_CWDTEN;
	if(nEventID == IDC_CHECK_WDT_POWER_DOWN)
	{
		if(m_bWDTPowerDown)
			uConfig0 &= ~M05X_FLASH_CONFIG_CWDTEN;
	}
	else
	{
		if(!m_bWDTEnable)
			uConfig0 |= M05X_FLASH_CONFIG_CWDTPDEN;
	}

	if(m_bCheckBrownOutDetect)
		uConfig0 &= ~M05X_FLASH_CONFIG_CBODEN;
	else
		uConfig0 |= M05X_FLASH_CONFIG_CBODEN;

	if(m_bCheckBrownOutReset)
		uConfig0 &= ~M05X_FLASH_CONFIG_CBORST;
	else
		uConfig0 |= M05X_FLASH_CONFIG_CBORST;
	if(m_bClockFilterEnable)
		uConfig0 |= M05X_FLASH_CONFIG_CKF;
	else
		uConfig0 &= ~M05X_FLASH_CONFIG_CKF;
	if(m_bDataFlashEnable)
		uConfig0 &= ~M05X_FLASH_CONFIG_DFEN;
	else
		uConfig0 |= M05X_FLASH_CONFIG_DFEN;
	if(m_bSecurityLock)
		uConfig0 &= ~M05X_FLASH_CONFIG_LOCK;
	else
		uConfig0 |= M05X_FLASH_CONFIG_LOCK;

	m_ConfigValue.m_value[0] = uConfig0;

	TCHAR *pEnd;
	m_ConfigValue.m_value[1] = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
}



void CDialogConfiguration_M058::OnGUIEvent(int nEventID) 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig(nEventID);
	ConfigToGUI(nEventID);

	UpdateData(FALSE);
}

void CDialogConfiguration_M058::OnRadioBov() 
{
	OnGUIEvent();
}

void CDialogConfiguration_M058::OnRadioClk() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent();
}

void CDialogConfiguration_M058::OnRadioBs() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent();
}

void CDialogConfiguration_M058::OnRadioGp7() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent();
}

void CDialogConfiguration_M058::OnRadioIO() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent();
}

void CDialogConfiguration_M058::OnCheckClick() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent();
}


void CDialogConfiguration_M058::OnCheckClickWDTPD() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent(IDC_CHECK_WDT_POWER_DOWN);
}

void CDialogConfiguration_M058::OnCheckClickWDT() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent(IDC_CHECK_WDT_ENABLE);
}

void CDialogConfiguration_M058::OnChangeEditFlashBaseAddress() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	TCHAR *pEnd;
	unsigned int uConfig1 = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	m_sConfigValue1.Format(_T("0x%08X"), uConfig1);

	UpdateData(FALSE);
}

void CDialogConfiguration_M058::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	GUIToConfig(0);

	CDialog::OnOK();
}


CString CDialogConfiguration_M058::GetConfigWarning(const CAppConfig::M05x_configs_t &config)
{
	CString str;
	unsigned int uConfig0 = config.m_value[0];

	switch(uConfig0 & M05X_FLASH_CONFIG_CFOSC)
	{
	case M05X_FLASH_CONFIG_E12M:
		str += _T("   ") + _I(IDS_SELECT_EXTERNAL_12M_CLOCK);
		break;
	case M05X_FLASH_CONFIG_E32K:
		str += _T("   ") + _I(IDS_SELECT_EXTERNAL_32K_CLOCK);
		break;
	case M05X_FLASH_CONFIG_PLL:
		str += _T("   ") + _I(IDS_SELECT_PLL_CLOCK);
		break;
	default:
		;
	}

	BOOL bSecurityLock = ((uConfig0 & M05X_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);
	if(!bSecurityLock)
		str += _T("   ") + _I(IDS_DISABLE_SECURITY_LOCK);

	return str;
}

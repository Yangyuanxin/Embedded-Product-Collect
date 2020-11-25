// DialogConfiguration_MT500.cpp : 實作檔
//

#include "stdafx.h"
#include <deque>
#include <string>
#include <utility>
#include "Lang.h"
#include "ChipDefs.h"
#include "NumEdit.h"
#include "AppConfig.h"
#include "DialogConfiguration_MT500.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDialogConfiguration_MT500 對話方塊

CDialogConfiguration_MT500::CDialogConfiguration_MT500(BOOL bIsDataFlashFixed,
													   unsigned int uProgramMemorySize,
													   unsigned int uDataFlashSize,
													   CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_MT500::IDD, pParent)
	, m_bIsDataFlashFixed(bIsDataFlashFixed)
	, m_uProgramMemorySize(uProgramMemorySize)
	, m_uDataFlashSize(uDataFlashSize)
{
	//{{AFX_DATA_INIT(CDialogConfiguration_NUC1xx)
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
	m_bWDTEnable = FALSE;
	m_bWDTPowerDown = FALSE;
	m_bCHZ_Even0=TRUE;
	m_bCHZ_Even1=TRUE;
	m_BCHZ_Odd0=TRUE;
	m_BCHZ_Odd1=TRUE;
	m_BCHZ_BPWM=TRUE;
	m_sFlashBaseAddress = _T("");
	//}}AFX_DATA_INIT
}

//CDialogConfiguration_MT500::~CDialogConfiguration_MT500()
//{
//}

void CDialogConfiguration_MT500::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfiguration_NUC1xx)
	DDX_Control(pDX, IDC_CHECK_DATA_FLASH_ENABLE, m_DataFlashEnable);
	DDX_Control(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_FlashBaseAddress);
	DDX_Control(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_DataFlashSize);
	DDX_Control(pDX, IDC_SPIN_DATA_FLASH_SIZE, m_SpinDataFlashSize);
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
	DDX_Check(pDX, IDC_CHECK_WDT_ENABLE, m_bWDTEnable);
	DDX_Check(pDX, IDC_CHECK_WDT_POWER_DOWN, m_bWDTPowerDown);
	DDX_Check(pDX, IDC_CHECK_CHZ_BPWM_Ctrl, m_BCHZ_BPWM);
	DDX_Check(pDX, IDC_CHECK_CHZ_Odd1_Ctrl, m_BCHZ_Odd1);
	DDX_Check(pDX, IDC_CHECK_CHZ_Even1_Ctrl, m_bCHZ_Even1);
	DDX_Check(pDX, IDC_CHECK_CHZ_Odd0_Ctrl, m_BCHZ_Odd0);
	DDX_Check(pDX, IDC_CHECK_CHZ_Even0_Ctrl, m_bCHZ_Even0);
	DDX_Text(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_sFlashBaseAddress);
	DDX_Text(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_sDataFlashSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfiguration_MT500, CDialog)
		//{{AFX_MSG_MAP(CDialogConfiguration_MT500)
	ON_BN_CLICKED(IDC_RADIO_BOV_45, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_CLK_E12M, OnRadioClk)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM, OnRadioBs)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_DETECT, OnCheckClick)
	ON_EN_CHANGE(IDC_EDIT_FLASH_BASE_ADDRESS, OnChangeEditFlashBaseAddress)
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

	ON_BN_CLICKED(IDC_CHECK_WDT_POWER_DOWN, OnCheckClickWDTPD)
	ON_BN_CLICKED(IDC_CHECK_WDT_ENABLE, OnCheckClickWDT)

	ON_BN_CLICKED(IDC_CHECK_CHZ_BPWM_Ctrl, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_CHZ_Odd1_Ctrl, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_CHZ_Even1_Ctrl, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_CHZ_Odd0_Ctrl, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_CHZ_Even0_Ctrl, OnCheckClick)

	ON_BN_CLICKED(IDC_CHECK_WATCHDOG_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM_APROM, OnRadioBs)
	ON_BN_CLICKED(IDC_RADIO_BS_APROM_LDROM, OnRadioBs)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DATA_FLASH_SIZE, OnDeltaposSpinDataFlashSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDialogConfiguration_MT500::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UDACCEL pAccel[1];
	pAccel[0].nInc = 1;
	pAccel[0].nSec = 0;
	m_SpinDataFlashSize.SetAccel(1, pAccel);

	ConfigToGUI(0);

	UpdateData(FALSE);

	m_bIsInitialized = true;
	GetWindowRect(m_rect);
	AdjustDPI();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogConfiguration_MT500::ConfigToGUI(int nEventID)
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig1 = m_ConfigValue.m_value[1];

	switch(uConfig0 & NUC1XX_FLASH_CONFIG_CFOSC)
	{
	case NUC1XX_FLASH_CONFIG_E12M:
		m_nRadioClk = 0; break;
	case NUC1XX_FLASH_CONFIG_E32K:
		m_nRadioClk = 1; break;
	case NUC1XX_FLASH_CONFIG_PLL:
		m_nRadioClk = 2; break;
	case NUC1XX_FLASH_CONFIG_I10K:
		m_nRadioClk = 3; break;
	case NUC1XX_FLASH_CONFIG_I22M:
	default:
		m_nRadioClk = 4; break;
	}

	switch(uConfig0 & NUC1XX_FLASH_CONFIG_CBOV)
	{
	case NUC1XX_FLASH_CONFIG_CBOV_45:
		m_nRadioBov = 0; break;
	case NUC1XX_FLASH_CONFIG_CBOV_38:
		m_nRadioBov = 1; break;
	case NUC1XX_FLASH_CONFIG_CBOV_26:
		m_nRadioBov = 2; break;
	case NUC1XX_FLASH_CONFIG_CBOV_22:
	default:
		m_nRadioBov = 3; break;
	}

	//m_nRadioBS = ((uConfig0 & NUC1XX_FLASH_CONFIG_CBS) == 0 ? 0 : 1);
	switch(uConfig0 & NUC1XX_FLASH_CONFIG_CBS2)
	{
	case NUC1XX_FLASH_CONFIG_CBS_LD:
		m_nRadioBS = 0; break;
	case NUC1XX_FLASH_CONFIG_CBS_AP:
		m_nRadioBS = 1; break;
	case NUC1XX_FLASH_CONFIG_CBS_LD_AP:
		m_nRadioBS = 2; break;
	case NUC1XX_FLASH_CONFIG_CBS_AP_LD:
	default:
		m_nRadioBS = 3; break;
	}

	m_bWDTPowerDown = ((uConfig0 & NUC1XX_FLASH_CONFIG_CWDTPDEN) == 0 ? TRUE : FALSE);
	m_bWDTEnable = ((uConfig0 & NUC1XX_FLASH_CONFIG_CWDTEN) == 0 ? TRUE : FALSE);;
	if(!m_bWDTEnable) m_bWDTPowerDown = FALSE;

	m_bCheckBrownOutDetect = ((uConfig0 & NUC1XX_FLASH_CONFIG_CBODEN) == 0 ? TRUE : FALSE);
	m_bCheckBrownOutReset = ((uConfig0 & NUC1XX_FLASH_CONFIG_CBORST) == 0 ? TRUE : FALSE);
	m_bClockFilterEnable = ((uConfig0 & NUC1XX_FLASH_CONFIG_CKF) == NUC1XX_FLASH_CONFIG_CKF ? TRUE : FALSE);
	m_bDataFlashEnable = ((uConfig0 & NUC1XX_FLASH_CONFIG_DFEN) == 0 ? TRUE : FALSE);
	m_bSecurityLock = ((uConfig0 & NUC1XX_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);

	m_BCHZ_BPWM = ((uConfig0 & MT5XX_FLASH_CONFIG_CHZ_BPWM) == MT5XX_FLASH_CONFIG_CHZ_BPWM ? TRUE : FALSE);
	m_BCHZ_Odd1 = ((uConfig0 & MT5XX_FLASH_CONFIG_CHZ_Odd1) == MT5XX_FLASH_CONFIG_CHZ_Odd1 ? TRUE : FALSE);
	m_bCHZ_Even1 = ((uConfig0 & MT5XX_FLASH_CONFIG_CHZ_Event1) == MT5XX_FLASH_CONFIG_CHZ_Event1 ? TRUE : FALSE);
	m_BCHZ_Odd0 = ((uConfig0 & MT5XX_FLASH_CONFIG_CHZ_Odd0) == MT5XX_FLASH_CONFIG_CHZ_Odd0 ? TRUE : FALSE);
	m_bCHZ_Even0 = ((uConfig0 & MT5XX_FLASH_CONFIG_CHZ_Event0) == MT5XX_FLASH_CONFIG_CHZ_Event0 ? TRUE : FALSE);

	unsigned int uFlashBaseAddress = uConfig1;
	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);

	if(m_bIsDataFlashFixed)
	{
		m_sDataFlashSize.Format(_T("%.2fK"), m_uDataFlashSize / 1024.);
		m_SpinDataFlashSize.EnableWindow(FALSE);
	}
	else
	{
		unsigned int uPageNum = uFlashBaseAddress / NUC1XX_FLASH_PAGE_SIZE;
		unsigned int uLimitNum = m_uProgramMemorySize / NUC1XX_FLASH_PAGE_SIZE;
		unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * NUC1XX_FLASH_PAGE_SIZE) : 0;
		m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);
		m_SpinDataFlashSize.EnableWindow(m_bDataFlashEnable ? TRUE : FALSE);
	}

	m_sConfigValue0.Format(_T("0x%08X"), uConfig0);
	m_sConfigValue1.Format(_T("0x%08X"), uConfig1);
}

void CDialogConfiguration_MT500::GUIToConfig(int nEventID)
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig1;

	uConfig0 &= ~NUC1XX_FLASH_CONFIG_CFOSC;
	switch(m_nRadioClk)
	{
	case 0:
		uConfig0 |= NUC1XX_FLASH_CONFIG_E12M; break;
	case 1:
		uConfig0 |= NUC1XX_FLASH_CONFIG_E32K; break;
	case 2:
		uConfig0 |= NUC1XX_FLASH_CONFIG_PLL; break;
	case 3:
		uConfig0 |= NUC1XX_FLASH_CONFIG_I10K; break;
	case 4:
		uConfig0 |= NUC1XX_FLASH_CONFIG_CFOSC;	/* New spec! */ 
		break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & NUC1XX_FLASH_CONFIG_CFOSC);
	}

	uConfig0 &= ~NUC1XX_FLASH_CONFIG_CBOV;
	switch(m_nRadioBov)
	{
	case 0:
		uConfig0 |= NUC1XX_FLASH_CONFIG_CBOV_45; break;
	case 1:
		uConfig0 |= NUC1XX_FLASH_CONFIG_CBOV_38; break;
	case 2:
		uConfig0 |= NUC1XX_FLASH_CONFIG_CBOV_26; break;
	case 3:
		uConfig0 |= NUC1XX_FLASH_CONFIG_CBOV_22; break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & NUC1XX_FLASH_CONFIG_CBOV);
	}

	//if(m_nRadioBS == 0)
	//	uConfig0 &= ~NUC1XX_FLASH_CONFIG_CBS;
	//else
	//	uConfig0 |= NUC1XX_FLASH_CONFIG_CBS;
	uConfig0 &= ~NUC1XX_FLASH_CONFIG_CBS2;
	switch(m_nRadioBS)
	{
	case 0:
		uConfig0 |= NUC1XX_FLASH_CONFIG_CBS_LD; break;
	case 1:
		uConfig0 |= NUC1XX_FLASH_CONFIG_CBS_AP; break;
	case 2:
		uConfig0 |= NUC1XX_FLASH_CONFIG_CBS_LD_AP; break;
	case 3:
		uConfig0 |= NUC1XX_FLASH_CONFIG_CBS_AP_LD; break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & NUC1XX_FLASH_CONFIG_CBS2);
	}
	if(m_bWDTPowerDown)
		uConfig0 &= ~NUC1XX_FLASH_CONFIG_CWDTPDEN;
	else
		uConfig0 |= NUC1XX_FLASH_CONFIG_CWDTPDEN;
	if(m_bWDTEnable)
		uConfig0 &= ~NUC1XX_FLASH_CONFIG_CWDTEN;
	else
		uConfig0 |= NUC1XX_FLASH_CONFIG_CWDTEN;
	if(nEventID == IDC_CHECK_WDT_POWER_DOWN)
	{
		if(m_bWDTPowerDown)
			uConfig0 &= ~NUC1XX_FLASH_CONFIG_CWDTEN;
	}
	else
	{
		if(!m_bWDTEnable)
			uConfig0 |= NUC1XX_FLASH_CONFIG_CWDTPDEN;
	}

	if(m_bCheckBrownOutDetect)
		uConfig0 &= ~NUC1XX_FLASH_CONFIG_CBODEN;
	else
		uConfig0 |= NUC1XX_FLASH_CONFIG_CBODEN;

	if(m_bCheckBrownOutReset)
		uConfig0 &= ~NUC1XX_FLASH_CONFIG_CBORST;
	else
		uConfig0 |= NUC1XX_FLASH_CONFIG_CBORST;
	if(m_bClockFilterEnable)
		uConfig0 |= NUC1XX_FLASH_CONFIG_CKF;
	else
		uConfig0 &= ~NUC1XX_FLASH_CONFIG_CKF;
	if(m_bDataFlashEnable)
		uConfig0 &= ~NUC1XX_FLASH_CONFIG_DFEN;
	else
		uConfig0 |= NUC1XX_FLASH_CONFIG_DFEN;
	if(m_bSecurityLock)
		uConfig0 &= ~NUC1XX_FLASH_CONFIG_LOCK;
	else
		uConfig0 |= NUC1XX_FLASH_CONFIG_LOCK;


	if(m_BCHZ_BPWM)
		uConfig0 |= MT5XX_FLASH_CONFIG_CHZ_BPWM;
	else
		uConfig0 &= ~MT5XX_FLASH_CONFIG_CHZ_BPWM;

	if(m_BCHZ_Odd1)
		uConfig0 |= MT5XX_FLASH_CONFIG_CHZ_Odd1;
	else
		uConfig0 &= ~MT5XX_FLASH_CONFIG_CHZ_Odd1;

	if(m_bCHZ_Even1)
		uConfig0 |= MT5XX_FLASH_CONFIG_CHZ_Event1;
	else
		uConfig0 &= ~MT5XX_FLASH_CONFIG_CHZ_Event1;

	if(m_BCHZ_Odd0)
		uConfig0 |= MT5XX_FLASH_CONFIG_CHZ_Odd0;
	else
		uConfig0 &= ~MT5XX_FLASH_CONFIG_CHZ_Odd0;

	if(m_bCHZ_Even0)
		uConfig0 |= MT5XX_FLASH_CONFIG_CHZ_Event0;
	else
		uConfig0 &= ~MT5XX_FLASH_CONFIG_CHZ_Event0;

	//Removed if(m_bWatchDogEnable)
	//uConfig0 |= NUC1XX_FLASH_CONFIG_CWDTEN;  ?????

	m_ConfigValue.m_value[0] = uConfig0;

	TCHAR *pEnd;
	uConfig1 = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	m_ConfigValue.m_value[1] = uConfig1;
}

void CDialogConfiguration_MT500::OnGUIEvent(int nEventID) 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig(nEventID);
	ConfigToGUI(nEventID);

	UpdateData(FALSE);
}
void CDialogConfiguration_MT500::OnRadioBov() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig(0);
	ConfigToGUI(0);

	UpdateData(FALSE);
}

void CDialogConfiguration_MT500::OnRadioClk() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_MT500::OnRadioBs() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_MT500::OnCheckClick() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_MT500::OnCheckClickWDTPD() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent(IDC_CHECK_WDT_POWER_DOWN);
}

void CDialogConfiguration_MT500::OnCheckClickWDT() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent(IDC_CHECK_WDT_ENABLE);
}

void CDialogConfiguration_MT500::OnChangeEditFlashBaseAddress() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	TCHAR *pEnd;
	unsigned int uFlashBaseAddress = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	m_sConfigValue1.Format(_T("0x%08X"), uFlashBaseAddress);

	if(m_bIsDataFlashFixed)
	{
		m_sDataFlashSize.Format(_T("%.2fK"), m_uDataFlashSize / 1024.);
	}
	else
	{
		unsigned int uPageNum = uFlashBaseAddress / NUC1XX_FLASH_PAGE_SIZE;
		unsigned int uLimitNum = m_uProgramMemorySize / NUC1XX_FLASH_PAGE_SIZE;
		unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * NUC1XX_FLASH_PAGE_SIZE) : 0;
		m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);
	}

	UpdateData(FALSE);
}

void CDialogConfiguration_MT500::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	
	GUIToConfig(0);
	
	CDialog::OnOK();
}

// CDialogConfiguration_MT500 訊息處理常式
CString CDialogConfiguration_MT500::GetConfigWarning(const CAppConfig::MT5xx_configs_t &config)
{
	CString str;
	unsigned int uConfig0 = config.m_value[0];
	
	switch(uConfig0 & NUC1XX_FLASH_CONFIG_CFOSC)
	{
	case NUC1XX_FLASH_CONFIG_E12M:
		str += _T("   ") + _I(IDS_SELECT_EXTERNAL_12M_CLOCK);
		break;
	case NUC1XX_FLASH_CONFIG_E32K:
		str += _T("   ") + _I(IDS_SELECT_EXTERNAL_32K_CLOCK);
		break;
	case NUC1XX_FLASH_CONFIG_PLL:
		str += _T("   ") + _I(IDS_SELECT_PLL_CLOCK);
		break;
	default:
		;
	}

	BOOL bSecurityLock = ((uConfig0 & NUC1XX_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);
	if(!bSecurityLock)
		str += _T("   ") + _I(IDS_DISABLE_SECURITY_LOCK);

	return str;
}

void CDialogConfiguration_MT500::OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	TCHAR *pEnd;
	unsigned int uFlashBaseAddress = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	unsigned int uPageNum = uFlashBaseAddress / NUC1XX_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / NUC1XX_FLASH_PAGE_SIZE;

	if(pNMUpDown->iDelta == 1)
		uPageNum += 1;
	else if(pNMUpDown->iDelta == -1 && uPageNum > 0)
		uPageNum -= 1;

	uFlashBaseAddress = 0 + min(uPageNum, uLimitNum) * NUC1XX_FLASH_PAGE_SIZE;
	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);
	m_sConfigValue1.Format(_T("0x%08X"), uFlashBaseAddress);

	if(m_bIsDataFlashFixed)
	{
		m_sDataFlashSize.Format(_T("%.2fK"), m_uDataFlashSize / 1024.);
	}
	else
	{
		unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * NUC1XX_FLASH_PAGE_SIZE) : 0;
		m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);
	}

	UpdateData(FALSE);
	*pResult = 0;
}

void CDialogConfiguration_MT500::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar != NULL && pScrollBar->GetDlgCtrlID() == m_SpinDataFlashSize.GetDlgCtrlID())
		return;

	CDialogResize2::OnVScroll(nSBCode, nPos, pScrollBar);
}
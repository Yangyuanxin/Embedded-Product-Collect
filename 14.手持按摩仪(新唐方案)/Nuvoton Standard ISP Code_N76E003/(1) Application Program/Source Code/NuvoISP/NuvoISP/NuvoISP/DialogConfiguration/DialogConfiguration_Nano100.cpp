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
#include "DialogConfiguration_Nano100.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_Nano100 dialog


CDialogConfiguration_Nano100::CDialogConfiguration_Nano100(unsigned int uProgramMemorySize,
														   CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_Nano100::IDD, pParent)
	, m_uProgramMemorySize(uProgramMemorySize)
{
	//{{AFX_DATA_INIT(CDialogConfiguration_Nano100)
	m_nRadioClk = -1;
	m_nRadioBor = -1;
	m_nRadioBS = -1;
	m_sConfigValue0 = _T("");
	m_sConfigValue1 = _T("");
	m_bClockFilterEnable = FALSE;
	m_bDataFlashEnable = FALSE;
	m_bSecurityLock = FALSE;
	m_sFlashBaseAddress = _T("");
	//}}AFX_DATA_INIT
}


void CDialogConfiguration_Nano100::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfiguration_Nano100)
	DDX_Control(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_FlashBaseAddress);
	DDX_Control(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_DataFlashSize);
	DDX_Control(pDX, IDC_SPIN_DATA_FLASH_SIZE, m_SpinDataFlashSize);
	DDX_Radio(pDX, IDC_RADIO_CLK_E12M, m_nRadioClk);
	DDX_Radio(pDX, IDC_RADIO_BOR_DISABLE, m_nRadioBor);
	DDX_Radio(pDX, IDC_RADIO_BS_LDROM, m_nRadioBS);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_0, m_sConfigValue0);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_1, m_sConfigValue1);
	DDX_Check(pDX, IDC_CHECK_CLOCK_FILTER_ENABLE, m_bClockFilterEnable);
	DDX_Check(pDX, IDC_CHECK_DATA_FLASH_ENABLE, m_bDataFlashEnable);
	DDX_Check(pDX, IDC_CHECK_SECURITY_LOCK, m_bSecurityLock);
	DDX_Text(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_sFlashBaseAddress);
	DDX_Text(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_sDataFlashSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfiguration_Nano100, CDialog)
	//{{AFX_MSG_MAP(CDialogConfiguration_Nano100)
	ON_BN_CLICKED(IDC_RADIO_BOV_45, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_CLK_E12M, OnRadioClk)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM, OnRadioBs)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_DETECT, OnCheckClick)
	ON_EN_CHANGE(IDC_EDIT_FLASH_BASE_ADDRESS, OnChangeEditFlashBaseAddress)
	ON_BN_CLICKED(IDC_RADIO_BOR_DISABLE, OnRadioBor)
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
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM_APROM, OnRadioBs)
	ON_BN_CLICKED(IDC_RADIO_BS_APROM_LDROM, OnRadioBs)
	ON_BN_CLICKED(IDC_RADIO_BOR_25, OnRadioBor)
	ON_BN_CLICKED(IDC_RADIO_BOR_20, OnRadioBor)
	ON_BN_CLICKED(IDC_RADIO_BOR_17, OnRadioBor)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DATA_FLASH_SIZE, OnDeltaposSpinDataFlashSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_Nano100 message handlers

BOOL CDialogConfiguration_Nano100::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	UDACCEL pAccel[1];
	pAccel[0].nInc = 1;
	pAccel[0].nSec = 0;
	m_SpinDataFlashSize.SetAccel(1, pAccel);

	ConfigToGUI();

	UpdateData(FALSE);

	m_bIsInitialized = true;
	GetWindowRect(m_rect);
	AdjustDPI();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CDialogConfiguration_Nano100::ConfigToGUI()
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig1 = m_ConfigValue.m_value[1];

	switch(uConfig0 & NANO100_FLASH_CONFIG_CFOSC)
	{
	case NANO100_FLASH_CONFIG_E12M:
		m_nRadioClk = 0; break;
	case NANO100_FLASH_CONFIG_I12M:
	default:
		m_nRadioClk = 4; break;
	}

	switch(uConfig0 & NANO100_FLASH_CONFIG_CBORST)
	{
	case NANO100_FLASH_CONFIG_CBORST_RESERVED:
		m_nRadioBor = 0; break;
	case NANO100_FLASH_CONFIG_CBORST_25:
		m_nRadioBor = 1; break;
	case NANO100_FLASH_CONFIG_CBORST_20:
		m_nRadioBor = 2; break;
	case NANO100_FLASH_CONFIG_CBORST_17:
	default:
		m_nRadioBor = 3; break;
	}

	switch(uConfig0 & NANO100_FLASH_CONFIG_CBS)
	{
	case NANO100_FLASH_CONFIG_CBS_LD:
		m_nRadioBS = 0; break;
	case NANO100_FLASH_CONFIG_CBS_AP:
		m_nRadioBS = 1; break;
	case NANO100_FLASH_CONFIG_CBS_LD_AP:
		m_nRadioBS = 2; break;
	case NANO100_FLASH_CONFIG_CBS_AP_LD:
	default:
		m_nRadioBS = 3; break;
	}

	m_bClockFilterEnable = ((uConfig0 & NANO100_FLASH_CONFIG_CKF) == NANO100_FLASH_CONFIG_CKF ? TRUE : FALSE);
	//if((uConfig0 & NANO100_FLASH_CONFIG_CBS)==NANO100_FLASH_CONFIG_CBS_AP)		//2013 0521
	//{
	//	GetDlgItem(IDC_CHECK_DATA_FLASH_ENABLE)->EnableWindow(1);
	m_bDataFlashEnable = ((uConfig0 & NANO100_FLASH_CONFIG_DFEN) == 0 ? TRUE : FALSE);
	//}
	//else
	//{
	//	GetDlgItem(IDC_CHECK_DATA_FLASH_ENABLE)->EnableWindow(0);
	//	m_bDataFlashEnable=0;
	//}
	m_bSecurityLock = ((uConfig0 & NANO100_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);

	unsigned int uFlashBaseAddress = uConfig1;
	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);

	unsigned int uPageNum = uFlashBaseAddress / NANO100_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / NANO100_FLASH_PAGE_SIZE;
	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * NANO100_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);
	m_SpinDataFlashSize.EnableWindow(m_bDataFlashEnable ? TRUE : FALSE);

	m_sConfigValue0.Format(_T("0x%08X"), uConfig0);
	m_sConfigValue1.Format(_T("0x%08X"), uConfig1);
}

void CDialogConfiguration_Nano100::GUIToConfig()
{
	//unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig0 = 0xFFFFFFFF;
	unsigned int uConfig1;

	uConfig0 &= ~NANO100_FLASH_CONFIG_CFOSC;
	switch(m_nRadioClk)
	{
	case 0:
		uConfig0 |= NANO100_FLASH_CONFIG_E12M; break;
	case 4:
		uConfig0 |= NANO100_FLASH_CONFIG_I12M;	/* New spec! */ 
		break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & NANO100_FLASH_CONFIG_CFOSC);
	}

	uConfig0 &= ~NANO100_FLASH_CONFIG_CBS;
	switch(m_nRadioBS)
	{
	case 0:
		uConfig0 |= NANO100_FLASH_CONFIG_CBS_LD; break;
	case 1:
		uConfig0 |= NANO100_FLASH_CONFIG_CBS_AP; break;
	case 2:
		uConfig0 |= NANO100_FLASH_CONFIG_CBS_LD_AP; break;
	case 3:
		uConfig0 |= NANO100_FLASH_CONFIG_CBS_AP_LD; break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & NANO100_FLASH_CONFIG_CBS);
	}

	uConfig0 &= ~NANO100_FLASH_CONFIG_CBORST;
	switch(m_nRadioBor)
	{
	case 0:
		uConfig0 |= NANO100_FLASH_CONFIG_CBORST_RESERVED; break;
	case 1:
		uConfig0 |= NANO100_FLASH_CONFIG_CBORST_25; break;
	case 2:
		uConfig0 |= NANO100_FLASH_CONFIG_CBORST_20; break;
	case 3:
		uConfig0 |= NANO100_FLASH_CONFIG_CBORST_17; break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & NANO100_FLASH_CONFIG_CBORST);
	}

	if(m_bClockFilterEnable)
		uConfig0 |= NANO100_FLASH_CONFIG_CKF;
	else
		uConfig0 &= ~NANO100_FLASH_CONFIG_CKF;
	//if((m_bDataFlashEnable)&&(m_nRadioBS==1))		//2013 0521
	if(m_bDataFlashEnable)
		uConfig0 &= ~NANO100_FLASH_CONFIG_DFEN;
	else
		uConfig0 |= NANO100_FLASH_CONFIG_DFEN;
	if(m_bSecurityLock)
		uConfig0 &= ~NANO100_FLASH_CONFIG_LOCK;
	else
		uConfig0 |= NANO100_FLASH_CONFIG_LOCK;
	//Removed if(m_bWatchDogEnable)
	uConfig0 |= NANO100_FLASH_CONFIG_CWDTEN;

	m_ConfigValue.m_value[0] = uConfig0;
	
	TCHAR *pEnd;	
	uConfig1 = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	m_ConfigValue.m_value[1] = uConfig1;
}


void CDialogConfiguration_Nano100::OnRadioBov() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig();
	ConfigToGUI();

	UpdateData(FALSE);
}

void CDialogConfiguration_Nano100::OnRadioClk() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_Nano100::OnRadioBs() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_Nano100::OnCheckClick() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_Nano100::OnChangeEditFlashBaseAddress() 
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

	unsigned int uPageNum = uFlashBaseAddress / NANO100_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / NANO100_FLASH_PAGE_SIZE;
	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * NANO100_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);

	UpdateData(FALSE);
}

void CDialogConfiguration_Nano100::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	
	GUIToConfig();
	
	CDialog::OnOK();
}


CString CDialogConfiguration_Nano100::GetConfigWarning(const CAppConfig::Nano100_configs_t &config)
{
	CString str;
	unsigned int uConfig0 = config.m_value[0];
	
	switch(uConfig0 & NANO100_FLASH_CONFIG_CFOSC)
	{
	case NANO100_FLASH_CONFIG_E12M:
		str += _T("   ") + _I(IDS_SELECT_EXTERNAL_12M_CLOCK);
		break;
	//case NANO100_FLASH_CONFIG_E32K:
	//	str += _T("   ") + _I(IDS_SELECT_EXTERNAL_32K_CLOCK);
	//	break;
	//case NANO100_FLASH_CONFIG_PLL:
	//	str += _T("   ") + _I(IDS_SELECT_PLL_CLOCK);
	//	break;
	default:
		;
	}

	BOOL bSecurityLock = ((uConfig0 & NANO100_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);
	if(!bSecurityLock)
		str += _T("   ") + _I(IDS_DISABLE_SECURITY_LOCK);

	return str;
}

void CDialogConfiguration_Nano100::OnRadioBor() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();	
}

void CDialogConfiguration_Nano100::OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	TCHAR *pEnd;
	unsigned int uFlashBaseAddress = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	unsigned int uPageNum = uFlashBaseAddress / NANO100_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / NANO100_FLASH_PAGE_SIZE;

	if(pNMUpDown->iDelta == 1)
		uPageNum += 1;
	else if(pNMUpDown->iDelta == -1 && uPageNum > 0)
		uPageNum -= 1;

	uFlashBaseAddress = 0 + min(uPageNum, uLimitNum) * NANO100_FLASH_PAGE_SIZE;
	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);
	m_sConfigValue1.Format(_T("0x%08X"), uFlashBaseAddress);

	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * NANO100_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);

	UpdateData(FALSE);
	*pResult = 0;
}

void CDialogConfiguration_Nano100::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar != NULL && pScrollBar->GetDlgCtrlID() == m_SpinDataFlashSize.GetDlgCtrlID())
		return;

	CDialogResize2::OnVScroll(nSBCode, nPos, pScrollBar);
}

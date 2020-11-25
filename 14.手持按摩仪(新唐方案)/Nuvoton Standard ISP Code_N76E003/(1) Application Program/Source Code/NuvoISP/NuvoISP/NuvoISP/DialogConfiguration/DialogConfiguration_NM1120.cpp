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
#include "DialogConfiguration_NM1120.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_NM1120 dialog


CDialogConfiguration_NM1120::CDialogConfiguration_NM1120(unsigned int uProgramMemorySize,
															   CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_NM1120::IDD, pParent)
	, m_uProgramMemorySize(uProgramMemorySize)
{
	//{{AFX_DATA_INIT(CDialogConfiguration_NM1120)
	m_nRadioClk = -1;
	m_nRadioBor = -1;
	m_nRadioBov = -1;
	m_nRadioBS = -1;
	m_sConfigValue0 = _T("");
	m_sConfigValue1 = _T("");
	m_bDataFlashEnable = FALSE;
	m_bPwmDbgEnable = FALSE;
	m_bSecurityLock = FALSE;
	m_sFlashBaseAddress = _T("");
	m_bCheckBrownOutReset = FALSE;
	m_bCheckBrownOutEnable = FALSE;
	//}}AFX_DATA_INIT
}


void CDialogConfiguration_NM1120::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfiguration_NM1120)
	DDX_Control(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_FlashBaseAddress);
	DDX_Control(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_DataFlashSize);
	DDX_Control(pDX, IDC_SPIN_DATA_FLASH_SIZE, m_SpinDataFlashSize);
	DDX_Radio(pDX, IDC_RADIO_BOV_20, m_nRadioBov);
	DDX_Radio(pDX, IDC_RADIO_BS_LDROM, m_nRadioBS);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_0, m_sConfigValue0);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_1, m_sConfigValue1);
	DDX_Check(pDX, IDC_PWM_DEBUG_ENABLE, m_bPwmDbgEnable);
	DDX_Check(pDX, IDC_CHECK_DATA_FLASH_ENABLE, m_bDataFlashEnable);
	DDX_Check(pDX, IDC_CHECK_SECURITY_LOCK, m_bSecurityLock);
	DDX_Text(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_sFlashBaseAddress);
	DDX_Text(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_sDataFlashSize);
//	DDX_Check(pDX, IDC_RADIO_HIRC_ENABLE, m_bRadioHIRC);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_ENABLE, m_bCheckBrownOutEnable);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_RESET, m_bCheckBrownOutReset);
	DDX_Radio(pDX, IDC_RADIO_IO_TRI, m_nRadioIO);
	DDX_CBIndex(pDX, IDC_COMBO_GPA0_RINI, m_nGPA0rini);
	DDX_CBIndex(pDX, IDC_COMBO_GPA1_RINI, m_nGPA1rini);
	DDX_CBIndex(pDX, IDC_COMBO_GPA2_RINI, m_nGPA2rini);
	DDX_CBIndex(pDX, IDC_COMBO_GPA3_RINI, m_nGPA3rini);
	DDX_CBIndex(pDX, IDC_COMBO_GPA4_RINI, m_nGPA4rini);
	DDX_CBIndex(pDX, IDC_COMBO_GPA5_RINI, m_nGPA5rini);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogConfiguration_NM1120, CDialog)
	//{{AFX_MSG_MAP(CDialogConfiguration_NM1120)
	ON_EN_CHANGE(IDC_EDIT_FLASH_BASE_ADDRESS, OnChangeEditFlashBaseAddress)
	ON_BN_CLICKED(IDC_RADIO_BOV_43, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_40, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_37, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_30, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_27, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_24, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_22, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_20, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BS_APROM, OnRadioBs)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM, OnRadioBs)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM_APROM, OnRadioBs)
	ON_BN_CLICKED(IDC_RADIO_BS_APROM_LDROM, OnRadioBs)
	ON_BN_CLICKED(IDC_PWM_DEBUG_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_DATA_FLASH_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_SECURITY_LOCK, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_RESET, OnCheckClick)
	ON_BN_CLICKED(IDC_RADIO_IO_TRI, OnRadioIO)
	ON_BN_CLICKED(IDC_RADIO_IO_PO, OnRadioIO)
	ON_BN_CLICKED(IDC_RADIO_HIRC_ENABLE, OnCheckClick)
	ON_CBN_SELCHANGE(IDC_COMBO_GPA0_RINI, OnComboChange)
	ON_CBN_SELCHANGE(IDC_COMBO_GPA1_RINI, OnComboChange)
	ON_CBN_SELCHANGE(IDC_COMBO_GPA2_RINI, OnComboChange)
	ON_CBN_SELCHANGE(IDC_COMBO_GPA3_RINI, OnComboChange)
	ON_CBN_SELCHANGE(IDC_COMBO_GPA4_RINI, OnComboChange)
	ON_CBN_SELCHANGE(IDC_COMBO_GPA5_RINI, OnComboChange)

	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DATA_FLASH_SIZE, OnDeltaposSpinDataFlashSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_NM1120 message handlers

BOOL CDialogConfiguration_NM1120::OnInitDialog() 
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




void CDialogConfiguration_NM1120::ConfigToGUI()
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig1 = m_ConfigValue.m_value[1];

	m_nGPA0rini = (uConfig0 & NM1120_FLASH_CONFIG_GPA0NIRI) >> 16;
	m_nGPA1rini = (uConfig0 & NM1120_FLASH_CONFIG_GPA1NIRI) >> 18;
	m_nGPA2rini = (uConfig0 & NM1120_FLASH_CONFIG_GPA2NIRI) >> 20;
	m_nGPA3rini = (uConfig0 & NM1120_FLASH_CONFIG_GPA3NIRI) >> 22;
	m_nGPA4rini = (uConfig0 & NM1120_FLASH_CONFIG_GPA4NIRI) >> 24;
	m_nGPA5rini = (uConfig0 & NM1120_FLASH_CONFIG_GPA5NIRI) >> 26;

	if(m_nGPA0rini == 3)
		m_nGPA0rini = 2;
	if(m_nGPA1rini == 3)
		m_nGPA1rini = 2;
	if(m_nGPA2rini == 3)
		m_nGPA2rini = 2;
	if(m_nGPA3rini == 3)
		m_nGPA3rini = 2;
	if(m_nGPA4rini == 3)
		m_nGPA4rini = 2;
	if(m_nGPA5rini == 3)
		m_nGPA5rini = 2;

	switch(uConfig0 & NM1120_FLASH_CONFIG_CBOV)
	{
		case NM1120_FLASH_CONFIG_CBOV_20:
			m_nRadioBov = 0; break;
		case NM1120_FLASH_CONFIG_CBOV_22:
			m_nRadioBov = 1; break;
		case NM1120_FLASH_CONFIG_CBOV_24:
			m_nRadioBov = 2; break;
		case NM1120_FLASH_CONFIG_CBOV_27:
			m_nRadioBov = 3; break;
		case NM1120_FLASH_CONFIG_CBOV_30:
			m_nRadioBov = 4; break;
		case NM1120_FLASH_CONFIG_CBOV_37:
			m_nRadioBov = 5; break;
		case NM1120_FLASH_CONFIG_CBOV_40:
			m_nRadioBov = 6; break;
		case NM1120_FLASH_CONFIG_CBOV_43:
			m_nRadioBov = 7; break;
		default:
			m_nRadioBov = 7; break;
	}

	switch(uConfig0 & NM1120_FLASH_CONFIG_CBS)
	{
		case NM1120_FLASH_CONFIG_CBS_LD:
			m_nRadioBS = 0; break;
		case NM1120_FLASH_CONFIG_CBS_AP:
			m_nRadioBS = 1; break;
		case NM1120_FLASH_CONFIG_CBS_LD_AP:
			m_nRadioBS = 2; break;
		case NM1120_FLASH_CONFIG_CBS_AP_LD:
			m_nRadioBS = 3; break;
		default:
			m_nRadioBS = 1; break;
	}

	m_nRadioIO = ((uConfig0 & NM1120_FLASH_CONFIG_CIOINI) == 0 ? 1 : 0);
//	m_bRadioHIRC = ((uConfig0 & NM1120_FLASH_CONFIG_CKFHIRC) == 0 ? 0 : 1);

	m_bPwmDbgEnable = ((uConfig0 & NM1120_FLASH_CONFIG_PWM_DBGEN) == 0 ? TRUE : FALSE);
	m_bCheckBrownOutEnable = ((uConfig0 & NM1120_FLASH_CONFIG_CBOVEN) == 0 ? TRUE : FALSE);
	m_bCheckBrownOutReset = ((uConfig0 & NM1120_FLASH_CONFIG_CBORST) == 0 ? TRUE : FALSE);
	m_bDataFlashEnable = ((uConfig0 & NM1120_FLASH_CONFIG_DFEN) == 0 ? TRUE : FALSE);
	m_bSecurityLock = ((uConfig0 & NM1120_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);

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

void CDialogConfiguration_NM1120::GUIToConfig()
{
	//unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig0 = 0xFFFFFFFF;
	unsigned int uConfig1;

	uConfig0 &= ~NM1120_FLASH_CONFIG_GPA0NIRI;
	switch(m_nGPA0rini)
	{
		case 0:
			uConfig0 |= (0x0 << 16);
			break;
		case 1:
			uConfig0 |= (0x1 << 16);
			break;
		case 2:
		case 3:
			uConfig0 |= (0x3 << 16);
			break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & NM1120_FLASH_CONFIG_GPA0NIRI);
			break;
	}

	uConfig0 &= ~NM1120_FLASH_CONFIG_GPA1NIRI;
	switch(m_nGPA1rini)
	{
		case 0:
			uConfig0 |= (0x0 << 18);
			break;
		case 1:
			uConfig0 |= (0x1 << 18);
			break;
		case 2:
		case 3:
			uConfig0 |= (0x3 << 18);
			break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & NM1120_FLASH_CONFIG_GPA1NIRI);
			break;
	}

	uConfig0 &= ~NM1120_FLASH_CONFIG_GPA2NIRI;
	switch(m_nGPA2rini)
	{
		case 0:
			uConfig0 |= (0x0 << 20);
			break;
		case 1:
			uConfig0 |= (0x1 << 20);
			break;
		case 2:
		case 3:
			uConfig0 |= (0x3 << 20);
			break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & NM1120_FLASH_CONFIG_GPA2NIRI);
			break;
	}

	uConfig0 &= ~NM1120_FLASH_CONFIG_GPA3NIRI;
	switch(m_nGPA3rini)
	{
		case 0:
			uConfig0 |= (0x0 << 22);
			break;
		case 1:
			uConfig0 |= (0x1 << 22);
			break;
		case 2:
		case 3:
			uConfig0 |= (0x3 << 22);
			break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & NM1120_FLASH_CONFIG_GPA3NIRI);
			break;
	}

	uConfig0 &= ~NM1120_FLASH_CONFIG_GPA4NIRI;
	switch(m_nGPA4rini)
	{
		case 0:
			uConfig0 |= (0x0 << 24);
			break;
		case 1:
			uConfig0 |= (0x1 << 24);
			break;
		case 2:
		case 3:
			uConfig0 |= (0x3 << 24);
			break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & NM1120_FLASH_CONFIG_GPA4NIRI);
			break;
	}

	uConfig0 &= ~NM1120_FLASH_CONFIG_GPA5NIRI;
	switch(m_nGPA5rini)
	{
		case 0:
			uConfig0 |= (0x0 << 26);
			break;
		case 1:
			uConfig0 |= (0x1 << 26);
			break;
		case 2:
		case 3:
			uConfig0 |= (0x3 << 26);
			break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & NM1120_FLASH_CONFIG_GPA5NIRI);
			break;
	}

	uConfig0 &= ~NM1120_FLASH_CONFIG_CBOV;
	switch(m_nRadioBov)
	{
		case 0:	//2.0V
			uConfig0 |= NM1120_FLASH_CONFIG_CBOV_20;
			break;
		case 1:	//2.2V
			uConfig0 |= NM1120_FLASH_CONFIG_CBOV_22;
			break;
		case 2:	//2.4V
			uConfig0 |= NM1120_FLASH_CONFIG_CBOV_24;
			break;
		case 3:	//2.7V
			uConfig0 |= NM1120_FLASH_CONFIG_CBOV_27;
			break;
		case 4:	//3.0V
			uConfig0 |= NM1120_FLASH_CONFIG_CBOV_30;
			break;
		case 5:	//3.7V
			uConfig0 |= NM1120_FLASH_CONFIG_CBOV_37;
			break;
		case 6:	//4.0V
			uConfig0 |= NM1120_FLASH_CONFIG_CBOV_40;
			break;
		case 7:	//4.3V
			uConfig0 |= NM1120_FLASH_CONFIG_CBOV_43;
			break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & NM1120_FLASH_CONFIG_CBOV);
			break;
	}

	if(m_nRadioIO)
		uConfig0 &= ~NM1120_FLASH_CONFIG_CIOINI;
	else
		uConfig0 |= NM1120_FLASH_CONFIG_CIOINI;

	uConfig0 &= ~NM1120_FLASH_CONFIG_CBS;
	switch(m_nRadioBS)
	{
		case 0:
			uConfig0 |= NM1120_FLASH_CONFIG_CBS_LD; break;
		case 1:
			uConfig0 |= NM1120_FLASH_CONFIG_CBS_AP; break;
		case 2:
			uConfig0 |= NM1120_FLASH_CONFIG_CBS_LD_AP; break;
		case 3:
			uConfig0 |= NM1120_FLASH_CONFIG_CBS_AP_LD; break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & NM1120_FLASH_CONFIG_CBS);
			break;
	}

	if(m_bCheckBrownOutReset)
		uConfig0 &= ~NM1120_FLASH_CONFIG_CBORST;
	else
		uConfig0 |= NM1120_FLASH_CONFIG_CBORST;

	if(m_bCheckBrownOutEnable)
		uConfig0 &= ~NM1120_FLASH_CONFIG_CBOVEN;
	else
		uConfig0 |= NM1120_FLASH_CONFIG_CBOVEN;

	if(m_bPwmDbgEnable)
		uConfig0 &= ~NM1120_FLASH_CONFIG_PWM_DBGEN;
	else
		uConfig0 |= NM1120_FLASH_CONFIG_PWM_DBGEN;

	if(m_bDataFlashEnable)
		uConfig0 &= ~NM1120_FLASH_CONFIG_DFEN;
	else
		uConfig0 |= NM1120_FLASH_CONFIG_DFEN;

	if(m_bSecurityLock)
		uConfig0 &= ~NM1120_FLASH_CONFIG_LOCK;
	else
		uConfig0 |= NM1120_FLASH_CONFIG_LOCK;


	m_ConfigValue.m_value[0] = uConfig0;

	TCHAR *pEnd;
	uConfig1 = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	m_ConfigValue.m_value[1] = uConfig1;
}


void CDialogConfiguration_NM1120::OnRadioBov() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig();
	ConfigToGUI();

	UpdateData(FALSE);
}

void CDialogConfiguration_NM1120::OnRadioIO() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_NM1120::OnRadioBs() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_NM1120::OnCheckClick() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_NM1120::OnComboChange()
{
	UpdateData(TRUE);

	GUIToConfig();
	ConfigToGUI();

	UpdateData(FALSE);
}

void CDialogConfiguration_NM1120::OnChangeEditFlashBaseAddress() 
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

void CDialogConfiguration_NM1120::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	GUIToConfig();

	CDialog::OnOK();
}


CString CDialogConfiguration_NM1120::GetConfigWarning(const CAppConfig::NM1120_configs_t &config)
{
	CString str;
	unsigned int uConfig0 = config.m_value[0];

	BOOL bSecurityLock = ((uConfig0 & NM1120_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);
	if(!bSecurityLock)
		str += _T("   ") + _I(IDS_DISABLE_SECURITY_LOCK);

	return str;
}

void CDialogConfiguration_NM1120::OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult)
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

void CDialogConfiguration_NM1120::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar != NULL && pScrollBar->GetDlgCtrlID() == m_SpinDataFlashSize.GetDlgCtrlID())
		return;

	CDialogResize2::OnVScroll(nSBCode, nPos, pScrollBar);
}

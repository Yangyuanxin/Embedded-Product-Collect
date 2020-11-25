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
#include "DialogConfiguration_N570.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_N570 dialog


CDialogConfiguration_N570::CDialogConfiguration_N570(unsigned int uProgramMemorySize,
														 CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_N570::IDD, pParent)
	, m_uProgramMemorySize(uProgramMemorySize)
{
	//{{AFX_DATA_INIT(CDialogConfiguration_N570)
	m_nRadioBov = -1;
	m_nRadioBS = -1;
	m_sConfigValue0 = _T("");
	m_sConfigValue1 = _T("");
	m_bCheckLowVolResetEnable = FALSE;
	m_bCheckBrownOutHysteresis = FALSE;
	m_bCheckBrownOutEnable = FALSE;
	m_bCheckBrownOutReset = FALSE;
	m_bDataFlashEnable = FALSE;
	m_bSecurityLock = FALSE;
	m_sFlashBaseAddress = _T("");
	//}}AFX_DATA_INIT
}


void CDialogConfiguration_N570::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfiguration_N570)
	DDX_Control(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_FlashBaseAddress);
	DDX_Control(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_DataFlashSize);
	DDX_Control(pDX, IDC_SPIN_DATA_FLASH_SIZE, m_SpinDataFlashSize);
	DDX_Radio(pDX, IDC_RADIO_BOV_46, m_nRadioBov);
	DDX_Radio(pDX, IDC_RADIO_BS_LDROM, m_nRadioBS);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_0, m_sConfigValue0);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_1, m_sConfigValue1);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_ENABLE, m_bCheckBrownOutEnable);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_RESET, m_bCheckBrownOutReset);
	DDX_Check(pDX, IDC_CHECK_DATA_FLASH_ENABLE, m_bDataFlashEnable);
	DDX_Check(pDX, IDC_CHECK_SECURITY_LOCK, m_bSecurityLock);
	DDX_Check(pDX, IDC_CHECK_LVOL_RSTEN, m_bCheckLowVolResetEnable);
	DDX_Check(pDX, IDC_CHECK_BOD_HYS_EN, m_bCheckBrownOutHysteresis);
	DDX_Text(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_sFlashBaseAddress);
	DDX_Text(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_sDataFlashSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfiguration_N570, CDialog)
	//{{AFX_MSG_MAP(CDialogConfiguration_N570)
	ON_BN_CLICKED(IDC_RADIO_BOV_46, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_42, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_39, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_37, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_36, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_34, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_31, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_30, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_28, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_26, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_24, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_22, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_21, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_20, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_19, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_18, OnRadioBov)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_RESET, OnCheckClick)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM, OnRadioBs)
	ON_BN_CLICKED(IDC_RADIO_BS_APROM, OnRadioBs)
	ON_BN_CLICKED(IDC_CHECK_DATA_FLASH_ENABLE, OnCheckClick)
	ON_EN_CHANGE(IDC_EDIT_FLASH_BASE_ADDRESS, OnChangeEditFlashBaseAddress)
	ON_BN_CLICKED(IDC_CHECK_LVOL_RSTEN, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_BOD_HYS_EN, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_SECURITY_LOCK, OnCheckClick)

	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DATA_FLASH_SIZE, OnDeltaposSpinDataFlashSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_N570 message handlers

BOOL CDialogConfiguration_N570::OnInitDialog() 
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




void CDialogConfiguration_N570::ConfigToGUI()
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig1 = m_ConfigValue.m_value[1];

	switch(uConfig0 & N570_FLASH_CONFIG_CBOV)
	{
	case N570_FLASH_CONFIG_CBOV_46:
		m_nRadioBov = 0; break;
	case N570_FLASH_CONFIG_CBOV_42:
		m_nRadioBov = 1; break;
	case N570_FLASH_CONFIG_CBOV_39:
		m_nRadioBov = 2; break;
	case N570_FLASH_CONFIG_CBOV_37:
		m_nRadioBov = 3; break;
	case N570_FLASH_CONFIG_CBOV_36:
		m_nRadioBov = 4; break;
	case N570_FLASH_CONFIG_CBOV_34:
		m_nRadioBov = 5; break;
	case N570_FLASH_CONFIG_CBOV_31:
		m_nRadioBov = 6; break;
	case N570_FLASH_CONFIG_CBOV_30:
		m_nRadioBov = 7; break;
	case N570_FLASH_CONFIG_CBOV_28:
		m_nRadioBov = 8; break;
	case N570_FLASH_CONFIG_CBOV_26:
		m_nRadioBov = 9; break;
	case N570_FLASH_CONFIG_CBOV_24:
		m_nRadioBov = 10; break;
	case N570_FLASH_CONFIG_CBOV_22:
		m_nRadioBov = 11; break;	
	case N570_FLASH_CONFIG_CBOV_21:
		m_nRadioBov = 12; break;
	case N570_FLASH_CONFIG_CBOV_20:
		m_nRadioBov = 13; break;
	case N570_FLASH_CONFIG_CBOV_19:
		m_nRadioBov = 14; break;
	case N570_FLASH_CONFIG_CBOV_18:
		m_nRadioBov = 15; break;
	default:
		m_nRadioBov = 0; break;
	}

	m_bCheckBrownOutReset = ((uConfig0 & N570_FLASH_CONFIG_CBORST) == 0 ? TRUE : FALSE);
	m_bCheckBrownOutEnable = ((uConfig0 & N570_FLASH_CONFIG_CBOVEN) == 0 ? TRUE : FALSE);

	m_nRadioBS = ((uConfig0 & N570_FLASH_CONFIG_CBS) == 0 ? 0 : 1);
	m_bCheckLowVolResetEnable = ((uConfig0 & N570_FLASH_CONFIG_CLVR) == 0 ? TRUE : FALSE);
	m_bCheckBrownOutHysteresis = ((uConfig0 & N570_FLASH_CONFIG_CBHYS) == 0 ? FALSE : TRUE);
	m_bDataFlashEnable = ((uConfig0 & N570_FLASH_CONFIG_DFEN) == 0 ? TRUE : FALSE);
	m_bSecurityLock = ((uConfig0 & N570_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);

	unsigned int uFlashBaseAddress = uConfig1;
	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);

	unsigned int uPageNum = uFlashBaseAddress / NUMICRO_M0_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / NUMICRO_M0_FLASH_PAGE_SIZE;
	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * NUMICRO_M0_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);
	m_SpinDataFlashSize.EnableWindow(m_bDataFlashEnable ? TRUE : FALSE);

	m_sConfigValue0.Format(_T("0x%08X"), uConfig0);
	m_sConfigValue1.Format(_T("0x%08X"), uConfig1);
}

void CDialogConfiguration_N570::GUIToConfig()
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig1;

	uConfig0 &= ~N570_FLASH_CONFIG_CBOV;
	switch(m_nRadioBov)
	{
	case 0:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_46; break;
	case 1:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_42; break;
	case 2:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_39; break;
	case 3:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_37; break;
	case 4:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_36; break;
	case 5:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_34; break;
	case 6:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_31; break;
	case 7:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_30; break;
	case 8:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_28; break;
	case 9:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_26; break;
	case 10:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_24; break;
	case 11:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_22; break;
	case 12:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_21; break;
	case 13:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_20; break;
	case 14:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_19; break;
	case 15:
		uConfig0 |= N570_FLASH_CONFIG_CBOV_18; break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & N570_FLASH_CONFIG_CBOV);
	}

	if(m_bCheckBrownOutReset)
		uConfig0 &= ~N570_FLASH_CONFIG_CBORST;
	else
		uConfig0 |= N570_FLASH_CONFIG_CBORST;

	if(m_nRadioBS == 0)
		uConfig0 &= ~N570_FLASH_CONFIG_CBS;
	else
		uConfig0 |= N570_FLASH_CONFIG_CBS;

	if(m_bCheckBrownOutEnable)
		uConfig0 &= ~N570_FLASH_CONFIG_CBOVEN;
	else
		uConfig0 |= N570_FLASH_CONFIG_CBOVEN;

	if(m_bDataFlashEnable)
		uConfig0 &= ~N570_FLASH_CONFIG_DFEN;
	else
		uConfig0 |= N570_FLASH_CONFIG_DFEN;

	if(m_bSecurityLock)
		uConfig0 &= ~N570_FLASH_CONFIG_LOCK;
	else
		uConfig0 |= N570_FLASH_CONFIG_LOCK;

	if(m_bCheckLowVolResetEnable)
		uConfig0 &= ~N570_FLASH_CONFIG_CLVR;
	else
		uConfig0 |= N570_FLASH_CONFIG_CLVR;

	if(m_bCheckBrownOutHysteresis)
		uConfig0 |= N570_FLASH_CONFIG_CBHYS;
	else
		uConfig0 &= ~N570_FLASH_CONFIG_CBHYS;

	m_ConfigValue.m_value[0] = uConfig0;

	TCHAR *pEnd;
	uConfig1 = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	m_ConfigValue.m_value[1] = uConfig1;
}


void CDialogConfiguration_N570::OnRadioBov() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig();
	ConfigToGUI();

	UpdateData(FALSE);
}

void CDialogConfiguration_N570::OnRadioBs() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_N570::OnCheckClick() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_N570::OnChangeEditFlashBaseAddress() 
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

	unsigned int uPageNum = uFlashBaseAddress / NUMICRO_M0_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / NUMICRO_M0_FLASH_PAGE_SIZE;
	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * NUMICRO_M0_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);

	UpdateData(FALSE);
}

void CDialogConfiguration_N570::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	
	GUIToConfig();
	
	CDialog::OnOK();
}


CString CDialogConfiguration_N570::GetConfigWarning(const CAppConfig::AU91xx_configs_t &config)
{
	CString str;
	unsigned int uConfig0 = config.m_value[0];

	BOOL bSecurityLock = ((uConfig0 & AU91XX_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);
	if(!bSecurityLock)
		str += _T("   ") + _I(IDS_DISABLE_SECURITY_LOCK);

	return str;
}

void CDialogConfiguration_N570::OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	TCHAR *pEnd;
	unsigned int uFlashBaseAddress = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	unsigned int uPageNum = uFlashBaseAddress / NUMICRO_M0_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / NUMICRO_M0_FLASH_PAGE_SIZE;

	if(pNMUpDown->iDelta == 1)
		uPageNum += 1;
	else if(pNMUpDown->iDelta == -1 && uPageNum > 0)
		uPageNum -= 1;

	uFlashBaseAddress = 0 + min(uPageNum, uLimitNum) * NUMICRO_M0_FLASH_PAGE_SIZE;
	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);
	m_sConfigValue1.Format(_T("0x%08X"), uFlashBaseAddress);

	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * NUMICRO_M0_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);

	UpdateData(FALSE);
	*pResult = 0;
}

void CDialogConfiguration_N570::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar != NULL && pScrollBar->GetDlgCtrlID() == m_SpinDataFlashSize.GetDlgCtrlID())
		return;

	CDialogResize2::OnVScroll(nSBCode, nPos, pScrollBar);
}


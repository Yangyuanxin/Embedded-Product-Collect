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
#include "DialogConfiguration_NM1200.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_NM1200 dialog


CDialogConfiguration_NM1200::CDialogConfiguration_NM1200(unsigned int uProgramMemorySize,
														 CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_NM1200::IDD, pParent)
	, m_uProgramMemorySize(uProgramMemorySize)
{
	//{{AFX_DATA_INIT(CDialogConfiguration_NM1200)
	m_nRadioBov = -1;
	m_nRadioBS = -1;
	m_sConfigValue0 = _T("");
	m_sConfigValue1 = _T("");
	m_bDataFlashEnable = FALSE;
	m_bSecurityLock = FALSE;
	m_sFlashBaseAddress = _T("");
	m_bCheckBrownOutReset = FALSE;
	//}}AFX_DATA_INIT
}


void CDialogConfiguration_NM1200::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfiguration_NM1200)
	DDX_Control(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_FlashBaseAddress);
	DDX_Control(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_DataFlashSize);
	DDX_Control(pDX, IDC_SPIN_DATA_FLASH_SIZE, m_SpinDataFlashSize);
	DDX_Radio(pDX, IDC_RADIO_BOV_DISABLE, m_nRadioBov);
	DDX_Radio(pDX, IDC_RADIO_BS_LDROM, m_nRadioBS);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_0, m_sConfigValue0);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_1, m_sConfigValue1);
	DDX_Check(pDX, IDC_CHECK_DATA_FLASH_ENABLE, m_bDataFlashEnable);
	DDX_Check(pDX, IDC_CHECK_SECURITY_LOCK, m_bSecurityLock);
	DDX_Text(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_sFlashBaseAddress);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_RESET, m_bCheckBrownOutReset);
	DDX_Radio(pDX, IDC_RADIO_IO_TRI, m_nRadioIO);
	DDX_Radio(pDX, IDC_RADIO_RC_44M, m_nRadioRC);
	DDX_Text(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_sDataFlashSize);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogConfiguration_NM1200, CDialog)
	//{{AFX_MSG_MAP(CDialogConfiguration_NM1200)
	ON_BN_CLICKED(IDC_RADIO_BOV_DISABLE, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM, OnRadioBs)
	ON_EN_CHANGE(IDC_EDIT_FLASH_BASE_ADDRESS, OnChangeEditFlashBaseAddress)
	ON_BN_CLICKED(IDC_RADIO_BOV_43, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_37, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_30, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_27, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_24, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_22, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_20, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_17, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BS_APROM, OnRadioBs)
	ON_BN_CLICKED(IDC_CHECK_DATA_FLASH_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_SECURITY_LOCK, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_RESET, OnCheckClick)
	ON_BN_CLICKED(IDC_RADIO_IO_TRI, OnRadioIO)
	ON_BN_CLICKED(IDC_RADIO_IO_BI, OnRadioIO)
	ON_BN_CLICKED(IDC_RADIO_RC_22M, OnRadioRC)
	ON_BN_CLICKED(IDC_RADIO_RC_24M, OnRadioRC)
	ON_BN_CLICKED(IDC_RADIO_RC_44M, OnRadioRC)
	ON_BN_CLICKED(IDC_RADIO_RC_48M, OnRadioRC)
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

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_NM1200 message handlers

BOOL CDialogConfiguration_NM1200::OnInitDialog() 
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




void CDialogConfiguration_NM1200::ConfigToGUI()
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig1 = m_ConfigValue.m_value[1];

	if(uConfig0 & MINI51_FLASH_CONFIG_CBOD2VEN)
	{
		m_nRadioBov = 0;
	}
	else
	{
		if(uConfig0 & MINI51_FLASH_CONFIG_CBOV2)
		{
			switch(uConfig0 & MINI51_FLASH_CONFIG_CBOV)
			{
				//case MINI51_FLASH_CONFIG_CBOV_DISABLE:
				//	m_nRadioBov = 0; break;
				case MINI51_FLASH_CONFIG_CBOV_44:
					m_nRadioBov = 6; break;
				case MINI51_FLASH_CONFIG_CBOV_38:
					m_nRadioBov = 4; break;
				case MINI51_FLASH_CONFIG_CBOV_27:
					m_nRadioBov = 2; break;
				case MINI51_FLASH_CONFIG_CBOV_27_:
					m_nRadioBov = 1; break;
				default:
					m_nRadioBov = 0; break;
			}
		}
		else
		{
			switch(uConfig0 & MINI51_FLASH_CONFIG_CBOV)
			{
				//case MINI51_FLASH_CONFIG_CBOV_DISABLE:
				//	m_nRadioBov = 0; break;
				case MINI51_FLASH_CONFIG_CBOV_44:
					m_nRadioBov = 8; break;
				case MINI51_FLASH_CONFIG_CBOV_38:
					m_nRadioBov = 7; break;
				case MINI51_FLASH_CONFIG_CBOV_27:
					m_nRadioBov = 5; break;
				case MINI51_FLASH_CONFIG_CBOV_27_:
					m_nRadioBov = 3; break;
				default:
					m_nRadioBov = 0; break;
			}
		}
	}

	//m_nRadioBS = ((uConfig0 & MINI51_FLASH_CONFIG_CBS) == 0 ? 0 : 1);

	switch(uConfig0 & MINI51_FLASH_CONFIG_CBS2)
	{
		case MINI51_FLASH_CONFIG_CBS_LD:
			m_nRadioBS = 0; break;
		case MINI51_FLASH_CONFIG_CBS_AP:
			m_nRadioBS = 1; break;
		case MINI51_FLASH_CONFIG_CBS_LD_AP:
			m_nRadioBS = 2; break;
		case MINI51_FLASH_CONFIG_CBS_AP_LD:
			m_nRadioBS = 3; break;
		default:
			m_nRadioBS = 3; break;
	}

	m_nRadioIO = ((uConfig0 & MINI51_FLASH_CONFIG_CIOINI) == 0 ? 1 : 0);
	if(uConfig0 & MINI51_FLASH_CONFIG_RCTRIM_S)
	{
		if(uConfig0 & MINI51_FLASH_CONFIG_RCCLK_D2)
			m_nRadioRC = 2;
		else
			m_nRadioRC = 0;
	}
	else
	{
		if(uConfig0 & MINI51_FLASH_CONFIG_RCCLK_D2)
			m_nRadioRC = 3;
		else
			m_nRadioRC = 1;
	}

	m_bCheckBrownOutReset = ((uConfig0 & MINI51_FLASH_CONFIG_CBORST) == 0 ? TRUE : FALSE);
	m_bDataFlashEnable = ((uConfig0 & MINI51_FLASH_CONFIG_DFEN) == 0 ? TRUE : FALSE);
	m_bSecurityLock = ((uConfig0 & MINI51_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);

	unsigned int uFlashBaseAddress = uConfig1;
	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);

	unsigned int uPageNum = uFlashBaseAddress / MINI51_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / MINI51_FLASH_PAGE_SIZE;
	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * MINI51_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);
	m_SpinDataFlashSize.EnableWindow(m_bDataFlashEnable ? TRUE : FALSE);

	m_sConfigValue0.Format(_T("0x%08X"), uConfig0);
	m_sConfigValue1.Format(_T("0x%08X"), uConfig1);
}

void CDialogConfiguration_NM1200::GUIToConfig()
{
	//unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig0 = 0xFFFFFFFF;
	unsigned int uConfig1;

	uConfig0 &= ~MINI51_FLASH_CONFIG_CBOV;
	uConfig0 &= ~MINI51_FLASH_CONFIG_CBOV2;
	uConfig0 &= ~MINI51_FLASH_CONFIG_CBOD2VEN;
	switch(m_nRadioBov)
	{
		case 0:	//Disable BOD
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV_DISABLE;
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV2;
			uConfig0 |= MINI51_FLASH_CONFIG_CBOD2VEN;
			break;
		case 1:	//1.7V
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV2;
			break;
		case 2:	//2.0V
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV_27;
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV2;
			break;
		case 3:	//2.2V
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV_27_;
			break;
		case 4:	//2.4V
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV_38;
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV2;
			break;
		case 5:	//2.7V
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV_27;
			break;
		case 6:	//3.0V
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV_44;
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV2;
			break;
		case 7:	//3.7V
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV_38;
			break;
		case 8:	//4.3V
			uConfig0 |= MINI51_FLASH_CONFIG_CBOV_44;
			break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & MINI51_FLASH_CONFIG_CBOV);
			break;
	}

	if(m_nRadioIO)
		uConfig0 &= ~MINI51_FLASH_CONFIG_CIOINI;
	else
		uConfig0 |= MINI51_FLASH_CONFIG_CIOINI;

	switch(m_nRadioRC)
	{
		case 0:	// 44M
			uConfig0 |= MINI51_FLASH_CONFIG_RCTRIM_S;
			uConfig0 &= ~MINI51_FLASH_CONFIG_RCCLK_D2;
			break;
		case 1:	// 48M
			uConfig0 &= ~MINI51_FLASH_CONFIG_RCTRIM_S;
			uConfig0 &= ~MINI51_FLASH_CONFIG_RCCLK_D2;
			break;
		case 2:	// 22M
			uConfig0 |= MINI51_FLASH_CONFIG_RCTRIM_S;
			uConfig0 |= MINI51_FLASH_CONFIG_RCCLK_D2;
			break;
		case 3:	// 24M
			uConfig0 &= ~MINI51_FLASH_CONFIG_RCTRIM_S;
			uConfig0 |= MINI51_FLASH_CONFIG_RCCLK_D2;
			break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & MINI51_FLASH_CONFIG_RCTRIM_S);
			uConfig0 |= (m_ConfigValue.m_value[0] & MINI51_FLASH_CONFIG_RCCLK_D2);
			break;
	}

	//if(m_nRadioBS == 0)
	//	uConfig0 &= ~MINI51_FLASH_CONFIG_CBS;
	//else
	//	uConfig0 |= MINI51_FLASH_CONFIG_CBS;

	uConfig0 &= ~MINI51_FLASH_CONFIG_CBS2;
	switch(m_nRadioBS)
	{
		case 0:
			uConfig0 |= MINI51_FLASH_CONFIG_CBS_LD; break;
		case 1:
			uConfig0 |= MINI51_FLASH_CONFIG_CBS_AP; break;
		case 2:
			uConfig0 |= MINI51_FLASH_CONFIG_CBS_LD_AP; break;
		case 3:
			uConfig0 |= MINI51_FLASH_CONFIG_CBS_AP_LD; break;
		default:
			/* Keep old value */
			uConfig0 |= (m_ConfigValue.m_value[0] & MINI51_FLASH_CONFIG_CBS2);
			break;
	}

	if(m_bCheckBrownOutReset)
		uConfig0 &= ~MINI51_FLASH_CONFIG_CBORST;
	else
		uConfig0 |= MINI51_FLASH_CONFIG_CBORST;

	if(m_bDataFlashEnable)
		uConfig0 &= ~MINI51_FLASH_CONFIG_DFEN;
	else
		uConfig0 |= MINI51_FLASH_CONFIG_DFEN;

	if(m_bSecurityLock)
		uConfig0 &= ~MINI51_FLASH_CONFIG_LOCK;
	else
		uConfig0 |= MINI51_FLASH_CONFIG_LOCK;


	m_ConfigValue.m_value[0] = uConfig0;

	TCHAR *pEnd;
	uConfig1 = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	m_ConfigValue.m_value[1] = uConfig1;
}


void CDialogConfiguration_NM1200::OnRadioBov() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig();
	ConfigToGUI();

	UpdateData(FALSE);
}

void CDialogConfiguration_NM1200::OnRadioRC() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_NM1200::OnRadioIO() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_NM1200::OnRadioBs() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_NM1200::OnCheckClick() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_NM1200::OnChangeEditFlashBaseAddress() 
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

	unsigned int uPageNum = uFlashBaseAddress / MINI51_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / MINI51_FLASH_PAGE_SIZE;
	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * MINI51_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);

	UpdateData(FALSE);
}

void CDialogConfiguration_NM1200::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	GUIToConfig();

	CDialog::OnOK();
}


CString CDialogConfiguration_NM1200::GetConfigWarning(const CAppConfig::Mini51_configs_t &config)
{
	CString str;
	unsigned int uConfig0 = config.m_value[0];

	BOOL bSecurityLock = ((uConfig0 & MINI51_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);
	if(!bSecurityLock)
		str += _T("   ") + _I(IDS_DISABLE_SECURITY_LOCK);

	return str;
}

void CDialogConfiguration_NM1200::OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	TCHAR *pEnd;
	unsigned int uFlashBaseAddress = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	unsigned int uPageNum = uFlashBaseAddress / MINI51_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / MINI51_FLASH_PAGE_SIZE;

	if(pNMUpDown->iDelta == 1)
		uPageNum += 1;
	else if(pNMUpDown->iDelta == -1 && uPageNum > 0)
		uPageNum -= 1;

	uFlashBaseAddress = 0 + min(uPageNum, uLimitNum) * MINI51_FLASH_PAGE_SIZE;
	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);
	m_sConfigValue1.Format(_T("0x%08X"), uFlashBaseAddress);

	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * MINI51_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);

	UpdateData(FALSE);
	*pResult = 0;
}

void CDialogConfiguration_NM1200::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar != NULL && pScrollBar->GetDlgCtrlID() == m_SpinDataFlashSize.GetDlgCtrlID())
		return;

	CDialogResize2::OnVScroll(nSBCode, nPos, pScrollBar);
}

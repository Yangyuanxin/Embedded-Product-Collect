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
#include "DialogConfiguration_Mini51CN.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_Mini51CN dialog


CDialogConfiguration_Mini51CN::CDialogConfiguration_Mini51CN(unsigned int uProgramMemorySize,
															 bool m_bIsMini58,
															 CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_Mini51CN::IDD, pParent)
	, m_uProgramMemorySize(uProgramMemorySize)
	, m_bIsMini58(m_bIsMini58)
{
	//{{AFX_DATA_INIT(CDialogConfiguration_Mini51)
	m_nRadioBov = -1;
	m_nRadioBS = -1;
	m_sConfigValue0 = _T("");
	m_sConfigValue1 = _T("");
	m_bClockFilterEnable = FALSE;
	m_bDataFlashEnable = FALSE;
	m_bSecurityLock = FALSE;
	m_sFlashBaseAddress = _T("");
	m_bCheckBrownOutReset = FALSE;
	//}}AFX_DATA_INIT
}


void CDialogConfiguration_Mini51CN::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfiguration_Mini51CN)
	DDX_Control(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_FlashBaseAddress);
	DDX_Control(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_DataFlashSize);
	DDX_Control(pDX, IDC_SPIN_DATA_FLASH_SIZE, m_SpinDataFlashSize);
	DDX_Radio(pDX, IDC_RADIO_BOV_DISABLE, m_nRadioBov);
	DDX_Radio(pDX, IDC_RADIO_BS_LDROM, m_nRadioBS);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_0, m_sConfigValue0);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_1, m_sConfigValue1);
	DDX_Check(pDX, IDC_CHECK_CLOCK_FILTER_ENABLE, m_bClockFilterEnable);
	DDX_Check(pDX, IDC_CHECK_DATA_FLASH_ENABLE, m_bDataFlashEnable);
	DDX_Check(pDX, IDC_CHECK_SECURITY_LOCK, m_bSecurityLock);
	DDX_Text(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_sFlashBaseAddress);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_RESET, m_bCheckBrownOutReset);
	DDX_Radio(pDX, IDC_RADIO_IO_TRI, m_nRadioIO);
	DDX_Text(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_sDataFlashSize);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogConfiguration_Mini51CN, CDialog)
	//{{AFX_MSG_MAP(CDialogConfiguration_Mini51CN)
	ON_BN_CLICKED(IDC_RADIO_BOV_DISABLE, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM, OnRadioBs)
	ON_BN_CLICKED(IDC_CHECK_CLOCK_FILTER_ENABLE, OnCheckClick)
	//ON_EN_CHANGE(IDC_EDIT_FLASH_BASE_ADDRESS, OnChangeEditFlashBaseAddress)
	ON_EN_KILLFOCUS(IDC_EDIT_FLASH_BASE_ADDRESS, OnKillfocusEditFlashBaseAddress)
	ON_BN_CLICKED(IDC_RADIO_BOV_45, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_38, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_27, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BOV_22, OnRadioBov)
	ON_BN_CLICKED(IDC_RADIO_BS_APROM, OnRadioBs)
	ON_BN_CLICKED(IDC_CHECK_DATA_FLASH_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_SECURITY_LOCK, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_BROWN_OUT_RESET, OnCheckClick)
	ON_BN_CLICKED(IDC_RADIO_IO_TRI, OnRadioIO)
	ON_BN_CLICKED(IDC_RADIO_IO_BI, OnRadioIO)
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
// CDialogConfiguration_Mini51 message handlers

BOOL CDialogConfiguration_Mini51CN::OnInitDialog() 
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




void CDialogConfiguration_Mini51CN::ConfigToGUI()
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig1 = m_ConfigValue.m_value[1];
	unsigned int uFlashBaseAddress = uConfig1;

	if (m_bIsMini58)
	{
		switch(uConfig0 & (MINI51_FLASH_CONFIG_CBOD2VEN | MINI51_FLASH_CONFIG_CBOV))
		{
			case (MINI51_FLASH_CONFIG_CBOD2VEN | MINI51_FLASH_CONFIG_CBOV):
				m_nRadioBov = 0;
				break;
			case MINI51_FLASH_CONFIG_CBOV_44:
				m_nRadioBov = 1;
				break;
			case (MINI51_FLASH_CONFIG_CBOD2VEN |MINI51_FLASH_CONFIG_CBOV_38):
			case MINI51_FLASH_CONFIG_CBOV_38:
				m_nRadioBov = 2;
				break;
			case (MINI51_FLASH_CONFIG_CBOD2VEN | MINI51_FLASH_CONFIG_CBOV_27):
			case (MINI51_FLASH_CONFIG_CBOD2VEN | MINI51_FLASH_CONFIG_CBOV_27_):
			case MINI51_FLASH_CONFIG_CBOV_27:
				m_nRadioBov = 3;
				break;
			case MINI51_FLASH_CONFIG_CBOV_27_:
				m_nRadioBov = 4;
				break;
			default:
				m_nRadioBov = 0;
				break;
		}
	}
	else //if (m_uDID == 0x2600)
	{
		if(uConfig0 & MINI51_FLASH_CONFIG_CBOD2VEN)
		{
			m_nRadioBov=0;
		}
		else
		{
			switch(uConfig0 & MINI51_FLASH_CONFIG_CBOV)
			{
			//case MINI51_FLASH_CONFIG_CBOV_DISABLE:
			//	m_nRadioBov = 0; break;
			case MINI51_FLASH_CONFIG_CBOV_44:
				m_nRadioBov = 1; break;
			case MINI51_FLASH_CONFIG_CBOV_38:
				m_nRadioBov = 2; break;
			case MINI51_FLASH_CONFIG_CBOV_27:
				m_nRadioBov = 3; break;
			case MINI51_FLASH_CONFIG_CBOV_27_:
				m_nRadioBov = 4; break;
			default:
				m_nRadioBov = 2; break;
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
	default:
		m_nRadioBS = 3; break;
	}

	m_nRadioIO = ((uConfig0 & MINI51_FLASH_CONFIG_CIOINI) == 0 ? 1 : 0);

	m_bClockFilterEnable = ((uConfig0 & MINI51_FLASH_CONFIG_CKF) == MINI51_FLASH_CONFIG_CKF ? TRUE : FALSE);
	m_bCheckBrownOutReset = ((uConfig0 & MINI51_FLASH_CONFIG_CBORST) == 0 ? TRUE : FALSE);
	m_bDataFlashEnable = ((uConfig0 & MINI51_FLASH_CONFIG_DFEN) == 0 ? TRUE : FALSE);
	m_bSecurityLock = ((uConfig0 & MINI51_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);

	if(m_bDataFlashEnable)
	{
		uFlashBaseAddress = ((uFlashBaseAddress >= MINI51_FLASH_PAGE_SIZE) && (uFlashBaseAddress < m_uProgramMemorySize)) ? uFlashBaseAddress : (m_uProgramMemorySize - MINI51_FLASH_PAGE_SIZE);
		uFlashBaseAddress = (uFlashBaseAddress & MINI51_FLASH_CONFIG_DFBA) / MINI51_FLASH_PAGE_SIZE * MINI51_FLASH_PAGE_SIZE;
		uConfig1 = uFlashBaseAddress;
	}

	m_SpinDataFlashSize.EnableWindow(m_bDataFlashEnable);

	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable && (uFlashBaseAddress < m_uProgramMemorySize)) ? ((m_uProgramMemorySize - uFlashBaseAddress) / 1024.) : 0.);

	m_sConfigValue0.Format(_T("0x%08X"), uConfig0);
	m_sConfigValue1.Format(_T("0x%08X"), uConfig1);
}

void CDialogConfiguration_Mini51CN::GUIToConfig()
{
	//unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig0 = 0xFFFFFFFF;
	unsigned int uConfig1;

	uConfig0 &= ~MINI51_FLASH_CONFIG_CBOV;
	uConfig0 &= ~MINI51_FLASH_CONFIG_CBOD2VEN;
	switch(m_nRadioBov)
	{
	case 0:
		uConfig0 |= MINI51_FLASH_CONFIG_CBOV_DISABLE;
		uConfig0 |= MINI51_FLASH_CONFIG_CBOD2VEN;
		break;
	case 1:
		uConfig0 |= MINI51_FLASH_CONFIG_CBOV_44; break;
	case 2:
		uConfig0 |= MINI51_FLASH_CONFIG_CBOV_38; break;
	case 3:
		uConfig0 |= MINI51_FLASH_CONFIG_CBOV_27; break;
	case 4:
		uConfig0 |= MINI51_FLASH_CONFIG_CBOV_27_; break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & MINI51_FLASH_CONFIG_CBOV);
	}

	if(m_nRadioIO)
		uConfig0 &= ~MINI51_FLASH_CONFIG_CIOINI;
	else
		uConfig0 |= MINI51_FLASH_CONFIG_CIOINI;

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
	}

	if(m_bClockFilterEnable)
		uConfig0 |= MINI51_FLASH_CONFIG_CKF;
	else
		uConfig0 &= ~MINI51_FLASH_CONFIG_CKF;

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


void CDialogConfiguration_Mini51CN::OnRadioBov() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig();
	ConfigToGUI();

	UpdateData(FALSE);
}

void CDialogConfiguration_Mini51CN::OnRadioIO() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_Mini51CN::OnRadioBs() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

void CDialogConfiguration_Mini51CN::OnCheckClick() 
{
	// TODO: Add your control notification handler code here
	OnRadioBov();
}

//void CDialogConfiguration_Mini51CN::OnChangeEditFlashBaseAddress() 
void CDialogConfiguration_Mini51CN::OnKillfocusEditFlashBaseAddress()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	TCHAR *pEnd;
	unsigned int uFlashBaseAddress = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);

	if (m_bDataFlashEnable)
	{
		if (!((uFlashBaseAddress >= MINI51_FLASH_PAGE_SIZE) && (uFlashBaseAddress < m_uProgramMemorySize)))
			uFlashBaseAddress = m_uProgramMemorySize - MINI51_FLASH_PAGE_SIZE;

		uFlashBaseAddress &= ~(MINI51_FLASH_PAGE_SIZE - 1);

		m_sDataFlashSize.Format(_T("%.2fK"), (uFlashBaseAddress < m_uProgramMemorySize) ? ((m_uProgramMemorySize - uFlashBaseAddress) / 1024.) : 0.);
	}

	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);
	m_sConfigValue1.Format(_T("0x%08X"), uFlashBaseAddress);

	UpdateData(FALSE);
}

void CDialogConfiguration_Mini51CN::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	GUIToConfig();

	CDialog::OnOK();
}


CString CDialogConfiguration_Mini51CN::GetConfigWarning(const CAppConfig::Mini51_configs_t &config)
{
	CString str;
	unsigned int uConfig0 = config.m_value[0];

	BOOL bSecurityLock = ((uConfig0 & MINI51_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);
	if(!bSecurityLock)
		str += _T("   ") + _I(IDS_DISABLE_SECURITY_LOCK);

	return str;
}

void CDialogConfiguration_Mini51CN::OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	TCHAR *pEnd;
	unsigned int uFlashBaseAddress = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	if(pNMUpDown->iDelta == 1)
	{
		if((uFlashBaseAddress + MINI51_FLASH_PAGE_SIZE) < m_uProgramMemorySize)
			uFlashBaseAddress += MINI51_FLASH_PAGE_SIZE;
	}
	else if(pNMUpDown->iDelta == -1)
	{
		if(!(uFlashBaseAddress <= MINI51_FLASH_PAGE_SIZE))
			uFlashBaseAddress -= MINI51_FLASH_PAGE_SIZE;
	}
	uFlashBaseAddress = (uFlashBaseAddress & MINI51_FLASH_CONFIG_DFBA) / MINI51_FLASH_PAGE_SIZE * MINI51_FLASH_PAGE_SIZE;

	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable && (uFlashBaseAddress < m_uProgramMemorySize)) ? ((m_uProgramMemorySize - uFlashBaseAddress) / 1024.) : 0.);
	m_sConfigValue1.Format(_T("0x%08X"), uFlashBaseAddress);

	UpdateData(FALSE);

	*pResult = 0;
}

void CDialogConfiguration_Mini51CN::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar != NULL && pScrollBar->GetDlgCtrlID() == m_SpinDataFlashSize.GetDlgCtrlID())
		return;

	CDialogResize2::OnVScroll(nSBCode, nPos, pScrollBar);
}
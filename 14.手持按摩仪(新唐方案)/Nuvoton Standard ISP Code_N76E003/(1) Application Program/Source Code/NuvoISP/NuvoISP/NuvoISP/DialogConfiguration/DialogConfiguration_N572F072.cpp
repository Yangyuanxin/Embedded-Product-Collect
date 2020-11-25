// DialogConfiguration_N572F072.cpp : implementation file
//

 
#include "stdafx.h"
#include <deque>
#include <string>
#include <utility>
#include "Lang.h"
#include "ChipDefs.h"
#include "ChipDefs_N57x.h"
#include "NumEdit.h"
#include "AppConfig.h"
#include "DialogConfiguration_N572F072.h"


// CDialogConfiguration_N572F072 dialog

IMPLEMENT_DYNAMIC(CDialogConfiguration_N572F072, CDialog)

CDialogConfiguration_N572F072::CDialogConfiguration_N572F072(CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_N572F072::IDD, pParent)
	, m_uCoarseTimingCotrol(0)
	, m_uFineTimingControl(0)
	, m_nRadioCVDTV(0)
	, m_bWatchDogEnable(FALSE)
	, m_bProtect8K(FALSE)
	, m_bVoltageDetectorEnable(FALSE)
	, m_bSecurityLock(FALSE)
	, m_csConfigValue0(_T(""))
{

}

CDialogConfiguration_N572F072::~CDialogConfiguration_N572F072()
{
}

void CDialogConfiguration_N572F072::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_CSPI0_CT_072, m_uCoarseTimingCotrol);
	DDX_CBIndex(pDX, IDC_COMBO_CSPI0_FT_072, m_uFineTimingControl);
	DDX_Radio(pDX, IDC_RADIO_CVDTV_27_072, m_nRadioCVDTV);
	DDX_Check(pDX, IDC_CHECK_CWDTEN_072, m_bWatchDogEnable);
	DDX_Check(pDX, IDC_CHECK_PROTECTION_8K_072, m_bProtect8K);
	DDX_Check(pDX, IDC_CHECK_CVDEN_072, m_bVoltageDetectorEnable);
	DDX_Check(pDX, IDC_CHECK_LOCK_072, m_bSecurityLock);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_0_072, m_csConfigValue0);
}


BEGIN_MESSAGE_MAP(CDialogConfiguration_N572F072, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_CSPI0_CT_072,      &CDialogConfiguration_N572F072::OnChange)
	ON_CBN_SELCHANGE(IDC_COMBO_CSPI0_FT_072,      &CDialogConfiguration_N572F072::OnChange)
	ON_BN_CLICKED   (IDC_RADIO_CVDTV_27_072,      &CDialogConfiguration_N572F072::OnChange)
	ON_BN_CLICKED   (IDC_RADIO_CVDTV_30_072,      &CDialogConfiguration_N572F072::OnChange)
	ON_BN_CLICKED   (IDC_CHECK_CWDTEN_072,        &CDialogConfiguration_N572F072::OnChange)
	ON_BN_CLICKED   (IDC_CHECK_PROTECTION_8K_072, &CDialogConfiguration_N572F072::OnChange)
	ON_BN_CLICKED   (IDC_CHECK_CVDEN_072,         &CDialogConfiguration_N572F072::OnChange)
	ON_BN_CLICKED   (IDC_CHECK_LOCK_072,          &CDialogConfiguration_N572F072::OnChange)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CDialogConfiguration_N572F072 message handlers
BOOL CDialogConfiguration_N572F072::OnInitDialog()
{
	CDialog::OnInitDialog ();

	ConfigToGUI ();
	UpdateData (FALSE);

	m_bIsInitialized = true;
	GetWindowRect(m_rect);
	AdjustDPI();

	return TRUE;
}

void CDialogConfiguration_N572F072::OnOK ()
{
	UpdateData (TRUE);
	GUIToConfig ();
	CDialog::OnOK ();
}

void CDialogConfiguration_N572F072::OnChange()
{
	// TODO: Add your control notification handler code here
	UpdateData (TRUE);

	GUIToConfig ();
	ConfigToGUI ();

	UpdateData (FALSE);
}

void CDialogConfiguration_N572F072::ConfigToGUI ()
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];

	// Get CSPI0_CT
	// ------------------------------------------------------------------------
	m_uCoarseTimingCotrol = ((uConfig0&N572F072_FLASH_CONFIG_CSPI0_CT)>>6);
	// ------------------------------------------------------------------------

	// Get CSPI0_FT
	// ------------------------------------------------------------------------
	m_uFineTimingControl  = ((uConfig0&N572F072_FLASH_CONFIG_CSPI0_FT)>>4);
	// ------------------------------------------------------------------------

	// Get CVDTV
	// ------------------------------------------------------------------------
	m_nRadioCVDTV = (uConfig0&N572F072_FLASH_CONFIG_CVDTV) ? 1 : 0;
	// ------------------------------------------------------------------------

	// Get WatchDogEnable
	// ------------------------------------------------------------------------
	m_bWatchDogEnable = ((uConfig0&N572F072_FLASH_CONFIG_CWDTEN) == 0) ? FALSE : TRUE;
	// ------------------------------------------------------------------------

	// Get Protection On 8K Flash
	// ------------------------------------------------------------------------
	m_bProtect8K = ((uConfig0&N572F072_FLASH_CONFIG_PRTB) == 0) ? TRUE : FALSE;
	// ------------------------------------------------------------------------

	// Get Voltage Detector Enable
	// ------------------------------------------------------------------------
	m_bVoltageDetectorEnable = ((uConfig0&N572F072_FLASH_CONFIG_CVDEN) == 0) ? TRUE : FALSE;
	// ------------------------------------------------------------------------

	// Get Security Lock
	// ------------------------------------------------------------------------
	m_bSecurityLock = ((uConfig0&N572F072_FLASH_CONFIG_LOCK) == 0) ? TRUE : FALSE;
	// ------------------------------------------------------------------------

	m_csConfigValue0.Format (_T ("0x%08X"), m_ConfigValue.m_value[0]);
}

void CDialogConfiguration_N572F072::GUIToConfig ()
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];

	// Set CSPI0_CT
	// ------------------------------------------------------------------------
	uConfig0 &= ~N572F072_FLASH_CONFIG_CSPI0_CT;
	uConfig0 |= (m_uCoarseTimingCotrol<<6);
	// ------------------------------------------------------------------------

	// Get CSPI0_FT
	// ------------------------------------------------------------------------
	uConfig0 &= ~N572F072_FLASH_CONFIG_CSPI0_FT;
	uConfig0 |= (m_uFineTimingControl<<4);
	// ------------------------------------------------------------------------

	// Get CVDTV
	// ------------------------------------------------------------------------
	if(m_nRadioCVDTV == 0)
		uConfig0 &= ~N572F072_FLASH_CONFIG_CVDTV;
	else
		uConfig0 |= N572F072_FLASH_CONFIG_CVDTV;
	// ------------------------------------------------------------------------

	// Get WatchDogEnable
	// ------------------------------------------------------------------------
	if(m_bWatchDogEnable)
		uConfig0 |= N572F072_FLASH_CONFIG_CWDTEN;
	else
		uConfig0 &= ~N572F072_FLASH_CONFIG_CWDTEN;
	// ------------------------------------------------------------------------

	// Get Protection On 8K Flash
	// ------------------------------------------------------------------------
	if(m_bProtect8K)
		uConfig0 &= ~N572F072_FLASH_CONFIG_PRTB;
	else
		uConfig0 |= N572F072_FLASH_CONFIG_PRTB;
	// ------------------------------------------------------------------------

	// Get Voltage Detector Enable
	// ------------------------------------------------------------------------
	if(m_bVoltageDetectorEnable)
		uConfig0 &= ~N572F072_FLASH_CONFIG_CVDEN;
	else
		uConfig0 |= N572F072_FLASH_CONFIG_CVDEN;
	// ------------------------------------------------------------------------

	// Get Security Lock
	// ------------------------------------------------------------------------
	if(m_bSecurityLock)
		uConfig0 &= ~N572F072_FLASH_CONFIG_LOCK;
	else
		uConfig0 |= N572F072_FLASH_CONFIG_LOCK;
	// ------------------------------------------------------------------------

	m_ConfigValue.m_value[0] = uConfig0;
}

CString CDialogConfiguration_N572F072::GetConfigWarning(const CAppConfig::N572_configs_t &config)
{
	CString str;
	unsigned int uConfig0 = config.m_value[0];

	BOOL bSecurityLock = ((uConfig0 & N572_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);
	if(bSecurityLock)
		str += _I(IDS_ENABLE_SECURITY_LOCK);

	return str;
}

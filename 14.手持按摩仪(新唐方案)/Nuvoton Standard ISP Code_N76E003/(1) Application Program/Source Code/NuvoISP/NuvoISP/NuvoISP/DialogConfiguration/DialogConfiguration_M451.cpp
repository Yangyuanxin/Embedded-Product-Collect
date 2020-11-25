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
#include "DialogConfiguration_M451.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_M451 dialog


CDialogConfiguration_M451::CDialogConfiguration_M451(unsigned int uProgramMemorySize,
													 CWnd* pParent /*=NULL*/)
	: CDialogResize2(CDialogConfiguration_M451::IDD, pParent)
	, m_uProgramMemorySize(uProgramMemorySize)
{
	//{{AFX_DATA_INIT(CDialogConfiguration_M451)
	m_nRadioClk = -1;
	m_nRadioBov = -1;
	m_nRadioBS = -1;
	m_sConfigValue0 = _T("");
	m_sConfigValue1 = _T("");
	m_bCheckBrownOutDetect = FALSE;
	m_bCheckBrownOutReset = FALSE;
	m_bClockFilterEnable = FALSE;
	m_bDataFlashEnable = FALSE;
	m_bLDWRProtectEnable = FALSE;
	m_bSecurityLock = FALSE;
	m_bWDTEnable = FALSE;
	m_bWDTPowerDown = FALSE;
	m_nRadioGPG = -1;
	m_nRadioGPG32K = -1;
	m_nRadioRMIIEnable = -1;
	m_nRadioIO = -1;
	//uConfig2_Test=-1;
	m_sFlashBaseAddress = _T("");
	m_sFlashWriteProtect = _T("");
	//}}AFX_DATA_INIT
}


void CDialogConfiguration_M451::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize2::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfiguration_M451)
	DDX_Control(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_FlashBaseAddress);
//	DDX_Control(pDX, IDC_EDIT_FLASH_WR_PROTECT, m_FlashWriteProtect);
	DDX_Control(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_DataFlashSize);
	DDX_Control(pDX, IDC_SPIN_DATA_FLASH_SIZE, m_SpinDataFlashSize);
	DDX_Radio(pDX, IDC_RADIO_CLK_E12M, m_nRadioClk);
	DDX_Radio(pDX, IDC_RADIO_BOV_45, m_nRadioBov);
	DDX_Radio(pDX, IDC_RADIO_BS_LDROM, m_nRadioBS);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_0, m_sConfigValue0);
	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_1, m_sConfigValue1);
//	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_2, m_sConfigValue2);		//CY 2013/6/6
//	DDX_Text(pDX, IDC_STATIC_CONFIG_VALUE_3, m_sConfigValue3);		//CY 2013/6/7
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_DETECT, m_bCheckBrownOutDetect);
	DDX_Check(pDX, IDC_CHECK_BROWN_OUT_RESET, m_bCheckBrownOutReset);
//	DDX_Check(pDX, IDC_CHECK_CLOCK_FILTER_ENABLE, m_bClockFilterEnable);
	DDX_Check(pDX, IDC_CHECK_DATA_FLASH_ENABLE, m_bDataFlashEnable);
//	DDX_Check(pDX, IDC_CHECK_LDROM_WR_PROTECT, m_bLDWRProtectEnable);
	DDX_Check(pDX, IDC_CHECK_SECURITY_LOCK, m_bSecurityLock);
	DDX_Check(pDX, IDC_CHECK_WDT_ENABLE, m_bWDTEnable);
	DDX_Check(pDX, IDC_CHECK_WDT_POWER_DOWN, m_bWDTPowerDown);
	DDX_Radio(pDX, IDC_RADIO_GPF_GPIO, m_nRadioGPG);
//	DDX_Radio(pDX, IDC_RADIO_GPG2_GPIO, m_nRadioGPG32K);
//	DDX_Radio(pDX, IDC_RADIO_MII_MODE, m_nRadioRMIIEnable);
	DDX_Radio(pDX, IDC_RADIO_IO_TRI, m_nRadioIO);
	DDX_Text(pDX, IDC_EDIT_FLASH_BASE_ADDRESS, m_sFlashBaseAddress);
//	DDX_Text(pDX, IDC_EDIT_FLASH_WR_PROTECT, m_sFlashWriteProtect);
	DDX_Text(pDX, IDC_EDIT_DATA_FLASH_SIZE, m_sDataFlashSize);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogConfiguration_M451, CDialog)
	//{{AFX_MSG_MAP(CDialogConfiguration_M451)
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
	ON_BN_CLICKED(IDC_CHECK_LDROM_WR_PROTECT, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_SECURITY_LOCK, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_WATCHDOG_ENABLE, OnCheckClick)
	ON_BN_CLICKED(IDC_CHECK_WDT_POWER_DOWN, OnCheckClickWDTPD)
	ON_BN_CLICKED(IDC_CHECK_WDT_ENABLE, OnCheckClickWDT)
	ON_BN_CLICKED(IDC_RADIO_GPF_GPIO, OnRadioGpf)
	ON_BN_CLICKED(IDC_RADIO_GPF_CRYSTAL, OnRadioGpf)
	ON_BN_CLICKED(IDC_RADIO_IO_TRI, OnRadioIO)
	ON_BN_CLICKED(IDC_RADIO_IO_BI, OnRadioIO)
	ON_BN_CLICKED(IDC_RADIO_BS_LDROM_APROM, OnRadioBs)
	ON_BN_CLICKED(IDC_RADIO_BS_APROM_LDROM, OnRadioBs)
	ON_BN_CLICKED(IDC_RADIO_BS_MKROM, OnRadioBs)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DATA_FLASH_SIZE, OnDeltaposSpinDataFlashSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfiguration_M451 message handlers

BOOL CDialogConfiguration_M451::OnInitDialog() 
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




void CDialogConfiguration_M451::ConfigToGUI(int nEventID)
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig1 = m_ConfigValue.m_value[1];

	switch(uConfig0 & M451_FLASH_CONFIG_CFOSC)
	{
	case M451_FLASH_CONFIG_E12M:
		m_nRadioClk = 0; break;
	case M451_FLASH_CONFIG_I22M:
	default:
		m_nRadioClk = 1; break;
	}

	switch(uConfig0 & M451_FLASH_CONFIG_CBOV)
	{
	case M451_FLASH_CONFIG_CBOV_45:
		m_nRadioBov = 0; break;
	case M451_FLASH_CONFIG_CBOV_38:
		m_nRadioBov = 1; break;
	case M451_FLASH_CONFIG_CBOV_26:
		m_nRadioBov = 2; break;
	case M451_FLASH_CONFIG_CBOV_22:
	default:
		m_nRadioBov = 3; break;
	}

	switch(uConfig0 & M451_FLASH_CONFIG_CBS2)
	{
	case M451_FLASH_CONFIG_CBS_LD:
		m_nRadioBS = 0; break;
	case M451_FLASH_CONFIG_CBS_AP:
		m_nRadioBS = 1; break;
	case M451_FLASH_CONFIG_CBS_LD_AP:
		m_nRadioBS = 2; break;
	case M451_FLASH_CONFIG_CBS_AP_LD:
		m_nRadioBS = 3; break;
	case M451_FLASH_CONFIG_CBS_MK:
		m_nRadioBS = 4; break;
	default:
		m_nRadioBS = 1; break;
	}
	m_nRadioGPG = ((uConfig0 & M451_FLASH_CONFIG_CGPFMFP) == 0 ? 0 : 1);
	m_nRadioIO = ((uConfig0 & M451_FLASH_CONFIG_CIOINI) == 0 ? 1 : 0);
	m_bWDTPowerDown = ((uConfig0 & M451_FLASH_CONFIG_CWDTPDEN) == 0 ? TRUE : FALSE);
	m_bWDTEnable = ((uConfig0 & M451_FLASH_CONFIG_CWDTEN) == 0 ? TRUE : FALSE);;
	if(!m_bWDTEnable) m_bWDTPowerDown = FALSE;

	m_bCheckBrownOutDetect = ((uConfig0 & M451_FLASH_CONFIG_CBODEN) == 0 ? TRUE : FALSE);
	m_bCheckBrownOutReset = ((uConfig0 & M451_FLASH_CONFIG_CBORST) == 0 ? TRUE : FALSE);
	m_bDataFlashEnable = ((uConfig0 & M451_FLASH_CONFIG_DFEN) == 0 ? TRUE : FALSE);
	m_bSecurityLock = ((uConfig0 & M451_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);

	unsigned int uFlashBaseAddress = uConfig1;
	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);

	unsigned int uPageNum = uFlashBaseAddress / M451_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / M451_FLASH_PAGE_SIZE;
	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * M451_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);
	m_SpinDataFlashSize.EnableWindow(m_bDataFlashEnable ? TRUE : FALSE);

	//m_sFlashWriteProtect.Format(_T("%X"), uConfig2_Test);		//CY 2013/6/7
	//m_sFlashWriteProtect.Format(_T("%X"), m_ConfigValue.m_value[2]);		//CY 2013/6/7

	m_sConfigValue0.Format(_T("0x%08X"), uConfig0);
	m_sConfigValue1.Format(_T("0x%08X"), uConfig1);
	//m_sConfigValue2.Format(_T("0x%08X"), m_ConfigValue.m_value[2]);
	//m_sConfigValue3.Format(_T("0x%08X"), au32Config[3]);
	//m_sConfigValue3.Format(_T("0x%08X"), m_ConfigValue.m_value[3]);
}

void CDialogConfiguration_M451::GUIToConfig(int nEventID)
{
	unsigned int uConfig0 = m_ConfigValue.m_value[0];
	unsigned int uConfig1;
	//unsigned int uConfig2;

	uConfig0 &= ~M451_FLASH_CONFIG_CFOSC;
	switch(m_nRadioClk)
	{
	case 0:
		uConfig0 |= M451_FLASH_CONFIG_E12M; break;
	case 1:
		uConfig0 |= M451_FLASH_CONFIG_I22M;	break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & M451_FLASH_CONFIG_CFOSC);
	}

	uConfig0 &= ~M451_FLASH_CONFIG_CBOV;
	switch(m_nRadioBov)
	{
	case 0:
		uConfig0 |= M451_FLASH_CONFIG_CBOV_45; break;
	case 1:
		uConfig0 |= M451_FLASH_CONFIG_CBOV_38; break;
	case 2:
		uConfig0 |= M451_FLASH_CONFIG_CBOV_26; break;
	case 3:
		uConfig0 |= M451_FLASH_CONFIG_CBOV_22; break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & M451_FLASH_CONFIG_CBOV);
	}

	uConfig0 &= ~M451_FLASH_CONFIG_CBS2;
	switch(m_nRadioBS)
	{
	case 0:
		uConfig0 |= M451_FLASH_CONFIG_CBS_LD; break;
	case 1:
		uConfig0 |= M451_FLASH_CONFIG_CBS_AP; break;
	case 2:
		uConfig0 |= M451_FLASH_CONFIG_CBS_LD_AP; break;
	case 3:
		uConfig0 |= M451_FLASH_CONFIG_CBS_AP_LD; break;
	case 4:
		uConfig0 |= M451_FLASH_CONFIG_CBS_MK; break;
	default:
		/* Keep old value */
		uConfig0 |= (m_ConfigValue.m_value[0] & M451_FLASH_CONFIG_CBS2);
	}
	if(m_nRadioGPG == 0)
		uConfig0 &= ~M451_FLASH_CONFIG_CGPFMFP;
	else
		uConfig0 |= M451_FLASH_CONFIG_CGPFMFP;

	if(m_nRadioIO == 0)
		uConfig0 |= M451_FLASH_CONFIG_CIOINI;
	else
		uConfig0 &= ~M451_FLASH_CONFIG_CIOINI;

	if(m_bWDTPowerDown)
		uConfig0 &= ~M451_FLASH_CONFIG_CWDTPDEN;
	else
		uConfig0 |= M451_FLASH_CONFIG_CWDTPDEN;

	if(m_bWDTEnable)
	{
		uConfig0 &= ~M451_FLASH_CONFIG_CWDTEN;
		if(!m_bWDTPowerDown)
		{
			uConfig0 &= ~M451_FLASH_CONFIG_CWDTEN_BIT1;
			uConfig0 &= ~M451_FLASH_CONFIG_CWDTEN_BIT0;
		}
	}
	else
	{
		uConfig0 |= M451_FLASH_CONFIG_CWDTEN;
	}

	if(nEventID == IDC_CHECK_WDT_POWER_DOWN)
	{
		if(m_bWDTPowerDown)
		{
			uConfig0 &= ~M451_FLASH_CONFIG_CWDTEN;
			uConfig0 |= M451_FLASH_CONFIG_CWDTEN_BIT1;
			uConfig0 |= M451_FLASH_CONFIG_CWDTEN_BIT0;
		}
	}
	else
	{
		if(!m_bWDTEnable)
		{
			uConfig0 |= M451_FLASH_CONFIG_CWDTPDEN;
			uConfig0 |= M451_FLASH_CONFIG_CWDTEN_BIT1;
			uConfig0 |= M451_FLASH_CONFIG_CWDTEN_BIT0;
		}
	}

	if(m_bCheckBrownOutDetect)
		uConfig0 &= ~M451_FLASH_CONFIG_CBODEN;
	else
		uConfig0 |= M451_FLASH_CONFIG_CBODEN;

	if(m_bCheckBrownOutReset)
		uConfig0 &= ~M451_FLASH_CONFIG_CBORST;
	else
		uConfig0 |= M451_FLASH_CONFIG_CBORST;

	if(m_bDataFlashEnable)
		uConfig0 &= ~M451_FLASH_CONFIG_DFEN;
	else
		uConfig0 |= M451_FLASH_CONFIG_DFEN;

	if(m_bSecurityLock)
		uConfig0 &= ~M451_FLASH_CONFIG_LOCK;
	else
		uConfig0 |= M451_FLASH_CONFIG_LOCK;

	m_ConfigValue.m_value[0] = uConfig0;

	TCHAR *pEnd;
	uConfig1 = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	m_ConfigValue.m_value[1] = uConfig1;

	//uConfig2 = ::_tcstoul(m_sFlashWriteProtect, &pEnd, 16);
	//uConfig2_Test = uConfig2;
	//m_ConfigValue.m_value[2] = uConfig2;
}



void CDialogConfiguration_M451::OnGUIEvent(int nEventID) 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GUIToConfig(nEventID);
	//au32Config[3]=FMC_CRC8(au32Config,3);	//CY 2013/6/7
	//m_ConfigValue.m_value[3] = au32Config[3];
	ConfigToGUI(nEventID);

	UpdateData(FALSE);
}

void CDialogConfiguration_M451::OnRadioBov() 
{
	//OnGUIEvent();
	UpdateData(TRUE);

	GUIToConfig(0);
	//au32Config[3]=FMC_CRC8(au32Config,3);	//CY 2013/6/7
	//m_ConfigValue.m_value[3] = au32Config[3];
	ConfigToGUI(0);

	UpdateData(FALSE);
}

void CDialogConfiguration_M451::OnRadioClk() 
{
	// TODO: Add your control notification handler code here
	//OnGUIEvent();
	OnRadioBov();
}

void CDialogConfiguration_M451::OnRadioBs() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent();
}

void CDialogConfiguration_M451::OnRadioGpf() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent();
}

void CDialogConfiguration_M451::OnRadioIO() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent();
}

void CDialogConfiguration_M451::OnCheckClick() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent();
}

void CDialogConfiguration_M451::OnCheckClickWDTPD() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent(IDC_CHECK_WDT_POWER_DOWN);
}

void CDialogConfiguration_M451::OnCheckClickWDT() 
{
	// TODO: Add your control notification handler code here
	OnGUIEvent(IDC_CHECK_WDT_ENABLE);
}

void CDialogConfiguration_M451::OnChangeEditFlashBaseAddress() 
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

	unsigned int uPageNum = uFlashBaseAddress / M451_FLASH_PAGE_SIZE;
	unsigned int uLimitNum = m_uProgramMemorySize / M451_FLASH_PAGE_SIZE;
	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * M451_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);

	UpdateData(FALSE);
}


void CDialogConfiguration_M451::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	GUIToConfig(0);
	//au32Config[3]=FMC_CRC8(au32Config,3);	//CY 2013/6/7
	//m_ConfigValue.m_value[3] = au32Config[3];
	CDialog::OnOK();
}


CString CDialogConfiguration_M451::GetConfigWarning(const CAppConfig::M451_configs_t &config)
{
	CString str;
	unsigned int uConfig0 = config.m_value[0];

	switch(uConfig0 & M451_FLASH_CONFIG_CFOSC)
	{
	case M451_FLASH_CONFIG_E12M:
		str += _T("   ") + _I(IDS_SELECT_EXTERNAL_12M_CLOCK);
		break;
	default:
		;
	}

	BOOL bSecurityLock = ((uConfig0 & M451_FLASH_CONFIG_LOCK) == 0 ? TRUE : FALSE);
	if(!bSecurityLock)
		str += _T("   ") + _I(IDS_DISABLE_SECURITY_LOCK);

	return str;
}
unsigned int CDialogConfiguration_M451::FMC_CRC8(unsigned int au32Data[], unsigned int i32Count)
{
    int         i32ByteIdx;
    unsigned char     i, u8Cnt, u8InData;
    unsigned char    au8CRC[4] = { 0xff, 0xff, 0xff, 0xff };

    for (i32ByteIdx = 0; i32ByteIdx < 4; i32ByteIdx++)
    {
        for (u8Cnt = 0; u8Cnt < i32Count; u8Cnt++)
        {
            for (i = 0x80; i != 0; i /= 2)
            {
                if ((au8CRC[i32ByteIdx] & 0x80)!=0)
                {
                    au8CRC[i32ByteIdx] *= 2;
                    au8CRC[i32ByteIdx] ^= 7;
                }
                else
                    au8CRC[i32ByteIdx] *= 2;

                u8InData = (au32Data[u8Cnt] >> (i32ByteIdx * 8)) & 0xff;

                if ((u8InData & i) != 0)
                    au8CRC[i32ByteIdx]^=0x7;
            }
        }
    }
    return (au8CRC[0] | au8CRC[1] << 8 | au8CRC[2] << 16 | au8CRC[3] << 24);
}

void CDialogConfiguration_M451::OnDeltaposSpinDataFlashSize(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	TCHAR *pEnd;
	unsigned int uFlashBaseAddress = ::_tcstoul(m_sFlashBaseAddress, &pEnd, 16);
	unsigned int uPageNum = uFlashBaseAddress / M451_FLASH_PAGE_SIZE; //APROM
	unsigned int uLimitNum = m_uProgramMemorySize / M451_FLASH_PAGE_SIZE;

	if(pNMUpDown->iDelta == 1)
		uPageNum += 1;
	else if(pNMUpDown->iDelta == -1 && uPageNum > 0)
		uPageNum -= 1;

	uFlashBaseAddress = 0 + min(uPageNum, uLimitNum) * M451_FLASH_PAGE_SIZE;
	m_sFlashBaseAddress.Format(_T("%X"), uFlashBaseAddress);
	m_sConfigValue1.Format(_T("0x%08X"), uFlashBaseAddress);

	unsigned int uDataFlashSize = (uPageNum < uLimitNum) ? ((uLimitNum - uPageNum) * M451_FLASH_PAGE_SIZE) : 0;
	m_sDataFlashSize.Format(_T("%.2fK"), (m_bDataFlashEnable ? uDataFlashSize : 0) / 1024.);

	UpdateData(FALSE);
	*pResult = 0;
}

void CDialogConfiguration_M451::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar != NULL && pScrollBar->GetDlgCtrlID() == m_SpinDataFlashSize.GetDlgCtrlID())
		return;

	CDialogResize2::OnVScroll(nSBCode, nPos, pScrollBar);
}
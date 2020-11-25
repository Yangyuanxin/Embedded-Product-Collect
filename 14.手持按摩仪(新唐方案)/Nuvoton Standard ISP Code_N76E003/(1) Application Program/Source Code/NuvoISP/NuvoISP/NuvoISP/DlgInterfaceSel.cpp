// DlgInterfaceSel.cpp : implementation file
//

#include "stdafx.h"
#include <string>
#include <vector>
#include "Resource.h"
#include "DlgInterfaceSel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgInterfaceSel dialog


DlgInterfaceSel::DlgInterfaceSel(
	UINT nIDTemplate,
									 CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(DlgInterfaceSel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void DlgInterfaceSel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgInterfaceSel)
	DDX_Control(pDX, IDC_COMBO_COM_PORT, m_SelComPort);
	DDX_Control(pDX, IDC_COMBO_INTERFACE, m_SelInterface);
	DDX_Control(pDX, IDC_LINK_NUVOTON_M0, m_LinkNuvotonM0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgInterfaceSel, CDialog)
	//{{AFX_MSG_MAP(DlgInterfaceSel)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_INTERFACE, OnSelchangeComboLanguage)
	ON_BN_CLICKED(IDC_LINK_NUVOTON_M0, OnLinkNuvotonM0)
	ON_CBN_SELCHANGE(IDC_COMBO_COM_PORT, OnComboChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgInterfaceSel message handlers

int DlgInterfaceSel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

BOOL DlgInterfaceSel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	
	m_SelInterface.AddString(_T("USB"));
	m_SelInterface.AddString(_T("UART"));

	m_SelInterface.SetCurSel(0);
	OnSelchangeComboLanguage();

	if(ScanPCCom())
		m_SelComPort.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void DlgInterfaceSel::OnOK() 
{
	// TODO: Add extra validation here
	m_Interface = m_SelInterface.GetCurSel() + 1;
	m_SelComPort.GetWindowText(m_ComNum);

	if((m_Interface == 2) && (m_ComNum == _T("Scan Port")))
	{
		AfxMessageBox(_T("Please Select Active COM Port"));
		return;
	}

	CDialog::OnOK();
}


void DlgInterfaceSel::OnSelchangeComboLanguage() 
{
	// TODO: Add your control notification handler code here
	m_SelComPort.EnableWindow(m_SelInterface.GetCurSel() == 1);
	//OnOK();
}


void DlgInterfaceSel::OnLinkNuvotonM0() 
{
	// TODO: Add your control notification handler code here
	m_LinkNuvotonM0.VisitURL();
}

UINT DlgInterfaceSel::ScanPCCom()
{

	m_SelComPort.ResetContent();
	m_SelComPort.AddString(_T("Scan Port"));
    UINT nComNum = 0;
    HKEY hKEY;
    LONG hResult = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKEY);

    if(hResult != ERROR_SUCCESS) { //如果無法打開hKEY,則中止程式的執行
        //AfxMessageBox("錯誤：無法打開有關註冊表項");
        return  0 ;
    }

    TCHAR strInf[30];
    DWORD type_1 = REG_SZ;
    DWORD cbData_1 = 10;
    DWORD aa=30, num = 0, a1, a2, a3, a4, a5, a6, a7;

    hResult = ::RegQueryInfoKey(hKEY, strInf, &a7, NULL, &a3, &a1, &a2, &num, &a4, &a5, &a6, NULL);

    if(hResult != ERROR_SUCCESS) { //如果無法打開hKEY,則中止程式的執行
        //AfxMessageBox("錯誤：無法打開有關註冊表項");
        RegCloseKey(hKEY);
        return   0;
    }

    BYTE portName[30];
    CString csr;
    for(DWORD i = 0 ; i < num ; i++) {
        aa = 30 ;
        cbData_1 = 30;
        hResult = ::RegEnumValue(hKEY, i, strInf, &aa, NULL, &type_1, portName, &cbData_1);
        if((hResult != ERROR_SUCCESS)&&(hResult != ERROR_MORE_DATA)) { //如果無法打開hKEY,則中止程式的執行
            //AfxMessageBox("錯誤：無法獲取有關註冊表項");
            continue;
        }
        csr.Format(_T("%s"), portName);
        m_SelComPort.AddString(csr);
        nComNum++;

    }
    RegCloseKey(hKEY);
	m_SelComPort.SetCurSel(0);
    return nComNum;
}

void DlgInterfaceSel::OnComboChange()
{
	if(m_SelComPort.GetCurSel() == 0)
	{
		int portcnt = ScanPCCom();
		printf("Num Port = %d\n", portcnt);
	}
}

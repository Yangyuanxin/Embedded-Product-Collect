#include "stdafx.h"
#include "HyperLink.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "About.h"
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

CAboutDlg::CAboutDlg(const CString &sUpdateURL)
:	CDialog(CAboutDlg::IDD)
,	m_sUpdateURL(sUpdateURL)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_LINK_NUVOTON_M0, m_LinkNuvotonM0);
	DDX_Control(pDX, IDC_LINK_NUVOTON, m_LinkNuvoton);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_LINK_NUVOTON, OnLinkNuvoton)
	ON_BN_CLICKED(IDC_LINK_NUVOTON_M0, OnLinkNuvotonM0)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAboutDlg::OnLinkNuvoton() 
{
	// TODO: Add your control notification handler code here
	m_LinkNuvoton.VisitURL();
}


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_LinkNuvoton.SetWindowText(m_sUpdateURL);
	m_LinkNuvoton.SetAutoSize(TRUE);
	m_LinkNuvoton.SetURL(m_sUpdateURL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnLinkNuvotonM0() 
{
	// TODO: Add your control notification handler code here
	m_LinkNuvotonM0.VisitURL();
}

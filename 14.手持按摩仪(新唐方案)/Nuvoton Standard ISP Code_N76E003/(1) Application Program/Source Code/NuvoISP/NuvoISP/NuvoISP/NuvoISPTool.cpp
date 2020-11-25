
// ISPTool.cpp : 定義應用程式的類別行為。
//

#include "stdafx.h"
#include "NuvoISPTool.h"
#include "Lang.h"
#include "DlgInterfaceSel.h"


#include "DlgNuvoISP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CISPToolApp

BEGIN_MESSAGE_MAP(CISPToolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CISPToolApp 建構

CISPToolApp::CISPToolApp()
{
	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
}


// 僅有的一個 CISPToolApp 物件

CISPToolApp theApp;


// CISPToolApp 初始設定

BOOL CISPToolApp::InitInstance()
{
	CWinApp::InitInstance();

	AfxInitRichEdit();
	
	//AllocConsole();
	//SetConsoleTitle(_T("[Clientside dumper]: Console"));
	//freopen("CONOUT$", "w+t", stdout);
	//freopen("CONOUT$", "w+t", stderr);
	//freopen("CONIN$", "r+t", stdin);	

	// 建立殼層管理員，以防對話方塊包含
	// 任何殼層樹狀檢視或殼層清單檢視控制項。
	CShellManager *pShellManager = new CShellManager;

	// 標準初始設定
	// 如果您不使用這些功能並且想減少
	// 最後完成的可執行檔大小，您可以
	// 從下列程式碼移除不需要的初始化常式，
	// 變更儲存設定值的登錄機碼
	// TODO: 您應該適度修改此字串
	// (例如，公司名稱或組織名稱)
	SetRegistryKey(_T("本機 AppWizard 所產生的應用程式"));

	//CISPToolDlg dlg;
	//m_pMainWnd = &dlg;
	//INT_PTR nResponse = dlg.DoModal();
	//CDlg_NUC4xx dlg = CDlg_NUC4xx();
	//CNuvoISPDlg isp;
	//isp.DoModal();

	DlgInterfaceSel InterfaceDlg;
	INT_PTR nResponse = InterfaceDlg.DoModal();

	if (nResponse == IDOK)
	{
		unsigned int it = InterfaceDlg.m_Interface;
		CString str = InterfaceDlg.m_ComNum;

		//SetLangID(0x0404);
		AfxSetResourceHandle(m_hInstance);

		CNuvoISPDlg MainDlg;

		MainDlg.SetInterface(it, str);
		nResponse = MainDlg.DoModal();
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置於使用 [取消] 來停止使用對話方塊時
		// 處理的程式碼
	}

	// 刪除上面所建立的殼層管理員。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}
	//FreeConsole();
	// 因為已經關閉對話方塊，傳回 FALSE，所以我們會結束應用程式，
	// 而非提示開始應用程式的訊息。
	return FALSE;
}

void CISPToolApp::SetLangID(LANGID langID)
{
	//Free language dll;
	if(m_hLangResouce != NULL)
		FreeLibrary(m_hLangResouce);
	m_hLangResouce = NULL;

	if(langID == 0)
		langID = GetSystemDefaultLangID();

	//File name for language dll;
	TCHAR *pszLangFile = NULL;

	//Check the dll name 
	size_t szLangCount;
	const LANG_DEF_T *pLang = GetLangDefs(&szLangCount);
	for(size_t i = 0; i < szLangCount; ++i)
	{
		if(pLang[i].m_langID == langID)
		{
			pszLangFile = pLang[i].m_pszLangFile;
			break;
		}
	}

	//Load the language dll
	if(pszLangFile != NULL)
		m_hLangResouce = ::LoadLibrary(pszLangFile);

	if (m_hLangResouce != NULL)
		AfxSetResourceHandle(m_hLangResouce); // get resources from the DLL 
	else
		AfxSetResourceHandle(m_hInstance); // get resources from the DLL 
}
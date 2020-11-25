// DialogHex.cpp : implementation file
//

#include "stdafx.h"
#include "Lang.h"
#include "Resource.h"
#include "DialogHex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogHex dialog

/* DialogUpdateFW.h */
#define MSG_USER_EVENT				(WM_APP+1)
#define MSG_REFRESH_DATA			11
/* DialogMain.h */
#define MSG_REFRESH_DATA_EXT		21
/* Nu_Link.h */
template <class T>
inline T *vector_ptr(std::vector<T> &v)
{
	if(v.size() > 0)
		return &v[0];
	else
		return NULL;
}

template <class T>
inline const T *vector_ptr(const std::vector<T> &v)
{
	if(v.size() > 0)
		return &v[0];
	else
		return NULL;
}

CDialogHex::CDialogHex(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogHex::IDD, pParent)
	, m_pHexData(NULL)
	, m_sAltInfo()
	, m_uStartByte(0)
{
	//{{AFX_DATA_INIT(CDialogHex)
	m_nRadioUINT = -1;
	//}}AFX_DATA_INIT
}


void CDialogHex::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogHex)
	DDX_Control(pDX, IDC_EDIT_DATAVIEW, m_Edit_DataView);
	DDX_Control(pDX, IDC_RICHEDIT_DATAVIEW, m_REdit_DataView);
	DDX_Control(pDX, IDC_BUTTON_SAVE_AS, m_ButtonSaveAs);
	DDX_Radio(pDX, IDC_RADIO_UINT8, m_nRadioUINT);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogHex, CDialog)
	//{{AFX_MSG_MAP(CDialogHex)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RADIO_UINT8, OnRadioUint)
	ON_BN_CLICKED(IDC_RADIO_UINT16, OnRadioUint)
	ON_BN_CLICKED(IDC_RADIO_UINT32, OnRadioUint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogHex message handlers

BOOL CDialogHex::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	//m_ButtonRefresh.ShowWindow(!m_bEnableRefExt ? SW_SHOW : SW_HIDE);
	//m_ButtonRefreshExt.ShowWindow(m_bEnableRefExt ? SW_SHOW : SW_HIDE);

	//m_ButtonSaveAs.ShowWindow(m_bEnableSaveAs ? SW_SHOW : SW_HIDE);
#if 0
	if(m_Font1.CreateFont(11,0,0,0,0,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH|FF_MODERN,_T("Courier New")))
		m_REdit_DataView.SetFont(&m_Font1);
	else
#endif
	if(m_Font0.CreateFont(11,0,0,0,0,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH|FF_MODERN,_T("Lucida Console")))
		m_REdit_DataView.SetFont(&m_Font0);
	else
	if(m_Font2.CreateFont(11,0,0,0,0,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH|FF_MODERN,_T("FixedSys")))
		m_REdit_DataView.SetFont(&m_Font2);

	

	m_nRadioUINT = 0;	//8 bits hex viewer
	UpdateData(FALSE);

#if 0
	/* For test only */
	m_tmpdata_test.resize(rand() % 1200000);
	for(int i = 0; i < m_tmpdata_test.size(); ++i)
		m_tmpdata_test[i] = rand();
	m_pHexData = &m_tmpdata_test;
	OnRadioUint();
#endif
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogHex::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
	CRect rcClient;
	GetClientRect(rcClient);
	ClientToScreen(&rcClient);

	
	if(m_ButtonSaveAs.GetSafeHwnd())
	{
		/* Set button */
		CRect rcButton;
		m_ButtonSaveAs.GetWindowRect(rcButton);
	
		rcButton.left += (rcClient.right - 12 - rcButton.right);
		rcButton.right = rcClient.right - 12;
		rcButton.top += (rcClient.bottom - 2 - rcButton.bottom);
		rcButton.bottom = rcClient.bottom - 2;

		ScreenToClient(rcButton);

		LONG lHeight = rcButton.bottom - rcButton.top;
		rcButton.top -= lHeight + 3;
		rcButton.bottom -= lHeight + 3;
		m_ButtonSaveAs.MoveWindow(rcButton);

		/* Set label */
		/* Set edit area */
		CRect rcEdit;
		rcEdit.left = rcClient.left + 0;
		rcEdit.right = rcButton.left - 12;
		rcEdit.top = rcClient.top + 1;
		rcEdit.bottom = rcClient.bottom - 0;
		ScreenToClient(rcEdit);
		m_REdit_DataView.MoveWindow(rcEdit);
		//m_Edit_DataView.MoveWindow(rcEdit);

		/* Set radio box */
		int nRadioIDs[] =
		{
			IDC_RADIO_UINT8,
			IDC_RADIO_UINT16,
			IDC_RADIO_UINT32
		};

		for(int i = 0; i < sizeof(nRadioIDs) / sizeof(nRadioIDs[0]); ++i)
		{
			CButton *pRadio = (CButton *)GetDlgItem(nRadioIDs[i]);
			if(pRadio != NULL && pRadio->GetSafeHwnd())
			{
				CRect rcRadio;
				pRadio->GetWindowRect(rcRadio);
				int nHeight = rcRadio.Height();

				rcRadio.right += rcButton.left - 1 - rcRadio.left;
				rcRadio.left = rcButton.left - 1;
				rcRadio.bottom += (rcClient.top + 6 + i * (nHeight + 4) - rcRadio.top);
				rcRadio.top = rcClient.top + 6 + i * (nHeight + 4);
				ScreenToClient(&rcRadio);

				pRadio->MoveWindow(rcRadio);
			}
		}

	}

}

void CDialogHex::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CDialogHex::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
}

void CDialogHex::SetHexData(const std::vector<unsigned char> *pHexData,
							const CString &sAltInfo)
{
	m_pHexData = pHexData;
	m_sAltInfo = sAltInfo;
	m_uStartByte = 0;
	OnRadioUint();
	//UpdateRefData();
}

void CDialogHex::SetHexData(const std::vector<unsigned char> *pHexData,
							unsigned int uStartByte,
							const CString &sAltInfo)
{
	m_pHexData = pHexData;
	m_sAltInfo = sAltInfo;
	m_uStartByte = uStartByte;
	OnRadioUint();
	//UpdateRefData();
}

void CDialogHex::UpdateHexView(int nWidth)
{
	if(m_pHexData == NULL || m_pHexData->size() == 0)
	{
		m_REdit_DataView.SetWindowText(m_sAltInfo);
		return;
	}

	int nMaxLineLength = 48;
	int nLines;
	int nWords_InLine;
	int nBytes_InLine = 16;
	int nBufSize_Line;
	int nBufSize_Total;
	int nWordWidth = nWidth * 2;

#if 1
	/* Use nBytes_InLine to decide other parameters */
	nWords_InLine = nBytes_InLine / nWidth;
	nBytes_InLine = nWidth * nWords_InLine;
	nMaxLineLength = nWords_InLine * (nWordWidth + 1);
	nLines = (m_pHexData->size() + nBytes_InLine - 1) / nBytes_InLine;

	nBufSize_Line = 11/*Address*/
		+ (nWordWidth + 1/*Blank*/) * nWords_InLine + 1/*\r*/;
	nBufSize_Total = nBufSize_Line * nLines;
#else
	/* Use nMaxLineLength to decide other parameters */
	nWords_InLine = (nMaxLineLength + 1) / (nWordWidth + 1);
	nBytes_InLine =  nWidth * nWords_InLine;
	nLines = (m_pHexData->size() + nBytes_InLine - 1) / nBytes_InLine;
	
	nBufSize_Line = 11/*Address*/
		+ (nWordWidth + 1/*Blank*/) * nWords_InLine + 1/*\r*/;
	nBufSize_Total = nBufSize_Line * nLines;
#endif

	std::vector<TCHAR> str;
	str.resize(nBufSize_Total + 1);

	if(nWidth == 1)
	{
		TCHAR *pLine = vector_ptr(str);
		//const TCHAR *pEnd = vector_ptr(str) + str.size();
		const unsigned char *pData = vector_ptr((*m_pHexData));
		const unsigned char *pDataEnd = (const unsigned char *)(vector_ptr((*m_pHexData))
			+ m_pHexData->size());
		pDataEnd = (const unsigned char *)((unsigned int)pDataEnd / nWidth * nWidth);

		for(int j = 0; j < nLines; ++j)
		{
			TCHAR *pPos = pLine;

			if(pData < pDataEnd)
				pPos += swprintf_s(pPos, 12/*pEnd - pPos*/, _T("%08X:  "), m_uStartByte + j * nBytes_InLine);

			for(int i = 0; i < nWords_InLine && pData < pDataEnd; ++i)
			{
				if(i + 1 < nWords_InLine)
					pPos += swprintf_s(pPos, 4/*pEnd - pPos*/, _T("%02X "), (unsigned int)*pData);
				else
					pPos += swprintf_s(pPos, 5/*pEnd - pPos*/, _T("%02X\r\n"), (unsigned int)*pData);
				++pData;
			}
			pLine += nBufSize_Line;
		}
	}
	else if(nWidth == 2)
	{
		TCHAR *pLine = vector_ptr(str);
		//const TCHAR *pEnd = vector_ptr(str) + str.size();
		const unsigned short *pData = (const unsigned short *)vector_ptr((*m_pHexData));
		const unsigned short *pDataEnd = (const unsigned short *)(vector_ptr((*m_pHexData))
			+ m_pHexData->size());
		
		for(int j = 0; j < nLines; ++j)
		{
			TCHAR *pPos = pLine;

			if(pData < pDataEnd)
				pPos += swprintf_s(pPos, 12/*pEnd - pPos*/, _T("%08X:  "), m_uStartByte + j * nBytes_InLine);

			for(int i = 0; i < nWords_InLine && pData < pDataEnd; ++i)
			{
				unsigned int uValue = 0;
				if(pData + 1 <= pDataEnd)
					uValue = (unsigned int)*pData;
				else
					memcpy(&uValue, pData, (unsigned char *)pDataEnd - (unsigned char *)pData);

				if(i + 1 < nWords_InLine)
					pPos += swprintf_s(pPos, 6/*pEnd - pPos*/, _T("%04X "), uValue);
				else
					pPos += swprintf_s(pPos, 7/*pEnd - pPos*/, _T("%04X\r\n"), uValue);
				++pData;
			}
			pLine += nBufSize_Line;
		}
	}
	else //if(nWidth == 4)
	{
		TCHAR *pLine = vector_ptr(str);
		//const TCHAR *pEnd = vector_ptr(str) + str.size();
		const unsigned int *pData = (const unsigned int *)vector_ptr((*m_pHexData));
		const unsigned int *pDataEnd = (const unsigned int *)(vector_ptr((*m_pHexData))
			+ m_pHexData->size());
		
		for(int j = 0; j < nLines; ++j)
		{
			TCHAR *pPos = pLine;

			if(pData < pDataEnd)
				pPos += swprintf_s(pPos, 12/*pEnd - pPos*/, _T("%08X:  "), m_uStartByte + j * nBytes_InLine);

			for(int i = 0;
				i < nWords_InLine && pData < pDataEnd;
				++i)
			{
				unsigned int uValue = 0;
				if(pData + 1 <= pDataEnd)
					uValue = (unsigned int)*pData;
				else
					memcpy(&uValue, pData, (unsigned char *)pDataEnd - (unsigned char *)pData);

				if(i + 1 < nWords_InLine)
					pPos += swprintf_s(pPos, 10/*pEnd - pPos*/, _T("%08X "), (unsigned int)*pData);
				else
					pPos += swprintf_s(pPos, 11/*pEnd - pPos*/, _T("%08X\r\n"), (unsigned int)*pData);
				++pData;
			}
			pLine += nBufSize_Line;
		}
	}
	

	m_REdit_DataView.SetWindowText(vector_ptr(str));
	//m_Edit_DataView.SetWindowText(vector_ptr(str));
}

BOOL CDialogHex::EnableWindow(BOOL bEnable)
{
	int nRadioIDs[] =
	{
		IDC_RADIO_UINT8,
		IDC_RADIO_UINT16,
		IDC_RADIO_UINT32
	};
	for(int i = 0; i < sizeof(nRadioIDs) / sizeof(nRadioIDs[0]); ++i)
	{
		CButton *pRadio = (CButton *)GetDlgItem(nRadioIDs[i]);
		if(pRadio != NULL && pRadio->GetSafeHwnd())
			pRadio->EnableWindow(bEnable);
	}

	m_REdit_DataView.EnableWindow(bEnable);

	return CDialog::EnableWindow(bEnable);
}

void CDialogHex::OnRadioUint() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	switch(m_nRadioUINT)
	{
	case 0:
		UpdateHexView(1);
		break;
	case 1:
		UpdateHexView(2);
		break;
	case 2:
	default:
		UpdateHexView(4);
	}
}

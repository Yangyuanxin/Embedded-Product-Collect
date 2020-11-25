#include "StdAfx.h"
#include "Resource.h"
#include "Lang.h"

CString LoadStringFromID(UINT uID, TCHAR *szID)
{
	CString str;
	if(str.LoadString(uID) == FALSE)
		return szID;
	return str;
}


const LANG_DEF_T m_sLanguages[] =
{
	//LANG_ENGLISH
	{0x0c09, NULL, IDS_LANG_ENGLISH},
	//LANG_CHINESE_SIMPLIFIED
	{0x0804, _T("lan_zhcn.dll"), IDS_LANG_SIMPLIFIED_CHINESE},
	//LANG_CHINESE_TRADITIONAL
	{0x0404, _T("lan_zhtw.dll"), IDS_LANG_TRADITIONAL_CHINESE},
};

const LANG_DEF_T *GetLangDefs(size_t *pCount)
{
	if(pCount != NULL)
		*pCount = sizeof(m_sLanguages) / sizeof(m_sLanguages[0]);
	return m_sLanguages;
}

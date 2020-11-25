#ifndef INC__LANG_H__
#define INC__LANG_H__

extern CString LoadStringFromID(UINT uID, TCHAR *szID);

#define _I(uID)	LoadStringFromID(uID, _T(#uID))

typedef struct
{
	LANGID	m_langID;
	TCHAR	*m_pszLangFile;
	UINT	m_uNameID;
} LANG_DEF_T;


const LANG_DEF_T *GetLangDefs(size_t *pCount);


#endif

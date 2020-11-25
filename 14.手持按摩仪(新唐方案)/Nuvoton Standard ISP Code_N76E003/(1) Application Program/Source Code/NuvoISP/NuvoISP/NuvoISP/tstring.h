#ifndef INC__TSTRING_H__
#define INC__TSTRING_H__

#include <windows.h>

#ifdef _UNICODE
#include <xstring>
#else
#include <string>
#endif

namespace std
{
#if 0
	typedef basic_string<
		TCHAR,
		char_traits<TCHAR>,
		allocator<TCHAR> >
		tstring;
#endif

#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}


inline size_t asc_to_ucs2(
	wchar_t *ucs2,
	const char *asc,
	size_t count )
{
	return MultiByteToWideChar( CP_ACP,
		0,
		asc, -1, ucs2, ( ucs2 == NULL ? 0 : count ) );
}

inline size_t ucs2_to_asc(
	char *asc,
	const wchar_t *ucs2,
	size_t count )
{
	//printf( "In ucs2_to_utf8 unicode: %x %d\n", utf8, count );
	return WideCharToMultiByte( CP_ACP,
		0,
		ucs2, -1, asc, ( asc == NULL ? 0 : count ), NULL, NULL );
}

inline size_t asc_to_tchar(
	TCHAR *tch,
	const char *asc,
	size_t count)
{
#ifdef _UNICODE
	return asc_to_ucs2(tch, asc, count);
#else
	_snprintf(tch, count, "%s", asc);
	return count;
#endif
}

inline size_t tchar_to_asc(
	char *asc,
	const TCHAR *tch,
	size_t count)
{
#ifdef _UNICODE
	return ucs2_to_asc(asc, tch, count);
#else
	_snprintf(asc, count, "%s", tch);
	return count;
#endif
}

#endif


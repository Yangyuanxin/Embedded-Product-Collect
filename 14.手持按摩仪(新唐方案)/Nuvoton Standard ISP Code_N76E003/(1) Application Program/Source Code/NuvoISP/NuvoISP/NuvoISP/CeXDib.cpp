#include "stdafx.h"
#include "CeXDib.h"

#ifndef _MFC_VER
#include <windows.h>
#include <tchar.h>
#pragma message("    compiling for Win32")
#endif

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

CCeXDib::CCeXDib()
{
	m_hDib = NULL;
	m_dwLineWidth = 0;
	m_wColors = 0;

	m_hMemDC = NULL;
	m_hBitmap = NULL;
	m_lpBits = NULL;

	FreeResources();
}

CCeXDib::~CCeXDib()
{
	FreeResources();
}

void CCeXDib::FreeResources()
{
	if (m_hMemDC)	
		::DeleteDC(m_hMemDC);
	if (m_hBitmap)	
		::DeleteObject(m_hBitmap);
	if (m_hDib)		
		delete m_hDib;

	m_hDib = NULL;
	m_hMemDC = NULL;
	m_hBitmap = NULL;
	m_lpBits = NULL;
	memset(&m_bi, 0, sizeof(m_bi));
} // End of FreeResources

HDIB CCeXDib::Create(DWORD dwWidth, DWORD dwHeight, WORD wBitCount)
{
    LPBITMAPINFOHEADER  lpbi = NULL;	// Pointer to BITMAPINFOHEADER
    DWORD               dwLen = 0;		// Size of memory block

	FreeResources();

	// Following <switch> is taken from
	// CDIBSectionLite class by Chris Maunder
    switch (wBitCount) 
    {
	    case 1:  m_wColors = 2;   break;
#ifdef _WIN32_WCE
        case 2:  m_wColors = 4;   break;   // winCE only       
#endif
        case 4:  m_wColors = 16;  break;
        case 8:  m_wColors = 256; break;
        case 16:
        case 24:
        case 32: m_wColors = 0;   break;   // 16,24 or 32 bpp have no color table

        default:
           m_wColors = 0;
    } // switch
/*
    // Make sure bits per pixel is valid
    if (wBitCount <= 1)			wBitCount = 1;
    else if (wBitCount <= 4)	wBitCount = 4;
    else if (wBitCount <= 8)	wBitCount = 8;
    else				        wBitCount = 24;

    switch (wBitCount)
	{
        case 1:
            m_wColors = 2;
			break;
        case 4:
            m_wColors = 16;
			break;
        case 8:
            m_wColors = 256;
			break;
        default:
            m_wColors = 0;
			break;
    } // switch
*/
    m_dwLineWidth = WIDTHBYTES(wBitCount * dwWidth);

    // Initialize BITMAPINFOHEADER
    m_bi.biSize = sizeof(BITMAPINFOHEADER);
    m_bi.biWidth = dwWidth;         // fill in width from parameter
    m_bi.biHeight = dwHeight;       // fill in height from parameter
    m_bi.biPlanes = 1;              // must be 1
    m_bi.biBitCount = wBitCount;    // from parameter
    m_bi.biCompression = BI_RGB;    
    m_bi.biSizeImage = m_dwLineWidth * dwHeight;
    m_bi.biXPelsPerMeter = 0;
    m_bi.biYPelsPerMeter = 0;
    m_bi.biClrUsed = 0;
    m_bi.biClrImportant = 0;

    // Calculate size of memory block required to store the DIB.  This
    // block should be big enough to hold the BITMAPINFOHEADER, the color
    // table, and the bits.
    dwLen = GetSize();

	m_hDib = new HDIB[dwLen]; // Allocate memory block to store our bitmap
    if (m_hDib == NULL) return NULL;

    // Use our bitmap info structure to fill in first part of
    // our DIB with the BITMAPINFOHEADER
	lpbi = (LPBITMAPINFOHEADER)(m_hDib);
    *lpbi = m_bi;

    return m_hDib; // Return handle to the DIB
} // End of Create

DWORD CCeXDib::GetSize()
{
	return m_bi.biSize + m_bi.biSizeImage + GetPaletteSize();
} // End of GetSize

DWORD CCeXDib::GetPaletteSize()
{
	return (m_wColors * sizeof(RGBQUAD));
} // End of GetPaletteSize

LPBYTE CCeXDib::GetBits()
{
	if (m_hDib)	
		return ((LPBYTE)m_hDib + *(LPDWORD)m_hDib + GetPaletteSize()); 

	return NULL;
} // End of GetBits

DWORD CCeXDib::GetWidth()
{
	return m_bi.biWidth;
} // End of GetWidth

DWORD CCeXDib::GetHeight()
{
	return m_bi.biHeight;
} // End of GetHeight

DWORD CCeXDib::GetLineWidth()
{
	return m_dwLineWidth;
} // End of GetLineWidth

void CCeXDib::BlendPalette(COLORREF crColor, DWORD dwPerc)
{
	if (m_hDib == NULL || m_wColors == 0) 
		return;

	LPBYTE iDst = (LPBYTE)(m_hDib) + sizeof(BITMAPINFOHEADER);

	long i,r,g,b;

	RGBQUAD* pPal = (RGBQUAD*)iDst;

	r = GetRValue(crColor);
	g = GetGValue(crColor);
	b = GetBValue(crColor);

	if (dwPerc > 100) 
		dwPerc = 100;

	for (i = 0; i < m_wColors; i++)
	{
		pPal[i].rgbBlue = (BYTE)((pPal[i].rgbBlue * (100 - dwPerc) + b * dwPerc) / 100);
		pPal[i].rgbGreen = (BYTE)((pPal[i].rgbGreen * (100 - dwPerc) + g * dwPerc) / 100);
		pPal[i].rgbRed = (BYTE)((pPal[i].rgbRed * (100 - dwPerc) + r * dwPerc) / 100);
	} // for
} // End of BlendPalette

void CCeXDib::Clear(BYTE byVal)
{
	if (m_hDib) 
		memset(GetBits(), byVal, m_bi.biSizeImage);
} // End of Clear

void CCeXDib::SetPixelIndex(DWORD dwX, DWORD dwY, BYTE byI)
{
	if ((m_hDib == NULL) || (m_wColors == 0) ||
		((long)dwX < 0) || ((long)dwY < 0) || (dwX >= (DWORD)m_bi.biWidth) || (dwY >= (DWORD)m_bi.biHeight)) return;

	LPBYTE iDst = GetBits();
	iDst[(m_bi.biHeight - dwY - 1) * m_dwLineWidth + dwX] = byI;
} // End of SetPixelIndex

void CCeXDib::Clone(CCeXDib* src)
{
	Create(src->GetWidth(), src->GetHeight(), src->GetBitCount());
	if (m_hDib) 
		memcpy(m_hDib, src->m_hDib, GetSize());
} // End of Clone

WORD CCeXDib::GetBitCount()
{
	return m_bi.biBitCount;
} // End of GetBitCount

void CCeXDib::SetPaletteIndex(BYTE byIdx, BYTE byR, BYTE byG, BYTE byB)
{
	if (m_hDib && m_wColors)
	{
		LPBYTE iDst = (LPBYTE)(m_hDib) + sizeof(BITMAPINFOHEADER);
		if ((byIdx >= 0) && (byIdx < m_wColors))
		{	
			long ldx = byIdx * sizeof(RGBQUAD);
			iDst[ldx++] = (BYTE)byB;
			iDst[ldx++] = (BYTE)byG;
			iDst[ldx++] = (BYTE)byR;
			iDst[ldx] = (BYTE)0;
		} // if
	} // if
} // End of SetPaletteIndex

void CCeXDib::Draw(HDC hDC, DWORD dwX, DWORD dwY)
{
	HBITMAP	hBitmap = NULL;
	HBITMAP	hOldBitmap = NULL;
	HDC		hMemDC = NULL;

	if (m_hBitmap == NULL)
	{
		m_hBitmap = CreateDIBSection(hDC, (BITMAPINFO*)m_hDib, DIB_RGB_COLORS, &m_lpBits, NULL, 0);
		if (m_hBitmap == NULL)	return;
		if (m_lpBits == NULL)
		{
			::DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
			return;
		} // if
	} // if

    memcpy(m_lpBits, GetBits(), m_bi.biSizeImage);

	if (m_hMemDC == NULL)
	{
		m_hMemDC = CreateCompatibleDC(hDC);
		if (m_hMemDC == NULL)	return;
	} // if

	hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);

	BitBlt(hDC, dwX, dwY, m_bi.biWidth, m_bi.biHeight, m_hMemDC, 0, 0, SRCCOPY);

	SelectObject(m_hMemDC, hOldBitmap);
} // End of Draw

void CCeXDib::Copy(HDC hDC, DWORD dwX, DWORD dwY)
{
	if (m_hBitmap == NULL)
	{
		m_hBitmap = CreateDIBSection(hDC, (BITMAPINFO*)m_hDib, DIB_RGB_COLORS, &m_lpBits, NULL, 0);
		if (m_hBitmap == NULL)
			return;
		if (m_lpBits == NULL)
		{
			::DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
			return;
		} // if
	} // if
	
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, m_hBitmap);
	::BitBlt(hMemDC, 0, 0, m_bi.biWidth, m_bi.biHeight, hDC, dwX, dwY, SRCCOPY);
	::SelectObject(hMemDC, hOldBitmap);
} // End of Copy

void CCeXDib::SetGrayPalette()
{
	RGBQUAD		pal[256];
	RGBQUAD*	ppal;
	LPBYTE		iDst;
	int			ni;

	if (m_hDib == NULL || m_wColors == 0) return;

	ppal = (RGBQUAD*)&pal[0];
	iDst = (LPBYTE)(m_hDib) + sizeof(BITMAPINFOHEADER);
	for (ni = 0; ni < m_wColors; ni++)
	{
		pal[ni] = RGB2RGBQUAD(RGB(ni,ni,ni));
	} // for

	pal[0] = RGB2RGBQUAD(RGB(0,0,0));
	pal[m_wColors-1] = RGB2RGBQUAD(RGB(255,255,255));

	memcpy(iDst, ppal, GetPaletteSize());
} // End of SetGrayPalette

RGBQUAD CCeXDib::RGB2RGBQUAD(COLORREF cr)
{
	RGBQUAD c;
	c.rgbRed = GetRValue(cr);	/* get R, G, and B out of DWORD */
	c.rgbGreen = GetGValue(cr);
	c.rgbBlue = GetBValue(cr);
	c.rgbReserved=0;
	return c;
} // End of RGB2RGBQUAD

WORD CCeXDib::GetNumColors()
{
	return m_wColors;
} // End of GetNumColors

BOOL CCeXDib::WriteBMP(LPCTSTR bmpFileName)
{
	BITMAPFILEHEADER	hdr;
	HANDLE	hFile;
	DWORD	nByteWrite;

	if (*bmpFileName == _T('\0') || m_hDib == 0) return 0;

	hFile=CreateFile(			// open if exist ini file
		bmpFileName,			// pointer to name of the file 
		GENERIC_WRITE,			// access mode 
		0,						// share mode 
		NULL,					// pointer to security descriptor 
		CREATE_ALWAYS,			// how to create 
		FILE_ATTRIBUTE_NORMAL,	// file attributes 
		NULL				 	// handle to file with attributes to copy  
		);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;

    // Fill in the fields of the file header
	hdr.bfType = BFT_BITMAP;
	hdr.bfSize = GetSize() + sizeof(BITMAPFILEHEADER);
	hdr.bfReserved1 = hdr.bfReserved2 = 0;
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER)+
					m_bi.biSize + GetPaletteSize();

    // Write the file header
	WriteFile(						// write ini (sync mode <-> no overlapped)
		hFile,						// handle of file to write 
		(LPSTR) &hdr,				// address of buffer that contains data  
		sizeof(BITMAPFILEHEADER),	// number of bytes to write 
		&nByteWrite,				// address of number of bytes written 
		NULL	 					// address of structure for data 
		);

    // Write the DIB header and the bits
	WriteFile(						// write ini (sync mode <-> no overlapped)
		hFile,						// handle of file to write 
		(LPSTR) m_hDib,				// address of buffer that contains data  
		GetSize(),					// number of bytes to write 
		&nByteWrite,				// address of number of bytes written 
		NULL	 					// address of structure for data 
		);

	CloseHandle(hFile);				// free file handle

	return TRUE;
} // End of WriteBMP

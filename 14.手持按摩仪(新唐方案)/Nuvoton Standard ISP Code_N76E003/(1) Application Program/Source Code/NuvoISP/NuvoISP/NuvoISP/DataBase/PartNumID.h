// PartNumID.h: interface for the CPartNum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTNUMID_H__224714FF_DE1A_41FC_81B6_4B998BDC9FE6__INCLUDED_)
#define AFX_PARTNUMID_H__224714FF_DE1A_41FC_81B6_4B998BDC9FE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <sstream>


struct CPartNumID
{
	char szPartNumber[32];
	unsigned int uID;
	unsigned int uProjectCode;
};

bool QueryDataBase(unsigned int uID);
std::string GetPartNumber(unsigned int uID);
bool UpdateSizeInfo(unsigned int uID, unsigned int uConfig0, unsigned int uConfig1,
                unsigned int *puAPROM_Size,
                unsigned int *puNVM_Addr, unsigned int *puNVM_Size);

#endif // !defined(AFX_PARTNUMID_H__224714FF_DE1A_41FC_81B6_4B998BDC9FE6__INCLUDED_)


#if !defined(_FILE_INFO_H_)
#define _FILE_INFO_H_

#include <sys/stat.h>
#include "tstring.h"
#include <fstream>
#include <vector>


struct fileinfo {
	//std::tstring& filename;
	CString filename;
	size_t st_size;
	unsigned short usCheckSum;
	std::vector<unsigned char>	vbuf;
	fileinfo() :filename(_T("")), st_size(0), usCheckSum(0)
	{

	}
};

size_t getFilesize(const std::tstring& filename);
bool UpdateFileInfo(CString strFN, struct fileinfo* sfinfo);

size_t mfwrite(const void *ptr, size_t len, _TCHAR *_FileName);
size_t mfread(void *ptr, size_t len, _TCHAR *_FileName);

#endif // _FILE_INFO_H_
#include "StdAfx.h"
#include "fileinfo.h"
#include <sys/stat.h>

BOOL IsFolder2(const CString& path)   
{   
    WIN32_FIND_DATA fd;   
    BOOL ret = FALSE;   
    HANDLE hFind = FindFirstFile(path, &fd);   
    if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))   
    {   
        ret = TRUE;   
    }   
    FindClose(hFind);   
    return ret;   
}   

/**
 * Get the size of a file.
 * @param filename The name of the file to check size for
 * @return The filesize, or 0 if the file does not exist.
 */
size_t getFilesize(const std::tstring& filename) {
    struct _stat st;
    if(_tstat(filename.c_str(), &st) != 0) {
        return 0;
    }
    return st.st_size;   
}


#include <fstream>
#include <vector>
#if 0
bool UpdateFileInfo(CString strFN, struct fileinfo* sfinfo)
{
    std::ifstream       file(strFN);
    if (file)
    {
        /*
         * Get the size of the file
         */
        file.seekg(0,std::ios::end);
        std::streampos          length = file.tellg();
        file.seekg(0,std::ios::beg);

        /*
         * Use a vector as the buffer.
         * It is exception safe and will be tidied up correctly.
         * This constructor creates a buffer of the correct length.
         * Because char is a POD data type it is not initialized.
         *
         * Then read the whole file into the buffer.
         */
        //std::vector<char>       buffer(length);
        //file.read(&buffer[0],length);

		sfinfo->filename = strFN.GetBuffer(0);
		sfinfo->st_size = length;
		sfinfo->vbuf.resize(length);
        file.read(&(sfinfo->vbuf[0]),length);

		if(file)
			printf("all characters read successfully.\n");
		else
			printf("error: only %i could be read\n", file.gcount());

		unsigned short cks = 0;
		for(size_t i = 0; i < length; i++)
			cks += sfinfo->vbuf[i];
		sfinfo->usCheckSum = cks;
		return true;

    }	
	
	return false;
}
#else
#include <string>
#include <cstdio>
#include <cerrno>

bool UpdateFileInfo(CString strFN, struct fileinfo* sfinfo)
{
	
    if(IsFolder2(strFN)) {
		printf("THis is a folder\n");
		return false;
	}
		
    std::FILE *fp = _tfopen(strFN.GetBuffer(0), _T("rb"));
    if (fp) {
    std::fseek(fp, 0, SEEK_END);
	int length = std::ftell(fp);
    std::rewind(fp);

	sfinfo->filename = strFN;
	sfinfo->st_size = length;
	sfinfo->vbuf.resize(length);

    size_t result = std::fread(&(sfinfo->vbuf[0]), 1, length, fp);
		unsigned short cks = 0;
		for(size_t i = 0; i < length; i++)
			cks += sfinfo->vbuf[i];
		sfinfo->usCheckSum = cks;
		
	//if(result == length)
	//	printf("all characters read successfully. (%d, %X)\n", length, cks);
	//else
	//	printf("error: only %i could be read\n", result);
	
    std::fclose(fp);
    return (result == length);
  }
  return false;
}
#endif

size_t mfwrite(const void *ptr, size_t len, _TCHAR *_FileName)
{
    size_t ret = 0;
    std::FILE *fp = _tfopen(_FileName, _T("wb"));
    ret = std::fwrite(ptr, 1, len, fp);
    std::fclose(fp);
    return ret;
}

size_t mfread(void *ptr, size_t len, _TCHAR *_FileName)
{
    size_t ret = 0;
    std::FILE *fp = _tfopen(_FileName, _T("rb"));
    ret = std::fread(ptr, 1, len, fp);
    std::fclose(fp);
    return ret;
}
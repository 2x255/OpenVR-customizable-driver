#pragma once

#if defined(_WINDOWS)

#include "textfile.h"

bool TextFile::Release(void)
{
	fLoaded = false;

	if(src){
		delete [] src;
		src = NULL;
	}
	liSize = {0, 0};

	return true;
}
bool TextFile::Load(const char *strFilePath)
{
	if(fLoaded){return false;}

	HANDLE hFile = CreateFileA(strFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE){return false;}

	LARGE_INTEGER liCurrentIndex = {0, 0};
	if(!SetFilePointerEx(hFile, liCurrentIndex, NULL, FILE_BEGIN)){return false;};
	if(!GetFileSizeEx(hFile, &liSize)){return false;};

	DWORD dwRead;

	if(!(src = new char[liSize.QuadPart+1])){return false;};
	/*ファイル読み込み*/
	while(liCurrentIndex.QuadPart < liSize.QuadPart){
		if(!ReadFile(hFile, &src[liCurrentIndex.QuadPart],
			(liSize.HighPart - liCurrentIndex.QuadPart > static_cast<LONGLONG>(MAXDWORD)) ? MAXDWORD : liSize.LowPart, &dwRead, NULL)){return false;};
		liCurrentIndex.QuadPart += static_cast<LONGLONG>(dwRead);
	}

	CloseHandle(hFile);
	hFile = NULL;

	fLoaded = true;

	return true;
}
bool TextFile::IsLoaded(void)
{
	return fLoaded;
}
const LARGE_INTEGER &TextFile::GetSize(void)
{
	return liSize;
}

char *TextFile::GetString(void)
{
	if(!fLoaded){return NULL;};
	return src;
}
bool TextFile::Replace(char cSrc, char cDest)
{
	if(!fLoaded){return false;};

	/*\nを\0に置換、最後に\0を追加*/
	for(LARGE_INTEGER li = {0, 0}; li.QuadPart < liSize.QuadPart; li.QuadPart++){
		if(src[li.QuadPart] == cSrc){
			src[li.QuadPart] = cDest;
		}
	}
	src[liSize.QuadPart] = '\0';

	return true;
}

#endif
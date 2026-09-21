#pragma once

#if defined(_WINDOWS)
#include <Windows.h>

class TextFile{
	bool fLoaded = false;
	LARGE_INTEGER liSize = {0, 0};
	char *src = NULL;/*ファイルサイズ+1バイト確保ひ、最後に\0を追加し文字列として扱えるようにしておく*/

public:
	bool Release(void);
	bool Load(const char *strFilePath);
	bool IsLoaded(void);

	const LARGE_INTEGER &GetSize(void);
	char *GetString(void);
	bool Replace(char cSrc, char cDest);
};

#endif

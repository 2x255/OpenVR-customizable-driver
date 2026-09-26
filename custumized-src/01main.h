#pragma once

/*文字列操作関数(_s系でない関数)による警告を止める*/
#pragma warning(disable: 4996)

#if defined(_WINDOWS)
#include <stdio.h>
#include <Windows.h>

#define TSTR_MAX 512

#define NR(value) if(!(value)){return false;}
#define NRNull(value) if(!(value)){return NULL;}
#define sc(type, value) static_cast<type>(value)
#define u(n) sc(UINT, n)
#define ll(n) sc(long long, n)
#define ErrorDef(str) 

	/*ArrayManager*/
//#include "01array.h"

	/*ShiftJIS基準*/
	class FilePath{
		bool fInitialized = false;
		bool fInitializedA = false;

		/*tstr*/
		TCHAR tstrParent[TSTR_MAX+2] = {};/*x64/Debug以下にある場合はそれより上のフォルダ*/
		TCHAR tstrParentDR[TSTR_MAX+2] = {};/*x64/Debug以下にあろうと関係なく実行ファイルの親フォルダ.DRはDebug/Releaseの略*/
		UINT uTstrParentPathLen = 0;
		UINT uTstrParentPathDRLen = 0;
		TCHAR tstrFreeA[TSTR_MAX+2] = {};
		TCHAR tstrFreeB[TSTR_MAX+2] = {};

		/*str*/
		char strParent[TSTR_MAX+2] = {};/*x64/Debug以下にある場合はそれより上のフォルダ*/
		char strParentDR[TSTR_MAX+2] = {};/*x64/Debug以下にあろうと関係なく実行ファイルの親フォルダ.DRはDebug/Releaseの略*/
		UINT uStrParentPathLen = 0;
		UINT uStrParentPathDRLen = 0;
		char strFreeA[TSTR_MAX+2] = {};
		char strFreeB[TSTR_MAX+2] = {};

	public:
		/*tstrPathを親ディレクトリ名のパスに切り落とします。*/
		TCHAR *CutUniqueName(TCHAR *tstrPath);
		char *CutUniqueNameA(char *strPath);
		/*ファイル固有名だけを取得しまう*/
		TCHAR *GetUniqueName(const TCHAR *tstrPath, TCHAR *tstrGet, UINT uMax);
		char *GetUniqueNameA(const char *strPath, char *tstrGet, UINT uMax);
		bool Initialize(HMODULE hModule);
		bool InitializeA(HMODULE hModule);
		const TCHAR *RelativePath(const TCHAR *tstrPath);
		const char *RelativePathA(const char *strPath);
		const TCHAR *RelativePathDR(const TCHAR *tstrPath);
		const char *RelativePathDRA(const char *strPath);
	};
	extern FilePath fp;



#endif
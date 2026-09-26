#pragma once

#include "01main.h"



	TCHAR *FilePath::CutUniqueName(TCHAR *tstrPath)
	{
		NRNull(tstrPath);

		UINT uLastBackSlashIndex = 0;
		TCHAR tc = 0;
		bool fIsFirstByte = true;

		for(int i = 0; tstrPath[i] != '\0'; i++){
			tc = tstrPath[i];

			if(fIsFirstByte){
				if(!(tc - '\\')){
					if(uLastBackSlashIndex == i-1){
						/*\\が連続する場合ここではより前のもののindexを持ちたいので最初の1つの時だけ代入*/
					}else{
						uLastBackSlashIndex = i;
					}
				}else if(0x81 <= tc && tc <= 0x8F || 0xE0 <= tc && tc <= 0xEF){
					fIsFirstByte = false;
				}
			}else{
				fIsFirstByte = true;
			}
		}
		tstrPath[uLastBackSlashIndex] = '\0';

		return tstrPath;
	}
	TCHAR *FilePath::GetUniqueName(const TCHAR *tstrPath, TCHAR *tstrGet, UINT uMax)
	{
		NRNull(tstrGet);

		UINT uLastBackSlashIndex = 0;
		TCHAR tc = 0;
		bool fIsFirstByte = true;

		for(int i = 0; tstrPath[i] != '\0'; i++){
			tc = tstrPath[i];

			if(fIsFirstByte){
				if(!(tc - '\\')){
					/*\\が連続する場合ここではより後のもののindexを持ちたいので常に代入*/
					uLastBackSlashIndex = i;
				}else if(0x81 <= tc && tc <= 0x8F || 0xE0 <= tc && tc <= 0xEF){
					fIsFirstByte = false;
				}
			}else{
				fIsFirstByte = true;
			}
		}

		if(u(lstrlen(&tstrPath[uLastBackSlashIndex])) > uMax){
			ErrorDef("tstrGetの長さが足りません。");
			return NULL;
		}

		NRNull(lstrcpy(tstrGet, &tstrPath[uLastBackSlashIndex+1]));

		return tstrGet;
	}

	bool FilePath::Initialize(HMODULE hModule)
	{
		NR(GetModuleFileName(hModule, tstrParent, TSTR_MAX) && !GetLastError());
		NR(CutUniqueName(tstrParent));
		NR(lstrcpy(tstrParentDR, tstrParent));

		/*VisualStudio開発環境での動作を考慮しtstrParentの"\x64\Debug又は\x64\Releaseを削除"*/
		NR(GetUniqueName(tstrParent, tstrFreeA, TSTR_MAX));
		if(!lstrcmp(tstrFreeA, TEXT("Debug")) || !lstrcmp(tstrFreeA, TEXT("Release"))){
			NR(lstrcpy(tstrFreeB, tstrParent));
			NR(CutUniqueName(tstrFreeB));
			NR(GetUniqueName(tstrFreeB, tstrFreeA, TSTR_MAX));

			if(!lstrcmp(tstrFreeA, TEXT("x64"))){
				NR(CutUniqueName(tstrParent));
				NR(CutUniqueName(tstrParent));
			}
		}

		uTstrParentPathLen = u(lstrlen(tstrParent));
		uTstrParentPathDRLen = u(lstrlen(tstrParentDR));

		fInitialized = true;

		return true;
	}

	const TCHAR *FilePath::RelativePath(const TCHAR *tstrPath)
	{
		NRNull(fInitialized);
		NRNull(tstrPath);
		NRNull(uTstrParentPathLen + 1/*'\\*/ + lstrlen(tstrPath) + 1/*'\0*/ <= TSTR_MAX);

		wsprintf(tstrFreeA, TEXT("%s\\%s"), tstrParent, tstrPath);

		return tstrFreeA;
	}

	const TCHAR *FilePath::RelativePathDR(const TCHAR *tstrPath)
	{
		NRNull(fInitialized);
		NRNull(tstrPath);
		NRNull(uTstrParentPathDRLen + 1/*'\\*/ + lstrlen(tstrPath) + 1/*'\0*/ <= TSTR_MAX);

		wsprintf(tstrFreeA, TEXT("%s\\%s"), tstrParentDR, tstrPath);

		return tstrFreeA;
	}

	FilePath fp;

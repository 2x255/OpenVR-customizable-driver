#pragma once

#include "01main.h"

	char *FilePath::CutUniqueNameA(char *strPath)
	{
		NRNull(strPath);

		UINT uLastBackSlashIndex = 0;
		char tc = 0;
		bool fIsFirstByte = true;

		for(int i = 0; strPath[i] != '\0'; i++){
			tc = strPath[i];

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
		strPath[uLastBackSlashIndex] = '\0';

		return strPath;
	}
	char *FilePath::GetUniqueNameA(const char *strPath, char *strGet, UINT uMax)
	{
		NRNull(strGet);

		UINT uLastBackSlashIndex = 0;
		char tc = 0;
		bool fIsFirstByte = true;

		for(int i = 0; strPath[i] != '\0'; i++){
			tc = strPath[i];

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

		if(u(strlen(&strPath[uLastBackSlashIndex])) > uMax){
			ErrorDef("strGetの長さが足りません。");
			return NULL;
		}

		strcpy(strGet, &strPath[uLastBackSlashIndex+1]);

		return strGet;
	}

	bool FilePath::InitializeA(HMODULE hModule)
	{
		NR(GetModuleFileNameA(hModule, strParent, TSTR_MAX) && !GetLastError());
		NR(CutUniqueNameA(strParent));
		strcpy(strParentDR, strParent);

		/*VisualStudio開発環境での動作を考慮しstrParentの"\x64\Debug又は\x64\Releaseを削除"*/
		NR(GetUniqueNameA(strParent, strFreeA, TSTR_MAX));
		if(!strcmp(strFreeA, ("Debug")) || !strcmp(strFreeA, ("Release"))){
			strcpy(strFreeB, strParent);
			NR(CutUniqueNameA(strFreeB));
			NR(GetUniqueNameA(strFreeB, strFreeA, TSTR_MAX));

			if(!strcmp(strFreeA, ("x64"))){
				NR(CutUniqueNameA(strParent));
				NR(CutUniqueNameA(strParent));
			}
		}

		uStrParentPathLen = u(strlen(strParent));
		uStrParentPathDRLen = u(strlen(strParentDR));

		fInitializedA = true;

		return true;
	}

	const char *FilePath::RelativePathA(const char *strPath)
	{
		NRNull(fInitializedA);
		NRNull(strPath);
		NRNull(uStrParentPathLen + 1/*'\\*/ + strlen(strPath) + 1/*'\0*/ <= TSTR_MAX);

		sprintf(strFreeA, ("%s\\%s"), strParent, strPath);

		return strFreeA;
	}

	const char *FilePath::RelativePathDRA(const char *strPath)
	{
		NRNull(fInitializedA);
		NRNull(strPath);
		NRNull(uStrParentPathDRLen + 1/*'\\*/ + strlen(strPath) + 1/*'\0*/ <= TSTR_MAX);

		sprintf(strFreeA, ("%s\\%s"), strParentDR, strPath);

		return strFreeA;
	}


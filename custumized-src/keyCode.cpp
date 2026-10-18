#pragma once

#include "keyCode.h"


void KeyCodeManager::LoadByFile(const char *filepath, unsigned int __size_a, unsigned int __size_b)
{
	size_a = __size_a;
	size_b = __size_b;

	/*既定値で初期化してから、ファイルを読み込み出来るなら読み込み*/
	std::vector<unsigned int> temp;
	for(int i = 0; i < size_a; i++){
		keyCode.push_back(temp);
		for(int j = 0; j < size_b; j++){
			keyCode[i].push_back(0);
		}
	}
	
#if defined(_WINDOWS)
	do{
		TextFile textfile;
		if(!textfile.Load(filepath)){break;};
		if(!textfile.Replace('\n', '\0')){break;};
		if(!textfile.Replace('\r', '\0')){break;};
		if(!textfile.Replace(' ', '\0')){break;};
		if(!textfile.Replace('=', '\0')){break;};
		char *src = textfile.GetString();
		if(!src){break;}

		/*01 = 123 の場合、[0][1]に123(10進数)を代入*/
		unsigned int keyCodeIndex = 0;
		unsigned int uMode = 0;/*偶数:index　奇数:*/
		for(LARGE_INTEGER li = {0, 0}, liLen = textfile.GetSize(); li.QuadPart < liLen.QuadPart; li.QuadPart += strlen(&src[li.QuadPart])+1){
			if(src[li.QuadPart] == '/'){
				/*コメント行*/
			}else{
				if(src[li.QuadPart] == '\0'){
					/*なにもしない*/
				}else{
					if(!(uMode % 2)){/*index読み込み*/
						keyCodeIndex = atoi(&src[li.QuadPart]);
					}else{/*code読み込み*/
						if(size_a <= keyCodeIndex / size_b){/*indexの値が間違っている場合はスキップ*/
						}else{
							if('0' <= src[li.QuadPart] && src[li.QuadPart] <= '9'){/*仮想キーコード値指定*/
								unsigned int code = atoi(&src[li.QuadPart]);
								keyCode[keyCodeIndex/size_b][keyCodeIndex%size_b] = code;
							}else{/*文字指定*/
								keyCode[keyCodeIndex/size_b][keyCodeIndex%size_b] = src[li.QuadPart];
							}
						}
					}
					uMode++;
				}
			}
		}
		src = NULL;
		textfile.Release();
	}while(false);

#endif

	return;
}

std::vector<unsigned int>& KeyCodeManager::operator[](unsigned int index)
{
	static std::vector<unsigned int> damy(10, 0);
	if(size_a <= index){
		return damy;
	}
	return keyCode[index];
}

bool KeyCodeManager::GetState(void)
{
	return fState;
}

void KeyCodeManager::SetState(bool fNewState)
{
	fState = fNewState;
	return;
}

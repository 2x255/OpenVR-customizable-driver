#pragma once

#include "textfile.h"

#include <vector>

class KeyCodeManager{
	unsigned int size_a = 0, size_b = 0;
	std::vector<std::vector<unsigned int>> keyCode;/*[size_a][size_b]*/

	bool fState = true;/*“ü—Íó‘Ô[ó•t/’â~]*/

public:
	void LoadByFile(const char *filepath, unsigned int __size_a, unsigned int __size_b);

	bool GetState(void);
	void SetState(bool fNewState);

	std::vector<unsigned int>&operator[](unsigned int index);
};


#pragma once

#include "textfile.h"

#include <vector>

class KeyCodeManager{
	unsigned int size_a = 0, size_b = 0;
	std::vector<std::vector<unsigned int>> keyCode;/*[size_a][size_b]*/

public:
	void LoadByFile(const char *filepath, unsigned int __size_a, unsigned int __size_b);

	std::vector<unsigned int>&operator[](unsigned int index);
};
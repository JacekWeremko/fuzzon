//============================================================================
// Name        : arrayness.cpp
// Author      : Jacek Weremko
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include <stdint.h>
#include <stddef.h>


int FuzzonSUTEntryPoint(int argc, char **argv)
{
	int arrays_defs = 0;
	std::cin >> arrays_defs;

	for(int array_def=0; array_def<arrays_defs; ++array_def)
	{
		int array_len = 0;
		int array[16] = { 0 };

		std::cin >> array_len;
		if (array_len < 4)
		{
			return 1;
		}
		if (array_len > 20)
		{
			return 2;
		}

		for(int i=0; i<array_len; ++i)
		{
			int new_value;
			std::cin >> new_value;
			array[i] = new_value;
		}
	}
	return 0;
}



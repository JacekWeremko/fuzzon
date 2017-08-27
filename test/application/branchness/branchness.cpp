//============================================================================
// Name        : Branchness.cpp
// Author      : Jacek Weremko
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <cstring>
#include <string>
#include <iostream>

#include <stdint.h>
#include <stddef.h>


bool func_a(char a)
{
	if (a == 'a')
	{
		return true;
	}
	return false;
}

bool func_b(char b)
{
	return b == 'b' ? true : false;
}

bool func_c(char c)
{
	return c == 'c' ? true : false;
}

bool func_d(char d)
{
	return d == 'd' ? true : false;
}

void crash_me()
{
	volatile int* pc = 0;
	volatile int crash = *pc;
	return;
}

int main(int argc, char **argv)
{
	int guard = 0;

	if (argc < 2)
	{
		return 2;
	}
//	std::string test_stdin1, test_stdin2, test_stdin3;

//	std::cout << "cout test1";
//	std::cerr << "cerr test1";
//	std::cout << "cout test2";
//	std::cerr << "cerr test2";
//	std::cerr << "cerr test1";
//	std::cout << "cout test3";
//	std::cin >> test_stdin1;

	std::string argv1 = std::string(argv[1]);
	char data[8] = { 0 };
	std::memcpy(&data[0], argv1.c_str(), argv1.size());

	if (func_a(data[0]))
	{
		guard++;
		if (func_b(data[1]))
		{
			guard++;
			if (func_c(data[2]))
			{
				guard++;
				if (func_d(data[3]))
				{
					guard++;
//					crash_me();
				}
			}
		}
	}

//	crash_me();

	if (func_a(data[4]))
	{
		guard++;
		if (func_b(data[5]))
		{
			guard++;
			if (func_c(data[6]))
			{
				guard++;
				if (func_d(data[7]))
				{
					guard++;
					crash_me();
				}
			}
		}
	}
	return 1;
}


/*
 * fuzzon_test_case.cpp
 *
 *  Created on: Aug 22, 2017
 *      Author: dablju
 */

#include "fuzzon_testcase.h"

namespace fuzzon {

TestCase::TestCase()
{
	length_ = 10;
	data_ = new uint8_t[length_];
}

TestCase::TestCase(uint8_t* data, size_t length) : data_(data), length_(length)
{

}

TestCase::TestCase(std::string serialized)
{
	length_ = serialized.length();
	data_ = new uint8_t[length_];
	std::copy(serialized.begin(), serialized.end(), data_);
}

TestCase::~TestCase()
{
	delete [] data_;
}

std::string TestCase::string()
{
	return std::string(reinterpret_cast<char const*>(data_), length_);
}

char** TestCase::argv()
{
	return reinterpret_cast<char**>(&data_);
}

const size_t TestCase::argc()
{
	size_t argc = 0;
	auto as_string = this->string();
	std::string::size_type space_position = 0;
	while((space_position = as_string.find(' ', space_position)) != std::string::npos)
	{
	    space_position++;
	}
	return argc;
}

} /* namespace fuzzon */

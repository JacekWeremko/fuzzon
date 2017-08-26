#include "fuzzon_testcase.h"


namespace fuzzon {

TestCase::TestCase() : length_(10), data_(new uint8_t[length_])
{
//	length_ = 10;
//	data_ = std::make_shared<uint8_t>(uint8_t[length_]);
}

TestCase::TestCase(uint8_t* data, size_t length) : length_(length), data_(std::shared_ptr<uint8_t>(data))
{
	int cahce = length_;
	length_ = cahce;
}

TestCase::TestCase(std::string serialized)
{
	length_ = serialized.length();
	data_ = std::shared_ptr<uint8_t>(new uint8_t[length_]);
	std::copy(serialized.begin(), serialized.end(), data_.get());
}

TestCase::~TestCase()
{
//	delete [] data_;
//	data_ = NULL;
	length_ = 0;
}

std::string TestCase::string()
{
	return std::string(reinterpret_cast<char const*>(data_.get()), length_ -1);
}

char** TestCase::argv()
{
	return reinterpret_cast<char**>(&data_);
}

const size_t TestCase::argc()
{
	size_t argc = 1;
	auto as_string = this->string();
	std::string::size_type space_position = 0;
	while((space_position = as_string.find(' ', space_position)) != std::string::npos)
	{
	    space_position++;
	    argc++;
	}
	return argc;
}

} /* namespace fuzzon */

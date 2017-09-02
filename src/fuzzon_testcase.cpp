#include "fuzzon_testcase.h"
#include "fuzzon_random.h"

#include <iterator>

namespace fuzzon {

TestCase::TestCase(uint8_t* data, size_t length)
{
	data_.assign(data, data + length);
}

TestCase::TestCase(TestCase&& move_me) noexcept : data_(std::move(move_me.data_))
{
}

TestCase::TestCase(const TestCase& copy_me) : data_(copy_me.data_)
{
}

TestCase::TestCase(const std::string& serialized)
{
	std::copy(serialized.begin(), serialized.end(), std::back_inserter(data_));
}

uint8_t* const TestCase::data()
{
	return data_.data();
}

size_t TestCase::length()
{
	return data_.size();
}

std::string TestCase::string()
{
	return std::string(reinterpret_cast<char const*>(data()), length());
}

} /* namespace fuzzon */

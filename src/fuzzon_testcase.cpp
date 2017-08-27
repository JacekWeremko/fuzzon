#include "fuzzon_testcase.h"
#include "fuzzon_random.h"


namespace fuzzon {

TestCase::TestCase() : length_(Random::Get()->GenerateInt()), data_(new uint8_t[length_])
{
}

TestCase::TestCase(uint8_t* data, size_t length) : length_(length), data_(std::shared_ptr<uint8_t>(data))
{
}

TestCase::TestCase(std::string serialized)
{
	length_ = serialized.length();
	data_ = std::shared_ptr<uint8_t>(new uint8_t[length_]);
	std::copy(serialized.begin(), serialized.end(), data_.get());
}

uint8_t* const TestCase::data()
{
	return data_.get();
}
const size_t TestCase::length()
{
	return length_;
}

std::string TestCase::string()
{
	return std::string(reinterpret_cast<char const*>(data_.get()), length_);
}

} /* namespace fuzzon */

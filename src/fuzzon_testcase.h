#ifndef FUZZON_TESTCASE_H_
#define FUZZON_TESTCASE_H_

#include <string>
#include <memory>

namespace fuzzon {

class TestCase {
public:
	TestCase();
	TestCase(uint8_t* data, size_t length);
	TestCase(std::string serialized);

	uint8_t* const data();
	const size_t length();

	std::string string();

private:
	size_t length_;
	std::shared_ptr<uint8_t> data_;
};

} /* namespace fuzzon */

#endif /* FUZZON_TESTCASE_H_ */

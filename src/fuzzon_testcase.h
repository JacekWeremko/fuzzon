
#ifndef FUZZON_TESTCASE_H_
#define FUZZON_TESTCASE_H_

#include <string>
#include <memory>

namespace fuzzon {

class TestCase {
public:
	TestCase();
//	TestCase(TestCase const&);
//	void operator=(TestCase const&);


	TestCase(uint8_t* data, size_t length);
	TestCase(std::string serialized);

	virtual ~TestCase();


	uint8_t* const data() { return data_.get(); }
	const size_t length() { return length_; }

	char** argv();
	const size_t argc();

	std::string string();

private:
	size_t length_;
	std::shared_ptr<uint8_t> data_;
//	std::string serialized_;
};

} /* namespace fuzzon */

#endif /* FUZZON_TESTCASE_H_ */

/*
 * fuzzon_test_case.h
 *
 *  Created on: Aug 22, 2017
 *      Author: dablju
 */

#ifndef FUZZON_TESTCASE_H_
#define FUZZON_TESTCASE_H_

#include <string>

namespace fuzzon {

class TestCase {
public:
	TestCase();
	TestCase(uint8_t* data, size_t length);
	TestCase(std::string serialized);

	virtual ~TestCase();


	uint8_t* const data() { return data_; }
	const size_t length() { return length_; }

	char** argv();
	const size_t argc();

	std::string string();

private:
	uint8_t* data_;
	size_t length_;
//	std::string serialized_;
};

} /* namespace fuzzon */

#endif /* FUZZON_TESTCASE_H_ */

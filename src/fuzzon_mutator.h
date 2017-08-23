/*
 * fuzzon_mutator.h
 *
 *  Created on: Aug 21, 2017
 *      Author: dablju
 */

#ifndef FUZZON_MUTATOR_H_
#define FUZZON_MUTATOR_H_

#include "fuzzon_testcase.h"

#include <boost/random.hpp>

namespace fuzzon
{

class Mutator
{
public:
	Mutator(bool preserve_special_chars);
	virtual ~Mutator();

	TestCase Mutate(TestCase mutate_me);

private:
	int FlipBit(uint8_t* data, size_t data_size);
	int FlipByte(uint8_t* data, size_t data_size);
	int ChangeByte(uint8_t* data, size_t data_size);

	bool preserve_special_chars_;
	boost::random::mt19937 generator_;
};

}




#endif /* FUZZON_MUTATOR_H_ */

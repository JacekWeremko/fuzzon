
#ifndef FUZZON_MUTATOR_H_
#define FUZZON_MUTATOR_H_

#include "fuzzon_testcase.h"

namespace fuzzon
{

class Mutator
{
public:
	Mutator(bool preserve_special_chars);
	virtual ~Mutator();

	TestCase Mutate(TestCase mutate_me);


	int FlipBit(uint8_t* data, size_t data_size);
	int FlipByte(uint8_t* data, size_t data_size);
	int ChangeByte(uint8_t* data, size_t data_size);

private:
	bool preserve_special_chars_;
};

}




#endif /* FUZZON_MUTATOR_H_ */

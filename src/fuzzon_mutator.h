
#ifndef FUZZON_MUTATOR_H_
#define FUZZON_MUTATOR_H_

#include "fuzzon_testcase.h"

namespace fuzzon
{

class Mutator
{
public:
	Mutator(std::string input_alphabet, std::string output_alphabet);
	virtual ~Mutator();

	TestCase Mutate(TestCase mutate_me);


	int FlipBit(uint8_t* data, size_t data_size);
	int FlipByte(uint8_t* data, size_t data_size);
	int ChangeByte(uint8_t* data, size_t data_size);

private:
	const size_t mutation_guard = 1000;
	std::string input_alphabet_;
	std::string output_alphabet_;
};

}




#endif /* FUZZON_MUTATOR_H_ */

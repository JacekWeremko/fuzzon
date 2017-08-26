
#include "fuzzon_mutator.h"
#include "fuzzon_random.h"

namespace fuzzon {

Mutator::Mutator(std::string input_alphabet) :
		input_alphabet_(input_alphabet)
{
}

Mutator::~Mutator() {
	// TODO Auto-generated destructor stub
}


TestCase Mutator::Mutate(TestCase mutate_me)
{
	auto new_test_case = TestCase(mutate_me.string());
	//TODO: add strategy selection
	auto result = ChangeByte(new_test_case.data(), new_test_case.length());
	return new_test_case;
}

int Mutator::FlipBit(uint8_t* data, size_t data_size)
{
	uint8_t* selected_byte = nullptr;
	int bit_to_flip = 0;
	for(size_t mutation_idx=0; mutation_idx<mutation_guard; mutation_idx++)
	{
		bit_to_flip = Random::Get()->GenerateInt(0, (data_size*8)-1);
		selected_byte = &data[bit_to_flip / 8];
		if (input_alphabet_.find(*selected_byte) != std::string::npos)
		{
			break;
		}
	}
	(*selected_byte) = ((*selected_byte) ^ (1 << (bit_to_flip % 8)));
    return 0;
}

int Mutator::FlipByte(uint8_t* data, size_t data_size)
{
	uint8_t* selected_byte = nullptr;
	for(size_t mutation_idx=0; mutation_idx<mutation_guard; mutation_idx++)
	{
		auto byte_to_flip = Random::Get()->GenerateInt(0, data_size-1);
		selected_byte = &data[byte_to_flip];
		if (input_alphabet_.find(*selected_byte) != std::string::npos)
		{
			break;
		}
	}

    (*selected_byte) = ~(*selected_byte);
    return 0;
}

int Mutator::ChangeByte(uint8_t* data, size_t data_size)
{
	uint8_t* selected_byte = nullptr;
	int byte_to_flip = 0;
	for(size_t mutation_idx=0; mutation_idx<mutation_guard; mutation_idx++)
	{
		auto byte_to_flip = Random::Get()->GenerateInt(0, data_size-1);
		selected_byte = &data[byte_to_flip];
		if (input_alphabet_.find(*selected_byte) != std::string::npos)
		{
			break;
		}
	}

    data[byte_to_flip] = *Random::Get()->GenerateString(1).c_str();
	return 0;
}


} /* namespace fuzzon */

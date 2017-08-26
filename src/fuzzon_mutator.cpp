
#include "fuzzon_mutator.h"
#include "fuzzon_random.h"
#include "utils/logger.h"

namespace fuzzon {

Mutator::Mutator(std::string input_alphabet) :
		type_preservation_(true),
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
	for(size_t mutation_idx=0; mutation_idx<mutation_guard; mutation_idx++)
	{
		const auto bit_to_flip = Random::Get()->GenerateInt(0, (data_size*8)-1);
		auto& selected_byte = data[bit_to_flip / 8];
		if (input_alphabet_.find(selected_byte) != std::string::npos)
		{
			selected_byte = selected_byte ^ (1 << (bit_to_flip % 8));
		    return 0;
		}
	}
    return -1;
}

int Mutator::FlipByte(uint8_t* data, size_t data_size)
{
	for(size_t mutation_idx=0; mutation_idx<mutation_guard; mutation_idx++)
	{
		const auto byte_to_flip = Random::Get()->GenerateInt(0, data_size-1);
		auto& selected_byte = data[byte_to_flip];
		if (input_alphabet_.find(selected_byte) != std::string::npos)
		{
			selected_byte = ~selected_byte;
			return 0;
		}
	}
    return -1;
}

int Mutator::ChangeByte(uint8_t* data, size_t data_size)
{
	char selected_byte = '\0';
	int byte_to_flip = 0;
	size_t mutation_idx = 0;
	for(; mutation_idx<mutation_guard; mutation_idx++)
	{
		byte_to_flip = Random::Get()->GenerateInt(0, data_size-1);
		selected_byte = data[byte_to_flip];
		if (input_alphabet_.find(selected_byte) != std::string::npos)
		{
			break;
		}
	}
	if (mutation_idx == mutation_guard)
	{
		Logger::Get()->debug("mutation_idx ==mutation_guard");
	}

	for(size_t mutation_idx = 0; mutation_idx<mutation_guard; mutation_idx++)
	{
		const auto new_char = type_preservation_ == true ?
				Random::Get()->GenerateChar(selected_byte) : Random::Get()->GenerateChar();

		if (new_char == data[byte_to_flip])
		{
			continue;
		}
		data[byte_to_flip] = new_char;
		break;
	}
	return 0;
}


} /* namespace fuzzon */

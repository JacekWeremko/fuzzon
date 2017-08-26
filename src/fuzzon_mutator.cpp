
#include "fuzzon_mutator.h"
#include "fuzzon_random.h"

namespace fuzzon {

Mutator::Mutator(bool preserve_special_chars) : preserve_special_chars_(preserve_special_chars)
{
	BOOST_ASSERT_MSG(preserve_special_chars_ == false, "Not implemented yet");
}

Mutator::~Mutator() {
	// TODO Auto-generated destructor stub
}


TestCase Mutator::Mutate(TestCase mutate_me)
{
	static boost::random::uniform_int_distribution<> alphabet(1, 127);

	auto mutate_me_tmp = mutate_me.string();
	auto change_position = alphabet(Random::Get()->GetGenerator()) % (mutate_me_tmp.size() - 2);
	auto change_character = std::to_string(alphabet(Random::Get()->GetGenerator()));
	mutate_me_tmp.replace(change_position, change_position+1, change_character);
	return TestCase(mutate_me_tmp);
}

int Mutator::FlipBit(uint8_t* data, size_t data_size)
{
	boost::random::uniform_int_distribution<> bit_selector(0, (data_size*8)-1);

    const auto bit_to_flip = bit_selector(Random::Get()->GetGenerator());
    auto& selected_byte = data[bit_to_flip / 8];
    selected_byte = (selected_byte ^ (1 << (bit_to_flip % 8)));
    return 0;
}

int Mutator::FlipByte(uint8_t* data, size_t data_size)
{
	boost::random::uniform_int_distribution<> byte_selector(0, data_size-1);

	const auto byte_to_flip = byte_selector(Random::Get()->GetGenerator());
    auto& selected_byte = data[byte_to_flip];
    selected_byte = ~selected_byte;
    return 0;
}

int Mutator::ChangeByte(uint8_t* data, size_t data_size)
{
	static boost::random::uniform_int_distribution<> alphabet(32, 127);
	boost::random::uniform_int_distribution<> byte_selector(0, data_size-1);

	const auto byte_to_flip = byte_selector(Random::Get()->GetGenerator());
    data[byte_to_flip] = alphabet(Random::Get()->GetGenerator());
	return 0;
}


} /* namespace fuzzon */

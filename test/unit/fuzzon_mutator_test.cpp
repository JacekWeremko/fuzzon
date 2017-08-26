
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/mpl/list.hpp>

#include "../../src/fuzzon_mutator.h"
#include "../../src/fuzzon_random.h"


BOOST_AUTO_TEST_SUITE(fuzzon_mutator_suite)

BOOST_AUTO_TEST_CASE(flip_bite)
{
	std::string input_alphabet(
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"1234567890");
	fuzzon::Mutator mutator_(input_alphabet);

	for(size_t iters=0; iters<10; iters++)
	{
		uint8_t test_data[10] = { 'A','B','c','d','5','6','7','8','9','0' };
		uint8_t mutate_me[10] = { 'A','B','c','d','5','6','7','8','9','0' };
		mutator_.FlipBit(&mutate_me[0], sizeof(mutate_me));

		int mutated_bytes = 0, mutated_bites = 0;
		for(size_t i=0; i<10; ++i)
		{
			if(test_data[i] != mutate_me[i])
			{
				mutated_bytes++;
				auto only_one_bit_set = test_data[i] ^ mutate_me[i];
				for(size_t j=0; j<8; j++)
				{
					bool lowset_bit = (only_one_bit_set >> j) & 0x1;
					if (lowset_bit)
					{
						mutated_bites++;
					}
				}
			}
		}
		BOOST_TEST(mutated_bytes == 1);
		BOOST_TEST(mutated_bites == 1);
	}
}


BOOST_AUTO_TEST_CASE(flip_byte)
{
	std::string input_alphabet(
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"1234567890");
	fuzzon::Mutator mutator_(input_alphabet);

	for(size_t iters=0; iters<10; iters++)
	{
		uint8_t test_data[10] = { 'A','B','c','d','5','6','7','8','9','0' };
		uint8_t mutate_me[10] = { 'A','B','c','d','5','6','7','8','9','0' };
		mutator_.FlipByte(&mutate_me[0], sizeof(mutate_me));

		int mutated_bytes = 0;
		for(size_t i=0; i<10; ++i)
		{
			if(test_data[i] != mutate_me[i])
			{
				mutated_bytes++;
			}
		}
		BOOST_TEST(mutated_bytes == 1);
	}
}

void test_type_preservation()
{
	std::string input_alphabet(
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"1234567890"
			"!@#$%^&*()"
			"`~-_=+[{]}\\|;:'\",<.>/? ");
	fuzzon::Mutator mutator_(input_alphabet);

	for(size_t iters=0; iters<10; iters++)
	{
		uint8_t test_data[10] = { 'A','B','c','d','5','6','!','^',':',';' };
		uint8_t mutate_me[10] = { 'A','B','c','d','5','6','!','^',':',';' };

		mutator_.ChangeByte(&mutate_me[0], sizeof(mutate_me));

		int mutated_bytes = 0;
		for(size_t i=0; i<10; ++i)
		{
			if(test_data[i] != mutate_me[i])
			{
				mutated_bytes++;
				auto test_date_type = fuzzon::Random::Get()->FindCharType(test_data[i]);
				auto mutate_me_type = fuzzon::Random::Get()->FindCharType(mutate_me[i]);

				BOOST_TEST(test_date_type == mutate_me_type);
			}
		}
		BOOST_TEST(mutated_bytes == 1);
	}
}

BOOST_AUTO_TEST_CASE(type_preservation)
{
	test_type_preservation();
}

BOOST_AUTO_TEST_SUITE_END()



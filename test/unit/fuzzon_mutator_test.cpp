
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/mpl/list.hpp>

#include "../../src/fuzzon_mutator.h"


BOOST_AUTO_TEST_SUITE(fuzzon_mutator_suite)

BOOST_AUTO_TEST_CASE(flib_bite)
{
	fuzzon::Mutator mutator_(false);

	for(size_t iters=0; iters<10; iters++)
	{
		uint8_t test_data[10] = { 1,2,3,4,5,6,7,8,9,0 };
		uint8_t mutate_me[10] = { 1,2,3,4,5,6,7,8,9,0 };
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


BOOST_AUTO_TEST_CASE(flib_byte)
{
	fuzzon::Mutator mutator_(false);

	for(size_t iters=0; iters<10; iters++)
	{
		uint8_t test_data[10] = { 1,2,3,4,5,6,7,8,9,0 };
		uint8_t mutate_me[10] = { 1,2,3,4,5,6,7,8,9,0 };
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

BOOST_AUTO_TEST_SUITE_END()



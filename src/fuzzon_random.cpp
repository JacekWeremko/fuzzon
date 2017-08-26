/*
 * fuzzon_random.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: dablju
 */

#include "fuzzon_random.h"

#include <ctime>
#include <cstdint>


namespace fuzzon {

Random::Random() :
		alphabet_small_letters_("abcdefghijklmnopqrstuvwxyz"),
		alphabet_capital_letters_("ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
		alphabet_nums_("1234567890"),
		alphabet_nums_special_("!@#$%^&*()"),
		alphabet_other_special_("`~-_=+[{]}\\|;:'\",<.>/? ")
{

	generator_ = boost::random::mt19937(static_cast<std::uint32_t>(std::time(0)));
	alphabet_ = std::string(alphabet_small_letters_ + alphabet_capital_letters_ + alphabet_nums_);
}

int Random::GenerateInt(int min, int max)
{
	static const int default_min = 1;
	static const int default_max = 5;

	BOOST_ASSERT(min <= max);
	min = min == -1 ? default_min : min;
	max = max == -1 ? default_max : max;


	boost::random::uniform_int_distribution<> min_to_max(min, max);
	return min_to_max(generator_);
}

std::string Random::GenerateString(int length)
{
	static const int max_length = 5;

	length = length == -1 ? max_length : length;


	const auto string_length = GenerateInt(1, max_length);
	std::stringstream random;
	for (size_t i=0; i<string_length; i++)
	{
		random << alphabet_[GenerateInt(0, (sizeof(alphabet_) - 1))];
	}
	return random.str();
}

//template<typename generate_type>
//generate_type Random::Generate(int min, int max)
//{
//	static const int default_min = 1;
//	static const int default_max = 10;
//
//	BOOST_ASSERT(min <= max);
//	min = min == -1 ? default_min : min;
//	max = max == -1 ? default_max : max;
//
//
//	boost::random::uniform_int_distribution<> min_to_max(min, max);
//	return min_to_max(generator_);
//}
//
//template<typename generate_type>
//generate_type Random::Generate(int length)
//{
//	static const int max_length = 5;
//
//	length = length == -1 ? max_length : length;
//
//
//	const auto string_length = Generate<int>(1, max_length);
//	std::stringstream random;
//	for (size_t i=0; i<string_length; i++)
//	{
//		random << alphabet_[Generate<int>(0, (sizeof(alphabet_) - 1))];
//	}
//	return random.str();
//}

Random::~Random() {
	// TODO Auto-generated destructor stub
}

} /* namespace fuzzon */

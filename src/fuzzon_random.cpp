#include "fuzzon_random.h"
#include "utils/logger.h"

#include <boost/algorithm/string/find.hpp>
#include <ctime>
#include <cstdint>



namespace fuzzon {

Random::Random()
{
	const std::string alphabet_small_letters_("abcdefghijklmnopqrstuvwxyz");
	const std::string alphabet_capital_letters_("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	const std::string alphabet_nums_("1234567890");
	const std::string alphabet_nums_special_("!@#$%^&*()");
	const std::string alphabet_other_special_("`~-_=+[{]}\\|;:'\",<.>/? ");

	alphabets_ =
	{
			{ AlphabetType::SmallLetters, alphabet_small_letters_ },
			{ AlphabetType::CapitalLetters, alphabet_capital_letters_ },
			{ AlphabetType::Nums, alphabet_nums_ },
			{ AlphabetType::NumsSpecials, alphabet_nums_special_ },
			{ AlphabetType::OtherSpecials, alphabet_other_special_ },
	};

	generator_ = boost::random::mt19937(static_cast<std::uint32_t>(std::time(0)));
//	alphabet_ = std::string(
//			alphabets_[AlphabetType::SmallLetters] +
//			alphabets_[AlphabetType::CapitalLetters] +
//			alphabets_[AlphabetType::Nums]);
	alphabet_ = std::string(
			alphabets_[AlphabetType::SmallLetters]);
}

void Random::SetAlphabet(std::string alphabet)
{
	alphabet_ = alphabet;
}

int Random::GenerateInt(int min, int max)
{
	static const int default_min = 1;
	static const int default_max = 3;

	BOOST_ASSERT(min <= max);
	min = min == -1 ? default_min : min;
	max = max == -1 ? default_max : max;


	boost::random::uniform_int_distribution<> min_to_max(min, max);
	return min_to_max(generator_);
}

std::string Random::GenerateString(int length)
{
	static const int min_length = 1;
	static const int max_length = 10;

	length = length == -1 ? max_length : length;

	const auto string_length = GenerateInt(1, max_length);
	std::stringstream random;
	for (size_t i=0; i<string_length; i++)
	{
		random << alphabet_[GenerateInt(0, alphabet_.size() - 1)];
	}
	return random.str();
}

char Random::GenerateChar()
{
	return alphabet_[GenerateInt(0, alphabet_.size() - 1)];
}

char Random::GenerateChar(char same_type_as_me)
{
	auto alphabet_type = FindCharType(same_type_as_me);
	if (alphabet_type != AlphabetType::Unknown)
	{
		const auto& alphabet = alphabets_[alphabet_type];
		return alphabet[GenerateInt(0, alphabet.size() - 1)];

	}
	Logger::Get()->critical("Alphabet not found! Generating random");
	return GenerateChar();
}

Random::AlphabetType Random::FindCharType(char what_is_my_type)
{
	for(const auto& alphabet : alphabets_)
	{
		if (alphabet.second.find(what_is_my_type) != std::string::npos)
		{
			return alphabet.first;
		}
	}
	return AlphabetType::Unknown;
}

} /* namespace fuzzon */

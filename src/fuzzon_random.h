#ifndef FUZZON_RANDOM_H_
#define FUZZON_RANDOM_H_

#include <boost/random.hpp>
#include <map>
#include <string>

namespace fuzzon {

class Random {
public:
	enum AlphabetType
	{
			SmallLetters = 0,
			CapitalLetters,
			Nums,
			NumsSpecials,
			OtherSpecials,
			Unknown,
	};

	Random(Random const&) = delete;
	void operator=(Random const&) = delete;

	static Random* Get()
	{
		static Random generator;
		return &generator;
	}

	void SetAlphabet(std::string alphabet);

	int GenerateInt(int min = -1, int max = -1);
	std::string GenerateString(int length = -1);
	char GenerateChar();
	char GenerateChar(char same_type_as_me);

	AlphabetType FindCharType(char what_is_my_type);

private:
	Random();

	boost::random::mt19937 generator_;
	std::string alphabet_;

	std::map<AlphabetType, std::string> alphabets_;
};

} /* namespace fuzzon */

#endif /* FUZZON_RANDOM_H_ */

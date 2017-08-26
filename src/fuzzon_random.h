/*
 * fuzzon_random.h
 *
 *  Created on: Aug 25, 2017
 *      Author: dablju
 */

#ifndef FUZZON_RANDOM_H_
#define FUZZON_RANDOM_H_

#include <random>
#include <boost/random.hpp>
#include <map>

namespace fuzzon {

class Random {
public:
	Random(Random const&) = delete;
	void operator=(Random const&) = delete;

	static Random* Get()
	{
		static Random generator;
		return &generator;
	}

	virtual ~Random();

	void SetAlphabet(std::string alphabet)
	{
		alphabet_ = alphabet;
	}


	enum AlphabetType
	{
			SmallLetters = 0,
			CapitalLetters,
			Nums,
			NumsSpecials,
			OtherSpecials,
			Unknown,
	};


	int GenerateInt(int min = -1, int max = -1);
	std::string GenerateString(int length = -1);
	char GenerateChar();
	char GenerateChar(char same_type_as_me);


	AlphabetType FindCharType(char what_is_my_type);

//	template<typename generate_type>
//	generate_type Generate(int min = -1, int max = -1);
//
//	template<typename generate_type>
//	generate_type Generate(int length = -1);


private:
	Random();

	boost::random::mt19937 generator_;
	std::string alphabet_;
//	std::vector<std::string> alphabets_;

	std::map<AlphabetType, std::string> alphabets_;
};

} /* namespace fuzzon */

#endif /* FUZZON_RANDOM_H_ */

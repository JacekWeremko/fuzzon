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

//	boost::random::mt19937& GetGenerator()
//	{
//		return generator_;
//	}

	void SetAlphabet(std::string alphabet)
	{
		alphabet_ = alphabet;
	}


	int GenerateInt(int min = -1, int max = -1);
	std::string GenerateString(int length = -1);

//	template<typename generate_type>
//	generate_type Generate(int min = -1, int max = -1);
//
//	template<typename generate_type>
//	generate_type Generate(int length = -1);


private:
	Random();

//	std::random_device rand_;
	boost::random::mt19937 generator_;
	std::string alphabet_;
	const std::string alphabet_small_letters_;
	const std::string alphabet_capital_letters_;
	const std::string alphabet_nums_;
	const std::string alphabet_nums_special_;
	const std::string alphabet_other_special_;
};

} /* namespace fuzzon */

#endif /* FUZZON_RANDOM_H_ */

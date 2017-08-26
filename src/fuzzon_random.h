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

	boost::random::mt19937& GetGenerator()
	{
		return generator_;
	}
private:
	Random();

//	std::random_device rand_;
	boost::random::mt19937 generator_;
};

} /* namespace fuzzon */

#endif /* FUZZON_RANDOM_H_ */

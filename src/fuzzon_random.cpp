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

Random::Random()
{
	generator_ = boost::random::mt19937(static_cast<std::uint32_t>(std::time(0)));
}

Random::~Random() {
	// TODO Auto-generated destructor stub
}

} /* namespace fuzzon */

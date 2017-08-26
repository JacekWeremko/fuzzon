
#define BOOST_TEST_MODULE fuzzon_test
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

//#include "fuzzon_test_main.h"
#include <boost/test/unit_test.hpp>


int FuzzonSUTEntryPoint(int argc, char**argv)
{
	return 0;
}

int main(int argc, char* argv[], char* envp[])
{
	auto val = boost::unit_test::unit_test_main(init_unit_test, argc, argv);
	return val;
}


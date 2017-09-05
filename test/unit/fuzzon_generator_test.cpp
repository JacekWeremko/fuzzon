

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/test_exec_monitor.hpp>
#include <boost/mpl/list.hpp>

#include "../../src/fuzzon_generator.h"

BOOST_AUTO_TEST_SUITE(fuzzon_generator_suite)

void generate_and_validate(boost::filesystem::path intput_format) {
  std::string output_directory = "";
  fuzzon::Generator test_cases_generator(intput_format.string());

  fuzzon::TestCase new_test_case = test_cases_generator.generateNext();
  BOOST_TEST_MESSAGE(new_test_case.string());
  BOOST_TEST((int)test_cases_generator.IsValid(new_test_case) == 1);
  return;
}

BOOST_AUTO_TEST_CASE(generate_sinle_string_json_arrayness_propsal) {
  //	auto intput_format =
  //boost::filesystem::current_path()/".."/"test"/"application"/"arrayness"/"arrayness_propsal2.json";
  //	generate_and_validate(intput_format);
}

BOOST_AUTO_TEST_CASE(generate_sinle_string_json_branchness) {
  //	auto intput_format =
  //boost::filesystem::current_path()/".."/"test"/"application"/"branchness"/"branchness.json";
  //	generate_and_validate(intput_format);
}

BOOST_AUTO_TEST_SUITE_END()

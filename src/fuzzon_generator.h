
#ifndef FUZZON_GENERATOR_H_
#define FUZZON_GENERATOR_H_


#undef RAPIDJSON_ASSERT
#define RAPIDJSON_ASSERT(x)

#include "fuzzon_testcase.h"

#include <boost/random.hpp>

#include <string>
#include <rapidjson/document.h>

namespace fuzzon
{

class Generator
{
public:
	Generator(std::string format_filepath);

	TestCase generateNext();

	bool IsValid(TestCase validate_me);

private:
	int ParseJson(rapidjson::Value& current, rapidjson::Document& new_document, rapidjson::Value& new_document_current);
	int StripJson(rapidjson::Value& current, std::stringstream& output);


//	rapidjson::Value JsonFindValue(rapidjson::Document& document, std::string path);
//	std::string JsonFindValue(std::string path);

	void PrintJson(std::string document_title, const rapidjson::Document& print_me);
	void PrintJson(std::string value_title, rapidjson::Value& print_me);
	void PrintJson(std::string title, rapidjson::Value::ConstMemberIterator print_me);
	void PrintJson(std::string title, rapidjson::Value::ConstMemberIterator& print_me);


	std::string format_filepath_;
};

}

#endif /* FUZZON_GENERATOR_H_ */

#ifndef FUZZON_GENERATOR_H_
#define FUZZON_GENERATOR_H_


#include "fuzzon_testcase.h"

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <boost/random.hpp>
#include <string>


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
	rapidjson::Value* GetLast(rapidjson::Value& top, const rapidjson::Pointer& find_me);

	bool JsonInsertInteger(rapidjson::Document& document, rapidjson::Value& current, std::string new_value_name, int minimum, int maximum);
	bool JsonInsertString(rapidjson::Document& document, rapidjson::Value& current, std::string new_value_name, int length);

	bool JsonInsert(rapidjson::Document& document, rapidjson::Value& current,
					std::string new_element_name, rapidjson::Value& new_element_value);

	void PrintJson(std::string document_title, const rapidjson::Document& print_me);
	void PrintJson(std::string value_title, rapidjson::Value& print_me);
	void PrintJson(std::string title, rapidjson::Value::ConstMemberIterator print_me);
	void PrintJson(std::string title, rapidjson::Value::ConstMemberIterator& print_me);

	std::string input_format_filepath_;
	rapidjson::Document input_format_;
};

}

#endif /* FUZZON_GENERATOR_H_ */

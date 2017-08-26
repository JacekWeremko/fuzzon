

#include "fuzzon_generator.h"
#include "utils/logger.h"
#include "fuzzon_random.h"


#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/pointer.h>
#include <rapidjson/prettywriter.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <regex>

namespace fuzzon
{

Generator::Generator(std::string format_filepath) :
	format_filepath_(format_filepath)
{

}

TestCase Generator::generateNext()
{
	std::ifstream t(format_filepath_);
	std::string format_filepath_content((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    rapidjson::Document input_format;
    input_format.Parse(format_filepath_content.c_str());
//    assert(input_format.IsObject());

    static const char* kTypeNames[] =
        { "Null", "False", "True", "Object", "Array", "String", "Number" };

    rapidjson::Document generated_input;
    generated_input.SetObject();

//	rapidjson::Value properties(rapidjson::kObjectType);
////	generated_input.PushBack(properties, generated_input.GetAllocator());
//	generated_input.AddMember("properties", properties, generated_input.GetAllocator());
//
//	auto properties_ref = generated_input.FindMember("properties");
//
//	rapidjson::Value new_value(8);
//	properties_ref->value.AddMember("test01", new_value, generated_input.GetAllocator());

	for (auto& m : input_format.GetObject())
	{
//		std::string name = m.name.GetString();
//		std::string type = std::string(kTypeNames[m.value.GetType()]);
//		Logger::Get()->info(std::string(name + " is " + type));

		if (m.value.IsObject())
		{
			generated_input.AddMember("properties", rapidjson::Value(rapidjson::kObjectType), generated_input.GetAllocator());
			ParseJson(m.value, generated_input, generated_input.FindMember("properties")->value);
		}
	}

	PrintJson("Input format", input_format);
	PrintJson("Generated input data", generated_input);

	std::stringstream output;
	int result = StripJson(generated_input, output);
	Logger::Get()->info("Stripped input data :" + output.str());


//	rapidjson::Value::MemberIterator it = input_format.FindMember("properties");
//	if (it->value.IsObject())
//	{
//		ParseJson(it->value, ss);
//	}

//	for (auto cuttent_it = input_format.MemberBegin(); cuttent_it != input_format.MemberEnd(); ++cuttent_it)
//	{
//		std::string name = cuttent_it->name.GetString();
//		std::string type = std::string(kTypeNames[cuttent_it->value.GetType()]);
//		Logger::Get()->info(std::string(name + " is " + type));
//		if (cuttent_it->value.IsObject())
//		{
//			ParseJson(cuttent_it->value, ss);
//		}
//	}

//    if (input_format.IsObject())
//    {
//		for (auto& m : input_format.GetObject())
//		{
//			std::string name = m.name.GetString();
//			std::string type = std::string(kTypeNames[m.value.GetType()]);
//			Logger::Get()->info(std::string(name + " is " + type));
//			if (m.value.IsObject())
//			{
////				ParseJson(m.value, ss);
//			}
//		}
//    }

//    rapidjson::Value* document = &input_format;
//	for (auto& m : document)
//	{
//		std::string name = m.name.GetString();
//		std::string type = std::string(kTypeNames[m.value.GetType()]);
//		Logger::Get()->info(std::string(name + " is " + type));
//		if (m.value.IsObject())
//		{
////				ParseJson(m.value, ss);
//		}
//	}



    return TestCase();
}

//int Generator::ParseJson(rapidjson::Value& current, rapidjson::Document& new_document)
int Generator::ParseJson(rapidjson::Value& current, rapidjson::Document& new_document, rapidjson::Value& new_document_current)
{
    if (!current.IsObject())
    {
    	Logger::Get()->critical(std::string(current.GetString()) + " is not an object!");
    	return -1;
    }

	for (auto cuttent_it = current.MemberBegin(); cuttent_it != current.MemberEnd(); ++cuttent_it)
	{
		{
		    static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
			std::string name = cuttent_it->name.GetString();
			std::string type = std::string(kTypeNames[cuttent_it->value.GetType()]);
			Logger::Get()->trace("Parse: " + std::string(name) + " is " + std::string((type)));

			static const std::vector<std::string> reserved = { "id", "type", "uniqueItems", "length" };
			if (std::find(reserved.begin(), reserved.end(), name) != reserved.end())
			{
				continue;
			}
		}

		if (!cuttent_it->value.IsObject() || !cuttent_it->value.HasMember("type"))
		{
//				throw std::string(type) + " handling is not implemented yet";
			Logger::Get()->critical(std::string(cuttent_it->value.GetString()) + " is not an object or has not 'type' member.");
			return -1;
		}

		auto element_type = std::string(cuttent_it->value.FindMember("type")->value.GetString());
		if (std::string("integer").compare(element_type) == 0)
		{
			JsonInsertInteger(new_document, new_document_current, std::string(cuttent_it->name.GetString()));
		}
		if (std::string("string").compare(element_type) == 0)
		{
			JsonInsertString(new_document, new_document_current, std::string(cuttent_it->name.GetString()));
		}
		else if (std::string("object").compare(element_type) == 0)
		{
			new_document_current.AddMember(rapidjson::Value(cuttent_it->name, new_document.GetAllocator()),
										   rapidjson::Value(rapidjson::kObjectType),
										   new_document.GetAllocator());
			PrintJson("AddMember object1 " + std::string(cuttent_it->name.GetString()), new_document);
			//auto new_document_obect = new_document_current.FindMember(cuttent_it->name);
			auto new_document_obect = new_document_current.MemberEnd() - 1;

			ParseJson(cuttent_it->value, new_document, new_document_obect->value);
		}
		else if (std::string("array").compare(element_type) == 0)
		{
			int array_length = 0;
			{
				auto length = cuttent_it->value.FindMember("length");
				PrintJson("FindMember ", length);

				auto length_ref = length->value.FindMember("$ref");
//				PrintJson("FindMember ", length_ref);

				auto array_length_value = rapidjson::Pointer(length_ref->value.GetString()).Get(new_document);
				PrintJson("array_length", *array_length_value);

				array_length = array_length_value->GetInt();
			}

			new_document_current.AddMember(rapidjson::Value(cuttent_it->name, new_document.GetAllocator()),
										   rapidjson::Value(rapidjson::kObjectType),
										   new_document.GetAllocator());
			PrintJson("AddMember object2 " + std::string(cuttent_it->name.GetString()), new_document);
			auto new_document_array = new_document_current.FindMember(cuttent_it->name);


			auto items = cuttent_it->value.FindMember("items");
//			auto properties = items->value.FindMember("properties");
			auto type_of_array_elements = items->value.FindMember("type");
			if ((std::string("object").compare(std::string(type_of_array_elements->value.GetString())) == 0) ||
				(std::string("array").compare(std::string(type_of_array_elements->value.GetString())) == 0))
			{
				for(size_t idx=0; idx<array_length; idx++)
				{
					new_document_array->value.AddMember(rapidjson::Value(items->name, new_document.GetAllocator()),
												   rapidjson::Value(rapidjson::kObjectType),
												   new_document.GetAllocator());
					PrintJson("AddMember object3 " + std::string(items->name.GetString()), new_document);
					//auto new_array_item = new_document_array->value.FindMember(items->name);
					auto new_array_item = new_document_array->value.MemberEnd() - 1;

					ParseJson(items->value, new_document, new_array_item->value);
				}
			}
			else if (std::string("integer").compare(std::string(type_of_array_elements->value.GetString())) == 0)
			{
				new_document_array->value.AddMember(rapidjson::Value(items->name, new_document.GetAllocator()),
											        rapidjson::Value(rapidjson::kArrayType),
											        new_document.GetAllocator());
				PrintJson("AddMember object4 " + std::string(items->name.GetString()), new_document);
				auto new_array_item = new_document_array->value.MemberEnd() - 1;


				for(size_t idx=0; idx<array_length; idx++)
				{
					JsonInsertInteger(new_document, new_array_item->value, std::string(cuttent_it->name.GetString()));
				}
			}
			else if (std::string("string").compare(std::string(type_of_array_elements->value.GetString())) == 0)
			{
				new_document_array->value.AddMember(rapidjson::Value(items->name, new_document.GetAllocator()),
											        rapidjson::Value(rapidjson::kArrayType),
											        new_document.GetAllocator());
				PrintJson("AddMember object4 " + std::string(items->name.GetString()), new_document);
				auto new_array_item = new_document_array->value.MemberEnd() - 1;


				for(size_t idx=0; idx<array_length; idx++)
				{
					JsonInsertString(new_document, new_array_item->value, std::string(cuttent_it->name.GetString()));
				}
			}

			else
			{
				Logger::Get()->critical(std::string(type_of_array_elements->value.GetString()) +
						" handling of array element type not implemented yet!");
			}

		}
		else
		{
//						throw std::string(type) + " handling is not implemented yet";
			Logger::Get()->critical(element_type + " handling is not implemented yet");
		}
	}
	return 0;
}

int Generator::StripJson(rapidjson::Value& current, std::stringstream& output)
{
//	if (!current.IsObject())
//	{
//		Logger::Get()->critical(std::string(current.GetString()) + " is not an object! Cannot traverse.");
//		return -1;
//	}

	if (current.IsInt())
	{
		output << current.GetInt() << " ";
	}
	else if (current.IsString())
	{
		output << current.GetString() << " ";
	}
	else if (current.IsArray())
	{
		for(auto idx=0; idx < current.Size(); idx++)
		{
			auto& elem = current[idx];
			StripJson(elem, output);
		}
	}
	else if (current.IsObject())
	{
		for (auto it = current.MemberBegin(); it != current.MemberEnd(); ++it)
		{
			StripJson(it->value, output);
		}
	}
	else
	{
		Logger::Get()->critical("Stripping not supported for given type.");
	}

	return 0;
}


//rapidjson::Value Generator::JsonFindValue(rapidjson::Document& document, std::string path)
//{
//	std::vector<std::string> path_parts;
//	boost::split(path_parts, path, boost::is_any_of("\\"));
//
//	Logger::Get()->trace("JsonFindValue: " + path);
//	rapidjson::Value current_top = document.GetObject();
//    if (!current_top.IsObject())
//    {
//    	Logger::Get()->critical(std::string(current_top.GetString()) + " is not an object! Cannot traverse.");
//    	return current_top;
//    }
//
//    for(const auto& current_element_name : path_parts)
//    {
//    	Logger::Get()->trace("current_top : " + std::string(current_top.GetString()));
//    	Logger::Get()->trace("current_element_name: " + current_element_name);
//    	if (current_top.IsArray())
//    	{
//    		auto last_array_element = current_top.End() - 1;
//    		auto last_element = last_array_element->FindMember(current_element_name.c_str());
//    		current_top = last_element->value;
//    	}
//    	else if (current_top.IsObject())
//    	{
//    		auto last_element = current_top.FindMember(current_element_name.c_str());
//    		current_top = last_element->value;
//    	}
//    	else
//    	{
//    		Logger::Get()->critical("wtf");
//    	}
//
//    }
//	return current_top;
//}

bool Generator::IsValid(TestCase validate_me)
{
	// TODO:implememnt
	return true;
}

void Generator::PrintJson(std::string document_title, const rapidjson::Document& print_me)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	print_me.Accept(writer);

	Logger::Get()->debug(document_title + " : \r\n" + std::string(buffer.GetString()));
	return;
}

void Generator::PrintJson(std::string value_title, rapidjson::Value& print_me)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	print_me.Accept(writer);

	Logger::Get()->debug(value_title + " : \r\n" + std::string(buffer.GetString()));
	return;
}

void Generator::PrintJson(std::string title, rapidjson::Value::ConstMemberIterator print_me)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	print_me->value.Accept(writer);

	Logger::Get()->debug(title + std::string(print_me->name.GetString()) + " : \r\n" + std::string(buffer.GetString()));
	return;
}

void Generator::PrintJson(std::string title, rapidjson::Value::ConstMemberIterator& print_me)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	print_me->value.Accept(writer);

	Logger::Get()->debug(title + std::string(print_me->name.GetString()) + " : \r\n" + std::string(buffer.GetString()));
	return;
}

//void Generator::PrintValue(rapidjson::Value& print_me)
//{
//    static const char* kTypeNames[] =
//        { "Null", "False", "True", "Object", "Array", "String", "Number" };
//
//    rapidjson::GenericMember
//	std::string name = print_me.name.GetString();
//	std::string type = std::string(kTypeNames[print_me.value.GetType()]);
//	Logger::Get()->info(std::string(name + " ..is.. " + type));
//	return;
//}


int Generator::JsonInsertInteger(rapidjson::Document& document, rapidjson::Value& current, std::string new_value_name)
{
	rapidjson::Value new_element_value = rapidjson::Value(Random::Get()->GenerateInt());
	return JsonInsert(document, current, new_value_name, new_element_value);
}

int Generator::JsonInsertString(rapidjson::Document& document, rapidjson::Value& current, std::string new_value_name)
{
	rapidjson::Value new_element_value = rapidjson::Value(Random::Get()->GenerateString().c_str(), document.GetAllocator());
	return JsonInsert(document, current, new_value_name, new_element_value);
}

int Generator::JsonInsert(rapidjson::Document& document, rapidjson::Value& current,
						  std::string new_element_name, rapidjson::Value& new_element_value)
{
	if (current.IsArray())
	{
		current.PushBack(new_element_value, document.GetAllocator());
		PrintJson("AddMember", document);
	}
	else if (current.IsObject())
	{
		current.AddMember(
				rapidjson::Value(new_element_name.c_str(), document.GetAllocator()),
				new_element_value, document.GetAllocator());
		PrintJson("AddMember" + new_element_name, document);
	}
	else
	{
		Logger::Get()->critical(std::string(current.GetString()) + " insertion failed.");
	}
	return 0;
}

//template<typename generate_type>
//generate_type Generator::JsonInsert(rapidjson::Document& document, rapidjson::Value& current, std::string new_value_name)
//{
//	auto random = rapidjson::Value(Random::Get()->Generate<generate_type>(), document.GetAllocator());
//
//	if (current.IsArray())
//	{
//		current.PushBack(random, document.GetAllocator());
//		PrintJson("AddMember integer", document);
//	}
//	else if (current.IsObject())
//	{
//		current.AddMember(rapidjson::Value(new_value_name.c_str(), document.GetAllocator()),
//									   random,
//									   document.GetAllocator());
//		PrintJson("AddMember integer " + new_value_name, document);
//	}
//	else
//	{
//		Logger::Get()->critical(std::string(current.GetString()) + " generation failed");
//	}
//	return 0;
//}

}





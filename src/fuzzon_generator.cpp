

#include "fuzzon_generator.h"
#include "utils/logger.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <boost/filesystem.hpp>

#include <iostream>


namespace fuzzon
{

Generator::Generator(std::string format_filepath, std::string output_directory) :
	format_filepath_(format_filepath),
	output_directory_(output_directory)
{

}

TestCase Generator::generateNext()
{
    rapidjson::Document input_format;
    input_format.Parse(format_filepath_.c_str());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    input_format.Accept(writer);

    std::cout << buffer.GetString() << std::endl;

//    static const char* kTypeNames[] =
//        { "Null", "False", "True", "Object", "Array", "String", "Number" };
//
//    const int output_count = 1;
//    for(size_t i=0; i< output_count; ++i)
//    {
//    	auto file_name = std::string("output_" + std::to_string(i) + ".input");
//    	auto file_path = boost::filesystem::path(output_directory_)/file_name;
//
//    	for (auto& m : input_format.GetObject())
//    	{
//    		std::string name = m.name.GetString();
//    		Logger::Get()->info(std::string(name + " is " + std::string(kTypeNames[m.value.GetType()])));
//    	}
//
//    }

    return TestCase();
}

}



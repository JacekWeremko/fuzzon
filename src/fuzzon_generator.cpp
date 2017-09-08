/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_generator.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <regex>
#include <vector>

#include "./fuzzon_random.h"
#include "./utils/logger.h"

namespace fuzzon {

Generator::Generator(std::string format_filepath)
    : input_format_filepath_(format_filepath) {
  std::ifstream t(input_format_filepath_);
  std::string format_filepath_content((std::istreambuf_iterator<char>(t)),
                                      std::istreambuf_iterator<char>());

  input_format_.Parse(format_filepath_content.c_str());
  //    assert(input_format_.IsObject());
}

TestCase Generator::generateNext() {
  rapidjson::Document intput_data;
  intput_data.SetObject();

  for (auto& current : input_format_.GetObject()) {
    if (current.value.IsObject()) {
      intput_data.AddMember("properties",
                            rapidjson::Value(rapidjson::kObjectType),
                            intput_data.GetAllocator());
      ParseJson(current.value, intput_data,
                intput_data.FindMember("properties")->value);
    }
  }

  PrintJson("Input format", input_format_);
  PrintJson("Generated input data", intput_data);
  std::stringstream input_stripped;
  int result = StripJson(intput_data, input_stripped);
  BOOST_ASSERT(result == 0);

  Logger::Get()->debug("Stripped input data :" + input_stripped.str());
  return TestCase(input_stripped.str());
}

int Generator::ParseJson(rapidjson::Value& current,
                         rapidjson::Document& new_document,
                         rapidjson::Value& new_document_current) {
  if (!current.IsObject()) {
    Logger::Get()->critical(std::string(current.GetString()) +
                            " is not an object!");
    return -1;
  }

  for (auto cuttent_it = current.MemberBegin();
       cuttent_it != current.MemberEnd(); ++cuttent_it) {
    {
      static const char* kTypeNames[] = {"Null",  "False",  "True",  "Object",
                                         "Array", "String", "Number"};
      std::string name = cuttent_it->name.GetString();
      std::string type = std::string(kTypeNames[cuttent_it->value.GetType()]);
      //      Logger::Get()->trace("Parse: " +
      // std::string(name) + " is " + std::string((type)));

      static const std::vector<std::string> reserved = {
          "id", "type", "uniqueItems", "length"};
      if (std::find(reserved.begin(), reserved.end(), name) != reserved.end()) {
        continue;
      }
    }

    if (!cuttent_it->value.IsObject() || !cuttent_it->value.HasMember("type")) {
      //        throw std::string(type) + " handling is
      // not implemented yet";
      Logger::Get()->critical(std::string(cuttent_it->value.GetString()) +
                              " is not an object or has not 'type' member.");
      return -1;
    }

    auto element_type =
        std::string(cuttent_it->value.FindMember("type")->value.GetString());
    if (std::string("integer").compare(element_type) == 0) {
      auto minimum = cuttent_it->value.FindMember("minimum");
      auto minimum_value = minimum == cuttent_it->value.MemberEnd()
                               ? -1
                               : minimum->value.GetInt();

      auto maximum = cuttent_it->value.FindMember("maximum");
      auto maximum_value = maximum == cuttent_it->value.MemberEnd()
                               ? -1
                               : maximum->value.GetInt();

      JsonInsertInteger(new_document, new_document_current,
                        std::string(cuttent_it->name.GetString()),
                        minimum_value, maximum_value);
    }
    if (std::string("string").compare(element_type) == 0) {
      auto length = cuttent_it->value.FindMember("length");
      auto length_value =
          length == cuttent_it->value.MemberEnd() ? -1 : length->value.GetInt();
      JsonInsertString(new_document, new_document_current,
                       std::string(cuttent_it->name.GetString()), length_value);
    } else if (std::string("object").compare(element_type) == 0) {
      new_document_current.AddMember(
          rapidjson::Value(cuttent_it->name, new_document.GetAllocator()),
          rapidjson::Value(rapidjson::kObjectType),
          new_document.GetAllocator());
      PrintJson(
          "AddMember object1 " + std::string(cuttent_it->name.GetString()),
          new_document);
      // auto new_document_obect =
      // new_document_current.FindMember(cuttent_it->name);
      auto new_document_obect = new_document_current.MemberEnd() - 1;

      ParseJson(cuttent_it->value, new_document, new_document_obect->value);
    } else if (std::string("array").compare(element_type) == 0) {
      int array_length = 0;
      {
        auto length = cuttent_it->value.FindMember("length");
        PrintJson("FindMember ", length);

        auto length_ref = length->value.FindMember("$ref");
        PrintJson("FindMember ", length_ref);

        // auto new_array_item =
        // new_document_array->value.MemberEnd() - 1;

        auto array_length_value = GetLast(
            new_document, rapidjson::Pointer(length_ref->value.GetString()));
        PrintJson("array_length", *array_length_value);

        array_length = array_length_value->GetInt();
      }

      new_document_current.AddMember(
          rapidjson::Value(cuttent_it->name, new_document.GetAllocator()),
          rapidjson::Value(rapidjson::kObjectType),
          new_document.GetAllocator());
      PrintJson("AddMember array " + std::string(cuttent_it->name.GetString()),
                new_document);
      auto new_document_array =
          new_document_current.FindMember(cuttent_it->name);

      auto items = cuttent_it->value.FindMember("items");
      // auto properties =
      // items->value.FindMember("properties");
      auto type_of_array_elements = items->value.FindMember("type");
      if ((std::string("object").compare(
               std::string(type_of_array_elements->value.GetString())) == 0) ||
          (std::string("array").compare(
               std::string(type_of_array_elements->value.GetString())) == 0)) {
        for (size_t idx = 0; idx < array_length; idx++) {
          new_document_array->value.AddMember(
              rapidjson::Value(items->name, new_document.GetAllocator()),
              rapidjson::Value(rapidjson::kObjectType),
              new_document.GetAllocator());
          PrintJson(
              "AddMember array/object " + std::string(items->name.GetString()),
              new_document);
          // auto new_array_item =
          // new_document_array->value.FindMember(items->name);
          auto new_array_item = new_document_array->value.MemberEnd() - 1;

          ParseJson(items->value, new_document, new_array_item->value);
        }
      } else if (std::string("integer").compare(std::string(
                     type_of_array_elements->value.GetString())) == 0) {
        new_document_array->value.AddMember(
            rapidjson::Value(items->name, new_document.GetAllocator()),
            rapidjson::Value(rapidjson::kArrayType),
            new_document.GetAllocator());
        PrintJson("AddMember integer " + std::string(items->name.GetString()),
                  new_document);
        auto new_array_item = new_document_array->value.MemberEnd() - 1;

        auto minimum = cuttent_it->value.FindMember("minimum");
        auto minimum_value = minimum == cuttent_it->value.MemberEnd()
                                 ? -1
                                 : minimum->value.GetInt();

        auto maximum = cuttent_it->value.FindMember("maximum");
        auto maximum_value = maximum == cuttent_it->value.MemberEnd()
                                 ? -1
                                 : maximum->value.GetInt();
        for (size_t idx = 0; idx < array_length; idx++) {
          JsonInsertInteger(new_document, new_array_item->value,
                            std::string(cuttent_it->name.GetString()),
                            minimum_value, maximum_value);
        }
      } else if (std::string("string").compare(std::string(
                     type_of_array_elements->value.GetString())) == 0) {
        new_document_array->value.AddMember(
            rapidjson::Value(items->name, new_document.GetAllocator()),
            rapidjson::Value(rapidjson::kArrayType),
            new_document.GetAllocator());
        PrintJson("AddMember string " + std::string(items->name.GetString()),
                  new_document);
        auto new_array_item = new_document_array->value.MemberEnd() - 1;

        auto length = cuttent_it->value.FindMember("length");
        auto length_value = length == cuttent_it->value.MemberEnd()
                                ? -1
                                : length->value.GetInt();
        for (size_t idx = 0; idx < array_length; idx++) {
          JsonInsertString(new_document, new_array_item->value,
                           std::string(cuttent_it->name.GetString()),
                           length_value);
        }
      } else {
        Logger::Get()->critical(
            std::string(type_of_array_elements->value.GetString()) +
            " handling of array element type not implemented yet!");
      }

    } else {
      // throw std::string(type)
      //+ " handling is not implemented yet";
      Logger::Get()->critical(element_type +
                              " handling is not implemented yet");
    }
  }
  return 0;
}

int Generator::StripJson(rapidjson::Value& current, std::stringstream& output) {
  if (current.IsInt()) {
    output << current.GetInt() << " ";
  } else if (current.IsString()) {
    output << current.GetString() << " ";
  } else if (current.IsArray()) {
    for (auto idx = 0; idx < current.Size(); idx++) {
      auto& elem = current[idx];
      StripJson(elem, output);
    }
  } else if (current.IsObject()) {
    for (auto it = current.MemberBegin(); it != current.MemberEnd(); ++it) {
      StripJson(it->value, output);
    }
  } else {
    Logger::Get()->critical("Stripping not supported for given type.");
  }
  return 0;
}

rapidjson::Value* Generator::GetLast(rapidjson::Value& top,
                                     const rapidjson::Pointer& find_me) {
  rapidjson::Value* elem = &top;
  for (auto i = 0; i < find_me.GetTokenCount(); ++i) {
    auto current_token = find_me.GetTokens() + i;
    if (elem->IsArray()) {
      elem = &elem[elem->Size() - 1];
    } else if (elem->IsObject()) {
      auto elem_member = elem->MemberEnd() - 1;
      for (auto im = 0; im < elem->MemberCount(); im++) {
        std::string elem_member_name =
            std::string(elem_member->name.GetString());
        std::string current_token_name = std::string(current_token->name);
        if (std::string(elem_member_name).compare(current_token_name) == 0) {
          elem = &elem_member->value;
          break;
        }
        elem_member = elem_member - 1;
      }
    } else {
      return nullptr;
    }
  }
  return elem;
}

bool Generator::IsValid(TestCase validate_me) {
  // TODO:implememnt
  return true;
}

bool Generator::JsonInsertInteger(rapidjson::Document& document,
                                  rapidjson::Value& current,
                                  std::string new_value_name,
                                  int minimum,
                                  int maximum) {
  rapidjson::Value new_element_value =
      rapidjson::Value(Random::Get()->GenerateInt(minimum, maximum));
  return JsonInsert(document, current, new_value_name, new_element_value);
}

bool Generator::JsonInsertString(rapidjson::Document& document,
                                 rapidjson::Value& current,
                                 std::string new_value_name,
                                 int length) {
  rapidjson::Value new_element_value = rapidjson::Value(
      Random::Get()->GenerateString(length).c_str(), document.GetAllocator());
  return JsonInsert(document, current, new_value_name, new_element_value);
}

bool Generator::JsonInsert(rapidjson::Document& document,
                           rapidjson::Value& current,
                           std::string new_element_name,
                           rapidjson::Value& new_element_value) {
  if (current.IsArray()) {
    current.PushBack(new_element_value, document.GetAllocator());
    PrintJson("AddMember", document);
  } else if (current.IsObject()) {
    current.AddMember(
        rapidjson::Value(new_element_name.c_str(), document.GetAllocator()),
        new_element_value, document.GetAllocator());
    PrintJson("AddMember", document);
  } else {
    Logger::Get()->critical(std::string(current.GetString()) +
                            " insertion failed.");
    return false;
  }
  return true;
}

void Generator::PrintJson(std::string document_title,
                          const rapidjson::Document& print_me) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  print_me.Accept(writer);

  Logger::Get()->debug(document_title + " : \r\n" +
                       std::string(buffer.GetString()));
  return;
}

void Generator::PrintJson(std::string value_title, rapidjson::Value& print_me) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  print_me.Accept(writer);

  Logger::Get()->debug(value_title + " : \r\n" +
                       std::string(buffer.GetString()));
  return;
}

void Generator::PrintJson(std::string title,
                          rapidjson::Value::ConstMemberIterator print_me) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  print_me->value.Accept(writer);

  Logger::Get()->debug(title + std::string(print_me->name.GetString()) +
                       ": \r\n" + std::string(buffer.GetString()));
  return;
}

void Generator::PrintJson(std::string title,
                          rapidjson::Value::ConstMemberIterator& print_me) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  print_me->value.Accept(writer);

  Logger::Get()->debug(title + std::string(print_me->name.GetString()) +
                       ": \r\n" + std::string(buffer.GetString()));
  return;
}

}  // namespace fuzzon

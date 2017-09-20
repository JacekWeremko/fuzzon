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
#include <utility>

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
  LOG_DEBUG("Stripped input data :" + input_stripped.str());
  std::string in = std::string(input_stripped.str() + '\0');

  return TestCase(in, TestCase::Generation);
}

std::string FindType(const rapidjson::Value& current) {
  auto name = std::string("");
  {
    auto mtype = current.FindMember("type");
    if (mtype == current.MemberEnd()) {
      name = "object";
    } else {
      name = std::string(mtype->value.GetString());
    }
  }
  return name;
}

int Generator::TryFindMin(const rapidjson::Value& current) {
  auto min = current.FindMember("minimum");
  auto min_value = min == current.MemberEnd() ? -1 : min->value.GetInt();
  return min_value;
}

int Generator::TryFindMax(const rapidjson::Value& current) {
  auto max = current.FindMember("maximum");
  auto max_value = max == current.MemberEnd() ? -1 : max->value.GetInt();
  return max_value;
}

int Generator::TryFindLength(rapidjson::Document& new_doc,
                             const rapidjson::Value& current) {
  auto array_length = 0;
  const auto length = current.FindMember("length");
  PrintJson("FindMember ", length);

  if (length->value.IsObject()) {
    const auto length_ref = length->value.FindMember("$ref");
    PrintJson("FindMember ", length_ref);

    auto length_value_ptr = rapidjson::Pointer(length_ref->value.GetString());
    auto array_length_value = GetLast(new_doc, length_value_ptr);
    PrintJson("array_length", *array_length_value);

    array_length = array_length_value->GetInt();
  } else if (length->value.IsNumber()) {
    array_length = length->value.GetInt();
  } else {
    LOG_CRITICAL("Array length handling supports only refs and integers.");
  }
  return array_length;
}

std::string Generator::TryFindContent(const rapidjson::Value& current) {
  const auto content = current.FindMember("content");

  if (content == current.MemberEnd()) {
    LOG_TRACE("Content not found. fallback")
    return "";
  }
  PrintJson("FindMember ", content);
  return std::string((content->value.GetString()));
}

int Generator::ParseJson(rapidjson::Value& current,
                         rapidjson::Document& new_doc,
                         rapidjson::Value& last_child) {
  if (!current.IsObject()) {
    LOG_CRITICAL(std::string(current.GetString()) + " is not an object!");
    return -1;
  }

  for (auto it = current.MemberBegin(); it != current.MemberEnd(); ++it) {
    {
      static const char* kTypeNames[] = {"Null",  "False",  "True",  "Object",
                                         "Array", "String", "Number"};
      auto name = it->name.GetString();
      auto type = std::string(kTypeNames[it->value.GetType()]);
      LOG_TRACE("Parse: " + std::string(name) + " is " + std::string((type)));

      static const std::vector<std::string> reserved = {
          "id", "type", "uniqueItems", "length"};
      if (std::find(reserved.begin(), reserved.end(), name) != reserved.end()) {
        continue;
      }
    }

    if (!it->value.IsObject()) {
      LOG_CRITICAL(std::string(it->value.GetString()) + " is not an object!");
      return -1;
    }

    auto it_name = std::string(it->name.GetString());
    auto it_type = FindType(it->value);
    LOG_TRACE("Parsing : " + it_name + " typeof " + it_type);

    if (std::string("integer").compare(it_type) == 0) {
      auto min = TryFindMin(it->value);
      auto max = TryFindMax(it->value);
      JsonInsertInteger(new_doc, last_child, it_name, min, max);
    }
    if (std::string("string").compare(it_type) == 0) {
      auto value = TryFindContent(it->value);
      if (value != "") {
        JsonInsertString(new_doc, last_child, it_name, value);
      } else {
        auto min = TryFindMin(it->value);
        auto max = TryFindMax(it->value);
        JsonInsertString(new_doc, last_child, it_name, min, max);
      }
    } else if (std::string("object").compare(it_type) == 0) {
      last_child.AddMember(rapidjson::Value(it->name, new_doc.GetAllocator()),
                           rapidjson::Value(rapidjson::kObjectType),
                           new_doc.GetAllocator());
      PrintJson("AddMember object " + it_name, new_doc);

      auto new_last_child = last_child.MemberEnd() - 1;
      ParseJson(it->value, new_doc, new_last_child->value);
    } else if (std::string("array").compare(it_type) == 0) {
      int array_length = TryFindLength(new_doc, it->value);
      last_child.AddMember(rapidjson::Value(it->name, new_doc.GetAllocator()),
                           rapidjson::Value(rapidjson::kObjectType),
                           new_doc.GetAllocator());
      PrintJson("AddMember array " + it_name, new_doc);
      auto new_document_array = last_child.FindMember(it->name);

      auto items = it->value.FindMember("items");
      auto array_elements_type = FindType(items->value);

      if ((std::string("object").compare(array_elements_type) == 0) ||
          (std::string("array").compare(array_elements_type) == 0)) {
        for (auto idx = 0; idx < array_length; idx++) {
          new_document_array->value.AddMember(
              rapidjson::Value(items->name, new_doc.GetAllocator()),
              rapidjson::Value(rapidjson::kObjectType), new_doc.GetAllocator());
          PrintJson(
              "AddMember array/object " + std::string(items->name.GetString()),
              new_doc);

          auto new_array_item = new_document_array->value.MemberEnd() - 1;
          ParseJson(items->value, new_doc, new_array_item->value);
        }
      } else if (std::string("integer").compare(array_elements_type) == 0) {
        new_document_array->value.AddMember(
            rapidjson::Value(items->name, new_doc.GetAllocator()),
            rapidjson::Value(rapidjson::kArrayType), new_doc.GetAllocator());
        PrintJson("AddMember integer " + std::string(items->name.GetString()),
                  new_doc);

        auto new_array_item = new_document_array->value.MemberEnd() - 1;

        auto min = TryFindMin(it->value);
        auto max = TryFindMax(it->value);
        for (auto idx = 0; idx < array_length; idx++) {
          JsonInsertInteger(new_doc, new_array_item->value, it_name, min, max);
        }
      } else if (std::string("string").compare(array_elements_type) == 0) {
        new_document_array->value.AddMember(
            rapidjson::Value(items->name, new_doc.GetAllocator()),
            rapidjson::Value(rapidjson::kArrayType), new_doc.GetAllocator());
        PrintJson("AddMember string " + std::string(items->name.GetString()),
                  new_doc);

        auto new_array_item = new_document_array->value.MemberEnd() - 1;

        auto min = TryFindMin(it->value);
        auto max = TryFindMax(it->value);

        for (size_t idx = 0; idx < array_length; idx++) {
          JsonInsertString(new_doc, new_array_item->value, it_name, min, max);
        }
      } else {
        LOG_CRITICAL(array_elements_type +
                     " handling of array element type not implemented yet!");
      }

    } else {
      LOG_TRACE(it_type + " handling is not implemented yet");
    }
  }
  return 0;
}

int Generator::StripJson(rapidjson::Value& current, std::vector<char>& output) {
  if (current.IsInt()) {
    auto push_me = current.GetInt();

    for (int i = 0; i < 4; ++i) {
      output.push_back(push_me >> (i * 8));
    }

  } else if (current.IsString()) {
    auto push_me = std::string(current.GetString());
    for (auto& ch : push_me) {
      output.push_back(ch);
    }
    output.push_back(' ');
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
    LOG_CRITICAL("Stripping not supported for given type.");
  }

  std::vector<char> v;
  std::string str(v.begin(), v.end());

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
    LOG_CRITICAL("Stripping not supported for given type.");
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
                                 int minimum_value,
                                 int maximum_value) {
  rapidjson::Value new_element_value = rapidjson::Value(
      Random::Get()->GenerateString(minimum_value, maximum_value).c_str(),
      document.GetAllocator());
  return JsonInsert(document, current, new_value_name, new_element_value);
}

bool Generator::JsonInsertString(rapidjson::Document& document,
                                 rapidjson::Value& current,
                                 std::string new_value_name,
                                 std::string content) {
  rapidjson::Value new_element_value =
      rapidjson::Value(content.c_str(), document.GetAllocator());
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
    LOG_CRITICAL(std::string(current.GetString()) + " insertion failed.");
    return false;
  }
  return true;
}

void Generator::PrintJson(std::string document_title,
                          const rapidjson::Document& print_me) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  print_me.Accept(writer);

  LOG_TRACE(document_title + " : \r\n" + std::string(buffer.GetString()));
  return;
}

void Generator::PrintJson(std::string value_title, rapidjson::Value& print_me) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  print_me.Accept(writer);

  LOG_TRACE(value_title + " : \r\n" + std::string(buffer.GetString()));
  return;
}

void Generator::PrintJson(std::string title,
                          rapidjson::Value::ConstMemberIterator print_me) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  print_me->value.Accept(writer);

  LOG_TRACE(title + std::string(print_me->name.GetString()) + ": \r\n" +
            std::string(buffer.GetString()));
  return;
}

void Generator::PrintJson(std::string title,
                          rapidjson::Value::ConstMemberIterator& print_me) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  print_me->value.Accept(writer);

  LOG_TRACE(title + std::string(print_me->name.GetString()) + ": \r\n" +
            std::string(buffer.GetString()));
  return;
}

}  // namespace fuzzon

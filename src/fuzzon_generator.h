/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_GENERATOR_H_
#define SRC_FUZZON_GENERATOR_H_

#include "./fuzzon_testcase.h"

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <boost/random.hpp>
#include <string>
#include <vector>

namespace fuzzon {

class Generator {
 public:
  explicit Generator(std::string format_filepath);

  TestCase generateNext();

  bool IsValid(TestCase validate_me);

 private:
  int ParseJson(rapidjson::Value& current,
                rapidjson::Document& new_document,
                rapidjson::Value& new_document_current);

  int StripJson(rapidjson::Value& current, std::vector<char>& output);
  int StripJson(rapidjson::Value& current, std::stringstream& output);

  rapidjson::Value* GetLast(rapidjson::Value& top,
                            const rapidjson::Pointer& find_me);

  bool JsonInsertInteger(rapidjson::Document& document,
                         rapidjson::Value& current,
                         std::string new_value_name,
                         int minimum,
                         int maximum);
  bool JsonInsertString(rapidjson::Document& document,
                        rapidjson::Value& current,
                        std::string new_value_name,
                        int minimum_value,
                        int maximum_value);
  bool JsonInsertString(rapidjson::Document& document,
                        rapidjson::Value& current,
                        std::string new_value_name,
                        std::string content);

  bool JsonInsert(rapidjson::Document& document,
                  rapidjson::Value& current,
                  std::string new_element_name,
                  rapidjson::Value& new_element_value);

  void PrintJson(std::string document_title,
                 const rapidjson::Document& print_me);
  void PrintJson(std::string value_title, rapidjson::Value& print_me);
  void PrintJson(std::string title,
                 rapidjson::Value::ConstMemberIterator print_me);
  void PrintJson(std::string title,
                 rapidjson::Value::ConstMemberIterator& print_me);

  int TryFindMin(const rapidjson::Value& current);
  int TryFindMax(const rapidjson::Value& current);
  int TryFindLength(rapidjson::Document& new_doc,
                    const rapidjson::Value& current);
  std::string TryFindContent(const rapidjson::Value& current);

  std::string input_format_filepath_;
  rapidjson::Document input_format_;
};

}  // namespace fuzzon

#endif  // SRC_FUZZON_GENERATOR_H_

/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_TESTCASE_H_
#define SRC_FUZZON_TESTCASE_H_

#include <string>
#include <memory>
#include <vector>

namespace fuzzon {

class TestCase {
 public:
  TestCase() = delete;
  explicit TestCase(uint8_t* data, size_t length);   // move semantic
  TestCase(TestCase&& move_me) noexcept;             // move semantic
  TestCase(const TestCase& copy_me);                 // copy semantic
  explicit TestCase(const std::string& serialized);  // copy semantic

  uint8_t* const data();

  size_t length_bit();
  size_t length_byte();
  size_t length_word();
  size_t length_dword();

  std::string string();

 private:
  std::vector<uint8_t> data_;
};

} /* namespace fuzzon */

#endif  // SRC_FUZZON_TESTCASE_H_

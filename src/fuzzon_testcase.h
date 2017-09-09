/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_TESTCASE_H_
#define SRC_FUZZON_TESTCASE_H_

#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <algorithm>
#include <ostream>

namespace fuzzon {

// template <typename T>
// class TestCaseGeneric {
class TestCase {
 public:
  TestCase() = delete;

  // move semantic
  explicit TestCase(uint8_t* data, size_t length) { data_.assign(data, data + length); }
  explicit TestCase(std::vector<char>&& move_me) noexcept : data_(move_me) {}
  TestCase(TestCase&& move_me) noexcept : data_(std::move(move_me.data_)) {}

  // copy semantic
  TestCase(const TestCase& copy_me) : data_(copy_me.data_) {}
  explicit TestCase(const std::string& serialized) {
    std::copy(serialized.begin(), serialized.end(), std::back_inserter(data_));
  }

  using iterator = typename std::vector<char>::iterator;
  using const_iterator = typename std::vector<char>::const_iterator;

  char& operator[](int idx) { return data_[idx]; }
  const char& operator[](int idx) const { return data_[idx]; }

  bool operator==(TestCase const& rhs) const { return data_ == rhs.data_; }
  size_t size() const { return data_.size(); }
  iterator begin() { return data_.begin(); }
  iterator end() { return data_.end(); }

  iterator erase(const_iterator position);
  iterator erase(const_iterator first, const_iterator last);

  //  const iterator begin() const { return data_.begin(); }
  //  const iterator end() const { return data_.end(); }

  size_t length_byte() const { return data_.size(); }
  size_t length_bit() const { return length_byte() * 8; }
  size_t length_word() const { return length_byte() / 2; }
  size_t length_dword() const { return length_byte() / 4; }

  std::string string() const { return std::string(data_.data(), data_.size()); }

  std::vector<char>& vec() { return data_; }

  friend std::ostream& operator<<(std::ostream& os, const TestCase& print_me) {
    os << "{";
    os << "\"data\" : \"";
    for (const auto& elem : print_me.data_) {
      os << elem;
    }
    os << "\"";
    os << "}";
    return os;
  }

 private:
  std::vector<char> data_;
};

// using TestCase = TestCaseGeneric<char>;

} /* namespace fuzzon */

#endif  // SRC_FUZZON_TESTCASE_H_

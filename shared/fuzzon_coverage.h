/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SHARED_FUZZON_COVERAGE_H_
#define SHARED_FUZZON_COVERAGE_H_

#include <stdint.h>
#include <openssl/md5.h>

#include <iostream>
#include <algorithm>
#include <array>

namespace fuzzon {

class Coverage {
 public:
  enum TrackMode { Raw, Flow };
  enum CompreseMode { Log2 };

  explicit Coverage(TrackMode mode);

  // Helpers
  void Compress(CompreseMode comprese_mode = CompreseMode::Log2);
  void ComputeHash();
  void Merge(const Coverage& merge_me);

  // Info getters
  int GetTotalPCCounter() const;
  int GetVisitedPCCounter();

  // Operators compare hash
  bool operator==(const Coverage&) const;
  bool operator!=(const Coverage&) const;

  // Compares content
  bool IsTheSame(const Coverage&) const;

  // Tracing interface
  void SetPCLimit(size_t value);
  void TracePC(uintptr_t PC);
  void TracePC(uint32_t idx, uintptr_t PC);

  // Printers
  void PrintTrace() const;

  friend std::ostream& operator<<(std::ostream& os, const Coverage& print_me) {
    os << "{" << std::endl;
    os << "\"mode\" : " << print_me.mode_ << "," << std::endl;
    os << "\"pc_total\" : " << print_me.pc_total_ << "," << std::endl;
    os << "\"pc_visited\" : " << print_me.pc_visited_ << "," << std::endl;
    os << "\"last_pc\" : " << print_me.last_pc_ << "," << std::endl;
    os << "\"pc_flow\" : {" << std::endl;
    for (auto i = 0, j = 0; i < print_me.pc_flow_.size(); i++) {
      if (print_me.pc_flow_[i] != 0) {
        if (j == 0) {
          os << "\"" << std::to_string(i) << "\" : " << print_me.pc_flow_[i];
        } else {
          os << "," << std::endl;
          os << "\"" << std::to_string(i) << "\" : " << print_me.pc_flow_[i];
        }
        j++;
      }
    }
    os << std::endl << "}," << std::endl;
    os << "\"hash\" : \"";
    for (const auto& elem : print_me.hash_) {
      os << std::hex << static_cast<int>(elem);
    }
    os << "\"" << std::endl;
    os << "}";
    return os;
  }

 private:
  TrackMode mode_;
  uint32_t pc_total_;
  uint32_t pc_visited_;
  uintptr_t last_pc_;

  //   static const int pc_flow_size_ = 64 * 1024;
  static const int pc_flow_size_ = 128;
  //  static const int pc_flow_size_ = 15;
  std::array<uint32_t, pc_flow_size_> pc_flow_;

  std::array<unsigned char, MD5_DIGEST_LENGTH> hash_;
};

} /* namespace fuzzon */

#endif  // SHARED_FUZZON_COVERAGE_H_

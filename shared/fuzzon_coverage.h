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

  explicit Coverage(TrackMode mode = Raw);
  void Reset();

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
  bool IsPcFlowTheSame(const Coverage&) const;
  bool IsDataFlowTheSame(const Coverage&) const;

  // Tracing interface
  void SetPCGuardsCount(size_t value);
  void TracePC(uintptr_t pc);
  void TracePC(uint32_t idx, uintptr_t pc);

  template <typename valueT>
  void TraceData(uintptr_t pc, valueT value) {
    //    TracePC(pc);
    //    flow_data_[value % flow_pcs_.size()]++;
    //    flow_data_[pc % flow_data_.size()] += value;
  }

  template <typename valueT>
  void TraceCmp(uintptr_t pc, valueT arg1, valueT arg2) {
    auto set_me = 0;
    if (arg1 > arg2) {
      set_me |= 1 << 0;
    } else if (arg1 < arg2) {
      set_me |= 1 << 1;
    }
    if (arg1 == arg2) {
      set_me |= 1 << 2;
    }

    flow_data_[pc % flow_data_.size()] |= set_me;
  }

  // Printers
  void PrintTrace() const;

  friend std::ostream& operator<<(std::ostream& os, const Coverage& print_me) {
    os << "{" << std::endl;
    os << "\"mode\" : " << print_me.mode_ << "," << std::endl;
    os << "\"pc_total\" : " << print_me.pc_total_ << "," << std::endl;
    os << "\"pc_visited\" : " << print_me.pc_visited_ << "," << std::endl;
    os << "\"last_pc\" : " << print_me.last_pc_ << "," << std::endl;
    os << "\"pc_flow\" : {" << std::endl;
    for (auto i = 0, j = 0; i < print_me.flow_pcs_.size(); i++) {
      if (print_me.flow_pcs_[i] != 0) {
        if (j == 0) {
          os << "\"" << std::to_string(i) << "\" : " << print_me.flow_pcs_[i];
        } else {
          os << "," << std::endl;
          os << "\"" << std::to_string(i) << "\" : " << print_me.flow_pcs_[i];
        }
        j++;
      }
    }
    os << std::endl << "}," << std::endl;
    os << "\"hash\" : \"";
    for (const auto& elem : print_me.hash_pcs_) {
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
  static const int flow_array_size_ = 128;
  //  static const int pc_flow_size_ = 15;
  std::array<uint32_t, flow_array_size_> flow_pcs_;
  std::array<uint32_t, flow_array_size_> flow_data_;

  std::array<unsigned char, MD5_DIGEST_LENGTH> hash_pcs_;
  std::array<unsigned char, MD5_DIGEST_LENGTH> hash_data_;
};

} /* namespace fuzzon */

#endif  // SHARED_FUZZON_COVERAGE_H_

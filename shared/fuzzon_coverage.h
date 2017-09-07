/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SHARED_FUZZON_COVERAGE_H_
#define SHARED_FUZZON_COVERAGE_H_

#include <stdint.h>
#include <openssl/md5.h>

#include <iostream>
#include <array>

namespace fuzzon {

class Coverage {
 public:
  enum TrackMode { Raw, Flow };
  enum CompreseMode { Log2 };

  explicit Coverage(TrackMode mode);

  void Compress(CompreseMode comprese_mode = CompreseMode::Log2);
  void ComputeHash();

  // Operators compare hash
  bool operator==(const Coverage&) const;
  bool operator!=(const Coverage&) const;

  // Compares content
  bool IsTheSame(const Coverage&) const;

  void SetPCLimit(size_t value);
  void TracePC(uintptr_t PC);
  void TracePC(uint32_t idx, uintptr_t PC);
  void PrintTrace() const;

  //  friend ostream &operator<<( ostream &output, const Distance &D ) {
  friend std::ostream& operator<<(std::ostream& output,
                                  const Coverage& print_me) {
    for (auto i = 0; i < print_me.pc_flow_.size(); i++) {
      if (print_me.pc_flow_[i] != 0) {
        output << i;
        output << ":" << std::hex << print_me.pc_flow_[i] << " ";
        output << print_me.pc_flow_[i] << " ";
        //        output << i << ":" << std::hex << pc_flow_[i] << " ";
      }
    }
    return output;
  }

 private:
  TrackMode mode_;
  size_t pc_total_;
  uintptr_t last_pc_;

  //   static const int pc_flow_size_ = 64 * 1024;
  static const int pc_flow_size_ = 128;
  //  static const int pc_flow_size_ = 15;
  std::array<uint32_t, pc_flow_size_> pc_flow_;

  std::array<unsigned char, MD5_DIGEST_LENGTH> hash_;
};

} /* namespace fuzzon */

#endif  // SHARED_FUZZON_COVERAGE_H_

/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_coverage.h"

#include <sstream>
#include <cmath>
#include <limits>

#include "./utils/logger.h"

namespace fuzzon {

Coverage::Coverage(TrackMode mode)
    : mode_(mode),
      pc_total_(std::numeric_limits<uint32_t>::max()),
      last_pc_(0) {
  pc_flow_.fill(0);
}

void Coverage::Compress(CompreseMode comprese_mode) {
  if (comprese_mode == CompreseMode::Log2) {
    for (auto& elem : pc_flow_) {
      if (elem == 0) {
        continue;
      }
      elem = std::log2(elem) + 1;
    }
  }
}

void Coverage::ComputeHash() {
  MD5(reinterpret_cast<const unsigned char*>(pc_flow_.data()),
      pc_flow_.size() * sizeof(pc_flow_[0]), hash_.data());
}

bool Coverage::operator==(const Coverage& compare_with_me) const {
  return hash_ == compare_with_me.hash_;
}

bool Coverage::operator!=(const Coverage& compare_with_me) const {
  return !(*this == compare_with_me);
}

bool Coverage::IsTheSame(const Coverage& compare_with_me) const {
  for (size_t idx = 0; idx < pc_flow_.size(); idx++) {
    if (pc_flow_[idx] != compare_with_me.pc_flow_[idx]) {
      return false;
    }
  }
  return true;
}

void Coverage::SetPCLimit(size_t value) {
  pc_total_ = value;
}

void Coverage::TracePC(uintptr_t PC) {
  uintptr_t current = 0;
  if (mode_ == TrackMode::Raw) {
    current = PC;
  } else {
    current = last_pc_ == 0 ? PC : (PC ^ last_pc_);
  }

  //  if (pc_total_ < guards_nax_lookup_size_) {
  //    auto idx = guards_lookup_.insert(current);
  //    pc_flow_[*idx.first]++;
  //  } else {
  pc_flow_[current % pc_flow_.size()]++;
  //  }

  last_pc_ = PC >> 1;
}

void Coverage::PrintTrace() const {
  std::stringstream output;
  for (size_t i = 0; i < pc_flow_.size(); i++) {
    if (pc_flow_[i] != 0) {
      output << i << ":" << std::hex << pc_flow_[i] << " ";
    }
  }
  Logger::Get()->info("pc_flow : " + output.str());
  return;
}
} /* namespace fuzzon */
